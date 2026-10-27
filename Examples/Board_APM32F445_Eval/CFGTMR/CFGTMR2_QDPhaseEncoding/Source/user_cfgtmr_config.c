/*!
 * @file        user_peripheral_config.c
 *
 * @brief       Peripheral configurations
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
#include "user_cfgtmr_config.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CFGTMR2_QDPhaseEncoding
  @{
  */

/** @defgroup CFGTMR2_QDPhaseEncoding_Variables Variables
  @{
  */

/*******************************************************************************
 *                              CFGTMR Combined PWM output config
 ******************************************************************************/

/* CFGTMR user initialize config */
CFGTMR_USER_CONFIG_T g_cfgtmr0InitConfig =
{
    {
        .softSync     = true,              /* Software trigger state */
        .hwSync0      = false,             /* Hardware trigger 1 state */
        .hwSync1      = false,             /* Hardware trigger 2 state */
        .hwSync2      = false,             /* Hardware trigger 3 state */
        .maxLoadingPoint  = false,             /* Max loading point state */
        .minLoadingPoint  = false,             /* Min loading point state */
        .inverterSync     = CFGTMR_PWM_SYNC,     /* Update mode for ICTRL register */
        .outRegSync       = CFGTMR_PWM_SYNC,     /* Update mode for SWOCTRL register */
        .maskRegSync      = CFGTMR_PWM_SYNC,     /* Update mode for OMASK register */
        .initCounterSync  = CFGTMR_PWM_SYNC,     /* Update mode for INITV register */
        .autoClrTrigger   = true,              /* Automatic clear of the trigger */
        .syncPoint        = CFGTMR_UPDATE_NOW,   /* Synchronization point */
    },
    .cfgtmrMode        = CFGTMR_MODE_EDGE_ALIGNED_PWM,  /* Mode of operation for CFGTMR */
    .BDMMode         = CFGTMR_BDM_MODE_11,            /* CFGTMR debug mode */
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_2,            /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK,    /* CFGTMR clock source */
    .isEnableInitTrg = false,                       /* Initialization trigger */
    .isEnableInterrupt = false,                     /* Interrupt state */
};

/* CFGTMR PWM fault parameter config */
CFGTMR_PWM_FAULT_PARAM_T g_cfgtmr0PWMFaultConfig =
{
    .faultMode             = CFGTMR_FAULT_CONTROL_DISABLED, /* Fault mode */
    .faultFilterVal        = 0U,    /* Fault filter value */
    .outputPINStateOnFault = false, /* Output pin state on fault (safe state or tri-state) */
    .isPWMFaultInterrupt   = false, /* PWM fault interrupt state */
    .faultChnParam  = {
        {
            false,             /* Fault channel state */
            false,             /* Fault channel filter state */
            CFGTMR_POLARITY_LOW, /* Channel output state on fault */
        },
        {
            false,             /* Fault channel state */
            false,             /* Fault channel filter state */
            CFGTMR_POLARITY_LOW, /* Channel output state on fault */
        },
        {
            false,             /* Fault channel state */
            false,             /* Fault channel filter state */
            CFGTMR_POLARITY_LOW, /* Channel output state on fault */
        },
        {
            false,             /* Fault channel state */
            false,             /* Fault channel filter state */
            CFGTMR_POLARITY_LOW, /* Channel output state on fault */
        },
    }
};

/* CFGTMR the PWM of the combined channels config */
CFGTMR_COMBINED_CHAN_PARAM_T g_cfgtmr0PWMCombinedChannelsConfig[CFGTMR_PWM_COMBINED_CHANNEL_NUMBER] =
{
    {
        .hwChannelId = 0,               /* Physical hardware channel ID for channel (n) */
        .enableModifiedCombine = true,  /* Enable/disable the modified combine mode for channels (n) and (n+1) */
        .firstEdge   = 0U,              /* First edge time, range: 0 - 0xFFFF */
        .secondEdge  = 0x4000U,         /* Second edge time, range: 0 - 0xFFFF */
        .mainChnPolarity   = CFGTMR_POLARITY_HIGH, /* Polarity for channel (n) */
        .enableSecondChnOutput = true,           /* Enabled/disabled the channel (n+1) output */
        .secondChnPolarity = CFGTMR_MAIN_INVERTED, /* Select channel (n+1) output polarity relative to channel (n) */
        .isExternalTrigger = false,              /* Enable/disable external Trigger for channel (n) */
        .isExternalTriggerOnNextChn = false,     /* Enable/disable external Trigger for channel (n+1) */
        .mainChnSafeState   = CFGTMR_LOW_STATE,    /* Safe state of the PWM channel n when faults are detected */
        .secondChnSafeState = CFGTMR_LOW_STATE,    /* Safe state of the PWM channel n+1 when faults are detected */
        .enableDeadTime     = false,             /* Dead time enabled/disabled */
    },
    {
        .hwChannelId = 2,               /* Physical hardware channel ID for channel (n) */
        .enableModifiedCombine = true,  /* Enable/disable the modified combine mode for channels (n) and (n+1) */
        .firstEdge   = 0x2000U,         /* First edge time, range: 0 - 0xFFFF */
        .secondEdge  = 0x6000U,         /* Second edge time, range: 0 - 0xFFFF */
        .mainChnPolarity       = CFGTMR_POLARITY_HIGH, /* Polarity for channel (n) */
        .enableSecondChnOutput = true,           /* Enabled/disabled the channel (n+1) output */
        .secondChnPolarity = CFGTMR_MAIN_INVERTED, /* Select channel (n+1) output polarity relative to channel (n) */
        .isExternalTrigger = false,              /* Enable/disable external Trigger for channel (n) */
        .isExternalTriggerOnNextChn = false,     /* Enable/disable external Trigger for channel (n+1) */
        .mainChnSafeState   = CFGTMR_LOW_STATE,    /* Safe state of the PWM channel n when faults are detected */
        .secondChnSafeState = CFGTMR_LOW_STATE,    /* Safe state of the PWM channel n+1 when faults are detected */
        .enableDeadTime     = false,             /* Dead time enabled/disabled */
    },
};

