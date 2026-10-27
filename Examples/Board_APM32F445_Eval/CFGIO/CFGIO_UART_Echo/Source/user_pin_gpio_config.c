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
#include "user_pin_gpio_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CFGIO_UART_Echo
  @{
*/

/** @defgroup CFGIO_UART_Echo_Variables Variables
  @{
*/

PIN_SETTINGS_CFG_T g_pinsConfig[NUM_OF_CONFIGURED_PINS0] = {
    {   /* CFGIO_UART TX */
        .base           = PMA,
        .pinPmIdx       = 0U,
        .mux            = PM_MUX_ALT4,
        .gpioBase       = NULL,
        .clrIntFlg      = false,
        .digitFilt      = false,
        .passiveFilt    = false,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .pullCfg        = PM_INTERNAL_PULL_NOT_ENABLED,
    },
    {   /* CFGIO_UART RX */
        .base           = PMA,
        .pinPmIdx       = 1U,
        .mux            = PM_MUX_ALT4,
        .gpioBase       = NULL,
        .clrIntFlg      = false,
        .digitFilt      = false,
        .passiveFilt    = false,
        .pinLk          = false,
        .intCfg         = PM_DMA_INT_DISABLED,
        .drvSel         = PM_LOW_DRIVE_STRENGTH,
        .pullCfg        = PM_INTERNAL_PULL_NOT_ENABLED,
    }
};

/**@} end of group CFGIO_UART_Echo_Variables*/
/**@} end of group CFGIO_UART_Echo*/
/**@} end of group Examples*/
