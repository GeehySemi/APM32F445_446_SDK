/*!
 * @file        user_clock_config.c
 *
 * @brief       This file provides all the setting of user clock
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

/* Included */
#include "user_clock_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CLOCK_SeparateConfig
  @{
*/

/** @defgroup CLOCK_SeparateConfig_Variables Variables
  @{
*/
CLOCK_SRC_CONFIG_T SoscClockSource =
{
    .enable = true,
    .refClk = XOSC_INT_OSC,
    .refFreq = CPU_EXTERNAL_CLK_HZ,
    .outputDiv1 = 1U,
    .outputDiv2 = 1U
};
CLOCK_SRC_CONFIG_T HsiClockSource =
{
    .enable = true,
    .outputDiv1 = 1U,
    .outputDiv2 = 1U
};

SYS_CLK_CONFIG_T systemClockCfg =
{
    .src = HSICLK_CLK,
    .dividers[0] = SCG_SYS_CLOCK_DIV_BY_1,
    .dividers[1] = SCG_SYS_CLOCK_DIV_BY_1,
    .dividers[2] = SCG_SYS_CLOCK_DIV_BY_2
};
MODULE_CLK_CONFIG_T PMDClockCfg =
{
    .gating = true,
    .src = CLOCK_NAME_MAX_COUNT,
    .div = DIV_BY_ONE,
    .mul = MUL_BY_ONE
};
MODULE_CLK_CONFIG_T PMCClockCfg =
{
    .gating = true,
    .src = CLOCK_NAME_MAX_COUNT,
    .div = DIV_BY_ONE,
    .mul = MUL_BY_ONE
};
MODULE_CLK_CONFIG_T lpuart1ClockCfg =
{
    .gating = true,
    .src = SYSOSC_CLK,
    .div = DIV_BY_ONE,
    .mul = MUL_BY_ONE
};

/**@} end of group CLOCK_SeparateConfig_Variables*/
/**@} end of group CLOCK_SeparateConfig*/
/**@} end of group Examples*/
