/*!
 * @file        user_peripheral_config.c
 *
 * @brief       Peripheral configurations
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
#include "user_lpi2c_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPI2C_TwoBoardsInterrupt_Slave
  @{
  */

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Variables Variables
  @{
  */

/* Configuration for the LPI2C slave */
LPI2C_SLAVE_USER_CONFIG_T g_lpi2cSlaveUserConfig =
{
    .i2cMode       = LPI2C_FAST_MODE,
    .transferType  = LPI2C_USE_INTERRUPTS,
    .isAddrFor10bit   = false,
    .slaveAddr     = LPI2C_SLAVE_ADDR,
    .isListening   = true,
    .dmaChannel    = 0U,
    .slaveCallback = LPI2C_SlaveCallback,
    .callbackParam = NULL
};

/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Variables */
/**@} end of group LPI2C_TwoBoardsInterrupt_Slave */
/**@} end of group Examples */
