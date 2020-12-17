/*
 * TimeBase.h
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_DRIVERS_TIMEBASE_HPP_
#define INCLUDE_DRIVERS_TIMEBASE_HPP_

#include "stdint.h"
#include "stm32f1xx_hal.h"

class TimeBase {
public:
    static constexpr uint32_t TIM_FREQ = 1000000;

    TimeBase();
    virtual ~TimeBase();

    static int init();

    static uint32_t getMicroSeconds();
    static void incMicroSeconds();

    static TIM_HandleTypeDef mTimHandle;
    static volatile uint32_t mMicroSeconds;
};

#endif /* INCLUDE_DRIVERS_TIMEBASE_HPP_ */
