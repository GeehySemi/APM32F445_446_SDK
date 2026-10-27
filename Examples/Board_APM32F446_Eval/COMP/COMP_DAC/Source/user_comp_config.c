/*!
 * @file        user_comp_config.c
 *
 * @brief       Comp configurations
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
#include "user_comp_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup COMP_DAC
  @{
*/

/** @defgroup COMP_DAC_Variables Variables
  @{
*/
const COMP_MODULE_T g_compConfig =
{
    .comparator =
    {
        .dmaTrigStatus              = false,
        .outputIntTrig              = COMP_EVEN_INT_BY_BOTH_EDGES,
        .mode                       = COMP_CONSTANT_MODE,
        .filtSmpCnt                 = 0U,
        .filtSmpPeriod              = 0U,
        .pwrMode                    = COMP_SPEED_LOW,
        .invertStatus               = COMP_NORMAL,
        .offset                     = COMP_LEVEL_OFFSET_0,
        .hysLev                     = COMP_LEVEL_HYS_0,
        .outputSel                  = COMP_COUT,
        .pinStatus                  = COMP_UNUSABLE
    },
    .trigMode =
    {
        .roundRobinStatus           = false,
        .roundRobinIntStatus        = false,
        .fixedPm                    = COMP_PLUS_FIXED,
        .fixedChannel               = 0U,
        .samplesNumber              = 0U,
        .initDelay                  = 0U,

        /* enable all Channels from 0 for round robin check */
        .roundRobinChannelsStatus   = 255U,

        /* Initial comparison results for channel 0 to 7  is 1 */
        .programedStatus            = 255U
    },
    .dac =
    {
        .voltRefSrc     = COMP_VOLT_REF_SRC_VIN1,
        .volt           = 85U,
        .state          = true,
    },
    .mux =
    {
        .posInputMux    = 0U,
        .negInputMux    = 1U,
        .posPmMux       = COMP_MUX,
        .negPmMux       = COMP_DAC
    }
};

/**@} end of group COMP_DAC_Variables*/
/**@} end of group COMP_DAC*/
/**@} end of group Examples*/
