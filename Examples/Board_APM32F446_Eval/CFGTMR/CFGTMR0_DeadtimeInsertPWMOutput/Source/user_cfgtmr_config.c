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

/** @addtogroup CFGTMR0_DeadtimeInsertPWMOutput
  @{
  */

/** @defgroup CFGTMR0_DeadtimeInsertPWMOutput_Variables Variables
  @{
  */

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
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_1,            /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK,    /* CFGTMR clock source */
    .isEnableInitTrg = false,                     /* Initialization trigger */
    .isEnableInterrupt = false,                       /* Interrupt state */
};

/* CFGTMR PWM fault parameter config */
CFGTMR_PWM_FAULT_PARAM_T g_cfgtmr0PWMFaultConfig =
{
    .faultMode             = CFGTMR_FAULT_CONTROL_DISABLED, /* Fault mode */
    .faultFilterVal      = 0U,    /* Fault filter value */
    .outputPINStateOnFault = false, /* Output pin state on fault (safe state or tri-state) */
    .isPWMFaultInterrupt     = false, /* PWM fault interrupt state */
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
        .hwChannelId = 0,                        /* Hardware channel ID */
        .dutyCycle = 0x4000U,                    /* Duty cycle percent 0-0x8000 */
        .polarity = CFGTMR_POLARITY_HIGH,          /* Polarity of the PWM signal generated on MCU pin. */
        .safeState = CFGTMR_LOW_STATE,             /* Safe state of the PWM channel when faults are detected */
        .enableSecondChnOutput = true,           /* Enabled/disabled the channel (n+1) output */
        .secondChnPolarity = CFGTMR_MAIN_INVERTED, /* Select channel (n+1) output relative to channel (n) */
        .enableDeadTime = true,                  /* Enable/disable dead time */
        .isExternalTrigger = false,              /* Enable/disable external Trigger */
    },
};

/* CFGTMR PWM initialize config */
CFGTMR_PWM_PARAM_T g_cfgtmr0PWMInitConfig =
{
    .numOfIndependentPwmChns  = 1U,                         /* Independent PWM channels number */
    .numOfCombinedPwmChns     = 0U,                         /* cCombined PWM channels number */
    .mode                     = CFGTMR_MODE_EDGE_ALIGNED_PWM, /* CFGTMR operation mode, Configured as PWM mode */
    .uFreqHZ                  = 10000U,                     /* PWM frequency in HZ */
    .deadTimeVal              = 12U,                        /* Dead time value */
    .deadTimePsc              = CFGTMR_DEADTIME_DIV_16,       /* Dead time prescale */
    .pwmIndependentChnCfg     = g_cfgtmr0PWMIndependentChannelsConfig, /* Configuration for independent PWM channels */
    .pwmCombinedChnCfg        = NULL,                                /* Configuration for combined PWM channels */
    .pwmFaultCfg              = &g_cfgtmr0PWMFaultConfig               /* Configuration for PWM fault */
};

/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput_Variables */
/**@} end of group CFGTMR0_DeadtimeInsertPWMOutput */
/**@} end of group Examples */
