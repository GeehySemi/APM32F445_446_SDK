/*!
 * @file        user_lpitmr_config.c
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
#include "user_lpitmr_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPITMR_Counter
  @{
  */

/** @defgroup LPITMR_Counter_Variables Variables
  @{
  */

LPITMR_USER_CONFIG_T g_lpitmrConfig =
{
    .runInDebugEn = false,
    .runInDozeEn = false,
};

LPITMR_USER_CHANNEL_CONFIG_T g_lpitmrChannelConfig =
{
    .timerMode = LPITMR_PERIODIC_COUNTER,
    .periodUnits = LPIT_PERIOD_UNITS_COUNTS,
    .period = 9999999U,
    .triggerSrc = LPITMR_TRIG_SRC_INTERNAL,
    .triggerSelect = 0,
    .reloadOnTriggerEn = true,
    .stopOnInterruptEn = true,
    .startOnTriggerEn = true,
    .channelChainEn = false,
    .interruptEn = true
};

/**@} end of group LPITMR_Counter_Variables */
/**@} end of group LPITMR_Counter */
/**@} end of group Examples */
