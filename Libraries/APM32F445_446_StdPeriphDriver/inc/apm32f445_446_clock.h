/*!
 * @file        apm32f445_446_clock.h
 *
 * @brief       This file contains all the functions prototypes for the CLOCK firmware library.
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


#ifndef APM32F445_446_CLOCK_H
#define APM32F445_446_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"



/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CLOCK_Driver CLOCK Driver
  @{
*/

/** @defgroup CLOCK_Macros Macros
  @{
*/

/*! @brief TClk clock frequency. */
#define NUMBER_OF_TCLK_INPUTS 3U

/*! @brief The maximum number of system clock dividers and system clock divider indexes. */
#define SYS_CLK_MAX_INDEX    3U
#define CORE_CLK_INDEX       0U
#define BUS_CLK_INDEX        1U
#define SLOW_CLK_INDEX       2U

#define    CLK_SRC_OFF        0x00U             /*  Clock is off */
#define    CLK_SRC_SYSOSC       0x01U             /*  SYSOSCCLK - System Oscillator Bus Clock */
#define    CLK_SRC_LSI        0x02U             /*  LSICLK - Low speed internal Clock */
#define    CLK_SRC_HSI        0x03U             /*  HSICLK - High speed internal Clock */
#define    CLK_SRC_SYSPLL       0x06U             /*  SYSPLLCLK System PLL clock */
#define    CLK_SRC_SYSOSC_DIV1  0x01U             /*  SYSOSCCLK - System Oscillator Bus Clock */
#define    CLK_SRC_LSI_DIV1   0x02U             /*  LSICLK - Low speed internal Clock */
#define    CLK_SRC_HSI_DIV1   0x03U             /*  HSICLK - High speed internal Clock */
#define    CLK_SRC_SYSPLL_DIV1  0x06U             /*  SYSPLLCLK System PLL clock */
#define    CLK_SRC_SYSOSC_DIV2  0x01U             /*  SYSOSCCLK - System Oscillator Bus Clock */
#define    CLK_SRC_LSI_DIV2   0x02U             /*  LSICLK - Low speed internal Clock */
#define    CLK_SRC_HSI_DIV2   0x03U             /*  HSICLK - High speed internal Clock */
#define    CLK_SRC_SYSPLL_DIV2  0x06U             /*  SYSPLLCLK System PLL clock */
#ifdef FEATURE_WITH_LPO_PERIPHERAL_CLOCK_SOURCE
#define    CLK_SRC_LPO        0x07U             /*  LPO clock */
#endif

/**@} end of group CLOCK_Macros*/


/** @defgroup CLOCK_Variables Variables
  @{
*/

/*! @brief Peripheral features list
 *         Constant array storing the mappings between clock names of the peripherals and feature lists.
 */
extern const uint8_t peripheralFeaturesList[CLOCK_NAME_MAX_COUNT];
extern uint32_t g_tclkFreq[NUMBER_OF_TCLK_INPUTS];      /*  TCLKx clocks */
/*! @brief EXTAL0 clock frequency. */
extern uint32_t g_extal0ClkFreq;
/*! @brief RTC_CLKIN clock frequency. */
extern uint32_t g_rtcClkInFreq;

/**@} end of group CLOCK_Variables*/


/** @defgroup CLOCK_Enumerations Enumerations
  @{
*/
/*!
 * @brief SCG system clock type.
 */
typedef enum
{
    SCG_SYSCLK_CORE,  /*  Core clock.        */
    SCG_SYSCLK_BUS,   /*  BUS clock.         */
    SCG_SYSCLK_SLOW,  /*  System slow clock. */
    SCG_SYSCLK_MAX,   /*  Max value.         */
} SCG_SYSCLK_TYPE_T;

/*!
 * @brief SCG asynchronous clock type.
 */
typedef enum
{
    SCG_ASYNCLK_DIV1   = 0U,   /*  Clock divider 1  */
    SCG_ASYNCLK_DIV2   = 1U,   /*  Clock divider 2  */
    SCG_ASYNCLK_MAX    = 2U,   /*  Max value.       */
} SCG_ASYNCLK_TYPE_T;

/*!
 * @brief SCG system clock modes.
 */
typedef enum
{
    SCG_SYSCLK_MODE_CURRENT = 0U,  /*  Current mode.            */
    SCG_SYSCLK_MODE_RUN     = 1U,  /*  Run mode.                */
    SCG_SYSCLK_MODE_VLPR    = 2U,  /*  Very Low Power Run mode. */
    SCG_SYSCLK_MODE_HSRUN   = 3U,  /*  High Speed Run mode.     */
    SCG_SYSCLK_MODE_NONE           /*  MAX value.               */
} SCG_SYSCLK_MODE_T;
/*!
 * @brief SIM CLK32KSEL clock source select
 */
typedef enum
{
    SIM_RTCCLK_SEL_SYSOSCDIV1_CLK   = 0x0U,           /*  SYSOSCDIV1 clock          */
    SIM_RTCCLK_SEL_LPO_32K        = 0x1U,           /*  32 kHz LPO clock        */
    SIM_RTCCLK_SEL_RTC_CLKIN      = 0x2U,           /*  RTC_CLKIN clock         */
    SIM_RTCCLK_SEL_HSIDIV1_CLK    = 0x3U,           /*  HSIDIV1 clock           */
} SIM_RTC_CLK_SEL_SRC_T;

/*!
 * @brief SIM LPOCLKSEL clock source select
 */
typedef enum
{
    SIM_LPOCLK_SEL_LPO_128K    = 0x0,           /*  128 kHz LPO clock */
    SIM_LPOCLK_SEL_NO_CLOCK    = 0x1,           /*  No clock */
    SIM_LPOCLK_SEL_LPO_32K     = 0x2,           /*  32 kHz LPO clock which is divided by the 128 kHz LPO clock */
    SIM_LPOCLK_SEL_LPO_1K      = 0x3,           /*  1 kHz LPO clock which is divided by the 128 kHz LPO clock */
} SIM_LPOCLK_SEL_SRC_T;

/*!
 * @brief SIM CLKOUT select
 */
