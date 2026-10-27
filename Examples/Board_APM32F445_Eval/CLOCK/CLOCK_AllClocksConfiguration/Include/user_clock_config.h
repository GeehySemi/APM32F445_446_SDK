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

/** @addtogroup CLOCK_AllClocksConfiguration
  @{
*/

/** @defgroup CLOCK_AllClocksConfiguration_Macros Macros
  @{
*/
/*!
 * @brief SCG asynchronous clock type.
 */
/* Count of user configuration structures */
#define CLOCK_CONFIG_CNT                           1U /*Count of user configuration */

/* Count of user Callbacks structures */
#define CLOCK_CALLBACK_CNT                         0U /*Count of user Callbacks */


/* Count of peripheral clock user configuration 0*/
#define PERIPHERAL_CLOCKS_CNT                         32U /* Count of peripheral clock user configurations. */

/**@} end of group CLOCK_AllClocksConfiguration_Macros*/

/* User configuration structure 0*/
extern CLOCK_MANAGER_USER_CONFIG_T g_clockConfig;
/* User peripheral configuration structure 0*/
extern PERIPHERAL_CLOCK_CONFIG_T g_peripheralClockConfig[PERIPHERAL_CLOCKS_CNT];
/* Array of User callbacks */
/* The tool do not support generate Callbacks configuration. It's always empty. */
extern CLOCK_MANAGER_CALLBACK_USER_CONFIG_T *g_clockCallbacksArr[];
/* Array of pointers to User configuration structures */
extern CLOCK_MANAGER_USER_CONFIG_T const *g_clockConfigsArr[CLOCK_CONFIG_CNT];

/** @defgroup CLOCK_AllClocksConfiguration_Functions Functions
  @{
*/
extern void UserConfigMainClockInit(void);
/**@} end of group CLOCK_AllClocksConfiguration_Functions*/
/**@} end of group CLOCK_AllClocksConfiguration*/
/**@} end of group Examples*/

#endif /* USER_CLOCK_CONFIG_H */