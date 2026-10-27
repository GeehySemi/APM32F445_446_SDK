/*!
 * @file        user_ewdt_config.c
 *
 * @brief       Peripheral configurations
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
#include "user_einj_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup EINJ_Injection
  @{
  */

/** @defgroup EINJ_Injection_Variables Variables
  @{
  */

const EINJ_USER_CHANNEL_CONFIG_T g_einjChannel0SingleBitConfig[EINJ_CHANNEL_COUNTER] =
{
    {
        .channel = EINJ_CHANNEL0,
        .checkBitMask = 0x00U,
        .dataMask = 0x01U,
        .enable = true
    },
    {
        .channel = EINJ_CHANNEL1,
        .checkBitMask = 0x00U,
        .dataMask = 0x00U,
        .enable = false
    }
};

const EINJ_USER_CHANNEL_CONFIG_T g_einjChannel0MultBitConfig[EINJ_CHANNEL_COUNTER] =
{
    {
        .channel = EINJ_CHANNEL0,
        .checkBitMask = 0x0FU,
        .dataMask = 0x00U,
        .enable = true
    },
    {
        .channel = EINJ_CHANNEL1,
        .checkBitMask = 0x00U,
        .dataMask = 0x00U,
        .enable = false
    }
};

/**@} end of group EINJ_Injection_Variables */
/**@} end of group EINJ_Injection */
/**@} end of group Examples */
