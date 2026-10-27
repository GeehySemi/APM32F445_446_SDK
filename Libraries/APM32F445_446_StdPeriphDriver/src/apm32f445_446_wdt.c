/*!
 * @file        apm32f445_446_wdt.c
 *
 * @brief       This file provides all the WDT firmware functions
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
#include "apm32f445_446_wdt.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup WDT_Driver WDT Driver
  @{
*/

/** @defgroup WDT_Marcos Marcos
  @{
*/

/**@} end of group WDT_Marcos*/

/** @defgroup WDT_Enumerations Enumerations
  @{
*/

/**@} end of group WDT_Enumerations*/

/** @defgroup WDT_Structures Structures
  @{
*/

/**@} end of group WDT_Structures*/

/** @defgroup WDT_Variables Variables
  @{
*/

/*! @brief Table of base addresses for WDT instances. */
static WDT_T * const g_wdtBasePointer[] = { WDT };

/*! @brief Table to save WDT IRQ enum numbers defined in CMSIS header file. */
static const IRQn_Type g_wdtIrqNum[] = { WDT_EWDT_IRQn };

/**@} end of group WDT_Variables*/

/** @defgroup WDT_Functions Functions
  @{
*/
/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static uint32_t WDT_ReadClockSourceFreq(WDT_CLK_SRC_T wdtclk);


/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief  Configures the WDT driver
 *
 * @param ins: WDT peripheral ins number
 *
 * @param wdtConfig pointer to the WDT user configuration structure
 *
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_Init(uint32_t ins, const WDT_CONFIG_T * wdtcfg)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base          = g_wdtBasePointer[ins];
    STATUS_T state       = STATUS_SUCCESS;
    STATUS_T stateClkSrc = STATUS_SUCCESS;
    uint32_t prevClkHz, crtClkHz;

    /* Check if the previous clock source and the configuration clock source
     * are enabled(if not, the counter will not be incremented) */
    prevClkHz = WDT_ReadClockSourceFreq(WDT_HW_ReadClockSource(g_wdtBasePointer[ins]));
    crtClkHz  = WDT_ReadClockSourceFreq(wdtcfg->clkSrc);

    if ((prevClkHz == 0U) || (crtClkHz == 0U))
    {
        stateClkSrc = STATUS_ERROR;
    }

    if (stateClkSrc != STATUS_SUCCESS)
    {
        state = STATUS_ERROR;
    }
    else
    {
        /* If window mode enabled and window value greater than or equal to the timeout value. Or timeout value is 0 */
        if (((POINTER_IS_NULL(wdtcfg->windowEnable) != false) && (wdtcfg->windowValue >= wdtcfg->timeoutValue)) || \
            (wdtcfg->timeoutValue <= FEATURE_WDT_TIMEOUT_VALUE_MIN))
        {
            state = STATUS_ERROR;
        }
        else
        {
            /* Configure the WDT module */
            state = WDT_HW_Config(base, wdtcfg);
        }

        if (state == STATUS_SUCCESS)
        {
            /* Enable WDT timeout interrupt */
            INT_SYS_EnableIRQ(g_wdtIrqNum[ins]);
        }
    }

    return state;
}

/*!
 * @brief  Reset the WDT driver
 *
 * @param ins: WDT peripheral ins number
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures WDT module and de-initializes successful
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_DeInit(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    INT_SYS_DisableIRQGlobal();

    /* If allowed reconfigures WDT */
    if (!isEnable)
    {
        state = STATUS_ERROR;
    }
    else
    {
        /* Disable WDT timeout interrupt */
        INT_SYS_DisableIRQ(g_wdtIrqNum[ins]);

        /* Disable WDT */
        WDT_HW_Reset(base);
    }

    /* Enable global interrupt */
    INT_SYS_EnableIRQGlobal();

    return state;
}

/*!
 * @brief  Reads the current configuration of the WDT
 *
 * @param ins:  WDT peripheral ins number
 *
 * @param[out] configures  the current configuration
 *
 * @return None
 */
