/*!
 * @file        apm32f445_446_clock.c
 *
 * @brief       This file provides all the CLOCK firmware functions
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
#include "apm32f445_446_clock.h"


/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CLOCK_Driver CLOCK Driver
  @{
*/

/** @defgroup CLOCK_Macros Macros
  @{
*/

#define CLOCK_QSPI_MCR_SCLKCFG_CLK_MOD       0x40U    /* Quadspi Clocking mode selection  */
#define CLOCK_QSPI_MCR_SCLKCFG_EXT_DQS       0x20U    /* Use external DQS (HyperRAM mode) */
#define CLOCK_QSPI_MCR_SCLKCFG_CLK_SRC       0x10U    /* QuadSPI source clock selection   */

#if FEATURE_WITH_SYSPLL_CLK
/* @brief System PLL base multiplier value, it is the multiplier value when SCG_SYSPLLCFG[SYSPLLMULCFG]=0. */
#define SCG_SYSPLL_MULT_BASE 16U

/*
 * @brief System PLL base divider value, it is the PLL reference clock divider value when
 * SCG_SYSPLLCFG[PLLRDIVCFG]=0.
 */
#define SCG_SYSPLL_PREDIV_BASE 1U

/*
 * @brief System PLL reference clock after SCG_SYSPLLCFG[PLLRDIVCFG] should be in the range of
 * SCG_SYSPLL_REF_MIN to SCG_SYSPLL_REF_MAX.
 */
#define SCG_SYSPLL_REF_MIN 8000000U
#define SCG_SYSPLL_REF_MAX 32000000U
#endif

/*
 * @brief LPO 128K fixed clock frequency.
 */
#define LPO_128K_FREQUENCY 128000UL

/*
 * @brief LPO 32K fixed clock frequency.
 */
#define LPO_32K_FREQUENCY 32000UL

/*
 * @brief LPO 1K fixed clock frequency.
 */
#define LPO_1K_FREQUENCY 1000UL

/*
 * @brief Running modes.
 */
#define HIGH_SPEED_RUNNING_MODE (1UL << 7U)
#define RUN_SPEED_RUNNING_MODE  (1UL << 0U)
#define VLPR_SPEED_RUNNING_MODE (1UL << 2U)


#define MODES_MAX_INDEX 7U

#define CLOCK_MAX_FREQS_IN_VLPR_MODE                                                                        \
{/* SYS_CLK    BUS_CLK     SLOW_CLK */                                                                      \
{       0UL,       0UL,         0UL},               /*  Invalid entry */                                    \
{ 4000000UL, 4000000UL,   1000000UL},               /*  Maximum frequencies when system clock is SYSOSC   */  \
{ 4000000UL, 4000000UL,   1000000UL},               /*  Maximum frequencies when system clock is LSICLK */  \
{ 4000000UL, 4000000UL,   1000000UL},               /*  Maximum frequencies when system clock is HSICLK */  \
{       0UL,       0UL,         0UL},               /*  Invalid entry */                                    \
{       0UL,       0UL,         0UL},               /*  Invalid entry */                                    \
{ 4000000UL, 4000000UL,   1000000UL},               /*  Maximum frequencies when system clock is SYSPLL   */  \
}

#define CLOCK_MAX_FREQS_IN_RUN_MODE                                                                        \
{/* SYS_CLK    BUS_CLK     SLOW_CLK */                                                                     \
{       0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{80000000UL, 48000000UL,  26670000UL},              /*  Maximum frequencies when system clock is SYSOSC   */ \
{80000000UL, 48000000UL,  26670000UL},              /*  Maximum frequencies when system clock is LSICLK */ \
{80000000UL, 48000000UL,  26670000UL},              /*  Maximum frequencies when system clock is HSICLK */ \
{       0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{       0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{80000000UL, 40000000UL,  26670000UL},              /*  Maximum frequencies when system clock is SYSPLL   */ \
}


#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
#define CLOCK_MAX_FREQS_IN_HSRUN_MODE                                                                       \
{/* SYS_CLK    BUS_CLK     SLOW_CLK */                                                                      \
{        0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{112000000UL, 56000000UL,  28000000UL},              /*  Maximum frequencies when system clock is SYSOSC */   \
{112000000UL, 56000000UL,  28000000UL},              /*  Maximum frequencies when system clock is LSICLK */ \
{112000000UL, 56000000UL,  28000000UL},              /*  Maximum frequencies when system clock is HSICLK */ \
{        0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{        0UL,        0UL,         0UL},              /*  Invalid entry */                                   \
{112000000UL, 56000000UL,  28000000UL},              /*  Maximum frequencies when system clock is SYSPLL   */ \
}
#endif

/*
 * @brief Number of peripheral clocks.
 */

#if defined(PCM_FLASH_INDEX)
    #define TEMP_FLASH 1U
#else
    #define TEMP_FLASH 0U
#endif
#if defined(PCM_DMACHM_INDEX)
    #define TEMP_DMACHM 1U
#else
    #define TEMP_DMACHM 0U
#endif
#if defined(PCM_CAN0_INDEX)
    #define TEMP_CAN0 1U
#else
    #define TEMP_CAN0 0U
#endif
#if defined(PCM_CAN1_INDEX)
    #define TEMP_CAN1 1U
#else
    #define TEMP_CAN1 0U
#endif
#if defined(PCM_CFGTMR3_INDEX)
    #define TEMP_CFGTMR3 1U
#else
    #define TEMP_CFGTMR3 0U
#endif
#if defined(PCM_ADC1_INDEX)
    #define TEMP_ADC1 1U
#else
    #define TEMP_ADC1 0U
#endif
#if defined(PCM_CAN2_INDEX)
    #define TEMP_CAN2 1U
#else
    #define TEMP_CAN2 0U
#endif
#if defined(PCM_LPSPI0_INDEX)
    #define TEMP_LPSPI0 1U
#else
    #define TEMP_LPSPI0 0U
#endif
#if defined(PCM_LPSPI1_INDEX)
    #define TEMP_LPSPI1 1U
#else
    #define TEMP_LPSPI1 0U
#endif
#if defined(PCM_LPSPI2_INDEX)
    #define TEMP_LPSPI2 1U
#else
    #define TEMP_LPSPI2 0U
#endif
#if defined(PCM_PDU1_INDEX)
    #define TEMP_PDU1 1U
#else
    #define TEMP_PDU1 0U
#endif
#if defined(PCM_CRC_INDEX)
    #define TEMP_CRC 1U
#else
    #define TEMP_CRC 0U
#endif
#if defined(PCM_PDU0_INDEX)
    #define TEMP_PDU0 1U
#else
    #define TEMP_PDU0 0U
#endif
#if defined(PCM_LPITMR_INDEX)
    #define TEMP_LPITMR 1U
#else
    #define TEMP_LPITMR 0U
#endif
#if defined(PCM_CFGTMR0_INDEX)
    #define TEMP_CFGTMR0 1U
#else
    #define TEMP_CFGTMR0 0U
#endif
#if defined(PCM_CFGTMR1_INDEX)
    #define TEMP_CFGTMR1 1U
#else
    #define TEMP_CFGTMR1 0U
#endif
#if defined(PCM_CFGTMR2_INDEX)
    #define TEMP_CFGTMR2 1U
#else
    #define TEMP_CFGTMR2 0U
#endif
#if defined(PCM_ADC0_INDEX)
    #define TEMP_ADC0 1U
#else
    #define TEMP_ADC0 0U
#endif
#if defined(PCM_RTC_INDEX)
    #define TEMP_RTC 1U
#else
    #define TEMP_RTC 0U
#endif
#if defined(PCM_LPTMR_INDEX)
    #define TEMP_LPTMR 1U
#else
    #define TEMP_LPTMR 0U
#endif
#if defined(PCM_PMA_INDEX)
    #define TEMP_PMA 1U
#else
    #define TEMP_PMA 0U
#endif
#if defined(PCM_PMB_INDEX)
    #define TEMP_PMB 1U
#else
    #define TEMP_PMB 0U
#endif
#if defined(PCM_PMC_INDEX)
    #define TEMP_PMC 1U
#else
    #define TEMP_PMC 0U
#endif
#if defined(PCM_PMD_INDEX)
    #define TEMP_PMD 1U
#else
    #define TEMP_PMD 0U
#endif
#if defined(PCM_PME_INDEX)
    #define TEMP_PME 1U
#else
    #define TEMP_PME 0U
#endif
#if defined(PCM_IO_INDEX)
    #define TEMP_IO 1U
#else
    #define TEMP_IO 0U
#endif
#if defined(PCM_EWDT_INDEX)
    #define TEMP_EWDT 1U
#else
    #define TEMP_EWDT 0U
#endif
#if defined(PCM_LPI2C_INDEX)
    #define TEMP_LPI2C 1U
#else
    #define TEMP_LPI2C 0U
#endif
#if defined(PCM_LPUART0_INDEX)
    #define TEMP_LPUART0 1U
#else
    #define TEMP_LPUART0 0U
#endif
#if defined(PCM_LPUART1_INDEX)
    #define TEMP_LPUART1 1U
#else
    #define TEMP_LPUART1 0U
#endif
#if defined(PCM_LPUART2_INDEX)
    #define TEMP_LPUART2 1U
#else
    #define TEMP_LPUART2 0U
#endif
#if defined(PCM_CFGTMR4_INDEX)
    #define TEMP_CFGTMR4 1U
#else
    #define TEMP_CFGTMR4 0U
#endif
#if defined(PCM_CFGTMR5_INDEX)
    #define TEMP_CFGTMR5 1U
#else
    #define TEMP_CFGTMR5 0U
#endif
#if defined(PCM_CFGTMR6_INDEX)
    #define TEMP_CFGTMR6 1U
#else
    #define TEMP_CFGTMR6 0U
#endif
#if defined(PCM_CFGTMR7_INDEX)
    #define TEMP_CFGTMR7 1U
#else
    #define TEMP_CFGTMR7 0U
#endif
#if defined(PCM_COMP_INDEX)
    #define TEMP_COMP 1U
#else
    #define TEMP_COMP 0U
#endif

#define CLOCK_PERIPHERALS_COUNT (TEMP_FLASH + TEMP_DMACHM + TEMP_CAN0 + TEMP_CAN1 + TEMP_CFGTMR3 + TEMP_ADC1 + TEMP_CAN2 + TEMP_LPSPI0 + TEMP_LPSPI1 + TEMP_LPSPI2 + TEMP_PDU1 + TEMP_CRC + TEMP_PDU0 + TEMP_LPITMR + \
                                  TEMP_CFGTMR0 + TEMP_CFGTMR1 + TEMP_CFGTMR2 + TEMP_ADC0 + TEMP_RTC + TEMP_LPTMR + TEMP_PMA + TEMP_PMB + TEMP_PMC + TEMP_PMD + TEMP_PME + TEMP_IO + TEMP_EWDT + TEMP_LPI2C + TEMP_LPUART0 + \
                                  TEMP_LPUART1 + TEMP_LPUART2 + TEMP_CFGTMR4 + TEMP_CFGTMR5 + TEMP_CFGTMR6 + TEMP_CFGTMR7 + TEMP_COMP)

/**@} end of group CLOCK_Macros*/


/** @defgroup CLOCK_Variables Variables
  @{
*/

CLOCK_MANAGER_STATE_T g_clockState;

/* This frequency values should be set by different boards. */
/* SIM */
uint32_t g_tclkFreq[NUMBER_OF_TCLK_INPUTS];      /* TCLKx clocks    */
/* RTC */
uint32_t g_rtcClkInFreq;                         /* RTC CLKIN clock */
/* SCG */
uint32_t g_extal0ClkFreq;                        /* EXTAL0 clock    */

/*! @brief Clock name mappings
 *         Constant array storing the mappings between clock names and peripheral clock control indexes.
 *         If there is no peripheral clock control index for a clock name, then the corresponding value is
 *         PCM_UNUSEFUL_INDEX.
 */
const uint16_t g_clockNameMappings[] = PCM_CLOCK_NAME_MAPPINGS;

/*! @brief Peripheral features list
 *         Constant array storing the mappings between clock names of the peripherals and feature lists.
 */
const uint8_t g_peripheralFeaturesList[] = PERIPHERAL_FEATURES;

/**@} end of group CLOCK_Variables*/


/** @defgroup CLOCK_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static void CLOCK_ReadDefaultConfig(CLOCK_MANAGER_USER_CONFIG_T * defaultClockCfg);
static STATUS_T CLOCK_ConfigScgSetting(const SCG_CONFIG_T * scgCfg);
static void CLOCK_ConfigPcmSetting(const PCM_CONFIG_T * peripheralClkCfg);
static void CLOCK_ConfigSimSetting(const SIM_CLOCK_CONFIG_T * simClkCfg);
static void CLOCK_ConfigPmuSetting(const PMU_CONFIG_T * pmuClkCfg);
static STATUS_T CLOCK_ConfigModulesFromScg(const SCG_CONFIG_T * scgConfig);
static STATUS_T CLOCK_ConfigLsiClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2);
static STATUS_T CLOCK_ConfigHsiClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2);
static STATUS_T CLOCK_ConfigSoscClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2);
static STATUS_T CLOCK_ConfigSysPllClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2);
static STATUS_T CLOCK_ConfigLSICLK(bool enable, const SCG_LSICLK_CONFIG_T * lsiConfig);
static STATUS_T CLOCK_ConfigHSICLK(bool enable, const SCG_HSICLK_CONFIG_T * hsiConfig);
static STATUS_T CLOCK_ConfigSYSOSC(bool enable, const SCG_SYSOSC_CONFIG_T * sysOscConfig);
static STATUS_T CLOCK_ConfigSYSPLL(bool enable, const SCG_SYSPLL_CONFIG_T * sysPllConfig);
static STATUS_T CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_T curSystemClockSrouce);
static STATUS_T CLOCK_ConfigTemporarySystemClock(void);
static STATUS_T CLOCK_ChangeSystemClock(const SCG_SYS_CLK_CONFIG_T * changeToClock);
static STATUS_T CLOCK_ConfigSystemClockCfg(SCG_SYSCLK_MODE_T mode, SCG_SYS_CLK_CONFIG_T const * config);
static void CLOCK_ConfigClockGate(CLOCK_NAMES_T peripheralClock, bool gate);
static SCG_SYSCLK_MODE_T CLOCK_ReadCurrentRunMode(void);
static SCG_SYSCLK_MODE_T CLOCK_ReadSysClockMode(const PWR_MODES_T mode);
static void CLOCK_ReadCurrentSysClkCfg(SCG_SYS_CLK_CONFIG_T * systemClockConfig);
static void CLOCK_ReadSystemClockCfg(SCG_SYSCLK_MODE_T systemClkMode, SCG_SYS_CLK_CONFIG_T *systemClkCfgPtr);
static SCG_SYS_CLOCK_SRC_T CLOCK_ReadSysClockSource(CLOCK_NAMES_T src);
static CLOCK_NAMES_T CLOCK_ReadDefaultModuleClkCfgSource(void);
static SCG_SYSOSC_EXT_REF_T CLOCK_ReadExtRef(XOSC_REF_T refClk);
static uint32_t CLOCK_ReadSrcFreq(SCG_SYS_CLOCK_SRC_T src);
static STATUS_T CLOCK_ReadFreq(CLOCK_NAMES_T peripheralClockName, uint32_t * freq);
static STATUS_T CLOCK_ReadScgClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency);
static uint32_t CLOCK_ReadSysAsyncFreq(CLOCK_NAMES_T clockSource, SCG_ASYNCLK_TYPE_T clockType);
static uint32_t CLOCK_ReadSysOscFreq(void);
static uint32_t CLOCK_ReadLsiFreq(void);
static uint32_t CLOCK_ReadHsiFreq(void);
static uint32_t CLOCK_ReadSysPllFreq(void);
static STATUS_T CLOCK_ReadSimClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency);
static STATUS_T CLOCK_ReadOtherSimClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency);
static STATUS_T CLOCK_ReadPcmClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency);
static uint32_t CLOCK_ReadPeripheralClockFreq(CLOCK_NAMES_T peripheralClockName, SCG_ASYNCLK_TYPE_T div);
static uint32_t CLOCK_ReadSystemClockFreq(SCG_SYSCLK_TYPE_T clockType);
static uint32_t CLOCK_ReadSimRtcClkFreq(void);
static uint32_t CLOCK_ReadLpoFreq(void);
static uint32_t CLOCK_ReadScgClkOutFreq(void);
static uint32_t CLOCK_ReadSimClkOutFreq(void);
static STATUS_T CLOCK_ReadCFGtmrOptionFreq(CLOCK_NAMES_T peripheralClockName, uint32_t * freq);
static STATUS_T CLOCK_CheckPCMClock(CLOCK_NAMES_T peripheralClockName);
static STATUS_T SIM_CheckPeripheralClockGate(CLOCK_NAMES_T peripheralClockName);
static SCG_ASYNCLK_DIV_T CLOCK_ConvertDividerValue(uint16_t div);


/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Install the pre-defined clock configuration table to the clock manager.
 *
 * @param clockConfigsPtr   Pointer to CLOCK_MANAGER_USER_CONFIG_T table.
 *
 * @param configsNumber   Number of clock configurations in table.
 *
 * @param callbacksPtr   Pointer to CLOCK_MANAGER_CALLBACK_USER_CONFIG_T table.
 *
 * @param callbacksNumber   Number of callback configurations in table.
 *
 * @retval STATUS_T Value.
 */
STATUS_T CLOCK_SYS_ClockManagerInit(CLOCK_MANAGER_USER_CONFIG_T const **clockConfigsPtr,
                                     uint8_t configsNumber,
                                     CLOCK_MANAGER_CALLBACK_USER_CONFIG_T **callbacksPtr,
                                      uint8_t callbacksNumber)
{
    g_clockState.cfgTable     = clockConfigsPtr;
    g_clockState.clockConfigNum  = configsNumber;
    g_clockState.callbackConfig  = callbacksPtr;
    g_clockState.callbackNum     = callbacksNumber;

    /*
     * errorCallbackIndex is the index of the callback which returns error
     * during clock mode switch. If all callbacks return success, then the
     * errorCallbackIndex is callbacksNumber.
     */
    g_clockState.errorCallbackIndex = callbacksNumber;

    return STATUS_SUCCESS;
}

/*!
 * @brief Set the system to target configuration, it
 * only sets the clock modules registers for clock mode change, but not send
 * notifications to drivers. This function is different by different SoCs.
 *
 * @param clockConfig   CLOCK_MANAGER_USER_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
STATUS_T CLOCK_SYS_Init(CLOCK_MANAGER_USER_CONFIG_T const * clockConfig)
{
    STATUS_T resultValue;
    CLOCK_MANAGER_USER_CONFIG_T clockConfig_default;
    CLOCK_MANAGER_USER_CONFIG_T const * clockCfg = clockConfig;

    if (POINTER_IS_NULL(clockConfig) == true)
    {
        /* Read default configuration */
        CLOCK_ReadDefaultConfig(&clockConfig_default);
        clockCfg = &clockConfig_default;
    }

    /* Set SCG settings. */
    resultValue = CLOCK_ConfigScgSetting(&clockCfg->scgClkCfg);

    if (STATUS_SUCCESS == resultValue)
    {
        /* Set SIM settings. */
        CLOCK_ConfigSimSetting(&clockCfg->simClkCfg);

        /* Set PMU settings. */
        CLOCK_ConfigPmuSetting(&clockCfg->pmuClkCfg);

        /* Set PCM settings. */
        CLOCK_ConfigPcmSetting(&clockCfg->pcmClkCfg);
    }

    return resultValue;
}

/*!
 * @brief Set system clock configuration according to pre-defined structure.
 *
 * @details This function sets system to target clock configuration; before transition,
 * clock manager will send notifications to all drivers registered to the
 * callback table.  When graceful policy is used, if some drivers are not ready
 * to change, clock transition will not occur, all drivers still work in
 * previous configuration and error is returned. When forceful policy is used,
 * all drivers should stop work and system changes to new clock configuration.
 * The function should be called only on run mode.
 *
 * @param targetConfigIdx   Clock configuration index.
 *
 * @param policy   CLOCK_MANAGER_POLICY_T Value.
 *
 * @retval STATUS_T Value.
 */
STATUS_T CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIdx, CLOCK_MANAGER_POLICY_T policy)
{
    uint8_t callbackIndex;
    bool setConfigSuccess;           /* SetConfiguraiton status */
    STATUS_T returnCode = STATUS_SUCCESS;
    STATUS_T callbackStatus = STATUS_SUCCESS;
    const CLOCK_MANAGER_CALLBACK_USER_CONFIG_T * callbackCfg;
    CLOCK_NOTIFY_STRUCT_T notifyStruct;


    notifyStruct.targetClkCfgIdx = targetConfigIdx;
    notifyStruct.policy                 = policy;

    /* Disable interrupts */
    INT_SYS_DisableIRQGlobal();
    /* Set errorcallbackindex as callbackNum, which means no callback error now.*/
    g_clockState.errorCallbackIndex = g_clockState.callbackNum;

    /* First step: Send "BEFORE" notification. */
    notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_BEFORE;

    /* Send notification to all callback. */
    for (callbackIndex = 0; callbackIndex < g_clockState.callbackNum; callbackIndex++)
    {
        callbackCfg = g_clockState.callbackConfig[callbackIndex];
        if ((callbackCfg) && (callbackCfg->callbackType != CLOCK_MANAGER_CALLBACK_AFTER))
        {
            if (STATUS_SUCCESS != (*callbackCfg->callback)(&notifyStruct, callbackCfg->callbackData))
            {
                g_clockState.errorCallbackIndex = callbackIndex;

                if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                {
                    /* Save the error callback index. */
                    returnCode = STATUS_NOTIFY_BEFORE_ERROR;
                    break;
                }
            }
        }
    }

    /* If all callback success or forceful policy is used. */
    if ((STATUS_SUCCESS == returnCode) || (policy == CLOCK_MANAGER_POLICY_FORCIBLE))
    {
        /* clock mode switch. */
        returnCode = CLOCK_SYS_Init(g_clockState.cfgTable[targetConfigIdx]);
        setConfigSuccess = (STATUS_SUCCESS == returnCode);

        g_clockState.curConfigIndex = targetConfigIdx;
    }
    else
    {
        /* Unsuccessful setConfiguration */
        setConfigSuccess = false;
    }

    if(setConfigSuccess != false)
    {
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_AFTER;

        for (callbackIndex = 0; callbackIndex < g_clockState.callbackNum; callbackIndex++)
        {
            callbackCfg = g_clockState.callbackConfig[callbackIndex];
            if ((callbackCfg) && (callbackCfg->callbackType != CLOCK_MANAGER_CALLBACK_BEFORE))
            {
                callbackStatus = (*callbackCfg->callback)(&notifyStruct, callbackCfg->callbackData);

            }
            if (STATUS_SUCCESS != callbackStatus)
            {
                g_clockState.errorCallbackIndex = callbackIndex;
                if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                {
                    /* Save the error callback index. */
                    returnCode = STATUS_NOTIFY_AFTER_ERROR;
                    break;
                }
            }
        }
    }
    else /* Error occurs, need to send "RECOVER" notification. */
    {
        if (0U != g_clockState.callbackNum)
        {
            notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_RECOVER;
            callbackIndex = (callbackIndex == g_clockState.callbackNum) ? (callbackIndex - 1U) : callbackIndex;
            for(;;)
            {
                callbackCfg = g_clockState.callbackConfig[callbackIndex];
                if (callbackCfg != NULL)
                {
                    (void)(*callbackCfg->callback)(&notifyStruct, callbackCfg->callbackData);
                }
                if(callbackIndex == 0U)
                {
                    break;
                }
                callbackIndex--;
            }
        }
    }

    /* Enable interrupts */
    INT_SYS_EnableIRQGlobal();

    return returnCode;
}

/*!
 * @brief Config a clock source's setting.
 *
 * @details  The clock source is configured based on the provided configuration.
 * All values from the previous configuration of clock source are overwritten.
 * If no configuration is provided, then a default one is used.
 *
 * @param clockSource   CLOCK_NAMES_T Value.
 *
 * @param clockSourceConfig   CLOCK_SRC_CONFIG_T pointer.
 *
 * @retval STATUS_T Value.
 */
STATUS_T CLOCK_SYS_ConfigClockSource(CLOCK_NAMES_T clockSource, const CLOCK_SRC_CONFIG_T * clockSourceConfig)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    bool clkSrcEnable = true;

    SCG_ASYNCLK_DIV_T div1, div2;

    if (POINTER_IS_NULL(clockSourceConfig) == true)
    {
        div1 = SCG_ASYNCLK_DIV_BY_1;
        div2 = SCG_ASYNCLK_DIV_BY_1;
    }
    else
    {
        div1 = CLOCK_ConvertDividerValue(clockSourceConfig->outputDiv1);
        div2 = CLOCK_ConvertDividerValue(clockSourceConfig->outputDiv2);
    }

    /* Check whether the command enables and disables the clock source */
    if ((POINTER_IS_NULL(clockSourceConfig) == true) && (clockSourceConfig->enable == false))
    {
        clkSrcEnable = false;
    }

    if (clockSource == SYSOSC_CLK)
    {
        returnCode = CLOCK_ConfigSoscClockSource(clkSrcEnable, clockSourceConfig, div1, div2);
    }
    else if (clockSource == LSICLK_CLK)
    {
        returnCode = CLOCK_ConfigLsiClockSource(clkSrcEnable, clockSourceConfig, div1, div2);
    }
    else if (clockSource == HSICLK_CLK)
    {
        returnCode = CLOCK_ConfigHsiClockSource(clkSrcEnable, clockSourceConfig, div1, div2);
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (clockSource == SYSPLL_CLK)
    {
        returnCode = CLOCK_ConfigSysPllClockSource(clkSrcEnable, clockSourceConfig, div1, div2);
    }
#endif
    else if (clockSource == SIM_LPO_CLK)
    {
        if (clkSrcEnable)
        {
            SIM_HW_ConfigLpoClocks(SIM,true,true,SIM_LPOCLK_SEL_LPO_128K,SIM_RTCCLK_SEL_SYSOSCDIV1_CLK);
            PMU_HW_ConfigLpoDisable(PMU, true);
        }
        else
        {
            PMU_HW_ConfigLpoDisable(PMU, false);
        }
    }
    else
    {}

    return returnCode;
}

/*!
 * @brief Config the system clocks (core, bus and flash clocks).
 *
 * @param mode   PWR_MODES_T pointer.
 *
 * @param systemClockConfig   SYS_CLK_CONFIG_T pointer.
 *
 * @retval STATUS_T Value.
 */
STATUS_T CLOCK_SYS_ConfigSystemClock(const PWR_MODES_T * mode, const SYS_CLK_CONFIG_T * systemClockConfig)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    SCG_SYSCLK_MODE_T systemClockMode, currentSystemClockMode;
    SCG_SYS_CLK_CONFIG_T sysClkCfg;

    const SCG_SYS_CLOCK_DIV_T sysClkDivMappings[] = {SCG_SYS_CLOCK_DIV_BY_1,SCG_SYS_CLOCK_DIV_BY_1,SCG_SYS_CLOCK_DIV_BY_2,SCG_SYS_CLOCK_DIV_BY_3,SCG_SYS_CLOCK_DIV_BY_4,
                                                     SCG_SYS_CLOCK_DIV_BY_5,SCG_SYS_CLOCK_DIV_BY_6,SCG_SYS_CLOCK_DIV_BY_7,SCG_SYS_CLOCK_DIV_BY_8,SCG_SYS_CLOCK_DIV_BY_9,
                                                     SCG_SYS_CLOCK_DIV_BY_10,SCG_SYS_CLOCK_DIV_BY_11,SCG_SYS_CLOCK_DIV_BY_12,SCG_SYS_CLOCK_DIV_BY_13,SCG_SYS_CLOCK_DIV_BY_14,
                                                     SCG_SYS_CLOCK_DIV_BY_15,SCG_SYS_CLOCK_DIV_BY_16};

    currentSystemClockMode = CLOCK_ReadCurrentRunMode();

    /* The power mode is not provided, the current one will be used by default. */
    if (POINTER_IS_NULL(mode) == true)
    {
        systemClockMode = currentSystemClockMode;
    }
    else
    {
        systemClockMode = CLOCK_ReadSysClockMode(*mode);
    }

    /* System clock configuration is not provided, a default one will be used. */
    if (POINTER_IS_NULL(systemClockConfig) == true)
    {
        /* Find a valid clock source. */
        CLOCK_ReadSystemClockCfg(systemClockMode,&sysClkCfg);
    }
    else
    {
        sysClkCfg.divCore = sysClkDivMappings[systemClockConfig->dividers[0U]];
        sysClkCfg.divBus  = sysClkDivMappings[systemClockConfig->dividers[1U]];
        sysClkCfg.divSlow = sysClkDivMappings[systemClockConfig->dividers[2U]];
        sysClkCfg.src = CLOCK_ReadSysClockSource(systemClockConfig->src);
    }

    /* System clock is configured in the current mode,
     * set configuration and wait until the system clock is changed. */
    if (systemClockMode == currentSystemClockMode)
    {
        returnValue = CLOCK_ChangeSystemClock(&sysClkCfg);
    }
    /* System clock is not configured in the current mode, just set configuration. */
    else
    {
        returnValue = CLOCK_ConfigSystemClockCfg(systemClockMode,&sysClkCfg);
    }
    return returnValue;
}


