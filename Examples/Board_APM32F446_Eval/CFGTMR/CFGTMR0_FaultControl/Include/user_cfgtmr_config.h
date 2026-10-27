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

#ifndef USER_CFGTMR_CONFIG_H
#define USER_CFGTMR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_cfgtmr.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CFGTMR0_FaultControl
  @{
  */
/**@} end of group CFGTMR0_FaultControl */
/**@} end of group Examples */

extern CFGTMR_USER_CONFIG_T g_cfgtmr0InitConfig;
extern CFGTMR_PWM_FAULT_PARAM_T g_cfgtmr0PWMFaultConfig;
extern CFGTMR_INDEPENDENT_CHAN_PARAM_T g_cfgtmr0PWMIndependentChannelsConfig[2];
extern CFGTMR_PWM_PARAM_T g_cfgtmr0PWMInitConfig;

extern CFGTMR_USER_CONFIG_T g_cfgtmr1InitConfig;
extern CFGTMR_PWM_FAULT_PARAM_T g_cfgtmr1PWMFaultConfig;
extern CFGTMR_INDEPENDENT_CHAN_PARAM_T g_cfgtmr1PWMIndependentChannelsConfig[1];
extern CFGTMR_PWM_PARAM_T g_cfgtmr1PWMInitConfig;

#ifdef __cplusplus
}
#endif

#endif /* USER_CFGTMR_CONFIG_H */
