/*
 * FreeValveControl.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include <FreeValveControl.hpp>
#include <TimeBase.hpp>
#include "Tasks.hpp"
#include "interrupt_priorities.h"
#include "string.h"

FreeValveControl::FreeValveControl(const char * name, UBaseType_t prio,
        configSTACK_DEPTH_TYPE stackSize) : Task(Tasks::FVC_ID, name, prio, stackSize) {

}

FreeValveControl::~FreeValveControl() {
}

int FreeValveControl::setup() {
    TimeBase::init();

    if (calcValveMaps(mIntakeMap, mExhaustMap, TRIGGERS_PER_ROTATION) != ReturnType::OK) {
        return ReturnType::ERROR;
    }

    /* setup the GPIO */
    if (initGpio() != ReturnType::OK) {
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}

int FreeValveControl::loop() {
    /* any last setup here */

    /* task loop */
    for (;;) {
        /* wait for the sync from the trigger wheel interrupt*/
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    /* we shouldn't get here */
    return ReturnType::ERROR;
}

int FreeValveControl::initGpio() {
    /* initialize gpio's */
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = HALL_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = EXHAUST_GPIO_PIN | INTAKE_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* assign the hall sensor interrupt */
    ExternalInterrupt::getInstance()->attachInterrupt(HALL_EXTI_LINE, INTERRUPT_PRIORITY_HALL_SENSOR_EXTI, onHallDetected, this);

    return ReturnType::OK;
}

int FreeValveControl::calcValveMaps(uint8_t * intake, uint8_t * exhaust, uint16_t len) {
    /* sanity check */
    if ((intake == nullptr) | (exhaust == nullptr) | (len == 0)) {
        return ReturnType::ERROR;
    }

    /* zero everything out */
    memset(intake, 0x00, len);
    memset(exhaust, 0x00, len);

    for (int i = 0; i < len; i++) {
        int iDeg = i * DEG_PER_MAGNET;
        int eDeg = iDeg + 360;

        /* intake */
        if (iDeg >= INTAKE_OPEN_DEG && iDeg < INTAKE_CLOSED_DEG) {
            intake[i] = 1;
        }

        /* exhaust */
        if (eDeg >= EXHAUST_OPEN_DEG && eDeg < EXHAUST_CLOSED_DEG) {
            exhaust[i] = 1;
        }
    }

    return ReturnType::OK;
}

void FreeValveControl::onHallDetected(void * param) {
    /* Get the task */
    FreeValveControl * task = (FreeValveControl *) param;

    TriggerParams_t trigger;
    memcpy(&trigger, (void *) &task->mTriggerParams, sizeof(TriggerParams_t));

    /* Increment the counter */
    trigger.triggerCount++;

    /* Calculate the time gap */
    uint32_t triggerTime = TimeBase::getMicroSeconds();
    trigger.timeGap = triggerTime - trigger.lastTriggerTime;

    if (trigger.timeGap > (trigger.lastTimeGap + (trigger.lastTimeGap >> 1))) {
        /* check what cycle we're on */
        if (trigger.cycle) {
            trigger.triggerCount = 0;
        } else {
            trigger.triggerCount = TRIGGERS_PER_ROTATION;
        }

        trigger.cycle = !trigger.cycle;
    }

    if (trigger.cycle) { // Intake Cycle
        /* we're focused on the intake valve */
        HAL_GPIO_WritePin(GPIOB, INTAKE_GPIO_PIN, (GPIO_PinState) task->mIntakeMap[trigger.triggerCount]);
        /* keep the exhaust closed */
        HAL_GPIO_WritePin(GPIOB, EXHAUST_GPIO_PIN, GPIO_PinState::GPIO_PIN_RESET);
    } else { // Exhaust Cycle
        /* we're focused on the exhaust */
        HAL_GPIO_WritePin(GPIOB, EXHAUST_GPIO_PIN, (GPIO_PinState) task->mExhaustMap[trigger.triggerCount]);
        /* keep the intake closed */
        HAL_GPIO_WritePin(GPIOB, INTAKE_GPIO_PIN, GPIO_PinState::GPIO_PIN_RESET);
    }


    /* Check the trigger count */
    if (trigger.triggerCount >= TRIGGERS_PER_ROTATION - 1) {
        trigger.triggerCount = 0;
        //trigger.cycle = !trigger.cycle; //used for debugging with a constant square wave signal
    }

    /* store the time gap */
    trigger.lastTimeGap = trigger.timeGap;
    trigger.lastTriggerTime = triggerTime;

    /* write back to the task trigger params */
    memcpy((void *) &task->mTriggerParams, &trigger,  sizeof(TriggerParams_t));
}
