/*!
 * @file        apm32f445_446_power.c
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

/* Includes */
#include "apm32f445_446_power.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup POWER_Driver POWER Driver
  @{
*/

/** @defgroup POWER_Macros Macros
  @{
*/

/* Timeout used for waiting to set new mode */
#define SYSMC_TIMEOUT           1000U

/**@} end of group POWER_Macros*/

/** @defgroup POWER_Variables Variables
  @{
*/

/* Power manager internal structure. */
static POWER_STATE_T g_powerManagerState;
/* Save system clock configuration */
static SYS_CLK_CONFIG_T g_sysClkCfg;
static bool g_changeClkVlp = false;
#if FEATURE_WITH_SYSPLL_CLK
static bool g_enableSYSPLL = false;
#endif
static bool g_enableHSI = false;
static bool g_enableSYSOSC = false;

/**@} end of group POWER_Variables*/

/** @defgroup POWER_Functions Functions
  @{
*/
/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static STATUS_T POWER_DoInit(void);
static STATUS_T POWER_DoDeInit(void);
static STATUS_T POWER_UpdateInitClk(const SYS_CLK_CONFIG_T *const sysClk);
static STATUS_T POWER_EnableVlpClockSrc(void);
static STATUS_T POWER_DisableVlpClockSrc(void);
static STATUS_T POWER_SwitchToVlprClk(const SYS_CLK_CONFIG_T *const sysClk);
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
static STATUS_T POWER_EnterHsrunMode(void);
#endif
static STATUS_T POWER_SwitchToRunningPowerMode(const POWER_USER_CONFIG_T *const configPtr);
static STATUS_T POWER_SwitchToSleepingPowerMode(const POWER_USER_CONFIG_T *const configPtr);
static STATUS_T POWER_DoConfigMode(const POWER_USER_CONFIG_T *const configPtr);
static void POWER_ReadEnableClockSrc(void);
static STATUS_T POWER_CallbacksManagement(POWER_NOTIFY_STRUCT_T *notifyStruct,
                                       uint8_t *currentStaticCallback,
                                       POWER_POLICY_T policy);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Power manager initialization for operation.
 *
 * @param powerCfgsPtr A pointer to an array of pointers to all power
 *  configurations which will be handled by Power manager.
 * @param cfgsNum  Number of power configurations. Size of powerCfgsPtr array.
 * @param callbacksPtr A pointer to an array of pointers to callback configurations.
 *  If there are no callbacks to register during Power manager initialization, use NULL value.
 * @param callbacksNum Number of registered callbacks. Size of callbacksPtr array.

 * @retval An error code or STATUS_SUCCESS.
 */
STATUS_T POWER_SYS_Init(POWER_USER_CONFIG_T *(*powerCfgsPtr)[],
                        uint8_t cfgsNum,
                        POWER_CALLBACK_USER_CONFIG_T *(*callbacksPtr)[],
                        uint8_t callbacksNum)
{
    /* Store references to user-defined power mode configurations */
    g_powerManagerState.configsNumber = cfgsNum;
    g_powerManagerState.currentCfgIdex = 0U;
    g_powerManagerState.configs = (POWER_USER_CONFIG_T * (*)[])powerCfgsPtr;

    /* Store references to user-defined callback configurations and increment call-back handle counter */
    if (POINTER_IS_NULL(callbacksPtr))
    {
        g_powerManagerState.callbacks = NULL;
        g_powerManagerState.callbacksMaxNum = 0U;
        g_powerManagerState.errorCallbackIdx = 0U;
    }
    else
    {
        g_powerManagerState.callbacks = (POWER_CALLBACK_USER_CONFIG_T * (*)[])callbacksPtr;
        g_powerManagerState.callbacksMaxNum = callbacksNum;
        /* Default value of handle of last call-back that returned error */
        g_powerManagerState.errorCallbackIdx = callbacksNum;
    }

    POWER_DoInit();

    return STATUS_SUCCESS;
}

/*!
 * @brief This function deinitializes the Power manager.
 *
 * @param None.
 *
 * @retval An error code or STATUS_SUCCESS.
 */
STATUS_T POWER_SYS_DeInit(void)
{
    g_powerManagerState.configsNumber = 0U;
    g_powerManagerState.configs = NULL;
    g_powerManagerState.callbacksMaxNum = 0U;
    g_powerManagerState.callbacks = NULL;

    POWER_DoDeInit();
    return STATUS_SUCCESS;
}

/*!
 * @brief This function returns the default power manager configuration structure.
 *
 * @param cfg POWER_USER_CONFIG_T pointer.
 *
 * @retval None
 */
void POWER_SYS_DefaultConfig(POWER_USER_CONFIG_T *const cfg)
{
    cfg->powerMode = POWER_MODE_RUN;
    cfg->sleepOnExitValue = false;
}

/*!
 * @brief This function configures the power mode.
 *
 * @param powerModeIdx Requested power mode represented as an index into
 *        array of user-defined power mode configurations passed to the POWER_SYS_Init().
 * @param policy Transaction policy
 *
 * @retval An error code or STATUS_SUCCESS.
 */
STATUS_T POWER_SYS_ConfigPowerMode(uint8_t powerModeIdx, POWER_POLICY_T policy)
{
    POWER_USER_CONFIG_T *userConfigPtr;
    STATUS_T returnValue;
    STATUS_T errorCode;
    bool SwitchOK;
    uint8_t currentCallback = 0U;
    POWER_NOTIFY_STRUCT_T notify;

    userConfigPtr = (*g_powerManagerState.configs)[powerModeIdx];

    g_powerManagerState.errorCallbackIdx = g_powerManagerState.callbacksMaxNum;

    notify.targetPowerCfgPtr = userConfigPtr;
    notify.targetPowerCfgIdx = powerModeIdx;
    notify.policy = policy;
    notify.type = POWER_NOTIFY_BEFORE;

    returnValue = POWER_CallbacksManagement(&notify, &currentCallback, policy);

    /* Power mode switch */
    /* In case that any call-back returned error code and  policy doesn't force the mode switch go to after switch call-backs */
    if ((policy != POWER_POLICY_FORCIBLE) && (returnValue != STATUS_SUCCESS))
    {
        /* Unsuccessful switch */
        SwitchOK = false;
    }
    else
    {
        returnValue = POWER_DoConfigMode(userConfigPtr);
        SwitchOK = (STATUS_SUCCESS == returnValue) ? true : false;
    }

    if (SwitchOK == false)
    {
        /* End of unsuccessful switch */
        /* Notify those which have been called before the power mode change */
        notify.type = POWER_NOTIFY_RECOVER;
        errorCode = POWER_CallbacksManagement(&notify, &currentCallback, POWER_POLICY_FORCIBLE);
        (void)(errorCode);
    }
    else
    {
        /* End of successful switch */
        /* Update current configuration index */
        g_powerManagerState.currentCfgIdex = powerModeIdx;

        /* Notify those which asked to be called after the power mode change */
        notify.type = POWER_NOTIFY_AFTER;
        returnValue = POWER_CallbacksManagement(&notify, &currentCallback, POWER_POLICY_FORCIBLE);
    }

    return returnValue;
}

/*!
 * @brief This function returns currently running power mode.
 *
 * @param None.
 *
 * @retval Currently used run power mode.
 */
POWER_MODE_T POWER_SYS_ReadCurrentRunMode(void)
{
    POWER_MODE_T returnValue;

    if (POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC) == RUN_STATUS)
    {
        /* Run mode */
        returnValue = POWER_MODE_RUN;
    }
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    else if (POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC) == HSRUN_STATUS)
    {
        /* High speed run mode */
        returnValue = POWER_MODE_HSRUN;
    }