/* CFGTMR PWM initialize config */
CFGTMR_PWM_PARAM_T g_cfgtmr0PWMInitConfig =
{
    .numOfIndependentPwmChns  = 0U,                               /* Independent PWM channels number */
    .numOfCombinedPwmChns     = CFGTMR_PWM_COMBINED_CHANNEL_NUMBER, /* cCombined PWM channels number */
    .mode                     = CFGTMR_MODE_EDGE_ALIGNED_PWM,       /* CFGTMR operation mode, Configured as PWM mode */
    .uFreqHZ                  = 1000U,                            /* PWM frequency in HZ */
    .deadTimeVal              = 0U,                               /* Dead time value */
    .deadTimePsc              = CFGTMR_DEADTIME_DIV_1,              /* Dead time prescale */
    .pwmIndependentChnCfg     = NULL,                             /* Configuration for independent PWM channels */
    .pwmCombinedChnCfg        = g_cfgtmr0PWMCombinedChannelsConfig,  /* Configuration for combined PWM channels */
    .pwmFaultCfg              = &g_cfgtmr0PWMFaultConfig             /* Configuration for PWM fault */
};

/*******************************************************************************
 *                              CFGTMR Quadrature decoder mode config
 ******************************************************************************/

/* CFGTMR user initialize config for input capture */
CFGTMR_USER_CONFIG_T g_cfgtmr2InitConfig =
{
    {
        .softSync     = true,              /* Software trigger state */
        .hwSync0      = false,             /* Hardware trigger 1 state */
        .hwSync1      = false,             /* Hardware trigger 2 state */
        .hwSync2      = false,             /* Hardware trigger 3 state */
        .maxLoadingPoint  = false,             /* Max loading point state */
        .minLoadingPoint  = false,             /* Min loading point state */
        .inverterSync     = CFGTMR_SYSTEM_CLOCK, /* Update mode for ICTRL register */
        .outRegSync       = CFGTMR_SYSTEM_CLOCK, /* Update mode for SWOCTRL register */
        .maskRegSync      = CFGTMR_SYSTEM_CLOCK, /* Update mode for OMASK register */
        .initCounterSync  = CFGTMR_SYSTEM_CLOCK, /* Update mode for INITV register */
        .autoClrTrigger   = false,             /* Automatic clear of the trigger */
        .syncPoint        = CFGTMR_UPDATE_NOW,   /* Synchronization point */
    },
    .cfgtmrMode        = CFGTMR_MODE_QUADRATURE_DECODER,/* Mode of operation for CFGTMR */
    .BDMMode         = CFGTMR_BDM_MODE_11,            /* CFGTMR debug mode */
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_1,            /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK,    /* CFGTMR clock source */
    .isEnableInitTrg = false,                       /* Initialization trigger */
    .isEnableInterrupt = true,                      /* Interrupt state */
};

/* CFGTMR Quadrature decoder initialize config */
CFGTMR_QUAD_DECODE_CONFIG_T g_cfgtmr2QuadDecodeInitConfig =
{
    .initVal      = 0U,                     /* Counter initial value */
    .maxVal       = 10U,                    /* Counter maximum value */
    .mode         = CFGTMR_QUAD_PHASE_ENCODE, /* Quadrature decode modes */
    .phaseACfg = {
        .isPhaseInputFilter = false,            /* Enable/Disable phase filter */
        .phaseFilterVal     = 0,                /* Filter value */
        .phasePolarity      = CFGTMR_QUAD_PHASE_NORMAL, /* Phase polarity */
    },
    .phaseBCfg = {
        .isPhaseInputFilter = false,
        .phaseFilterVal     = 0,
        .phasePolarity      = CFGTMR_QUAD_PHASE_NORMAL,
    },
};

/**@} end of group CFGTMR2_QDPhaseEncoding_Variables */
/**@} end of group CFGTMR2_QDPhaseEncoding */
/**@} end of group Examples */