typedef enum
{
    SIM_CLKOUT_SEL_SYS_SCG_CLKOUT       = 0U,     /*  SCG CLKOUT                                   */
    SIM_CLKOUT_SEL_SYS_SYSOSC_DIV2_CLK    = 2U,     /*  SYSOSC DIV2 CLK                                */
    SIM_CLKOUT_SEL_SYS_LSI_DIV2_CLK     = 4U,     /*  LSI DIV2 CLK                                 */
    SIM_CLKOUT_SEL_SYS_HSI_DIV2_CLK     = 6U,     /*  HSI DIV2 CLK                                 */
    SIM_CLKOUT_SEL_SYS_HCLK             = 7U,     /*  HCLK                                         */
    SIM_CLKOUT_SEL_SYS_SYSPLL_DIV2_CLK    = 8U,     /*  SYSPLL DIV2 CLK                                */
    SIM_CLKOUT_SEL_SYS_BUS_CLK          = 9U,     /*  BUS_CLK                                      */
    SIM_CLKOUT_SEL_SYS_LPO_128K_CLK     = 10U,    /*  LPO_CLK 128 Khz                              */
    SIM_CLKOUT_SEL_SYS_LPO_CLK          = 12U,    /*  LPO_CLK as selected by SIM LPO CLK Select    */
    SIM_CLKOUT_SEL_SYS_RTC_CLK          = 14U,    /*  RTC CLK as selected by SIM CLK 32 KHz Select */
} SIM_CLKOUT_SEL_SRC_T;

/*!
 * @brief SIM CLKOUT divider
 */
typedef enum
{
    SIM_CLKOUT_DIV_BY_1 = 0x0U,        /*  Divided by 1 */
    SIM_CLKOUT_DIV_BY_2 = 0x1U,        /*  Divided by 2 */
    SIM_CLKOUT_DIV_BY_3 = 0x2U,        /*  Divided by 3 */
    SIM_CLKOUT_DIV_BY_4 = 0x3U,        /*  Divided by 4 */
    SIM_CLKOUT_DIV_BY_5 = 0x4U,        /*  Divided by 5 */
    SIM_CLKOUT_DIV_BY_6 = 0x5U,        /*  Divided by 6 */
    SIM_CLKOUT_DIV_BY_7 = 0x6U,        /*  Divided by 7 */
    SIM_CLKOUT_DIV_BY_8 = 0x7U,        /*  Divided by 8 */
} SIM_CLKOUT_DIV_T;


/*!
 * @brief Debug trace clock source select
 */
typedef enum
{
    CLOCK_TRACE_SRC_CORE_CLK     = 0x0          /*  core clock     */
} CLOCK_TRACE_SRC_T;


/*!
 * @brief SCG system clock source.
 */
typedef enum
{
    SCG_SYS_CLOCK_SRC_SYS_OSC    = 1U,     /*  System OSC. */
    SCG_SYS_CLOCK_SRC_LSICLK     = 2U,     /*  Low speed internal. */
    SCG_SYS_CLOCK_SRC_HSICLK     = 3U,     /*  High speed internal. */
#if FEATURE_WITH_SYSPLL_CLK
    SCG_SYS_CLOCK_SRC_SYS_PLL  = 6U,       /*  System PLL. */
#endif
    SCG_SYS_CLOCK_SRC_NONE     = 255U      /*  MAX value.  */
} SCG_SYS_CLOCK_SRC_T;

/*!
 * @brief SCG system clock divider value.
 */
typedef enum
{
    SCG_SYS_CLOCK_DIV_BY_1   = 0U,     /*  Divided by 1. */
    SCG_SYS_CLOCK_DIV_BY_2   = 1U,     /*  Divided by 2. */
    SCG_SYS_CLOCK_DIV_BY_3   = 2U,     /*  Divided by 3. */
    SCG_SYS_CLOCK_DIV_BY_4   = 3U,     /*  Divided by 4. */
    SCG_SYS_CLOCK_DIV_BY_5   = 4U,     /*  Divided by 5. */
    SCG_SYS_CLOCK_DIV_BY_6   = 5U,     /*  Divided by 6. */
    SCG_SYS_CLOCK_DIV_BY_7   = 6U,     /*  Divided by 7. */
    SCG_SYS_CLOCK_DIV_BY_8   = 7U,     /*  Divided by 8. */
    SCG_SYS_CLOCK_DIV_BY_9   = 8U,     /*  Divided by 9. */
    SCG_SYS_CLOCK_DIV_BY_10  = 9U,     /*  Divided by 10. */
    SCG_SYS_CLOCK_DIV_BY_11  = 10U,    /*  Divided by 11. */
    SCG_SYS_CLOCK_DIV_BY_12  = 11U,    /*  Divided by 12. */
    SCG_SYS_CLOCK_DIV_BY_13  = 12U,    /*  Divided by 13. */
    SCG_SYS_CLOCK_DIV_BY_14  = 13U,    /*  Divided by 14. */
    SCG_SYS_CLOCK_DIV_BY_15  = 14U,    /*  Divided by 15. */
    SCG_SYS_CLOCK_DIV_BY_16  = 15U,    /*  Divided by 16. */
} SCG_SYS_CLOCK_DIV_T;


/*!
 * @brief SCG ClockOut type.
 */
typedef enum
{
    SCG_CLKOUT_SRC_SCG_SLOW   = 0U,   /*  SCG SLOW.   */
    SCG_CLKOUT_SRC_SYSOSC       = 1U,   /*  System OSC. */
    SCG_CLKOUT_SRC_LSICLK     = 2U,   /*  Low speed internal. */
    SCG_CLKOUT_SRC_HSICLK     = 3U,   /*  High speed internal. */
    SCG_CLKOUT_SRC_SYSPLL       = 6U,   /*  System PLL. */
} SCG_CLKOUT_SRC_T;



/*!
 * @brief SCG asynchronous clock divider value.
 */