/*!
 * @brief Config a peripheral module clock.
 *
 * @details  This function configures a module clock according to the configuration.
 * If no configuration is provided (moduleClkConfig is null), then a default one is used
 * moduleClkConfig must be passed as null when module doesn't support protocol clock.
 * @param peripheralClock   CLOCK_NAMES_T Value.
 *
 * @param moduleClockConfig   MODULE_CLK_CONFIG_T pointer.
 *
 * @retval None.
 */
void CLOCK_SYS_ConfigModuleClock(CLOCK_NAMES_T peripheralClock, const MODULE_CLK_CONFIG_T * moduleClockConfig)
{
    uint32_t src = 0U, div = 0U, mul = 0U;
    MODULE_CLK_CONFIG_T defaultModuleClockConfig;
    const MODULE_CLK_CONFIG_T * moduleClkCfg;

    /* Configuration is not provided, a default one will be used. */
    if (POINTER_IS_NULL(moduleClockConfig) == true)
    {
        defaultModuleClockConfig.gating = true;
        defaultModuleClockConfig.mul = 1U;
        defaultModuleClockConfig.div = 1U;
        defaultModuleClockConfig.src = CLOCK_ReadDefaultModuleClkCfgSource();
        moduleClkCfg = &defaultModuleClockConfig;
    }
    else
    {
        moduleClkCfg = moduleClockConfig;
    }

    /* Check that clock gating is configurable from PCM */
    if ((g_peripheralFeaturesList[peripheralClock] & WITH_CLOCK_GATING_IN_SIM) == 0U)
    {
        /* Check that protocol clock is supported by the peripheral corresponding to the clock name */
        if ((g_peripheralFeaturesList[peripheralClock] & (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_PROTOCOL_CLOCK_FROM_ASYNC2)) != 0U)
        {
            if (moduleClkCfg->src == LSICLK_CLK)
            {
                src = CLK_SRC_LSI;
            }
            else if (moduleClkCfg->src == HSICLK_CLK)
            {
                src = CLK_SRC_HSI;
            }
            else if (moduleClkCfg->src == SYSOSC_CLK)
            {
                src = CLK_SRC_SYSOSC;
            }
#if FEATURE_WITH_SYSPLL_CLK
            else if (moduleClkCfg->src == SYSPLL_CLK)
            {
                src = CLK_SRC_SYSPLL;
            }
#endif
            else
            {
                src = CLK_SRC_OFF;
            }
        }

        /* Check that multiplier is supported by the peripheral corresponding to the clock name */
        if ((g_peripheralFeaturesList[peripheralClock] & WITH_MULTIPLIER) != 0U)
        {
            mul = ((uint32_t)moduleClkCfg->mul) - 1U;
        }

        /* Check that divider is supported by the peripheral corresponding to the clock name */
        if ((g_peripheralFeaturesList[peripheralClock] & WITH_DIVIDER) != 0U)
        {
            div = ((uint32_t)moduleClkCfg->div) - 1U;
        }

        /* Disable the peripheral clock */
        PCM_HW_ConfigPeripheralClkEn(PCM, peripheralClock, false);

        if (moduleClkCfg->gating)
        {
            /* Set peripheral clock control */
            PCM_HW_ConfigPeripheralClockControl(PCM, peripheralClock, true, src, div, mul);
        }
    }
    else
    {
        CLOCK_ConfigClockGate(peripheralClock, moduleClkCfg->gating);
    }
}

/*!
 * @brief Read the current system clock source
 *
 * @param systemClockConfig   SYS_CLK_CONFIG_T pointer.
 *
 * @retval None.
 */
