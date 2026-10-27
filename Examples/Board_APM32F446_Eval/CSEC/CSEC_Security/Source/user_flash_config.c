/*!
 * @file        user_flash_config.c
 *
 * @brief       Flash configurations
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

/* Includes */
#include "user_flash_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CSEC_Security
  @{
*/

/** @defgroup CSEC_Security_Variables Variables
  @{
*/

/* Flash configuration */
FLASH_USER_CONFIG_T g_flashConfig =
{
    .eeramBase  = 0x14000000U,      /* Base address of CFGRAM block */
    .dflashBase = 0x10000000U,      /* Base address of Data Flash block */
    .pflashBase = 0x00000000U,      /* Base address of Program Flash block */
    .pflashSize = 0x00080000U,      /* Size of Program Flash block */
    .callBack   = CALLBACK_IS_NULL  /* If using callback, any code reachable from
                                       this function must not be placed in a Flash
                                       block targeted for a program/erase operation. */
};

/**@} end of group CSEC_Security_Variables*/
/**@} end of group CSEC_Security*/
/**@} end of group Examples*/
