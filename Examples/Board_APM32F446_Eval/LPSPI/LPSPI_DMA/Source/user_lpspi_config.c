/*!
 * @file        user_lpspi_config.c
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
#include "user_lpspi_config.h"
#include "apm32f445_446_lpspi.h"
#include "apm32f445_446_dma.h"
/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup SPI_DMA
  @{
  */

/** @defgroup SPI_DMA_Variables Variables
  @{
*/

/* Define state structure for current SPI instance */
LPSPI_STATE_T g_spiState;

LPSPI_MASTER_CFG_T g_spiMasterCfg = {
    .bitNumber          = 8U,
    .baudBitsPer        = 100000UL,
    .lpspiModSrcClk     = 8000000UL,
    .selectPcs          = LPSPI_PER_PCS0,
    .selectPcsPolar     = LPSPI_SIGNAL_ACTIVE_HIGH,
    .selectClkPhase     = LPSPI_CLOCK_PHASE_CFG_1ST,
    .clkPolar           = LPSPI_CLK_SIGNAL_HIGH,
    .callback           = NULL,
    .callbackParam      = NULL,
    .pcsKeeps           = false,
    .sendLsbFirst       = false,
    .transferType       = LPSPI_TRANSFER_DMA,
    .rxDMAChan          = 0U,
    .txDMAChan          = 1U
};

/* LPSPI Slave Configurations 0 */
LPSPI_SLAVE_CFG_T g_spiSlaveCfg = {
    .bitNumber          = 8U,
    .selectPcs          = LPSPI_PER_PCS0,
    .selectPcsPolar     = LPSPI_SIGNAL_ACTIVE_HIGH,
    .selectClkPhase     = LPSPI_CLOCK_PHASE_CFG_1ST,
    .clkPolar           = LPSPI_CLK_SIGNAL_HIGH,
    .callback           = NULL,
    .callbackParam      = NULL,
    .sendLsbFirst       = false,
    .transferType       = LPSPI_TRANSFER_DMA,
    .rxDMAChan          = 2U,
    .txDMAChan          = 3U
};

/**@} end of group SPI_DMA_Variables*/
/**@} end of group SPI_DMA */
/**@} end of group Examples */
