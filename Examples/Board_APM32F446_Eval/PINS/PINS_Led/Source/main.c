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

/** @addtogroup PINS_Led
  @{
  */

/** @defgroup PINS_Led_Macros Macros
  @{
  */

#define LED_GPIO            GPIOD
#define BLUE_LED_PIN        (0U)
#define RED_LED_PIN         (15U)
#define GREEN_LED_PIN       (16U)

#define WHITE_LED_PIN       (~0x00018001) /* NOTE:For testing reference only! */

/**@} end of group PINS_Led_Macros*/


/** @defgroup PINS_Led_Functions Functions
  @{
  */

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
    /* uart init */
    COM_Init();
    printf("\r\nport pin interrupt/DMA status : %d\r\n",PINS_ReadPinIntSel(LED_RED_PORT_BASE,LED_RED_PIN));
    /* Turn on all LEDs */
    PINS_WritePins(LED_GPIO,WHITE_LED_PIN);
    printf("Green led pins output status: %d\r\n",(PINS_ReadPinsOutput(LED_GREEN_GPIO_BASE)&0x8000)>>LED_GREEN_PIN);
    printf("Blue led pins output status: %d\r\n",(PINS_ReadPinsOutput(LED_BLUE_GPIO_BASE)&0x10000)>>LED_BLUE_PIN);
    printf("Red led pins output status: %d\r\n\r\n",(PINS_ReadPinsOutput(LED_RED_GPIO_BASE)&0x1)>>LED_RED_PIN);
    while (1)
    {
        if(BTN_GetState(BUTTON_KEY1) == 0)
        {
            /* Turn on the red light */
            PINS_ClrPins(LED_RED_GPIO_BASE, 1U << LED_RED_PIN);
            PINS_SetPins(LED_GREEN_GPIO_BASE, 1U << LED_GREEN_PIN);
            PINS_SetPins(LED_BLUE_GPIO_BASE, 1U << LED_BLUE_PIN);
        }
        if(BTN_GetState(BUTTON_KEY2) == 0)
        {
            /* Turn off all LEDs */
            PINS_WritePin(LED_GPIO, BLUE_LED_PIN, 1);
            PINS_WritePin(LED_GPIO, GREEN_LED_PIN, 1);
            PINS_WritePin(LED_GPIO, RED_LED_PIN, 1);
        }
    }
}


/**@} end of group PINS_Led_Functions */
/**@} end of group PINS_Led */
/**@} end of group Examples */
