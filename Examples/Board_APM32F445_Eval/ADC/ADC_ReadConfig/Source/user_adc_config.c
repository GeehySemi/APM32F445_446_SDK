/*!
 * @file        user_adc_config.c
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

/* Includes */
#include "user_adc_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup ADC_ReadConfig
  @{
  */

/** @defgroup ADC_ReadConfig_Variables Variables
  @{
  */
ADC_CHAN_CONFIG_T adcChanCfg0 = {
    .channel            = ADC_INPUT_CHANNEL_EXT12,
    .interruptEnable    = false
};

/**@} end of group ADC_ReadConfig_Variables */
/**@} end of group ADC_ReadConfig */
/**@} end of group Examples */
