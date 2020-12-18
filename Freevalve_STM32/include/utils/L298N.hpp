/*
 * L298N.hpp
 *
 *  Created on: Aug 19, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_UTILS_L298N_HPP_
#define INCLUDE_UTILS_L298N_HPP_

#include <motor.hpp>
#include "stm32f1xx_hal.h"

class L298N: public Motor {
public:
    /* Timer periods pre-calculated */
    static constexpr uint16_t TIMER_PERIOD_40KHZ = 1299;
    static constexpr uint16_t TIMER_PERIOD_20KHZ = 3599;
    static constexpr uint16_t TIMER_PERIOD_10KHz = 7199;
    static constexpr uint16_t TIMER_PERIOD_5KHZ  = 14399;
    static constexpr uint16_t TIMER_PERIOD_2_5KHZ  = 28799;

    /* direction change delay */
    static constexpr uint16_t DIRECTION_CHANGE_DELAY_US = 500;

    /* pwm channel */
    static constexpr uint32_t PWM_CHANNEL = TIM_CHANNEL_4;
    static constexpr uint32_t PWM_PIN = GPIO_PIN_11;

    /* motor direction pins */
    static constexpr uint32_t FORWARD_PIN = GPIO_PIN_8;
    static constexpr uint32_t BACKWARD_PIN = GPIO_PIN_7;

    /**
     * Constructor
     */
    L298N();

    /**
     * Destructor
     */
    virtual ~L298N();

    /**
     * Set the motor direction
     * @param direction: -1 for backwards (close),
     * 1 for forwards (open), 0 for motor off
     *
     * @return
     */
    virtual int setDirection(int direction) override;

    /**
     * Initialize the motor driver
     * @return
     */
    virtual int init() override;

    /**
     * Set the duty cycle
     * @param percent -100 to 100
     *
     * @return
     */
    virtual int setDutyCycle(float percent) override;

    /**
     * Initialize motor GPIO's
     * @return
     */
    int initGpio();

    /**
     * Initialize PWM timers and output channels
     * @return
     */
    int initPwm();

    /**
     * Enable PWM output
     *
     * @return
     */
    int enablePwm();

    /**
     * Disable PWM output
     *
     * @return
     */
    int disablePwm();

protected:
    TIM_HandleTypeDef mPwmTim; //!< PWM timer handle
    uint16_t mTimerPeriod;  //!< timer period

};

#endif /* INCLUDE_UTILS_L298N_HPP_ */
