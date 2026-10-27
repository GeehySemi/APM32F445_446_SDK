/*!
 * @file        user_lpTmr_config.c
 *
 * @brief       lpTmr configurations
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

/** @addtogroup LIN_Slave
  @{
  */

/** @defgroup LIN_Slave_Variables Variables
  @{
  */

/*! lpTmr1 configuration structure */

const LPTMR_CONFIG_T g_lptmrConfig = {
  .workMode = LPTMR_WORKMODE_TIMER,
  .dmaReq = false,
  .intEnable = true,
  .freeRun = false,
  .compareValue = 1999U,
  .cntUnits = LPTMR_COUNTER_UNITS_TICKS,
  .clkSelect = LPTMR_CLOCKSOURCE_PCM,
  .psc = LPTMR_PRESCALE_2,
  .bypassPsc = false,
  .pinSelect = LPTMR_PINSELECT_TRGMUX,
  .pinPolarity = LPTMR_PINPOLARITY_RISING,
};

/**@} end of group LIN_Slave_Variables */
/**@} end of group LIN_Slave */
/**@} end of group Examples */
