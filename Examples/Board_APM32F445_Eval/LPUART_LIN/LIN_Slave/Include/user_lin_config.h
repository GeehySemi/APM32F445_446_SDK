/*!
 * @file        user_lin_config.h
 *
 * @brief       lin configurations
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

#ifndef USER_LIN_CONFIG_H
#define USER_LIN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_lin.h"
#include <stdint.h>


/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LIN_Slave
  @{
  */

/** @defgroup LIN_Slave_Macros Macros
  @{
  */

#define LIN1_INSTANCE (2U)

/**@} end of group LIN_Slave_Macros */
/**@} end of group LIN_Slave */
/**@} end of group Examples */

extern LIN_STATE_T g_lin1State;
extern LIN_CFG_T g_lin1UserConfig;

extern uint32_t LinGetTimeIntervalCallback(uint32_t *ns);


#ifdef __cplusplus
}
#endif

#endif /* USER_LIN_CONFIG_H */
