/*!
 * @file        user_rtc_config.h
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

#ifndef USER_RTC_CONFIG_H
#define USER_RTC_CONFIG_H

/* Includes */
#include "apm32f445_446_rtc.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup RTC_Second
  @{
  */
/**@} end of group RTC_Second */
/**@} end of group Examples */

extern RTC_INIT_CONFIG_T g_rtcInitConfig;
extern RTC_TIMEDATE_T g_rtcTimeDate;
extern RTC_SECONDS_INT_CONFIG_T g_rtcSecondsConfig;
extern RTC_INTERRUPT_CONFIG_T g_rtcIntConfig;
extern RTC_ALARM_CONFIG_T g_rtcAlarmConfig;

#endif /* USER_RTC_CONFIG_H */
