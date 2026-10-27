/*!
 * @file        user_power_config.c
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

/* Includes */
#include "user_power_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CAN_PretendedNetworking
  @{
*/

/** @defgroup CAN_PretendedNetworking_Variables Variables
  @{
*/

POWER_USER_CONFIG_T g_powerConfigHSRUN =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_HSRUN,
};

POWER_USER_CONFIG_T g_powerConfigRUN =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_RUN,
};

POWER_USER_CONFIG_T g_powerConfigVLPR =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_VLPR,
};

POWER_USER_CONFIG_T g_powerConfigSTOP1 =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_STOP1,
};

POWER_USER_CONFIG_T g_powerConfigSTOP2 =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_STOP2,
};

POWER_USER_CONFIG_T g_powerConfigVLPS =
{
    .sleepOnExitValue = false,
    .powerMode = POWER_MODE_VLPS,
};

POWER_USER_CONFIG_T * g_powerConfigs[POWER_CONFIG_COUNT] =
{
    &g_powerConfigHSRUN,
    &g_powerConfigRUN,
    &g_powerConfigVLPR,
    &g_powerConfigSTOP1,
    &g_powerConfigSTOP2,
    &g_powerConfigVLPS
};

POWER_CALLBACK_USER_CONFIG_T * g_powerManagerCallbacks[1] = {(void *)0};

/**@} end of group CAN_PretendedNetworking_Variables*/
/**@} end of group CAN_PretendedNetworking*/
/**@} end of group Examples*/
