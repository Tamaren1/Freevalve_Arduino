/*
 * L298N.cpp
 *
 *  Created on: Aug 19, 2020
 *      Author: whitfijs
 */

#include <L298N.hpp>
#include <math.h>

L298N::L298N() : mTimerPeriod(TIMER_PERIOD_20KHZ) {

}

L298N::~L298N() {
}

/**
 * @brief  This function provides a delay (in microseconds)
 * @param  microseconds: delay in microseconds
 */
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds) {
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

int L298N::init() {
    if (initPwm() != ReturnType::OK) {
        return ReturnType::ERROR;
    }

    if (initGpio() != ReturnType::OK) {
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}

int L298N::initGpio() {

    /* Enable GPIOA Channel Clock  */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* TIM1 GPIO Configuration    ------> TIM2_CH4 */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = PWM_PIN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;

    /* set up direction pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = FORWARD_PIN | BACKWARD_PIN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    setDirection(ZERO);

    return ReturnType::OK;
}

int L298N::initPwm() {
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

    __HAL_RCC_TIM1_CLK_ENABLE();


    mPwmTim.Instance = TIM1;
    mPwmTim.Init.Prescaler = 0;
    mPwmTim.Init.CounterMode = TIM_COUNTERMODE_UP;
    mPwmTim.Init.Period = mTimerPeriod;
    mPwmTim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    mPwmTim.Init.RepetitionCounter = 0;
    mPwmTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&mPwmTim) != HAL_OK) {
        return ReturnType::ERROR;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&mPwmTim, &sMasterConfig) != HAL_OK) {
      return ReturnType::ERROR;
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    /* PWM channel direction */
    if (HAL_TIM_PWM_ConfigChannel(&mPwmTim, &sConfigOC, PWM_CHANNEL) != HAL_OK) {
      return ReturnType::ERROR;
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&mPwmTim, &sBreakDeadTimeConfig) != HAL_OK) {
      return ReturnType::ERROR;
    }

    return ReturnType::OK;
}

int L298N::enablePwm() {
    if (HAL_TIM_PWM_Start(&mPwmTim, PWM_CHANNEL) != HAL_OK) {
      /* PWM Generation Error */
      return ReturnType::ERROR;
    }

    return ReturnType::OK;
}

int L298N::disablePwm() {
    if (HAL_TIM_PWM_Stop(&mPwmTim, PWM_CHANNEL) != HAL_OK) {
        /* PWM Generation Error */
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}

int L298N::setDutyCycle(float percent) {
    TIM_OC_InitTypeDef sConfigOC;

    if (percent == 0) {
        /* Turn off both ways */
        setDirection(ZERO);

        /* we don't need to mess with anything else here */
        return ReturnType::OK;
    } else if (percent < 0) {
        /* go back */
        setDirection(BACKWARD);
        percent *= -1;
    } else {
        /* go forward */
        setDirection(FORWARD);
    }

    /* set the duty cycle */
    uint16_t pulse = roundf((mTimerPeriod * (uint32_t)percent) / 100.0f);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&mPwmTim, &sConfigOC, PWM_CHANNEL) != HAL_OK) {
      return ReturnType::ERROR;
    }

    /* make sure its on */
    enablePwm();

    return ReturnType::OK;
}

int L298N::setDirection(int direction) {
    static int previousDir = FORWARD;

    if (previousDir != direction) {
        /* Disable both and wait a moment for the controller to settle */
        disablePwm();
        DWT_Delay_us(DIRECTION_CHANGE_DELAY_US);
    }

    switch (direction) {
    case FORWARD: {
        HAL_GPIO_WritePin(GPIOA, FORWARD_PIN, GPIO_PinState::GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, BACKWARD_PIN, GPIO_PinState::GPIO_PIN_RESET);
        break;
    }
    case BACKWARD: {
        HAL_GPIO_WritePin(GPIOA, BACKWARD_PIN, GPIO_PinState::GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, FORWARD_PIN, GPIO_PinState::GPIO_PIN_RESET);
        break;
    }
    default:
        /* do nothing -- both should be off here */
        break;
    }

    if (previousDir != direction && direction != ZERO) {
        enablePwm();
    }

    /* keep track of the current direction */
    previousDir = direction;
    return ReturnType::OK;
}
