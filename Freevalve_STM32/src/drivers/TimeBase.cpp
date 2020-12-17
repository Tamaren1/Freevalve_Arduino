/*
 * TimeBase.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include <TimeBase.hpp>
#include "return.hpp"
#include "interrupt_priorities.h"

volatile uint32_t TimeBase::mMicroSeconds;
TIM_HandleTypeDef TimeBase::mTimHandle;

TimeBase::TimeBase() {
    // TODO Auto-generated constructor stub

}

TimeBase::~TimeBase() {
    // TODO Auto-generated destructor stub
}

int TimeBase::init() {
    /*Configure the TIM2 IRQ priority */
  HAL_NVIC_SetPriority(TIM3_IRQn, INTERRUPT_PRIORITY_RTC_WKUP ,0U);

  /* Enable the TIM2 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

    /* enable the clock */
    __HAL_RCC_TIM3_CLK_ENABLE();


    /* Set TIMx instance 1MHz */
    mTimHandle.Instance = TIM3;
    mTimHandle.Init.Period            = 10-1;
    mTimHandle.Init.Prescaler         = 72-1;
    mTimHandle.Init.ClockDivision     = 0;
    mTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    mTimHandle.Init.RepetitionCounter = 0;
    mTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&mTimHandle) != HAL_OK) {
      /* Initialization Error */
        return ReturnType::ERROR;
    }

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    if (HAL_TIM_Base_Start_IT(&mTimHandle) != HAL_OK) {
      /* Starting Error */
      return ReturnType::ERROR;
    }

    mMicroSeconds = 0;
    return ReturnType::OK;
}

uint32_t TimeBase::getMicroSeconds() {
    return mMicroSeconds;
}

void TimeBase::incMicroSeconds() {
    mMicroSeconds += (mTimHandle.Init.Period+1);
}

extern "C" void TIM3_IRQHandler(void) {
  HAL_TIM_IRQHandler(&TimeBase::mTimHandle);

  TimeBase::incMicroSeconds();
}

