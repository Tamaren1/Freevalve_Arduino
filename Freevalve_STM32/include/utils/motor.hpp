/*
 * motor.hpp
 *
 *  Created on: Aug 7, 2020
 *      Author: jwhitfield
 */

#ifndef INCLUDE_DRIVERS_MOTOR_HPP_
#define INCLUDE_DRIVERS_MOTOR_HPP_

#include "return.hpp"
#include "stdint.h"

class Motor {
public:
    static constexpr int ZERO = 0;
    static constexpr int FORWARD = 1;
    static constexpr int BACKWARD = -1;

    Motor();
    virtual ~Motor();

    /**
     * Initialize the motor driver
     * @return
     */
    virtual int init() {return ReturnType::OK;};

    /**
     * Set the motor direction
     * @param direction: -1 for backwards (close),
     * 1 for forwards (open), 0 for motor off
     *
     * @return
     */
    virtual int setDirection(int direction) {return ReturnType::OK;};

    /**
     * Set the duty cycle
     * @param percent -100 to 100
     *
     * @return
     */
    virtual int setDutyCycle(float percent) {return ReturnType::OK;};

};

#endif /* INCLUDE_DRIVERS_MOTOR_HPP_ */
