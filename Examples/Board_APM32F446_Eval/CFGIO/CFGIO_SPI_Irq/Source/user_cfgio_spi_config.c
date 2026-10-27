/*!
 * @file        user_cfgio_spi_config.c
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
#include "user_cfgio_spi_config.h"
#include "apm32f445_446_cfgio_spi.h"
#include "apm32f445_446_cfgio_i2c.h"
#include "apm32f445_446_dma.h"
/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CFGIO_SPI_Irq
  @{
  */

/** @defgroup CFGIO_SPI_Irq_Variables
  @{
  */

CFGIO_SPI_MASTER_CFG_T g_CFGIOSpiMasterCfg = {
  .baudrate = 100000UL,
  .cfgspiDirType = CFGIO_USE_INTERRUPTS,
  .firstBitOrder = CFGIO_SPI_SEND_MSB_FIRST,
  .transferSize = CFGIO_SPI_DATA_SIZE_1BYTE,
  .clockPol = 0U,
  .clockPha = 0U,
  .mosiPin = 0U,
  .misoPin = 1U,
  .sckPin = 2U,
  .ssPin = 3U,
  .callback = NULL,
  .callbackParam = NULL
};

/* CFGIOSPI slave configuration structure */
CFGIO_SPI_SLAVE_CFG_T g_CFGIOSpiSlaveCfg = {
  .cfgspiDirType = CFGIO_USE_INTERRUPTS,
  .firstBitOrder = CFGIO_SPI_SEND_MSB_FIRST,
  .transferSize = CFGIO_SPI_DATA_SIZE_1BYTE,
  .clockPol = 0U,
  .clockPha = 0U,
  .mosiPin = 4U,
  .misoPin = 5U,
  .sckPin = 6U,
  .ssPin = 7U,
  .callback = NULL,
  .callbackParam = NULL
};


/**@} end of group CFGIO_SPI_Irq_Variables */
/**@} end of group CFGIO_SPI_Irq */
/**@} end of group Examples */