typedef enum
{
    SCG_ASYNCLK_DISABLE   = 0U,        /*  Clock output is disabled.  */
    SCG_ASYNCLK_DIV_BY_1  = 1U,        /*  Divided by 1.              */
    SCG_ASYNCLK_DIV_BY_2  = 2U,        /*  Divided by 2.              */
    SCG_ASYNCLK_DIV_BY_4  = 3U,        /*  Divided by 4.              */
    SCG_ASYNCLK_DIV_BY_8  = 4U,        /*  Divided by 8.              */
    SCG_ASYNCLK_DIV_BY_16 = 5U,        /*  Divided by 16.             */
    SCG_ASYNCLK_DIV_BY_32 = 6U,        /*  Divided by 32.             */
    SCG_ASYNCLK_DIV_BY_64 = 7U         /*  Divided by 64.             */
} SCG_ASYNCLK_DIV_T;


/*!
 * @brief SCG system OSC monitor mode.
 */
typedef enum
{
    SCG_SYSOSC_MONITOR_DISABLE = 0U,                         /*  Monitor disable.                          */
    SCG_SYSOSC_MONITOR_INT     = 1U,                         /*  Interrupt when system OSC error detected. */
    SCG_SYSOSC_MONITOR_RESET   = 2U,                         /*  Reset when system OSC error detected.     */
} SCG_SYSOSC_MONITOR_MODE_T;

/*!
 * @brief SCG OSC frequency range select
 */
typedef enum
{
    SCG_SYSOSC_RANGE_MID    = 2U,  /*  Medium frequency range selected for the crystal OSC (4 Mhz to 8 Mhz).  */
    SCG_SYSOSC_RANGE_HIGH   = 3U,  /*  High frequency range selected for the crystal OSC (8 Mhz to 40 Mhz).   */
} SCG_SYSOSC_RANGE_T;

/*!
 * @brief SCG OSC high gain oscillator select.
 */
typedef enum
{
    SCG_SYSOSC_GAIN_LOW    = 0x0,  /*  Configure crystal oscillator for low-power operation */
    SCG_SYSOSC_GAIN_HIGH   = 0x1,  /*  Configure crystal oscillator for high-gain operation */
} SCG_SYSOSC_GAIN_T;

/*!
 * @brief SCG OSC external reference clock select.
 */
typedef enum
{
    SCG_SYSOSC_REF_EXT   = 0x0,     /*  External reference clock requested    */
    SCG_SYSOSC_REF_OSC   = 0x1,     /*  Internal oscillator of OSC requested. */
} SCG_SYSOSC_EXT_REF_T;


/*!
 * @brief SCG lsi clock frequency range.
 */
typedef enum
{
    SCG_LSICLK_RANGE_HIGH = 1U,  /*  Low speed internal high range clock (8 MHz). */
} SCG_LSICLK_RANGE_T;

/*!
 * @brief SCG hsi clock frequency range.
 */
typedef enum
{
    SCG_HSICLK_RANGE_48M,   /*  High speed internal is trimmed to 48MHz.  */
} SCG_HSICLK_RANGE_T;

/*!
 * @brief SCG system PLL monitor mode.
 */
typedef enum
{
    SCG_SYSPLL_MONITOR_DISABLE = 0U,                         /*  Monitor disable.                          */
    SCG_SYSPLL_MONITOR_INT     = 1U,                         /*  Interrupt when system PLL error detected. */
    SCG_SYSPLL_MONITOR_RESET   = 2U                          /*  Reset when system PLL error detected.     */
} SCG_SYSPLL_MONITOR_MODE_T;


/*!
 * @brief SCG system PLL predivider.
 */
typedef enum
{
    SCG_SYSPLL_CLK_PREDIV_BY_1 = 0U,
    SCG_SYSPLL_CLK_PREDIV_BY_2 = 1U,
    SCG_SYSPLL_CLK_PREDIV_BY_3 = 2U,
    SCG_SYSPLL_CLK_PREDIV_BY_4 = 3U,
    SCG_SYSPLL_CLK_PREDIV_BY_5 = 4U,
    SCG_SYSPLL_CLK_PREDIV_BY_6 = 5U,
    SCG_SYSPLL_CLK_PREDIV_BY_7 = 6U,
    SCG_SYSPLL_CLK_PREDIV_BY_8 = 7U

} SCG_SYSPLL_CLK_PREDIV_T;

/*!
 * @brief SCG system PLL multiplier.
 */
typedef enum
{
    SCG_SYSPLL_CLK_MUL_BY_16 = 0U,
    SCG_SYSPLL_CLK_MUL_BY_17 = 1U,
    SCG_SYSPLL_CLK_MUL_BY_18 = 2U,
    SCG_SYSPLL_CLK_MUL_BY_19 = 3U,
    SCG_SYSPLL_CLK_MUL_BY_20 = 4U,
    SCG_SYSPLL_CLK_MUL_BY_21 = 5U,
    SCG_SYSPLL_CLK_MUL_BY_22 = 6U,
    SCG_SYSPLL_CLK_MUL_BY_23 = 7U,
    SCG_SYSPLL_CLK_MUL_BY_24 = 8U,
    SCG_SYSPLL_CLK_MUL_BY_25 = 9U,
    SCG_SYSPLL_CLK_MUL_BY_26 = 10U,
    SCG_SYSPLL_CLK_MUL_BY_27 = 11U,
    SCG_SYSPLL_CLK_MUL_BY_28 = 12U,
    SCG_SYSPLL_CLK_MUL_BY_29 = 13U,
    SCG_SYSPLL_CLK_MUL_BY_30 = 14U,
    SCG_SYSPLL_CLK_MUL_BY_31 = 15U,
    SCG_SYSPLL_CLK_MUL_BY_32 = 16U,
    SCG_SYSPLL_CLK_MUL_BY_33 = 17U,
    SCG_SYSPLL_CLK_MUL_BY_34 = 18U,
    SCG_SYSPLL_CLK_MUL_BY_35 = 19U,
    SCG_SYSPLL_CLK_MUL_BY_36 = 20U,
    SCG_SYSPLL_CLK_MUL_BY_37 = 21U,
    SCG_SYSPLL_CLK_MUL_BY_38 = 22U,
    SCG_SYSPLL_CLK_MUL_BY_39 = 23U,
    SCG_SYSPLL_CLK_MUL_BY_40 = 24U,
    SCG_SYSPLL_CLK_MUL_BY_41 = 25U,
    SCG_SYSPLL_CLK_MUL_BY_42 = 26U,
    SCG_SYSPLL_CLK_MUL_BY_43 = 27U,
    SCG_SYSPLL_CLK_MUL_BY_44 = 28U,
    SCG_SYSPLL_CLK_MUL_BY_45 = 29U,
    SCG_SYSPLL_CLK_MUL_BY_46 = 30U,
    SCG_SYSPLL_CLK_MUL_BY_47 = 31U
} SCG_SYSPLL_CLK_MUL_T;

