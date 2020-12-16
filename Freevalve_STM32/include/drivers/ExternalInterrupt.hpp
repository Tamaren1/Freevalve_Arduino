/*
 * ExternalInterrupt.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_DRIVERS_EXTERNALINTERRUPT_HPP_
#define INCLUDE_DRIVERS_EXTERNALINTERRUPT_HPP_

#include "stdint.h"
#include "return.hpp"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"

class ExternalInterrupt {
public:
    static constexpr int NUM_EXTI_LINES = 16;

    typedef void (*callback)(void *); //!< External interrupt

    ExternalInterrupt();
    virtual ~ExternalInterrupt();

    static int attachInterrupt(uint8_t line, uint8_t prio, callback cb, void * param);

    static ExternalInterrupt * getInstance() {
        return (ExternalInterrupt *) mInstance;
    }

    static int setInstance(ExternalInterrupt * instance) {
        if (instance == nullptr) {
            return ReturnType::ERROR;
        }

        mInstance = instance;
        return ReturnType::OK;
    }

    typedef struct {
        callback cb;
        void * param;
    } callback_t;

    static volatile callback_t mCallbacks[NUM_EXTI_LINES];
    static volatile ExternalInterrupt * mInstance;

    static int getIrq(uint8_t line, IRQn_Type &irqn);
};

#endif /* INCLUDE_DRIVERS_EXTERNALINTERRUPT_HPP_ */
