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

/** @addtogroup CFGTMR1_OCMatchSet
  @{
  */

/** @defgroup CFGTMR1_OCMatchSet_Macros Macros
  @{
*/

/* CFGTMR instance */
#define CFGTMR1_INSTANCE           (1U)

/**@} end of group CFGTMR1_OCMatchSet_Macros*/

/** @defgroup CFGTMR1_OCMatchSet_Functions Functions
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
    CFGTMR_STATE_T cfgtmr1State;
    STATUS_T statusCode = STATUS_SUCCESS;

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
    CFGTMR_Init(CFGTMR1_INSTANCE, &g_cfgtmr1InitConfig, &cfgtmr1State);

    /* Initialize CFGTMR output compare mode */
    statusCode = CFGTMR_InitOutputCompare(CFGTMR1_INSTANCE, &g_cfgtmr1OCInitConfig);
    if (statusCode != STATUS_SUCCESS)
    {
        printf("\r\nInitialize CFGTMR output compare fail, statusCode = %04x.\r\n", statusCode);
    }
    else
    {
        printf("\r\nInitialize CFGTMR output compare successful.\r\n");
    }

    while (1)
    {

    }
}

/**@} end of group CFGTMR1_OCMatchSet_Functions */
/**@} end of group CFGTMR1_OCMatchSet */
/**@} end of group Examples */
