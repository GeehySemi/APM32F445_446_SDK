/*!
 * @file        user_dma_config.h
 *
 * @brief       dma configurations
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

#ifndef USER_DMA_CONFIG_H
#define USER_DMA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "user_clock_config.h"
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPUART_DMA
  @{
  */

/** @defgroup LPUART_DMA_Macros Macros
  @{
*/

#define DMA_CHANNEL_0       0U
#define DMA_CHANNEL_1       1U
#define DMA_CHANNEL_2       2U
#define DMA_CHANNEL_COUNT   3U

/**@} end of group LPUART_DMA_Macros*/
/**@} end of group LPUART_DMA */
/**@} end of group Examples */

extern DMA_CHANNEL_CONFIG_T g_dmaChannel0Config;
extern DMA_CHANNEL_CONFIG_T g_dmaChannel1Config;
extern DMA_CHANNEL_CONFIG_T g_dmaChannel2Config;
extern DMA_STATE_T g_dmaControllerState;
extern DMA_CHN_STATE_T g_dmaChannel0State;
extern DMA_CHN_STATE_T g_dmaChannel1State;
extern DMA_CHN_STATE_T g_dmaChannel2State;
extern const DMA_USER_CONFIG_T g_dmaConfig;
extern DMA_CHN_STATE_T * const g_dmaChannelStates[DMA_CHANNEL_COUNT];
extern const DMA_CHANNEL_CONFIG_T * const g_dmaChannelConfigs[DMA_CHANNEL_COUNT];

extern void DmaCallback(void *parameter, DMA_CHANNEL_STATUS_T status);

#ifdef __cplusplus
}
#endif

#endif  /* USER_DMA_CONFIG_H */