void WDT_ReadConfig(uint32_t ins, WDT_CONFIG_T * const wdtCfg)
{
    WDT_INSTANCE_VALIDITY(ins);

    const WDT_T *baseAddr = g_wdtBasePointer[ins];

    *wdtCfg = WDT_HW_ReadConfig(baseAddr);
}

/*!
 * @brief  Configures WDT_CONFIG_T's numbers default value
 *
 * @param[out] configures the default configuration
 *
 * @return None
 */
void WDT_DefaultConfig(WDT_CONFIG_T * const cfg)
{
    /* Construct CSTS register new value */
    cfg->clkSrc       = WDT_LPO_CLOCK;
    cfg->intEnable       = false;
    cfg->updateEnable    = true;
    cfg->windowEnable       = false;
    cfg->opMode.wait     = false;
    cfg->opMode.stop     = false;
    cfg->opMode.debug    = false;
    cfg->prescalerEnable = false;
    /* Construct WIN register new value */
    cfg->windowValue     = FEATURE_WDT_WIN_RESET_VALUE;
    /* Construct TOVAL register new value */
    cfg->timeoutValue    = FEATURE_WDT_TIMEOUT_VALUE;

}

/*!
 * @brief  Enables the WDT timeout interrupt and sets a function to be
 * called when a timeout interrupt is received, before reset
 *
 * @param ins: WDT peripheral ins number
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures WDT timeout interrupt
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_EnableInterrupt(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    /* If allowed reconfigures WDT */
    if (!isEnable)
    {
        state = STATUS_ERROR;
    }
    else
    {
        INT_SYS_DisableIRQGlobal();
        WDT_HW_EnableInterrupt(base);
        INT_SYS_EnableIRQGlobal();
    }

    return state;
}

/*!
 * @brief  Disables the WDT timeout interrupt
 *
 * @param ins: WDT peripheral ins number
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures WDT timeout interrupt
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_DisableInterrupt(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    /* If allowed reconfigures WDT */
    if (!isEnable)
    {
        state = STATUS_ERROR;
    }
    else
    {
        INT_SYS_DisableIRQGlobal();
        WDT_HW_DisableInterrupt(base);
        INT_SYS_EnableIRQGlobal();
    }

    return state;
}

/*!
 * @brief  Clear interrupt flag of the WDT
 *
 * @param ins: WDT peripheral ins number
 *
 * @return None
 */
void WDT_ClearIntFlag(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];

    WDT_HW_ClearIntFlag(base);
}

/*!
 * @brief  Refreshes the WDT counter
 *
 * @param ins: WDT peripheral ins number
 *
 * @return None
 */
void WDT_Refresh(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];

    INT_SYS_DisableIRQGlobal();
    WDT_HW_Refresh(base);
    INT_SYS_EnableIRQGlobal();
}

/*!
 * @brief  Reads the value of the WDT counter
 *
 * @param ins: WDT peripheral ins number
 *
 * @return the value of the WDT counter
 */
uint16_t WDT_ReadCounter(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    const WDT_T *base = g_wdtBasePointer[ins];

    return (uint16_t)base->CNT.reg;
}

/*!
 * @brief  Configures window mode and window value of the WDT
 *
 * @param ins: WDT peripheral ins number
 *
 * @param enable enable/disable window mode and window value
 *
 * @param windowvalue the value of the WDT window
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures window value success
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_ConfigWindow(uint32_t ins, bool enable, uint16_t winvalue)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    if (isEnable)
    {
        if (enable)
        {
            WDT_HW_EnableWindowMode(base);
            WDT_HW_ConfigWindowValue(base, winvalue);
        }
        else
        {
            WDT_HW_DisableWindowMode(base);
        }
    }
    else
    {
        state = STATUS_ERROR;
    }

    return state;
}

/*!
 * @brief  Configures the mode operation of the WDT
 *
 * @param ins: WDT peripheral ins number
 *
 * @param enable enable/disable mode of the WDT
 *
 * @param setMode select mode of the WDT
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures mode operation of the WDT
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_ConfigMode(uint32_t ins, bool enable, WDT_MODE_T cfgmode)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    if (isEnable)
    {
        if(enable)
        {
            if(cfgmode == WDT_MODE_DEBUG)
            {
                WDT_HW_EnableDebug(base);
            }
            else if(cfgmode == WDT_MODE_WAIT)
            {
                WDT_HW_EnableWait(base);
            }
            else if(cfgmode == WDT_MODE_STOP)
            {
                WDT_HW_EnableStop(base);
            }
        }
        else
        {
            if(cfgmode == WDT_MODE_DEBUG)
            {
                WDT_HW_DisableDebug(base);
            }
            else if(cfgmode == WDT_MODE_WAIT)
            {
                WDT_HW_DisableWait(base);
            }
            else if(cfgmode == WDT_MODE_STOP)
            {
                WDT_HW_DisableStop(base);
            }
        }
    }
    else
    {
        state = STATUS_ERROR;
    }

    return state;
}

/*!
 * @brief Configures the value of the WDT timeout
 *
 * @param ins: WDT peripheral ins number
 *
 * @param timeout the value of the WDT timeout
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures WDT timeout
 *        - STATUS_ERROR: Operation failed
 */