void CLOCK_SYS_ReadSystemClockSource(SYS_CLK_CONFIG_T *systemClockConfig)
{
    if (SCG_SYS_CLOCK_SRC_SYS_OSC == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SYSOSC_CLK;
    }
    else if (SCG_SYS_CLOCK_SRC_LSICLK == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = LSICLK_CLK;
    }
    else if (SCG_SYS_CLOCK_SRC_HSICLK == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = HSICLK_CLK;
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (SCG_SYS_CLOCK_SRC_SYS_PLL == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SYSPLL_CLK;
    }
#endif
    else
    {
        /* Invalid system clock source. */
        systemClockConfig->src = LSICLK_CLK;
    }

    /* Core divider */
    systemClockConfig->dividers[0U] = (uint16_t) (SCG_HW_ReadCurrentCoreClockDividerRatio(SCG) + 1U);

    /* Bus divider */
    systemClockConfig->dividers[1U] = (uint16_t) (SCG_HW_ReadCurrentBusClockDividerRatio(SCG) + 1U);

    /* Slow divider */
    systemClockConfig->dividers[2U] = (uint16_t) (SCG_HW_ReadCurrentSlowClockDividerRatio(SCG) + 1U);
}

/*!
 * @brief Read current clock configuration index.
 *
 * @param None.
 *
 * @retval The current clock configuration index.
 */
uint8_t CLOCK_SYS_ReadCurrentClockConfigIndex(void)
{
    return g_clockState.curConfigIndex;
}

/*!
 * @brief Read the callback which returns error in last clock switch.
 *
 * @details  When graceful policy is used, if some IP is not ready to change clock
 * setting, the callback will return error and system stay in current
 * configuration. Applications can use this function to check which
 * IP callback returns error.
 *
 * @param None.
 *
 * @retval CLOCK_MANAGER_CALLBACK_USER_CONFIG_T pointer.
 */
CLOCK_MANAGER_CALLBACK_USER_CONFIG_T *CLOCK_SYS_ReadErrorCallback(void)
{
    CLOCK_MANAGER_CALLBACK_USER_CONFIG_T *returnValue;

    /* If all callbacks return success. */
    if (g_clockState.errorCallbackIndex >= g_clockState.clockConfigNum)
    {
        returnValue = NULL;
    }
    else
    {
        returnValue = g_clockState.callbackConfig[g_clockState.errorCallbackIndex];
    }
    return returnValue;
}

/*!
 * @brief Wrapper over CLOCK_ReadFreq function.
 *
 * @param clockName  is the name of the peripheral clock.
 *
 * @param frequency  returned clock frequency value in Hertz
 *
 * @retval STATUS_T Code.
 */
STATUS_T CLOCK_SYS_ReadFreq(CLOCK_NAMES_T clockName, uint32_t *frequency)
{
    return CLOCK_ReadFreq(clockName,frequency);
}


/*******************************************************************************
                        PRIVATE FUNCTION
*******************************************************************************/

/*!
 * @brief Read the system to a default defaultClockCfguration, it
 * only Read the clock modules registers for clock mode change, but not send
 * notifications to drivers.
 *
 * @param defaultClockCfg        CLOCK_MANAGER_USER_CONFIG_T pointer
 *
 * @retval  None.
 */
static void CLOCK_ReadDefaultConfig(CLOCK_MANAGER_USER_CONFIG_T * defaultClockCfg)
{
    uint32_t i = 0U;
    PERIPHERAL_CLOCK_CONFIG_T peripheralClkCfg[CLOCK_PERIPHERALS_COUNT] = {
#ifdef PCM_COMP_INDEX
        {
            .clkName        = COMP_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CRC_INDEX
        {
            .clkName        = CRC_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_DMACHM_INDEX
        {
            .clkName        = DMACHM_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_EWDT_INDEX
        {
            .clkName        = EWDT_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PMA_INDEX
        {
            .clkName        = PMA_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PMB_INDEX
        {
            .clkName        = PMB_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PMC_INDEX
        {
            .clkName        = PMC_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PMD_INDEX
        {
            .clkName        = PMD_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PME_INDEX
        {
            .clkName        = PME_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_RTC_INDEX
        {
            .clkName        = RTC_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif

#ifdef PCM_CAN0_INDEX
        {
            .clkName        = CAN0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CAN1_INDEX
        {
            .clkName        = CAN1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CAN2_INDEX
        {
            .clkName        = CAN2_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PDU0_INDEX
        {
            .clkName        = PDU0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_PDU1_INDEX
        {
            .clkName        = PDU1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif

#ifdef PCM_FLASH_INDEX
        {
            .clkName        = FLASH_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_OFF,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR0_INDEX
        {
            .clkName        = CFGTMR0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR1_INDEX
        {
            .clkName        = CFGTMR1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR2_INDEX
        {
            .clkName        = CFGTMR2_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR3_INDEX
        {
            .clkName        = CFGTMR3_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_ADC0_INDEX
        {
            .clkName        = ADC0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_ADC1_INDEX
        {
            .clkName        = ADC1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_IO_INDEX
        {
            .clkName        = CFGIO_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPI2C_INDEX
        {
            .clkName        = LPI2C_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPITMR_INDEX
        {
            .clkName        = LPITMR_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPSPI0_INDEX
        {
            .clkName        = LPSPI0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPSPI1_INDEX
        {
            .clkName        = LPSPI1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPSPI2_INDEX
        {
            .clkName        = LPSPI2_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPTMR_INDEX
        {
            .clkName        = LPTMR_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPUART0_INDEX
        {
            .clkName        = LPUART0_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPUART1_INDEX
        {
            .clkName        = LPUART1_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_LPUART2_INDEX
        {
            .clkName        = LPUART2_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR4_INDEX
        {
            .clkName        = CFGTMR4_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR5_INDEX
        {
            .clkName        = CFGTMR5_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR6_INDEX
        {
            .clkName        = CFGTMR6_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif
#ifdef PCM_CFGTMR7_INDEX
        {
            .clkName        = CFGTMR7_CLK,
            .clkGate        = true,
            .clkSrc         = CLK_SRC_LSI_DIV1,
            .frac           = MUL_BY_ONE,
            .div            = DIV_BY_ONE,
        },
#endif

    };

    /* SCG */
    defaultClockCfg->scgClkCfg.sysOscCfg.init                     = true;
    defaultClockCfg->scgClkCfg.sysOscCfg.freq                     = 8000000U;
    /* SYSOSCCSR */
    defaultClockCfg->scgClkCfg.sysOscCfg.monitorMode              = SCG_SYSOSC_MONITOR_DISABLE;
    defaultClockCfg->scgClkCfg.sysOscCfg.locked                   = false;
    /* SYSOSCCFG */
    defaultClockCfg->scgClkCfg.sysOscCfg.extRef                   = SCG_SYSOSC_REF_OSC;
    defaultClockCfg->scgClkCfg.sysOscCfg.gain                     = SCG_SYSOSC_GAIN_LOW;
    defaultClockCfg->scgClkCfg.sysOscCfg.range                    = SCG_SYSOSC_RANGE_MID;
    /* SYSOSCDIV */
    defaultClockCfg->scgClkCfg.sysOscCfg.div1                     = SCG_ASYNCLK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.sysOscCfg.div2                     = SCG_ASYNCLK_DIV_BY_1;

    /* SYSPLL */
    defaultClockCfg->scgClkCfg.sysPllCfg.init                     = true;
    /* SYSPLLCSR */
    defaultClockCfg->scgClkCfg.sysPllCfg.monitorMode              = SCG_SYSPLL_MONITOR_DISABLE;
    defaultClockCfg->scgClkCfg.sysPllCfg.locked                   = false;
    /* SYSPLLCFG */
    defaultClockCfg->scgClkCfg.sysPllCfg.prediv                   = SCG_SYSPLL_CLK_PREDIV_BY_1;
    defaultClockCfg->scgClkCfg.sysPllCfg.mul                      = SCG_SYSPLL_CLK_MUL_BY_28;
    defaultClockCfg->scgClkCfg.sysPllCfg.src                      = 0U;
    /* SYSPLLDIV */
    defaultClockCfg->scgClkCfg.sysPllCfg.div1                     = SCG_ASYNCLK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.sysPllCfg.div2                     = SCG_ASYNCLK_DIV_BY_1;

    /* LSI */
    defaultClockCfg->scgClkCfg.lsiCfg.init                     = true;
    /* LSICLKCSR */
    defaultClockCfg->scgClkCfg.lsiCfg.enableInStop             = false;
    defaultClockCfg->scgClkCfg.lsiCfg.enableInLowPower         = true;
    defaultClockCfg->scgClkCfg.lsiCfg.locked                   = false;
    /* LSIFSEL */
    defaultClockCfg->scgClkCfg.lsiCfg.range                    = SCG_LSICLK_RANGE_HIGH;
    /* LSIDIV */
    defaultClockCfg->scgClkCfg.lsiCfg.div1                     = SCG_ASYNCLK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.lsiCfg.div2                     = SCG_ASYNCLK_DIV_BY_1;

    /* HSI */
    defaultClockCfg->scgClkCfg.hsiCfg.init                     = true;
     /* HSICLKCSR */
    defaultClockCfg->scgClkCfg.hsiCfg.regulator                = true;
    defaultClockCfg->scgClkCfg.hsiCfg.locked                   = false;
    /* HSIFSEL */
    defaultClockCfg->scgClkCfg.hsiCfg.range                    = SCG_HSICLK_RANGE_48M;
    /* HSIDIV */
    defaultClockCfg->scgClkCfg.hsiCfg.div1                     = SCG_ASYNCLK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.hsiCfg.div2                     = SCG_ASYNCLK_DIV_BY_1;


    defaultClockCfg->scgClkCfg.rtcCfg.init                      = true;
    defaultClockCfg->scgClkCfg.rtcCfg.rtcClkInFreq              = 0U;

    defaultClockCfg->scgClkCfg.clkOutCfg.init                   = true;
    defaultClockCfg->scgClkCfg.clkOutCfg.src                    = SCG_CLKOUT_SRC_HSICLK;

    defaultClockCfg->scgClkCfg.clockModeCfg.init                = true;

    /*  VCCR - VLPR Clock Control Register        */
    defaultClockCfg->scgClkCfg.clockModeCfg.vcCtrCfg.src      = SCG_SYS_CLOCK_SRC_LSICLK;
    defaultClockCfg->scgClkCfg.clockModeCfg.vcCtrCfg.divCore  = SCG_SYS_CLOCK_DIV_BY_2;
    defaultClockCfg->scgClkCfg.clockModeCfg.vcCtrCfg.divBus   = SCG_SYS_CLOCK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.clockModeCfg.vcCtrCfg.divSlow  = SCG_SYS_CLOCK_DIV_BY_4;

#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
    /*  HCCR - HSRUN Clock Control Register        */
    defaultClockCfg->scgClkCfg.clockModeCfg.hcCtrCfg.src      = SCG_SYS_CLOCK_SRC_SYS_PLL;
    defaultClockCfg->scgClkCfg.clockModeCfg.hcCtrCfg.divCore  = SCG_SYS_CLOCK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.clockModeCfg.hcCtrCfg.divBus   = SCG_SYS_CLOCK_DIV_BY_2;
    defaultClockCfg->scgClkCfg.clockModeCfg.hcCtrCfg.divSlow  = SCG_SYS_CLOCK_DIV_BY_4;
#endif

    /*  RCCR - Run Clock Control Register          */
    defaultClockCfg->scgClkCfg.clockModeCfg.rcCtrCfg.src      = SCG_SYS_CLOCK_SRC_HSICLK;
    defaultClockCfg->scgClkCfg.clockModeCfg.rcCtrCfg.divCore  = SCG_SYS_CLOCK_DIV_BY_1;
    defaultClockCfg->scgClkCfg.clockModeCfg.rcCtrCfg.divBus   = SCG_SYS_CLOCK_DIV_BY_2;
    defaultClockCfg->scgClkCfg.clockModeCfg.rcCtrCfg.divSlow  = SCG_SYS_CLOCK_DIV_BY_2;

    /* PCM */
    defaultClockCfg->pcmClkCfg.peripheralClocks                    = peripheralClkCfg;
    defaultClockCfg->pcmClkCfg.count                               = CLOCK_PERIPHERALS_COUNT;

    /* SIM */
    /*  Low Power Clock defaultClockCfguration.     */
    defaultClockCfg->simClkCfg.lpoClkCfg.init                 = true;
    defaultClockCfg->simClkCfg.lpoClkCfg.enableLpo1k          = true;
    defaultClockCfg->simClkCfg.lpoClkCfg.enableLpo32k         = true;
    defaultClockCfg->simClkCfg.lpoClkCfg.srcLpoClk            = SIM_LPOCLK_SEL_LPO_128K;
    defaultClockCfg->simClkCfg.lpoClkCfg.srcRtcClk            = SIM_RTCCLK_SEL_SYSOSCDIV1_CLK;
    /*  Clock Out defaultClockCfguration.           */
    defaultClockCfg->simClkCfg.clkOutCfg.init                 = true;
    defaultClockCfg->simClkCfg.clkOutCfg.enable               = false;
    defaultClockCfg->simClkCfg.clkOutCfg.src                  = SIM_CLKOUT_SEL_SYS_SCG_CLKOUT;
    defaultClockCfg->simClkCfg.clkOutCfg.div                  = SIM_CLKOUT_DIV_BY_1;
    /*  Quad Spi Internal Reference Clock Gating. */
    defaultClockCfg->simClkCfg.qspiRefClkGating.enableQspiRefClk   = true;
    /*  TCLK CLOCK defaultClockCfguration. */
    defaultClockCfg->simClkCfg.tclkCfg.init                     = true;
    defaultClockCfg->simClkCfg.tclkCfg.tclkFreq[0]              = 0U;
    defaultClockCfg->simClkCfg.tclkCfg.tclkFreq[1]              = 0U;
    defaultClockCfg->simClkCfg.tclkCfg.tclkFreq[2]              = 0U;
    for (i = 0; i < CFGTMR_INSTANCE_COUNT; i++)
    {
        defaultClockCfg->simClkCfg.tclkCfg.extPinSrc[i]              = 0U;
    }
    /*  Platform Gate Clock defaultClockCfguration. */
    defaultClockCfg->simClkCfg.platGateClkCfg.init                 = true;
    defaultClockCfg->simClkCfg.platGateClkCfg.enableMsc            = true;
    defaultClockCfg->simClkCfg.platGateClkCfg.enableMpu            = true;
    defaultClockCfg->simClkCfg.platGateClkCfg.enableDma            = true;
    defaultClockCfg->simClkCfg.platGateClkCfg.enableErep           = true;
    defaultClockCfg->simClkCfg.platGateClkCfg.enableEinj           = true;
     /*  Debug trace Clock Configuration. */
    defaultClockCfg->simClkCfg.traceClkCfg.init               = true;
    defaultClockCfg->simClkCfg.traceClkCfg.divEnable          = true;
    defaultClockCfg->simClkCfg.traceClkCfg.src                = CLOCK_TRACE_SRC_CORE_CLK;
    defaultClockCfg->simClkCfg.traceClkCfg.div                = 0U;
    defaultClockCfg->simClkCfg.traceClkCfg.divFraction        = false;

    /* PMU */
    /*  Low Power Clock defaultClockCfguration.     */
    defaultClockCfg->pmuClkCfg.lpoClkCfg.init                 = true;
    defaultClockCfg->pmuClkCfg.lpoClkCfg.enable               = true;
    defaultClockCfg->pmuClkCfg.lpoClkCfg.trimValue            = 0;
}


/*!
 * @brief config the SCG blocks
 *
 * @param scgCfg   SCG_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigScgSetting(const SCG_CONFIG_T * scgCfg)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    g_rtcClkInFreq = 0U; /* reset the value RTC_clk frequency. */

    if (POINTER_IS_NULL(scgCfg) == false)
    {
        /* Configure a temporary system clock source: HSICLK */
        statusValue = CLOCK_ConfigTemporarySystemClock();

        if (statusValue == STATUS_SUCCESS)
        {
            /* Configure clock sources from SCG */
            statusValue = CLOCK_ConfigModulesFromScg(scgCfg);
        }

        if (statusValue == STATUS_SUCCESS)
        {

            /* Configure SCG clock modes. */
            if (scgCfg->clockModeCfg.init)
            {
                /* Configure SCG clock modes */
                statusValue = CLOCK_ConfigSystemClockCfg(SCG_SYSCLK_MODE_RUN, &(scgCfg->clockModeCfg.rcCtrCfg));
#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
                if (statusValue == STATUS_SUCCESS)
                {
                    statusValue = CLOCK_ConfigSystemClockCfg(SCG_SYSCLK_MODE_HSRUN, &(scgCfg->clockModeCfg.hcCtrCfg));
                }
#endif
                if (statusValue == STATUS_SUCCESS)
                {
                    statusValue = CLOCK_ConfigSystemClockCfg(SCG_SYSCLK_MODE_VLPR, &(scgCfg->clockModeCfg.vcCtrCfg));
                }
            }

            if (scgCfg->rtcCfg.init)
            {
                /* RTC Clock settings. */
                g_rtcClkInFreq = scgCfg->rtcCfg.rtcClkInFreq;
            }

            /* Configure SCG ClockOut. */
            if (scgCfg->clkOutCfg.init)
            {
                /* ClockOut settings. */
                SCG_HW_ConfigClockoutSourceSel(SCG, (uint32_t)scgCfg->clkOutCfg.src);
            }
        }
    }

    return statusValue;
}

/*!
 * @brief config the PCM block.
 *
 * @param peripheralClkCfg     PCM_CONFIG_T pointer
 *
 * @retval  None.
 */
static void CLOCK_ConfigPcmSetting(const PCM_CONFIG_T * peripheralClkCfg)
{
    uint32_t i;

    if ((POINTER_IS_NULL(peripheralClkCfg) == false) && (POINTER_IS_NULL(peripheralClkCfg->peripheralClocks) == false))
    {
        for (i = 0U; i < peripheralClkCfg->count; i++)
        {
            /* Disable the peripheral clock */
            PCM_HW_ConfigPeripheralClkEn(PCM, peripheralClkCfg->peripheralClocks[i].clkName, false);

            /* Set peripheral clock control */
            PCM_HW_ConfigPeripheralClockControl(PCM,
                                          peripheralClkCfg->peripheralClocks[i].clkName,
                                          peripheralClkCfg->peripheralClocks[i].clkGate,
                                          (uint32_t)peripheralClkCfg->peripheralClocks[i].clkSrc,
                                          (uint32_t)peripheralClkCfg->peripheralClocks[i].div,
                                          (uint32_t)peripheralClkCfg->peripheralClocks[i].frac);
        }
    }
}

/*!
 * @brief config the SIM block.
 *
 * @param simClkCfg     SIM_CLOCK_CONFIG_T pointer
 *
 * @retval  None.
 */
static void CLOCK_ConfigSimSetting(const SIM_CLOCK_CONFIG_T * simClkCfg)
{
    uint8_t index;

    /* ClockOut settings. */
    if (simClkCfg->clkOutCfg.init)
    {
        SIM_HW_ConfigClockout(SIM, simClkCfg->clkOutCfg.enable, simClkCfg->clkOutCfg.src, simClkCfg->clkOutCfg.div);
    }

    /* Platform Gate Clock settings. */
    if (simClkCfg->platGateClkCfg.init)
    {
        SIM_HW_ConfigMscmClockGate(SIM, simClkCfg->platGateClkCfg.enableMsc);
        SIM_HW_ConfigMpuClockGate(SIM, simClkCfg->platGateClkCfg.enableMpu);
        SIM_HW_ConfigDmaClockGate(SIM, simClkCfg->platGateClkCfg.enableDma);
        SIM_HW_SetErepClockGate(SIM, simClkCfg->platGateClkCfg.enableErep);
        SIM_HW_ConfigEinjClockGate(SIM, simClkCfg->platGateClkCfg.enableEinj);
    }

    /* Low Power Clock settings from SIM. */
    if (simClkCfg->lpoClkCfg.init)
    {
        SIM_HW_ConfigLpoClocks(SIM, simClkCfg->lpoClkCfg.enableLpo1k, simClkCfg->lpoClkCfg.enableLpo32k,
                         simClkCfg->lpoClkCfg.srcLpoClk, simClkCfg->lpoClkCfg.srcRtcClk);
    }

    /* Debug trace Clock settings. */
    if (simClkCfg->traceClkCfg.init)
    {
        SIM_HW_ClearTraceClockConfig(SIM);
        SIM_HW_ConfigTraceClockSource(SIM, simClkCfg->traceClkCfg.src);

        SIM_HW_ConfigTraceClock(SIM, false, 0U, 0U);

        if (simClkCfg->traceClkCfg.divEnable)
        {
            SIM_HW_ConfigTraceClock(SIM, simClkCfg->traceClkCfg.divEnable, simClkCfg->traceClkCfg.div, simClkCfg->traceClkCfg.divFraction);
        }
    }

    /* TCLK Clock settings. */
    if (simClkCfg->tclkCfg.init)
    {
        for (index = 0; index < NUMBER_OF_TCLK_INPUTS; index++)
        {
            g_tclkFreq[index] = simClkCfg->tclkCfg.tclkFreq[index];
        }

        /* CFGTMROPT0 clock settings */
        for (index = 0; index < CFGTMR_INSTANCE_COUNT; index++)
        {
            SIM_HW_ConfigExtPinSourceCFGtmr(SIM, index, simClkCfg->tclkCfg.extPinSrc[index]);
        }
    }
}

/*!
 * @brief config the PMU block.
 *
 * @param pmuClkCfg     PMU_CONFIG_T pointer
 *
 * @retval  None.
 */
static void CLOCK_ConfigPmuSetting(const PMU_CONFIG_T * pmuClkCfg)
{
    /* Low Power Clock settings from PMU. */
    if (pmuClkCfg->lpoClkCfg.init)
    {
        /* Write trimming value. */
        PMU_HW_ConfigLpoTrimValue(PMU, pmuClkCfg->lpoClkCfg.trimValue);

        /* Enable/disable the low power oscillator. */
        PMU_HW_ConfigLpoDisable(PMU, pmuClkCfg->lpoClkCfg.enable);
    }
}

/*!
 * @brief config all modules from SCG (LSICLK, HSICLK, SYSOSC and SYSPLL)
 *
 * @param scgConfig   SCG_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigModulesFromScg(const SCG_CONFIG_T * scgConfig)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T systemClockConfig;
    const SCG_SYS_CLK_CONFIG_T * nextSystemClockConfig;
    const SCG_SYS_CLOCK_DIV_T tempSystemClock[TEMP_SYS_CLK_INDEX][TEMP_SYS_DIV_INDEX] = TEMP_SYSTEM_CLOCK_CONFIGS;

    /* Configure all clock sources that are different from the
     * current system clock source HSICLK (LSICLK, SYSOSC, SYSPLL). */
    statusValue = CLOCK_ConfigSYSOSC(scgConfig->sysOscCfg.init, &scgConfig->sysOscCfg);
    if (statusValue == STATUS_SUCCESS)
    {
        statusValue = CLOCK_ConfigLSICLK(scgConfig->lsiCfg.init, &scgConfig->lsiCfg);
#if FEATURE_WITH_SYSPLL_CLK
        if (statusValue == STATUS_SUCCESS)
        {
            statusValue = CLOCK_ConfigSYSPLL(scgConfig->sysPllCfg.init,&scgConfig->sysPllCfg);
        }
#endif
    }

    /* Get the next system clock source */
    if (CLOCK_ReadCurrentRunMode() == SCG_SYSCLK_MODE_RUN)
    {
        nextSystemClockConfig = &scgConfig->clockModeCfg.rcCtrCfg;
    }
    else if (CLOCK_ReadCurrentRunMode() == SCG_SYSCLK_MODE_VLPR)
    {
        nextSystemClockConfig = &scgConfig->clockModeCfg.vcCtrCfg;
    }
    else if (CLOCK_ReadCurrentRunMode() == SCG_SYSCLK_MODE_HSRUN)
    {
        nextSystemClockConfig = &scgConfig->clockModeCfg.hcCtrCfg;
    }
    else
    {
        statusValue = STATUS_ERROR;
        nextSystemClockConfig = NULL;
    }

    if (statusValue == STATUS_SUCCESS)
    {
        /* The current system clock source is HSICLK.
         * Verify whether the next system clock source is HSICLK. */
        if (nextSystemClockConfig->src == SCG_SYS_CLOCK_SRC_HSICLK)
        {
            /* If they are the same, search for a temporary system clock source
             * (use one of the following sources: SYSOSC, SYSPLL, LSICLK)
             * Assume that a temporary clock is not found statusValue = ERROR. */
            statusValue = STATUS_ERROR;

            /* SYSOSC is enabled */
            if (scgConfig->sysOscCfg.init && (statusValue == STATUS_ERROR))
            {
                systemClockConfig.src     = SCG_SYS_CLOCK_SRC_SYS_OSC;
                systemClockConfig.divCore = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_SYS_DIV];
                systemClockConfig.divBus  = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_BUS_DIV];
                systemClockConfig.divSlow = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_SLOW_DIV];
                statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);
            }

#if FEATURE_WITH_SYSPLL_CLK
            /* SYSPLL is enabled and SYSOSC configuration for system clock source is not valid */
            if (scgConfig->sysPllCfg.init && (statusValue == STATUS_ERROR))
            {
                systemClockConfig.src     = SCG_SYS_CLOCK_SRC_SYS_PLL;
                systemClockConfig.divCore = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_SYS_DIV];
                systemClockConfig.divBus  = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_BUS_DIV];
                systemClockConfig.divSlow = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_SLOW_DIV];
                statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);
            }
#endif

            /* LSICLK is enabled and SYSPLL configuration for system clock source is not valid */
            if (scgConfig->lsiCfg.init && (statusValue == STATUS_ERROR))
            {
                systemClockConfig.src     = SCG_SYS_CLOCK_SRC_LSICLK;
                systemClockConfig.divCore = tempSystemClock[TEMP_LSICLK_CLK][TEMP_SYS_DIV];
                systemClockConfig.divBus  = tempSystemClock[TEMP_LSICLK_CLK][TEMP_BUS_DIV];
                systemClockConfig.divSlow = tempSystemClock[TEMP_LSICLK_CLK][TEMP_SLOW_DIV];
                statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);
            }

            /* Changed to a temporary system clock source. */
            if (statusValue == STATUS_SUCCESS)
            {
                /* Configure the remaining clock source (HSICLK). */
                statusValue = CLOCK_ConfigHSICLK(scgConfig->hsiCfg.init, &scgConfig->hsiCfg);

                if (statusValue == STATUS_SUCCESS)
                {
                    /* Change to the next system clock source. */
                    systemClockConfig.src     = nextSystemClockConfig->src;
                    systemClockConfig.divCore = nextSystemClockConfig->divCore;
                    systemClockConfig.divBus  = nextSystemClockConfig->divBus;
                    systemClockConfig.divSlow = nextSystemClockConfig->divSlow;
                    statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);
                }
            }
        }
        else
        {   /* Change to the next system clock source. */
            systemClockConfig.src     = nextSystemClockConfig->src;
            systemClockConfig.divCore = nextSystemClockConfig->divCore;
            systemClockConfig.divBus  = nextSystemClockConfig->divBus;
            systemClockConfig.divSlow = nextSystemClockConfig->divSlow;
            statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);

            if (statusValue == STATUS_SUCCESS)
            {
                /* Configure the remaining clock source (HSICLK) */
                statusValue = CLOCK_ConfigHSICLK(scgConfig->hsiCfg.init, &scgConfig->hsiCfg);
            }
        }
    }

    return statusValue;
}

/*!
 * @brief Config LSICLK clock.
 *
 * @param clockSourceEnable   Enable/Disable.
 *
 * @param clockSourceConfig   CLOCK_SRC_CONFIG_T pointer.
 *
 * @param div1   SCG_ASYNCLK_DIV_T Value.
 *
 * @param div2   SCG_ASYNCLK_DIV_T Value.
 *
 * @retval STATUS_T Value.
 */
static STATUS_T CLOCK_ConfigLsiClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T curSysClkCfg;
    bool revertTmpSysClkTransition = false;
    SCG_LSICLK_CONFIG_T scgLsiCfg;

    /* Check whether the current system clock source is LSICLK */
    if (((uint32_t)SCG_SYS_CLOCK_SRC_LSICLK) == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        if (clockSourceEnable == false)
        {
            /* Can't disable LSICLK, it's used as system clock source */
            returnCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration */
            CLOCK_ReadCurrentSysClkCfg(&curSysClkCfg);

            /* Transition to a temporary system clock source */
            returnCode = CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_LSICLK);

            revertTmpSysClkTransition = (returnCode == STATUS_SUCCESS) ? true : false;
        }
    }

    if (returnCode == STATUS_SUCCESS)
    {
        if (POINTER_IS_NULL(clockSourceConfig) == true)
        {
            returnCode = CLOCK_ConfigLSICLK(true, NULL);
        }
        else
        {
            /* Check VLP modes */
            scgLsiCfg.enableInLowPower  = true;
            scgLsiCfg.enableInStop  = true;
            scgLsiCfg.locked            = false;

            scgLsiCfg.div1              = div1;
            scgLsiCfg.div2              = div2;

            scgLsiCfg.range             = SCG_LSICLK_RANGE_HIGH;

            returnCode = CLOCK_ConfigLSICLK(clockSourceEnable, &scgLsiCfg);
        }

        /* If system clock source was LSICLK and LSICLK has been set successfully,
         * then transition back to LSICLK */
        if ((returnCode == STATUS_SUCCESS) && (revertTmpSysClkTransition == true))
        {
            returnCode = CLOCK_ChangeSystemClock(&curSysClkCfg);
        }
    }

    return returnCode;
}

/*!
 * @brief Config HSICLK clock.
 *
 * @param clockSourceEnable   Enable/Disable.
 *
 * @param clockSourceConfig   CLOCK_SRC_CONFIG_T pointer.
 *
 * @param div1   SCG_ASYNCLK_DIV_T Value.
 *
 * @param div2   SCG_ASYNCLK_DIV_T Value.
 *
 * @retval STATUS_T Value.
 */
static STATUS_T CLOCK_ConfigHsiClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T curSysClkCfg;
    bool revertTmpSysClkTransition = false;
    SCG_HSICLK_CONFIG_T scgHsiCfg;

    /* Check whether the current system clock source is HSICLK */
    if (((uint32_t)SCG_SYS_CLOCK_SRC_HSICLK) == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        if (clockSourceEnable == false)
        {
            /* Can't disable HSICLK, it's used as system clock source. */
            returnCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CLOCK_ReadCurrentSysClkCfg(&curSysClkCfg);

            /* Transition to a temporary system clock source. */
            returnCode = CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_HSICLK);

            revertTmpSysClkTransition = (returnCode == STATUS_SUCCESS) ? true : false;
        }
    }

    if (returnCode == STATUS_SUCCESS)
    {
        if (POINTER_IS_NULL(clockSourceConfig) == true)
        {
            returnCode = CLOCK_ConfigHSICLK(clockSourceEnable, NULL);
        }
        else
        {
            scgHsiCfg.locked            = false;

            scgHsiCfg.div1              = div1;
            scgHsiCfg.div2              = div2;

            scgHsiCfg.range             = SCG_HSICLK_RANGE_48M;
            scgHsiCfg.regulator         = true;

            returnCode = CLOCK_ConfigHSICLK(clockSourceEnable, &scgHsiCfg);
        }

        /* If system clock source was HSICLK and HSICLK has been set successfully,
         * then transition back to HSICLK */
        if ((returnCode == STATUS_SUCCESS) && (revertTmpSysClkTransition == true))
        {
            returnCode = CLOCK_ChangeSystemClock(&curSysClkCfg);
        }
    }

    return returnCode;
}


/*!
 * @brief Config SYSOSC clock.
 *
 * @param clockSourceEnable   Enable/Disable.
 *
 * @param clockSourceConfig   CLOCK_SRC_CONFIG_T pointer.
 *
 * @param div1   SCG_ASYNCLK_DIV_T Value.
 *
 * @param div2   SCG_ASYNCLK_DIV_T Value.
 *
 * @retval STATUS_T Value.
 */
static STATUS_T CLOCK_ConfigSoscClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T curSysClkCfg;
    bool revertTmpSysClkTransition = false;
    SCG_SYSOSC_CONFIG_T scgSoscCfg;

    /* Check whether the current system clock source is SYSOSC */
    if (((uint32_t)SCG_SYS_CLOCK_SRC_SYS_OSC) == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        if (clockSourceEnable == false)
        {
            /* Can't disable SYSOSC, it's used as system clock source. */
            returnCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CLOCK_ReadCurrentSysClkCfg(&curSysClkCfg);

            /* Transition to a temporary system clock source. */
            returnCode = CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_SYS_OSC);

            revertTmpSysClkTransition = (returnCode == STATUS_SUCCESS) ? true : false;
        }
    }

    if (returnCode == STATUS_SUCCESS)
    {
        if (POINTER_IS_NULL(clockSourceConfig) == true)
        {
            returnCode = CLOCK_ConfigSYSOSC(clockSourceEnable, NULL);
        }
        else
        {
            scgSoscCfg.freq              = clockSourceConfig->refFreq;

            scgSoscCfg.monitorMode       = SCG_SYSOSC_MONITOR_DISABLE;
            scgSoscCfg.locked            = false;

            scgSoscCfg.div1              = div1;
            scgSoscCfg.div2              = div2;

            scgSoscCfg.extRef            = CLOCK_ReadExtRef(clockSourceConfig->refClk);

            scgSoscCfg.gain              = SCG_SYSOSC_GAIN_LOW;

            if((scgSoscCfg.freq >= 4000000U) && (scgSoscCfg.freq <= 8000000U))
            {
                scgSoscCfg.range             = SCG_SYSOSC_RANGE_MID;
            }
            else if((scgSoscCfg.freq >= 8000000U) && (scgSoscCfg.freq <= 40000000U))
            {
                scgSoscCfg.range             = SCG_SYSOSC_RANGE_HIGH;
            }
            else
            {
                /* Frequency of the input reference clock is invalid. */
            }

            returnCode = CLOCK_ConfigSYSOSC(clockSourceEnable, &scgSoscCfg);
        }

        /* If system clock source was SYSOSC and SYSOSC has been set successfully,
         * then transition back to SYSOSC */
        if ((returnCode == STATUS_SUCCESS) && (revertTmpSysClkTransition == true))
        {
            returnCode = CLOCK_ChangeSystemClock(&curSysClkCfg);
        }
    }

    return returnCode;
}

#if FEATURE_WITH_SYSPLL_CLK
/*!
 * @brief Config SYSPLL clock.
 *
 * @param clockSourceEnable   Enable/Disable.
 *
 * @param clockSourceConfig   CLOCK_SRC_CONFIG_T pointer.
 *
 * @param div1   SCG_ASYNCLK_DIV_T Value.
 *
 * @param div2   SCG_ASYNCLK_DIV_T Value.
 *
 * @retval STATUS_T Value.
 */
static STATUS_T CLOCK_ConfigSysPllClockSource(bool clockSourceEnable,
                                            const CLOCK_SRC_CONFIG_T * clockSourceConfig,
                                            SCG_ASYNCLK_DIV_T div1,
                                            SCG_ASYNCLK_DIV_T div2)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T curSysClkCfg;
    bool revertTmpSysClkTransition = false;
    SCG_SYSPLL_CONFIG_T scgSysPllCfg;

    /* Check whether the current system clock source is PLL */
    if (((uint32_t)SCG_SYS_CLOCK_SRC_SYS_PLL) == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        if (clockSourceEnable == false)
        {
            /* Can't disable PLL, it's used as system clock source. */
            returnCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CLOCK_ReadCurrentSysClkCfg(&curSysClkCfg);

            /* Transition to a temporary system clock source. */
            returnCode = CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_SYS_PLL);

            revertTmpSysClkTransition = (returnCode == STATUS_SUCCESS) ? true : false;
        }
    }

    if (returnCode == STATUS_SUCCESS)
    {
        if (POINTER_IS_NULL(clockSourceConfig) == true)
        {
            returnCode = CLOCK_ConfigSYSPLL(clockSourceEnable, NULL);
        }
        else
        {
            scgSysPllCfg.monitorMode       = SCG_SYSPLL_MONITOR_DISABLE;
            scgSysPllCfg.locked            = false;
            scgSysPllCfg.div1              = div1;
            scgSysPllCfg.div2              = div2;
            scgSysPllCfg.prediv = (uint8_t)(clockSourceConfig->div - SCG_SYSPLL_PREDIV_BASE);
            scgSysPllCfg.mul = (uint8_t)(clockSourceConfig->mul - SCG_SYSPLL_MULT_BASE);

            if (returnCode == STATUS_SUCCESS)
            {
                returnCode = CLOCK_ConfigSYSPLL(clockSourceEnable, &scgSysPllCfg);
            }
        }

        /* If system clock source was PLL and PLL has been set successfully,
         * then transition back to PLL */
        if ((returnCode == STATUS_SUCCESS) && (revertTmpSysClkTransition == true))
        {
            returnCode = CLOCK_ChangeSystemClock(&curSysClkCfg);
        }
    }

    return returnCode;
}
#endif


/*!
 * @brief config LSICLK module based on provided configuration.
 *
 * @param enable   Enable/Disable.
 *
 * @param lsiConfig   SCG_LSICLK_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigLSICLK(bool enable, const SCG_LSICLK_CONFIG_T * lsiConfig)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_LSICLK_CONFIG_T defaultLsiConfig;
    const SCG_LSICLK_CONFIG_T * lsiCfg;
    uint32_t timeout;

    if (POINTER_IS_NULL(lsiConfig) == true)
    {
        defaultLsiConfig.enableInStop      = false;
        defaultLsiConfig.enableInLowPower  = true;
        defaultLsiConfig.locked            = false;

        defaultLsiConfig.div1              = SCG_ASYNCLK_DIV_BY_1;
        defaultLsiConfig.div2              = SCG_ASYNCLK_DIV_BY_1;

        defaultLsiConfig.range             = SCG_LSICLK_RANGE_HIGH;

        lsiCfg = &defaultLsiConfig;
    }
    else
    {
        lsiCfg = lsiConfig;
    }

    /* If clock is used by system, return STATUS_BUSY. */
    if (SCG_HW_ReadLsiAsSystemClock(SCG))
    {
        statusValue = STATUS_BUSY;
    }
    /* Disable LSICLK */
    else
    {
        /* Clear LOCK bit field */
        SCG_HW_ClearLsiLockStatus(SCG);

        /* Disable monitor, disable clock and clear error. */
        SCG_HW_ClearLsiControlStatus(SCG);
    }

    /* Configure LSICLK. */
    if ((statusValue == STATUS_SUCCESS))
    {
        if (enable)
        {
            /* Now start to set up LSICLK clock. */
            /* Step 1. Setup dividers. */
            SCG_HW_ConfigLsiAsyncConfig(SCG, lsiCfg->div1, lsiCfg->div2);

            /* Step 2. Set LSICLK configuration: frequency range. */
            SCG_HW_ConfigLsiRang(SCG, lsiCfg->range);

            /* Step 3. Set LSICLK control: enable clock, configure source in STOP and VLP modes, configure lock feature. */
            SCG_HW_ConfigLsiControl(SCG, lsiCfg->enableInStop, lsiCfg->enableInLowPower, lsiCfg->locked);

            /* Wait for LSICLK to initialize */
            timeout = LSICLK_STABILIZATION_TIMEOUT;

            do
            {
                timeout--;
            }
            while ((CLOCK_ReadLsiFreq() == 0U) && (timeout > 0U));

            if (timeout == 0U)
            {
                statusValue = STATUS_TIMEOUT;
            }
        }
    }

    return statusValue;
}

/*!
 * @brief Config HSICLK module based on provided configuration.
 *
 * @param enable  Enable/Disable.
 *
 * @param hsiConfig  SCG_HSICLK_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigHSICLK(bool enable, const SCG_HSICLK_CONFIG_T * hsiConfig)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_HSICLK_CONFIG_T defaultHsiConfig;
    const SCG_HSICLK_CONFIG_T * hsiCfg;
    uint32_t timeout;

    if (POINTER_IS_NULL(hsiConfig) == true)
    {
        defaultHsiConfig.regulator         = true;
        defaultHsiConfig.locked            = false;

        defaultHsiConfig.div1              = SCG_ASYNCLK_DIV_BY_1;
        defaultHsiConfig.div2              = SCG_ASYNCLK_DIV_BY_1;

        defaultHsiConfig.range             = SCG_HSICLK_RANGE_48M;

        hsiCfg = &defaultHsiConfig;
    }
    else
    {
        hsiCfg = hsiConfig;
    }

    /* If clock is used by system, return STATUS_BUSY. */
    if (SCG_HW_ReadHsiAsSystemClock(SCG))
    {
        statusValue = STATUS_BUSY;
    }
    /* Disable HSICLK */
    else
    {
        /* Clear LOCK bit field */
        SCG_HW_ClearHsiLockStatus(SCG);

        /* Disable monitor, disable clock and clear error. */
        SCG_HW_ClearHsiControlStatus(SCG);
    }

    /* Configure HSICLK. */
    if (enable && (statusValue == STATUS_SUCCESS))
    {
        /* Now start to set up HSICLK clock. */
        /* Step 1. Setup dividers. */
        SCG_HW_ConfigHsiAsyncConfig(SCG, hsiCfg->div1, hsiCfg->div2);

        /* Step 2. Set HSICLK configuration. */
        SCG_HW_ConfigHsiRang(SCG, hsiCfg->range);

        /* Step 3. Enable clock, config regulator and locking feature. */
        SCG_HW_ConfigHsiControl(SCG, hsiCfg->regulator, hsiCfg->locked);

        /* Wait for HSICLK to initialize */
        timeout = HSICLK_STABILIZATION_TIMEOUT;

        do
        {
            timeout--;
        }
        while ((CLOCK_ReadHsiFreq() == 0U) && (timeout > 0U));

        if (timeout == 0U)
        {
            statusValue = STATUS_TIMEOUT;
        }
    }

    return statusValue;
}

/*!
 * @brief config SYSOSC module based on provided configuration.
 *
 * @param enable  Enable/Disable.
 *
 * @param sysOscConfig  SCG_SYSOSC_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigSYSOSC(bool enable, const SCG_SYSOSC_CONFIG_T * sysOscConfig)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_SYSOSC_CONFIG_T defaultSoscConfig;
    const SCG_SYSOSC_CONFIG_T * sysOscCfg;
    uint32_t timeout;

    if (POINTER_IS_NULL(sysOscConfig) == true)
    {
        defaultSoscConfig.monitorMode       = SCG_SYSOSC_MONITOR_DISABLE;
        defaultSoscConfig.locked            = false;

        defaultSoscConfig.div1              = SCG_ASYNCLK_DIV_BY_1;
        defaultSoscConfig.div2              = SCG_ASYNCLK_DIV_BY_1;

        defaultSoscConfig.extRef            = SCG_SYSOSC_REF_OSC;
        defaultSoscConfig.gain              = SCG_SYSOSC_GAIN_LOW;
        defaultSoscConfig.range             = SCG_SYSOSC_RANGE_MID;

        defaultSoscConfig.freq              = 8000000U;

        sysOscCfg = &defaultSoscConfig;
    }
    else
    {
        sysOscCfg = sysOscConfig;
    }

    /* If clock is used by system, return STATUS_BUSY. */
    if (SCG_HW_ReadSoscAsSystemClock(SCG))
    {
        statusValue = STATUS_BUSY;
    }
    /* Disable SYSOSC */
    else
    {
        /* Clear LOCK bit field */
        SCG_HW_ClearSoscLockStatus(SCG);

        /* Disable monitor, disable clock and clear error. */
        SCG_HW_ClearSoscControlStatus(SCG);

        g_extal0ClkFreq = 0U;
    }

    /* Configure SYSOSC. */
    if (enable && (statusValue == STATUS_SUCCESS))
    {
        /* Now start to set up OSC clock. */
        /* Step 1. Setup dividers. */
        SCG_HW_ConfigSoscAsyncConfig(SCG, sysOscCfg->div1, sysOscCfg->div2);

        /* Step 2. Set OSC configuration. */
        SCG_HW_ConfigSoscCfg(SCG, sysOscCfg->range, sysOscCfg->gain, sysOscCfg->extRef);

        /* Step 3. Enable clock, configure monitor, lock register. */
        if (sysOscCfg->monitorMode == SCG_SYSOSC_MONITOR_DISABLE)
        {
            SCG_HW_ConfigSoscControl(SCG, false, false, sysOscCfg->locked);
        }
        else if (sysOscCfg->monitorMode == SCG_SYSOSC_MONITOR_INT)
        {
            SCG_HW_ConfigSoscControl(SCG, true, false, sysOscCfg->locked);
        }
        else if (sysOscCfg->monitorMode == SCG_SYSOSC_MONITOR_RESET)
        {
            SCG_HW_ConfigSoscControl(SCG, true, true, sysOscCfg->locked);
        }
        else
        {
            statusValue = STATUS_ERROR;
        }

        g_extal0ClkFreq = sysOscCfg->freq;

        /* Wait for System OSC to initialize */
        timeout = SYSOSC_STABILIZATION_TIMEOUT;

        do
        {
            timeout--;
        }
        while ((CLOCK_ReadSysOscFreq() == 0U) && (timeout > 0U));

        if (timeout == 0U)
        {
            statusValue = STATUS_TIMEOUT;
        }
    }

    return statusValue;
}

#if FEATURE_WITH_SYSPLL_CLK

/*!
 * @brief config SYSPLL module based on provided configuration.
 *
 * @param enable  Enable/Disable.
 *
 * @param sysPllConfig  SCG_SYSPLL_CONFIG_T pointer.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigSYSPLL(bool enable, const SCG_SYSPLL_CONFIG_T * sysPllConfig)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_SYSPLL_CONFIG_T defaultSysPllConfig;
    const SCG_SYSPLL_CONFIG_T * sysPllCfg;
    uint32_t srcFreq, timeout;

    if (POINTER_IS_NULL(sysPllConfig) == true)
    {
        defaultSysPllConfig.monitorMode    = SCG_SYSPLL_MONITOR_DISABLE;
        defaultSysPllConfig.locked         = false;

        defaultSysPllConfig.div1           = SCG_ASYNCLK_DIV_BY_1;
        defaultSysPllConfig.div2           = SCG_ASYNCLK_DIV_BY_1;

        defaultSysPllConfig.prediv         = 0;
        defaultSysPllConfig.mul           = 7;
        defaultSysPllConfig.src            = 0;

        sysPllCfg = &defaultSysPllConfig;
    }
    else
    {
        sysPllCfg = sysPllConfig;
    }

    /* If clock is used by system, return STATUS_BUSY. */
    if (SCG_HW_ReadSysPllAsSystemClock(SCG))
    {
        statusValue = STATUS_BUSY;
    }
    /* Disable the SYSPLL. */
    else
    {
        /* Clear LOCK bit field */
        SCG_HW_ClearSysPllLockStatus(SCG);

        /* Disable monitor, disable clock and clear error. */
        SCG_HW_ClearSysPllControlStatus(SCG);
    }

    /* Configure SYSPLL. */
    if (enable && (statusValue == STATUS_SUCCESS))
    {
        srcFreq = CLOCK_ReadSysOscFreq();

        /* Pre-divider checking. */
        srcFreq /= (((uint32_t)sysPllCfg->prediv) + SCG_SYSPLL_PREDIV_BASE);

        SCG_HW_ConfigSysPllAsyncConfig(SCG, sysPllCfg->div1, sysPllCfg->div2);

        /* Step 2. Set PLL configuration. */
        SCG_HW_ConfigSysPllCfg(SCG, sysPllCfg->prediv, sysPllCfg->mul);

        /* Step 3. Enable clock, configure monitor, lock register. */
        if (sysPllCfg->monitorMode == SCG_SYSPLL_MONITOR_DISABLE)
        {
            SCG_HW_ConfigSysPllControl(SCG, false, false, sysPllCfg->locked);
        }
        else if (sysPllCfg->monitorMode == SCG_SYSPLL_MONITOR_INT)
        {
            SCG_HW_ConfigSysPllControl(SCG, true, false, sysPllCfg->locked);
        }
        else if (sysPllCfg->monitorMode == SCG_SYSPLL_MONITOR_RESET)
        {
            SCG_HW_ConfigSysPllControl(SCG, true, true, sysPllCfg->locked);
        }
        else
        {
            statusValue = STATUS_ERROR;
        }

        /* Wait for System PLL to initialize */
        timeout = SYSPLL_STABILIZATION_TIMEOUT;

        do
        {
            timeout--;
        }
        while ((CLOCK_ReadSysPllFreq() == 0U) && (timeout > 0U));

        if (timeout == 0U)
        {
            statusValue = STATUS_TIMEOUT;
        }
    }

    return statusValue;
}
#endif


/*!
 * @brief Transition to a temporary system clock
 *
 * @param curSystemClockSrouce    SCG_SYS_CLOCK_SRC_T value.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ChangeToOtherSysClk(SCG_SYS_CLOCK_SRC_T curSystemClockSrouce)
{
    SCG_SYS_CLK_CONFIG_T systemClockCfg;

    const SCG_SYS_CLOCK_DIV_T tempSystemClock[TEMP_SYS_CLK_INDEX][TEMP_SYS_DIV_INDEX] = TEMP_SYSTEM_CLOCK_CONFIGS;

    STATUS_T statusValue = STATUS_ERROR;


    if (statusValue == STATUS_ERROR)
    {
        /* SYSOSC is functional, it is not the current system clock source */
        if ((CLOCK_ReadSysOscFreq() != 0U) && (curSystemClockSrouce != SCG_SYS_CLOCK_SRC_SYS_OSC))
        {
            systemClockCfg.src     = SCG_SYS_CLOCK_SRC_SYS_OSC;
            systemClockCfg.divCore = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_SYS_DIV];
            systemClockCfg.divBus  = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_BUS_DIV];
            systemClockCfg.divSlow = tempSystemClock[TEMP_SYSOSC_CLK][TEMP_SLOW_DIV];
            statusValue = CLOCK_ChangeSystemClock(&systemClockCfg);
        }
    }
#if FEATURE_WITH_SYSPLL_CLK
    if (statusValue != STATUS_SUCCESS)
    {
        /* SYSPLL is functional, and is not the current system clock source, no valid source has been found yet. */
        if ((CLOCK_ReadSysPllFreq() != 0U) && (curSystemClockSrouce != SCG_SYS_CLOCK_SRC_SYS_PLL))
        {
            systemClockCfg.src     = SCG_SYS_CLOCK_SRC_SYS_PLL;
            systemClockCfg.divCore = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_SYS_DIV];
            systemClockCfg.divBus  = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_BUS_DIV];
            systemClockCfg.divSlow = tempSystemClock[TEMP_SYSPLL_CLK][TEMP_SLOW_DIV];
            statusValue = CLOCK_ChangeSystemClock(&systemClockCfg);
        }
    }
#endif

    if (statusValue != STATUS_SUCCESS)
    {
        /* HSICLK is functional, it is not the current system clock source, no valid source has been found yet. */
        if ((CLOCK_ReadHsiFreq() != 0U) && (curSystemClockSrouce != SCG_SYS_CLOCK_SRC_HSICLK))
        {
            systemClockCfg.src     = SCG_SYS_CLOCK_SRC_HSICLK;
            systemClockCfg.divCore = tempSystemClock[TEMP_HSICLK_CLK][TEMP_SYS_DIV];
            systemClockCfg.divBus  = tempSystemClock[TEMP_HSICLK_CLK][TEMP_BUS_DIV];
            systemClockCfg.divSlow = tempSystemClock[TEMP_HSICLK_CLK][TEMP_SLOW_DIV];
            statusValue = CLOCK_ChangeSystemClock(&systemClockCfg);
        }
    }

    if (statusValue != STATUS_SUCCESS)
    {
        /* LSICLK is functional, it is not the current system clock source, no valid source has been found yet. */
        if ((CLOCK_ReadLsiFreq() != 0U) && (curSystemClockSrouce != SCG_SYS_CLOCK_SRC_LSICLK))
        {
            systemClockCfg.src     = SCG_SYS_CLOCK_SRC_LSICLK;
            systemClockCfg.divCore = tempSystemClock[TEMP_LSICLK_CLK][TEMP_SYS_DIV];
            systemClockCfg.divBus  = tempSystemClock[TEMP_LSICLK_CLK][TEMP_BUS_DIV];
            systemClockCfg.divSlow = tempSystemClock[TEMP_LSICLK_CLK][TEMP_SLOW_DIV];
            statusValue = CLOCK_ChangeSystemClock(&systemClockCfg);
        }
    }

    return statusValue;
}


/*!
 * @brief Config and transitions to a temporary system clock source: HSICLK
 *
 * @param None.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T CLOCK_ConfigTemporarySystemClock(void)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    SCG_SYS_CLK_CONFIG_T systemClockConfig;
    const SCG_SYS_CLOCK_DIV_T tempSystemClock[TEMP_SYS_CLK_INDEX][TEMP_SYS_DIV_INDEX] = TEMP_SYSTEM_CLOCK_CONFIGS;

    /* If the current system clock source is not HSICLK:
     * 1. Enable HSICLK (if it's not enabled)
     * 2. Switch to HSICLK.
     */
    if (SCG_HW_ReadCurrentSystemClockSource(SCG) != ((uint32_t)SCG_SYS_CLOCK_SRC_HSICLK))
    {
        /* If HSICLK is not on, then HSICLK is configured
         * with the default configuration */
        if (CLOCK_ReadHsiFreq() == 0UL)
        {
            statusValue = CLOCK_ConfigHSICLK(true, NULL);
        }

        /* HSICLK is enabled, transition the system clock source to HSICLK. */
        if (statusValue == STATUS_SUCCESS)
        {
            systemClockConfig.src     = SCG_SYS_CLOCK_SRC_HSICLK;
            systemClockConfig.divCore = tempSystemClock[TEMP_HSICLK_CLK][TEMP_SYS_DIV];
            systemClockConfig.divBus  = tempSystemClock[TEMP_HSICLK_CLK][TEMP_BUS_DIV];
            systemClockConfig.divSlow = tempSystemClock[TEMP_HSICLK_CLK][TEMP_SLOW_DIV];
            statusValue = CLOCK_ChangeSystemClock(&systemClockConfig);
        }
    }

    return statusValue;
}

/*!
 * @brief Used by CLOCK_ConfigTemporarySystemClock and
 * CLOCK_ConfigModulesFromScg functions
 *
 * @param changeToClock        SCG_SYS_CLK_CONFIG_T pointer
 *
 * @retval  STATUS_T Code.
 *
 */
static STATUS_T CLOCK_ChangeSystemClock(const SCG_SYS_CLK_CONFIG_T * changeToClock)
{
    SCG_SYSCLK_MODE_T run_mode;
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t timeout;

    /* Get & Convert Run mode from SYSMC to SCG defines*/
    run_mode = CLOCK_ReadCurrentRunMode();

    /* Check the current mode */
    if(run_mode == SCG_SYSCLK_MODE_NONE)
    {
        returnValue = STATUS_ERROR;
    }
    else
    {
        /* Update run mode configuration */
        returnValue = CLOCK_ConfigSystemClockCfg(run_mode, changeToClock);

        if (returnValue == STATUS_SUCCESS)
        {
            /* Wait for system clock to transition. */
            timeout = 10U;

            while ((SCG_HW_ReadCurrentSystemClockSource(SCG) != ((uint32_t)changeToClock->src)) && (timeout > 0U))
            {
                timeout--;
            }

            if (timeout == 0U)
            {
                returnValue = STATUS_TIMEOUT;
            }
        }
    }

    return returnValue;
}


/*!
 * @brief Config the system configuration for the specified mode.
 *
 * @param mode        SCG_SYSCLK_MODE_T pointer.
 *
 * @param config        SCG_SYS_CLK_CONFIG_T pointer
 *
 * @retval  STATUS_T Code.
 *
 */
static STATUS_T CLOCK_ConfigSystemClockCfg(SCG_SYSCLK_MODE_T mode, SCG_SYS_CLK_CONFIG_T const * config)
{
    STATUS_T statusValue = STATUS_SUCCESS;
    uint32_t sourceFreq = 0U;

    /* The maximum clock frequencies of system clocks in all power modes */
    const uint32_t maxSystemClocksFreqOfVLPR[MODES_MAX_INDEX][SYS_CLK_MAX_INDEX]  = CLOCK_MAX_FREQS_IN_VLPR_MODE;
    const uint32_t maxSystemClocksFreqOfRUN[MODES_MAX_INDEX][SYS_CLK_MAX_INDEX]   = CLOCK_MAX_FREQS_IN_RUN_MODE;
#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
    const uint32_t maxSystemClocksFreqOfHSRUN[MODES_MAX_INDEX][SYS_CLK_MAX_INDEX] = CLOCK_MAX_FREQS_IN_HSRUN_MODE;
#endif
    const uint32_t sysFreqMul = ((uint32_t)config->divCore) + 1UL;
    const uint32_t busFreqMul = (((uint32_t)config->divCore) + 1UL) * (((uint32_t)config->divBus) + 1UL);
    const uint32_t slowFreqMul = (((uint32_t)config->divCore) + 1UL) * (((uint32_t)config->divSlow) + 1UL);

    sourceFreq = CLOCK_ReadSrcFreq(config->src);
    sourceFreq >>= 4U;

    if(mode == SCG_SYSCLK_MODE_VLPR)
    {
        /* Verify the frequencies of sys, bus and slow clocks. */
        if ((sourceFreq > (sysFreqMul  * (maxSystemClocksFreqOfVLPR[(uint32_t)config->src][CORE_CLK_INDEX] >> 4U))) ||    /* Sys(core) clock */
            (sourceFreq > (busFreqMul  * (maxSystemClocksFreqOfVLPR[(uint32_t)config->src][BUS_CLK_INDEX] >> 4U)))  ||    /* Bus clock */
            (sourceFreq > (slowFreqMul * (maxSystemClocksFreqOfVLPR[(uint32_t)config->src][SLOW_CLK_INDEX] >> 4U))))      /* Slow clock */
        {
            /* Configuration for the next system clock source is not valid. */
            statusValue = STATUS_ERROR;
        }
        else
        {
            SCG_HW_ConfigVlprClockControl(SCG, (uint32_t)config->src, (uint32_t)config->divCore, (uint32_t)config->divBus, (uint32_t)config->divSlow);
        }
    }
    else if(mode == SCG_SYSCLK_MODE_RUN)
    {
        /* Verify the frequencies of sys, bus and slow clocks. */
        if ((sourceFreq > (sysFreqMul  * (maxSystemClocksFreqOfRUN[(uint32_t)config->src][CORE_CLK_INDEX] >> 4U))) ||    /* Sys(core) clock */
            (sourceFreq > (busFreqMul  * (maxSystemClocksFreqOfRUN[(uint32_t)config->src][BUS_CLK_INDEX] >> 4U)))  ||    /* Bus clock */
            (sourceFreq > (slowFreqMul * (maxSystemClocksFreqOfRUN[(uint32_t)config->src][SLOW_CLK_INDEX] >> 4U))))      /* Slow clock */
        {
            /* Configuration for the next system clock source is not valid. */
            statusValue = STATUS_ERROR;
        }
        else
        {
            SCG_HW_ConfigRunClockControl(SCG, (uint32_t)config->src, (uint32_t)config->divCore, (uint32_t)config->divBus, (uint32_t)config->divSlow);
        }
    }
#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
    else if(mode == SCG_SYSCLK_MODE_HSRUN)
    {
        /* Verify the frequencies of sys, bus and slow clocks. */
        if ((sourceFreq > (sysFreqMul  * (maxSystemClocksFreqOfHSRUN[(uint32_t)config->src][CORE_CLK_INDEX] >> 4U))) ||   /* Sys(core) clock */
            (sourceFreq > (busFreqMul * (maxSystemClocksFreqOfHSRUN[(uint32_t)config->src][BUS_CLK_INDEX] >> 4U)))  ||    /* Bus clock */
            (sourceFreq > (slowFreqMul * (maxSystemClocksFreqOfHSRUN[(uint32_t)config->src][SLOW_CLK_INDEX] >> 4U))))     /* Slow clock */
        {
            /* Configuration for the next system clock source is not valid. */
            statusValue = STATUS_ERROR;
        }
        else
        {
            SCG_HW_ConfigHsrunClockControl(SCG, (uint32_t)config->src, (uint32_t)config->divCore, (uint32_t)config->divBus, (uint32_t)config->divSlow);
        }
    }
#endif
    else
    {
        /* Invalid mode */
        statusValue = STATUS_ERROR;
    }

    return statusValue;
}

/*!
 * @brief Config clock gate.
 *
 * @param peripheralClock   CLOCK_NAMES_T Value.
 *
 * @param gate   Enable/Disable value of clock.
 *
 * @retval None.
 */
static void CLOCK_ConfigClockGate(CLOCK_NAMES_T peripheralClock, bool gate)
{
    if (peripheralClock == SIM_MSC_CLK)
    {
        SIM_HW_ConfigMscmClockGate(SIM, gate);
    }
    else if (peripheralClock == SIM_MPU_CLK)
    {
        SIM_HW_ConfigMpuClockGate(SIM, gate);
    }
    else if (peripheralClock == SIM_DMA_CLK)
    {
        SIM_HW_ConfigDmaClockGate(SIM, gate);
    }
    else if (peripheralClock == SIM_EREP_CLK)
    {
        SIM_HW_SetErepClockGate(SIM, gate);
    }
    else if (peripheralClock == SIM_EINJ_CLK)
    {
        SIM_HW_ConfigEinjClockGate(SIM, gate);
    }
    else
    {}
}


/*!
 * @brief Used by CLOCK_ConfigScgSetting function
 *
 * @param None.
 *
 * @retval  SCG_SYSCLK_MODE_T Mode.
 *
 */
static SCG_SYSCLK_MODE_T CLOCK_ReadCurrentRunMode(void)
{
    SCG_SYSCLK_MODE_T sysClockMode;

    /* Get the current running sysClockMode */
    if (HIGH_SPEED_RUNNING_MODE == SYSMC_HW_ReadCurrentRunningPowerMode(SYSMC))
    {
        sysClockMode = SCG_SYSCLK_MODE_HSRUN;
    }
    else if (RUN_SPEED_RUNNING_MODE == SYSMC_HW_ReadCurrentRunningPowerMode(SYSMC))
    {
        sysClockMode = SCG_SYSCLK_MODE_RUN;
    }
    else if (VLPR_SPEED_RUNNING_MODE == SYSMC_HW_ReadCurrentRunningPowerMode(SYSMC))
    {
        sysClockMode = SCG_SYSCLK_MODE_VLPR;
    }
    else
    {
        sysClockMode = SCG_SYSCLK_MODE_NONE;
    }

    return sysClockMode;
}


/*!
 * @brief Read default system clock mode.
 *
 * @param mode   PWR_MODES_T Value.
 *
 * @retval SCG_SYSCLK_MODE_T Value.
 */
static SCG_SYSCLK_MODE_T CLOCK_ReadSysClockMode(const PWR_MODES_T mode)
{
    SCG_SYSCLK_MODE_T systemClockMode;

    if (mode == RUN_MODE)
    {
        systemClockMode = SCG_SYSCLK_MODE_RUN;
    }
    else if (mode == HSRUN_MODE)
    {
        systemClockMode = SCG_SYSCLK_MODE_HSRUN;
    }
    else if (mode == VLPR_MODE)
    {
        systemClockMode = SCG_SYSCLK_MODE_VLPR;
    }
    else
    {
        systemClockMode = SCG_SYSCLK_MODE_NONE;
    }

    return systemClockMode;
}

/*!
 * @brief Read the current system clock config
 *
 * @param systemClockConfig    SCG_SYS_CLK_CONFIG_T pointer.
 *
 * @retval None.
 */
static void CLOCK_ReadCurrentSysClkCfg(SCG_SYS_CLK_CONFIG_T * systemClockConfig)
{
    const SCG_SYS_CLOCK_DIV_T sysClkDivMappings[((uint32_t) SCG_SYS_CLOCK_DIV_BY_16)+ 1U] = {
            SCG_SYS_CLOCK_DIV_BY_1, SCG_SYS_CLOCK_DIV_BY_2,
            SCG_SYS_CLOCK_DIV_BY_3, SCG_SYS_CLOCK_DIV_BY_4,
            SCG_SYS_CLOCK_DIV_BY_5, SCG_SYS_CLOCK_DIV_BY_6,
            SCG_SYS_CLOCK_DIV_BY_7, SCG_SYS_CLOCK_DIV_BY_8,
            SCG_SYS_CLOCK_DIV_BY_9, SCG_SYS_CLOCK_DIV_BY_10,
            SCG_SYS_CLOCK_DIV_BY_11, SCG_SYS_CLOCK_DIV_BY_12,
            SCG_SYS_CLOCK_DIV_BY_13, SCG_SYS_CLOCK_DIV_BY_14,
            SCG_SYS_CLOCK_DIV_BY_15, SCG_SYS_CLOCK_DIV_BY_16 };

    /* Save the current system clock source configuration */
    if (1U == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SCG_SYS_CLOCK_SRC_SYS_OSC;
    }
    else if (2U == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SCG_SYS_CLOCK_SRC_LSICLK;
    }
    else if (3U == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SCG_SYS_CLOCK_SRC_HSICLK;
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (6U == SCG_HW_ReadCurrentSystemClockSource(SCG))
    {
        systemClockConfig->src = SCG_SYS_CLOCK_SRC_SYS_PLL;
    }
#endif
    else
    {

    }

    systemClockConfig->divCore = sysClkDivMappings[SCG_HW_ReadCurrentCoreClockDividerRatio(SCG)];
    systemClockConfig->divBus  = sysClkDivMappings[SCG_HW_ReadCurrentBusClockDividerRatio(SCG)];
    systemClockConfig->divSlow = sysClkDivMappings[SCG_HW_ReadCurrentSlowClockDividerRatio(SCG)];
}


/*!
 * @brief Read the system clock config
 *
 * @param systemClkMode  SCG_SYSCLK_MODE_T pointer.
 *
 * @param systemClkCfgPtr  SCG_SYS_CLK_CONFIG_T pointer.
 *
 * @retval  None.
 */
static void CLOCK_ReadSystemClockCfg(SCG_SYSCLK_MODE_T systemClkMode, SCG_SYS_CLK_CONFIG_T *systemClkCfgPtr)
{
    const SCG_SYS_CLOCK_DIV_T defSystemClock[TEMP_SYS_CLK_INDEX][TEMP_SYS_DIV_INDEX] = TEMP_SYSTEM_CLOCK_CONFIGS;
    bool readClkFreqState;

    if (CLOCK_ReadHsiFreq() != 0U)
    {
        readClkFreqState = true;
        (*systemClkCfgPtr).src     = SCG_SYS_CLOCK_SRC_HSICLK;
        (*systemClkCfgPtr).divCore = defSystemClock[TEMP_HSICLK_CLK][TEMP_SYS_DIV];
        (*systemClkCfgPtr).divBus  = defSystemClock[TEMP_HSICLK_CLK][TEMP_BUS_DIV];
        (*systemClkCfgPtr).divSlow = defSystemClock[TEMP_HSICLK_CLK][TEMP_SLOW_DIV];
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (CLOCK_ReadSysPllFreq() != 0U)
    {
        readClkFreqState = true;
        (*systemClkCfgPtr).src     = SCG_SYS_CLOCK_SRC_SYS_PLL;
        (*systemClkCfgPtr).divCore = defSystemClock[TEMP_SYSPLL_CLK][TEMP_SYS_DIV];
        (*systemClkCfgPtr).divBus  = defSystemClock[TEMP_SYSPLL_CLK][TEMP_BUS_DIV];
        (*systemClkCfgPtr).divSlow = defSystemClock[TEMP_SYSPLL_CLK][TEMP_SLOW_DIV];
    }
#endif
    else if (CLOCK_ReadSysOscFreq() != 0U)
    {
        readClkFreqState = true;
        (*systemClkCfgPtr).src     = SCG_SYS_CLOCK_SRC_SYS_OSC;
        (*systemClkCfgPtr).divCore = defSystemClock[TEMP_SYSOSC_CLK][TEMP_SYS_DIV];
        (*systemClkCfgPtr).divBus  = defSystemClock[TEMP_SYSOSC_CLK][TEMP_BUS_DIV];
        (*systemClkCfgPtr).divSlow = defSystemClock[TEMP_SYSOSC_CLK][TEMP_SLOW_DIV];
    }
    else
    {
        readClkFreqState = false;
    }

    if (readClkFreqState == false)
    {
        if (CLOCK_ReadLsiFreq() != 0U)
        {
            (*systemClkCfgPtr).src     = SCG_SYS_CLOCK_SRC_LSICLK;
            if (systemClkMode == SCG_SYSCLK_MODE_VLPR)
            {
                (*systemClkCfgPtr).divCore = FEATURE_VLPR_SYS_CLK;
                (*systemClkCfgPtr).divBus  = FEATURE_VLPR_BUS_CLK;
                (*systemClkCfgPtr).divSlow = FEATURE_VLPR_SLOW_CLK;
            }
            else
            {
                (*systemClkCfgPtr).divCore = defSystemClock[TEMP_LSICLK_CLK][TEMP_SYS_DIV];
                (*systemClkCfgPtr).divBus  = defSystemClock[TEMP_LSICLK_CLK][TEMP_BUS_DIV];
                (*systemClkCfgPtr).divSlow = defSystemClock[TEMP_LSICLK_CLK][TEMP_SLOW_DIV];
            }
        }
        else
        {}
    }

    (void)systemClkMode;
}


/*!
 * @brief Read default system clock source.
 *
 * @param src   CLOCK_NAMES_T Value.
 *
 * @retval SCG_SYS_CLOCK_SRC_T Value.
 */
static SCG_SYS_CLOCK_SRC_T CLOCK_ReadSysClockSource(CLOCK_NAMES_T src)
{
    SCG_SYS_CLOCK_SRC_T returnSource;

    if (src == SYSOSC_CLK)
    {
        returnSource = SCG_SYS_CLOCK_SRC_SYS_OSC;
    }
    else if (src == LSICLK_CLK)
    {
        returnSource = SCG_SYS_CLOCK_SRC_LSICLK;
    }
    else if (src == HSICLK_CLK)
    {
        returnSource = SCG_SYS_CLOCK_SRC_HSICLK;
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (src == SYSPLL_CLK)
    {
        returnSource = SCG_SYS_CLOCK_SRC_SYS_PLL;
    }
#endif
    else
    {
        returnSource = SCG_SYS_CLOCK_SRC_NONE;
    }

    return returnSource;
}

/*!
 * @brief Read default module clock source.
 *
 * @param None.
 *
 * @retval CLOCK_NAMES_T Value.
 */
static CLOCK_NAMES_T CLOCK_ReadDefaultModuleClkCfgSource(void)
{
    CLOCK_NAMES_T returnValue = CLOCK_NAME_MAX_COUNT;

    if (CLOCK_ReadLsiFreq() != 0U)
    {
        returnValue = LSICLK_CLK;
    }
    else if (CLOCK_ReadHsiFreq() != 0U)
    {
        returnValue = HSICLK_CLK;
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if (CLOCK_ReadSysPllFreq() != 0U)
    {
        returnValue = SYSPLL_CLK;
    }
#endif
    else if (CLOCK_ReadSysOscFreq() != 0U)
    {
        returnValue = SYSOSC_CLK;
    }
    else
    {
        returnValue = CLOCK_NAME_MAX_COUNT;
    }

    return returnValue;
}

/*!
 * @brief Read external reference value for SYSOSC clock.
 *
 * @param refClk   XOSC_REF_T Value.
 *
 * @retval SCG_SYSOSC_EXT_REF_T Value.
 */
static SCG_SYSOSC_EXT_REF_T CLOCK_ReadExtRef(XOSC_REF_T refClk)
{
    SCG_SYSOSC_EXT_REF_T extRef = SCG_SYSOSC_REF_EXT;

    if (refClk == XOSC_EXT_REF)
    {
        extRef = SCG_SYSOSC_REF_EXT;
    }
    else if (refClk == XOSC_INT_OSC)
    {
        extRef = SCG_SYSOSC_REF_OSC;
    }
    else
    {
        /* Invalid input reference value */
    }

    return extRef;
}


/*!
 * @brief Read source frequency.
 *
 * @param src   SCG_SYS_CLOCK_SRC_T value.
 *
 * @retval  source frequency.
 *
 */
static uint32_t CLOCK_ReadSrcFreq(SCG_SYS_CLOCK_SRC_T src)
{
    uint32_t sourceFreq = 0U;

    if(src == SCG_SYS_CLOCK_SRC_SYS_OSC)
    {
        sourceFreq = CLOCK_ReadSysOscFreq();
    }
    else if(src == SCG_SYS_CLOCK_SRC_LSICLK)
    {
        sourceFreq = CLOCK_ReadLsiFreq();
    }
    else if(src == SCG_SYS_CLOCK_SRC_HSICLK)
    {
        sourceFreq = CLOCK_ReadHsiFreq();
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if(src == SCG_SYS_CLOCK_SRC_SYS_PLL)
    {
        sourceFreq = CLOCK_ReadSysPllFreq();
    }
#endif
    else
    {
        sourceFreq = 0U;
    }

    return sourceFreq;
}


/*!
 * @brief Read the clock frequency of a given clock.
 *
 * @param peripheralClockName  is the name of the peripheral clock.
 *
 * @param freq  given frequency pointer.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadFreq(CLOCK_NAMES_T peripheralClockName, uint32_t * freq)
{
    STATUS_T returnValue;

    /* Frequency of the clock name from SCG */
    if (peripheralClockName < SCG_CLOCKS_END)
    {
        returnValue = CLOCK_ReadScgClockFreq(peripheralClockName, freq);
    }
    /* Frequency of the clock name from SIM */
    else if (peripheralClockName < SIM_CLOCKS_END)
    {
        returnValue = CLOCK_ReadSimClockFreq(peripheralClockName, freq);
    }
    /* Frequency of the clock name from PCM */
    else if (peripheralClockName < PCM_CLOCKS_END)
    {
        returnValue = CLOCK_ReadPcmClockFreq(peripheralClockName, freq);
    }
    /* Invalid clock name */
    else
    {
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief Read the frequency of a given clock from SCG
 *
 * @param clockName   is the name of the peripheral clock.
 *
 * @param frequency  given frequency pointer.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadScgClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    uint32_t freq = 0U;

    switch (clockName)
    {
        /* Main clocks */
        case CLKOUT_CLK:
            freq = CLOCK_ReadSimClkOutFreq();
            break;
        case CORE_CLK:
            freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_CORE);
            break;
        case BUS_CLK:
            freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_BUS);
            break;
        case SLOW_CLK:
            freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_SLOW);
            break;
        /* Other internal clocks used by peripherals. */
        case SYSOSC_CLK:
            freq = CLOCK_ReadSysOscFreq();
            break;
        case LSICLK_CLK:
            freq = CLOCK_ReadLsiFreq();
            break;
        case HSICLK_CLK:
            freq = CLOCK_ReadHsiFreq();
            break;
#if FEATURE_WITH_SYSPLL_CLK
        case SYSPLL_CLK:
            freq = CLOCK_ReadSysPllFreq();
            break;
#endif
        case SYSOSCDIV1_CLK:
            freq = CLOCK_ReadSysAsyncFreq(SYSOSC_CLK, SCG_ASYNCLK_DIV1);
            break;
        case SYSOSCDIV2_CLK:
            freq = CLOCK_ReadSysAsyncFreq(SYSOSC_CLK, SCG_ASYNCLK_DIV2);
            break;
        case LSIDIV1_CLK:
            freq = CLOCK_ReadSysAsyncFreq(LSICLK_CLK, SCG_ASYNCLK_DIV1);
            break;
        case LSIDIV2_CLK:
            freq = CLOCK_ReadSysAsyncFreq(LSICLK_CLK, SCG_ASYNCLK_DIV2);
            break;
        case HSIDIV1_CLK:
            freq = CLOCK_ReadSysAsyncFreq(HSICLK_CLK, SCG_ASYNCLK_DIV1);
            break;
        case HSIDIV2_CLK:
            freq = CLOCK_ReadSysAsyncFreq(HSICLK_CLK, SCG_ASYNCLK_DIV2);
            break;
#if FEATURE_WITH_SYSPLL_CLK
        case SYSPLLDIV1_CLK:
            freq = CLOCK_ReadSysAsyncFreq(SYSPLL_CLK, SCG_ASYNCLK_DIV1);
            break;
        case SYSPLLDIV2_CLK:
            freq = CLOCK_ReadSysAsyncFreq(SYSPLL_CLK, SCG_ASYNCLK_DIV2);
            break;
#endif
        case RTC_CLKIN_CLK:
            freq = g_rtcClkInFreq;
            break;
        case SCG_CLKOUT_CLK:
            freq = CLOCK_ReadScgClkOutFreq();
            break;
        default:
            returnCode = STATUS_UNSUPPORTED;
            break;
    }

    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}


/*!
 * @brief Read SCG asynchronous clock frequency from a clock source
 *
 * @param clockSource   the name of the peripheral clock.
 *
 * @param clockType   SCG asynchronous clock type.
 *
 * @retval ClockOut source.
 */
static uint32_t CLOCK_ReadSysAsyncFreq(CLOCK_NAMES_T clockSource, SCG_ASYNCLK_TYPE_T clockType)
{
    uint32_t freq, div = 0U;

    if(clockType == SCG_ASYNCLK_DIV1)
    {
        if(clockSource == HSICLK_CLK)
        {
            freq = CLOCK_ReadHsiFreq();
            div = SCG_HW_ReadHsiFirstAsyncDivider(SCG);
        }
        else if(clockSource == LSICLK_CLK)
        {
            freq = CLOCK_ReadLsiFreq();
            div = SCG_HW_ReadLsiFirstAsyncDivider(SCG);
        }
        else if(clockSource == SYSOSC_CLK)
        {
            freq = CLOCK_ReadSysOscFreq();
            div = SCG_HW_ReadSoscFirstAsyncDivider(SCG);
        }
#if FEATURE_WITH_SYSPLL_CLK
        else if(clockSource == SYSPLL_CLK)
        {
            freq = CLOCK_ReadSysPllFreq();
            div = SCG_HW_ReadSysPllFirstAsyncDivider(SCG);
        }
#endif
        else
        {
            /* Invalid clock source type */
            freq = 0U;
        }
    }
    else if(clockType == SCG_ASYNCLK_DIV2)
    {
        if(clockSource == HSICLK_CLK)
        {
            freq = CLOCK_ReadHsiFreq();
            div = SCG_HW_ReadHsiSecondAsyncDivider(SCG);
        }
        else if(clockSource == LSICLK_CLK)
        {
            freq = CLOCK_ReadLsiFreq();
            div = SCG_HW_ReadLsiSecondAsyncDivider(SCG);
        }
        else if(clockSource == SYSOSC_CLK)
        {
            freq = CLOCK_ReadSysOscFreq();
            div = SCG_HW_ReadSoscSecondAsyncDivider(SCG);
        }
#if FEATURE_WITH_SYSPLL_CLK
        else if(clockSource == SYSPLL_CLK)
        {
            freq = CLOCK_ReadSysPllFreq();
            div = SCG_HW_ReadSysPllSecondAsyncDivider(SCG);
        }
#endif
        else
        {
            /* Invalid clock source type */
            freq = 0U;
        }
    }

    if (div != 0U)
    {
        freq = (freq >> (div - 1U));
    }
    else  /* Output disabled. */
    {
        freq = 0U;
    }

    return freq;
}

/*!
 * @brief Read SCG System OSC clock frequency (SYSOSC).
 *
 * @retval  SCG System OSC clock frequency (SYSOSC).
 */
static uint32_t CLOCK_ReadSysOscFreq(void)
{
    uint32_t returnValue;
    if (SCG_HW_ReadSoscEnableStatus(SCG)) /* System OSC clock is valid. */
    {
        returnValue = g_extal0ClkFreq;
    }
    else
    {
        returnValue = 0U;
    }

    return returnValue;
}

/*!
 * @brief Read SCG Low speed internal clock frequency (LSICLK)
 *
 * @retval  SCG Low speed internal clock frequency (LSICLK).
 */
static uint32_t CLOCK_ReadLsiFreq(void)
{
    uint32_t returnValue = 0U;

    if (SCG_HW_ReadLsiEnableStatus(SCG)) /* LSICLK is valid. */
    {
        if (SCG_HW_ReadLsiRange(SCG) != 0U)
        {
            returnValue = FEATURE_SCG_LSICLK_HIGH_RANGE_FREQUENCY;
        }
    }

    return returnValue;
}

/*!
 * @brief Read SCG High speed internal clock frequency (HSICLK).
 *
 * @retval  SCG High speed internal clock frequency (HSICLK).
 */
static uint32_t CLOCK_ReadHsiFreq(void)
{
    uint32_t returnValue;

    const uint32_t hsiFreq[] = {
        FEATURE_SCG_HSICLK_FREQUENCY,
    };

    if (SCG_HW_ReadHsiEnableStatus(SCG)) /* HSICLK is valid. */
    {
        returnValue = hsiFreq[SCG_HW_ReadHsiRange(SCG)];
    }
    else
    {
        returnValue = 0U;
    }

    return returnValue;
}

#if FEATURE_WITH_SYSPLL_CLK
/*!
 * @brief Read SCG System PLL clock frequency (SYSPLL).
 *
 * @retval  SCG System PLL clock frequency (SYSPLL).
 */
static uint32_t CLOCK_ReadSysPllFreq(void)
{
    uint32_t freq, returnValue;

    if (SCG_HW_ReadSysPllEnableStatus(SCG)) /* System PLL is valid. */
    {
        /* Get source SysPll frequency. */
#ifdef SCG_SYSPLLCFG_SOURCE_MASK
        if (0U == SCG_ReadSourcSysPll(SCG))
        {
            freq = CLOCK_ReadSysOscFreq();
        }
        else if (1U == SCG_ReadSourcSysPll(SCG))
        {
            freq = CLOCK_ReadHsiFreq();
        }
        else
        {
            freq = 0U;
        }
#else
        freq = CLOCK_ReadSysOscFreq();
#endif

        if (freq != 0U) /* If source is valid. */
        {
            freq /= (SCG_HW_ReadSysPllPredivider(SCG) + SCG_SYSPLL_PREDIV_BASE);    /* Pre-divider. */
            freq *= (SCG_HW_ReadSysPllMultiplier(SCG) + SCG_SYSPLL_MULT_BASE);      /* Multiplier. */
            freq = freq >> 1U;  /* Divide VCO by 2. */
        }

        returnValue = freq;
    }
    else
    {
        returnValue = 0U;
    }

    return returnValue;
}
#endif


/*!
 * @brief Read the frequency of a given clock from SIM
 *
 * @param clockName   is the name of the peripheral clock.
 *
 * @param frequency   is the frequency of a given clock.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadSimClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    uint32_t clockPinSelect;
    uint32_t freq = 0U;

    if (clockName == SIM_CFGTMR0_CLK)
    {
        clockPinSelect = SIM_HW_ReadCFGtmr0ExternalClkPinMode(SIM);
        if (clockPinSelect < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_tclkFreq[clockPinSelect];
        }
    }
    else if (clockName == SIM_CFGTMR1_CLK)
    {
        clockPinSelect = SIM_HW_ReadCFGtmr1ExternalClkPinMode(SIM);
        if (clockPinSelect < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_tclkFreq[clockPinSelect];
        }
    }
    else if (clockName == SIM_CFGTMR2_CLK)
    {
        clockPinSelect = SIM_HW_ReadCFGtmr2ExternalClkPinMode(SIM);
        if (clockPinSelect < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_tclkFreq[clockPinSelect];
        }
    }
    else if (clockName == SIM_CFGTMR3_CLK)
    {
        clockPinSelect = SIM_HW_ReadCFGtmr3ExternalClkPinMode(SIM);
        if (clockPinSelect < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_tclkFreq[clockPinSelect];
        }
    }
    else
    {

        returnCode = CLOCK_ReadOtherSimClockFreq(clockName, &freq);
    }

    /* get interface clock of some special module */
    if ((clockName > SIM_LPO_128K_CLK) && (clockName <= SIM_MSC_CLK) && (returnCode == STATUS_SUCCESS))
    {
        freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_BUS);
    }
    /* return status mcu gate off if frequency is 0. */
    if ((freq == 0UL) && (returnCode != STATUS_UNSUPPORTED))
    {
        returnCode = STATUS_GATE_OFF;
    }

    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;

}

/*!
 * @brief Read the frequency of other clock from SIM
 *
 * @param clockName   is the name of the peripheral clock.
 *
 * @param frequency   given frequency pointer.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadOtherSimClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    uint32_t freq = 0 ;

    if (clockName == SIM_CLKOUT_CLK)
    {
        freq = CLOCK_ReadSimClkOutFreq();
    }
    else if (clockName == SIM_RTCCLK_CLK)
    {
        freq = CLOCK_ReadSimRtcClkFreq();
    }
    else if (clockName == SIM_LPO_CLK)
    {
        if (PMU_HW_ReadLpoDisableMode(PMU))
        {
            freq = CLOCK_ReadLpoFreq();
        }
    }
    else if (clockName == SIM_LPO_1K_CLK)
    {
        if (PMU_HW_ReadLpoDisableMode(PMU))
        {
            freq = ((SIM_HW_ReadLpo32kClkStatus(SIM)) && (SIM_HW_ReadLpo1kClkStatus(SIM))) ? LPO_1K_FREQUENCY : 0UL;
        }
    }
    else if (clockName == SIM_LPO_32K_CLK)
    {
        if (PMU_HW_ReadLpoDisableMode(PMU))
        {
            freq = SIM_HW_ReadLpo32kClkStatus(SIM) ? LPO_32K_FREQUENCY : 0UL;
        }
    }
    else if (clockName == SIM_LPO_128K_CLK)
    {
        if (PMU_HW_ReadLpoDisableMode(PMU))
        {
            freq = LPO_128K_FREQUENCY;
        }
    }
    else
    {
        returnCode = SIM_CheckPeripheralClockGate(clockName);
    }

    if(frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}


/*!
 * @brief Read the clock frequency of peripheral functional clock.
 *
 * @param clockName  is the name of the peripheral clock.
 *
 * @param frequency  given frequency pointer.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadPcmClockFreq(CLOCK_NAMES_T clockName, uint32_t * frequency)
{
    STATUS_T returnCode = STATUS_SUCCESS;
    uint32_t freq = 0U;
    uint32_t interfaceFreq = 0U;

    returnCode = CLOCK_CheckPCMClock(clockName);

    if (returnCode == STATUS_SUCCESS)
    {
        if ((g_peripheralFeaturesList[clockName] & WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK) != 0U)
        {
            /* Check whether SYS CLOCK is clocked. */
            interfaceFreq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_CORE);
            if (interfaceFreq != 0U)
            {
                returnCode = STATUS_SUCCESS;
            }
            else
            {
                returnCode = STATUS_GATE_OFF;
            }
        }
        else if ((g_peripheralFeaturesList[clockName] & WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK) != 0U)
        {
            /* Check whether BUS CLOCK is clocked. */
            interfaceFreq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_BUS);
            if (interfaceFreq != 0U)
            {
                returnCode = STATUS_SUCCESS;
            }
            else
            {
                returnCode = STATUS_GATE_OFF;
            }
        }
        else if ((g_peripheralFeaturesList[clockName] & WITH_INTERFACE_CLOCK_FROM_SLOW_CLOCK) != 0U)
        {
            /* Check whether SLOW CLOCK is clocked. */
            interfaceFreq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_SLOW);
            if (interfaceFreq != 0U)
            {
                returnCode = STATUS_SUCCESS;
            }
            else
            {
                returnCode = STATUS_GATE_OFF;
            }
        }
        else
        {   /* It's an issue in peripheral features list, each peripheral must have one interface clock. */
            returnCode = STATUS_UNSUPPORTED;
        }

        if (returnCode == STATUS_SUCCESS)
        {
            /* Check whether peripheral has protocol clock (functional clock) */
            if ((g_peripheralFeaturesList[clockName] & (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_PROTOCOL_CLOCK_FROM_ASYNC2)) != 0U)
            {
                if ((g_peripheralFeaturesList[clockName] & WITH_PROTOCOL_CLOCK_FROM_ASYNC2) != 0U)
                {
                    /* Check whether the functional clock is clocked */
                    freq = CLOCK_ReadPeripheralClockFreq(clockName, SCG_ASYNCLK_DIV2);
                }

                if ((g_peripheralFeaturesList[clockName] & WITH_PROTOCOL_CLOCK_FROM_ASYNC1) != 0U)
                {
                    /* Check whether the functional clock is clocked */
                    freq = CLOCK_ReadPeripheralClockFreq(clockName, SCG_ASYNCLK_DIV1);
                }

                if (freq == 0U)
                {
                    returnCode = STATUS_GATE_OFF;
                }
            }
            else
            {
                freq = interfaceFreq;
            }
        }
    }
    (void)interfaceFreq;

    /* If frequency reference is provided, write this value */
    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}


