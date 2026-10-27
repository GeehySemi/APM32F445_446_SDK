/*!
 * @file        user_can_config.h
 *
 * @brief       CAN configurations
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

#ifndef USER_CAN_CONFIG_H
#define USER_CAN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_can.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CAN_ClassicFrames
  @{
*/
/**@} end of group CAN_ClassicFrames*/
/**@} end of group Examples*/

extern CAN_USER_CONFIG_T g_canConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_CAN_CONFIG_H */
