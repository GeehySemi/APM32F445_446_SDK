/*!
 * @file        apm32f445_446_power.h
 *
 * @brief       This file provides all the POWER firmware functions
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

#ifndef APM32F445_446_POWER_H
#define APM32F445_446_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_clock.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup POWER_Driver POWER Driver
  @{
*/

/** @defgroup POWER_Enumerations Enumerations
  @{
*/

/*!
 * @brief Power modes enumeration.
 */
typedef enum
{
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    POWER_MODE_HSRUN,            /* High speed run mode. */
#endif
    POWER_MODE_RUN,              /* Run mode. */
    POWER_MODE_VLPR,             /* Very low power run mode. */
    POWER_MODE_STOP1,            /* Stop 1 mode. */
    POWER_MODE_STOP2,            /* Stop 2 mode. */
    POWER_MODE_VLPS,             /* Very low power stop mode. */
    POWER_MODE_MAX
} POWER_MODE_T;

/*!
 * @brief Power Modes in PMSTAT
 */
typedef enum
{
    RUN_STATUS     = 0x01U,              /* Current power mode status is RUN */
    STOP_STATUS    = 0x02U,              /* Current power mode status is STOP */
    VLPR_STATUS    = 0x04U,              /* Current power mode status is VLPR */
    VLPW_STATUS    = 0x08U,              /* Current power mode status is VLPW */
    VLPS_STATUS    = 0x10U,              /* Current power mode status is VLPS */
    HSRUN_STATUS   = 0x80U,              /* Current power mode status is HSRUN */
    INVALID_STATUS = 0xFFU               /* Non-existing power mode */
} POWER_MODE_STATUS_T;

/*!
 * @brief Run mode definition
 */
typedef enum
{
    SYSMC_RUN   = 0U,                   /* Normal RUN mode */
    SYSMC_VLPR  = 2U,                   /* Very Low Power RUN mode */
    SYSMC_HSRUN = 3U                    /* High Speed Run mode */
} SYSMC_RUN_MODE_T;

/*!
 * @brief Stop mode definition
 */
typedef enum
{
    SYSMC_STOP            = 0U,         /* Normal STOP mode */
    SYSMC_VLPS            = 2U          /* Very low power STOP mode */
} SYSMC_STOP_MODE_T;

/*!
 * @brief STOP option
 */
typedef enum
{
    SYSMC_STOP1         = 0x01U,         /* Stop with both bus and system clocks disabled */
    SYSMC_STOP2         = 0x02U          /* Stop with bus clock enabled and system clock disabled */
} SYSMC_STOP_OPTION_T;

/*!
 * @brief System Reset Source Name definitions
 */
typedef enum
{
    RMU_BY_LOW_VOLT_DETECT      = 1U,        /* Reset by Low voltage detec */
    RMU_BY_LOSS_OF_CLK          = 2U,        /* Reset by a loss of clock  */
    RMU_BY_LOSS_OF_LOCK         = 3U,        /* Reset by a loss of lock */
#if FEATURE_RMU_WITH_CMU_LOSS_OF_CLOCK
    RMU_BY_CMU_LOC              = 4U,        /* Reset by CMU Loss of lock*/
#endif
    RMU_BY_WATCH_DOG            = 5U,        /* Reset by Watch dog */
    RMU_BY_EXTERNAL_PIN         = 6U,        /* Reset by External pin */
    RMU_BY_POWER_ON             = 7U,        /* Reset by Power on */
    RMU_BY_SJTAG                = 8U,        /* Reset by JTAG generated */
    RMU_BY_CORE_LOCKUP          = 9U,        /* Reset by core lockup */
    RMU_BY_SOFTWARE             = 10U,       /* Reset by Software */
    RMU_BY_SMDM_AP              = 11U,       /* Reset by MDM-AP system */
    RMU_BY_STOP_MODE_ACK_ERR    = 13U,       /* Reset by Stop mode ack error */
    RMU_SRC_NAME_MAX
} RMU_SRC_NAME_T;

/*!
 * @brief Power manager policies
 */
typedef enum
{
    POWER_POLICY_AGREEMENT,       /* Power mode is changed if all of the callbacks return success. */
    POWER_POLICY_FORCIBLE         /* Power mode is changed regardless of the result of callbacks. */
} POWER_POLICY_T;

