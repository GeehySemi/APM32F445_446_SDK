/*!
 * @file        user_power_config.h
 *
 * @brief       Power configurations
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

#ifndef USER_POWER_CONFIG_H
#define USER_POWER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_power.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CAN_PretendedNetworking
  @{
*/

/** @defgroup CAN_PretendedNetworking_Macros Macros
  @{
*/

/*! @brief Number of user configuration structures */
#define POWER_CONFIG_COUNT  6U

/*! @brief Number of user callbacks */
#define POWER_CALLBACK_CNT 0U

/**@} end of group CAN_PretendedNetworking_Macros*/
/**@} end of group CAN_PretendedNetworking*/
/**@} end of group Examples*/

/*! @brief Array of pointers to User configuration structures */
extern POWER_USER_CONFIG_T * g_powerConfigs[POWER_CONFIG_COUNT];

extern POWER_USER_CONFIG_T g_powerConfigHSRUN;
extern POWER_USER_CONFIG_T g_powerConfigRUN;
extern POWER_USER_CONFIG_T g_powerConfigVLPR;
extern POWER_USER_CONFIG_T g_powerConfigSTOP1;
extern POWER_USER_CONFIG_T g_powerConfigSTOP2;
extern POWER_USER_CONFIG_T g_powerConfigVLPS;

/*! @brief Array of pointers to user defined static Callbacks configuration structures */
extern POWER_CALLBACK_USER_CONFIG_T * g_powerManagerCallbacks[1];

#ifdef __cplusplus
}
#endif

#endif /* USER_POWER_CONFIG_H */
