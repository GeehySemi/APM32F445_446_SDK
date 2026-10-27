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

/** @addtogroup CFGTMR0_DeadtimeInsertPWMOutput
  @{
*/

/** @defgroup CFGTMR0_DeadtimeInsertPWMOutput_Macros Macros
  @{
*/
/*!
 * @brief SCG asynchronous clock type.
 */

#define PERIPHERAL_CLOCKS_CNT                      32U

#define CLOCK_CONFIG_CNT                           1U

#define CLOCK_CALLBACK_CNT                         0U

/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput_Macros*/
/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput*/
/**@} end of group Examples*/

/*! @brief User clock configuration*/
extern CLOCK_MANAGER_USER_CONFIG_T g_clockConfig;

/*! @brief User peripheral clock configuration */
extern PERIPHERAL_CLOCK_CONFIG_T g_peripheralClockConfig[PERIPHERAL_CLOCKS_CNT];

/*! @brief Array of pointers to user clock configuration */
extern CLOCK_MANAGER_USER_CONFIG_T const *g_clockConfigsArr[CLOCK_CONFIG_CNT];

/*! @brief Array of user callbacks */
extern CLOCK_MANAGER_CALLBACK_USER_CONFIG_T *g_clockCallbacksArr[];

#endif /* USER_CLOCK_CONFIG_H */