/*!
 * @brief The PM notification type. Used to notify registered callbacks.
 */
typedef enum
{
    POWER_NOTIFY_RECOVER = 0x00U,  /* Notify IP to recover to previous work state. */
    POWER_NOTIFY_BEFORE  = 0x01U,  /* Notify IP that the system will change the power setting. */
    POWER_NOTIFY_AFTER   = 0x02U   /* Notify IP that the system has changed to a new power setting. */
} POWER_NOTIFY_T;

/*!
 * @brief The callback type indicates when a callback will be invoked.
 */
typedef enum
{
    POWER_CALLBACK_BEFORE       = 0x01U, /* Before callback. */
    POWER_CALLBACK_AFTER        = 0x02U, /* After callback. */
    POWER_CALLBACK_BEFORE_AFTER = 0x03U  /* Before-After callback. */
} POWER_CALLBACK_TYPE_T;

/**@} end of group POWER_Enumerations*/

/** @defgroup POWER_Structures Structures
  @{
*/

/*!
 * @brief Power mode user configuration structure
 */
typedef struct
{
    POWER_MODE_T powerMode;             /* Power mode */
    bool sleepOnExitValue;              /* Sleep on exit option.
                                             true:  ARM core returns to sleep (wait modes) or deep sleep state (stop modes)
                                             false: ARM core stays woken-up. */
} POWER_USER_CONFIG_T;

/*!
 * @brief Power mode protection configuration
 */
typedef struct
{
    bool vlpProtect;                             /* VLP protect*/
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    bool hsrunProtect;                           /* HSRUN protect */
#endif
} SYSMC_POWER_MODE_PROTECTION_CONFIG_T;

/*!
 * @brief Power mode control configuration used for calling the SYSMC_SYS_SetPowerMode API
 */
typedef struct
{
    POWER_MODE_T powerMode;              /* Power mode, value of POWER_MODE_T */
    SYSMC_STOP_OPTION_T stopOption;      /* STOPO option, value of SYSMC_STOP_OPTION_T */
} SYSMC_POWER_MODE_CONFIG_T;

/*!
 * @brief Power notification structure passed to registered callback function
 */
typedef struct
{
    POWER_USER_CONFIG_T *targetPowerCfgPtr;      /* Pointer of target power configuration */
    uint8_t targetPowerCfgIdx;                   /* Index of target power configuration . */
    POWER_POLICY_T policy;                       /* Power mode transition policy */
    POWER_NOTIFY_T type;                         /* Type of Power mode notification */
} POWER_NOTIFY_STRUCT_T;

/*!
 * @brief Callback-specific data
 */
typedef void POWER_CALLBACK_DATA_T;

/*!
 * @brief Callback prototype.
 *
 * @details Declaration of callback. It is common for all registered callbacks.
 * Function pointer of this type is part of POWER_CALLBACK_USER_CONFIG_T callback
 * configuration structure.
 *
 * @param notify Notification structure.
 * @param dataPtr Callback data. Pointer to the data passed during callback registration. Intended to
 *  pass any driver or application data such as internal state information.
 *
 * @retval An error code or STATUS_SUCCESS.
 */
typedef STATUS_T (* POWER_CALLBACK_T)(POWER_NOTIFY_STRUCT_T *notify,
                                      POWER_CALLBACK_DATA_T *dataPtr);

/*!
 * @brief callback configuration structure
 */
typedef struct
{
    POWER_CALLBACK_T function;      /* Pointer of the callback function */
    POWER_CALLBACK_TYPE_T type;     /* Indicates when the callback is called */
    POWER_CALLBACK_DATA_T *data;    /* Pointer of the data passed to the callback */
} POWER_CALLBACK_USER_CONFIG_T;

/*!
 * @brief Power manager internal state structure
 */
typedef struct
{
    POWER_USER_CONFIG_T * (*configs)[];                 /* Pointer of power configure table.*/
    uint8_t configsNumber;                              /* Number of power configurations */
    POWER_CALLBACK_USER_CONFIG_T * (*callbacks)[];      /* Pointer of callback table. */
    uint8_t callbacksMaxNum;                            /* Callback configurations Max number*/
    uint8_t errorCallbackIdx;                           /* Error callback index. */
    uint8_t currentCfgIdex;                             /* Current configuration index */
} POWER_STATE_T;