#endif /*FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE*/
    else if (POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC) == VLPR_STATUS)
    {
        /* Very low power run mode */
        returnValue = POWER_MODE_VLPR;
    }
    else
    {
        returnValue = POWER_MODE_MAX;
    }

    return returnValue;
}


/*!
 * @brief This function returns index of power mode which was last set successfully.
 *
 * @details This function returns index of power mode which was last set using POWER_SYS_ConfigPowerMode().
 * If the power mode was entered even though some of the registered callbacks denied the mode change,
 * or if any of the callbacks invoked after the entering/restoring run mode failed, then the return
 * code of this function has STATUS_ERROR value.
 *
 * @param powerModeIdxPtr: Power mode which has been set represented as an index into array of power mode
 * configurations passed to the POWER_SYS_Init().
 *
 * @retval An error code or STATUS_SUCCESS.
 */
STATUS_T POWER_SYS_ReadPowerModeIndex(uint8_t *powerModeIdxPtr)
{
    STATUS_T returnValue;

    /* Pass index of user-defined configuration structure of currently running power mode */
    *powerModeIdxPtr = g_powerManagerState.currentCfgIdex;

    /* Return whether all call-backs executed without error */
    if (g_powerManagerState.errorCallbackIdx != g_powerManagerState.callbacksMaxNum)
    {
        returnValue = STATUS_ERROR;
    }
    else
    {
        returnValue = STATUS_SUCCESS;
    }

    return returnValue;
}

/*!
 * @brief This function returns the user configuration structure of the last successfully set power mode.
 *
 * @details This function returns a pointer to configuration structure which was last set using POWER_SYS_ConfigPowerMode().
 * If the current power mode was entered even though some of the registered callbacks denied
 * the mode change, or if any of the callbacks invoked after the entering/restoring run mode failed, then
 * the return code of this function has STATUS_ERROR value.
 *
 * @param powerModePtr Pointer to power mode configuration structure of the last set power mode.
 *
 * @retval An error code or STATUS_SUCCESS.
 */
STATUS_T POWER_SYS_ReadPowerModeConfig(POWER_USER_CONFIG_T **powerModePtr)
{
    STATUS_T returnValue;

    /* Pass reference to user-defined configuration structure of currently running power mode */
    *powerModePtr = (*g_powerManagerState.configs)[g_powerManagerState.currentCfgIdex];

    /* Return whether all call-backs executed without error */
    if (g_powerManagerState.errorCallbackIdx != g_powerManagerState.callbacksMaxNum)
    {
        returnValue = STATUS_ERROR;
    }
    else
    {
        returnValue = STATUS_SUCCESS;
    }

    return returnValue;
}

/*!
 * @brief This function will read the current reset source status for specified source.
 *
 * @param baseAddr rmu base pointer
 * @param srcName system reset source
 *
 * @retval  system reset source status
 *        - true : system reset source is reset
 *        - false : system reset source is not reset
 */
bool POWER_SYS_ReadResetSrcStatus(const RMU_T *const baseAddr, const RMU_SRC_NAME_T srcName)
{
    return POWER_HW_RMU_ReadResetSrcStatus(baseAddr, srcName);
}

/*!
 * @brief This function returns the last failed notification callback.
 *
 * @details This function returns the index of the last callback that failed during the power mode switch when
 * POWER_SYS_ConfigPowerMode() was called. The returned value represents the index in the array of registered callbacks.
 * If the last POWER_SYS_ConfigPowerMode() call ended successfully, a value equal to the number of registered callbacks
 * is returned.
 *
 * @param None.
 *
 * @retval Callback index of last failed callback or value equal to callbacks count.
 */
uint8_t POWER_SYS_ReadErrorCallbackIndex(void)
{
    return g_powerManagerState.errorCallbackIdx;
}

/*!
 * @brief This function returns the callback configuration structure for the last failed notification.
 *
 * This function returns a pointer to configuration structure of the last callback that failed during
 * the power mode switch when POWER_SYS_ConfigPowerMode() was called.
 * If the last POWER_SYS_ConfigPowerMode() call ended successfully, a NULL value is returned.
 *
 * @param None.
 *
 * @retval Pointer to the callback configuration which returns error.
 */
POWER_CALLBACK_USER_CONFIG_T *POWER_SYS_ReadErrorCallback(void)
{
    return (g_powerManagerState.errorCallbackIdx < g_powerManagerState.callbacksMaxNum) ? \
        (*g_powerManagerState.callbacks)[g_powerManagerState.errorCallbackIdx] : NULL;
}

/*******************************************************************************
                        PRIVATE FUNCTIONS
*******************************************************************************/
/*!
 * @brief This function implementation-specific configuration of power modes.
 *
 * @details This function performs the actual implementation-specific
 * initialization based on the provided power mode configurations.
 * In addition, This function get all clock source were enabled.
 * This one was used for update init clock when CPU jump from
 * very low power mode to run or high speed run mode.
 *
 * @retval Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_ERROR: Operation failed.
 */
