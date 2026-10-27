/*!
 * @file        osif.h
 *
 * @brief       This file provides all the OSIF functions
 *
 * @version     V1.0.0
 *
 * @date        2023-08-30
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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

#ifndef OSIF_H
#define OSIF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>
#include "device.h"

/** @addtogroup RTOS
  @{
*/

/** @addtogroup OSIF
  @{
*/

/** @defgroup OSIF_Macros Macros
  @{
*/
#define OSIF_WAIT_FOREVER 0xFFFFFFFFU

/**@} end of group OSIF_Macros*/

/** @defgroup OSIF_Structures Structures
  @{
*/

typedef uint8_t MUTEX_T;
typedef volatile uint8_t SEMAPHORE_T;

/**@} end of group OSIF_Structures*/

/** @defgroup OSIF_Functions Functions
  @{
*/
void OSIF_TimeDelay(const uint32_t delay);
void OSIF_Tick(void);
uint32_t OSIF_GetMilliseconds(void);

STATUS_T OSIF_SemCreate(SEMAPHORE_T * const semPtr, const uint8_t initValue);
STATUS_T OSIF_SemDestroy(const SEMAPHORE_T * const semPtr);
STATUS_T OSIF_SemWait(SEMAPHORE_T * const semPtr, const uint32_t timeout);
STATUS_T OSIF_SemPost(SEMAPHORE_T * const semPtr);

STATUS_T OSIF_MutexCreate(MUTEX_T * const mutexPtr);
STATUS_T OSIF_MutexDestroy(const MUTEX_T * const mutexPtr);
STATUS_T OSIF_MutexLock(const MUTEX_T * const mutexPtr, const uint32_t timeout);
STATUS_T OSIF_MutexUnlock(const MUTEX_T * const mutexPtr);

/**@} end of group OSIF_Functions*/
/**@} end of group OSIF*/
/**@} end of group RTOS*/

#ifdef __cplusplus
}
#endif

#endif /* OSIF_H */