/*!
 * @brief PCM fractional value select
 */
typedef enum
{
    MUL_BY_ONE   = 0x00U,             /*  Fractional value is zero */
    MUL_BY_TWO   = 0x01U              /*  Fractional value is one */
} PERIPHERAL_CLK_FRAC_T;

/*! @brief PCM divider value select
 */
typedef enum
{
    DIV_BY_ONE     = 0x00U,             /*  Divide by 1 (pass-through, no clock divide) */
    DIV_BY_TWO     = 0x01U,             /*  Divide by 2 */
    DIV_BY_THREE   = 0x02U,             /*  Divide by 3 */
    DIV_BY_FOUR    = 0x03U,             /*  Divide by 4 */
    DIV_BY_FIVE    = 0x04U,             /*  Divide by 5 */
    DIV_BY_SIX     = 0x05U,             /*  Divide by 6 */
    DIV_BY_SEVEN   = 0x06U,             /*  Divide by 7 */
    DIV_BY_EIGTH   = 0x07U              /*  Divide by 8 */
} PERIPHERAL_CLK_DIV_T;

/*!
 * @brief Power mode.
 */
typedef enum {

    NO_MODE       = 0U,
    RUN_MODE      = (1U<<0U),
    VLPR_MODE     = (1U<<1U),
    HSRUN_MODE    = (1U<<2U),
    STOP_MODE     = (1U<<3U),
    VLPS_MODE     = (1U<<4U),
    ALL_MODES     = 0x7FFFFFFF
} PWR_MODES_T;


/*!
 * @brief XOSC reference clock select (internal oscillator is bypassed or not)
 */
typedef enum
{
    XOSC_EXT_REF  = 0U,      /*  Internal oscillator is bypassed, external reference clock requested. */
    XOSC_INT_OSC  = 1U,      /*  Internal oscillator of XOSC requested. */
} XOSC_REF_T;

/*!
 * @brief The clock notification type.
 */
typedef enum
{
    CLOCK_MANAGER_NOTIFY_RECOVER = 0x00U,  /*  Notify IP to recover to previous work state.      */
    CLOCK_MANAGER_NOTIFY_BEFORE  = 0x01U,  /*  Notify IP that system will change clock setting.  */
    CLOCK_MANAGER_NOTIFY_AFTER   = 0x02U,  /*  Notify IP that have changed to new clock setting. */
} CLOCK_MANAGER_NOTIFY_T;

/*!
 * @brief The callback type, indicates what kinds of notification this callback handles.
 */
typedef enum
{
    CLOCK_MANAGER_CALLBACK_BEFORE       = 0x01U, /*  Callback handles BEFORE notification.          */
    CLOCK_MANAGER_CALLBACK_AFTER        = 0x02U, /*  Callback handles AFTER notification.           */
    CLOCK_MANAGER_CALLBACK_BEFORE_AFTER = 0x03U  /*  Callback handles BEFORE and AFTER notification */
} CLOCK_MANAGER_CALLBACK_TYPE_T;

/*!
 * @brief Clock transition policy.
 */
typedef enum
{
    CLOCK_MANAGER_POLICY_AGREEMENT,  /*  Clock transfers gracefully. */
    CLOCK_MANAGER_POLICY_FORCIBLE    /*  Clock transfers forcefully. */
} CLOCK_MANAGER_POLICY_T;

/**@} end of group CLOCK_Enumerations*/


/** @defgroup CLOCK_Structures Structures
  @{
*/
/*!
 * @brief SIM ClockOut configuration.
 */
typedef struct
{
    bool                  init;     /*  Initialize or not the ClockOut clock.  */
    bool                  enable;   /*  SIM ClockOut enable.                   */
    SIM_CLKOUT_SEL_SRC_T  src;      /*  SIM ClockOut source select.            */
    SIM_CLKOUT_DIV_T      div;      /*  SIM ClockOut divide ratio.             */
} SIM_CLKOUT_CONFIG_T;


/*!
 * @brief SIM LPO Clocks configuration.
 */
typedef struct
{
    bool                  init;          /*  Initialize or not the LPO clock.     */
    SIM_RTC_CLK_SEL_SRC_T srcRtcClk;     /*  RTC_CLK source select.               */
    SIM_LPOCLK_SEL_SRC_T  srcLpoClk;     /*  LPO clock source select.             */
    bool                  enableLpo32k;  /*  MSC Clock Gating Control enable.     */
    bool                  enableLpo1k;   /*  MSC Clock Gating Control enable.     */
} SIM_LPOCLK_CONFIG_T;

/*!
 * @brief SIM  Platform Gate Clock configuration.
 */
typedef struct
{
    bool      init;                               /*  Initialize or not the TCLK clock.   */
    uint32_t  tclkFreq[NUMBER_OF_TCLK_INPUTS];    /*  TCLKx frequency.                    */
    uint32_t  extPinSrc[CFGTMR_INSTANCE_COUNT];     /*  CFGTMRx frequency.                    */
} SIM_TCLK_CONFIG_T;

/*!
 * @brief SIM  Platform Gate Clock configuration.
 */
typedef struct
{
    bool init;           /*  Initialize or not the Trace clock.  */
    bool enableMsc;      /*  MSCM Clock Gating Control enable.   */
    bool enableMpu;      /*  MPU Clock Gating Control enable.    */
    bool enableDma;      /*  DMA Clock Gating Control enable.    */
    bool enableErep;     /*  EREP Clock Gating Control enable.   */
    bool enableEinj;     /*  EINJ Clock Gating Control enable.   */
} SIM_PLAT_GATE_CONFIG_T;