static STATUS_T POWER_DoInit(void)
{
    uint8_t i = 0U;

    SYSMC_POWER_MODE_PROTECTION_CONFIG_T powerModeProCfg;
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    powerModeProCfg.hsrunProtect = false;
#endif /* #if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE */
    powerModeProCfg.vlpProtect = false;

    for (i = 0U; i < g_powerManagerState.configsNumber; i++)
    {
        const POWER_USER_CONFIG_T * const config = (*g_powerManagerState.configs)[i];
        if ((config->powerMode == POWER_MODE_VLPR) || (config->powerMode == POWER_MODE_VLPS))
        {
            powerModeProCfg.vlpProtect    =   true;  /* Very low power mode is allowed. */
        }
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
        if (config->powerMode == POWER_MODE_HSRUN)
        {
            powerModeProCfg.hsrunProtect  =   true;  /* High speed mode is allowed. */
        }
#endif /* #if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE */
    }

    /* Biasing disabled, core logic can run in full performance */
    POWER_HW_PMU_DisableBiasen(PMU);

    /* Very low power modes and high speed mode are not protected. */
    POWER_HW_SYSMC_ConfigProtectionMode(SYSMC, &powerModeProCfg);
    /* Get all clock source were enabled. This one was used for update initialize clock when CPU
    came back RUN mode from very low power mode */
    POWER_ReadEnableClockSrc();
    return STATUS_SUCCESS;
}

/*!
 * @brief This function implementation-specific de-initialization of power manager.
 *
 * This function performs the actual implementation-specific de-initialization.
 *
 * @retval Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_ERROR: Operation failed.
 */
static STATUS_T POWER_DoDeInit(void)
{

    /* Biasing disabled, core logic can run in full performance */
    POWER_HW_PMU_DisableBiasen(PMU);

    return STATUS_SUCCESS;
}

/*!
 * @brief This function will update initialization or default clock source of run mode
            when MCU come back run mode.
 *
 * @param sysClk: SYS_CLK_CONFIG_T pointer.
 *
 * @retval Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - Others status code: Operation failed.
 */
static STATUS_T POWER_UpdateInitClk(const SYS_CLK_CONFIG_T *const sysClk)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    returnValue = CLOCK_SYS_ConfigSystemClock(NULL, sysClk);

    return returnValue;
}


/*!
 * @brief This function will enable SYSPLL, HSI, SYSOSC.
 *
 * @param None.
 *
 * @retval Operation status.
 *        - STATUS_SUCCESS: Operation was successful.
 *        - Others status code: Operation failed.
 */
static STATUS_T POWER_EnableVlpClockSrc(void)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t timeout = 0U;

    /* Config SYSOSC enable */
    timeout = SYSOSC_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigSYSOSCEnable(g_enableSYSOSC);
    while ((POWER_HW_SCG_ReadSYSOSCStatus(SCG) != g_enableSYSOSC) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }

    /* Config HSI enable */
    timeout = HSICLK_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigHSIEnable(g_enableHSI);
    while ((POWER_HW_SCG_ReadHSIStatus(SCG) != g_enableHSI) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }

#if FEATURE_WITH_SYSPLL_CLK
    /* Config SYSPLL enable */
    timeout = SYSPLL_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigSYSPLLEnable(g_enableSYSPLL);
    while ((POWER_HW_SCG_ReadSysPllStatus(SCG) != g_enableSYSPLL) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }
#endif /* #if FEATURE_WITH_SYSPLL_CLK */

    return returnValue;
}

/*!
 * @brief This function will disable SYSPLL, HSI, SYSOSC.
 *
 * @param None.
 *
 * @retval Operation status.
 *        - STATUS_SUCCESS: Operation was successful.
 *        - Others status code: Operation failed.
 */
static STATUS_T POWER_DisableVlpClockSrc(void)
{
    STATUS_T returnValue= STATUS_SUCCESS;
    uint32_t timeout;

    /* Config SYSOSC disable */
    timeout = SYSOSC_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigSYSOSCEnable(false);
    while ((POWER_HW_SCG_ReadSYSOSCStatus(SCG) != false) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }

    /* Config HSI disable */
    timeout = HSICLK_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigHSIEnable(false);
    while ((POWER_HW_SCG_ReadHSIStatus(SCG) != false) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }


#if FEATURE_WITH_SYSPLL_CLK
    /* Config SYSPLL disable */
    timeout = SYSPLL_STABILIZATION_TIMEOUT;
    POWER_HW_SCG_ConfigSYSPLLEnable(false);
    while ((POWER_HW_SCG_ReadSysPllStatus(SCG) != false) && (--timeout > 0U))
    {
    }

    if (timeout == 0U)
    {
        returnValue = STATUS_ERROR;
    }
#endif /* #if FEATURE_WITH_SYSPLL_CLK */
    return returnValue;
}

/*!
 * @brief This function switch to the very low power run clock.
 *
 * @param sysClk: SYS_CLK_CONFIG_T pointer.
 *
 * @retval Operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - Others status code: Operation failed.
 *
 * @note This function will change system clock in run mode before MCU enter very low power run mode.
 */
static STATUS_T POWER_SwitchToVlprClk(const SYS_CLK_CONFIG_T *const sysClk)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    SYS_CLK_CONFIG_T sysClkCfgOfVlpr;
    CLOCK_NAMES_T currentSysClkSrc = sysClk->src;

    if (currentSysClkSrc != LSICLK_CLK)
    {
        /* Config LSI the system clock source */
        sysClkCfgOfVlpr.src = LSICLK_CLK;
        sysClkCfgOfVlpr.dividers[0U] = (uint16_t)(POWER_HW_SCG_ReadVlprDivCore(SCG) + 1U);  /* Core clock divider */
        sysClkCfgOfVlpr.dividers[1U] = (uint16_t)(POWER_HW_SCG_ReadVlprDivBus(SCG) + 1U);   /* Bus clock divider */
        sysClkCfgOfVlpr.dividers[2U] = (uint16_t)(POWER_HW_SCG_ReadVlprDivLow(SCG) + 1U);   /* Bus clock divider */

        returnValue = CLOCK_SYS_ConfigSystemClock(NULL, &sysClkCfgOfVlpr);
    }

    return returnValue;
}

