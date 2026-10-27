/*!
 * @file        user_lpspi_config.h
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

#ifndef USER_LPSPI_CONFIG_H
#define USER_LPSPI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_lpspi.h"
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup SPI_Isr
  @{
  */

/** @defgroup SPI_Isr_Macros Macros
  @{
*/

#define LPSPI_1  1U

/**@} end of group SPI_Isr_Macros*/
/**@} end of group SPI_Isr */
/**@} end of group Examples */

extern LPSPI_STATE_T g_spiState;
extern LPSPI_MASTER_CFG_T g_spiMasterCfg;
extern LPSPI_SLAVE_CFG_T g_spiSlaveCfg;

#ifdef __cplusplus
}
#endif

#endif /* USER_LPSPI_CONFIG_H */
