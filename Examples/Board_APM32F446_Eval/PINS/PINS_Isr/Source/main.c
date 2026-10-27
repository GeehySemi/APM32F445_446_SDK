/*!
 * @file        main.c
 *
 * @brief       Main program
 *
 * @version     V1.0.0
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2024 Geehy Semiconductor
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

/* Includes */
#include "user_config.h"
#include "apm32f445_446_pins.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup PINS_Isr
  @{
  */

/** @defgroup PINS_Isr Functions
  @{
  */
/*!
 * @brief   Interrupt callback function for external interrupts
 */
void KEY_IRQHandler(void)
{
    /* Get interrupt flag */
    if((PINS_ReadPmIntFlg(PMC) & (1<<KEY1_PIN)) != 0)
    {
        LED_On(LED_RED);
        /* Clear interrupt flag */
        PINS_ClrPinIntFlgCmd(PMC, KEY1_PIN);
    }
    else if((PINS_ReadPmIntFlg(PMC) & (1<<KEY2_PIN)) != 0)
    {
        LED_Off(LED_RED);
        /* Clear interrupt flag */
        PINS_ClrPinIntFlgCmd(PMC, KEY2_PIN);
    }
    else
    {
        /* Clear interrupt flags for all pins in the port */
        PINS_ClrPmIntFlgCmd(PMC);
    }
}


/*!
 * @brief   Pins module initialization LED light
 */
void PINS_Led_Init()
{
    /* Enable Clock to Port D */
    CLOCK_SYS_ConfigModuleClock(PMD_CLK, NULL);
    /* Set pin mode */
    PINS_SetMuxModeSel(LED_RED_PORT_BASE, LED_RED_PIN, PM_MUX_AS_GPIO);
    PINS_SetMuxModeSel(LED_GREEN_PORT_BASE, LED_GREEN_PIN, PM_MUX_AS_GPIO);
    PINS_SetMuxModeSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_MUX_AS_GPIO);
    /* Set pin interrupt */
    PINS_SetPinIntSel(LED_RED_PORT_BASE, LED_RED_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(LED_GREEN_PORT_BASE, LED_GREEN_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_DMA_INT_DISABLED);
    /* GPIO Initialization */
    PINS_SetPins(LED_RED_GPIO_BASE, 1U << LED_RED_PIN);
    PINS_SetPins(LED_GREEN_GPIO_BASE, 1U << LED_GREEN_PIN);
    PINS_SetPins(LED_BLUE_GPIO_BASE, 1U << LED_BLUE_PIN);
    /* Set pin as output */
    PINS_SetPinDir(LED_RED_GPIO_BASE, LED_RED_PIN, 1U);
    PINS_SetPinDir(LED_GREEN_GPIO_BASE, LED_GREEN_PIN, 1U);
    PINS_SetPinDir(LED_BLUE_GPIO_BASE, LED_BLUE_PIN, 1U);

}

/*!
 * @brief   Pins module initialization button
 */
void PINS_Btn_Init()
{
    /* Enable PMC clock*/
    CLOCK_SYS_ConfigModuleClock(PMC_CLK, NULL);
    /* Set pin mode */
    PINS_SetMuxModeSel(KEY1_PORT_BASE, KEY1_PIN, PM_MUX_AS_GPIO);
    PINS_SetMuxModeSel(KEY2_PORT_BASE, KEY2_PIN, PM_MUX_AS_GPIO);
    /* Set pin interrupt */
    PINS_SetPinIntSel(KEY1_PORT_BASE, KEY1_PIN, PM_INT_RISING_EDGE);
    PINS_SetPinIntSel(KEY2_PORT_BASE, KEY2_PIN, PM_INT_RISING_EDGE);
    /* Set input direction*/
    PINS_SetPinDir(KEY1_GPIO_BASE, KEY1_PIN, 0);
    PINS_SetPinDir(KEY2_GPIO_BASE, KEY2_PIN, 0);

    PINS_ClrPinIntFlgCmd(KEY1_PORT_BASE, KEY1_PIN);
    PINS_ClrPinIntFlgCmd(KEY2_PORT_BASE, KEY2_PIN);
}

/*!
 * @brief   Main function
 */
int main(void)
{
    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);
    /* led init */
    PINS_Led_Init();
    /* button init */
    PINS_Btn_Init();
    /* set PMC interrupt callback function */
    INT_SYS_InstallHandler(PMC_IRQn,&KEY_IRQHandler,(ISR_T*) 0);
    /* Enable PMC interrupt */
    INT_SYS_EnableIRQ(PMC_IRQn);

    while(1)
    {

    }
}


/**@} end of group PINS_Isr Functions */
/**@} end of group PINS_Isr */
/**@} end of group Examples */
