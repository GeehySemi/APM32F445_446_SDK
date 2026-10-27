/*!
 * @file        user_cfgio_uart_config.h
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

#ifndef USER_CFGIO_UART_CONFIG_H
#define USER_CFGIO_UART_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f445_446_cfgio_uart.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CFGIO_UART_Echo
  @{
*/

/** @defgroup CFGIO_UART_Echo_Macros Macros
  @{
*/

/* UART instance */
#define CFGIO_UART_INSTANCE   (0U)

/**@} end of group CFGIO_UART_Echo_Macros*/
/**@} end of group CFGIO_UART_Echo*/
/**@} end of group Examples*/

/* UART configurations */
extern CFGIO_UART_USER_CONFIG_T g_uartRxConfig;
extern CFGIO_UART_USER_CONFIG_T g_uartTxConfig;

/* Declaration of callback functions */
extern void CFGIOUartRxCallback(void *driverState, UART_EVENT_T event, void *userData);
extern void CFGIOUartTxCallback(void *driverState, UART_EVENT_T event, void *userData);

#ifdef __cplusplus
}
#endif

#endif /* USER_CFGIO_UART_CONFIG_H */
