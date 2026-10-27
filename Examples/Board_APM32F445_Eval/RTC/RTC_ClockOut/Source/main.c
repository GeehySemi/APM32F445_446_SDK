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
#include "apm32f445_446_pins.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup RTC_ClockOut
  @{
  */

/** @defgroup RTC_ClockOut_Marcos Marcos
  @{
  */

#define RTC_INSTANCE    (0U)
#define RTC_CLKOUT_PORT (PMD)
#define RTC_CLKOUT_PIN  (13U)

/**@} end of group RTC_ClockOut_Marcos */

/** @defgroup RTC_ClockOut_Functions Functions
  @{
  */

void RTC_Initialize(void);
void PIN_Initialize(void);
void Second_Irq(void);

/*!
 * @brief Main function
 *
 * @param None
 *
 * @retval None
 */
int main(void)
{
    CLOCK_SYS_Init(&g_clockConfig);

    LED_Init();
    PIN_Initialize();
    RTC_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief RTC instance initialize
 *
 * @param None
 *
 * @retval None
 */
void RTC_Initialize(void)
{
    RTC_DeInit(RTC_INSTANCE);

    /* RTC initialize */
    RTC_Init(RTC_INSTANCE, &g_rtcConfig);

    /* Configure RTC alarm */
    RTC_AlarmConfig(RTC_INSTANCE, &g_rtcAlarmConfig);

    /* Configure RTC fault */
    RTC_FaultIntConfig(RTC_INSTANCE, &g_rtcIntConfig);

    INT_SYS_InstallHandler(RTC_Seconds_IRQn, Second_Irq, NULL);

    g_rtcSecondsConfig.rtcSecondsCallback = (void(*)(void*))Second_Irq;
    RTC_SecondsIntConfig(RTC_INSTANCE, &g_rtcSecondsConfig);

    /* Configure RTC clock out */
    RTC_ConfigClockOut(RTC, RTC_CLKOUT_SRC_TSIC);

    RTC_AssignTimeDate(RTC_INSTANCE, &g_rtcTimeDate);

    INT_SYS_EnableIRQ(RTC_Seconds_IRQn);

    /* Start RTC */
    RTC_StartCounter(RTC_INSTANCE);
}

/*!
 * @brief RTC clock out pin initialize
 *
 * @param None
 *
 * @retval None
 */
void PIN_Initialize(void)
{
    PINS_SetMuxModeSel(RTC_CLKOUT_PORT, RTC_CLKOUT_PIN, PM_MUX_ALT7);
    PINS_SetPinIntSel(RTC_CLKOUT_PORT, RTC_CLKOUT_PIN, PM_DMA_INT_DISABLED);
}

/*!
 * @brief RTC seconds interrupt server
 *
 * @param None
 *
 * @retval None
 */
void Second_Irq(void)
{
    LED_Toggle(LED_GREEN);
}

/**@} end of group RTC_ClockOut_Functions */
/**@} end of group RTC_ClockOut */
/**@} end of group Examples */