/*!
 * @brief Used by CLOCK_ReadPcmClockFreq function.
 *
 * @param peripheralClockName is the name of the peripheral clock.
 *
 * @param div   SCG_ASYNCLK_TYPE_T value.
 *
 * @retval freq of Peripheral Clock.
 */
static uint32_t CLOCK_ReadPeripheralClockFreq(CLOCK_NAMES_T peripheralClockName, SCG_ASYNCLK_TYPE_T div)
{
    uint32_t freq = 0;
    uint32_t fracValue = PCM_HW_ReadPeripheralFractionValue(PCM, peripheralClockName);
    uint32_t divValue = PCM_HW_ReadPeripheralDividerValue(PCM, peripheralClockName);

    /* Check division factor */
    if (((uint32_t)fracValue) <= ((uint32_t)divValue))
    {
        /* Check clock gate */
        if (PCM_HW_ReadPeripheralClkEn(PCM, peripheralClockName))
        {
            /* Check clock source */
            if ((uint32_t)CLK_SRC_SYSOSC == PCM_HW_ReadPeripheralClockSourceSel(PCM, peripheralClockName))
            {
                freq = CLOCK_ReadSysAsyncFreq(SYSOSC_CLK, div);
            }
            else if ((uint32_t)CLK_SRC_LSI == PCM_HW_ReadPeripheralClockSourceSel(PCM, peripheralClockName))
            {
                freq = CLOCK_ReadSysAsyncFreq(LSICLK_CLK, div);
            }
            else if ((uint32_t)CLK_SRC_HSI == PCM_HW_ReadPeripheralClockSourceSel(PCM, peripheralClockName))
            {
                freq = CLOCK_ReadSysAsyncFreq(HSICLK_CLK, div);
            }
#if FEATURE_WITH_SYSPLL_CLK
            else if ((uint32_t)CLK_SRC_SYSPLL == PCM_HW_ReadPeripheralClockSourceSel(PCM, peripheralClockName))
            {
                freq = CLOCK_ReadSysAsyncFreq(SYSPLL_CLK, div);
            }
#endif
#if FEATURE_WITH_LPO_PERIPHERAL_CLOCK_SOURCE
            else if ((uint32_t)CLK_SRC_LPO == PCM_HW_ReadPeripheralClockSourceSel(PCM, peripheralClockName))
            {
                (void)CLOCK_ReadSimClockFreq(SIM_LPO_CLK, &freq);
                divValue = 0U;
            }
#endif
            else
            {
                (void)CLOCK_ReadCFGtmrOptionFreq(peripheralClockName, &freq);
                divValue = 0U;
            }

            freq = freq / (divValue + 1U);
            freq = freq * (fracValue + 1U);
        }
    }

    return freq;
}


