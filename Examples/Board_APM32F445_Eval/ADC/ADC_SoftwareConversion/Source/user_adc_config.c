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

/** @addtogroup ADC_SoftwareConversion
  @{
  */

/** @defgroup ADC_SoftwareConversion_Variables Variables
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

ADC_AVG_CFG_T adcAverCfg0 = {
    .hwAverage      = ADC_HW_AVERAGE_32,
    .hwAvgEnable    = true
};

ADC_CALIBRATION_T adcCalibration0 = {
    .userGain       = ADC_DEFAULT_SAMPLE_TIME,
    .userOffset     = 20000u
};

/**@} end of group ADC_SoftwareConversion_Variables */
/**@} end of group ADC_SoftwareConversion */
/**@} end of group Examples */
