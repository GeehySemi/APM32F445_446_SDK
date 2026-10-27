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

#ifndef USER_EREP_CONFIG_H
#define USER_EREP_CONFIG_H

/* Includes */
#include "apm32f445_446_erep.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup EREP_Report
  @{
  */

/** @defgroup EREP_Report_Marcos Marcos
  @{
  */

#define EREP_CHANNEL_COUNTER    (2U)
#define EREP_CHANNEL0   (0U)
#define EREP_CHANNEL1   (1U)

/**@} end of group EREP_Report_Marcos */
/**@} end of group EREP_Report */
/**@} end of group Examples */

extern EREP_INTERRUPT_CONFIG_T g_erepIntConfig;
extern EREP_USER_CONFIG_T g_erepConfig[EREP_CHANNEL_COUNTER];

#endif /* USER_EREP_CONFIG_H */
