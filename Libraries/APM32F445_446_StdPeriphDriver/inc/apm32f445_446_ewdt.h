/*!
 * @file        apm32f445_446_ewdt.h
 *
 * @brief       This file provides all the EWDT firmware functions
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

#ifndef APM32F445_446_EWDT_H
#define APM32F445_446_EWDT_H

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup EWDT_Driver EWDT Driver
  @{
*/

/** @defgroup EWDT_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define EWDT_INSTANCE_COUNT 1U
#define EWDT_INSTANCE_VALIDITY(__instance__)    if(__instance__ >= EWDT_INSTANCE_COUNT){while(1);}

/**@} end of group EWDT_Macros*/

/** @defgroup EWDT_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief ewdt in assert logic.
 */
typedef enum
{
    EWDT_IN_ASSERT_DISABLED      = 0x00U,    /* Input pin disabled */
    EWDT_IN_ASSERT_ON_LOGIC_ZERO = 0x01U,    /* Input pin asserts EWDT when on logic 0 */
    EWDT_IN_ASSERT_ON_LOGIC_ONE  = 0x02U     /* Input pin asserts EWDT when on logic 1 */
} EWDT_IN_ASSERT_LOGIC_T;

/**@} end of group EWDT_Enumerations*/

/** @defgroup EWDT_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief ewdt init config.
 */
typedef struct
{
    EWDT_IN_ASSERT_LOGIC_T assertLogic; /* Assert logic for EWDT input pin */
    bool       interruptEnable;         /* Enable EWDT interrupt           */
    uint8_t    prescaler;               /* EWDT clock prescaler            */
    uint8_t    compareLow;              /* Compare low value               */
    uint8_t    compareHigh;             /* Compare high value              */
} EWDT_INIT_CONFIG_T;

/**@} end of group EWDT_Structures*/

/** @defgroup EWDT_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T EWDT_Init(uint32_t ins, const EWDT_INIT_CONFIG_T *cfg);
void EWDT_DefaultConfig(EWDT_INIT_CONFIG_T *cfg);

void EWDT_Reload(uint32_t ins);

EWDT_IN_ASSERT_LOGIC_T EWDT_ReadInputPinAssertLogic(uint32_t ins);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void EWDT_HW_Reload(EWDT_T *const ewdtBase);

bool EWDT_HW_IsEnabled(const EWDT_T *const ewdtBase);

void EWDT_HW_ConfigControl(EWDT_T *const ewdtBase, uint8_t value);
uint8_t EWDT_HW_ReadControl(const EWDT_T *const ewdtBase);

void EWDT_HW_ConfigCompareLow(EWDT_T *const ewdtBase, uint8_t value);
void EWDT_HW_ConfigCompareHigh(EWDT_T *const ewdtBase, uint8_t value);
void EWDT_HW_ConfigPrescaler(EWDT_T *const ewdtBase, uint8_t value);

/**@} end of group EWDT_Functions*/
/**@} end of group EWDT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_EWDT_H */
