/*
 * DWTTimebase.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_DRIVERS_DWTTIMEBASE_HPP_
#define INCLUDE_DRIVERS_DWTTIMEBASE_HPP_

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

class DWT_Timebase {
public:
    static constexpr uint32_t USEC_PER_SEC = 1000000;

    DWT_Timebase();
    virtual ~DWT_Timebase();

    static void initCycleCount() {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    };

    static uint32_t getCycleCount() {
        return DWT->CYCCNT;
    };

    static uint32_t cyclesToUsec(uint32_t cycles) {
        uint32_t clk = HAL_RCC_GetHCLKFreq();

        uint32_t usec = (cycles * USEC_PER_SEC);
        usec /= clk;
        return usec;
    };
};

#endif /* INCLUDE_DRIVERS_DWTTIMEBASE_HPP_ */
