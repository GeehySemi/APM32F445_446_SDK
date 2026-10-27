/*!
 * @file        user_wdt_config.c
 *
 * @brief       This file provides all the setting of user wdt
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
#include "user_wdt_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup WDT_FastTest
  @{
*/

/** @defgroup WDT_FastTest_Variables Variables
  @{
*/
const WDT_CONFIG_T wdtConfig =
{
    .clkSrc = WDT_LPO_CLOCK,
    .updateEnable = true,
    .intEnable = false,
    .timeoutValue = 0xAAAAU,
    .windowEnable = false,
    .windowValue = 0U,
    .prescalerEnable = true,
    .opMode =
    {
        false,    /* Wait Mode */
        false,    /* Stop Mode */
        false     /* Debug Mode*/
    }
};

/**@} end of group WDT_FastTest_Variables*/
/**@} end of group WDT_FastTest*/
/**@} end of group Examples*/
