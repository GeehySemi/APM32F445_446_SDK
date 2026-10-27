/*!
 * @file        apm32f445_446_int.c
 *
 * @brief       Main Interrupt Service Routines
 *
 * @version     V1.0.0
 *
 * @date        2026-01-31
 *
 * @attention
 *
 *  Copyright (C) 2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "apm32f445_446_int.h"
#include "main.h"
#include "user_wdt_config.h"
#include "stdio.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup WDT_Interrupt
  @{
  */

/** @defgroup WDT_Interrupt_INT_Functions INT_Functions
  @{
  */

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /** Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /** Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /** Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /** Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
    if (WDT_ReadCounter(WDT_INSTANCE) > wdtConfig.windowValue)
    {
        /* Reset Watchdog counter */
        WDT_Refresh(WDT_INSTANCE);
        printf("Feed WDT \r\n");
    }

}

/*!
 * @brief   This function handles WDT_EWDT IRQ Handler
 *
 * @details Triggered on WDOG timeout, reset delayed by 128 BUS_CLK cycles(2.6us, 48MHz).
 *
 * @param   None
 *
 * @retval  None
 *
 */
void WDT_EWDT_IRQHandler(void)
{
    if(WDT->CSTS.reg & 0x4000) //Watchdog Interrupt Flag
    {
        while(1)
        {
            GPIOD->PLLOT.reg = (1U << 16U);
        }
    }
}

/*!
 * @brief   This function handles RMU IRQ Handler
 *
 * @details Triggered after WDOG interrupt delay, reset delayed by 514 LPO cycles(~4ms, 128kHz).
 *
 * @param   None
 *
 * @retval  None
 *
 */
void RMU_IRQHandler(void)
{
    if(WDT->CSTS.reg & 0x4000) //Watchdog Interrupt Flag
    {
        while(1)
        {
            GPIOD->PLLOT.reg = (1U << 16U);
        }
    }
}

/**@} end of group WDT_Interrupt_INT_Functions */
/**@} end of group WDT_Interrupt */
/**@} end of group Examples */
