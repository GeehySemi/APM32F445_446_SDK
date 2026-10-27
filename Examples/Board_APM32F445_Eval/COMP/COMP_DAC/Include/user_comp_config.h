/*!
 * @file        user_peripheral_config.h
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

#ifndef USER_PERIPHERAL_CONFIG_H
#define USER_PERIPHERAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "user_clock_config.h"
#include "apm32f445_446_comp.h"


/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup COMP_DAC
  @{
*/

/** @addtogroup COMP_DAC_Macros Macros
  @{
*/

#define INST_COMPARATOR1 0U

/**@} end of group COMP_DAC_Macros */
/**@} end of group COMP_DAC */
/**@} end of group Examples */

extern const COMP_MODULE_T g_compConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_PERIPHERAL_CONFIG_H */








