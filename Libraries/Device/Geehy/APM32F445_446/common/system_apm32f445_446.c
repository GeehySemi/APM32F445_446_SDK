/*!
 * @file        system_apm32f445_446.c
 *
 * @brief       CMSIS Cortex-M4 Device Peripheral Access Layer System Source File
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

#include "device.h"
#include "system_apm32f445_446.h"

/** @addtogroup STARTUP
  @{
*/

/** @addtogroup APM32F445_446_System
  @{
*/

/** @defgroup APM32F445_446_System_Variables Variables
  @{
*/

/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/**@} end of group APM32F445_446_System_Variables*/

/** @defgroup APM32F445_446_System_Functions Functions
  @{
*/

/**
 * @brief WDT DISABLE
 *
 * @details This function disables the watchdog, enables FPU
 * and the power mode protection if the corresponding feature macro
 * is enabled. SystemInit is called from startup_device file.
 *
 * @param None.
 *
 * @retval None.
 */
void WDT_Disable(void)
{
    /* Write of the WDT unlock key to CNT register, must be done in order to allow any modifications*/
    WDT->CNT.reg = (uint32_t ) FEATURE_WDT_UNLOCK_VALUE;
    /* The dummy read is used in order to make sure that the WDT registers will be configured only
    * after the write of the unlock value was completed. */
    (void)WDT->CNT.reg;

    /* Initial write of WDT configuration register:
    * enables support for 32-bit refresh/unlock command write words,
    * clock select from LPO, update enable, watchdog disabled.
    */
    WDT->CSTS.reg = 0x2920U;

    /* Configure timeout */
    WDT->TOVAL.reg = (uint32_t )0xFFFF;
}
/**
 * @brief SystemInit
 *
 * @details This function disables the watchdog, enables FPU
 * and the power mode protection if the corresponding feature macro
 * is enabled. SystemInit is called from startup_device file.
 *
 * @param None.
 *
 * @retval None.
 */
void SystemInit(void)
{
/* FPU ENABLE*/
#ifdef ENABLE_FPU
    /* Enable CP10 and CP11 coprocessors */
    APM32_SCB->CPACR.bit.CP10 = APM32_SCB_CPACR_CP10_11;
    APM32_SCB->CPACR.bit.CP11 = APM32_SCB_CPACR_CP11_11;

    /* Disable lazy context save of floating point state by clearing LSPEN bit */
    APM32_SCB->FPCCR.bit.LSPEN = APM32_SCB_FPCCR_LSPEN_0;
#endif /* ENABLE_FPU */


/* WDT DISABLE*/
#if (WDT_DISABLE)
    WDT_Disable();
#endif /* (WDT_DISABLE) */


/* ENABLE CACHE */
#if defined(I_CACHE) && (ICACHE_ENABLE == 1)
    /* Invalidate and enable code cache */
    LMC->CCTRL.bit.IW0 = LMC_CCTRL_IW0_1;
    LMC->CCTRL.bit.IW1 = LMC_CCTRL_IW1_1;
    LMC->CCTRL.bit.ICCMD = LMC_CCTRL_ICCMD_1;
    LMC->CCTRL.bit.CEN = LMC_CCTRL_CEN_1;

#endif /* defined(I_CACHE) && (ICACHE_ENABLE == 1) */

}

/**
 * @brief SystemCoreClockUpdate
 *
 * @details This function must be called whenever the core clock is changed
 * during program execution. It evaluates the clock register settings and calculates
 * the current core clock.
 *
 * @param None.
 *
 * @retval None.
 */
void SystemCoreClockUpdate(void)
{
    uint32_t SCGOutClk = 0U;
    uint32_t div, prediv, mul;
    uint32_t regValue;
    bool isSysClkSrcValid = true;
    static const uint32_t HsiClkFreq[] = { FEATURE_SCG_HSICLK_FREQUENCY, };

    div = SCG->CLKCFG.bit.CCLKDIVRCFG + 1;

    if (SCG->CLKCFG.bit.SYSCLKSSEL == 0x1U)        /* System OSC */
    {
        SCGOutClk = CPU_EXTERNAL_CLK_HZ;
    }
    else if (SCG->CLKCFG.bit.SYSCLKSSEL == 0x2U) /* Low IRC */
    {
        regValue = SCG->LSIFSEL.bit.LSIFSEL;
        if (regValue != 0U)
        {
            SCGOutClk = FEATURE_SCG_LSICLK_HIGH_RANGE_FREQUENCY;
        }
    }
    else if (SCG->CLKCFG.bit.SYSCLKSSEL == 0x3U) /* High IRC */
    {
        regValue = SCG->HSIFSEL.bit.HSIFSEL;
        SCGOutClk= HsiClkFreq[regValue];
    }
    else if (SCG->CLKCFG.bit.SYSCLKSSEL == 0x6U) /* System PLL */
    {
        SCGOutClk = CPU_EXTERNAL_CLK_HZ;
        prediv = (SCG->SYSPLLCFG.bit.PLLRDIVCFG) + 1U;
        mul = (SCG->SYSPLLCFG.bit.SYSPLLMULCFG) + 16U;
        SCGOutClk = SCGOutClk * mul / (prediv * 2U);
    }
    else
    {
        isSysClkSrcValid = false;
    }

    if (isSysClkSrcValid == true)
    {
        SystemCoreClock = (SCGOutClk / div);
    }
}

