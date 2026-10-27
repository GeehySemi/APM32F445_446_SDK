/*!
 * @file        user_cfgio_spi_config.h
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

#ifndef USER_CFGIO_SPI_CONFIG_H
#define USER_CFGIO_SPI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_cfgio_spi.h"
#include "apm32f445_446_cfgio_i2c.h"
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup CFGIO_SPI_Irq
  @{
*/

/** @defgroup CFGIO_SPI_Irq_Macros Macros
  @{
*/
#define CFGIO_SPI_1  0U

/**@} end of group CFGIO_SPI_Irq_Macros*/
/**@} end of group CFGIO_SPI_Irq*/
/**@} end of group Examples*/

extern CFGIO_SPI_MASTER_CFG_T g_CFGIOSpiMasterCfg;
extern CFGIO_SPI_SLAVE_CFG_T g_CFGIOSpiSlaveCfg;

#ifdef __cplusplus
}
#endif

#endif /* USER_CFGIO_LPSPI_CONFIG_H */
