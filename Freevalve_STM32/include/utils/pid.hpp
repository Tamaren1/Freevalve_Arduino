/*
 * pid.hpp
 *
 *  Created on: Aug 8, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_UTILS_PID_HPP_
#define INCLUDE_UTILS_PID_HPP_

#include "return.hpp"
#include "math.h"
#include "stdint.h"

class Pid {
public:
    typedef struct {
        float setPoint;
        float output;
        float p;
        float i;
        float d;
        float friction;
        float spring;
    } PidOutput_t;

    typedef struct {
        int diff;
        float scale;
    } ControlTermScale_t;

    typedef struct {
        int8_t position;
        float compensation;
    }SpringTorqueLookup_t;

    typedef struct {
        float thetaLh;   //!< limp home position
        float thetaLhN; //!< limp home position low
        float thetaLhP; //!< limp home position high

        float mLhN; //!< spring preload low offset
        float mLhP; //!< spring preload high offset

        float kN; //!< spring constant negative
        float kP; //!< spring constant positive
    } SpringCompentationParam_t;

    typedef struct {
        float TcN;  //!< Static friction in negative direction
        float TcP;  //!< Static friction in the positve direction

        float k;  //!< Gain multiplier for TcN and TcP (should be a little greater than 1)

        float thetaD;  //!< Dead zone
        float thetaR;  //!< transition zone
    } StaticFrictionParam_t;


    /* PID gain scheduling */
    static constexpr int CONTROL_TERM_SCALE_SIZE = 5;
    static constexpr ControlTermScale_t PTermLookup[CONTROL_TERM_SCALE_SIZE] = {{0, 1}, {2, 1.10}, {5, 1.25}, {10, 1.25}, {25, 1.5}};
    static constexpr ControlTermScale_t ITermLookup[CONTROL_TERM_SCALE_SIZE] = {{0, 1}, {1, 0.5}, {2, 0.10}, {10, 0.05}, {25, 0.01}};
    static constexpr ControlTermScale_t DTermLookup[CONTROL_TERM_SCALE_SIZE] = {{0, 1}, {1, 1}, {5, 1.25}, {10, 1.5}, {25, 2}};

    /**
     * Constructors
     * @param kP: P gain
     * @param kI: I gain
     * @param kD: D gain
     * @param dt: controller update period
     */
    Pid(float kP, float kI, float kD, float dt);

    /**
     * Destructor
     */
    virtual ~Pid();

    /**
     * Update PID controller
     *
     * @param plantOutput: Output from the plant -- used to calculate error from setpoint
     * and should be on the same scale
     * @return
     */
    PidOutput_t update(float plantOutput);

    /**
     * Set the controller set point
     * @param position
     */
    void setDesiredPosition(float position);

    /**
     * Set the spring compensation parameters
     * @param thetaLh
     * @param thetaLhN
     * @param thetaLhP
     * @param mLhN
     * @param mLhP
     * @param kN
     * @param kP
     */
    void setCompensationParam(float thetaLh, float thetaLhN, float thetaLhP,
            float mLhN, float mLhP, float kN, float kP);

    /**
     * Set the static friction compensation parameters
     * @param TcN
     * @param TcP
     * @param k
     * @param thetaD
     * @param thetaR
     */
    void setStaticFrictionParam(float TcN, float TcP, float k,
            float thetaD, float thetaR);

private:

    float mSetPoint;

    float mKp;
    float mKi;
    float mKd;

    float mDt;

    float mITerm;

    SpringCompentationParam_t mCompParam;
    StaticFrictionParam_t mFrictionCompParam;

    float getSpringCompensation(int setPoint);
    float getFrictionCompensation(float error);
    float getTermScale(float error, ControlTermScale_t * lookup, int size);

    float calculateDerivative(float error, float lastError, float lastDerivative, float gamma);

    float interp(float x, float x0, float y0, float x1, float y1);


};

#endif /* INCLUDE_UTILS_PID_HPP_ */
