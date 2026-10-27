/*!
 * @file        user_peripheral_config.c
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
#include "user_lptmr_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPTMR_PeriodicInterrupt
  @{
  */

/** @defgroup LPTMR_PeriodicInterrupt_Variables Variables
  @{
  */

/* Configuration for the LPTMR module */
const LPTMR_CONFIG_T  g_lptmrConfig = {
    .workMode     = LPTMR_WORKMODE_TIMER,
    .dmaReq       = false,
    .intEnable    = true,
    .freeRun      = false,
    .clkSelect    = LPTMR_CLOCKSOURCE_LSICLKDIV2,
    .psc          = LPTMR_PRESCALE_2,
    .bypassPsc    = false,
    .compareValue = 1000000UL,
    .cntUnits     = LPTMR_COUNTER_UNITS_MICROSECONDS,
    .pinSelect    = LPTMR_PINSELECT_TRGMUX,
    .pinPolarity  = LPTMR_PINPOLARITY_RISING
};

/**@} end of group LPTMR_PeriodicInterrupt_Variables */
/**@} end of group LPTMR_PeriodicInterrupt */
/**@} end of group Examples */