STATUS_T WDT_ConfigTimeout(uint32_t ins, uint16_t Timeout)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    if (isEnable)
    {
        WDT_UNLOCK(base);

        base->TOVAL.reg = (uint32_t)Timeout;
    }
    else
    {
        state = STATUS_ERROR;
    }

    return state;
}

/*!
 * @brief  Changes the WDT test mode
 *
 * @param ins: WDT peripheral ins number
 *
 * @param testMode: Test modes for the WDT
 *
 * @return operation status
 *        - STATUS_SUCCESS: if allowed reconfigures WDT test mode
 *        - STATUS_ERROR: Operation failed
 *
 * @note  If the WDT is tested in mode, software should set this field
 *        to 0x01U in order to indicate that the WDT is functioning normally
 */
STATUS_T WDT_ConfigTestMode(uint32_t ins, WDT_TEST_MODE_T TestModel)
{
    WDT_INSTANCE_VALIDITY(ins);

    WDT_T *base = g_wdtBasePointer[ins];
    uint32_t regValue = base->CSTS.reg;
    STATUS_T state = STATUS_SUCCESS;
    bool isEnable = WDT_HW_IsUpdateEnabled(base);

    /* If allowed reconfigures WDT */
    if (isEnable)
    {
        regValue &= ~(0x18U);
        regValue |= (((uint32_t)(((uint32_t)(TestModel))<<3U))&0x18U);

        WDT_UNLOCK(base);

        base->CSTS.reg = regValue;
    }
    else
    {
        state = STATUS_ERROR;
    }

    return state;
}

/*!
 * @brief  Reads the WDT in which test mode
 *
 * @param ins: WDT peripheral ins number
 *
 * @return Test modes for the WDT
 */
WDT_TEST_MODE_T WDT_ReadTestMode(uint32_t ins)
{
    WDT_INSTANCE_VALIDITY(ins);

    const WDT_T *base = g_wdtBasePointer[ins];
    WDT_TEST_MODE_T testModel = WDT_TEST_DISABLED;
    uint32_t regValue;

    /* Read test mode */
    regValue = WDT_HW_ReadTestMode(base);
    if(regValue == 0U)
    {
        testModel = WDT_TEST_DISABLED;
    }
    else if(regValue == 1U)
    {
        testModel = WDT_TEST_USER;
    }
    else if(regValue == 2U)
    {
        testModel = WDT_TEST_LOW;
    }
    else if(regValue == 3U)
    {
        testModel = WDT_TEST_HIGH;
    }
    else
    {
        testModel = WDT_TEST_DISABLED;
    }

    return testModel;
}

/*******************************************************************************
                        PRIVATE FUNCTIONS
*******************************************************************************/

/*!
 * @brief Read the frequency of the specified WDT clock source.
 *
 * @param wdtclk    value of WDT_CLK_SRC_T.
 *
 * @return frequency of clock source.
 */
