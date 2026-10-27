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

/** @addtogroup CFGTMR2_QDPhaseEncoding
  @{
  */

/** @defgroup CFGTMR2_QDPhaseEncoding_Macros Macros
  @{
*/

/* CFGTMR instance */
#define CFGTMR0_INSTANCE           (0U)
#define CFGTMR2_INSTANCE           (2U)

/**@} end of group CFGTMR2_QDPhaseEncoding_Macros*/

/** @defgroup CFGTMR2_QDPhaseEncoding_Functions Functions
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
    CFGTMR_STATE_T cfgtmr2State;

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

    /* Initialize CFGTMR0 as combined PWM mode */
    CFGTMR_Init(CFGTMR0_INSTANCE, &g_cfgtmr0InitConfig, &cfgtmr0State);

    /* Initialize CFGTMR2 as quadrature decoder mode */
    CFGTMR_Init(CFGTMR2_INSTANCE, &g_cfgtmr2InitConfig, &cfgtmr2State);

    /* Initialize CFGTMR0 PWM */
    CFGTMR_InitPWM(CFGTMR0_INSTANCE, &g_cfgtmr0PWMInitConfig);

    /* Initialize CFGTMR2 quadrature decoder */
    CFGTMR_ConfigQuadDecodeStart(CFGTMR2_INSTANCE, &g_cfgtmr2QuadDecodeInitConfig);

    while (1)
    {

    }
}

/*!
 * @brief   CFGTMR2 Counter overflow and Reload IRQ handler.
 *
 * @param   None
 *
 * @retval  None
 */
void CFGTMR2_Ovf_Reload_IRQHandler(void)
{
    if (CFGTMR_ReadStatusFlags(CFGTMR2_INSTANCE) & CFGTMR_TIME_OVERFLOW_FLAG)
    {
        PINS_TogglePins(GPIOD, 1U << 1U);
    }

    CFGTMR_ClearStatusFlags(CFGTMR2_INSTANCE, (uint32_t)CFGTMR_TIME_OVERFLOW_FLAG);
}

/**@} end of group CFGTMR2_QDPhaseEncoding_Functions */
/**@} end of group CFGTMR2_QDPhaseEncoding */
/**@} end of group Examples */
