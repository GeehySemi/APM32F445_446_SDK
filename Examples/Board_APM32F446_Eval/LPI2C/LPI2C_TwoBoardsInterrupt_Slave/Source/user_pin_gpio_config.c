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

/** @addtogroup LPI2C_TwoBoardsInterrupt_Slave
  @{
  */

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Variables Variables
  @{
  */

PIN_SETTINGS_CFG_T g_pinsConfig[CONFIGURED_PINS_CNT] = {
    {
        .base            = PMC,
        .pinPmIdx        = 0U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOC,
        .dir             = GPIO_OUTPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMC,
        .pinPmIdx        = 1U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOC,
        .dir             = GPIO_OUTPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMC,
        .pinPmIdx        = 12U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOC,
        .dir             = GPIO_INPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMC,
        .pinPmIdx        = 13U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOC,
        .dir             = GPIO_INPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMD,
        .pinPmIdx        = 0U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOD,
        .dir             = GPIO_OUTPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMD,
        .pinPmIdx        = 15U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOD,
        .dir             = GPIO_OUTPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMD,
        .pinPmIdx        = 16U,
        .pullCfg         = PM_INTERNAL_PULL_NOT_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_AS_GPIO,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = GPIOD,
        .dir             = GPIO_OUTPUT_DIR,
        .digitFilt       = false,
        .initVal         = 0U,
    },
    {
        .base            = PMA,
        .pinPmIdx        = 2U,
        .pullCfg         = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_ALT3,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = NULL,
        .digitFilt       = false,
    },
    {
        .base            = PMA,
        .pinPmIdx        = 3U,
        .pullCfg         = PM_INTERNAL_PULL_UP_ENABLED,
        .drvSel          = PM_LOW_DRIVE_STRENGTH,
        .passiveFilt     = false,
        .mux             = PM_MUX_ALT3,
        .pinLk           = false,
        .intCfg          = PM_DMA_INT_DISABLED,
        .clrIntFlg       = false,
        .gpioBase        = NULL,
        .digitFilt       = false,
    },
};

/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Variables */
/**@} end of group LPI2C_TwoBoardsInterrupt_Slave */
/**@} end of group Examples */
