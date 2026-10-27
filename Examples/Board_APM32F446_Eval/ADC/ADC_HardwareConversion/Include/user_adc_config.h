/*!
 * @file        user_adc_config.h
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

#ifndef USER_ADC_CONFIG_H
#define USER_ADC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_adc.h"
#include "apm32f445_446_pdu.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup ADC_HardwareConversion
  @{
*/

/**@} end of group ADC_HardwareConversion*/
/**@} end of group Examples*/

extern ADC_CHAN_CONFIG_T adcChanCfg0;
extern ADC_CONV_CFG_T adcConvCfg0;
extern ADC_CALIBRATION_T adcCalibration0;
extern PDU_TIMER_CONFIG_T pduTimerCfg0;
extern PDU_ADC_PRETRIGGER_CONFIG_T pduAdcTriggerCfg0;

#ifdef __cplusplus
}
#endif
#endif /* USER_ADC_CONFIG_H */
