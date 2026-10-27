/*!
 * @file        user_peripherals_flash_FTFC.c
 *
 * @brief       This file defines user flash config
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
#include "user_flash_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup FLASH_Protection
  @{
*/

/** @defgroup FLASH_Protection_Variables Variables
  @{
*/
/* Flash user configuration */
FLASH_USER_CONFIG_T g_flashConfig =
{
    .eeramBase = FEATURE_FLASH_CFGRAM_START_ADDRESS,
    .dflashBase = FEATURE_FLASH_DF_START_ADDRESS,
    .pflashBase = 0x0U,
    .pflashSize = FEATURE_FLASH_PF_BLOCK_SIZE,
    .callBack = CALLBACK_IS_NULL
};

/**@} end of group FLASH_Protection_Variables*/
/**@} end of group FLASH_Protection*/
/**@} end of group Examples*/
