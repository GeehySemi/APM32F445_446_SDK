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

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Variables Variables
  @{
*/

PIN_SETTINGS_CFG_T g_pinsConfig[NUM_OF_CONFIGURED_PINS0] = {
    {
        .base           = PMA,
        .pinPmIdx       = 0U,
        .pullCfg        = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT4,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    },
    {
        .base           = PMA,
        .pinPmIdx       = 1U,
        .pullCfg        = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT4,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    },
    {
        .base           = PMA,
        .pinPmIdx       = 2U,
        .pullCfg        = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT3,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    },
    {
        .base           = PMA,
        .pinPmIdx       = 3U,
        .pullCfg        = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt    = false,
        .mux            = PM_MUX_ALT3,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .clrIntFlg      = false,
        .gpioBase       = NULL,
        .digitFilt      = false,
    }
};

/**@} end of group CFGIO_I2C_Master_Variables*/
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/
