/*!
 * @file        user_peripheral_config.h
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

#ifndef USER_PDU_CONFIG_H
#define USER_PDU_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_pdu.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup PDU_PeriodicInterrupt
  @{
  */
/**@} end of group PDU_PeriodicInterrupt */
/**@} end of group Examples */

extern const PDU_TIMER_CONFIG_T g_pduTimerConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_PDU_CONFIG_H */