/*!
 * @brief Read the SCG system clock frequency, these
 * clocks are used for core, platform, external and bus clock domains.
 *
 * @param clockType        SCG_SYSCLK_TYPE_T pointer
 *
 * @retval  SCG system clock frequency.
 */
static uint32_t CLOCK_ReadSystemClockFreq(SCG_SYSCLK_TYPE_T clockType)
{
    uint32_t frequency;

    if(SCG_HW_ReadCurrentSystemClockSource(SCG) == (uint32_t)SCG_SYS_CLOCK_SRC_SYS_OSC)
    {
        frequency = CLOCK_ReadSysOscFreq();
    }
    else if(SCG_HW_ReadCurrentSystemClockSource(SCG) == (uint32_t)SCG_SYS_CLOCK_SRC_LSICLK)
    {
        frequency = CLOCK_ReadLsiFreq();
    }
    else if(SCG_HW_ReadCurrentSystemClockSource(SCG) == (uint32_t)SCG_SYS_CLOCK_SRC_HSICLK)
    {
        frequency = CLOCK_ReadHsiFreq();
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if(SCG_HW_ReadCurrentSystemClockSource(SCG) == (uint32_t)SCG_SYS_CLOCK_SRC_SYS_PLL)
    {
        frequency = CLOCK_ReadSysPllFreq();
    }
#endif
    else
    {
        frequency = 0U;
    }

    frequency /= (SCG_HW_ReadCurrentCoreClockDividerRatio(SCG) + 1U);

    if(clockType == SCG_SYSCLK_CORE)
    {
        /* Intentionally left blank */
    }
    else if(clockType == SCG_SYSCLK_BUS)
    {
        frequency /= (SCG_HW_ReadCurrentBusClockDividerRatio(SCG) + 1U);
    }
    else if(clockType == SCG_SYSCLK_SLOW)
    {
        frequency /= (SCG_HW_ReadCurrentSlowClockDividerRatio(SCG) + 1U);
    }
    else
    {
        frequency = 0U;
    }

    return frequency;
}

/*!
 * @brief Read the SIM RTC Clock Frequency.
 *
 * @retval None.
 */
static uint32_t CLOCK_ReadSimRtcClkFreq(void)
{
    uint32_t freq;

    if(SIM_HW_ReadRtcClkSrc(SIM) == (uint32_t)SIM_RTCCLK_SEL_SYSOSCDIV1_CLK)
    {
        freq = CLOCK_ReadSysAsyncFreq(SYSOSC_CLK, SCG_ASYNCLK_DIV1);
    }
    else if(SIM_HW_ReadRtcClkSrc(SIM) == (uint32_t)SIM_RTCCLK_SEL_LPO_32K)
    {
        freq = SIM_HW_ReadLpo32kClkStatus(SIM) ? LPO_32K_FREQUENCY : 0UL;
    }
    else if(SIM_HW_ReadRtcClkSrc(SIM) == (uint32_t)SIM_RTCCLK_SEL_RTC_CLKIN)
    {
        freq = g_rtcClkInFreq;
    }
    else if(SIM_HW_ReadRtcClkSrc(SIM) == (uint32_t)SIM_RTCCLK_SEL_HSIDIV1_CLK)
    {
        freq = CLOCK_ReadSysAsyncFreq(HSICLK_CLK, SCG_ASYNCLK_DIV1);
    }
    else
    {
        /* Invalid RTCCLK selection.*/
        freq = 0U;
    }

    return freq;
}


/*!
 * @brief Read SIM LPO clock frequency (LPO)
 *
 * @retval None.
 */
static uint32_t CLOCK_ReadLpoFreq(void)
{
    uint32_t freq = 0U;

    if (0U == SIM_HW_ReadLpoClkSelectedSource(SIM))
    {
        /* SIM_LPOCLK_SEL_LPO_128K */
        freq = LPO_128K_FREQUENCY;
    }
    else if (1U == SIM_HW_ReadLpoClkSelectedSource(SIM))
    {
        /* SIM_LPOCLK_SEL_NO_CLOCK */
        freq = 0U;
    }
    else if (2U == SIM_HW_ReadLpoClkSelectedSource(SIM))
    {
        /* SIM_LPOCLK_SEL_LPO_32K */
        freq = SIM_HW_ReadLpo32kClkStatus(SIM) ? LPO_32K_FREQUENCY : 0UL;
    }
    else if (3U == SIM_HW_ReadLpoClkSelectedSource(SIM))
    {
        /* SIM_LPOCLK_SEL_LPO_1K */
        freq = ((SIM_HW_ReadLpo32kClkStatus(SIM)) && (SIM_HW_ReadLpo1kClkStatus(SIM))) ? LPO_1K_FREQUENCY : 0UL;
    }
    else
    {
        freq = 0U;
    }

    return freq;
}


/*!
 * @brief Read the SCG Clock Out Frequency.
 *
 * @retval The SCG Clock Out Frequency.
 */
static uint32_t CLOCK_ReadScgClkOutFreq(void)
{
    uint32_t freq;

    if(SCG_HW_ReadClockoutSourceSel(SCG) == (uint32_t)SCG_CLKOUT_SRC_SCG_SLOW)
    {
        freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_SLOW);
    }
    else if(SCG_HW_ReadClockoutSourceSel(SCG) == (uint32_t)SCG_CLKOUT_SRC_SYSOSC)
    {
        freq = CLOCK_ReadSysOscFreq();
    }
    else if(SCG_HW_ReadClockoutSourceSel(SCG) == (uint32_t)SCG_CLKOUT_SRC_LSICLK)
    {
        freq = CLOCK_ReadLsiFreq();
    }
    else if(SCG_HW_ReadClockoutSourceSel(SCG) == (uint32_t)SCG_CLKOUT_SRC_HSICLK)
    {
        freq = CLOCK_ReadHsiFreq();
    }
#if FEATURE_WITH_SYSPLL_CLK
    else if(SCG_HW_ReadClockoutSourceSel(SCG) == (uint32_t)SCG_CLKOUT_SRC_SYSPLL)
    {
        freq = CLOCK_ReadSysPllFreq();
    }
#endif

    return freq;
}


