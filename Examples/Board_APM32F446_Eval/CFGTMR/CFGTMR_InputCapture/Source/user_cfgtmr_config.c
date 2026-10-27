/*!
 * @file        user_peripheral_config.c
 *
 * @brief       Peripheral configurations
 *
 * @version     V1.0.0
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2024 Geehy Semiconductor
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

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CFGTMR_InputCapture
  @{
  */

/** @defgroup CFGTMR_InputCapture_Variables Variables
  @{
  */

/*******************************************************************************
 *                              CFGTMR PWM output config
 ******************************************************************************/

/* CFGTMR user initialize config for PWM output */
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
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_4,            /* CFGTMR clock prescaler */
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

/* CFGTMR the PWM of the independent channels config */
CFGTMR_INDEPENDENT_CHAN_PARAM_T g_cfgtmr0PWMIndependentChannelsConfig[1] =
{
    {
        .hwChannelId = 3,                        /* Hardware channel ID */
        .dutyCycle = 0x4000U,                    /* Duty cycle percent 0-0x8000 */
        .polarity = CFGTMR_POLARITY_LOW,           /* Polarity of the PWM signal generated on MCU pin. */
        .safeState = CFGTMR_LOW_STATE,             /* Safe state of the PWM channel when faults are detected */
        .enableSecondChnOutput = false,          /* Enabled/disabled the channel (n+1) output */
        .secondChnPolarity = CFGTMR_MAIN_INVERTED, /* Select channel (n+1) output relative to channel (n) */
        .enableDeadTime = false,                 /* Enable/disable dead time */
        .isExternalTrigger = false,              /* Enable/disable external Trigger */
    }
};

/* CFGTMR PWM initialize config */
CFGTMR_PWM_PARAM_T g_cfgtmr0PWMInitConfig =
{
    .numOfIndependentPwmChns  = 1U,                         /* Independent PWM channels number */
    .numOfCombinedPwmChns     = 0U,                         /* Combined PWM channels number */
    .mode                     = CFGTMR_MODE_EDGE_ALIGNED_PWM, /* CFGTMR operation mode, Configured as PWM mode */
    .uFreqHZ                  = 1000U,                      /* PWM frequency in HZ */
    .deadTimeVal              = 0U,                         /* Dead time value */
    .deadTimePsc              = CFGTMR_DEADTIME_DIV_1,        /* Dead time prescale */
    .pwmIndependentChnCfg     = g_cfgtmr0PWMIndependentChannelsConfig, /* Configuration for independent PWM channels */
    .pwmCombinedChnCfg        = NULL,                               /* Configuration for combined PWM channels */
    .pwmFaultCfg              = &g_cfgtmr0PWMFaultConfig               /* Configuration for PWM fault */
};

/*******************************************************************************
 *                              CFGTMR input capture config
 ******************************************************************************/

/* CFGTMR user initialize config for input capture */
CFGTMR_USER_CONFIG_T g_cfgtmr1InitConfig =
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
    .cfgtmrMode        = CFGTMR_MODE_INPUT_CAPTURE,  /* Mode of operation for CFGTMR */
    .BDMMode         = CFGTMR_BDM_MODE_11,         /* CFGTMR debug mode */
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_1,         /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK, /* CFGTMR clock source */
    .isEnableInitTrg = false,                    /* Initialization trigger */
    .isEnableInterrupt = false,                  /* Interrupt state */
};

/* CFGTMR input capture channels config */
CFGTMR_INPUT_CHAN_PARAM_T g_cfgtmr1InputCaptureChannelsConfig[1] =
{
    {
        .hwChannelId = 0U,                           /* Physical hardware channel ID */
        .edgeMode = CFGTMR_RISING_EDGE,                /* Edge alignment mode */
        .inputCaptureMode = CFGTMR_SIGNAL_MEASUREMENT, /* Input capture operation mode */
        .measurementMode = CFGTMR_FALLING_EDGE_PERIOD_MEASUREMENT, /* Signal measurement mode */
        .enableFilter = false,                  /* Filter state (enabled/disabled) */
        .filterVal = 0U,                        /* Filter value */
        .enableContinuousMode = true,           /* Continuous measurement state */
        .channelsCallbacksParams = NULL,        /* Vector of callbacks  parameters for channels events */
        .channelsCallbacks = NULL               /* Vector of callbacks for channels events */
    }
};

/* CFGTMR input capture initialize config */
CFGTMR_INPUT_PARAM_T g_cfgtmr1InputCaptureInitConfig =
{
    .numOfChannels  = 1U,       /* Number of input capture channel used */
    .maxCountVal    = 65535U,   /* Maximum counter value. 0x0U ~ 0xFFFFU */
    .inputChnConfig = g_cfgtmr1InputCaptureChannelsConfig /* Input channels configuration */
};

/**@} end of group CFGTMR_InputCapture_Variables */
/**@} end of group CFGTMR_InputCapture */
/**@} end of group Examples */
