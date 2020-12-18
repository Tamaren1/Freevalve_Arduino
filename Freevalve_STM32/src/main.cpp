
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2019 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Task.hpp"
#include "Tasks.hpp"

void SystemClock_Config(void);


int main(void) {
    /* Init the low level hardware **/
    HAL_MspInit();

    /* Configure the system clock **/
    SystemClock_Config();

    /* Update system clock **/
    SystemCoreClockUpdate();

    /* Set Interrupt Group Priority **/
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
    HAL_InitTick(0);

    if (Tasks::initTasks() != 0) {
        /** Failed to create and initialize tasks **/
        for (;;) {}
    }

    vTaskStartScheduler();

    while (1) {

    }
}

/**
 * @brief Application Idle hook.
 * @return None
 */
extern "C" void vApplicationIdleHook( void ) {
    __WFI();
}

/**
 * @brief FreeRTOS application stack overflow callback
 * @note Depending on the severity of the overflow, the input
 * parameters could themselves be corrupted, in which case the
 * pxCurrentTCB variable can be inspected directly
 * @param xTask: Task handle of offending task
 * @param pcTaskName: Task name of offending task
 * @return None
 */
extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName  ) {
    configASSERT(false);
}

/**
 * @brief Called if pvPortMalloc() returns NULL for any reason
 * @return None
 */
extern "C" void vApplicationMallocFailedHook ( void ) {
    configASSERT(false);
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    RCC_ClkInitTypeDef clkinitstruct;
    RCC_OscInitTypeDef oscinitstruct;

    /* Enable HSE Oscillator and activate PLL with HSE as source 72MHz */
    oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    oscinitstruct.HSEState        = RCC_HSE_ON;
    oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
    oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
    oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK) {
      /* Initialization Error */
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK) {
      /* Initialization Error */
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    }

    /**Configure the Systick interrupt time 
     */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
     */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void vConfigureTimerForRunTimeStats () {
}

uint32_t vGetRuntimeCounterValue() {
    return DWT->CYCCNT;
}

extern "C" void vAssertCalled( const char * const message ) {
    /* Check to see if we're in an interrupt */
    if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)) {
        /* We're not in an interrupt */
        taskENTER_CRITICAL();
        {
            /* trigger assert failed */
        }
        taskEXIT_CRITICAL();
    } else {
        /* We're in an interrupt */
        UBaseType_t savedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
        {
            /* trigger assert failed */
        }
        taskEXIT_CRITICAL_FROM_ISR(savedInterruptStatus);
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
extern "C" void assert_failed(char * file, uint32_t line) {
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
        // Debugger is connected
        printf("Assert failure in %s Line:%d\n", file, line);
        __BKPT();
    }

    for(;;);
}
#endif
