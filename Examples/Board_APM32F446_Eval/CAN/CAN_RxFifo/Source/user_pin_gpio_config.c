/*!
 * @file        user_pin_gpio_config.c
 *
 * @brief       Pins configurations
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
#include "user_pin_gpio_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CAN_RxFifo
  @{
*/

/** @defgroup CAN_RxFifo_Variables Variables
  @{
*/

PIN_SETTINGS_CFG_T g_pinsConfig[NUM_OF_CONFIGURED_PINS0] = {
    {
        .base           = PME,
        .pinPmIdx       = 4U,
        .pullCfg        = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT5,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    },
    {
        .base           = PME,
        .pinPmIdx       = 5U,
        .pullCfg        = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT5,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    }
};

/**@} end of group CAN_RxFifo_Variables*/
/**@} end of group CAN_RxFifo*/
/**@} end of group Examples*/
