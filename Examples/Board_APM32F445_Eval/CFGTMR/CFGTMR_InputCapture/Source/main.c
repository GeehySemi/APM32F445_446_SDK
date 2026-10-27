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

/** @addtogroup CFGTMR_InputCapture
  @{
  */

/** @defgroup CFGTMR_InputCapture_Macros Macros
  @{
*/

/* CFGTMR instance */
#define CFGTMR0_INSTANCE           (0U)
#define CFGTMR1_INSTANCE           (1U)

/**@} end of group CFGTMR_InputCapture_Macros*/

/** @defgroup CFGTMR_InputCapture_Functions Functions
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
    CFGTMR_STATE_T cfgtmr0State;
    CFGTMR_STATE_T cfgtmr1State;
    uint16_t icTimestamps = 0U;
    uint32_t icSignalFrequency = 0U;
    uint32_t cfgtmrClockFrequency = 0U;

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

    /* Initialize CFGTMR for PWM output mode */
    CFGTMR_Init(CFGTMR0_INSTANCE, &g_cfgtmr0InitConfig, &cfgtmr0State);

    /* Initialize CFGTMR for input capture mode */
    CFGTMR_Init(CFGTMR1_INSTANCE, &g_cfgtmr1InitConfig, &cfgtmr1State);

    /* Initialize CFGTMR PWM */
    CFGTMR_InitPWM(CFGTMR0_INSTANCE, &g_cfgtmr0PWMInitConfig);

    /* Initialize CFGTMR input capture */
    CFGTMR_InitInputCapture(CFGTMR1_INSTANCE, &g_cfgtmr1InputCaptureInitConfig);

    /* Get the CFGTMR counter clock frequency */
    cfgtmrClockFrequency = CFGTMR_ReadFrequency(CFGTMR1_INSTANCE);

    /* Wait the PWM to reach stability status */
    Delay(0x4FFFFU);

    /* Get input capture time stamps values */
    icTimestamps = CFGTMR_ReadInputCaptureMeasurement(CFGTMR1_INSTANCE, 0U);
    /* Calculate input capture signal frequency */
    icSignalFrequency = cfgtmrClockFrequency / icTimestamps;

    printf("\r\nPWM output frequency: %dHz, Measured frequency: %dHz\r\n", g_cfgtmr0PWMInitConfig.uFreqHZ, icSignalFrequency);

    printf("Please press KEY1 to change PWM output frequency or duty cycle!\r\n\r\n");

    while (1)
    {
        /* Change PWM output frequency, and measure the signal frequency again */
        if (BTN_GetState(BUTTON_KEY1) == 0U)
        {
            g_cfgtmr0PWMInitConfig.uFreqHZ += 1000U;
            if (g_cfgtmr0PWMInitConfig.uFreqHZ > 50000)
            {
                g_cfgtmr0PWMInitConfig.uFreqHZ = 1000U;
            }

            CFGTMR_UpdatePwmPeriod(CFGTMR0_INSTANCE, CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE, g_cfgtmr0PWMInitConfig.uFreqHZ, true);

            CFGTMR_UpdatePwmChannel(
                                  CFGTMR0_INSTANCE,
                                  g_cfgtmr0PWMIndependentChannelsConfig[0].hwChannelId,
                                  CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE,
                                  g_cfgtmr0PWMIndependentChannelsConfig[0].dutyCycle,
                                  0U,
                                  true);

            /* Wait the PWM to reach stability status */
            Delay(0x4FFFFU);

            /* Get input capture time stamps values */
            icTimestamps = CFGTMR_ReadInputCaptureMeasurement(CFGTMR1_INSTANCE, 0U);
            /* Calculate input capture signal frequency */
            icSignalFrequency = cfgtmrClockFrequency / icTimestamps;

            printf("PWM output frequency: %dHz, Measured frequency: %dHz\r\n\r\n", g_cfgtmr0PWMInitConfig.uFreqHZ, icSignalFrequency);
            while (BTN_GetState(BUTTON_KEY1) == 0U);
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

/**@} end of group CFGTMR_InputCapture_Functions */
/**@} end of group CFGTMR_InputCapture */
/**@} end of group Examples */
