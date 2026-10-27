/*!
 * @file        user_lpuart_config.c
 *
 * @brief       lpuart configuration
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
#include "user_lpuart_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup LPUART_DMA
  @{
*/

/** @defgroup LPUART_DMA_Variables Variables
  @{
*/

/* LPUART1 configuration structure */
const LPUART_CFG_T g_lpuart1UserConfig =
{
    .baudrate       = 115200U,
    .dataBits       = LPUART_8_BITS_DATA,
    .parityMode     = LPUART_PARITY_DISABLED,
    .stopBits       = LPUART_ONE_STOP_BIT,
    .transferType   = LPUART_USE_DMA,
    .rxDmaChannel   = 2U,
    .txDmaChannel   = 1U,
};

LPUART_STATE_T g_lpuart1Status;

/**@} end of group LPUART_DMA_Variables*/
/**@} end of group LPUART_DMA*/
/**@} end of group Examples*/
