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
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPTMR_PulseCounter
  @{
  */

/** @defgroup LPTMR_PulseCounter_Macros Macros
  @{
*/

/* LPTMR instance */
#define LPTMR0_INSTANCE           (0U)

/**@} end of group LPTMR_PulseCounter_Macros*/

/** @defgroup LPTMR_PulseCounter_Functions Functions
  @{
  */

void Delay(volatile uint32_t count);

/*!
 * @brief   Main function
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    bool runningState = false;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize pins */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);
    PINS_ClrPins(GPIOD, 1);

    /* Initialize LEDs */
    LED_Init();
    LED_On(LED_GREEN);

    /* Initialize Buttons */
    BTN_Init();

    /* Initialize UART */
    COM_Init();

    /* Configure the LPTMR peripheral in pulse counter mode */
    LPTMR_Init(LPTMR0_INSTANCE, &g_lptmrConfig, false);

    /* Enable LPTMR IRQ */
    INT_SYS_EnableIRQ(LPTMR_IRQn);

    /* Start LPTMR counter */
    LPTMR_StartCounter(LPTMR0_INSTANCE);

    /* Read LPTMR running state */
    runningState = LPTMR_ReadRunningState(LPTMR0_INSTANCE);

    if (runningState != true)
    {
        goto end;
    }

    printf("\r\nThis is an example of LPTMR pulse counter.\r\n");
    printf("You can press the KEY2 to change the input pin to PD5.\r\n");

    while (1)
    {
        if (BTN_GetState(BUTTON_KEY1) == 0U)
        {
            /* Configurate PD1 output pluse signal */
            PINS_SetPins(GPIOD, 1U << 1U);
            Delay(0xFFFFF);
            PINS_ClrPins(GPIOD, 1U << 1U);
            while (BTN_GetState(BUTTON_KEY1) == 0U);
        }

        if (BTN_GetState(BUTTON_KEY2) == 0U)
        {
            /* Change the input pin to PD5 */
            LPTMR_ConfigPinSetting(LPTMR0_INSTANCE, LPTMR_PINSELECT_ALT2, LPTMR_PINPOLARITY_FALLING);
            while (BTN_GetState(BUTTON_KEY2) == 0U);
        }
    }

end:
    LPTMR_DeInit(LPTMR0_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/*!
 * @brief   LPTMR IRQ handler.
 *
 * @param   None
 *
 * @retval  None
 */
void LPTMR_IRQHandler(void)
{
    uint16_t compareValue = 0U;

    if (LPTMR_ReadCompareFlag(LPTMR0_INSTANCE) != false)
    {
        /* Clear compare flag */
        LPTMR_ClearCompareFlag(LPTMR0_INSTANCE);

        /* Toggle green LED */
        LED_Toggle(LED_GREEN);

        /* Read compare value in pluse counter mode */
        LPTMR_ReadCompareValueByCount(LPTMR0_INSTANCE, &compareValue);

        /* Change the compare value in the pulse counter */
        if (compareValue == 4U)
        {
            /* Stop LPTMR counter */
            LPTMR_StopCounter(LPTMR0_INSTANCE);
            /* Change compare value to 2 in pulse counter mode, then the count value is 2 */
            LPTMR_ConfigCompareValueByCount(LPTMR0_INSTANCE, 2U);
            /* Start LPTMR counter */
            LPTMR_StartCounter(LPTMR0_INSTANCE);
        }
        else
        {
            /* Stop LPTMR counter */
            LPTMR_StopCounter(LPTMR0_INSTANCE);
            /* Change compare value to 4 in pulse counter mode, then the count value is 4 */
            LPTMR_ConfigCompareValueByCount(LPTMR0_INSTANCE, 4U);
            /* Start LPTMR counter */
            LPTMR_StartCounter(LPTMR0_INSTANCE);
        }
    }
}

/*!
 * @brief     Delay
 *
 * @param     count: delay count
 *
 * @retval    None
 */
void Delay(volatile uint32_t count)
{
    while (count--);
}

/**@} end of group LPTMR_PulseCounter_Functions */
/**@} end of group LPTMR_PulseCounter */
/**@} end of group Examples */
