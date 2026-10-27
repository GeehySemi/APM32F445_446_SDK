/*!
 * @file        apm32f445_446_tmc.h
 *
 * @brief       This file contains all the functions prototypes for the TMC firmware library.
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

#ifndef APM32F445_446_TMC_H
#define APM32F445_446_TMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup TMC_Driver TMC_Driver
  @{
*/

/** @defgroup TMC_Macros Macros
  @{
*/

/* TMC index offsets */
#define TMC_DMACHM0_INDEX           0U
#define TMC_EXTOUT0_INDEX           1U
#define TMC_EXTOUT1_INDEX           2U
#define TMC_ADC0_INDEX              3U
#define TMC_ADC1_INDEX              4U
#define TMC_COMP0_INDEX             7U
#define TMC_CFGTMR0_INDEX             10U
#define TMC_CFGTMR1_INDEX             11U
#define TMC_CFGTMR2_INDEX             12U
#define TMC_CFGTMR3_INDEX             13U
#define TMC_PDU0_INDEX              14U
#define TMC_PDU1_INDEX              15U
#define TMC_FLECCOM_INDEX           17U
#define TMC_LPITMR0_INDEX           18U
#define TMC_LPUART0_INDEX           19U
#define TMC_LPUART1_INDEX           20U
#define TMC_LPI2C_INDEX             21U
#define TMC_LPSPI0_INDEX            23U
#define TMC_LPSPI1_INDEX            24U
#define TMC_LPTMR0_INDEX            25U
#define TMC_CFGTMR4_INDEX             28U
#define TMC_CFGTMR5_INDEX             29U

/**@} end of group TMC_Macros*/

/** @defgroup TMC_Enumerations Enumerations
  @{
*/

/*!
 * @brief Enumeration for trigger source module of TMC
 */
typedef enum TMC_TRIGGER_E TMC_TRIGGER_T;

/*!
 * @brief Enumeration for target module of TMC
 */
typedef enum TMC_TARGET_E TMC_TARGET_T;

/**@} end of group TMC_Enumerations*/

/** @defgroup TMC_Structures Structures
  @{
*/

/*!
 * @brief Configuration structure for pairing source triggers with target modules.
 */
typedef struct
{
    TMC_TRIGGER_T triggeSrc; /* selects one of the TMC trigger sources */
    TMC_TARGET_T targetModule;   /* selects one of the TMC target modules  */
    bool lockTargetModuleReg;           /* if true, the LOCK bit of the target module register will be
                                           set by TMC_Init(), after the current mapping is configured */
} TMC_INOUT_MAPPING_CONFIG_T;

/*!
 * @brief User configuration structure for the TMC driver.
 */
typedef struct
{
    uint8_t inOutMapCfgsCount;                        /* number of in-out mappings defined in TMC configuration */
    const TMC_INOUT_MAPPING_CONFIG_T * inOutMapCfg; /* pointer to array of in-out mapping structures */
} TMC_USER_CONFIG_T;

/**@} end of group TMC_Structures*/

/** @defgroup TMC_Variables Variables
  @{
*/

/*! @brief Table of base addresses for TMC instances. */
extern TMC_T * const g_tmcBaseAddress[TMC_INSTANCE_COUNT];

/**@} end of group TMC_Variables*/

/** @defgroup TMC_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T TMC_Init(const uint32_t instance, const TMC_USER_CONFIG_T *const tmcUserConfig);
STATUS_T TMC_DeInit(const uint32_t instance);
STATUS_T TMC_ConfigTrigSourceForTargetModule(const uint32_t instance,
                                                const TMC_TRIGGER_T triggeSrc,
                                                const TMC_TARGET_T targetModule);
TMC_TRIGGER_T TMC_ReadTrigSourceForTargetModule(const uint32_t instance,
                                                        const TMC_TARGET_T targetModule);
void TMC_ConfigLockForTargetModule(const uint32_t instance, const TMC_TARGET_T targetModule);
bool TMC_ReadLockForTargetModule(const uint32_t instance, const TMC_TARGET_T targetModule);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
STATUS_T TMC_HW_Config(TMC_T *const base);
void TMC_HW_ConfigTrigSourceForTargetModule(TMC_T * const base,
                                             const TMC_TRIGGER_T triggeSrc,
                                             const TMC_TARGET_T targetModule);
TMC_TRIGGER_T TMC_HW_ReadTrigSourceForTargetModule(const TMC_T *const base, const TMC_TARGET_T targetModule);
void TMC_HW_ConfigLockForTargetModule(TMC_T *const base, const TMC_TARGET_T targetModule);
bool TMC_HW_ReadLockForTargetModule(const TMC_T *const base, const TMC_TARGET_T targetModule);
void TMC_HW_GenerateSWTrigger(void);

/**@} end of group TMC_Functions*/
/**@} end of group TMC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_TMC_H */
