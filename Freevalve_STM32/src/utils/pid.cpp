/*
 * pid.cpp
 *
 *  Created on: Aug 8, 2020
 *      Author: whitfijs
 */

#include <pid.hpp>

constexpr Pid::ControlTermScale_t Pid::PTermLookup[Pid::CONTROL_TERM_SCALE_SIZE];
constexpr Pid::ControlTermScale_t Pid::ITermLookup[Pid::CONTROL_TERM_SCALE_SIZE];
constexpr Pid::ControlTermScale_t Pid::DTermLookup[Pid::CONTROL_TERM_SCALE_SIZE];

Pid::Pid(float kP, float kI, float kD, float dt) :
    mSetPoint(0), mKp(kP), mKi(kI), mKd(kD),
    mDt(dt), mITerm(0.0f) {
}

Pid::~Pid() {
}

Pid::PidOutput_t Pid::update(float plantOutput) {
    static float lastError = 0;
    static float lastPlantOutput = 0;
    static float lastSetPoint = 0;
    static float lastDError = 0;

    float setPoint = mSetPoint;

    float dSetPoint = setPoint - lastSetPoint;
    lastSetPoint = setPoint;

    /* estimate dPlant/dt */
    float dPlantOuput = plantOutput - lastPlantOutput;
    lastPlantOutput = plantOutput;

    /* calculate error */
    float error = setPoint - plantOutput;

    /* calculate dError */
    float dError = calculateDerivative(error, lastError, lastDError, 0.7);
    lastError = error;
    lastDError = dError;

    /* Calculate scale for P, I, and D gain based on the error */
    float pScale = getTermScale(abs(error), (ControlTermScale_t *)PTermLookup, CONTROL_TERM_SCALE_SIZE);
    float iScale = getTermScale(abs(error), (ControlTermScale_t *)ITermLookup, CONTROL_TERM_SCALE_SIZE);
    float dScale = getTermScale(abs(error), (ControlTermScale_t *)DTermLookup, CONTROL_TERM_SCALE_SIZE);

    /* P term */
    float p = mKp * pScale * error;

    /* I Term */
    mITerm += error * mDt;
    float i = mITerm * mKi * iScale;

    if (i > 20) {
        mITerm = 20;
    } else if (i < -20) {
        mITerm = -20;
    }

    /* try to avoid windup */
    if (abs(error) < 2) {
        mITerm  /= 2.0;
    }

    /* D term */
    float dEdT = dError / mDt;
    float d = dEdT * mKd * dScale;

    /* Spring torque compensation */
    float springComp = getSpringCompensation(setPoint);

    /* static friction compensation */
    float staticCompensation = getFrictionCompensation(error);

    /* Calculate Output */
    float output = p + i + d + springComp + staticCompensation;

    /* Clamp output */
    if (output > 100) {
        output = 100;
        mITerm = 0;
    }

    if (output < -100) {
        output = -100;
        mITerm = 0;
    }

    return {mSetPoint, output, p, i, d, staticCompensation, springComp};
}

void Pid::setCompensationParam(float thetaLh, float thetaLhN, float thetaLhP,
        float mLhN, float mLhP, float kN, float kP) {
    /** TODO: check validity of parameters */

    mCompParam.thetaLh = thetaLh;
    mCompParam.thetaLhN = thetaLhN;
    mCompParam.thetaLhP = thetaLhP;
    mCompParam.mLhN = mLhN;
    mCompParam.mLhP = mLhP;
    mCompParam.kN = kN;
    mCompParam.kP = kP;
}


void Pid::setStaticFrictionParam(float TcN, float TcP, float k,
        float thetaD, float thetaR) {
    /** TODO: check validity of parameters */

    mFrictionCompParam.TcN = TcN;
    mFrictionCompParam.TcP = TcP;
    mFrictionCompParam.k = k;
    mFrictionCompParam.thetaD = thetaD;
    mFrictionCompParam.thetaR = thetaR;
}

float Pid::getSpringCompensation(int setPoint) {
    float compensation = 0;

    if (setPoint > mCompParam.thetaLhP) {
        /* Above the limp-home zone, behaves like a spring */
        compensation = mCompParam.mLhP + mCompParam.kP * (setPoint - mCompParam.thetaLhP);
    } else if ((setPoint > mCompParam.thetaLh) && (setPoint <= mCompParam.thetaLhP)) {
        /* between limp-home and limp-home+ */
        compensation = mCompParam.mLhP * (setPoint - mCompParam.thetaLh) /
                (mCompParam.thetaLhP - mCompParam.thetaLh);
    } else if ((setPoint > mCompParam.thetaLhN) && (setPoint <= mCompParam.thetaLh)) {
        /* between limp-home- and limp-home */
        compensation = mCompParam.mLhN * (mCompParam.thetaLh - setPoint) /
                (mCompParam.thetaLh - mCompParam.thetaLhN);
    } else if (setPoint <= mCompParam.thetaLhN) {
        /* Below the limp-home zone, behaves like a spring */
        compensation = mCompParam.mLhN - mCompParam.kN * (mCompParam.thetaLhN - setPoint);
    }

    return compensation;

}

float Pid::getFrictionCompensation(float error) {
    float compensation = 0;
    float absError = std::abs(error);
    float errorSign = 1;

    if (error < 0) {
        errorSign = -1;
    }

    if (absError <= mFrictionCompParam.thetaD) {
        /* inside the dead zone -- no compensation */
        compensation = 0;
    } else if ((absError > mFrictionCompParam.thetaD) &&
            absError < (mFrictionCompParam.thetaD + mFrictionCompParam.thetaR)) {
        /* Compensation will ramp up to TcN or TcP */
        float Tc = mFrictionCompParam.TcP;
        compensation = Tc * mFrictionCompParam.k *
                (error - mFrictionCompParam.thetaD) / mFrictionCompParam.thetaR;
    } else {
        if (error < 0) {
            compensation = mFrictionCompParam.TcN * mFrictionCompParam.k * errorSign;
        } else {
            compensation = mFrictionCompParam.TcP * mFrictionCompParam.k * errorSign;
        }
    }

    if (compensation < -mFrictionCompParam.TcN * mFrictionCompParam.k) {
        compensation = -mFrictionCompParam.TcN * mFrictionCompParam.k;
    } else if (compensation > mFrictionCompParam.TcP * mFrictionCompParam.k) {
        compensation = mFrictionCompParam.TcP * mFrictionCompParam.k;
    }
    return compensation;
}

float Pid::getTermScale(float error, ControlTermScale_t * lookup, int size) {
    float termScale = 0;
    int below = 0;
    int above = 0;
    for(int i = 0; i < size; i++) {

        if (error > lookup[i].diff) {
            below = i;
        }
    }
    if (below == CONTROL_TERM_SCALE_SIZE) {
        termScale = lookup[size - 1].scale;
    } else {
        above = below + 1;
        float x0 = lookup[below].diff;
        float x1 = lookup[above].diff;
        float y0 = lookup[below].scale;
        float y1 = lookup[above].scale;
        termScale = interp(error, x0, y0, x1, y1);
    }

    return termScale;
}

float Pid::calculateDerivative(float error, float lastError, float lastD, float gamma) {
    return (1 - gamma) * (error - lastError) + gamma * lastD;
}

float Pid::interp(float x, float x0, float y0, float x1, float y1) {
    float diff = (x - x0) / (x1 - x0);
    return  (y0 * (1 - diff) + y1 * diff);
}

void Pid::setDesiredPosition(float position) {
    mSetPoint = position;
}
