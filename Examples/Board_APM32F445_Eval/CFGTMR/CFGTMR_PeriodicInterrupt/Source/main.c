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

/** @addtogroup CFGTMR_PeriodicInterrupt
  @{
  */

/** @defgroup CFGTMR_PeriodicInterrupt_Macros Macros
  @{
*/

/* CFGTMR0 instance */
#define CFGTMR0_INSTANCE           (0U)

/**@} end of group CFGTMR_PeriodicInterrupt_Macros*/

/** @defgroup CFGTMR_PeriodicInterrupt_Functions Functions
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
    CFGTMR_STATE_T cfgtmr0State;

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

    /* Initialize CFGTMR */
    CFGTMR_Init(CFGTMR0_INSTANCE, &g_cfgtmr0InitConfig, &cfgtmr0State);

    /* Initialize counter */
    CFGTMR_InitCounter(CFGTMR0_INSTANCE, &g_cfgtmr0TimerParamConfig);

    /* Start Counter */
    CFGTMR_StartCounter(CFGTMR0_INSTANCE);

    while (1)
    {

    }
}

/*!
 * @brief   CFGTMR0 Counter overflow and Reload IRQ handler.
 *
 * @param   None
 *
 * @retval  None
 */
void CFGTMR0_Ovf_Reload_IRQHandler(void)
{
    static uint32_t cnt = 0;

    if (CFGTMR_ReadStatusFlags(CFGTMR0_INSTANCE) & CFGTMR_TIME_OVERFLOW_FLAG)
    {
        if (++cnt == 1000U)
        {
            printf("CFGTMR timer 1000ms!\r\n");
            LED_Toggle(LED_GREEN);
            cnt = 0;
        }
    }

    CFGTMR_ClearStatusFlags(CFGTMR0_INSTANCE, (uint32_t)CFGTMR_TIME_OVERFLOW_FLAG);
}

/**@} end of group CFGTMR_PeriodicInterrupt_Functions */
/**@} end of group CFGTMR_PeriodicInterrupt */
/**@} end of group Examples */
