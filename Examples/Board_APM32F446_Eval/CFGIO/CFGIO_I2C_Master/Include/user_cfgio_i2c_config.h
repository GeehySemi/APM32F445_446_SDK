/*!
 * @file        user_cfgio_i2c_config.h
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

#ifndef USER_CFGIO_I2C_CONFIG_H
#define USER_CFGIO_I2C_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_cfgio_i2c.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Macros Macros
  @{
*/

/* I2C instance */
#define CFGIO_I2C_INSTANCE  (0U)

/**@} end of group CFGIO_I2C_Master_Macros*/

/* I2C master configurations */
extern CFGIO_I2C_MASTER_USER_CONFIG_T g_i2cMasterConfig;

/** @defgroup CFGIO_I2C_Master_Functions INT_Functions
  @{
*/

/* I2C master callback */
extern void I2cMasterCallback(I2C_MASTER_EVENT_T masterEvent, void *userData);

/**@} end of group CFGIO_I2C_Master_Functions */
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/

#ifdef __cplusplus
}
#endif

#endif /* USER_CFGIO_I2C_CONFIG_H */