/*!
 * @brief SIM QSPI reference clock gating.
 */
typedef struct
{
    bool enableQspiRefClk;      /*  qspi internal reference clock gating control enable.          */
} SIM_QSPI_REF_CLK_GATING_T;
/*!
 * @brief SIM Debug Trace clock configuration.
 */
typedef struct
{
    bool               init;          /*  Initialize or not the Trace clock.  */
    bool               divEnable;     /*  Trace clock divider enable.         */
    CLOCK_TRACE_SRC_T  src;           /*  Trace clock select.                 */
    uint8_t            div;           /*  Trace clock divider divisor.        */
    bool               divFraction;   /*  Trace clock divider fraction.       */
} SIM_TRACRCLK_CONFIG_T;

/*!
 * @brief SIM configure structure.
 */
typedef struct
{
    SIM_CLKOUT_CONFIG_T    clkOutCfg;                 /*  Clock Out configuration.               */
    SIM_LPOCLK_CONFIG_T    lpoClkCfg;                 /*  Low Power Clock configuration.         */
    SIM_TCLK_CONFIG_T      tclkCfg;                   /*  TCLK, CFGTMR option Clock configuration. */
    SIM_PLAT_GATE_CONFIG_T platGateClkCfg;            /*  Platform Gate Clock configuration.     */
    SIM_TRACRCLK_CONFIG_T  traceClkCfg;               /*  Trace clock configuration.             */
    SIM_QSPI_REF_CLK_GATING_T qspiRefClkGating;       /*  Qspi Reference Clock Gating.           */
} SIM_CLOCK_CONFIG_T;
/*!
 * @brief SCG system clock configuration.
 */
typedef struct
{
    SCG_SYS_CLOCK_DIV_T divSlow;  /*  Slow clock divider.      */
    SCG_SYS_CLOCK_DIV_T divBus;   /*  BUS clock divider.       */
    SCG_SYS_CLOCK_DIV_T divCore;  /*  Core clock divider.      */
    SCG_SYS_CLOCK_SRC_T src;      /*  System clock source.     */
} SCG_SYS_CLK_CONFIG_T;
/*!
 * @brief SCG system OSC configuration.
 */
typedef struct
{
    uint32_t  freq;                        /*  System OSC frequency.                                */

    SCG_SYSOSC_MONITOR_MODE_T monitorMode;   /*  System OSC Clock monitor mode.                       */

    SCG_SYSOSC_EXT_REF_T extRef;             /*  System OSC External Reference Select.                */
    SCG_SYSOSC_GAIN_T    gain;               /*  System OSC high-gain operation.                      */

    SCG_SYSOSC_RANGE_T   range;              /*  System OSC frequency range.                          */

    SCG_ASYNCLK_DIV_T div1;                /*  Asynchronous peripheral source.                      */
    SCG_ASYNCLK_DIV_T div2;                /*  Asynchronous peripheral source.                      */

    bool enableInStop;                     /*  System OSC is enable or not in stop mode.            */
    bool enableInLowPower;                 /*  System OSC is enable or not in low power mode.       */

    bool locked;                           /*  System OSC Control Register can be written.          */

    bool init;                             /*  Initialize or not the System OSC module.             */
} SCG_SYSOSC_CONFIG_T;

/*!
 * @brief SCG lsi clock configuration.
 */
typedef struct
{
    SCG_LSICLK_RANGE_T range;       /*  Low speed internal frequency range.       */

    SCG_ASYNCLK_DIV_T div1;         /*  Asynchronous peripheral source.           */
    SCG_ASYNCLK_DIV_T div2;         /*  Asynchronous peripheral source.           */

    bool init;                      /*  Initialize or not the LSICLK module.      */
    bool enableInStop;              /*  LSICLK is enable or not in stop mode.     */
    bool enableInLowPower;          /*  LSICLK is enable or not in low power mode.*/

    bool locked;                    /*  LSICLK Control Register can be written.   */
} SCG_LSICLK_CONFIG_T;

/*!
 * @brief SCG hsi clock configuration.
 */
typedef struct
{
    SCG_HSICLK_RANGE_T range;      /*  High speed internal frequency range.      */

    SCG_ASYNCLK_DIV_T div1;        /*  Asynchronous peripheral source.           */
    SCG_ASYNCLK_DIV_T div2;        /*  Asynchronous peripheral source.           */

    bool enableInStop;             /*  HSICLK is enable or not in stop mode.     */
    bool enableInLowPower;         /*  HSICLK is enable or not in lowpower mode. */
    bool regulator;                /*  HSICLK regulator is enable or not.        */
    bool locked;                   /*  HSICLK Control Register can be written.   */

    bool init;                     /*  Initialize or not the HSICLK module.      */
} SCG_HSICLK_CONFIG_T;

/*!
 * @brief SCG system PLL configuration.
 */
typedef struct
{
    SCG_SYSPLL_MONITOR_MODE_T monitorMode; /*  Clock monitor mode selected.                    */

    uint8_t        prediv;               /*  PLL reference clock divider.                    */
    uint8_t        mul;                  /*  System PLL multiplier.                          */
    uint8_t        src;                  /*  System PLL source.                              */

    SCG_ASYNCLK_DIV_T div1;              /*  Asynchronous peripheral source.                 */
    SCG_ASYNCLK_DIV_T div2;              /*  Asynchronous peripheral source.                 */

    bool enableInStop;                   /*  System PLL clock is enable or not in stop mode. */

    bool locked;                         /*  System PLL Control Register can be written.     */
    bool init;                           /*  Initialize or not the System PLL module.        */
} SCG_SYSPLL_CONFIG_T;

/*!
 * @brief SCG RTC configuration.
 */
typedef struct
{
    uint32_t rtcClkInFreq;              /*  RTC_CLKIN frequency.                            */
    bool init;                          /*  Initialize or not the RTC.                      */
} SCG_RTC_CONFIG_T;

/*!
 * @brief SCG Clock Mode Configuration structure.
 */
