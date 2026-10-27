/*!
 * @file        user_lpi2c_config.c
 *
 * @brief       LPI2C configurations
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

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Variables Variables
  @{
*/

LPI2C_SLAVE_USER_CONFIG_T g_i2cSlaveConfig = {
    .i2cMode       = LPI2C_FAST_MODE,
    .transferType  = LPI2C_USE_INTERRUPTS,
    .isAddrFor10bit   = false,
    .slaveAddr     = I2C_SLAVE_ADDR,
    .isListening   = true,
    .dmaChannel    = 0U,
    .slaveCallback = I2cSlaveCallback,
    .callbackParam = NULL
};

/**@} end of group CFGIO_I2C_Master_Variables*/
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/
