/*!
 * @file        main.c
 *
 * @brief       Main program
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

/* Includes */
#include "user_config.h"
#include "apm32f445_446_pins.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup PINS_Read
  @{
  */

/** @defgroup PINS_Read_Functions Functions
  @{
  */

/*!
 * @brief   Main function
 */
int main(void)
{
    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);
    /* Enable Clock to Port D */
    CLOCK_SYS_ConfigModuleClock(PMD_CLK, NULL);
    /* uart init */
    COM_Init();

    /* Set pin mode */
    PINS_SetMuxModeSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_MUX_AS_GPIO);
    /* Set pin interrupt */
    PINS_SetPinIntSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_DMA_INT_DISABLED);
    /* Set pin as input */
    PINS_SetPinDir(LED_BLUE_GPIO_BASE, LED_BLUE_PIN, 0U);
    /* set pin input to enabled state */
    PINS_SetPmInputDisable(LED_BLUE_GPIO_BASE, 0U<<LED_BLUE_PIN);
    /* set pin Pull-up resistor */
    PINS_SetPullSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN,PM_INTERNAL_PULL_UP_ENABLED);
    printf("Dirs configuration of port: 0x%08X\r\n",PINS_ReadPinsDir(LED_BLUE_GPIO_BASE));
    printf("Blue led pins logic level: %d\r\n",(PINS_ReadPinsInput(LED_BLUE_GPIO_BASE)&0x10000)>>LED_BLUE_PIN);
    printf("Port input disable status: 0x%08X\r\n\r\n",PINS_ReadPmInputDisable(LED_BLUE_GPIO_BASE));

    /* Set pin as output */
    PINS_SetPinDir(LED_BLUE_GPIO_BASE, LED_BLUE_PIN, 1U);
    /* set pin input to disabled state */
    PINS_SetPmInputDisable(LED_BLUE_GPIO_BASE, 1U<<LED_BLUE_PIN);
    printf("Dirs configuration of port: 0x%08X\r\n",PINS_ReadPinsDir(LED_BLUE_GPIO_BASE));
    printf("Blue led pins logic level: %d\r\n",(PINS_ReadPinsOutput(LED_BLUE_GPIO_BASE)&0x10000)>>LED_BLUE_PIN);
    printf("Port input disable status: 0x%08X\r\n\r\n",PINS_ReadPmInputDisable(LED_BLUE_GPIO_BASE));
    while (1)
    {

    }
}

/**@} end of group PINS_Read_Functions */
/**@} end of group PINS_Read */
/**@} end of group Examples */
