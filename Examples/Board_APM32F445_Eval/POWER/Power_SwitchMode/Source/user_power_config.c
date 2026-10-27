/*!
 * @file        user_wdt_config.c
 *
 * @brief       This file provides all the setting of user power configurations
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

/* Included */
#include "user_power_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup Power_SwitchMode
  @{
*/

/** @defgroup Power_SwitchMode_Variables Variables
  @{
*/

/* User Configuration structure power_config */
POWER_USER_CONFIG_T g_powerConfigHSRUN =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_HSRUN,
};

POWER_USER_CONFIG_T g_powerConfigRUN =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_RUN,
};


POWER_USER_CONFIG_T g_powerConfigVLPR =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_VLPR,
};


POWER_USER_CONFIG_T g_powerConfigSTOP1 =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_STOP1,
};


POWER_USER_CONFIG_T g_powerConfigSTOP2 =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_STOP2,
};

POWER_USER_CONFIG_T g_powerConfigVLPS =
{
    .sleepOnExitValue         = false,
    .powerMode                = POWER_MODE_VLPS,
};

/*! @brief Array of pointers to User configuration structures */
POWER_USER_CONFIG_T * g_powerConfigsArr[POWER_CONFIG_COUNT] =
{
    &g_powerConfigHSRUN,
    &g_powerConfigRUN,
    &g_powerConfigVLPR,
    &g_powerConfigSTOP1,
    &g_powerConfigSTOP2,
    &g_powerConfigVLPS
};

/*! @brief Array of pointers to User defined Callbacks configuration structures */
POWER_CALLBACK_USER_CONFIG_T * g_powerCallbacksArr[6] =
{
    (void *)0,
    (void *)0,
    (void *)0,
    (void *)0,
    (void *)0,
    (void *)0
};

/**@} end of group Power_SwitchMode_Variables*/
/**@} end of group Power_SwitchMode*/
/**@} end of group Examples*/
