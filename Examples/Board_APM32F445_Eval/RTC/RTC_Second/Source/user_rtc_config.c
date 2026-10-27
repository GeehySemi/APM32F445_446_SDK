/*!
 * @file        user_rtc_config.c
 *
 * @brief       Peripheral configurations
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
#include "user_rtc_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup RTC_Second
  @{
  */

/** @defgroup RTC_Second_Variables Variables
  @{
  */

RTC_INIT_CONFIG_T g_rtcInitConfig =
{
    .clockOutConfig = RTC_CLKOUT_DISABLED,
    .clockSelect = RTC_CLK_SRC_LPO_1KHZ,
    .compensation = 0,
    .compensationInterval = 0,
    .updateEnable = false,
    .nonSupervisorAccessEnable = true
};

RTC_TIMEDATE_T g_rtcTimeDate =
{
    .year = 2023,
    .month = 1,
    .day = 1,
    .hour = 0,
    .minutes = 0,
    .seconds = 0
};

RTC_SECONDS_INT_CONFIG_T g_rtcSecondsConfig =
{
    .rtcSecondsCallback = NULL,
    .secondIntCfg = RTC_SECOND_INT_1HZ,
    .secondIntEnable = true,
    .secondsCallbackParams = 0
};

RTC_INTERRUPT_CONFIG_T g_rtcIntConfig =
{
    .rtcCallback = NULL,
    .overflowIntEnable = false,
    .timeInvalidIntEnable = false,
    .callbackParams = 0
};

RTC_ALARM_CONFIG_T g_rtcAlarmConfig =
{
    .alarmCallback = NULL,
    .callbackParams = 0,
    .alarmIntEnable = false,
    .alarmTime =
    {
        .year = 0,
        .month = 0,
        .day = 0,
        .hour = 0,
        .minutes = 0,
        .seconds = 0
    },
    .numberOfRepeats = 0,
    .repeatForever = false,
    .repetitionInterval = 0
};

/**@} end of group RTC_Second_Variables */
/**@} end of group RTC_Second */
/**@} end of group Examples */