#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
/*!
 * @brief This function to enter configuration to a Hsrun power mode.
 *
 * @param None.
 *
 * @retval Operation status.
 *        - STATUS_SUCCESS: Operation was successful.
 *        - Others status code: Operation failed.
 */
static STATUS_T POWER_EnterHsrunMode(void)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    SYSMC_POWER_MODE_CONFIG_T powerModeCfg;

#if FEATURE_WITH_SYSPLL_CLK
    bool checkEnableSYSPLL = POWER_HW_SCG_ReadHsrunSelectSYSPLL();

    if ((g_enableSYSPLL == false) && (g_enableHSI == false))
    {
        returnValue = STATUS_ERROR;
    }
    else if ((checkEnableSYSPLL && (g_enableSYSPLL == false)) || (!checkEnableSYSPLL && (g_enableHSI == false)))
    {
        returnValue = STATUS_ERROR;
    }
    else
#endif
    {
        if (g_changeClkVlp == false)
        {
            powerModeCfg.powerMode = POWER_MODE_HSRUN;
            /* Switch the mode */
            returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
        }
        else
        {
            returnValue = POWER_EnableVlpClockSrc();
            if (returnValue == STATUS_SUCCESS)
            {
                /* Update initialize clock configuration */
                returnValue = POWER_UpdateInitClk(&g_sysClkCfg);
            }

            if (returnValue == STATUS_SUCCESS)
            {
                g_changeClkVlp = false;
                powerModeCfg.powerMode = POWER_MODE_HSRUN;
                /* Switch the mode */
                returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
            }
            else
            {
                returnValue = STATUS_ERROR;
            }
        }
    }

    return returnValue;
}
#endif /* FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE */


/*!
 * @brief This function switch to a running power mode.
 *
 * @param cfgPtr: Pointer to user configuration structure
 *
 * @retval Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - Others status code: Operation failed.
 */
static STATUS_T POWER_SwitchToRunningPowerMode(const POWER_USER_CONFIG_T *const cfgPtr)
{
    SYSMC_POWER_MODE_CONFIG_T powerModeCfg;
    POWER_MODE_STATUS_T currentModeStatus = POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC);
    STATUS_T returnValue = STATUS_SUCCESS;

    /* Config the running mode */
    if (cfgPtr->powerMode == POWER_MODE_RUN)
    {
        /* Run mode */
        if (currentModeStatus != RUN_STATUS)
        {
            powerModeCfg.powerMode = POWER_MODE_RUN;
            /* Switch the mode */
            returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
        }
        if ((returnValue == STATUS_SUCCESS) && (g_changeClkVlp == true))
        {
            /* Enable all clock source */
            returnValue = POWER_EnableVlpClockSrc();
            if (returnValue == STATUS_SUCCESS)
            {
                /* Update initialize clock configuration */
                returnValue = POWER_UpdateInitClk(&g_sysClkCfg);
                if (returnValue == STATUS_SUCCESS)
                {
                    g_changeClkVlp = false;
                }
            }
        }
    }
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    /* Config the high speed run mode */
    else if (cfgPtr->powerMode == POWER_MODE_HSRUN)
    {
        /* High speed run mode */
        /* High speed run mode can be entered only from Run mode */
        if (currentModeStatus != HSRUN_STATUS)
        {
            if (currentModeStatus != RUN_STATUS)
            {
                powerModeCfg.powerMode = POWER_MODE_RUN;
                /* Switch the mode */
                returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
            }
            if (returnValue == STATUS_SUCCESS)
            {
                returnValue = POWER_EnterHsrunMode();
            }
        }
    }
#endif /* FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE */
    else if (cfgPtr->powerMode == POWER_MODE_VLPR)
    {
        /* Very low power run mode */
        if (currentModeStatus != VLPR_STATUS)
        {
            /* Very low power run mode can be entered only from Run mode */
            if (POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC) != RUN_STATUS)
            {
                powerModeCfg.powerMode = POWER_MODE_RUN;
                /* Switch the mode */
                returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
            }
            if (returnValue == STATUS_SUCCESS)
            {
                if (!g_changeClkVlp)
                {
                    CLOCK_SYS_ReadSystemClockSource(&g_sysClkCfg);
                }
                returnValue = POWER_SwitchToVlprClk(&g_sysClkCfg);
                if (returnValue == STATUS_SUCCESS)
                {
                    g_changeClkVlp = true;
                    powerModeCfg.powerMode = POWER_MODE_VLPR;
                    /* Disable all clock source except LSI */
                    returnValue = POWER_DisableVlpClockSrc();
                }
                if (returnValue == STATUS_SUCCESS)
                {
                    /* Switch the mode */
                    returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
                }
            }
        }
    }
    else
    {
        powerModeCfg.powerMode = POWER_MODE_MAX;
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief This function switch to a sleeping power mode.
 *
 * @param cfgPtr: Pointer to user configuration structure
 *
 * @retval Operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - Others status code: Operation failed.
 */
static STATUS_T POWER_SwitchToSleepingPowerMode(const POWER_USER_CONFIG_T *const cfgPtr)
{
    SYSMC_POWER_MODE_CONFIG_T powerModeCfg;
    STATUS_T returnValue = STATUS_SUCCESS;
    POWER_MODE_STATUS_T powerModeStatus = POWER_HW_SYSMC_ReadPowerModeStatus(SYSMC);

    if (cfgPtr->powerMode == POWER_MODE_VLPS )
    {
        /* Very low power stop mode can be entered only from Run mode or Very low power run mode*/
        if ((powerModeStatus != RUN_STATUS) && (powerModeStatus != VLPR_STATUS))
        {
            powerModeCfg.powerMode = POWER_MODE_RUN;
            returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
        }

        if (returnValue == STATUS_SUCCESS)
        {
            if (POWER_SYS_ReadCurrentRunMode() == POWER_MODE_RUN)
            {
                /* Get current source clock */
                if (!g_changeClkVlp)
                {
                    CLOCK_SYS_ReadSystemClockSource(&g_sysClkCfg);
                }
                returnValue = POWER_SwitchToVlprClk(&g_sysClkCfg);
                if (STATUS_SUCCESS == returnValue)
                {
                    g_changeClkVlp = true;
                    powerModeCfg.powerMode = POWER_MODE_VLPS;
                    /* Disable all clock source except LSI */
                    returnValue = POWER_DisableVlpClockSrc();
                }
            }
        }
        powerModeCfg.powerMode = POWER_MODE_VLPS;
    }
    else if (cfgPtr->powerMode == POWER_MODE_STOP1 || POWER_MODE_STOP2 )
    {
        if (powerModeStatus != RUN_STATUS)
        {
            powerModeCfg.powerMode = POWER_MODE_RUN;
            returnValue = POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg);
        }

        powerModeCfg.powerMode = cfgPtr->powerMode;
        /* Set the stop option value */
        if (POWER_MODE_STOP1 == cfgPtr->powerMode)
        {
            powerModeCfg.stopOption = SYSMC_STOP1;
        }
        else
        {
            powerModeCfg.stopOption = SYSMC_STOP2;
        }
    }
    else
    {
        powerModeCfg.powerMode = POWER_MODE_MAX;
        returnValue = STATUS_UNSUPPORTED;
    }

    if (returnValue == STATUS_SUCCESS)
    {
        /* Configure (deep) sleep state */
        if (!cfgPtr->sleepOnExitValue)
        {
            /* Do not re-enter (deep) sleep state on ISR exit */
            APM32_SCB->SCR.reg &= ~(0x01U << 1U);
        }
        else
        {
            /* Go back to (deep) sleep state on ISR exit */
            APM32_SCB->SCR.reg |= (0x01U << 1U);
        }

        if (POWER_HW_SYSMC_ConfigPowerMode(SYSMC, &powerModeCfg) != STATUS_SUCCESS)
        {
            returnValue = STATUS_TRANSITION_FAILED;
        }
    }

    return returnValue;
}

