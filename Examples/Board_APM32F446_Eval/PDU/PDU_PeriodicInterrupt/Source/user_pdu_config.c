/*!
 * @file        user_peripheral_config.c
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
#include "user_pdu_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup PDU_PeriodicInterrupt
  @{
  */

/** @defgroup PDU_PeriodicInterrupt_Variables Variables
  @{
  */

/* Configuration for the PDU timer */
const PDU_TIMER_CONFIG_T g_pduTimerConfig =
{
    .loadValMode            = PDU_LOAD_VAL_IMMEDIATELY,
    .clkPscDiv              = PDU_CLK_PSCDIV128,
    .clkPscMultFactor       = PDU_CLK_PSCMULT_FACTOR_AS_10,
    .triggerInput           = PDU_SOFTWARE_TRIGGER,
    .intEn                  = true,
    .seqErrIntEn            = false,
    .continuousModeEn       = true,
    .dmaEn                  = false,
    .instanceBackToBackEn   = false,
};

/**@} end of group PDU_PeriodicInterrupt_Variables */
/**@} end of group PDU_PeriodicInterrupt */
/**@} end of group Examples */