/**
 * @brief Configure SystemCoreClock
 *
 * @details
 *          OSC Frequency               8MHz
 *
 *          Normal Run Mode
 *          System Clock source         HSI (48MHz)
 *          CORE_CLK                    48MHz
 *          SYS_CLK                     48MHz
 *          BUS_CLK                     48MHz
 *          FLASH_CLK                   24MHz

 *          High Speed Run Mode
 *          System Clock source         SYSPLL (OSC 8MHz)
 *          VCO_CLK                     224MHz
 *          SYSPLL_CLK                    112MHz
 *          CORE_CLK                    112MHz
 *          SYS_CLK                     112MHz
 *          BUS_CLK                     56MHz
 *          FLASH_CLK                   28MHz
 *
 *          Very Low Power Run Mode
 *          System Clock source         LSI (8MHz)
 *          CORE_CLK                    4MHz
 *          SYS_CLK                     4MHz
 *          BUS_CLK                     4MHz
 *          FLASH_CLK                   1MHz
 *
 * @param None.
 *
 * @retval None.
 */
void SystemCoreClockConfigure(void)
{
    uint32_t value;

    /* System OSC Clock Monitor is enabled and detected an error */
    SCG->SYSOSCCSTS.reg |= (uint32_t)((uint32_t)((0x01U) << 26U));

    /* Set up OSC clock. */
    /*
     * Set System OSC Clock dividers
     * Divide1: 1, Divide by 1
     * Divide2: 1, Divide by 1
     */
    value = SCG->SYSOSCDIVCFG.reg;
    value &= (uint32_t)~((0x07U << 8U)| 0x07U);
    value |= (uint32_t)((uint32_t)((0x01U) << 8U) | 0x01U);
    SCG->SYSOSCDIVCFG.reg = value;

    /* Set OSC configuration.
     * Internal crystal oscillator of OSC selected (8MHz)
     * Configure crystal oscillator for low-gain operation
     * High frequency range selected for the crytstal oscillator
     */
    value = SCG->SYSOSCCFG.reg;
    value &= (uint32_t)~((0x03U << 4U)| 0x01U);
    value |= (uint32_t)((uint32_t)((0x03U) << 4U) | 0x01 << 2U);
    SCG->SYSOSCCFG.reg = value;

    /* Wait for the SYSOSCCSTS register can be writen*/
    while(SCG->SYSOSCCSTS.bit.LOCK == SCG_SYSOSCCSTS_LOCK_1)
    {
    }

    /* Enable System OSC clock. */
    SCG->SYSOSCCSTS.reg |= (uint32_t)(0x01U);

    /* Wait for OSC clock to be valid. */
    while (!(SCG->SYSOSCCSTS.reg & (uint32_t)(((0x01U) << 24U))));


    /* Init LSI */
    SCG->LSICSTS.reg = 0U;
    /*
     * Set Low speed internal Clock dividers
     * Divide1: 1, Divide by 1
     * Divide2: 1, Divide by 1
     */
    value = SCG->LSIDIV.reg;
    value &= (uint32_t)~((0x07U << 8U)| 0x07U);
    value |= (uint32_t)((uint32_t)((0x01U) << 8U) | 0x01U);
    SCG->LSIDIV.reg = value;

    /* Set LSI clock high frequency range(8 MHz)*/
    SCG->LSIFSEL.reg = 1U;

    /* Enable LSI clock. */
    SCG->LSICSTS.reg |= 0x01U;

    /* Wait for LSI clock to be valid. */
    while (!(SCG->LSICSTS.reg & (0x01U << 24U)))
    {}

    /* Init HSI */
    SCG->HSICSTS.reg = 0U;
    SCG->HSICSTS.reg |= (1U << 26U);
    /* Setup dividers. */
    /*
     * Set High speed internal Clock dividers
     * Divide1: 1, Divide by 1
     * Divide2: 1, Divide by 1
     */
    value = SCG->HSIDIV.reg;
    value &= (uint32_t)~((0x07U << 8U)| 0x07U);
    value |= (uint32_t)((uint32_t)((0x01U) << 8U) | 0x01U);
    SCG->HSIDIV.reg = value;

    /* Set HSI clock frequency range is trimmed to 48 MHz. */
    SCG->HSIFSEL.reg &= (uint32_t)~(0x03U);

    /* Enable clock. */
    SCG->HSICSTS.reg |= 0x01U;

    /* Wait for HSI clock to be valid. */
    while (!(SCG->HSICSTS.reg & (1U<<24)));


    /* Init SysPll */
    /* System PLL Clock Monitor is enabled and detected an error */
    SCG->SYSPLLCSTS.reg = 0U;
    SCG->SYSPLLCSTS.reg |= (1U << 26U);

    /* Setup dividers. */
    /*
     * Set SYSPLL_CLK dividers
     * Divide1: 1, Divide by 2
     * Divide2: 1, Divide by 4
     */
    value = SCG->SYSPLLDIV.reg;
    value &= (uint32_t)~((0x07U << 8U)| 0x07U);
    value |= (uint32_t)((uint32_t)((0x03U) << 8U) | 0x02U);
    SCG->SYSPLLDIV.reg = value;

    /* Set PLL configuration. */
    /*
     * Clock Source: System OSC (SYSOSC)
     * PLL Reference Clock Divider: Divided by 1
     * System PLL Multiplier: 28
     */
    /*SYSPLL Clock = Clock Source*(Multiplier+16) /(Divider+1) /2
                   =  8*(12+16)/(0+1) /2= 112MHz
     */
    value = SCG->SYSPLLCFG.reg;
    value &= (uint32_t)~((0x1FU << 16U) | (0x07U << 8U));
    value |= (uint32_t)((uint32_t)(0x0CU << 16U));
    SCG->SYSPLLCFG.reg = value;

    /* Enable PLL clock. */
    SCG->SYSPLLCSTS.reg |= 1U;

    /* Wait for PLL clock to be valid. */
    while (!(SCG->SYSPLLCSTS.reg & (1U << 24U)))
    {}

    /* Config use HSI as system clock source */
    /*
     * Select System Clock Source: HSI_CLK
     * Core Clock Divide: divided by 1
     * Bus Clock Divide: divided by 1
     * Slow Clock Divide: divided by 2
     */
   /*SYS_Clock = HSI_CLK /(Core Clock Divide) = 48MHz*/
    value = SCG->RUNCLKCTRL.reg;
    value &= (uint32_t)~((0x0FU << 24U) |(0x0FU << 16U) | (0x0FU << 4U)| 0x0FU);
    value |= (uint32_t)((uint32_t)((0x03U) << 24U) | 0x01U);
    SCG->RUNCLKCTRL.reg = value;

    /* Wait for clock source switch finished. */
    while ((SCG->RUNCLKCTRL.reg & 0x0F000000) != 0x03000000);


    /* Config use LSI as system clock source */
    /*
     * Select System Clock Source: LSI_CLK
     * Core Clock Divide: divided by 2
     * Bus Clock Divide: divided by 1
     * Slow Clock Divide: divided by 4
     */
   /*SYS_Clock = LSI_CLK /(Core Clock Divide) = 4MHz*/
    value = SCG->VLPRCLKCTRL.reg;
    value &= (uint32_t)~((0x0FU << 24U) |(0x0FU << 16U) | (0x0FU << 4U)| 0x0FU);
    value |= (uint32_t)((uint32_t)((0x02U) << 24U) | ((0x01U) << 16U) | 0x03U);
    SCG->VLPRCLKCTRL.reg = value;

    /* Wait for clock source switch finished. */
    while ((SCG->VLPRCLKCTRL.reg & 0x0F000000) != 0x02000000);

    /* Config use SYSPLL as system clock source */
    /*
     * Select System Clock Source: SYSPLL_CLK
     * Core Clock Divide: divided by 1
     * Bus Clock Divide: divided by 2
     * Slow Clock Divide: divided by 4
     */
   /*SYS_Clock = SYSPLL_CLK /(Core Clock Divide) = 112MHz*/
    value = SCG->HSRUNCLKCTRL.reg;
    value &= (uint32_t)~((0x0FU << 24U) |(0x0FU << 16U) | (0x0FU << 4U)| 0x0FU);
    value |= (uint32_t)((uint32_t)((0x06U) << 24U) | ((0x01U) << 4U) | 0x03U);
    SCG->HSRUNCLKCTRL.reg = value;

    /* Wait for clock source switch finished. */
    while ((SCG->HSRUNCLKCTRL.reg & 0x0F000000) != 0x06000000);
}

/**
 * @brief SystemSoftwareReset
 *
 * @details This function is used to initiate a system reset
 *
 * @param None.
 *
 * @retval None.
 */
void SystemSoftwareReset(void)
{
    uint32_t regValue;

    /* Clear register key */
    regValue = APM32_SCB->AIRCR.reg;
    regValue &= 0x0000FFFF;

    /* Configure System reset request bit and Register Key */
    regValue |= (FEATURE_SCB_VECTKEY << 16U) | (APM32_SCB_AIRCR_SYSRESETREQ_1 << 2);
    APM32_SCB->AIRCR.reg = regValue;
}

/**@} end of group APM32F445_446_System_Functions */
/**@} end of group APM32F445_446_System */
/**@} end of group CMSIS */

