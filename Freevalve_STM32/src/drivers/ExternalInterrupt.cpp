/*
 * ExternalInterrupt.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include <ExternalInterrupt.hpp>


volatile ExternalInterrupt * ExternalInterrupt::mInstance = nullptr;
volatile ExternalInterrupt::callback_t ExternalInterrupt::mCallbacks[ExternalInterrupt::NUM_EXTI_LINES];

ExternalInterrupt _exti;

ExternalInterrupt::ExternalInterrupt() {
    /* Set instance */
    setInstance(this);

    /* ensure that the callbacks and params are null */
    for (int i =0; i < NUM_EXTI_LINES; i++) {
        mCallbacks[i].cb = nullptr;
        mCallbacks[i].param = nullptr;
    }
}

ExternalInterrupt::~ExternalInterrupt() {
}

int ExternalInterrupt::attachInterrupt(uint8_t line, uint8_t prio, callback cb, void * param) {
    /* Make sure the priority is valid */
    if (IS_NVIC_PREEMPTION_PRIORITY(prio)) {
        return ReturnType::ERROR;
    }

    /* check if the exti number is valid */
    if (line >= NUM_EXTI_LINES) {
        return ReturnType::ERROR;
    }

    /* if the callback is already in use -- return an error */
    if (mCallbacks[line].cb != nullptr) {
        return ReturnType::ERROR;
    }

    /* assign callback and parameter */
    mCallbacks[line].cb = cb;
    mCallbacks[line].param = param;

    IRQn_Type irqn;
    if (getIrq(line, irqn) != ReturnType::OK) {
        return ReturnType::ERROR;
    }

    /* everything should be ready to go now */
    HAL_NVIC_SetPriority(irqn, prio, 0);
    HAL_NVIC_EnableIRQ(irqn);

    return ReturnType::OK;
}

int ExternalInterrupt::getIrq(uint8_t line, IRQn_Type &irqn) {
    IRQn_Type irq = (IRQn_Type) 0;

    /* no pretty way to do this */
    switch(line) {
    case 0:
        irq = EXTI0_IRQn;
        break;
    case 1:
        irq = EXTI1_IRQn;
        break;
    case 2:
        irq = EXTI2_IRQn;
        break;
    case 3:
        irq = EXTI3_IRQn;
        break;
    case 4:
        irq = EXTI4_IRQn;
        break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        irq = EXTI9_5_IRQn;
        break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        irq = EXTI15_10_IRQn;
        break;
    default:
        return ReturnType::ERROR;
        break;
    }

    irqn = irq;

    return ReturnType::OK;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    /* get the external interrupt instance */
    ExternalInterrupt * exti = ExternalInterrupt::getInstance();

    for (int i = 0; i < ExternalInterrupt::NUM_EXTI_LINES; i++) {
        uint16_t pin = 1 << i;
        if ( (pin == GPIO_Pin) && exti->mCallbacks[i].cb != nullptr) {
            /* run the callback */
            exti->mCallbacks[i].cb(exti->mCallbacks[i].param);
            return;
        }
    }
}

/****** Interrupt Handlers ******/
extern "C" void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

extern "C" void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

extern "C" void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

extern "C" void EXTI3_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

extern "C" void EXTI4_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

extern "C" void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

extern "C" void EXTI15_10_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}