/*!
 * @brief This function configures the power mode.
 *
 * @details This function performs the actual implementation-specific logic to switch to
 *          one of the defined power modes.
 *
 * @param cfgPtr: Pointer to user configuration structure
 *
 * @retval Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_TRANSITION_FAILED: Operation failed.
 */
static STATUS_T POWER_DoConfigMode(const POWER_USER_CONFIG_T *const cfgPtr)
{
    STATUS_T returnValue;

    /* Check whether the power mode is a sleeping or a running power mode */
    if (cfgPtr->powerMode > POWER_MODE_VLPR)
    {
        returnValue = POWER_SwitchToSleepingPowerMode(cfgPtr);
    }
    else
    {
        returnValue = POWER_SwitchToRunningPowerMode(cfgPtr);
    }

    return returnValue;
}



/*!
 * @brief This function will read status of SYSPLL, HSI, SYSOSC clock source enable or not.
 *
 * @param None.
 *
 * @retval None.
 */
static void POWER_ReadEnableClockSrc(void)
{
    g_enableSYSOSC = POWER_HW_SCG_ReadSYSOSCEnable();
    g_enableHSI = POWER_HW_SCG_ReadHSIEnable();
#if FEATURE_WITH_SYSPLL_CLK
    g_enableSYSPLL = POWER_HW_SCG_ReadSYSPLLEnable();
#endif /* #if FEATURE_WITH_SYSPLL_CLK */
}

/*!
 * @brief This function used by POWER_SYS_ConfigPowerMode function for callback management
 *
 * @param notify   notification structure.
 * @param currentCallback   index to array of statically registered call-backs.
 * @param policy    transaction policy.
 *
 * @retval An error code or STATUS_SUCCESS.
 */
