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

/** @addtogroup RTC_Second
  @{
  */

/** @defgroup RTC_Second_Marcos Marcos
  @{
  */

#define RTC_INSTANCE    (0U)

/**@} end of group RTC_Second_Marcos */

/** @defgroup RTC_Second_Functions Functions
  @{
  */

void RTC_Initialize(void);

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

    COM_Init();
    LED_Init();
    RTC_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief RTC interrupt server
 *
 * @param None
 *
 * @retval None
 */
void RTC_Second_Irq(void)
{
    RTC_TIMEDATE_T currentTime;

    /* Get current time */
    RTC_ReadCurrentTimeDate(RTC_INSTANCE, &currentTime);

    printf("%d-%d-%d %d:%d:%d\r\n", currentTime.year, currentTime.month, currentTime.day,
                            currentTime.hour, currentTime.minutes, currentTime.seconds);
    LED_Toggle(LED_GREEN);
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
    RTC_Init(RTC_INSTANCE, &g_rtcInitConfig);
    /* Configure alarm */
    RTC_AlarmConfig(RTC_INSTANCE, &g_rtcAlarmConfig);
    /* Configure fault interrupt */
    RTC_FaultIntConfig(RTC_INSTANCE, &g_rtcIntConfig);

    INT_SYS_InstallHandler(RTC_Seconds_IRQn, RTC_Second_Irq, NULL);

    g_rtcSecondsConfig.rtcSecondsCallback = (void(*)(void*))RTC_Second_Irq;
    /* Configure second interrupt */
    RTC_SecondsIntConfig(RTC_INSTANCE, &g_rtcSecondsConfig);

    RTC_AssignTimeDate(RTC_INSTANCE, &g_rtcTimeDate);

    /* Start RTC */
    RTC_StartCounter(RTC_INSTANCE);
}

/**@} end of group RTC_Second_Functions */
/**@} end of group RTC_Second */
/**@} end of group Examples */
