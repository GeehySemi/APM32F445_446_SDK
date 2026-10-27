/*!
 * @file        user_dma_config.h
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

#ifndef USER_DMA_CONFIG_H
#define USER_DMA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_lpspi.h"
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup SPI_DMA
  @{
  */

/** @defgroup SPI_DMA_Macros Macros
  @{
*/

/* DMA channel number 0 */
#define DMA_CHANNEL0_NUM   0U
/* DMA channel number 1 */
#define DMA_CHANNEL1_NUM   1U
/* DMA channel number 2 */
#define DMA_CHANNEL2_NUM   2U
/* DMA channel number 3 */
#define DMA_CHANNEL3_NUM   3U
/* Number of DMA channels */
#define DMA_CHANNELS_COUNT  4U

/**@} end of group SPI_DMA_Macros*/
/**@} end of group SPI_DMA */
/**@} end of group Examples */

/* A Driver state structure that holds driver runtime data */
extern DMA_STATE_T g_dmaState;

/* DMA channel state */
extern DMA_CHN_STATE_T g_dmaChannel0State;
extern DMA_CHN_STATE_T g_dmaChannel1State;
extern DMA_CHN_STATE_T g_dmaChannel2State;
extern DMA_CHN_STATE_T g_dmaChannel3State;

/* Array of channel state structures */
extern DMA_CHN_STATE_T * const g_dmaChannelStateArray[DMA_CHANNELS_COUNT];

/* Dma channel configuration */
extern DMA_CHANNEL_CONFIG_T g_dmaChannel0Config;
extern DMA_CHANNEL_CONFIG_T g_dmaChannel1Config;
extern DMA_CHANNEL_CONFIG_T g_dmaChannel2Config;
extern DMA_CHANNEL_CONFIG_T g_dmaChannel3Config;

/* Array of channel configuration structures */
extern const DMA_CHANNEL_CONFIG_T * const g_dmaChannelConfigArray[DMA_CHANNELS_COUNT];

/* DMA User configuration */
extern const DMA_USER_CONFIG_T g_dmaInitConfig;

#ifdef __cplusplus
}
#endif
#endif /* USER_ADC_CONFIG_H */