static STATUS_T POWER_CallbacksManagement(POWER_NOTIFY_STRUCT_T *notify,
                                       uint8_t *currentCallback,
                                       POWER_POLICY_T policy)
{
    uint8_t numOfCallbacks;
    STATUS_T returnValue = STATUS_SUCCESS;
    STATUS_T errorStatus = STATUS_SUCCESS;
    STATUS_T callbackStatus;
    POWER_CALLBACK_TYPE_T callbackTypeFilter;

    if (notify->type == POWER_NOTIFY_BEFORE)
    {
        numOfCallbacks = g_powerManagerState.callbacksMaxNum;
        callbackTypeFilter = POWER_CALLBACK_AFTER;
        errorStatus = STATUS_NOTIFY_BEFORE_ERROR;
    }
    else if (notify->type == POWER_NOTIFY_AFTER)
    {
        numOfCallbacks = g_powerManagerState.callbacksMaxNum;
        callbackTypeFilter = POWER_CALLBACK_BEFORE;
        errorStatus = STATUS_NOTIFY_AFTER_ERROR;
    }
    else if (notify->type == POWER_NOTIFY_RECOVER)
    {
        numOfCallbacks = g_powerManagerState.callbacksMaxNum;
        callbackTypeFilter = POWER_CALLBACK_AFTER;
        errorStatus = STATUS_NOTIFY_BEFORE_ERROR;
    }
    else
    {
        numOfCallbacks = 0U;
        callbackTypeFilter = POWER_CALLBACK_BEFORE;
    }

    /* From all statically registered call-backs... */
    for ((*currentCallback) = 0U; (*currentCallback) < numOfCallbacks; (*currentCallback)++)
    {
        /* Pointer to callback configuration */
        const POWER_CALLBACK_USER_CONFIG_T * const callbackCfg = ((*g_powerManagerState.callbacks)[*currentCallback]);

        /* Check pointer to static callback configuration */
        if ((callbackCfg != NULL) && (callbackTypeFilter != callbackCfg->type))
        {
            /* In case that call-back returned error code mark it,
             * store the call-back handle and eventually cancel the mode switch
             */
            callbackStatus = callbackCfg->function(notify, callbackCfg->data);
            if (STATUS_SUCCESS != callbackStatus)
            {
                returnValue = errorStatus;
                g_powerManagerState.errorCallbackIdx = *currentCallback;
                /* If not forcing power mode switch,
                 * call all already notified call-backs to revert their state as the mode change is canceled
                 */
                if (policy != POWER_POLICY_FORCIBLE)
                {
                    break;
                }
            }
        }
    }

    return returnValue;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configures the the RUN mode control setting.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param runMode: Run mode setting defined in SYSMC_RUN_MODE_T
 *
 * @retval None.
 */
void POWER_HW_SYSMC_ConfigRunModeControl(SYSMC_T *const baseAddr, const SYSMC_RUN_MODE_T runMode)
{
    baseAddr->PMCTRL.bit.RUNCTRL = SYSMC_PMCTRL_RUNCTRL_00;
    baseAddr->PMCTRL.bit.RUNCTRL = (uint32_t)runMode;
}

/*!
 * @brief Configures the STOP mode control setting.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param stopMode: Stop mode defined in SYSMC_STOP_MODE_T
 *
 * @retval None.
 */
void POWER_HW_SYSMC_ConfigStopModeControl(SYSMC_T *const baseAddr, const SYSMC_STOP_MODE_T stopMode)
{
    baseAddr->PMCTRL.bit.STOPCTRL = SYSMC_PMCTRL_STOPCTRL_000;
    baseAddr->PMCTRL.bit.STOPCTRL = (uint32_t)stopMode;
}

/*!
 * @brief Configures the STOPO (Stop Option).
 *
 * @details It controls the type of the stop operation when STOPM=STOP. When entering Stop mode
 * from RUN mode, the PMU, SCG and flash remain fully powered, allowing the device to
 * wakeup almost instantaneously at the expense of higher power consumption. In STOP2,
 * only system clocks are gated allowing peripherals running on bus clock to remain fully
 * functional. In STOP1, both system and bus clocks are gated.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param option STOPO option setting defined in SYSMC_STOP_OPTION_T
 *
 * @retval None.
 */
void POWER_HW_SYSMC_ConfigStopOption(SYSMC_T *const baseAddr, const SYSMC_STOP_OPTION_T option)
{
    baseAddr->STOPOSEL.bit.STOPOSEL = 0x00U;
    baseAddr->STOPOSEL.bit.STOPOSEL = (uint32_t)option;
}

/*!
 * @brief Configures the power mode.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param powerModeConfig Power mode configuration structure SYSMC_POWER_MODE_CONFIG_T

 * @retval errorCode SYSMC error code
 */
STATUS_T POWER_HW_SYSMC_ConfigPowerMode(SYSMC_T *const baseAddr,
                                        const SYSMC_POWER_MODE_CONFIG_T *const powerModeConfig)
{
    STATUS_T retCode;
    SYSMC_STOP_MODE_T stopMode;
    POWER_MODE_T powerMode = powerModeConfig->powerMode;

    /* Branch based on power mode name*/
    if (powerMode == POWER_MODE_RUN)
    {
        /* Biasing disabled, core logic can run in full performance */
        POWER_HW_PMU_DisableBiasen(PMU);
        /* Set to RUN mode. */
        POWER_HW_SYSMC_ConfigRunModeControl(baseAddr, SYSMC_RUN);
        /* Wait for stat change */
        if (!POWER_HW_SYSMC_WaitForStatChange(baseAddr, RUN_STATUS, SYSMC_TIMEOUT))
        {
            /* Timeout for power mode change expired. */
            retCode = STATUS_TRANSITION_FAILED;
        }
        else
        {
            retCode = STATUS_SUCCESS;
        }
    }
    else if (powerMode == POWER_MODE_VLPR)
    {
        /* Biasing enable before entering VLP* mode to educe MCU power consumption in low power mode*/
        POWER_HW_PMU_EnableBiasen(PMU);

        /* Set power mode to VLPR*/
        POWER_HW_SYSMC_ConfigRunModeControl(baseAddr, SYSMC_VLPR);
        /* Wait for stat change */
        if (!POWER_HW_SYSMC_WaitForStatChange(baseAddr, VLPR_STATUS, SYSMC_TIMEOUT))
        {
            /* Timeout for power mode change expired. */
            retCode = STATUS_TRANSITION_FAILED;
        }
        else
        {
            retCode = STATUS_SUCCESS;
        }
    }
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    else if (powerMode == POWER_MODE_HSRUN)
    {
        /* Biasing disabled, core logic can run in full performance */
        POWER_HW_PMU_DisableBiasen(PMU);
        /* Set power mode to HSRUN */
        POWER_HW_SYSMC_ConfigRunModeControl(baseAddr, SYSMC_HSRUN);
        /* Wait for stat change */
        if (!POWER_HW_SYSMC_WaitForStatChange(baseAddr, HSRUN_STATUS, SYSMC_TIMEOUT))
        {
            /* Timeout for power mode change expired. */
            retCode = STATUS_TRANSITION_FAILED;
        }
        else
        {
            retCode = STATUS_SUCCESS;
        }
    }
#endif /*FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE*/
    else if (powerMode == POWER_MODE_STOP1 || POWER_MODE_STOP2 || POWER_MODE_VLPS)
    {
        if ((powerMode == POWER_MODE_STOP1) || (powerMode == POWER_MODE_STOP2))
        {
            stopMode = SYSMC_STOP;
            POWER_HW_SYSMC_ConfigStopOption(baseAddr, powerModeConfig->stopOption);
        }
        else
        {
            /* Biasing enable before entering VLPS mode to educe MCU power consumption in low power mode*/
            POWER_HW_PMU_EnableBiasen(PMU);
            stopMode = SYSMC_VLPS;
        }

        /* Set power mode to specified STOP mode*/
        POWER_HW_SYSMC_ConfigStopModeControl(baseAddr, stopMode);

        /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP)*/
        APM32_SCB->SCR.reg |= (0x01U << 2U);

        /* Cpu is going into deep sleep state */
        WFI();

        /* check the current mode to control bias bit */
        if (POWER_HW_SYSMC_ReadPowerModeStatus(baseAddr) == RUN_STATUS)
        {
            POWER_HW_PMU_DisableBiasen(PMU);
        }
        else
        {
            POWER_HW_PMU_EnableBiasen(PMU);
        }

        retCode = STATUS_SUCCESS;
    }
    else
    {
        retCode = STATUS_UNSUPPORTED;
    }

    return retCode;
}

/*!
 * @brief Configures all power mode protection settings.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param protectConfig Configurations for the supported power mode protect settings
 *                      - See SYSMC_POWER_MODE_PROTECTION_CONFIG_T for details.
 *
 * @retval None.
 */
void POWER_HW_SYSMC_ConfigProtectionMode(SYSMC_T *const baseAddr,
                                        const SYSMC_POWER_MODE_PROTECTION_CONFIG_T *const protectConfig)
{
    /* Initialize the setting */
    uint32_t regValue = 0U;

    /* Check configurations for each mode and combine the setting together */
    if (protectConfig->vlpProtect)
    {
        regValue |= (0x01U << 5U);
    }

#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    if (protectConfig->hsrunProtect)
    {
        regValue |= (0x01U << 7U);
    }
#endif

    /* Write once into PMPRO register */
    baseAddr->PMPRO.reg = regValue;
}

