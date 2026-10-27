/*!
 * @file        user_pin_gpio_config.c
 *
 * @brief       Pins configurations
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
//#include "user_pin_gpio_config.h"
#include "user_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LIN_Slave
  @{
  */

/** @defgroup LIN_Slave_Variables Variables
  @{
  */

PIN_SETTINGS_CFG_T g_pinsConfig[NUM_OF_CONFIGURED_PINS] =
{
    {
        .base        = PME,
        .pinPmIdx    = 9u,
        .pullCfg     = PM_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilt = false,
        .drvSel      = PM_LOW_DRIVE_STRENGTH,
        .mux         = PM_MUX_AS_GPIO,
        .pinLk       = false,
        .intCfg      = PM_DMA_INT_DISABLED,
        .clrIntFlg   = false,
        .gpioBase    = GPIOE,
        .dir         = GPIO_OUTPUT_DIR,
        .digitFilt   = false,
        .initVal     = 0u,
    },
    {
        .base        = PMD,
        .pinPmIdx    = 7u,
        .pullCfg     = PM_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilt = false,
        .drvSel      = PM_LOW_DRIVE_STRENGTH,
        .mux         = PM_MUX_ALT2,
        .pinLk       = false,
        .intCfg      = PM_DMA_INT_DISABLED,
        .clrIntFlg   = false,
        .gpioBase    = NULL,
        .digitFilt   = false,
    },
    {
        .base        = PMD,
        .pinPmIdx    = 6u,
        .pullCfg     = PM_INTERNAL_PULL_NOT_ENABLED,
        .passiveFilt = false,
        .drvSel      = PM_LOW_DRIVE_STRENGTH,
        .mux         = PM_MUX_ALT2,
        .pinLk       = false,
        .intCfg      = PM_INT_EITHER_EDGE,
        .clrIntFlg   = false,
        .gpioBase    = NULL,
        .digitFilt   = false,
    }
};

/**@} end of group LIN_Slave_Variables */
/**@} end of group LIN_Slave */
/**@} end of group Examples */
