/*!
 * @file        user_clock_config.c
 *
 * @brief       This file provides all the setting of user clock
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

/* Included */
#include "user_clock_config.h"

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup LPI2C_TwoBoardsInterrupt_Slave
  @{
*/

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Variables Variables
  @{
*/
/*! @brief Peripheral clock configuration
 *
 * @details
 *    outputs:
 *           - {id: COMP_CLK.clkFreq : 48 MHz}
 *           - {id: CRC_CLK.clkFreq : 48 MHz}
 *           - {id: DMACHM_CLK.clkFreq : 48 MHz}
 *           - {id: EWDT_CLK.clkFreq : 48 MHz}
 *           - {id: PMA_CLK.clkFreq : 48 MHz}
 *           - {id: PMB_CLK.clkFreq : 48 MHz}
 *           - {id: PMC_CLK.clkFreq : 48 MHz}
 *           - {id: PMD_CLK.clkFreq : 48 MHz}
 *           - {id: PME_CLK.clkFreq : 48 MHz}
 *           - {id: RTC_CLK.clkFreq : 48 MHz}
 *           - {id: CAN0_CLK.clkFreq : 48 MHz}
 *           - {id: CAN1_CLK.clkFreq : 48 MHz}
 *           - {id: CAN2_CLK.clkFreq : 48 MHz}
 *           - {id: PDU0_CLK.clkFreq : 48 MHz}
 *           - {id: PDU1_CLK.clkFreq : 48 MHz}
 *           - {id: FLASH_CLK.clkFreq : 24 MHz}
 *           - {id: CFGTMR0_CLK.clkFreq : 8 MHz}
 *           - {id: CFGTMR1_CLK.clkFreq : 8 MHz}
 *           - {id: CFGTMR2_CLK.clkFreq : 8 MHz}
 *           - {id: CFGTMR3_CLK.clkFreq : 8 MHz}
 *           - {id: ADC0_CLK.clkFreq : 8 MHz}
 *           - {id: ADC1_CLK.clkFreq : 8 MHz}
 *           - {id: CFGIO_CLK.clkFreq : 8 MHz}
 *           - {id: LPI2C_CLK.clkFreq : 8 MHz}
 *           - {id: LPITMR_CLK.clkFreq : 8 MHz}
 *           - {id: LPSPI0_CLK.clkFreq : 8 MHz}
 *           - {id: LPSPI1_CLK.clkFreq : 8 MHz}
 *           - {id: LPSPI2_CLK.clkFreq : 8 MHz}
 *           - {id: LPTMR_CLK.clkFreq : 8 MHz}
 *           - {id: LPUART0_CLK.clkFreq : 8 MHz}
 *           - {id: LPUART1_CLK.clkFreq : 8 MHz}
 *           - {id: LPUART2_CLK.clkFreq : 8 MHz}
*/
PERIPHERAL_CLOCK_CONFIG_T g_peripheralClockConfig[PERIPHERAL_CLOCKS_CNT] = {
    {
        .clkName = COMP_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CRC_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = DMACHM_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = EWDT_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PMA_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PMB_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PMC_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PMD_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PME_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = RTC_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CAN0_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CAN1_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CAN2_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PDU0_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = PDU1_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = FLASH_CLK,
        .clkSrc = CLK_SRC_OFF,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CFGTMR0_CLK,
        .clkSrc = CLK_SRC_LSI_DIV1,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CFGTMR1_CLK,
        .clkSrc = CLK_SRC_LSI_DIV1,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CFGTMR2_CLK,
        .clkSrc = CLK_SRC_LSI_DIV1,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CFGTMR3_CLK,
        .clkSrc = CLK_SRC_LSI_DIV1,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = ADC0_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = ADC1_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = CFGIO_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPI2C_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPITMR_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPSPI0_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPSPI1_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPSPI2_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPTMR_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPUART0_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPUART1_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
    {
        .clkName = LPUART2_CLK,
        .clkSrc = CLK_SRC_LSI_DIV2,
        .clkGate = true,
        .div = DIV_BY_ONE,
        .frac = MUL_BY_ONE,
    },
};

/*! @brief User Configuration structure clock_managerCfg
 *
 * @details
 *    settings:
 *           - {id: LPO_1K_CLK.clkFreq : 1 kHz}
 *           - {id: LPO_CLK.clkFreq : 128 kHz}
 *           - {id: CLKOUT.clkFreq : 48 MHz}
 *           - {id: SCGCLKOUT.clkFreq : 48 MHz}
 *           - {id: SYSOSCDIV1_CLK.clkFreq : 8 MHz}
 *           - {id: SYSOSCDIV2_CLK.clkFreq : 8 MHz}
 *           - {id: LSICLKDIV1_CLK.clkFreq : 8 MHz}
 *           - {id: LSICLKDIV2_CLK.clkFreq : 8 MHz}
 *           - {id: HSICLKDIV1_CLK.clkFreq : 48 MHz}
 *           - {id: HSICLKDIV2_CLK.clkFreq : 48 MHz}
 *           - {id: SYSPLLDIV1_CLK.clkFreq : 56 MHz}
 *           - {id: SYSPLLDIV2_CLK.clkFreq : 28 MHz}
 *           - {id: SYSPLL_CLK_OUT.clkFreq : 112 MHz}
 *
 ***************Normal Run Mode *********************
 *           - {id: SYS_CLK.clkFreq : 48 MHz}
 *           - {id: CORE_CLK.clkFreq : 48 MHz}
 *           - {id: BUS_CLK.clkFreq : 48 MHz}
 *           - {id: FLASH_CLK.clkFreq : 24 MHz}
 *****************************************************
 *
 ***************High Speed Run Mode ******************
 *           - {id: SYS_CLK.clkFreq : 112 MHz}
 *           - {id: CORE_CLK.clkFreq : 112 MHz}
 *           - {id: BUS_CLK.clkFreq : 56 MHz}
 *           - {id: FLASH_CLK.clkFreq : 28 MHz}
 *****************************************************
 *
 *          - {id: SCG.HSICLKDIV1.scale, value: '1', locked: true}
 *          - {id: SCG.HSICLKDIV2.scale, value: '1', locked: true}
 *          - {id: SCG.LSICLKDIV1.scale, value: '1', locked: true}
 *          - {id: SCG.LSICLKDIV2.scale, value: '1', locked: true}
 *          - {id: SCG.SYSOSCDIV1.scale, value: '1', locked: true}
 *          - {id: SCG.SYSOSCDIV2.scale, value: '1', locked: true}
 *          - {id: SCG.SYSPLLDIV1.scale, value: '2', locked: true}
 *          - {id: SCG.SYSPLLDIV2.scale, value: '4', locked: true}
 *          - {id: SCG.SYSPLL_PREDIV.scale, value: '1', locked: true}
 *          - {id: SCG.SYSPLL_mul.scale, value: '28', locked: true}
 *          - {id: 'RUN:SCG.DIVCORE.scale', value: '1', locked: true}
 *          - {id: 'RUN:SCG.DIVBUS.scale', value: '1', locked: true}
 *          - {id: 'RUN:SCG.DIVSLOW.scale', value: '4', locked: true}
 *          - {id: 'RUN:SCG.SCSSEL.sel', value: SCG.HSI_CLK}
 *          - {id: 'HSRUN:SCG.DIVCORE.scale', value: '1', locked: true}
 *          - {id: 'HSRUN:SCG.DIVBUS.scale', value: '2', locked: true}
 *          - {id: 'HSRUN:SCG.DIVSLOW.scale', value: '4', locked: true}
 *          - {id: 'HSRUN:SCG.SCSSEL.sel', value: SCG.SYSPLL_CLK}
 *          - {id: 'VLPR:SCG.DIVCORE.scale', value: '2', locked: true}
 *          - {id: 'VLPR:SCG.DIVBUS.scale', value: '1', locked: true}
 *          - {id: 'VLPR:SCG.DIVSLOW.scale', value: '4', locked: true}
 *          - {id: 'VLPR:SCG.SCSSEL.sel', value: SCG.LSI_CLK}
 *          - {id: RTCCLKSEL.sel, value: SCG.HSICLKDIV1_CLK}
*/
CLOCK_MANAGER_USER_CONFIG_T g_clockConfig = {
    .scgClkCfg =
    {
        .sysOscCfg =
        {
            .init = true,
            .freq = 8000000U,                           /* System Oscillator frequency: 8000000Hz */
            .monitorMode = SCG_SYSOSC_MONITOR_DISABLE,    /* Monitor disabled */
            .locked = false,                            /* SYSOSC disabled */
            .extRef = SCG_SYSOSC_REF_OSC,                 /* Internal oscillator of OSC requested. */
            .gain = SCG_SYSOSC_GAIN_LOW,                  /* Configure crystal oscillator for low-gain operation */
            .range = SCG_SYSOSC_RANGE_HIGH,               /* High frequency range selected for the crystal oscillator of 8 MHz to 40 MHz. */
            .div1 = SCG_ASYNCLK_DIV_BY_1,               /* System OSC Clock Divider 1: divided by 1 */
            .div2 = SCG_ASYNCLK_DIV_BY_1,               /* System OSC Clock Divider 2: divided by 1 */
        },
        .lsiCfg =
        {
            .init = true,
            .enableInStop = true,               /* Enable LSI_CLK in stop mode */
            .enableInLowPower = true,           /* Enable LSI_CLK in low power mode */
            .locked = false,                    /* Unlocked */
            .range = SCG_LSICLK_RANGE_HIGH,     /* LSI_CLK high range clock (8 MHz) */
            .div1 = SCG_ASYNCLK_DIV_BY_1,       /* LSI_CLK Divider 1: divided by 1 */
            .div2 = SCG_ASYNCLK_DIV_BY_1,       /* LSI_CLK Divider 2: divided by 1 */
        },
        .hsiCfg =
        {
            .init = true,
            .regulator = true,                  /* HSI_CLK regulator is enabled */
            .locked = false,                    /* Unlocked */
            .range = SCG_HSICLK_RANGE_48M,      /*RANGE      */
            .div1 = SCG_ASYNCLK_DIV_BY_1,       /* HSI_CLK Divider 1: divided by 1 */
            .div2 = SCG_ASYNCLK_DIV_BY_1,       /* HSI_CLK Divider 2: divided by 1 */
        },
        .sysPllCfg =
        {
            .init = true,
            .monitorMode = SCG_SYSPLL_MONITOR_DISABLE,        /* Monitor disabled */
            .locked = false,                                /* Unlocked */
            .prediv = (uint8_t)SCG_SYSPLL_CLK_PREDIV_BY_1,    /* Divided by 1 */
            .mul = (uint8_t)SCG_SYSPLL_CLK_MUL_BY_28,         /* Multiply Factor is 28 */
            .src = 0U,
            .div1 = SCG_ASYNCLK_DIV_BY_2,                   /* System PLL Clock Divider 1: divided by 2 */
            .div2 = SCG_ASYNCLK_DIV_BY_4,                   /* System PLL Clock Divider 2: divided by 4 */
        },
        .clockModeCfg =
        {
            .init = true,
            .hcCtrCfg =
            {
                .src = SCG_SYS_CLOCK_SRC_SYS_PLL,    /* System PLL */
                .divCore = SCG_SYS_CLOCK_DIV_BY_1,   /* Core Clock Divider: divided by 1 */
                .divBus = SCG_SYS_CLOCK_DIV_BY_2,    /* Bus Clock Divider: divided by 2 */
                .divSlow = SCG_SYS_CLOCK_DIV_BY_4,   /* Slow Clock Divider: divided by 4 */
            },
            .rcCtrCfg =
            {
                .src = SCG_SYS_CLOCK_SRC_HSICLK,     /* HSI_CLK */
                .divCore = SCG_SYS_CLOCK_DIV_BY_1,   /* Core Clock Divider: divided by 1 */
                .divBus = SCG_SYS_CLOCK_DIV_BY_1,    /* Bus Clock Divider: divided by 1 */
                .divSlow = SCG_SYS_CLOCK_DIV_BY_2,   /* Slow Clock Divider: divided by 2 */
            },
            .vcCtrCfg =
            {
                .src = SCG_SYS_CLOCK_SRC_LSICLK,     /* LSI_CLK */
                .divCore = SCG_SYS_CLOCK_DIV_BY_2,   /* Core Clock Divider: divided by 2 */
                .divBus = SCG_SYS_CLOCK_DIV_BY_1,    /* Bus Clock Divider: divided by 1 */
                .divSlow = SCG_SYS_CLOCK_DIV_BY_4,   /* Slow Clock Divider: divided by 4 */
            },
        },
        .rtcCfg =
        {
            .init = false,
        },
        .clkOutCfg=
        {
            .init = true,
            .src = SCG_CLKOUT_SRC_HSICLK,                   /* HSI_CLK. */
        },
    },
    .simClkCfg =
    {
        .platGateClkCfg =
        {
            .init = true,
            .enableMsc = true,
            .enableMpu = true,
            .enableDma = true,
            .enableErep = true,
            .enableEinj = true,
        },
        .lpoClkCfg =
        {
            .init = true,
            .enableLpo1k = true,                            /* LPO1KCLKEN */
            .enableLpo32k = true,                           /*LPO32KCLKEN */
            .srcLpoClk = SIM_LPOCLK_SEL_LPO_128K,           /* 128 kHz LPO clock */
            .srcRtcClk = SIM_RTCCLK_SEL_SYSOSCDIV1_CLK,       /* SYSOSCDIV1 clock */
        },
        .clkOutCfg =
        {
            .init = true,
            .enable = true,                             /* Enabled */
            .src = SIM_CLKOUT_SEL_SYS_SCG_CLKOUT,       /* SCG CLKOUT clock select: SCG slow clock */
            .div = SIM_CLKOUT_DIV_BY_1,                 /* Divided by 1 */
        },
        .traceClkCfg =
        {
            .init = true,
            .divEnable = true,                  /*TRACEDIVEN */
            .src = CLOCK_TRACE_SRC_CORE_CLK,    /*TRACECLK_SEL */
            .div = 0U,                          /*TRACEDIV */
            .divFraction = false,               /*TRACEFRAC */
        },
        .tclkCfg =
        {
            .init = true,
            .tclkFreq[0] = 2000000U,    /*TCLK0 */
            .tclkFreq[1] = 0U,          /*TCLK1 */
            .tclkFreq[2] = 0U,          /*TCLK2 */
            .extPinSrc[0] = 0U,         /*CFGTMR0 ext pin source */
            .extPinSrc[1] = 0U,         /*CFGTMR1 ext pin source */
            .extPinSrc[2] = 0U,         /*CFGTMR2 ext pin source */
            .extPinSrc[3] = 0U,         /*CFGTMR3 ext pin source */
        },
    },
    .pmuClkCfg =
    {
        .lpoClkCfg =
        {
            .init = true,
            .enable = true,     /*Enable/disable LPO */
            .trimValue = 0,     /*Trimming value for LPO */
        },
    },
    .pcmClkCfg =
    {
        .count = PERIPHERAL_CLOCKS_CNT,              /*Number of the peripheral clock control configurations */
        .peripheralClocks = g_peripheralClockConfig,    /*Peripheral clock control configurations */
    },
};

/* Array of pointers to User configuration structures */
CLOCK_MANAGER_USER_CONFIG_T const * g_clockConfigsArr[] =
{
    &g_clockConfig
};

/* Callbacks configuration is empty. */
CLOCK_MANAGER_CALLBACK_USER_CONFIG_T * g_clockCallbacksArr[] =
{
    (void*)0
};

/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Variables*/
/**@} end of group LPI2C_TwoBoardsInterrupt_Slave*/
/**@} end of group Examples*/