/**@} end of group POWER_Structures*/

/** @defgroup POWER_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T POWER_SYS_Init(POWER_USER_CONFIG_T *(*powerConfigsPtr)[],
                        uint8_t configsNumber,
                        POWER_CALLBACK_USER_CONFIG_T *(*callbacksPtr)[],
                        uint8_t callbacksNumber);
STATUS_T POWER_SYS_DeInit(void);
void POWER_SYS_DefaultConfig(POWER_USER_CONFIG_T *const config);
STATUS_T POWER_SYS_ConfigPowerMode(uint8_t powerModeIdx, POWER_POLICY_T policy);
POWER_MODE_T POWER_SYS_ReadCurrentRunMode(void);
STATUS_T POWER_SYS_ReadPowerModeIndex(uint8_t *powerModeIdxPtr);
STATUS_T POWER_SYS_ReadPowerModeConfig(POWER_USER_CONFIG_T **powerModePtr);
bool POWER_SYS_ReadResetSrcStatus(const RMU_T *const baseAddr, const RMU_SRC_NAME_T srcName);
uint8_t POWER_SYS_ReadErrorCallbackIndex(void);
POWER_CALLBACK_USER_CONFIG_T *POWER_SYS_ReadErrorCallback(void);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
void POWER_HW_SYSMC_ConfigRunModeControl(SYSMC_T *const baseAddr, const SYSMC_RUN_MODE_T runMode);
void POWER_HW_SYSMC_ConfigStopModeControl(SYSMC_T *const baseAddr, const SYSMC_STOP_MODE_T stopMode);
void POWER_HW_SYSMC_ConfigStopOption(SYSMC_T *const baseAddr, const SYSMC_STOP_OPTION_T option);
STATUS_T POWER_HW_SYSMC_ConfigPowerMode(SYSMC_T *const baseAddr,
                                        const SYSMC_POWER_MODE_CONFIG_T *const powerModeConfig);
void POWER_HW_SYSMC_ConfigProtectionMode(SYSMC_T *const baseAddr,
                                        const SYSMC_POWER_MODE_PROTECTION_CONFIG_T *const protectConfig);
POWER_MODE_STATUS_T POWER_HW_SYSMC_ReadPowerModeStatus(const SYSMC_T *const baseAddr);
bool POWER_HW_SYSMC_WaitForStatChange(const SYSMC_T *const baseAddr,
                                        const POWER_MODE_STATUS_T mode,
                                        const uint32_t timeout);
uint32_t POWER_HW_SCG_ReadVlprDivCore(const SCG_T *const baseAddr);
uint32_t POWER_HW_SCG_ReadVlprDivBus(const SCG_T *const baseAddr);
uint32_t POWER_HW_SCG_ReadVlprDivLow(const SCG_T *const baseAddr);
void POWER_HW_SCG_ConfigSYSPLLEnable(bool enable);
bool POWER_HW_SCG_ReadSYSPLLEnable(void);
bool POWER_HW_SCG_ReadSysPllStatus(const SCG_T *const base);
bool POWER_HW_SCG_ReadHsrunSelectSYSPLL(void);
void POWER_HW_SCG_ConfigHSIEnable(bool enable);
bool POWER_HW_SCG_ReadHSIEnable(void);
bool POWER_HW_SCG_ReadHSIStatus(const SCG_T *const base);
void POWER_HW_SCG_ConfigSYSOSCEnable(bool enable);
bool POWER_HW_SCG_ReadSYSOSCEnable(void);
bool POWER_HW_SCG_ReadSYSOSCStatus(const SCG_T *const base);
void POWER_HW_PMU_EnableBiasen(PMU_T *const baseAddr);
void POWER_HW_PMU_DisableBiasen(PMU_T *const baseAddr);
bool POWER_HW_RMU_ReadResetSrcStatus(const RMU_T *const base, const RMU_SRC_NAME_T srcName);


/**@} end of group POWER_Functions*/
/**@} end of group POWER_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#ifdef __cplusplus
}
#endif

#endif /* APM32F445_446_POWER_H */
