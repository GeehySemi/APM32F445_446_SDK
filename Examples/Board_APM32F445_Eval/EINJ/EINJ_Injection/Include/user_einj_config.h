/*!
 * @file        user_einj_config.h
 *
 * @brief       Peripheral configurations
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

#ifndef USER_EINJ_CONFIG_H
#define USER_EINJ_CONFIG_H

/* Includes */
#include "apm32f445_446_einj.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup EINJ_Injection
  @{
  */

/** @defgroup EINJ_Injection_Marcos Marcos
  @{
  */

#define EINJ_CHANNEL_COUNTER    (2U)
#define EINJ_CHANNEL0           (0U)
#define EINJ_CHANNEL1           (1U)

/**@} end of group EINJ_Injection_Marcos */
/**@} end of group EINJ_Injection */
/**@} end of group Examples */

extern const EINJ_USER_CHANNEL_CONFIG_T g_einjChannel0SingleBitConfig[EINJ_CHANNEL_COUNTER];
extern const EINJ_USER_CHANNEL_CONFIG_T g_einjChannel0MultBitConfig[EINJ_CHANNEL_COUNTER];

#endif /* USER_EINJ_CONFIG_H */