/*!
 * @brief Reads the current power mode stat.
 *
 * @details This function returns the current power mode stat. Once application
 * switches the power mode, it should always check the stat to check whether it
 * runs into the specified mode or not. An application should check
 * this mode before switching to a different mode. The system requires that
 * only certain modes can switch to other specific modes. See the
 * reference manual for details and the power_mode_stat for information about
 * the power stat.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 *
 * @retval Current power mode stat
 */
POWER_MODE_STATUS_T POWER_HW_SYSMC_ReadPowerModeStatus(const SYSMC_T *const baseAddr)
{
    POWER_MODE_STATUS_T retValue;
    uint32_t regValue;

    regValue = baseAddr->PMSTS.bit.PMSTS;

    if (regValue == 1UL)
    {
        retValue = RUN_STATUS;
    }
    else if (regValue == 2UL)
    {
        retValue = STOP_STATUS;
    }
    else if (regValue == 4UL)
    {
        retValue = VLPR_STATUS;
    }
    else if (regValue == 16UL)
    {
        retValue = VLPS_STATUS;
    }
#if FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE
    else if (regValue == 128UL)
    {
        retValue = HSRUN_STATUS;
    }
#endif /*FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE*/
    else
    {
        retValue = INVALID_STATUS;
    }

    return retValue;
}

/*!
 * @brief Wait for the power mode status is changed or timeout.
 *
 * @param baseAddr: Base address for current SYSMC instance.
 * @param mode power mode status.
 * @param timeout timeout time.
 *
 * @retval change status.
 *         - true: power mode has been changed successfully.
 *         - false: timeout expired, power mode has not been changed.
 */
bool POWER_HW_SYSMC_WaitForStatChange(const SYSMC_T *const baseAddr,
                                        const POWER_MODE_STATUS_T mode,
                                        const uint32_t timeout)
{
    uint32_t i;
    bool retValue;

    /* Waiting for register read access. It's no longer apply when bus clock has very low frequency in HSRUN mode */
    if (mode == HSRUN_STATUS)
    {
        for (i = 0U; i < 100U; i++)
        {
            /* Do nothing */
        }
    }

    for (i = 0U; i < timeout; i++)
    {
        if (mode == POWER_HW_SYSMC_ReadPowerModeStatus(baseAddr))
        {
            /* Power mode has been changed successfully */
            break;
        }
    }

    /* If i greater or equal to timeout, then timeout expired(the power mode has not been changed) */
    retValue = (i < timeout);

    return retValue;
}

/*!
 * @brief Read core clock divide ratio value.
 *
 * @param baseAddr: Base address for current SCG instance.
 *
 * @retval core clock divide ratio value.
 */
uint32_t POWER_HW_SCG_ReadVlprDivCore(const SCG_T *const baseAddr)
{
    return ((baseAddr->VLPRCLKCTRL.reg & (0x0FU << 16U)) >> 16U);
}

/*!
 * @brief Read bus clock divide ratio value.
 *
 * @param baseAddr: Base address for current SCG instance.
 *
 * @retval bus clock divide ratio value.
 */
uint32_t POWER_HW_SCG_ReadVlprDivBus(const SCG_T *const baseAddr)
{
    return ((baseAddr->VLPRCLKCTRL.reg & (0x0FU << 4U)) >> 4U);
}

/*!
 * @brief Read slow clock divide ratio value.
 *
 * @param baseAddr: Base address for current SCG instance.
 *
 * @retval slow clock divide ratio value.
 */
uint32_t POWER_HW_SCG_ReadVlprDivLow(const SCG_T *const baseAddr)
{
    return ((baseAddr->VLPRCLKCTRL.reg & (0x0FU << 0U)) >> 0U);
}

#if FEATURE_WITH_SYSPLL_CLK
/*!
 * @brief Config the SYSPLL clock source was enabled or disabled
 *
 * @param enable: This variable select the SYSPLL available or not.
 *
 * @retval None.
 *
 * @note This one is used in the switching very low power mode sequence.
 */
void POWER_HW_SCG_ConfigSYSPLLEnable(bool enable)
{
    uint32_t regValue = SCG->SYSPLLCSTS.reg;
    regValue &= ~(0x01U << 0U);
    if (enable == true)
    {
        regValue |= (0x01U << 0U);
    }
    else
    {
        regValue |= (0x00U << 0U);
    }
    SCG->SYSPLLCSTS.reg = regValue;
}

/*!
 * @brief Reads status of SYSPLL clock source was enabled or disabled
 *
 * @param None.
 *
 * @retval sysPll clock enable
 *         - false: SYSPLL disable
 *         - true : SYSPLL enabled
 *
 * @note This one is used in the switching very low power mode sequence.
 */
bool POWER_HW_SCG_ReadSYSPLLEnable(void)
{
    return (((SCG->SYSPLLCSTS.reg & (0x01U << 0U)) >> 0U) != 0U) ? true : false;
}

/*!
 * @brief Reads sysPll clock status

 * @details This function checks whether SYSPLL is enabled and output clock is valid.
 *
 * @param base: scg base pointer
 *
 * @retval sysPll clock status
 *         - false: SYSPLL is not enabled or clock is not valid
 *         - true : SYSPLL is enabled and clock is valid
 */
bool POWER_HW_SCG_ReadSysPllStatus(const SCG_T *const base)
{
    return (((base->SYSPLLCSTS.reg & (0x01U << 24U)) >> 24U) != 0U) ? true : false;
}

#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
/*!
 * @brief Check sysPll clock source in HSRUN mode
 *
 * @param None.
 *
 * @retval true  : SYSPLL is clock source in HSRUN.
 *         false : SYSPLL is not clock source in HSRUN.
 *
 * @note This one is used in the switching very low power mode sequence.
 */
bool POWER_HW_SCG_ReadHsrunSelectSYSPLL(void)
{
    return (((SCG->HSRUNCLKCTRL.reg & (0x0FU << 24U)) >> 24U) == 6U) ? true : false;
}
#endif /* FEATURE_WITH_HIGH_SPEED_RUN_MODE */
#endif /* FEATURE_WITH_SYSPLL_CLK */

