/*!
 * @file        user_cfgio_uart_config.c
 *
 * @brief       CFGIO_UART configurations
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
#include "user_cfgio_uart_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_UART_Echo
  @{
*/

/** @defgroup CFGIO_UART_Echo_Variables Variables
  @{
*/

/* CFGIOUART configuration structure */
CFGIO_UART_USER_CONFIG_T g_uartRxConfig = {
  .direction = CFGIO_UART_DIR_RX,
  .transferType = CFGIO_USE_INTERRUPTS,
  .baudrate = 115200UL,
  .numBits = 8U,
  .cfgioPin = 3U,
  .dmaChannel = 0,
  .callback = CFGIOUartRxCallback,
  .callbackParam = NULL
};

/* CFGIOUART configuration structure */
CFGIO_UART_USER_CONFIG_T g_uartTxConfig = {
  .direction = CFGIO_UART_DIR_TX,
  .transferType = CFGIO_USE_INTERRUPTS,
  .baudrate = 115200UL,
  .numBits = 8U,
  .cfgioPin = 2U,
  .dmaChannel = 1,
  .callback = CFGIOUartTxCallback,
  .callbackParam = NULL
};

/**@} end of group CFGIO_UART_Echo_Variables*/
/**@} end of group CFGIO_UART_Echo*/
/**@} end of group Examples*/
