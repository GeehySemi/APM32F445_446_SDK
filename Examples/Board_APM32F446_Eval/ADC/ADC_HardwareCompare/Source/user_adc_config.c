/*!
 * @file        user_adc_config.c
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

/* Includes */
#include "user_adc_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup ADC_HardwareCompare
  @{
  */

 /** @defgroup ADC_HardwareCompare_Macros Macros
  @{
*/

/* Compare cvalue */
#define COMPVAL1    (0U)
#define COMPVAL2    (2000U)

/**@} end of group ADC_HardwareCompare_Macros*/

 /** @defgroup ADC_HardwareCompare_Variables Variables
  @{
*/
ADC_CHAN_CONFIG_T adcChanCfg0 = {
    .channel            = ADC_INPUT_CHANNEL_EXT12,
    .interruptEnable    = false
};

ADC_CONV_CFG_T adcConvCfg0 = {
    .resolution         = ADC_RESOLUTION_RATIO_12BIT,
    .voltRef            = ADC_VOLT_REF_VREF,
    .trigger            = ADC_SOFTWARE_TRIGGER,
    .selTrigger         = ADC_PDU_TRIGGER,
    .selPretrigger      = ADC_PDU_PRE_TRIGGER,
    .clockDivision      = ADC_CLK_DIVISION_4,
    .inputClock         = ADC_INPUT_CLK_1,
    .adcSampleTime      = 255U,
    .dmaEnable          = false,
    .supplyMonEnable    = false,
    .contConvEnable     = false
};

ADC_COMP_CFG_T adcCompareCfg0 = {
    .compEnable             = true,
    .compValue1             = COMPVAL1,
    .compValue2             = COMPVAL2,
    .compRangeFuncEnable    = true,
    .compGreaterThanEnable  = true
};

/**@} end of group ADC_HardwareCompare_Variables*/
/**@} end of group ADC_HardwareCompare */
/**@} end of group Examples */
