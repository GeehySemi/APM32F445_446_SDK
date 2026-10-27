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


/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup COMP_DAC
  @{
  */

/** @defgroup COMP_DAC_Functions Functions
  @{
  */

/*!
 * @brief   Comparator Interrupt Service
 *          It reads the output flags and based on the result will light the
 *          required LED:
 *          - Green if Vin is greater than DAC voltage
 *          - Red if Vin is lower than DAC voltage
 *          - Both LEDs if both edges are detected
 */
void ComparatorISR(void)
{
    /* Variable used to store comparator output flags */
    COMP_OUTPUT_TRIG_T outputFlags;

    /* Read output flags */
    COMP_GetOutputFlags(0, &outputFlags);

    if (outputFlags == COMP_EVEN_INT_BY_FALLING_EDGE)
    {
        LED_On(LED_RED);
        LED_Off(LED_GREEN);
    }
    else if (outputFlags == COMP_EVEN_INT_BY_RISING_EDGE)
    {
        LED_On(LED_GREEN);
        LED_Off(LED_RED);
    }
    else
    {
        /* Switch on both LEDs */
        LED_On(LED_RED);
        LED_On(LED_GREEN);
    }

    /* Clear output flags */
    COMP_ClearOutputFlags(0);
}

/*!
 * @brief Main function
 */
int main(void)
{
    bool exit = false;

    /* Setup system clocks, enable clock feed for PMs and Comparator */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr,
                               CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr,
                               CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();

    /* Initialize Comparator
     *  -   Positive source from AIN0
     *  -   Negative source from internal DAC with 1/3 of the reference voltage
     *      5V/3 = 1.677V (For APM32F446_EVAL).
     *  -   Output interrupt enabled for both edges
     */
    COMP_Init(0, &g_compConfig);

    /* Register Comparator interrupt handler */
    INT_SYS_InstallHandler(COMP_IRQn, &ComparatorISR, (ISR_T *)0);

    /* Enable Comparator interrupt */
    INT_SYS_EnableIRQ(COMP_IRQn);

    while (!exit);
}

/**@} end of group COMP_DAC_Functions */
/**@} end of group COMP_DAC */
/**@} end of group Examples */
