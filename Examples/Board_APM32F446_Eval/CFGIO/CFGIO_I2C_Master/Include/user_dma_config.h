/*!
 * @file        user_dma_config.h
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

#ifndef USER_DMA_CONFIG_H
#define USER_DMA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_I2C_Master
  @{
*/

/** @defgroup CFGIO_I2C_Master_Macros Macros
  @{
*/

/* Total number of configured DMA channels */
#define DMA_CHANNEL_COUNT  2U

/* Channel number for channel 0 */
#define DMA_CHANNEL_0   0U

/* Channel number for channel 1 */
#define DMA_CHANNEL_1   1U

/**@} end of group CFGIO_I2C_Master_Macros*/
/**@} end of group CFGIO_I2C_Master*/
/**@} end of group Examples*/

/* DMA controller state */
extern DMA_STATE_T g_dmaControllerState;

/* DMA channel 0 state structure */
extern DMA_CHN_STATE_T g_dmaChannel0State;

/* DMA channel 1 state structure */
extern DMA_CHN_STATE_T g_dmaChannel1State;

/* Array of DMA channel states */
extern DMA_CHN_STATE_T * const g_dmaChannelStates[DMA_CHANNEL_COUNT];

/* DMA user configuration */
extern const DMA_USER_CONFIG_T g_dmaConfig;

/* DMA channel 0 configuration */
extern DMA_CHANNEL_CONFIG_T g_dmaChannel0Config;

/* DMA channel 1 configuration */
extern DMA_CHANNEL_CONFIG_T g_dmaChannel1Config;

/* Array of DMA channel configurations */
extern const DMA_CHANNEL_CONFIG_T * const g_dmaChannelConfigs[DMA_CHANNEL_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* USER_DMA_CONFIG_H */