/*!
 * @brief Read the SIM Clock Out Frequency.
 *
 * @retval The SIM Clock Out Frequency.
 */
static uint32_t CLOCK_ReadSimClkOutFreq(void)
{
    uint32_t freq;

    if (SIM_HW_ReadClockoutStatus(SIM))
    {
        if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_SCG_CLKOUT)
        {
            freq = CLOCK_ReadScgClkOutFreq();
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_SYSOSC_DIV2_CLK)
        {
            freq = CLOCK_ReadSysAsyncFreq(SYSOSC_CLK, SCG_ASYNCLK_DIV2);
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_LSI_DIV2_CLK)
        {
            freq = CLOCK_ReadSysAsyncFreq(LSICLK_CLK, SCG_ASYNCLK_DIV2);
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_HSI_DIV2_CLK)
        {
            freq = CLOCK_ReadSysAsyncFreq(HSICLK_CLK, SCG_ASYNCLK_DIV2);
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_HCLK)
        {
            freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_CORE);
        }
#if FEATURE_WITH_SYSPLL_CLK
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_SYSPLL_DIV2_CLK)
        {
            freq = CLOCK_ReadSysAsyncFreq(SYSPLL_CLK, SCG_ASYNCLK_DIV2);
        }
#endif
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_BUS_CLK)
        {
            freq = CLOCK_ReadSystemClockFreq(SCG_SYSCLK_BUS);
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_LPO_128K_CLK)
        {
            freq = LPO_128K_FREQUENCY;
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_LPO_CLK)
        {
            freq = CLOCK_ReadLpoFreq();
        }
        else if(SIM_HW_ReadClockoutSelectedSource(SIM) == (uint32_t)SIM_CLKOUT_SEL_SYS_RTC_CLK)
        {
            freq = CLOCK_ReadSimRtcClkFreq();
        }
        else
        {
            /* Invalid SIM CLKOUT selection.*/
            freq = 0U;
        }

        /* Apply Divide Ratio */
        freq /= (SIM_HW_ReadClockoutDividerValue(SIM) + 1U);
    }
    else
    {
        /* Output disabled. */
        freq = 0U;
    }

    return freq;
}


/*!
 * @brief Read the clock frequency of a given clock.
 *
 * @param peripheralClockName  is the name of the peripheral clock.
 *
 * @param freq  given frequency pointer.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_ReadCFGtmrOptionFreq(CLOCK_NAMES_T peripheralClockName, uint32_t * freq)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    if (peripheralClockName == CFGTMR0_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR0_CLK, freq);
    }
    else if (peripheralClockName == CFGTMR1_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR1_CLK, freq);
    }
    else if (peripheralClockName == CFGTMR2_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR2_CLK, freq);
    }
    else if (peripheralClockName == CFGTMR3_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR3_CLK, freq);
    }
#if CFGTMR_INSTANCE_COUNT > 4U
    else if (peripheralClockName == CFGTMR4_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR4_CLK, freq);
    }
#endif
#if CFGTMR_INSTANCE_COUNT > 5U
    else if (peripheralClockName == CFGTMR5_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR5_CLK, freq);
    }
#endif
#if CFGTMR_INSTANCE_COUNT > 6U
    else if (peripheralClockName == CFGTMR6_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR3_CLK, freq);
    }
#endif
#if CFGTMR_INSTANCE_COUNT > 7U
    else if (peripheralClockName == CFGTMR7_CLK)
    {
        returnValue = CLOCK_ReadSimClockFreq(SIM_CFGTMR3_CLK, freq);
    }
#endif
    else
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}


/*!
 * @brief Checks if PCM clock name is valid and if the module is enabled.
 *
 * @param peripheralClockName  is the name of the peripheral clock.
 *
 * @retval  STATUS_T Code.
 */
static STATUS_T CLOCK_CheckPCMClock(CLOCK_NAMES_T peripheralClockName)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    if (PCM_HW_ReadPeripheralClkEn(PCM, peripheralClockName) == false)
    {
        /* Module is not clocked. */
        returnValue = STATUS_GATE_OFF;
    }
    else if ((peripheralClockName <= SIM_CLOCKS_END) ||
        (peripheralClockName == PCM_ASYNCH_DIV1_CLOCKS_END) ||
        (peripheralClockName == PCM_ASYNCH_DIV2_CLOCKS_END) ||
        (peripheralClockName == PCM_SYS_CLOCKS_END) ||
        (peripheralClockName == PCM_END_OF_BUS_CLOCKS) ||
        (peripheralClockName == PCM_SLOW_CLOCKS_END))
    {
        returnValue = STATUS_UNSUPPORTED;
    }
    else
    {
        returnValue = STATUS_SUCCESS;
    }

    return returnValue;
}

