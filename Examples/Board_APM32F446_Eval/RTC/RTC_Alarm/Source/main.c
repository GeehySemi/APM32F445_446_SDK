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

/** @addtogroup RTC_Alarm
  @{
  */

/** @defgroup RTC_Alarm_Marcos Marcos
  @{
  */

#define RTC_INSTANCE    (0U)

/**@} end of group RTC_Alarm_Marcos */

/** @defgroup RTC_Alarm_Functions Functions
  @{
  */

void RTC_Initialize(void);
void Alarm_Irq(void);
void Second_Irq(void);
void KEY1_Irq(void);
void KEY2_Irq(void);
void SetAlarm(uint8_t seconds);

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
    BTN_Init();

    BTN_InstallKey1Handler(KEY1_Irq);
    BTN_InstallKey2Handler(KEY2_Irq);

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
    /* RTC initialize */
    RTC_DeInit(RTC_INSTANCE);
    RTC_Init(RTC_INSTANCE, &g_rtcConfig);

    /* RTC alarm initialize */
    g_rtcAlarmConfig.alarmCallback = (void(*)(void*))Alarm_Irq;

    /* RTC fault interrupt initialize */
    RTC_FaultIntConfig(RTC_INSTANCE, &g_rtcIntConfig);

    /* RTC second interrupt initialize */
    g_rtcSecondsConfig.rtcSecondsCallback = (void(*)(void*))Second_Irq;
    RTC_SecondsIntConfig(RTC_INSTANCE, &g_rtcSecondsConfig);

    /* RTC configure time date */
    RTC_AssignTimeDate(RTC_INSTANCE, &g_rtcTimeDate);

    /* Enable rtc interrupt server */
    INT_SYS_InstallHandler(RTC_Seconds_IRQn, Second_Irq, NULL);
    INT_SYS_EnableIRQ(RTC_Seconds_IRQn);

    /* Start RTC counter */
    RTC_StartCounter(RTC_INSTANCE);
}

/*!
 * @brief RTC alarm interrupt server
 *
 * @param None
 *
 * @retval None
 */
void Alarm_Irq(void)
{
    RTC_TIMEDATE_T currentTime;

    RTC_ReadCurrentTimeDate(RTC_INSTANCE, &currentTime);
    printf("RTC alarm has finish in %d-%d-%d %d:%d:%d \r\n",
            currentTime.year, currentTime.month, currentTime.day,
            currentTime.hour, currentTime.minutes, currentTime.seconds);

    /* Clear alarm flag */
    RTC_AlarmConfig(RTC_INSTANCE, &g_rtcAlarmConfig);

    LED_Off(LED_GREEN);
}

/*!
 * @brief RTC Second interrupt server
 *
 * @param None
 *
 * @retval None
 */
void Second_Irq(void)
{
    RTC_TIMEDATE_T currentTime;
    /* Gets current time */
    RTC_ReadCurrentTimeDate(RTC_INSTANCE, &currentTime);
    printf("%d-%d-%d %d:%d:%d\r\n",
            currentTime.year, currentTime.month, currentTime.day,
            currentTime.hour, currentTime.minutes, currentTime.seconds);
}

/*!
 * @brief KEY1 interrupt server
 *
 * @param None
 *
 * @retval None
 */
void KEY1_Irq(void)
{
    SetAlarm(5);
}

/*!
 * @brief KEY2 interrupt server
 *
 * @param None
 *
 * @retval None
 */
void KEY2_Irq(void)
{
    SetAlarm(15);
}

/*!
 * @brief Set an alarm
 *
 * @param None
 *
 * @retval None
 */
void SetAlarm(uint8_t seconds)
{
    RTC_ReadCurrentTimeDate(RTC_INSTANCE, &g_rtcTimeDate);

    /* Set the alarm with paramer seconds */
    g_rtcTimeDate.minutes += (seconds / 60);
    g_rtcTimeDate.seconds += (seconds % 60);

    /* Start alarm */
    g_rtcAlarmConfig.alarmTime = g_rtcTimeDate;
    RTC_AlarmConfig(RTC_INSTANCE, &g_rtcAlarmConfig);

    /* Get the the for the next alarm */
    RTC_ReadNextAlarmTime(RTC_INSTANCE, &g_rtcTimeDate);
    printf("RTC alarm will coming after %d-%d-%d %d:%d:%d\r\n",
            g_rtcTimeDate.year, g_rtcTimeDate.month, g_rtcTimeDate.day,
            g_rtcTimeDate.hour, g_rtcTimeDate.minutes, g_rtcTimeDate.seconds);

    /* Alarm indicator light on */
    LED_On(LED_GREEN);
}

/**@} end of group RTC_Alarm_Functions */
/**@} end of group RTC_Alarm */
/**@} end of group Examples */
