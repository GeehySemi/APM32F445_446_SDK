/*!
 * @file        apm32f445_446_erep.h
 *
 * @brief       This file provides all the EREP firmware functions
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

#ifndef APM32F445_446_EREP_H
#define APM32F445_446_EREP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup EREP_Driver EREP Driver
  @{
*/

/** @defgroup EREP_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define EREP_INSTANCE_COUNT     (1U)

#define EREP_INSTANCE_VALIDITY(__ins__) if(__ins__ >= EREP_INSTANCE_COUNT){while(1);}

/**@} end of group EREP_Macros*/

/** @defgroup EREP_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

typedef enum
{
    EREP_EVENT_NONE              = 0U,   /* None events */
    EREP_EVENT_SINGLE_BIT        = 1U,   /* Single-bit correction ECC events */
    EREP_EVENT_NON_CORRECTABLE   = 2U    /* Non-correctable ECC events */
} EREP_ECC_ENEVT_T;

/**@} end of group EREP_Enumerations*/

/** @defgroup EREP_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief EREP interrupt notification configuration.
 */
typedef struct
{
    bool enableSingleCorrection;    /* Enable Single Correction Interrupt Notification */
    bool enableNonCorrectable;      /* Enable Non-Correctable Interrupt Notification */
} EREP_INTERRUPT_CONFIG_T;

/*!
 * @brief EREP user configuration.
 */
typedef struct
{
    uint8_t channel;                                /* The channel assignments */
    const EREP_INTERRUPT_CONFIG_T * interruptCfg;   /* Interrupt configuration */
} EREP_USER_CONFIG_T;

/**@} end of group EREP_Structures*/

/** @defgroup EREP_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void EREP_Init(uint32_t ins, uint8_t channelCnt, const EREP_USER_CONFIG_T *userConfigArray);
void EREP_DeInit(uint32_t ins);

void EREP_ConfigInterruptCfg(uint32_t ins, uint8_t channel, EREP_INTERRUPT_CONFIG_T interruptCfg);
void EREP_ReadInterruptCfg(uint32_t ins, uint8_t channel, EREP_INTERRUPT_CONFIG_T *const interruptPtr);

void EREP_ClearEvent(uint32_t ins, uint8_t channel, EREP_ECC_ENEVT_T eccEvent);

EREP_ECC_ENEVT_T EREP_ReadErrorDetail(uint32_t ins, uint8_t channel, uint32_t *addressPtr);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void EREP_HW_StartEventInterrupt(EREP_T *const erepBase, uint8_t channel, EREP_ECC_ENEVT_T eccEvent, bool enable);
uint8_t EREP_HW_EventInterruptVerify(const EREP_T *const erepBase, uint8_t channel, EREP_ECC_ENEVT_T eccEvent);

bool EREP_HW_EventIdentified(const EREP_T *const erepBase, uint8_t channel, EREP_ECC_ENEVT_T eccEvent);

void EREP_HW_ClearEventSingle(EREP_T *const erepBase, uint8_t channel);
void EREP_HW_ClearEventDouble(EREP_T *const erepBase, uint8_t channel);

uint32_t EREP_HW_ReadLastErrorAddress(const EREP_T *const erepBase, uint8_t channel);

void EREP_HW_Setup(EREP_T *const erepBase);

/**@} end of group EREP_Functions*/
/**@} end of group EREP_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#ifdef __cplusplus
}
#endif

#endif /* APM32F445_446_EREP_H */
