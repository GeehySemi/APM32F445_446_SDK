/*!
 * @file        user_ewdt_config.c
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
#include "user_erep_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup EREP_Report
  @{
  */

/** @defgroup EREP_Report_Variables Variables
  @{
  */

EREP_INTERRUPT_CONFIG_T g_erepIntConfig =
{
    .enableSingleCorrection = true,
    .enableNonCorrectable = true
};

EREP_USER_CONFIG_T g_erepConfig[EREP_CHANNEL_COUNTER] =
{
    {
        .channel = EREP_CHANNEL0,
        .interruptCfg = &g_erepIntConfig
    },
    {
        .channel = EREP_CHANNEL1,
        .interruptCfg = &g_erepIntConfig
    }
};

/**@} end of group EREP_Report_Variables */
/**@} end of group EREP_Report */
/**@} end of group Examples */