/*!
 * @brief check whether is the perepheral clock.
 *
 * @param peripheralClockName is the name of the peripheral clock.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T SIM_CheckPeripheralClockGate(CLOCK_NAMES_T peripheralClockName)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    if (peripheralClockName == SIM_EINJ_CLK)
    {
        if (!SIM_HW_ReadEinjClockGate(SIM))
        {
            /* EIM is not clocked. */
            returnValue = STATUS_GATE_OFF;
        }
    }
    else if (peripheralClockName == SIM_EREP_CLK)
    {
        if (!SIM_HW_ReadErepClockGate(SIM))
        {
            /* ERM is not clocked. */
            returnValue = STATUS_GATE_OFF;
        }
    }
    else if (peripheralClockName == SIM_DMA_CLK)
    {
        if (!SIM_HW_ReadDmaClockGate(SIM))
        {
            /* DMA is not clocked. */
            returnValue = STATUS_GATE_OFF;
        }
    }
    else if (peripheralClockName == SIM_MPU_CLK)
    {
        if (!SIM_HW_ReadMpuClockGate(SIM))
        {
            /* MPU is not clocked. */
            returnValue = STATUS_GATE_OFF;
        }
    }
    else if (peripheralClockName == SIM_MSC_CLK)
    {
        if (!SIM_HW_ReadMscmClockGate(SIM))
        {
            /* MSCM is not clocked. */
            returnValue = STATUS_GATE_OFF;
        }
    }
    else
    {
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief Converts an integer value to divider value type, value of SCG_ASYNCLK_DIV_T.
 *
 * @param sysClockConfig    SCG_SYS_CLK_CONFIG_T pointer.
 *
 * @retval SCG_ASYNCLK_DIV_T Value.
 */
static SCG_ASYNCLK_DIV_T CLOCK_ConvertDividerValue(uint16_t div)
{
    SCG_ASYNCLK_DIV_T returnValue;

    if (div == (1U << 6U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_64;
    }
    else if (div == (1U << 5U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_32;
    }
    else if (div == (1U << 4U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_16;
    }
    else if (div == (1U << 3U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_8;
    }
    else if (div == (1U << 2U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_4;
    }
    else if (div == (1U << 1U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_2;
    }
    else if (div == (1U << 0U))
    {
        returnValue = SCG_ASYNCLK_DIV_BY_1;
    }
    else if (div == 0U)
    {
        /* Pass - through */
    }
    else
    {
        returnValue = SCG_ASYNCLK_DISABLE;
    }

    return returnValue;
}


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Config the EINJ Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @param EINJ clock gate enable setting
 *
 * @retval None.
 */
void SIM_HW_ConfigEinjClockGate(SIM_T *base, bool enable)
{
    if(enable)
    {
        SIM_HW_EnableEinjClock(base);
    }
    else
    {
        SIM_HW_DisableEinjClock(base);
    }
}

/*!
 * @brief Enable the clock selection of EINJ.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_EnableEinjClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.EINJCLKEN = SIM_PCLKGCTRL_EINJCLKEN_1;
}

/*!
 * @brief Enable the clock selection of EINJ.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_DisableEinjClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.EINJCLKEN = SIM_PCLKGCTRL_EINJCLKEN_0;
}

/*!
 * @brief Read the EINJ Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @retval EINJ Clock Gating
 */
bool SIM_HW_ReadEinjClockGate(const SIM_T *base)
{
    return base->PCLKGCTRL.bit.EINJCLKEN;
}

/*!
 * @brief Config the EREP Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @param enable EREP clock gate enable setting
 *
 * @retval None.
 */
void SIM_HW_SetErepClockGate(SIM_T *base, bool enable)
{
    if(enable)
    {
        SIM_HW_EnableErepClock(base);
    }
    else
    {
        SIM_HW_DisableErepClock(base);
    }
}

/*!
 * @brief Enable the clock selection of EREP.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_EnableErepClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.EREPCLKEN = SIM_PCLKGCTRL_EREPCLKEN_1;
}

/*!
 * @brief Enable the clock selection of EREP.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_DisableErepClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.EREPCLKEN = SIM_PCLKGCTRL_EREPCLKEN_0;
}

/*!
 * @brief Read the EREP Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval EREP Clock Gating
 */
bool SIM_HW_ReadErepClockGate(const SIM_T *base)
{
    return base->PCLKGCTRL.bit.EREPCLKEN;
}

/*!
 * @brief Set the DMA Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @param enable DMA clock gate enable setting
 *
 * @retval None.
 */
void SIM_HW_ConfigDmaClockGate(SIM_T *base, bool enable)
{
    if(enable)
    {
        SIM_HW_EnableDmaClock(base);
    }
    else
    {
        SIM_HW_DisableDmaClock(base);
    }
}

/*!
 * @brief Enable the clock selection of DMA.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_EnableDmaClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.DMACLKEN = SIM_PCLKGCTRL_DMACLKEN_1;
}

/*!
 * @brief Enable the clock selection of DMA.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_DisableDmaClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.DMACLKEN = SIM_PCLKGCTRL_DMACLKEN_0;
}

/*!
 * @brief Read the DMA Clock Gate from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval DMA Clock Gating
 */
bool SIM_HW_ReadDmaClockGate(const SIM_T *base)
{
    return base->PCLKGCTRL.bit.DMACLKEN;
}

/*!
 * @brief Config the MPU Clock Gating from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @param enable MPU clock gate enable setting
 *
 * @retval None.
 */
void SIM_HW_ConfigMpuClockGate(SIM_T *base, bool enable)
{
    if(enable)
    {
        SIM_HW_EnableMpuClock(base);
    }
    else
    {
        SIM_HW_DisableMpuClock(base);
    }
}

/*!
 * @brief Enable the clock selection of MPU.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_EnableMpuClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.MPUCLKEN = SIM_PCLKGCTRL_MPUCLKEN_1;
}

/*!
 * @brief Enable the clock selection of MPU.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_DisableMpuClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.MPUCLKEN = SIM_PCLKGCTRL_MPUCLKEN_0;
}

/*!
 * @brief Read the MPU Clock Gating from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval MPU Clock Gating
 */
bool SIM_HW_ReadMpuClockGate(const SIM_T *base)
{
    return base->PCLKGCTRL.bit.MPUCLKEN;
}

/*!
 * @brief Config the MSCM Clock Gating from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 * @param enable MSCM clock gate enable setting
 *
 * @retval None.
 */
void SIM_HW_ConfigMscmClockGate(SIM_T *base, bool enable)
{
    if(enable)
    {
        SIM_HW_EnableMscmClock(base);
    }
    else
    {
        SIM_HW_DisableMscmClock(base);
    }
}

/*!
 * @brief Enable the clock selection of MSCM.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_EnableMscmClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.MSCCLKEN = SIM_PCLKGCTRL_MSCCLKEN_1;
}

/*!
 * @brief Enable the clock selection of MSCM.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval None.
 */
void SIM_HW_DisableMscmClock(SIM_T *base)
{
    base->PCLKGCTRL.bit.MSCCLKEN = SIM_PCLKGCTRL_MSCCLKEN_0;
}

/*!
 * @brief Read the MSCM Clock Gating from the Platform Clock Gating Control Register.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval MSCM Clock Gating
 */
bool SIM_HW_ReadMscmClockGate(const SIM_T *base)
{
    return base->PCLKGCTRL.bit.MSCCLKEN;
}

/*!
 * @brief Config ext pin clock source for CFGTMR
 *
 * @param base        sim base pointer
 * @param instance    cfgtmr instance
 * @param source      clock source
 *
 * @retval None.
 */
void SIM_HW_ConfigExtPinSourceCFGtmr(SIM_T *base, uint8_t instance, uint32_t source)
{
    switch (instance)
    {
        case 0U:
            base->CFGTMROPT0.bit.CFGTMR0EXCLKPINSEL = source;
            break;
        case 1U:
            base->CFGTMROPT0.bit.CFGTMR1EXCLKPINSEL = source;
            break;
        case 2U:
            base->CFGTMROPT0.bit.CFGTMR2EXCLKPINSEL = source;
            break;
        case 3U:
            base->CFGTMROPT0.bit.CFGTMR3EXCLKPINSEL = source;
            break;
        default:
            /* Do nothing */
            break;
    }
}

/*!
 * @brief Read CFGTMR0 external clock pin select setting.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval Selected CFGTMR0 External Clock Pin.
 */
uint32_t SIM_HW_ReadCFGtmr0ExternalClkPinMode(const SIM_T *base)
{
    return base->CFGTMROPT0.bit.CFGTMR0EXCLKPINSEL;
}

/*!
 * @brief Read CFGTMR1 external clock pin select setting.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval Selected CFGTMR1 External Clock Pin.
 */
uint32_t SIM_HW_ReadCFGtmr1ExternalClkPinMode(const SIM_T *base)
{
    return base->CFGTMROPT0.bit.CFGTMR1EXCLKPINSEL;
}

/*!
 * @brief Read CFGTMR2 external clock pin select setting.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval Selected CFGTMR2 External Clock Pin.
 */
uint32_t SIM_HW_ReadCFGtmr2ExternalClkPinMode(const SIM_T *base)
{
    return base->CFGTMROPT0.bit.CFGTMR2EXCLKPINSEL;
}

/*!
 * @brief Read CFGTMR3 external clock pin select setting.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval Selected CFGTMR3 External Clock Pin.
 */
uint32_t SIM_HW_ReadCFGtmr3ExternalClkPinMode(const SIM_T *base)
{
    return base->CFGTMROPT0.bit.CFGTMR3EXCLKPINSEL;
}

/*!
 * @brief Read SIM LPO 32KHz clock gating status (LPO_32K_CLOCK).
 *
 * @param base Base address for current SIM instance.
 *
 * @retval LPO 32KHz clock gating status.
 */
bool SIM_HW_ReadLpo32kClkStatus(const SIM_T *base)
{
    return base->LPOCLKSEL.bit.LPO32KCLKOEN;
}

/*!
 * @brief Read SIM LPO 1KHz clock gating status (LPO_1K_CLOCK).
 *
 * @param base Base address for current SIM instance.
 *
 * @retval LPO 1KHz clock gating status.
 */
bool SIM_HW_ReadLpo1kClkStatus(const SIM_T *base)
{
    return base->LPOCLKSEL.bit.LPO1KCLKOEN;
}

/*!
 * @brief Read the LPO Clock Slected Source.
 *
 * @param base Base address for current SIM instance.
 * @retval LPO Clock Source.
 */
uint32_t SIM_HW_ReadLpoClkSelectedSource(const SIM_T *base)
{
    return base->LPOCLKSEL.bit.CLKLPOSSEL;
}

/**
 * @brief Read the clock selection of RTCCLKSEL.
 *
 * @param base Base address for current SIM instance.
 *
 * @retval Current selection.
 */
uint32_t SIM_HW_ReadRtcClkSrc(const SIM_T *base)
{
    return (base->LPOCLKSEL.bit.CLK32KSSEL);
}

/*!
 * @brief Config the clock selection of LPOCLKSEL.
 *
 * @param base Base address for current SIM instance.
 * @param setting The value to set.
 * @note This function ignores init member
 *
 * @retval None.
 */
void SIM_HW_ConfigLpoClocks(SIM_T *base, bool enableLpo1k, bool enableLpo32k, uint32_t srcLpoClk, uint32_t srcRtcClk)
{
    uint32_t regValue = base->LPOCLKSEL.reg;

    regValue &= (uint32_t)~((0x03U << 4) | (0x03U << 2) | (0x01U << 1) | 0x01U);

    regValue |= (uint32_t)(((srcRtcClk & 0x03U) << 4) | ((srcLpoClk & 0x03U) << 2) | ((enableLpo32k & 0x01U) << 1) | (enableLpo1k & 0x01U));

    /* Write-once register. */
    base->LPOCLKSEL.reg = regValue;
}

/*!
 * @brief Read the clock out status
 *
 * @param base Base address for current SIM instance.
 * @retval clockout status
 */
bool SIM_HW_ReadClockoutStatus(const SIM_T *base)
{
    return base->CHIPCTRL.bit.CLKOUTEN;
}

/*!
 * @brief Read the clock out divider value
 *
 * @param base Base address for current SIM instance.
 * @retval clockout divider
 */
uint32_t SIM_HW_ReadClockoutDividerValue(const SIM_T *base)
{
    return base->CHIPCTRL.bit.CLKOUTDIVCFG;
}

/*!
 * @brief Read the clock out slected source.
 *
 * @param base Base address for current SIM instance.
 * @retval clockout source
 */
uint32_t SIM_HW_ReadClockoutSelectedSource(const SIM_T *base)
{
    return base->CHIPCTRL.bit.CLKOUTSEL;
}

/*!
 * @brief Config clockout
 *
 * @param base        sim base pointer
 * @param source      clock source
 * @param divider     clock divider
 *
 * @retval None.
 */
void SIM_HW_ConfigClockout(SIM_T *base, bool enable, uint32_t source, uint32_t divider)
{
    /* The bellow sequence should be follwed while CLK configuration */
    /* 1. Configure SIM_CHIPCTRL[CLKOUTSEL] */
    base->CHIPCTRL.bit.CLKOUTSEL = source;
    /* 2. Configure SIM_CHIPCTRL[CLKOUTDIVCFG] */
    base->CHIPCTRL.bit.CLKOUTDIVCFG = divider;
    /* 3. Enable SIM_CHIPCTRL[CLKOUTEN] */
    base->CHIPCTRL.bit.CLKOUTEN = enable;
}

/*!
 * @brief Configure clock trace source
 *
 * @param base        sim base pointer
 * @param source      clock trace source
 *
 * @retval None.
 */
void SIM_HW_ConfigTraceClockSource(SIM_T *base, uint32_t clkTraceSrc)
{
    /* Configure trace source. */
    base->CHIPCTRL.bit.TRACECLKSEL = clkTraceSrc;
}

/*!
 * @brief Clears SIM Debug Trace configuration.
 *
 * @param base Register base address for the SIM instance.
 *
 * @retval None.
 */
void SIM_HW_ClearTraceClockConfig(SIM_T *base)
{
    /* Disable divider. */
    base->SYSCLKDIV4.reg = 0U;
}

/*!
 * @brief Set SIM Debug Trace configuration.
 *
 * @param base        sim base pointer
 * @param source      clock trace source
 */
void SIM_HW_ConfigTraceClock(SIM_T *base, bool enable, uint32_t divider, uint32_t multiplier)
{
    base->SYSCLKDIV4.bit.DTRACEDEN = SIM_SYSCLKDIV4_DTRACEDEN_0;
    base->SYSCLKDIV4.bit.TCLKDIVDCFG = divider;
    base->SYSCLKDIV4.bit.TCLKDIVNCFG = multiplier;
    base->SYSCLKDIV4.bit.DTRACEDEN = enable;
}

/*!
 * @brief Gets the current running power mode.
 *
 * @param baseAddr  Base address for current SYSMC instance.
 *
 * @retval  Current power mode status.
 */
uint32_t SYSMC_HW_ReadCurrentRunningPowerMode(const SYSMC_T *const baseAddr)
{
    return baseAddr->PMSTS.bit.PMSTS;
}

/*!
 * @brief Config SYSOSC control register
 *
 * @param base        pcm base pointer
 * @param clockName   CLOCK_NAMES_T value
 * @param clockGate   clock gate enable or disable
 * @param clockSource peripheral clock source select
 * @param divider     peripheral clock divider select
 * @param multiplier  peripheral clock divider fraction
 *
 * @retval None.
 */
void PCM_HW_ConfigPeripheralClockControl(PCM_T *base, CLOCK_NAMES_T clockName, bool clockGate, uint32_t clockSource, uint32_t divider, uint32_t multiplier)
{
   /* Configure the peripheral clock source, the fractional clock divider and the clock gate */
   switch(clockName)
   {
       uint32_t value;
        case COMP_CLK:
            value = base->COMP0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->COMP0CLK.reg = value;
            break;
        case CRC_CLK:
            value = base->CRCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->CRCCLK.reg = value;
            break;
        case DMACHM_CLK:
            value = base->DMACHMCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->DMACHMCLK.reg = value;
            break;
        case EWDT_CLK:
            value = base->EWDTCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->EWDTCLK.reg = value;
            break;
        case PMA_CLK:
            value = base->PMACLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PMACLK.reg = value;
            break;
        case PMB_CLK:
            value = base->PMBCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PMBCLK.reg = value;
            break;
        case PMC_CLK:
            value = base->PMCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PMCCLK.reg = value;
            break;
        case PMD_CLK:
            value = base->PMDCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PMDCLK.reg = value;
            break;
        case PME_CLK:
            value = base->PMECLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PMECLK.reg = value;
            break;
        case RTC_CLK:
            value = base->RTCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->RTCCLK.reg = value;
            break;
        case CAN0_CLK:
            value = base->CAN0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->CAN0CLK.reg = value;
            break;
        case CAN1_CLK:
            value = base->CAN1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->CAN1CLK.reg = value;
            break;
        case CAN2_CLK:
            value = base->CAN2CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->CAN2CLK.reg = value;
            break;
        case PDU0_CLK:
            value = base->PDU0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PDU0CLK.reg = value;
            break;
        case PDU1_CLK:
            value = base->PDU1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->PDU1CLK.reg = value;
            break;
        case FLASH_CLK:
            value = base->FLASHCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((clockGate & 0x01U) << 30U);
            base->FLASHCLK.reg = value;
            break;
        case CFGTMR0_CLK:
            value = base->CFGTMR0CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR0CLK.reg = value;
            break;
        case CFGTMR1_CLK:
            value = base->CFGTMR1CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR1CLK.reg = value;
            break;
        case CFGTMR2_CLK:
            value = base->CFGTMR2CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR2CLK.reg = value;
            break;
        case CFGTMR3_CLK:
            value = base->CFGTMR3CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR3CLK.reg = value;
            break;
#if CFGTMR_INSTANCE_COUNT > 4U
        case CFGTMR4_CLK:
            value = base->CFGTMR4CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR4CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 5U
        case CFGTMR5_CLK:
            value = base->CFGTMR5CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR5CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 6U
        case CFGTMR6_CLK:
            value = base->CFGTMR6CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR6CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 7U
        case CFGTMR7_CLK:
            value = base->CFGTMR7CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGTMR7CLK.reg = value;
            break;
#endif
        case ADC0_CLK:
            value = base->ADC0CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->ADC0CLK.reg = value;
            break;
        case ADC1_CLK:
            value = base->ADC1CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->ADC1CLK.reg = value;
            break;
        case CFGIO_CLK:
            value = base->CFGIOCLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->CFGIOCLK.reg = value;
            break;
        case LPI2C_CLK:
            value = base->LPI2CCLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPI2CCLK.reg = value;
            break;
        case LPITMR_CLK:
            value = base->LPITMRCLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPITMRCLK.reg = value;
            break;
        case LPSPI0_CLK:
            value = base->LPSPI0CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPSPI0CLK.reg = value;
            break;
        case LPSPI1_CLK:
            value = base->LPSPI1CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPSPI1CLK.reg = value;
            break;
        case LPSPI2_CLK:
            value = base->LPSPI2CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPSPI2CLK.reg = value;
            break;
        case LPTMR_CLK:
            value = base->LPTMRCLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U)|(0x01U << 3U) | 0x07U);
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U)|((multiplier & 0x01U) << 3U)|(divider & 0x07U));
            base->LPTMRCLK.reg = value;
            break;
        case LPUART0_CLK:
            value = base->LPUART0CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPUART0CLK.reg = value;
            break;
        case LPUART1_CLK:
            value = base->LPUART1CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPUART1CLK.reg = value;
            break;
        case LPUART2_CLK:
            value = base->LPUART2CLK.reg;
            value &= (uint32_t)~((0x01U << 30U)|(0x07U << 24U));
            value |= (uint32_t)(((clockGate & 0x01U) << 30U)|((clockSource & 0x07U) << 24U));
            base->LPUART2CLK.reg = value;
            break;
        default :
            break;
   }
}

/*!
 * @brief Enables/disables the clock for a given peripheral.
 * For example, to enable the ADC0 clock, use like this:
 * @code
 *  PCM_HW_ConfigPeripheralClkEn(PCM, PCM_ADC0_CLOCK, true);
 * @endcode
 *
 * @param base        pcm base pointer
 * @param clockName   is the name of the peripheral clock
 * must be one of the following values (see the CLOCK_NAMES_T type from apm32f445_features.h or apm32f446_features.h)
 *    COMP_CLK
 *    CRC_CLK
 *    ...
 *    LPUART2_CLK
 * @param isClockEnabled  is the value of the command that enables/disables the clock
 *
 * @retval None.
 */
void PCM_HW_ConfigPeripheralClkEn(PCM_T *const base, const CLOCK_NAMES_T clockName, const bool isClockEnabled)
{
    uint32_t value;
    switch(clockName)
   {
        case COMP_CLK:
            value = base->COMP0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->COMP0CLK.reg = value;
            break;
        case CRC_CLK:
            value = base->CRCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CRCCLK.reg = value;
            break;
        case DMACHM_CLK:
            value = base->DMACHMCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->DMACHMCLK.reg = value;
            break;
        case EWDT_CLK:
            value = base->EWDTCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->EWDTCLK.reg = value;
            break;
        case PMA_CLK:
            value = base->PMACLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PMACLK.reg = value;
            break;
        case PMB_CLK:
            value = base->PMBCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PMBCLK.reg = value;
            break;
        case PMC_CLK:
            value = base->PMCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PMCCLK.reg = value;
            break;
        case PMD_CLK:
            value = base->PMDCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PMDCLK.reg = value;
            break;
        case PME_CLK:
            value = base->PMECLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PMECLK.reg = value;
            break;
        case RTC_CLK:
            value = base->RTCCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->RTCCLK.reg = value;
            break;
        case CAN0_CLK:
            value = base->CAN0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CAN0CLK.reg = value;
            break;
        case CAN1_CLK:
            value = base->CAN1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CAN1CLK.reg = value;
            break;
        case CAN2_CLK:
            value = base->CAN2CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CAN2CLK.reg = value;
            break;
        case PDU0_CLK:
            value = base->PDU0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PDU0CLK.reg = value;
            break;
        case PDU1_CLK:
            value = base->PDU1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->PDU1CLK.reg = value;
            break;
        case FLASH_CLK:
            value = base->FLASHCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->FLASHCLK.reg = value;
            break;
        case CFGTMR0_CLK:
            value = base->CFGTMR0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR0CLK.reg = value;
            break;
        case CFGTMR1_CLK:
            value = base->CFGTMR1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR1CLK.reg = value;
            break;
        case CFGTMR2_CLK:
            value = base->CFGTMR2CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR2CLK.reg = value;
            break;
        case CFGTMR3_CLK:
            value = base->CFGTMR3CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR3CLK.reg = value;
            break;
#if CFGTMR_INSTANCE_COUNT > 4U
        case CFGTMR4_CLK:
            value = base->CFGTMR4CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR4CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 5U
        case CFGTMR5_CLK:
            value = base->CFGTMR5CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR5CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 6U
        case CFGTMR6_CLK:
            value = base->CFGTMR6CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR6CLK.reg = value;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 7U
        case CFGTMR7_CLK:
            value = base->CFGTMR7CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGTMR7CLK.reg = value;
            break;
#endif
        case ADC0_CLK:
            value = base->ADC0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->ADC0CLK.reg = value;
            break;
        case ADC1_CLK:
            value = base->ADC1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->ADC1CLK.reg = value;
            break;
        case CFGIO_CLK:
            value = base->CFGIOCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->CFGIOCLK.reg = value;
            break;
        case LPI2C_CLK:
            value = base->LPI2CCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPI2CCLK.reg = value;
            break;
        case LPITMR_CLK:
            value = base->LPITMRCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPITMRCLK.reg = value;
            break;
        case LPSPI0_CLK:
            value = base->LPSPI0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPSPI0CLK.reg = value;
            break;
        case LPSPI1_CLK:
            value = base->LPSPI1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPSPI1CLK.reg = value;
            break;
        case LPSPI2_CLK:
            value = base->LPSPI2CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPSPI2CLK.reg = value;
            break;
        case LPTMR_CLK:
            value = base->LPTMRCLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPTMRCLK.reg = value;
            break;
        case LPUART0_CLK:
            value = base->LPUART0CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPUART0CLK.reg = value;
            break;
        case LPUART1_CLK:
            value = base->LPUART1CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPUART1CLK.reg = value;
            break;
        case LPUART2_CLK:
            value = base->LPUART2CLK.reg;
            value &= (uint32_t)~(0x01U << 30U);
            value |= (uint32_t)((isClockEnabled & 0x01U) << 30U);
            base->LPUART2CLK.reg = value;
            break;
        default :
            break;
   }
}

/*!
 * @brief Read the clock gate control mode.
 *
 * @param base        pcm base pointer
 * @param clockName   is the name of the peripheral clock
 * must be one of the following values (see the CLOCK_NAMES_T type from apm32f445_features.h or apm32f446_features.h)
 *    COMP_CLK
 *    CRC_CLK
 *    ...
 *    LPUART2_CLK
 *
 * @retval  the clock gate control mode
 *        - false : Clock is disabled
 *        - true : Clock is enabled
 */
bool PCM_HW_ReadPeripheralClkEn(const PCM_T *const base, const CLOCK_NAMES_T clockName)
{
    uint32_t gateValue;

    switch(clockName)
    {
        case COMP_CLK:
            gateValue = (((base->COMP0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CRC_CLK:
            gateValue = (((base->CRCCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case DMACHM_CLK:
            gateValue = (((base->DMACHMCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case EWDT_CLK:
            gateValue = (((base->EWDTCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PMA_CLK:
            gateValue = (((base->PMACLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PMB_CLK:
            gateValue = (((base->PMBCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PMC_CLK:
            gateValue = (((base->PMCCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PMD_CLK:
            gateValue = (((base->PMDCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PME_CLK:
            gateValue = (((base->PMECLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case RTC_CLK:
            gateValue = (((base->RTCCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CAN0_CLK:
            gateValue = (((base->CAN0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CAN1_CLK:
            gateValue = (((base->CAN1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CAN2_CLK:
            gateValue = (((base->CAN2CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PDU0_CLK:
            gateValue = (((base->PDU0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case PDU1_CLK:
            gateValue = (((base->PDU1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case FLASH_CLK:
            gateValue = (((base->FLASHCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CFGTMR0_CLK:
            gateValue = (((base->CFGTMR0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CFGTMR1_CLK:
            gateValue = (((base->CFGTMR1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CFGTMR2_CLK:
            gateValue = (((base->CFGTMR2CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CFGTMR3_CLK:
            gateValue = (((base->CFGTMR3CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
#if CFGTMR_INSTANCE_COUNT > 4U
        case CFGTMR4_CLK:
            gateValue = (((base->CFGTMR4CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 5U
        case CFGTMR5_CLK:
            gateValue = (((base->CFGTMR5CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 6U
        case CFGTMR6_CLK:
            gateValue = (((base->CFGTMR6CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 7U
        case CFGTMR7_CLK:
            gateValue = (((base->CFGTMR7CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
#endif
        case ADC0_CLK:
            gateValue = (((base->ADC0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case ADC1_CLK:
            gateValue = (((base->ADC1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case CFGIO_CLK:
            gateValue = (((base->CFGIOCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPI2C_CLK:
            gateValue = (((base->LPI2CCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPITMR_CLK:
            gateValue = (((base->LPITMRCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPSPI0_CLK:
            gateValue = (((base->LPSPI0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPSPI1_CLK:
            gateValue = (((base->LPSPI1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPSPI2_CLK:
            gateValue = (((base->LPSPI2CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPTMR_CLK:
            gateValue = (((base->LPTMRCLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPUART0_CLK:
            gateValue = (((base->LPUART0CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPUART1_CLK:
            gateValue = (((base->LPUART1CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        case LPUART2_CLK:
            gateValue = (((base->LPUART2CLK.reg >> 30U) & 0x01U) == 0U) ? false : true;
            break;
        default :
            break;
   }
   return gateValue;
}

/*!
 * @brief Read the selection of a clock source for a specific peripheral
 *
 * @param base        pcm base pointer
 * @param clockName   is the name of the peripheral clock
 * must be one of the following values (see the CLOCK_NAMES_T type from apm32f445_features.h or apm32f446_features.h)
 *    CFGTMR0_CLK
 *    CFGTMR1_CLK
 *    ...
 *    LPUART2_CLK
 *
 * @retval  the clock source
 */
uint32_t PCM_HW_ReadPeripheralClockSourceSel(const PCM_T *const base, const CLOCK_NAMES_T clockName)
{
    uint32_t sourceValue;
    sourceValue = CLK_SRC_OFF;
    switch(clockName)
    {
        case CFGTMR0_CLK:
            sourceValue = (base->CFGTMR0CLK.reg >> 24U) & 0x07U;
            break;
        case CFGTMR1_CLK:
            sourceValue = (base->CFGTMR1CLK.reg >> 24U) & 0x07U;
            break;
        case CFGTMR2_CLK:
            sourceValue = (base->CFGTMR2CLK.reg >> 24U) & 0x07U;
            break;
        case CFGTMR3_CLK:
            sourceValue = (base->CFGTMR3CLK.reg >> 24U) & 0x07U;
            break;
#if CFGTMR_INSTANCE_COUNT > 4U
        case CFGTMR4_CLK:
            sourceValue = (base->CFGTMR4CLK.reg >> 24U) & 0x07U;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 5U
        case CFGTMR5_CLK:
            sourceValue = (base->CFGTMR5CLK.reg >> 24U) & 0x07U;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 6U
        case CFGTMR6_CLK:
            sourceValue = (base->CFGTMR6CLK.reg >> 24U) & 0x07U;
            break;
#endif
#if CFGTMR_INSTANCE_COUNT > 7U
        case CFGTMR7_CLK:
            sourceValue = (base->CFGTMR7CLK.reg >> 24U) & 0x07U;
            break;
#endif
        case ADC0_CLK:
            sourceValue = (base->ADC0CLK.reg >> 24U) & 0x07U;
            break;
        case ADC1_CLK:
            sourceValue = (base->ADC1CLK.reg >> 24U) & 0x07U;
            break;
        case CFGIO_CLK:
            sourceValue = (base->CFGIOCLK.reg >> 24U) & 0x07U;
            break;
        case LPI2C_CLK:
            sourceValue = (base->LPI2CCLK.reg >> 24U) & 0x07U;
            break;
        case LPITMR_CLK:
            sourceValue = (base->LPITMRCLK.reg >> 24U) & 0x07U;
            break;
        case LPSPI0_CLK:
            sourceValue = (base->LPSPI0CLK.reg >> 24U) & 0x07U;
            break;
        case LPSPI1_CLK:
            sourceValue = (base->LPSPI1CLK.reg >> 24U) & 0x07U;
            break;
        case LPSPI2_CLK:
            sourceValue = (base->LPSPI2CLK.reg >> 24U) & 0x07U;
            break;
        case LPTMR_CLK:
            sourceValue = (base->LPTMRCLK.reg >> 24U) & 0x07U;
            break;
        case LPUART0_CLK:
            sourceValue = (base->LPUART0CLK.reg >> 24U) & 0x07U;
            break;
        case LPUART1_CLK:
            sourceValue = (base->LPUART1CLK.reg >> 24U) & 0x07U;
            break;
        case LPUART2_CLK:
            sourceValue = (base->LPUART2CLK.reg >> 24U) & 0x07U;
            break;
        default :
            break;
    }
    return sourceValue;
}

/*!
 * @brief Read the selection of the fractional value for a specific peripheral
 *
 * @param base        pcm base pointer
 * @param clockName   is the name of the peripheral clock
 * must be LPTMR_CLK (see the CLOCK_NAMES_T type from apm32f445_features.h or apm32f446_features.h)
 *
 * @retval  the fractional value.
 */
uint32_t PCM_HW_ReadPeripheralFractionValue(const PCM_T *const base, const CLOCK_NAMES_T clockName)
{
    uint32_t fracValue;
    fracValue = MUL_BY_ONE;
    switch(clockName)
    {
        case LPTMR_CLK:
            fracValue = (base->LPTMRCLK.reg >> 3U) & 0x01U;
            break;
        default :
            break;
   }
   return fracValue;
}

/*!
 * @brief Read the selection of the divider value for a specific peripheral
 *
 * @param base        pcm base pointer
 * @param clockName   is the name of the peripheral clock
 * must be LPTMR_CLK (see the CLOCK_NAMES_T type from apm32f445_features.h or apm32f446_features.h)
 *
 * @retval  the divider value.
 */
uint32_t PCM_HW_ReadPeripheralDividerValue(const PCM_T *const base, const CLOCK_NAMES_T clockName)
{
    uint32_t divValue;
    divValue = (uint32_t)DIV_BY_ONE;
    switch(clockName)
    {
        case LPTMR_CLK:
            divValue = base->LPTMRCLK.reg & 0x07U;
            break;
        default :
            break;
   }
   return divValue;
}


/*!
 * @brief Enables/Disables the Low Power Oscillator.
 *
 * @param baseAddr  Base address for current PMU instance.
 * @param enable    enable/disable the Low Power Oscillator.
 *
 * @retval None.
 */
void PMU_HW_ConfigLpoDisable(PMU_T *const baseAddr, const bool enable)
{
    baseAddr->REGCSTS.bit.LPODIS = (enable ? PMU_REGCSTS_LPODIS_0 : PMU_REGCSTS_LPODIS_1);
}

/*!
 * @brief Read the Low Power Oscillator status.
 *
 * @param baseAddr  Base address for current PMU instance.
 * @retval value LPO status
 *               false - LPO is disabled
 *               true - LPO is enabled
 */
bool PMU_HW_ReadLpoDisableMode(const PMU_T *const baseAddr)
{
    return ((baseAddr->REGCSTS.bit.LPODIS == PMU_REGCSTS_LPODIS_0) ? true : false);
}


/*!
 * @brief Config Low Power Oscillator Trimming Value.
 *
 * @param baseAddr  Base address for current PMU instance.
 * @param value     Trimming value
 *
 * @retval None.
 */
void PMU_HW_ConfigLpoTrimValue(PMU_T *const baseAddr, const int8_t decimalValue)
{
    int8_t decValue = decimalValue;
    uint8_t lpotrim, trimval;

    if (decValue < 0)
    {
        lpotrim = ((uint8_t)1U) << 5U;
        decValue = (int8_t)(decValue + (int8_t)(lpotrim));
    }
    trimval = (uint8_t)decValue;

    baseAddr->LPOSCTRIM.bit.TLPOF = trimval;
}


/*!
 * @brief Read SCG ClockOut source select
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval ClockOut source.
 */
uint32_t SCG_HW_ReadClockoutSourceSel(const SCG_T *base)
{
    return (base->SYSCLKOSEL.reg >> 24U) & 0x0FU;
}


/*!
 * @brief Read SCG current system clock source
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval Current system clock source.
 */
uint32_t SCG_HW_ReadCurrentSystemClockSource(const SCG_T *base)
{
    return (base->CLKCFG.reg >> 24U) & 0x0FU;
}

/*!
 * @brief Read SCG Current Core Clock Divide Ratio
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval Current Core Clock Divide Ratio.
 */
uint32_t SCG_HW_ReadCurrentCoreClockDividerRatio(const SCG_T *base)
{
    return (base->CLKCFG.reg >> 16U) & 0x0FU;
}

/*!
 * @brief Read SCG Current Bus Clock Divide Ratio
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval Current Bus Clock Divide Ratio.
 */
uint32_t SCG_HW_ReadCurrentBusClockDividerRatio(const SCG_T *base)
{
    return (base->CLKCFG.reg >> 4U) & 0x0FU;
}

/*!
 * @brief Read SCG Current Slow Clock Divide Ratio
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval Current Slow Clock Divide Ratio.
 */
uint32_t SCG_HW_ReadCurrentSlowClockDividerRatio(const SCG_T *base)
{
    return (base->CLKCFG.reg) & 0x0FU;
}

/*!
 * @brief Config SCG run system clock control(system clock source, bus, core and slow dividers)
 *
 * @param base Register base address for the SCG instance.
 * @param base System clock configuration address.
 *
 * @retval None.
 */
void SCG_HW_ConfigRunClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider)
{
    base->RUNCLKCTRL.reg = (uint32_t)((uint32_t)((source & 0x0FU) << 24U) | (uint32_t)((coreDivider & 0x0FU) << 16U) | ((busDivider & 0x0FU) << 4U) | (uint32_t)(slowDivider & 0x0FU));
}

/*!
 * @brief Config SCG vlpr system clock control (system clock source, bus, core and slow dividers).
 *
 * @param base Register base address for the SCG instance.
 * @param base System clock configuration address.
 *
 * @retval None.
 */
void SCG_HW_ConfigVlprClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider)
{
    base->VLPRCLKCTRL.reg = (uint32_t)((uint32_t)((source & 0x0FU) << 24U) | (uint32_t)((coreDivider & 0x0FU) << 16U) | ((busDivider & 0x0FU) << 4U) | (uint32_t)(slowDivider & 0x0FU));
}

#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
/*!
 * @brief Config SCG hsrun system clock control (system clock source, bus, core and slow dividers).
 *
 * @param base Register base address for the SCG instance.
 * @param base System clock configuration address.
 *
 * @retval None.
 */
void SCG_HW_ConfigHsrunClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider)
{
    base->HSRUNCLKCTRL.reg = (uint32_t)((uint32_t)((source & 0x0FU) << 24U) | (uint32_t)((coreDivider & 0x0FU) << 16U) | ((busDivider & 0x0FU) << 4U) | (uint32_t)(slowDivider & 0x0FU));
}
#endif

/*!
 * @brief Read the first asynchronous divider for HSICLK.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval HSICLK first asynchronous divider
 */
uint32_t SCG_HW_ReadHsiFirstAsyncDivider(const SCG_T *base)
{
    return (base->HSIDIV.reg) & 0x07U;
}

/*!
 * @brief Read the second asynchronous divider for HSICLK.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval HSICLK second asynchronous divider
 */
uint32_t SCG_HW_ReadHsiSecondAsyncDivider(const SCG_T *base)
{
    return (base->HSIDIV.reg >> 8U) & 0x07U;
}


/*!
 * @brief Config SCG asynchronous dividers for HSICLK.
 *
 * @param base Register base address for the SCG instance.
 * @param base Asynchronous first divider.
 * @param base Asynchronous second divider.
 *
 * @retval None.
 */
void SCG_HW_ConfigHsiAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2)
{
    base->HSIDIV.reg &= (uint32_t)~((0x07U << 8U)| 0x07U);
    base->HSIDIV.reg |= (uint32_t)((uint32_t)((div2 & 0x07U) << 8U) | div1);
}


/*!
 * @brief Read the first asynchronous divider for LSICLK.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval LSICLK first asynchronous divider.
 */
uint32_t SCG_HW_ReadLsiFirstAsyncDivider(const SCG_T *base)
{
    return (base->LSIDIV.reg) & 0x07U;
}

/*!
 * @brief Read the second asynchronous divider for LSICLK.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval LSICLK second asynchronous divider.
 */
uint32_t SCG_HW_ReadLsiSecondAsyncDivider(const SCG_T *base)
{
    return (base->LSIDIV.reg >> 8U) & 0x07U;
}


/*!
 * @brief Config SCG asynchronous dividers for LSICLK.
 *
 * @param base Register base address for the SCG instance.
 * @param base Asynchronous first divider
 * @param base Asynchronous second divider
 *
 * @retval None.
 */
void SCG_HW_ConfigLsiAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2)
{
    base->LSIDIV.reg &= (uint32_t)~((0x07U << 8U)| 0x07U);
    base->LSIDIV.reg |= (uint32_t)((uint32_t)((div2 & 0x07U) << 8U) | div1);
}


/*!
 * @brief Read the first asynchronous divider for SYSOSC.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval SYSOSC first asynchronous divider
 */
uint32_t SCG_HW_ReadSoscFirstAsyncDivider(const SCG_T *base)
{
    return (base->SYSOSCDIVCFG.reg) & 0x07U;
}

/*!
 * @brief Read the second asynchronous divider for SYSOSC.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval SYSOSC second asynchronous divider
 */
uint32_t SCG_HW_ReadSoscSecondAsyncDivider(const SCG_T *base)
{
    return (base->SYSOSCDIVCFG.reg >> 8U) & 0x07U;
}


/*!
 * @brief Config SCG asynchronous dividers for SYSOSC.
 *
 * This function sets SCG asynchronous dividers for SYSOSC.
 *
 * @param base Register base address for the SCG instance.
 * @param base Asynchronous first divider
 * @param base Asynchronous second divider
 *
 * @retval None.
 */
void SCG_HW_ConfigSoscAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2)
{
    base->SYSOSCDIVCFG.reg &= (uint32_t)~((0x07U << 8U)| 0x07U);
    base->SYSOSCDIVCFG.reg |= (uint32_t)((uint32_t)((div2 & 0x07U) << 8U) | div1);
}

#if FEATURE_WITH_SYSPLL_CLK
/*!
 * @brief Read the first asynchronous divider for SYSPLL.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval SYSPLL first asynchronous divider
 */
uint32_t SCG_HW_ReadSysPllFirstAsyncDivider(const SCG_T *base)
{
    return (base->SYSPLLDIV.reg) & 0x07U;
}

/*!
 * @brief Read the second asynchronous divider for SYSPLL.
 *
 * @param base Register base address for the SCG instance.
 *
 * @retval SYSPLL second asynchronous divider
 */
uint32_t SCG_HW_ReadSysPllSecondAsyncDivider(const SCG_T *base)
{
    return (base->SYSPLLDIV.reg >> 8U) & 0x07U;
}


/*!
 * @brief Config SCG asynchronous dividers for SYSPLL.
 *
 * @param base Register base address for the SCG instance.
 * @param base Asynchronous first divider
 * @param base Asynchronous second divider
 *
 * @retval None.
 */
void SCG_HW_ConfigSysPllAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2)
{
    base->SYSPLLDIV.reg &= (uint32_t)~((0x07U << 8U)| 0x07U);
    base->SYSPLLDIV.reg |= (uint32_t)((uint32_t)((div2 & 0x07U) << 8U) | div1);
}
#endif

/*!
 * @brief Config SCG ClockOut source select
 *
 * @param base Register base address for the SCG instance.
 * @param source used for ClockOut
 *
 * @retval None.
 */
void SCG_HW_ConfigClockoutSourceSel(SCG_T *base, uint32_t source)
{
    base->SYSCLKOSEL.reg &= (uint32_t)~(0x0FU << 24U);
    base->SYSCLKOSEL.reg |= (uint32_t)((source & 0x0FU) << 24U);
}

/*!
 * @brief Read lsi system clock mode status,checks whether LSICLK is used as system clock source.
 *
 * @param base        scg base pointer
 *
 * @retval  lsi system clock mode status
 *        - false : LSICLK is not the system clock source
 *        - true : LSICLK is the system clock source
 */
bool SCG_HW_ReadLsiAsSystemClock(const SCG_T *const base)
{
    return (((base->LSICSTS.reg >> 25U) & 0x01U) == SCG_LSICSTS_LSISEL_0) ? false : true;
}

/*!
 * @brief Read lsi clock status, checks whether LSICLK is enabled and output clock is valid.
 *
 * @param base        scg base pointer
 *
 * @retval  lsi  clock status
 *        - false : LSICLK is not enabled or clock is not valid
 *        - true  : LSICLK is enabled and clock is valid
 */
bool SCG_HW_ReadLsiEnableStatus(const SCG_T *const base)
{
    return ((((base->LSICSTS.reg >> 24U) & 0x01U) != SCG_LSICSTS_LSIVLDFLG_0) ? true : false);
}

/*!
 * @brief Read lsi clock range
 *
 * @param base  scg base pointer
 *
 * @retval  lsi  range value
 */
uint32_t SCG_HW_ReadLsiRange(const SCG_T *const base)
{
    return base->LSIFSEL.reg & 0x01U;
}

/*!
 * @brief Clears LSICLK Lock Control Status Register
 *
 * @param base        scg base pointer
 *
 * @retval None.
 */
void SCG_HW_ClearLsiLockStatus(SCG_T *base)
{
    uint32_t value;
    /* Clear LOCK bit field */
    value = base->LSICSTS.reg;
    value &= (uint32_t)~(0x01U << 23U);
    base->LSICSTS.reg = value;
}


/*!
 * @brief Clears LSICLK Control Status Register
 *
 * @param base        scg base pointer
 *
 * @retval None.
 */
void SCG_HW_ClearLsiControlStatus(SCG_T *base)
{
    /* Clear configuration. */
    base->LSICSTS.reg = 0U;
}

/*!
 * @brief Config LSICLK configuration register
 *
 * @param base        scg base pointer
 * @param range       lsi range value
 *
 * @retval None.
 */
void SCG_HW_ConfigLsiRang(SCG_T *base, uint8_t range)
{
    uint32_t value;

    value = base->LSIFSEL.reg;
    value &= (uint32_t)~(0x01U);
    value |= (uint32_t)(range & 0x01U);
    base->LSIFSEL.reg = value;
}

/*!
 * @brief Config LSICLK control register
 *
 * @param base                scg base pointer
 * @param enableInStop        enable in stop modes
 * @param enableInLowPower    enable in very low power modes
 * @param lockMode            control register can be written or not
 *
 * @retval None.
 */
void SCG_HW_ConfigLsiControl(SCG_T *base, bool enableInStop, bool enableInLowPower, bool lockMode)
{
    uint32_t value;

    value = base->LSICSTS.reg;
    value &= (uint32_t)~((0x01U << 23U) | (0x01U << 2U) |(0x01U << 1U) | 0x01U);
    value |= (uint32_t)(((lockMode & 0x01U) << 23U) | ((enableInLowPower & 0x01U) << 2U) | ((enableInStop & 0x01U) << 1U) | (SCG_LSICSTS_LSIEN_1 & 0x01U));
    base->LSICSTS.reg = value;
}


/*!
 * @brief Read hsi system clock mode status
 *
 * @param base        scg base pointer
 *
 * @retval  hsi system clock mode status
 *        - false : HSICLK is not the system clock source
 *        - true : HSICLK is the system clock source
 */
bool SCG_HW_ReadHsiAsSystemClock(const SCG_T *const base)
{
    return ((((base->HSICSTS.reg >> 25U) & 0x01U) == SCG_HSICSTS_HSISEL_0) ? false : true);
}

/*!
 * @brief Read hsi clock status
 *
 * @param base   scg base pointer
 *
 * @retval  hsi  clock status
 *        - false : HSICLK is not enabled or clock is not valid
 *        - true  : HSICLK is enabled and clock is valid
 */
bool SCG_HW_ReadHsiEnableStatus(const SCG_T *const base)
{
    return ((((base->HSICSTS.reg >> 24U) & 0x01U) == SCG_HSICSTS_HSIVLDFLG_0) ? false : true);
}

/*!
 * @brief Read hsi clock range
 *
 * @param base  scg base pointer
 *
 * @retval  hsi  frequency range value
 */
uint32_t SCG_HW_ReadHsiRange(const SCG_T *const base)
{
    return base->HSIFSEL.reg & 0x03U;
}

/*!
 * @brief Clears HSICLK lock register
 *
 * This function clears HSICLK lock.
 *
 * @param base  scg base pointer
 *
 * @retval  None.
 *
 */
void SCG_HW_ClearHsiLockStatus(SCG_T *base)
{
    /* Clear LOCK bit field */
    base->HSICSTS.reg &= ~(uint32_t)((SCG_HSICSTS_LOCK_1 & 0x01U) << 23U);
}

/*!
 * @brief Clears HSICLK control register
 *
 * @param base        scg base pointer
 *
 * @retval  None.
 *
 */
void SCG_HW_ClearHsiControlStatus(SCG_T *base)
{
    /* Disable monitor, disable clock and clear error. */
    base->HSICSTS.reg = 0U;
    base->HSICSTS.reg |= (uint32_t)((SCG_HSICSTS_HSIEFLG_1 & 0x01U) << 26U);
}


/*!
 * @brief Config HSICLK configuration register
 *
 * @param base        scg base pointer
 * @param range       hsi range value
 *
 * @retval  None.
 */
void SCG_HW_ConfigHsiRang(SCG_T *base, uint8_t range)
{
    /* Set PLL configuration: frequency range */
    uint32_t value;

    value = base->HSIFSEL.reg;
    value &= (uint32_t)~0x03U;
    (void)range;
    base->HSIFSEL.reg = value;
}

/*!
 * @brief Config HSICLK control register
 *
 * @param base        scg base pointer
 * @param regulator   regulator enable or disable
 * @param lockMode    control register can be written or not
 *
 * @retval  None.
 */
void SCG_HW_ConfigHsiControl(SCG_T *base, bool regulator, bool lockMode)
{
    uint32_t value;

    value = base->HSICSTS.reg;
    value &= (uint32_t)~((0x01U << 23U) | (0x01U << 3U) | 0x01U);
    value |= (uint32_t)(((lockMode & 0x01U) << 23U) | (((regulator? 0U : 1U) & 0x01U) << 3U) | SCG_HSICSTS_HSIEN_1);
    base->HSICSTS.reg = value;
}


/*!
 * @brief Read sysOsc system clock mode status
 *
 * @param base        scg base pointer
 *
 * @retval  sysOsc system clock mode status
 *        - false : SYSOSC is not the system clock source
 *        - true : SYSOSC is the system clock source
 */
bool SCG_HW_ReadSoscAsSystemClock(const SCG_T *const base)
{
    return ((((base->SYSOSCCSTS.reg >> 25U) & 0x01U) == SCG_SYSOSCCSTS_SYSOSCSEL_0) ? false : true);
}

/*!
 * @brief Read sysOsc clock status, checks whether SYSOSC is enabled and output clock is valid.
 *
 * @param base        scg base pointer
 *
 * @retval  sysOsc  clock status
 *        - false : SYSOSC is not enabled or clock is not valid
 *        - true  : SYSOSC is enabled and clock is valid
 */
bool SCG_HW_ReadSoscEnableStatus(const SCG_T *const base)
{
    return ((((base->SYSOSCCSTS.reg >> 24U) & 0x01U) != SCG_SYSOSCCSTS_SYSOSCVLDFLG_0) ? true : false);
}

/*!
 * @brief Clears SYSOSC lock register
 *
 * @param base        scg base pointer
 *
 * @retval  None.
 */
void SCG_HW_ClearSoscLockStatus(SCG_T *base)
{
    /* Clear LOCK bit field */
    uint32_t value;

    value = base->SYSOSCCSTS.reg;
    value &= (uint32_t)~(0x01U << 23U);
    base->SYSOSCCSTS.reg = value;
}

/*!
 * @brief Clears SYSOSC control register
 *
 * @param base        scg base pointer
 *
 * @retval  None.
 */
void SCG_HW_ClearSoscControlStatus(SCG_T *base)
{
    /* Disable monitor, disable clock and clear error. */
    base->SYSOSCCSTS.reg = 0U;
    base->SYSOSCCSTS.reg |= (uint32_t)((SCG_SYSOSCCSTS_SYSOSCCLKEFLG_1 & 0x01U) << 26U);
}

/*!
 * @brief Config SYSOSC configuration register
 *
 * @param base        scg base pointer
 * @param range       sysOsc range
 * @param gain        sysOsc gain
 * @param extRef      external reference
 *
 * @retval  None.
 */
void SCG_HW_ConfigSoscCfg(SCG_T *base, uint32_t range, uint32_t gain, uint32_t extRef)
{
    /* Set SYSOSC configuration. */
    uint32_t value;

    value = base->SYSOSCCFG.reg;
    value &= (uint32_t)~((0x03U << 4U) | (0x01U << 3U) | (0x01U << 2U));
    value |= (uint32_t)(((range & 0x03U) << 4U) | ((gain & 0x01U) << 3U) | ((extRef & 0x01U) << 2U));
    base->SYSOSCCFG.reg = value;
}

/*!
 * @brief Config SYSOSC control register
 *
 * @param base        scg base pointer
 * @param monitorMode clock monitor enablement
 * @param resetMode   clock monitor reset enablement
 * @param lockMode    control register can be written or not
 *
 * @retval  None.
 */
void SCG_HW_ConfigSoscControl(SCG_T *base, bool monitorMode, bool resetMode, bool lockMode)
{
    uint32_t value;

    value = base->SYSOSCCSTS.reg;
    value &= (uint32_t)~((0x01U << 23U) | (0x01U << 17U) | (0x01U << 16U) | 0x01U);
    value |= (uint32_t)((uint32_t)((lockMode & 0x01U) << 23U) | (uint32_t)((resetMode & 0x01U) << 17U) | (uint32_t)((monitorMode & 0x01U) << 16U)| (uint32_t)SCG_SYSOSCCSTS_SYSOSCEN_1);
    base->SYSOSCCSTS.reg = value;
}

#if FEATURE_WITH_SYSPLL_CLK
/*!
 * @brief Read sysPll system clock mode status, checks whether SYSPLL is used as system clock source.
 *
 * @param base        scg base pointer
 *
 * @retval  sysPll system clock mode status
 *        - false : SYSPLL is not the system clock source
 *        - true : SYSPLL is the system clock source
 */
bool SCG_HW_ReadSysPllAsSystemClock(const SCG_T *const base)
{
    return ((((base->SYSPLLCSTS.reg >> 25U) & 0x01U) == SCG_SYSPLLCSTS_SYSPLLSEL_0) ? false : true);
}

/*!
 * @brief Read sysPll clock status, checks whether SYSPLL is enabled and output clock is valid.
 *
 * @param base        scg base pointer
 *
 * @retval  sysPll  clock status
 *        - false : SYSPLL is not enabled or clock is not valid
 *        - true  : SYSPLL is enabled and clock is valid
 */
bool SCG_HW_ReadSysPllEnableStatus(const SCG_T *const base)
{
    return ((((base->SYSPLLCSTS.reg >> 24U) & 0x01U) != SCG_SYSPLLCSTS_SYSPLLVLDFLG_0) ? true : false);
}

/*!
 * @brief Read sysPll predivider
 *
 * @param base        scg base pointer
 *
 * @retval  sysPll predivider value
 */
uint32_t SCG_HW_ReadSysPllPredivider(const SCG_T *const base)
{
    return (base->SYSPLLCFG.reg >> 8U) & 0x07U;
}

/*!
 * @brief Read sysPll multiplier
 *
 * @param base        scg base pointer
 *
 * @retval  sysPll multiplier value
 */
uint32_t SCG_HW_ReadSysPllMultiplier(const SCG_T *const base)
{
    return (base->SYSPLLCFG.reg >> 16U) & 0x001FU;
}

/*!
 * @brief Clears SYSPLL lock register
 *
 * @param base        scg base pointer
 *
 * @retval  None.
 */
void SCG_HW_ClearSysPllLockStatus(SCG_T *base)
{
    /* Clear LOCK bit field */
    uint32_t value;

    value = base->SYSPLLCSTS.reg;
    value &= (uint32_t)~(0x01U << 23U);
    base->SYSPLLCSTS.reg = value;
}


/*!
 * @brief Clears SYSPLL control register
 *
 * @param base        scg base pointer
 *
 * @retval  None.
 */
void SCG_HW_ClearSysPllControlStatus(SCG_T *base)
{
    /* Disable monitor, disable clock and clear error. */
    base->SYSPLLCSTS.reg = 0U;
    base->SYSPLLCSTS.reg |= (uint32_t)((SCG_SYSPLLCSTS_SYSPLLCLKEFLG_1 & 0x01U) << 26U);
}

/*!
 * @brief Config SYSPLL configuration register
 *
 * @param base        scg base pointer
 * @param divider     sysPll predivider value
 * @param multiplier  sysPll multiplier value
 *
 * @retval  None.
 */
void SCG_HW_ConfigSysPllCfg(SCG_T *base, uint8_t divider, uint8_t multiplier)
{
    /* Set PLL configuration. */
    uint32_t value = base->SYSPLLCFG.reg;
    value &= (uint32_t)~((0x1FU << 16U) | (0x07U << 8U));
    value |= (uint32_t)(((multiplier & 0x1FU) << 16U) | ((divider & 0x07U) << 8U));
    base->SYSPLLCFG.reg = value;
}

/*!
 * @brief Config SYSPLL control register
 *
 * @param base        scg base pointer
 * @param monitorMode clock monitor enablement
 * @param resetMode   clock monitor reset enablement
 * @param lockMode    control register can be written or not
 *
 * @retval  None.
 */
void SCG_HW_ConfigSysPllControl(SCG_T *base, bool monitorMode, bool resetMode, bool lockMode)
{
    uint32_t value = base->SYSPLLCSTS.reg;
    value &= (uint32_t)~((0x01U << 23U) | (0x01U << 17U) | (0x01U << 16U) | 0x01U);
    value |= (uint32_t)((uint32_t)((lockMode & 0x01U) << 23U) | (uint32_t)((resetMode & 0x01U) << 17U) | (uint32_t)((monitorMode & 0x01U) << 16U)| (uint32_t)(SCG_SYSPLLCSTS_SYSPLLEN_1 & 0x01U));
    base->SYSPLLCSTS.reg = value;
}
#endif

/**@} end of group CLOCK_Functions*/

/**@} end of group CLOCK_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/