typedef struct
{
    SCG_SYS_CLK_CONFIG_T rcCtrCfg;          /*  Run Clock Control configuration.                 */
    SCG_SYS_CLK_CONFIG_T vcCtrCfg;          /*  VLPR Clock Control configuration.                */
    SCG_SYS_CLK_CONFIG_T hcCtrCfg;          /*  HSRUN Clock Control configuration.               */
    SCG_SYS_CLOCK_SRC_T    alternateClock;  /*  Alternate clock used during initialization       */
    bool                      init;         /*  Initialize or not the Clock Mode Configuration.  */
} SCG_CLOCK_MODE_CONFIG_T;

/*!
 * @brief SCG ClockOut Configuration structure.
 */
typedef struct
{
    SCG_CLKOUT_SRC_T        src;       /*  ClockOut source select.                          */
    bool                    init;      /*  Initialize or not the ClockOut.                  */
} SCG_CLOCKOUT_CONFIG_T;

/*!
 * @brief SCG configure structure.
 */
typedef struct
{
    SCG_LSICLK_CONFIG_T       lsiCfg;       /*  Low internal reference clock configuration.      */
    SCG_HSICLK_CONFIG_T       hsiCfg;       /*  High internal reference clock configuration.     */
    SCG_SYSOSC_CONFIG_T         sysOscCfg;      /*  System oscillator configuration.                 */
    SCG_SYSPLL_CONFIG_T       sysPllCfg;    /*  System Phase locked loop configuration.          */
    SCG_RTC_CONFIG_T          rtcCfg;       /*  Real Time Clock configuration.                   */
    SCG_CLOCKOUT_CONFIG_T     clkOutCfg;    /*  SCG ClockOut Configuration.                      */
    SCG_CLOCK_MODE_CONFIG_T   clockModeCfg; /*  SCG Clock Mode Configuration.                    */
} SCG_CONFIG_T;

/*! @brief PCM clock source select
 */
typedef uint8_t PERIPHERAL_CLOCK_SRC_T;

/*!
 * @brief PCM peripheral instance clock configuration.
 */
typedef struct
{
    CLOCK_NAMES_T clkName;
    bool clkGate;                                   /*  Peripheral clock gate.                     */
    PERIPHERAL_CLOCK_SRC_T clkSrc;                  /*  Peripheral clock source.                   */
    PERIPHERAL_CLK_FRAC_T frac;                     /*  Peripheral clock fractional value.         */
    PERIPHERAL_CLK_DIV_T div;                       /*  Peripheral clock divider value.            */
} PERIPHERAL_CLOCK_CONFIG_T;

/*!
 * @brief PCM configuration.
 */
typedef struct
{
    uint32_t count;                                     /*  Number of peripherals to be configured.               */
    PERIPHERAL_CLOCK_CONFIG_T * peripheralClocks;       /*  Pointer to the peripheral clock configurations array. */
} PCM_CONFIG_T;

/*!
 * @brief PMU LPO configuration.
 */
typedef struct
{
    bool                  init;             /*  Initialize or not the PMU LPO settings. */
    bool                  enable;           /*  Enable/disable LPO     */
    int8_t                trimValue;        /*  LPO trimming value     */
} PMU_LPOCLKC_CONFIG_T;

/*!
 * @brief PMU configure structure.
 */
typedef struct
{
    PMU_LPOCLKC_CONFIG_T lpoClkCfg;   /*  Low Power Clock configuration.     */
} PMU_CONFIG_T;

/*!
 * @brief Clock configuration structure.
 */
typedef struct
{
    SCG_CONFIG_T                scgClkCfg;      /*  SCG Clock configuration.      */
    SIM_CLOCK_CONFIG_T          simClkCfg;      /*  SIM Clock configuration.      */
    PCM_CONFIG_T                pcmClkCfg;      /*  PCM Clock configuration.      */
    PMU_CONFIG_T                pmuClkCfg;      /*  PMU Clock configuration.      */
} CLOCK_MANAGER_USER_CONFIG_T;

typedef CLOCK_MANAGER_USER_CONFIG_T CLOCK_USER_CONFIG_T;

/*! @brief module clock configuration.
 */
typedef struct
{
    bool          gating;                     /*  Clock gating. */
    CLOCK_NAMES_T src;                        /*  Clock source input (some modules don't have protocol clock) */
    uint16_t      mul;                        /*  Multiplier (some modules don't have fractional) */
    uint16_t      div;                        /*  Divider (some modules don't have divider) */

}MODULE_CLK_CONFIG_T;

/*!
 * @brief System clock configuration.
 */
typedef struct
{
    CLOCK_NAMES_T src;                         /*  System clock source. */
    uint16_t dividers[SYS_CLK_MAX_INDEX];      /*  System clock dividers. Value by which system clock is divided. 0 means that system clock is not divided. */
} SYS_CLK_CONFIG_T;

/*!
 * @brief Clock source configuration.
 */
typedef struct
{
    bool       enable;                   /*  Enable/disable clock source. */
    XOSC_REF_T refClk;                   /*  Bypass option. It applies to external oscillator clock sources                          */
    uint32_t   refFreq;                  /*  Frequency of the input reference clock. It applies to external oscillator clock sources */
    uint16_t   mul;                      /*  Multiplier. It applies to PLL clock sources. Valid range is 16 - 47. */
    uint16_t   div;                      /*  Divider. It applies to PLL clock sources. Valid range is 1-8. */

    uint16_t   outputDiv1;               /*  First output divider. It's used as protocol clock by modules. Zero means that divider is disabled.   /
                                          *   Possible values 0(disabled), 1, 2, 4, 8, 16, 32, 64; all the other values are not valid.             /
                                          */
    uint16_t   outputDiv2;               /*  Second output divider. It's used as protocol clock by modules. Zero means that divider is disabled.   /
                                          *   Possible values 0(disabled), 1, 2, 4, 8, 16, 32, 64; all the other values are not valid.              /
                                          */
} CLOCK_SRC_CONFIG_T;

/*!
 * @brief Clock notification structure passed to clock callback function.
 */
typedef struct
{
    uint8_t targetClkCfgIdx;           /*  Target clock configuration index. */
    CLOCK_MANAGER_POLICY_T policy;     /*  Clock transition policy.          */
    CLOCK_MANAGER_NOTIFY_T notifyType; /*  Clock notification type.          */
} CLOCK_NOTIFY_STRUCT_T;

