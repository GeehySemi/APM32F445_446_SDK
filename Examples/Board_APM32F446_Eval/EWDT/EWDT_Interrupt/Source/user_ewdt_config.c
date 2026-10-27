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
#include "user_ewdt_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup EWDT_Interrupt
  @{
  */

/** @defgroup EWDT_Interrupt_Variables Variables
  @{
  */

EWDT_INIT_CONFIG_T g_ewdtConfig =
{
    .interruptEnable = true,
    .prescaler = 255U,
    .compareHigh = 254U,
    .compareLow = 0U,
    .assertLogic = EWDT_IN_ASSERT_ON_LOGIC_ONE
};

/**@} end of group EWDT_Interrupt_Variables */
/**@} end of group EWDT_Interrupt */
/**@} end of group Examples */
