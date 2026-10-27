/*!
 * @file        apm32f445_446_wdt.h
 *
 * @brief       This file contains all the functions prototypes for the WDT firmware library.
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


#ifndef APM32F445_446_WDT_H
#define APM32F445_446_WDT_H

/* Includes */
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup WDT_Driver WDT Driver
  @{
*/

/** @defgroup WDT_Marcos Marcos
  @{
*/

#define WDT_INSTANCE_COUNT 1U
#define WDT_INSTANCE_VALIDITY(__instance__)    if(__instance__ >= WDT_INSTANCE_COUNT){while(1);}

/* The dummy read is used in order to make sure that any write to the
 * WDT registers will be started only after the write of the unlock value was completed.
 */
#define WDT_UNLOCK32(base)  \
{ \
    (base)->CNT.reg = FEATURE_WDT_UNLOCK_VALUE; \
    (void)(base)->CNT.reg; \
}

#define WDT_UNLOCK16(base)  \
{ \
    (base)->CNT.reg = FEATURE_WDT_UNLOCK16_FIRST_VALUE; \
    (void)(base)->CNT.reg; \
    (base)->CNT.reg = FEATURE_WDT_UNLOCK16_SECOND_VALUE; \
    (void)(base)->CNT.reg; \
}

#define WDT_UNLOCK(base)    \
{ \
    if (((base)->CSTS.reg & 0x2000U) != 0U) \
    { \
        WDT_UNLOCK32(base); \
    } \
    else \
    { \
        WDT_UNLOCK16(base); \
    } \
}

/**@} end of group WDT_Marcos*/

/** @defgroup WDT_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/
/*!
 * @brief Clock sources for the WDT
 */
typedef enum
{
    WDT_BUS_CLOCK                        = 0x00U, /* Bus clock */
    WDT_LPO_CLOCK                        = 0x01U, /* LPO clock */
    WDT_SYSOSC_CLOCK                       = 0x02U, /* SYSOSC clock */
    WDT_LSICLK_CLOCK                     = 0x03U  /* LSICLK clock */
} WDT_CLK_SRC_T;

/*!
 * @brief Test modes for the WDT
 */
typedef enum
{
    WDT_TEST_DISABLED                     = 0x00U, /* Test mode disabled */
    WDT_TEST_USER                         = 0x01U, /* User mode enabled. (Test mode disabled.) */
    WDT_TEST_LOW                          = 0x02U, /* Test mode enabled, only the low byte is used. */
    WDT_TEST_HIGH                         = 0x03U  /* Test mode enabled, only the high byte is used. */
} WDT_TEST_MODE_T;

/*!
 * @brief set modes for the WDT
 */
typedef enum
{
    WDT_MODE_DEBUG                        = 0x00U, /* Debug mode */
    WDT_MODE_WAIT                         = 0x01U, /* Wait mode */
    WDT_MODE_STOP                         = 0x02U  /* Stop mode */
} WDT_MODE_T;


/**@} end of group WDT_Enumerations*/

/** @defgroup WDT_Structures Structures
  @{
*/
/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/
/*!
 * @brief WDT option mode configuration structure
 */
typedef struct
{
    bool wait;  /* Wait mode */
    bool stop;  /* Stop mode */
    bool debug; /* Debug mode */
} WDT_OP_MODE_T;

/*!
 * @brief WDT user configuration structure
 * Implements : Wdt_Config_T_Class
 */
typedef struct
{
    WDT_CLK_SRC_T        clkSrc;          /* The clock source of the WDT */
    WDT_OP_MODE_T        opMode;          /* The modes in which the WDT is functional */
    bool                 updateEnable;    /* If true, further updates of the WDT are enabled */
    bool                 intEnable;       /* If true, an interrupt request is generated before reset */
    bool                 windowEnable;    /* If true, window mode is enabled */
    uint16_t             windowValue;     /* The window value */
    uint16_t             timeoutValue;    /* The timeout value */
    bool                 prescalerEnable; /* If true, a fixed 256 prescaling of the counter reference clock is enabled */
} WDT_CONFIG_T;

/**@} end of group WDT_Structures*/

/** @defgroup WDT_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T WDT_Init(uint32_t ins, const WDT_CONFIG_T * wdtcfg);
STATUS_T WDT_DeInit(uint32_t ins);
void WDT_ReadConfig(uint32_t ins, WDT_CONFIG_T * const wdtCfg);
void WDT_DefaultConfig(WDT_CONFIG_T * const cfg);
STATUS_T WDT_EnableInterrupt(uint32_t ins);
STATUS_T WDT_DisableInterrupt(uint32_t ins);
void WDT_ClearIntFlag(uint32_t ins);
void WDT_Refresh(uint32_t ins);
uint16_t WDT_ReadCounter(uint32_t ins);
STATUS_T WDT_ConfigWindow(uint32_t ins, bool enable, uint16_t winvalue);
STATUS_T WDT_ConfigMode(uint32_t ins, bool enable, WDT_MODE_T cfgmode);
STATUS_T WDT_ConfigTimeout(uint32_t ins, uint16_t Timeout);
STATUS_T WDT_ConfigTestMode(uint32_t ins, WDT_TEST_MODE_T TestModel);
WDT_TEST_MODE_T WDT_ReadTestMode(uint32_t ins);


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
bool WDT_HW_IsEnabled(const WDT_T *base);
void WDT_HW_Refresh(WDT_T * const base);
void WDT_HW_EnableWindowMode(WDT_T * const base);
void WDT_HW_DisableWindowMode(WDT_T * const base);
void WDT_HW_ConfigWindowValue(WDT_T * const base, uint16_t window);
void WDT_HW_ClearIntFlag(WDT_T * const base);
WDT_CLK_SRC_T WDT_HW_ReadClockSource(const WDT_T *base);
bool WDT_HW_IsUpdateEnabled(const WDT_T *base);
void WDT_HW_EnableDebug(WDT_T * const base);
void WDT_HW_DisableDebug(WDT_T * const base);
void WDT_HW_EnableWait(WDT_T * const base);
void WDT_HW_DisableWait(WDT_T * const base);
void WDT_HW_EnableStop(WDT_T * const base);
void WDT_HW_DisableStop(WDT_T * const base);
bool WDT_HW_IsUnlocked(const WDT_T *base);
bool WDT_HW_IsReconfigurationComplete(const WDT_T *base);
uint32_t WDT_HW_ReadTestMode(const WDT_T *base);
void WDT_HW_Reset(WDT_T * const base);
void WDT_HW_EnableInterrupt(WDT_T * const base);
void WDT_HW_DisableInterrupt(WDT_T * const base);
STATUS_T WDT_HW_Config(WDT_T * const base, const WDT_CONFIG_T *wdtcfg);
WDT_CONFIG_T WDT_HW_ReadConfig(const WDT_T *base);


/**@} end of group WDT_Functions*/
/**@} end of group WDT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_WDT_H */
