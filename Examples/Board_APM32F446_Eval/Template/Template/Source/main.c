/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "stdio.h"
#include "main.h"
#include "board.h"
#include "system_apm32f445_446.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup Template
  @{
  */
/** @defgroup Template_Variables Variables
  @{
  */

static __IO uint32_t g_timingDelay;

/**@} end of group Template_Variables */

/** @defgroup Template_Functions Functions
  @{
  */

/*!
 * @brief   Start SysTick
 *
 * @param   None
 *
 * @retval  None
 */
void SysTick_Init(void)
{
    /* SystemFrequency / 1000 = 1ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */
        while (1);
    }
}

/*!
 * @brief   Precise delay
 *
 * @param   delayTime Delay time in milliseconds
 *
 * @retval  None
 */
void SysTick_Delay(__IO uint32_t delayTime)
{
    g_timingDelay = delayTime;
    while (g_timingDelay != 0);
}

/*!
 * @brief   Called by the SysTick_Handler() every millisecond
 *
 * @param   None
 *
 * @retval  None
 */
void TimingDelay_Decrement(void)
{
    if (g_timingDelay != 0)
    {
        g_timingDelay--;
    }
}

/*!
 * @brief   Main function
 */
int main(void)
{
    const uint8_t maxLEDs = 3;
    uint8_t currentLED = 0U;

    SystemCoreClockConfigure(); /* Configure system core clock */
    SystemCoreClockUpdate();    /* Update system core clock info */

    SysTick_Init();
    LED_Init();
    COM_Init();
    BTN_Init();

    while(1)
    {
        printf("LED = %d \r\n", currentLED);

        LED_On(currentLED);
        SysTick_Delay(500);

        LED_Off(currentLED);
        SysTick_Delay(500);

        currentLED++;
        if (currentLED >= maxLEDs)
        {
            currentLED = 0U;
        }
    }
}

/**@} end of group Template_Functions */
/**@} end of group Template */
/**@} end of group Examples */