static uint32_t WDT_ReadClockSourceFreq(WDT_CLK_SRC_T wdtclk)
{
    uint32_t frequency = 0;

    if(wdtclk == WDT_BUS_CLOCK)
    {
        (void)CLOCK_SYS_ReadFreq(BUS_CLK, &frequency);
    }
    else if(wdtclk == WDT_LSICLK_CLOCK)
    {
        (void)CLOCK_SYS_ReadFreq(LSICLK_CLK, &frequency);
    }
    else if(wdtclk == WDT_SYSOSC_CLOCK)
    {
        (void)CLOCK_SYS_ReadFreq(SYSOSC_CLK, &frequency);
    }
    else if(wdtclk == WDT_LPO_CLOCK)
    {
        (void)CLOCK_SYS_ReadFreq(SIM_LPO_CLK, &frequency);
    }

    return frequency;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Checks if the WDT is enabled
 *
 * @param base WDT base pointer
 *
 * @return the state of the WDT
 */
bool WDT_HW_IsEnabled(const WDT_T *base)
{
    return ((base->CSTS.reg & 0x80U) >> 7U) != 0U;
}

/*!
 * @brief Refreshes the WDT counter
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_Refresh(WDT_T * const base)
{
    /* check bit CMDEN32 */
    if ((base->CSTS.reg & 0x2000U) != 0U)
    {
        base->CNT.reg = FEATURE_WDT_TRIGGER_VALUE;
    }
    else
    {
        base->CNT.reg = FEATURE_WDT_TRIGGER16_FIRST_VALUE;
        (void)base->CNT.reg;
        base->CNT.reg = FEATURE_WDT_TRIGGER16_SECOND_VALUE;
    }
}

/*!
 * @brief Enables window mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_EnableWindowMode(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x8000U) | (((uint32_t)(((uint32_t)(1U))<<15U))&0x8000U);
}

/*!
 * @brief Disables window mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_DisableWindowMode(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x8000U) | (((uint32_t)(((uint32_t)(0U))<<15U))&0x8000U);
}

/*!
 * @brief Configures the value of the WDT window
 *
 * @param base WDT base pointer
 *
 * @param window the value of the WDT window
 *
 * @return None
 */
void WDT_HW_ConfigWindowValue(WDT_T * const base, uint16_t window)
{
    WDT_UNLOCK(base);

    base->WIN.reg = window;
}

/*!
 * @brief Clears the Interrupt Flag
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_ClearIntFlag(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x4000U) | (((uint32_t)(((uint32_t)(1U))<<14U))&0x4000U);

    /* Read-after-write sequence to guarantee required serialization of memory operations */
    (void)base->CSTS.reg;
}

/*!
 * @brief Reads the clock source of the WDT
 *
 * @param base WDT base pointer
 *
 * @return mode clock source of the WDT
 */
WDT_CLK_SRC_T WDT_HW_ReadClockSource(const WDT_T *base)
{
    uint32_t regValue = base->CSTS.reg;
    uint32_t clkSrcSel;
    WDT_CLK_SRC_T wdtClkSource;

    clkSrcSel = (regValue & 0x300U) >> 8U;
    if(clkSrcSel == 0U)
    {
        wdtClkSource = WDT_BUS_CLOCK;
    }
    else if(clkSrcSel == 1U)
    {
        wdtClkSource = WDT_LPO_CLOCK;
    }
    else if(clkSrcSel == 2U)
    {
        wdtClkSource = WDT_SYSOSC_CLOCK;
    }
    else if(clkSrcSel == 3U)
    {
        wdtClkSource = WDT_LSICLK_CLOCK;
    }
    else
    {
        wdtClkSource = WDT_BUS_CLOCK;
    }

    return wdtClkSource;
}

/*!
 * @brief Checks if the WDT updates are allowed
 *
 * @param base WDT base pointer
 *
 * @return false if updates are not allowed, true if updates are allowed
 */
bool WDT_HW_IsUpdateEnabled(const WDT_T *base)
{
    return ((base->CSTS.reg & 0x20U) >> 5U) != 0U;
}

