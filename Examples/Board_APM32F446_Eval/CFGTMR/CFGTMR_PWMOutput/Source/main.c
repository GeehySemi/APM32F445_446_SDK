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

/** @addtogroup CFGTMR_PWMOutput
  @{
  */

/** @defgroup CFGTMR_PWMOutput_Macros Macros
  @{
*/

/* CFGTMR0 instance */
#define CFGTMR0_INSTANCE           (0U)

/**@} end of group CFGTMR_PWMOutput_Macros*/

/** @defgroup CFGTMR_PWMOutput_Functions Functions
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
    uint16_t dutyCycle = g_cfgtmr0PWMIndependentChannelsConfig[0].dutyCycle;
    uint32_t frequencyHZ = g_cfgtmr0PWMInitConfig.uFreqHZ;

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

    printf("\r\nCFGTMR current PWM output frequency %dKHz, duty cycle %0.2f%%.\r\n",
        frequencyHZ / 1000, ((float)dutyCycle / CFGTMR_MAX_DUTY_CYCLE) * 100);

    printf("You can press KEY1 to change the PWM frequency, and press KEY2 to change the PWM duty cycle!\r\n\r\n");

    while (1)
    {
        /* Change PWM output frequency */
        if (BTN_GetState(BUTTON_KEY1) == 0U)
        {
            frequencyHZ += 10000U;
            if (frequencyHZ > 100000)
            {
                frequencyHZ = 10000U;
            }

            CFGTMR_UpdatePwmPeriod(CFGTMR0_INSTANCE, CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE, frequencyHZ, true);

            CFGTMR_UpdatePwmChannel(
                                  CFGTMR0_INSTANCE,
                                  g_cfgtmr0PWMIndependentChannelsConfig[0].hwChannelId,
                                  CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE,
                                  dutyCycle,
                                  0U,
                                  true);

            printf("CFGTMR PWM output frequency %dKHz.\r\n", frequencyHZ / 1000);
            while (BTN_GetState(BUTTON_KEY1) == 0U);
        }

        /* Change PWM output duty cycle */
        if (BTN_GetState(BUTTON_KEY2) == 0U)
        {
             dutyCycle += 0x1000U;
            if (dutyCycle > CFGTMR_MAX_DUTY_CYCLE)
            {
                dutyCycle = 0x2000U;
            }

            CFGTMR_UpdatePwmChannel(
                                  CFGTMR0_INSTANCE,
                                  g_cfgtmr0PWMIndependentChannelsConfig[0].hwChannelId,
                                  CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE,
                                  dutyCycle,
                                  0U,
                                  true);

            printf("CFGTMR PWM output duty cycle %0.2f%%.\r\n", ((float)dutyCycle / CFGTMR_MAX_DUTY_CYCLE) * 100);
            while (BTN_GetState(BUTTON_KEY2) == 0U);
        }
    }
}

/**@} end of group CFGTMR_PWMOutput_Functions */
/**@} end of group CFGTMR_PWMOutput */
/**@} end of group Examples */