/*!
 * @brief Config the HSI clock source was enabled or disabled
 *
 * @param enable: This variable select the HSI available or not.
 *
 * @retval None.
 *
 * @note This one is used in the switching very low power mode sequence.
 */
void POWER_HW_SCG_ConfigHSIEnable(bool enable)
{
    uint32_t regValue = SCG->HSICSTS.reg;
    regValue &= ~(0x01U << 0);
    if (enable == true)
    {
        regValue |= (0x01U << 0U);
    }
    else
    {
        regValue |= (0x00U << 0U);
    }
    SCG->HSICSTS.reg = regValue;
}

/*!
 * @brief Read status of HSI clock source was enabled or disabled
 *
 * @param None.
 *
 * @retval true  : HSI enabled
 *         false : HSI disable
 *
 * @note This one is used in the switching very low power mode sequence.
 */
bool POWER_HW_SCG_ReadHSIEnable(void)
{
    return (((SCG->HSICSTS.reg & (0x01U << 0U)) >> 0U) != 0U) ? true : false;
}

/*!
 * @brief Read HSI clock status
 *
 * @details This function checks whether HSI is enabled and output clock is valid.
 *
 * @param base: scg base pointer
 *
 * @retval HSI clock status
 *         - false: HSI is not enabled or clock is not valid
 *         - true : HSI is enabled and clock is valid
 */
bool POWER_HW_SCG_ReadHSIStatus(const SCG_T *const base)
{
    return (((base->HSICSTS.reg & (0x01U << 24U)) >> 24U) != 0U) ? true : false;
}

/*!
 * @brief Config the SYSOSC clock source was enabled or disabled
 *
 * @param enable: This variable select the SYSOSC available or not.
 *
 * @retval None.
 *
 * @note This one is used in the switching very low power mode sequence.
 */
void POWER_HW_SCG_ConfigSYSOSCEnable(bool enable)
{
    uint32_t regValue = SCG->SYSOSCCSTS.reg;
    regValue &= ~(0x01U << 0U);
    if (enable == true)
    {
        regValue |= (0x01U << 0U);
    }
    else
    {
        regValue |= (0x00U << 0U);
    }
    SCG->SYSOSCCSTS.reg = regValue;
}

/*!
 * @brief Read status of SYSOSC clock source was enabled or disabled
 *
 * This function reads SYSOSC clock source enable or disabled.
 * This one is used in the switching very low power mode sequence.
 *
 * @retval true  : SYSOSC enabled
           false : SYSOSC disable
 */
bool POWER_HW_SCG_ReadSYSOSCEnable(void)
{
    return (((SCG->SYSOSCCSTS.reg & (0x01U << 0U)) >> 0U) != 0U) ? true : false;
}

/*!
 * @brief Gets SYSOSC clock status
 * This function checks whether SYSOSC is enabled and output clock is valid.
 *
 * @param base  scg base pointer
 *
 * @retval  SYSOSC  clock status
 *        - false : SYSOSC is not enabled or clock is not valid
 *        - true  : SYSOSC is enabled and clock is valid
 */
bool POWER_HW_SCG_ReadSYSOSCStatus(const SCG_T *const base)
{
    return (((base->SYSOSCCSTS.reg & (0x01U << 24U)) >> 24U) != 0U) ? true : false;
}

/*!
 * @brief Enable the BIASEN bit.
 *
 * @details bit enables source and well biasing for the core logic in low power mode
 *
 * @param baseAddr: Base address for current PMU instance.
 *
 * @retval None.
 */
void POWER_HW_PMU_EnableBiasen(PMU_T *const baseAddr)
{
    baseAddr->REGCSTS.bit.BEN = (uint8_t)PMU_REGCSTS_BEN_1;
}

/*!
 * @brief Disable the BIASEN bit.
 *
 * @details disabled, core logic can run in full performance
 *
 * @param baseAddr: Base address for current PMU instance.
 *
 * @retval None.
 */
void POWER_HW_PMU_DisableBiasen(PMU_T *const baseAddr)
{
    baseAddr->REGCSTS.bit.BEN = (uint8_t)PMU_REGCSTS_BEN_0;
}

/*!
 * @brief Read system reset source status
 *
 * @param base rmu base pointer
 * @param srcName system reset source
 *
 * @retval system reset source status
 *         - true : system reset source is reset
 *         - false : system reset source is not reset
 */
bool POWER_HW_RMU_ReadResetSrcStatus(const RMU_T *const base, const RMU_SRC_NAME_T srcName)
{
    bool returnValue = 0U;
    uint32_t regValue = 0U;

    if (srcName == RMU_BY_LOW_VOLT_DETECT)
    {
        regValue = base->RSTFLG.bit.LVDRSTFLG;
    }
    else if (srcName == RMU_BY_LOSS_OF_CLK)
    {
        regValue = base->RSTFLG.bit.CLKLRSTFLG;
    }
    else if (srcName == RMU_BY_LOSS_OF_LOCK)
    {
        regValue = base->RSTFLG.bit.LOCKLRSTFLG;
    }
    else if (srcName == RMU_BY_WATCH_DOG)
    {
        regValue = base->RSTFLG.bit.WDTTOFFLG;
    }
    else if (srcName == RMU_BY_EXTERNAL_PIN)
    {
        regValue = base->RSTFLG.bit.ERSTPFLG;
    }
    else if (srcName == RMU_BY_POWER_ON)
    {
        regValue = base->RSTFLG.bit.PORFLG;
    }
    else if (srcName == RMU_BY_SJTAG)
    {
        regValue = base->RSTFLG.bit.JTAGRSTFLG;
    }
    else if (srcName == RMU_BY_CORE_LOCKUP)
    {
        regValue = base->RSTFLG.bit.CLOCKEFLG;
    }
    else if (srcName == RMU_BY_SOFTWARE)
    {
        regValue = base->RSTFLG.bit.SWSETFLG;
    }
    else if (srcName == RMU_BY_SMDM_AP)
    {
        regValue = base->RSTFLG.bit.SRSTREQFLG;
    }
    else if (srcName == RMU_BY_STOP_MODE_ACK_ERR)
    {
        regValue = base->RSTFLG.bit.STOPACKEFLG;
    }
    else
    {
        regValue = 0U;
    }

    returnValue = (regValue == 0UL) ? false : true;

    return returnValue;
}


/**@} end of group POWER_Functions*/
/**@} end of group POWER_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