/*!
 * @brief Enables WDT in debug mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_EnableDebug(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x4U) | (((uint32_t)(((uint32_t)(1U))<<2U))&0x4U);
}

/*!
 * @brief Disables WDT in debug mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_DisableDebug(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x4U) | (((uint32_t)(((uint32_t)(0U))<<2U))&0x4U);
}

/*!
 * @brief Enables WDT in wait mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_EnableWait(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x2U) | (((uint32_t)(((uint32_t)(1U))<<1U))&0x2U);
}

/*!
 * @brief Disables WDT in wait mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_DisableWait(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg = (base->CSTS.reg & ~0x2U) | (((uint32_t)(((uint32_t)(0U))<<1U))&0x2U);
}

/*!
 * @brief Enables WDT in stop mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_EnableStop(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg |= 0x1U;
}

/*!
 * @brief Disables WDT in stop mode
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_DisableStop(WDT_T * const base)
{
    WDT_UNLOCK(base);

    base->CSTS.reg &= ~0x1U;
}

/*!
 * @brief Checks if the WDT is unlocked
 *
 * @param base WDT base pointer
 *
 * @return true if the module is unlocked, false if the module is locked
 */
bool WDT_HW_IsUnlocked(const WDT_T *base)
{
    return ((base->CSTS.reg & 0x800U) >> 11U) != 0U;
}

/*!
 * @brief Checks if the last configuration of the WDT was successful
 *
 * @param base WDT base pointer
 *
 * @return true if the configuration was successful, false if not
 */
bool WDT_HW_IsReconfigurationComplete(const WDT_T *base)
{
    return ((base->CSTS.reg & 0x400U) >> 10U) != 0U;
}

/*!
 * @brief Reads test mode of the WDT
 *
 * @param base WDT base pointer
 *
 * @return test mode of the WDT
 */
uint32_t WDT_HW_ReadTestMode(const WDT_T *base)
{
    return ((base->CSTS.reg & 0x18U) >> 3U);
}

/*!
 * @brief Reset the WDT
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_Reset(WDT_T * const base)
{
    /* Unlock WDT register */
    WDT_UNLOCK(base);
    /*
     * Disable WDT,
     * enables support for 32-bit refresh/unlock command,
     * LPO clock source,
     * allow updates,
     * disable watchdog interrupts,
     * disable window mode,
     * disable wait/debug/stop mode
     */
    base->CSTS.reg = FEATURE_WDT_CSTS_RESET_VALUE;
    base->TOVAL.reg = FEATURE_WDT_TIMEOUT_VALUE;
    base->WIN.reg = FEATURE_WDT_WIN_RESET_VALUE;

    /* Refresh counter value */
    WDT_HW_Refresh(base);

    while (!WDT_HW_IsReconfigurationComplete(base))
    {}
}

/*!
 * @brief Enable the WDT timeout interrupt
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_EnableInterrupt(WDT_T * const base)
{
    /* Unlock WDT register */
    WDT_UNLOCK(base);

    /* Enable WDT interrupt */
    base->CSTS.reg = (base->CSTS.reg & ~0x40U) | (((uint32_t)(((uint32_t)(1U))<<6U))&0x40U);

    while (!WDT_HW_IsReconfigurationComplete(base))
    {}
}

/*!
 * @brief Disable the WDT timeout interrupt
 *
 * @param base WDT base pointer
 *
 * @return None
 */
void WDT_HW_DisableInterrupt(WDT_T * const base)
{
    /* Unlock WDT register */
    WDT_UNLOCK(base);

    /* Disable WDT interrupt */
    base->CSTS.reg = (base->CSTS.reg & ~0x40U) | (((uint32_t)(((uint32_t)(0U))<<6U))&0x40U);

    while (!WDT_HW_IsReconfigurationComplete(base))
    {}
}

/*!
 * @brief Configures the WDT
 *
 * @param base WDT base pointer
 *
 * @param wdtConfig pointer to user configuration structure
 *
 * @return the state of the WDT
 */
