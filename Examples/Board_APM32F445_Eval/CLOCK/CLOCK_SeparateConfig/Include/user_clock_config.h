/*!
 * @file        user_clock_config.h
 *
 * @brief       The header of user_clock_config.c
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

#ifndef USER_CLOCK_CONFIG_H
#define USER_CLOCK_CONFIG_H

#include "apm32f445_446_clock.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CLOCK_SeparateConfig
  @{
*/

extern CLOCK_SRC_CONFIG_T SoscClockSource;
extern CLOCK_SRC_CONFIG_T HsiClockSource;
extern SYS_CLK_CONFIG_T systemClockCfg;
extern MODULE_CLK_CONFIG_T PMDClockCfg;
extern MODULE_CLK_CONFIG_T PMCClockCfg;
extern MODULE_CLK_CONFIG_T lpuart1ClockCfg;


/** @defgroup CLOCK_SeparateConfig_Functions Functions
  @{
*/
extern void UserConfigMainClockInit(void);
/**@} end of group CLOCK_SeparateConfig_Functions*/
/**@} end of group CLOCK_SeparateConfig*/
/**@} end of group Examples*/

#endif /* USER_CLOCK_CONFIG_H */