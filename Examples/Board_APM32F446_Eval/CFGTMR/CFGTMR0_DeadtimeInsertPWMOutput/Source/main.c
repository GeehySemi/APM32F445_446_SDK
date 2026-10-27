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

/** @addtogroup CFGTMR0_DeadtimeInsertPWMOutput
  @{
  */

/** @defgroup CFGTMR0_DeadtimeInsertPWMOutput_Macros Macros
  @{
*/

/* CFGTMR0 instance */
#define CFGTMR0_INSTANCE           (0U)

/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput_Macros*/

/** @defgroup CFGTMR0_DeadtimeInsertPWMOutput_Functions Functions
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

    /* Initialize CFGTMR PWM */
    CFGTMR_InitPWM(CFGTMR0_INSTANCE, &g_cfgtmr0PWMInitConfig);

    while (1)
    {

    }
}

/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput_Functions */
/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput */
/**@} end of group Examples */