STATUS_T WDT_HW_Config(WDT_T * const base, const WDT_CONFIG_T *wdtcfg)
{
    STATUS_T state = STATUS_SUCCESS;
    uint32_t regValue = base->CSTS.reg;
    bool isEnable1 = WDT_HW_IsEnabled(base);
    bool isEnable2 = WDT_HW_IsUpdateEnabled(base);

    INT_SYS_DisableIRQGlobal();

    if ((isEnable1 == false) && (isEnable2 == true))
    {
        /* Clear the bits used for configuration */
        regValue &= ~(0x8000U | 0x1000U | 0x300U | 0x40U | 0x20U | 0x4U | 0x2U | 0x1U);
        /* Construct CS register new value */
        regValue |= (((uint32_t)(((uint32_t)(wdtcfg->windowEnable ? 1U : 0U))<<15U))&0x8000U);
        regValue |= (((uint32_t)(((uint32_t)(wdtcfg->prescalerEnable ? 1U : 0U))<<12U))&0x1000U);
        regValue |= (((uint32_t)(((uint32_t)(wdtcfg->clkSrc))<<8U))&0x300U);
        regValue |= (((uint32_t)(((uint32_t)(wdtcfg->intEnable ? 1U : 0U))<<6U))&0x40U);
        regValue |= (((uint32_t)(((uint32_t)(wdtcfg->updateEnable ? 1U : 0U))<<5U))&0x20U);
        if (wdtcfg->opMode.wait)
        {
            regValue |= 0x2U;
        }
        if (wdtcfg->opMode.stop)
        {
            regValue |= 0x1U;
        }
        if (wdtcfg->opMode.debug)
        {
            regValue |= 0x4U;
        }

        /* Reset interrupt flags */
        regValue |= 0x4000U;
        /* Enable WDT in 32-bit mode */
        regValue |= 0x80U | 0x2000U;

        WDT_UNLOCK(base);

        while (!WDT_HW_IsUnlocked(base))
        {}

        base->CSTS.reg = regValue;
        base->TOVAL.reg = wdtcfg->timeoutValue;
        if (wdtcfg->windowEnable)
        {
            base->WIN.reg = wdtcfg->windowValue;
        }

        while (WDT_HW_IsUnlocked(base))
        {}

        while (!WDT_HW_IsReconfigurationComplete(base))
        {}
    }
    else
    {
        state = STATUS_ERROR;
    }

    INT_SYS_EnableIRQGlobal();

    return state;
}

/*!
 * @brief Reads the current WDT configuration
 *
 * @param base WDT base pointer
 *
 * @return the current WDT configuration
 */
WDT_CONFIG_T WDT_HW_ReadConfig(const WDT_T *base)
{
    WDT_CONFIG_T wdt_cfg;
    uint32_t regValue = base->CSTS.reg;
    uint32_t clkSrcSel;

    /* Construct CS register new value */
    wdt_cfg.intEnable       = ((regValue & 0x40U) != 0U);
    wdt_cfg.updateEnable    = ((regValue & 0x20U) != 0U);
    wdt_cfg.windowEnable       = ((regValue & 0x8000U) != 0U);
    wdt_cfg.prescalerEnable = ((regValue & 0x1000U) != 0U);
    wdt_cfg.opMode.wait     = ((regValue & 0x2U) != 0U);
    wdt_cfg.opMode.stop     = ((regValue & 0x1U) != 0U);
    wdt_cfg.opMode.debug    = ((regValue & 0x4U) != 0U);
    wdt_cfg.windowValue     = (uint16_t)base->WIN.reg;
    wdt_cfg.timeoutValue    = (uint16_t)base->TOVAL.reg;

    clkSrcSel = (regValue & 0x300U) >> 8U;
    if(clkSrcSel == 0U)
    {
        wdt_cfg.clkSrc = WDT_BUS_CLOCK;
    }
    else if(clkSrcSel == 1U)
    {
        wdt_cfg.clkSrc = WDT_LPO_CLOCK;
    }
    else if(clkSrcSel == 2U)
    {
        wdt_cfg.clkSrc = WDT_SYSOSC_CLOCK;
    }
    else if(clkSrcSel == 3U)
    {
        wdt_cfg.clkSrc = WDT_LSICLK_CLOCK;
    }
    else
    {
        wdt_cfg.clkSrc = WDT_BUS_CLOCK;
    }

    return wdt_cfg;
}

/**@} end of group WDT_Functions*/
/**@} end of group WDT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
