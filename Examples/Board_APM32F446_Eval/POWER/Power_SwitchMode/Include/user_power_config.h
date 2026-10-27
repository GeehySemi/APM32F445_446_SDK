/*!
 * @file        user_power_config.h
 *
 * @brief       Header file of user_power_config.c
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

#ifndef USER_POWER_CONFIG_H
#define USER_POWER_CONFIG_H

/* Included */
#include "apm32f445_446_power.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup Power_SwitchMode
  @{
*/

/** @defgroup Power_SwitchMode_Macros Macros
  @{
*/

/*! @brief Count of user configuration structures */
#define POWER_CONFIG_COUNT 6U
/*! @brief Count of user Callbacks */
#define POWER_CALLBACK_COUNT 0U

/**@} end of group Power_SwitchMode_Macros*/
/**@} end of group Power_SwitchMode*/
/**@} end of group Examples*/

/* User Configuration structure power_config */
extern POWER_USER_CONFIG_T g_powerConfigHSRUN;
extern POWER_USER_CONFIG_T g_powerConfigRUN;
extern POWER_USER_CONFIG_T g_powerConfigVLPR;
extern POWER_USER_CONFIG_T g_powerConfigSTOP1;
extern POWER_USER_CONFIG_T g_powerConfigSTOP2;
extern POWER_USER_CONFIG_T g_powerConfigVLPS;

/* Power_config array */
extern POWER_USER_CONFIG_T * g_powerConfigsArr[POWER_CONFIG_COUNT];

/* Power_callback array */
extern POWER_CALLBACK_USER_CONFIG_T * g_powerCallbacksArr[6];

#endif /* USER_POWER_CONFIG_H */
