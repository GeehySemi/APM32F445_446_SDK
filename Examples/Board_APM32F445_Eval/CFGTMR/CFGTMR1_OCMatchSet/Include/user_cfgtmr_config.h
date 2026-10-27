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

#ifndef USER_CFGTMR_CONFIG_H
#define USER_CFGTMR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_cfgtmr.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CFGTMR1_OCMatchSet
  @{
  */

/** @defgroup CFGTMR1_OCMatchSet_Macros Macros
  @{
*/

/* CFGTMR output compare channels number */
#define CFGTMR_OC_CHANNEL_NUMBER 3

/**@} end of group CFGTMR1_OCMatchSet_Macros*/
/**@} end of group CFGTMR1_OCMatchSet */
/**@} end of group Examples */

extern CFGTMR_USER_CONFIG_T g_cfgtmr1InitConfig;
extern CFGTMR_OUTPUT_CMP_CHAN_PARAM_T g_cfgtmr1OCChannelsConfig[CFGTMR_OC_CHANNEL_NUMBER];
extern CFGTMR_OUTPUT_CMP_PARAM_T g_cfgtmr1OCInitConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_CFGTMR_CONFIG_H */
