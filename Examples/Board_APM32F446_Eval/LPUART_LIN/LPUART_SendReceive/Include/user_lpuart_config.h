/*!
 * @file        user_lpuart_config.h
 *
 * @brief       lpuart configurations
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

#ifndef USER_LPUART_CONFIG_H
#define USER_LPUART_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "user_clock_config.h"
#include "apm32f445_446_lpuart.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPUART_SendReceive
  @{
  */

/** @defgroup LPUART_SendReceive_Macros Macros
  @{
*/

#define LPUART1_INSTANCE    (1U)

/**@} end of group LPUART_SendReceive_Macros*/
/**@} end of group LPUART_SendReceive */
/**@} end of group Examples */

extern const LPUART_CFG_T g_lpuart1UserConfig;
extern LPUART_STATE_T g_lpuart1Status;

#ifdef __cplusplus
}
#endif

#endif /* USER_LPUART_CONFIG_H */
