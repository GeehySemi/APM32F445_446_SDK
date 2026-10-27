/*!
 * @file        apm32f445_446_interrupt.h
 *
 * @brief       This file contains all the functions prototypes for the INTERRUPT firmware library.
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

#ifndef APM32F445_446_INTERRUPT_H
#define APM32F445_446_INTERRUPT_H

/* Includes */
#include "apm_core_cm4.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup INTERRUPT_Driver INTERRUPT Driver
  @{
*/

/** @defgroup INTERRUPT_Functions Functions
  @{
*/

typedef void (*ISR_T)(void);

/*! @brief Default ISR. */
void DefaultISR(void);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void INT_SYS_InstallHandler(IRQn_Type irqNum, const ISR_T newHandler, ISR_T* const oldHandler);

void INT_SYS_EnableIRQ(IRQn_Type irqNum);
void INT_SYS_DisableIRQ(IRQn_Type irqNum);
void INT_SYS_EnableIRQGlobal(void);
void INT_SYS_DisableIRQGlobal(void);

void INT_SYS_ConfigPriority(IRQn_Type irqNum, uint8_t priority);
uint8_t INT_SYS_ReadPriority(IRQn_Type irqNum);
void INT_SYS_ConfigPending(IRQn_Type irqNum);
void INT_SYS_ClearPending(IRQn_Type irqNum);
uint32_t INT_SYS_ReadPending(IRQn_Type irqNum);

#if FEATURE_INTERRUPT_WITH_ACTIVE_STATE
uint32_t INT_SYS_ReadActive(IRQn_Type irqNum);
#endif /* FEATURE_INTERRUPT_WITH_ACTIVE_STATE */

/**@} end of group INTERRUPT_Functions*/
/**@} end of group INTERRUPT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif /* APM32F445_446_INTERRUPT_H */
