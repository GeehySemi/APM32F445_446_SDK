/*!
 * @file        user_cfgio_i2c_config.c
 *
 * @brief       CFGIO_I2C configurations
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
#include "user_cfgio_i2c_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Variables Variables
  @{
*/

CFGIO_I2C_MASTER_USER_CONFIG_T g_i2cMasterConfig = {
    .slaveAddr = 0U,
    .sclPin = 2U,
    .sdaPin = 3U,
    .baudrate = 100000UL,
    .transferType = CFGIO_USE_INTERRUPTS,
    .txDmaChannel = 0,
    .rxDmaChannel = 1,
    .callback = I2cMasterCallback,
    .callbackParam = NULL
};

/**@} end of group CFGIO_I2C_Master_Variables*/
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/