/*!
 * @brief Type of clock callback functions.
 */
typedef STATUS_T (*CLOCK_MANAGER_CALLBACK_T)(CLOCK_NOTIFY_STRUCT_T *notify, void* callbackData);

/*!
 * @brief Structure for callback function and its parameter.
 */
typedef struct
{
    CLOCK_MANAGER_CALLBACK_T      callback;      /*  Entry of callback function.     */
    CLOCK_MANAGER_CALLBACK_TYPE_T callbackType;  /*  Callback type.                  */
    void* callbackData;                          /*  Parameter of callback function. */
} CLOCK_MANAGER_CALLBACK_USER_CONFIG_T;

/*!
 * @brief Clock manager state structure.
 */
typedef struct
{
    CLOCK_MANAGER_USER_CONFIG_T const **cfgTable;   /*Pointer to clock configure table.*/
    uint8_t clockConfigNum;                         /*  Number of clock configurations.  */
    uint8_t curConfigIndex;                         /*  Index of current configuration.  */
    CLOCK_MANAGER_CALLBACK_USER_CONFIG_T **callbackConfig; /*  Pointer to callback table.*/
    uint8_t callbackNum;                            /*  Number of clock callbacks.       */
    uint8_t errorCallbackIndex;                     /*  Index of callback returns error. */
} CLOCK_MANAGER_STATE_T;

/**@} end of group CLOCK_Structures*/

