/*!
 * @file        user_lin_config.c
 *
 * @brief       lin configurations
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
#include "user_lin_config.h"

#include <stdbool.h>
#include <stddef.h>
#include "user_lin_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup LIN_Master
  @{
*/

/** @defgroup LIN_Master_Variables Variables
  @{
*/

/*! lin1 configuration structure */

LIN_CFG_T g_lin1UserConfig = {
    .baudrate = 9600U,
    .nodeType = LIN_MASTER,
    .autobaudEnable = false,
    .getTimeIntervalCallback = LinGetTimeIntervalCallback,
    .classicPID = NULL,
    .numOfClassicPID = 255U,
};

/*! Driver state structure */
LIN_STATE_T g_lin1State;

/**@} end of group LIN_Master_Variables*/
/**@} end of group LIN_Master*/
/**@} end of group Examples*/