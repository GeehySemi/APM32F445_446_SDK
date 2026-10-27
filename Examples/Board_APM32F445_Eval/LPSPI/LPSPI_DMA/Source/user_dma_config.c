/*!
 * @file        user_dma_config.c
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
#include "user_dma_config.h"
#include "apm32f445_446_lpspi.h"
#include "apm32f445_446_dma.h"
/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup SPI_DMA
  @{
  */

/** @defgroup SPI_DMA_Variables Variables
  @{
*/

DMA_STATE_T g_dmaState;

DMA_CHN_STATE_T g_dmaChannel0State;

DMA_CHN_STATE_T g_dmaChannel1State;

DMA_CHN_STATE_T g_dmaChannel2State;

DMA_CHN_STATE_T g_dmaChannel3State;

DMA_CHN_STATE_T * const g_dmaChannelStateArray[] = {
    &g_dmaChannel0State,
    &g_dmaChannel1State,
    &g_dmaChannel2State,
    &g_dmaChannel3State,
};

DMA_CHANNEL_CONFIG_T g_dmaChannel0Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL0_NUM,
    .source = DMA_REQUEST_LPSPI1_RX,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

DMA_CHANNEL_CONFIG_T g_dmaChannel1Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL1_NUM,
    .source = DMA_REQUEST_LPSPI1_TX,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

DMA_CHANNEL_CONFIG_T g_dmaChannel2Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL2_NUM,
    .source = DMA_REQUEST_LPSPI1_RX,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

DMA_CHANNEL_CONFIG_T g_dmaChannel3Config = {
    .channelPriority = DMA_CHANNEL_DEFAULT_PRIORITY,
    .vtChannel = DMA_CHANNEL3_NUM,
    .source = DMA_REQUEST_LPSPI1_TX,
    .callback = NULL,
    .callbackParam = NULL,
    .enableTrigger = false,
};

const DMA_CHANNEL_CONFIG_T * const g_dmaChannelConfigArray[] = {
    &g_dmaChannel0Config,
    &g_dmaChannel1Config,
    &g_dmaChannel2Config,
    &g_dmaChannel3Config,
};

const DMA_USER_CONFIG_T g_dmaInitConfig = {
    .chnArbitration = DMA_ARBITRATION_FIXED_PRIORITY,
    .haltOnError = false
};

/**@} end of group SPI_DMA_Variables*/
/**@} end of group SPI_DMA */
/**@} end of group Examples */



