/*!
 * @file        user_dma_config.c
 *
 * @brief       DMA configurations
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
#include "user_dma_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Variables Variables
  @{
*/

DMA_STATE_T g_dmaControllerState;

DMA_CHN_STATE_T g_dmaChannel0State;
DMA_CHN_STATE_T g_dmaChannel1State;

DMA_CHN_STATE_T * const g_dmaChannelStates[DMA_CHANNEL_COUNT] = {
    &g_dmaChannel0State,
    &g_dmaChannel1State
};

DMA_CHANNEL_CONFIG_T g_dmaChannel0Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL_0,
    .source = DMA_REQUEST_CFGIO_SHIFTER1,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false
};

DMA_CHANNEL_CONFIG_T g_dmaChannel1Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL_1,
    .source = DMA_REQUEST_CFGIO_SHIFTER2,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false
};

const DMA_CHANNEL_CONFIG_T * const g_dmaChannelConfigs[DMA_CHANNEL_COUNT] = {
    &g_dmaChannel0Config,
    &g_dmaChannel1Config
};

const DMA_USER_CONFIG_T g_dmaConfig = {
    .chnArbitration = DMA_ARBITRATION_FIXED_PRIORITY,
    .haltOnError = false
};

/**@} end of group CFGIO_I2C_Master_Variables*/
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/
