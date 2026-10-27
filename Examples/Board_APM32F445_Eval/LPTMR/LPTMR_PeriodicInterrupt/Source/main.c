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
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPTMR_PeriodicInterrupt
  @{
  */

/** @defgroup LPTMR_PeriodicInterrupt_Macros Macros
  @{
*/

/* LPTMR instance */
#define LPTMR0_INSTANCE           (0U)

/**@} end of group LPTMR_PeriodicInterrupt_Macros*/

/** @defgroup LPTMR_PeriodicInterrupt_Functions Functions
  @{
  */

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

    /* Initialize LEDs */
    LED_Init();
    LED_On(LED_GREEN);

    /* Initialize Buttons */
    BTN_Init();

    /* Initialize UART */
    COM_Init();

    /* Configure the LPTMR peripheral in timer mode, timing time 1 sec */
    LPTMR_Init(LPTMR0_INSTANCE, &g_lptmrConfig, false);

    /* Enable IRQ for LPTMR */
    INT_SYS_EnableIRQ(LPTMR_IRQn);

    /* Start LPTMR counter */
    LPTMR_StartCounter(LPTMR0_INSTANCE);

    /* Read LPTMR running state */
    runningState = LPTMR_ReadRunningState(LPTMR0_INSTANCE);

    if (runningState != true)
    {
        goto end;
    }

    while (1)
    {
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
    uint32_t compareValueUs = 0U;

    if (LPTMR_ReadCompareFlag(LPTMR0_INSTANCE) != false)
    {
        /* Clear compare flag */
        LPTMR_ClearCompareFlag(LPTMR0_INSTANCE);

        /* Toggle green LED */
        LED_Toggle(LED_GREEN);

        /* Read the compare value in the timer mode */
        LPTMR_ReadCompareValueByUs(LPTMR0_INSTANCE, &compareValueUs);

        /* Change the compare value in the timer mode */
        if (compareValueUs == 1000000UL)
        {
            /* Stop LPTMR counter */
            LPTMR_StopCounter(LPTMR0_INSTANCE);
            /* Change compare value to 500000 in timer mode, then the timing time is 0.5s */
            LPTMR_ConfigCompareValueByUs(LPTMR0_INSTANCE, 500000UL);
            /* Start LPTMR counter */
            LPTMR_StartCounter(LPTMR0_INSTANCE);

            printf("LPTMR timing 500ms.\r\n");
        }
        else
        {
            /* Stop LPTMR counter */
            LPTMR_StopCounter(LPTMR0_INSTANCE);
            /* Change compare value to 1000000 in timer mode, then the timing time is 1s */
            LPTMR_ConfigCompareValueByUs(LPTMR0_INSTANCE, 1000000UL);
            /* Start LPTMR counter */
            LPTMR_StartCounter(LPTMR0_INSTANCE);

            printf("LPTMR timing 1000ms.\r\n");
        }
    }
}

/**@} end of group LPTMR_PeriodicInterrupt_Functions */
/**@} end of group LPTMR_PeriodicInterrupt */
/**@} end of group Examples */