/** @defgroup CLOCK_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CLOCK_SYS_ClockManagerInit(CLOCK_MANAGER_USER_CONFIG_T const **clockConfigsPtr,
                                            uint8_t configsNumber,
                                            CLOCK_MANAGER_CALLBACK_USER_CONFIG_T **callbacksPtr,
                                            uint8_t callbacksNumber);

STATUS_T CLOCK_SYS_Init(CLOCK_MANAGER_USER_CONFIG_T const * clockConfig);

STATUS_T CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIndex, CLOCK_MANAGER_POLICY_T policy);

STATUS_T CLOCK_SYS_ConfigClockSource(CLOCK_NAMES_T clockSource, const CLOCK_SRC_CONFIG_T * clockSourceConfig);

STATUS_T CLOCK_SYS_ConfigSystemClock(const PWR_MODES_T * mode, const SYS_CLK_CONFIG_T * systemClockConfig);

void CLOCK_SYS_ConfigModuleClock(CLOCK_NAMES_T peripheralClock, const MODULE_CLK_CONFIG_T * moduleClockConfig);

void CLOCK_SYS_ReadSystemClockSource(SYS_CLK_CONFIG_T *systemClockConfig);

uint8_t CLOCK_SYS_ReadCurrentClockConfigIndex(void);

CLOCK_MANAGER_CALLBACK_USER_CONFIG_T *CLOCK_SYS_ReadErrorCallback(void);

STATUS_T CLOCK_SYS_ReadFreq(CLOCK_NAMES_T clockName, uint32_t *frequency);


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

uint32_t SIM_HW_ReadRtcClkSrc(const SIM_T *base);
void SIM_HW_ConfigLpoClocks(SIM_T *base, bool enableLpo1k, bool enableLpo32k, uint32_t srcLpoClk, uint32_t srcRtcClk);
void SIM_HW_ConfigEinjClockGate(SIM_T *base, bool enable);
void SIM_HW_EnableEinjClock(SIM_T *base);
void SIM_HW_DisableEinjClock(SIM_T *base);
bool SIM_HW_ReadEinjClockGate(const SIM_T *base);
void SIM_HW_SetErepClockGate(SIM_T *base, bool enable);
void SIM_HW_EnableErepClock(SIM_T *base);
void SIM_HW_DisableErepClock(SIM_T *base);
bool SIM_HW_ReadErepClockGate(const SIM_T *base);
void SIM_HW_ConfigDmaClockGate(SIM_T *base, bool enable);
void SIM_HW_EnableDmaClock(SIM_T *base);
void SIM_HW_DisableDmaClock(SIM_T *base);
bool SIM_HW_ReadDmaClockGate(const SIM_T *base);
void SIM_HW_ConfigMpuClockGate(SIM_T *base, bool enable);
void SIM_HW_EnableMpuClock(SIM_T *base);
void SIM_HW_DisableMpuClock(SIM_T *base);
bool SIM_HW_ReadMpuClockGate(const SIM_T *base);
void SIM_HW_ConfigMscmClockGate(SIM_T *base, bool enable);
void SIM_HW_EnableMscmClock(SIM_T *base);
void SIM_HW_DisableMscmClock(SIM_T *base);
bool SIM_HW_ReadMscmClockGate(const SIM_T *base);
uint32_t SIM_HW_ReadCFGtmr0ExternalClkPinMode(const SIM_T *base);
uint32_t SIM_HW_ReadCFGtmr1ExternalClkPinMode(const SIM_T *base);
uint32_t SIM_HW_ReadCFGtmr2ExternalClkPinMode(const SIM_T *base);
uint32_t SIM_HW_ReadCFGtmr3ExternalClkPinMode(const SIM_T *base);
bool SIM_HW_ReadLpo32kClkStatus(const SIM_T *base);
bool SIM_HW_ReadLpo1kClkStatus(const SIM_T *base);
uint32_t SIM_HW_ReadLpoClkSelectedSource(const SIM_T *base);
bool SIM_HW_ReadClockoutStatus(const SIM_T *base);
uint32_t SIM_HW_ReadClockoutDividerValue(const SIM_T *base);
uint32_t SIM_HW_ReadClockoutSelectedSource(const SIM_T *base);
void SIM_HW_ConfigExtPinSourceCFGtmr(SIM_T *base, uint8_t instance, uint32_t source);
void SIM_HW_ConfigClockout(SIM_T *base, bool enable, uint32_t source, uint32_t divider);
void SIM_HW_ConfigTraceClockSource(SIM_T *base, uint32_t clkTraceSrc);
void SIM_HW_ClearTraceClockConfig(SIM_T *base);
void SIM_HW_ConfigTraceClock(SIM_T *base, bool enable, uint32_t divider, uint32_t multiplier);

uint32_t SYSMC_HW_ReadCurrentRunningPowerMode(const SYSMC_T *const baseAddr);

void PCM_HW_ConfigPeripheralClockControl(PCM_T *base, CLOCK_NAMES_T clockName, bool clockGate, uint32_t clockSource, uint32_t divider, uint32_t multiplier);
void PCM_HW_ConfigPeripheralClkEn(PCM_T *const base, const CLOCK_NAMES_T clockName, const bool isClockEnabled);
bool PCM_HW_ReadPeripheralClkEn(const PCM_T *const base, const CLOCK_NAMES_T clockName);
uint32_t PCM_HW_ReadPeripheralClockSourceSel(const PCM_T *const base, const CLOCK_NAMES_T clockName);
uint32_t PCM_HW_ReadPeripheralFractionValue(const PCM_T *const base, const CLOCK_NAMES_T clockName);
uint32_t PCM_HW_ReadPeripheralDividerValue(const PCM_T *const base, const CLOCK_NAMES_T clockName);

void PMU_HW_ConfigLpoDisable(PMU_T *const baseAddr, const bool enable);
bool PMU_HW_ReadLpoDisableMode(const PMU_T *const baseAddr);
void PMU_HW_ConfigLpoTrimValue(PMU_T *const baseAddr, const int8_t decimalValue);

uint32_t SCG_HW_ReadClockoutSourceSel(const SCG_T *base);
uint32_t SCG_HW_ReadCurrentSystemClockSource(const SCG_T *base);
uint32_t SCG_HW_ReadCurrentCoreClockDividerRatio(const SCG_T *base);
uint32_t SCG_HW_ReadCurrentBusClockDividerRatio(const SCG_T *base);
uint32_t SCG_HW_ReadCurrentSlowClockDividerRatio(const SCG_T *base);
void SCG_HW_ConfigRunClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider);
void SCG_HW_ConfigVlprClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider);
#if FEATURE_WITH_HIGH_SPEED_RUN_MODE
void SCG_HW_ConfigHsrunClockControl(SCG_T *base, uint32_t source, uint32_t coreDivider, uint32_t busDivider, uint32_t slowDivider);
#endif
uint32_t SCG_HW_ReadHsiFirstAsyncDivider(const SCG_T *base);
uint32_t SCG_HW_ReadHsiSecondAsyncDivider(const SCG_T *base);
void SCG_HW_ConfigHsiAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2);
uint32_t SCG_HW_ReadLsiFirstAsyncDivider(const SCG_T *base);
uint32_t SCG_HW_ReadLsiSecondAsyncDivider(const SCG_T *base);
void SCG_HW_ConfigLsiAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2);
uint32_t SCG_HW_ReadSoscFirstAsyncDivider(const SCG_T *base);
uint32_t SCG_HW_ReadSoscSecondAsyncDivider(const SCG_T *base);
void SCG_HW_ConfigSoscAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2);
#if FEATURE_WITH_SYSPLL_CLK
uint32_t SCG_HW_ReadSysPllFirstAsyncDivider(const SCG_T *base);
uint32_t SCG_HW_ReadSysPllSecondAsyncDivider(const SCG_T *base);
void SCG_HW_ConfigSysPllAsyncConfig(SCG_T *base, uint32_t div1, uint32_t div2);
#endif
void SCG_HW_ConfigClockoutSourceSel(SCG_T *base, uint32_t source);
bool SCG_HW_ReadLsiAsSystemClock(const SCG_T *const base);
bool SCG_HW_ReadLsiEnableStatus(const SCG_T *const base);
uint32_t SCG_HW_ReadLsiRange(const SCG_T *const base);
void SCG_HW_ClearLsiLockStatus(SCG_T *base);
void SCG_HW_ClearLsiControlStatus(SCG_T *base);
void SCG_HW_ConfigLsiRang(SCG_T *base, uint8_t range);
void SCG_HW_ConfigLsiControl(SCG_T *base, bool enableInStop, bool enableInLowPower, bool lockMode);
bool SCG_HW_ReadHsiAsSystemClock(const SCG_T *const base);
bool SCG_HW_ReadHsiEnableStatus(const SCG_T *const base);
uint32_t SCG_HW_ReadHsiRange(const SCG_T *const base);
void SCG_HW_ClearHsiLockStatus(SCG_T *base);
void SCG_HW_ClearHsiControlStatus(SCG_T *base);
void SCG_HW_ConfigHsiRang(SCG_T *base, uint8_t range);
void SCG_HW_ConfigHsiControl(SCG_T *base, bool regulator, bool lockMode);
bool SCG_HW_ReadSoscAsSystemClock(const SCG_T *const base);
bool SCG_HW_ReadSoscEnableStatus(const SCG_T *const base);
void SCG_HW_ClearSoscLockStatus(SCG_T *base);
void SCG_HW_ClearSoscControlStatus(SCG_T *base);
void SCG_HW_ConfigSoscCfg(SCG_T *base, uint32_t range, uint32_t gain, uint32_t extRef);
void SCG_HW_ConfigSoscControl(SCG_T *base, bool monitorMode, bool resetMode, bool lockMode);
#if FEATURE_WITH_SYSPLL_CLK
bool SCG_HW_ReadSysPllAsSystemClock(const SCG_T *const base);
bool SCG_HW_ReadSysPllEnableStatus(const SCG_T *const base);
uint32_t SCG_HW_ReadSysPllPredivider(const SCG_T *const base);
uint32_t SCG_HW_ReadSysPllMultiplier(const SCG_T *const base);
void SCG_HW_ClearSysPllLockStatus(SCG_T *base);
void SCG_HW_ClearSysPllControlStatus(SCG_T *base);
void SCG_HW_ConfigSysPllCfg(SCG_T *base, uint8_t divider, uint8_t multiplier);
void SCG_HW_ConfigSysPllControl(SCG_T *base, bool monitorMode, bool resetMode, bool lockMode);
#endif


/**@} end of group CLOCK_Functions*/
/**@} end of group CLOCK_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/


#endif /* APM32F445_446_CLOCK_H */

