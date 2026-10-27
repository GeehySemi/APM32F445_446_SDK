/*!
 * @file        user_pin_gpio_config.h
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

#ifndef USER_PIN_GPIO_CONFIG_H
#define USER_PIN_GPIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CAN_FdFrames
  @{
*/

/** @defgroup CAN_FdFrames_Macros Macros
  @{
*/

#define NUM_OF_CONFIGURED_PINS0 2

/**@} end of group CAN_FdFrames_Macros*/
/**@} end of group CAN_FdFrames*/
/**@} end of group Examples*/

extern PIN_SETTINGS_CFG_T g_pinsConfig[NUM_OF_CONFIGURED_PINS0];

#ifdef __cplusplus
}
#endif

#endif /* USER_PIN_GPIO_CONFIG_H */
