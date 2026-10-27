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

/** @addtogroup CFGTMR1_OCMatchSet
  @{
  */

/** @defgroup CFGTMR1_OCMatchSet_Variables Variables
  @{
  */

/**********************************************************************************
  CFGTMR1 Configuration: Output compare active mode.
  CFGTMR1_CLOCK = System clock / 64 = 750KHz,
  Generate 3 signals with 3 different delays:
  CFGTMR1_CH0(PB2)  delay: Compare value / CFGTMR1 counter clock = 0     / 750KHz  = 0us.
  CFGTMR1_CH1(PB3)  delay: Compare value / CFGTMR1 counter clock = 7500  / 750KHz  = 10ms.
  CFGTMR1_CH2(PA15) delay: Compare value / CFGTMR1 counter clock = 15000 / 750KHz  = 20ms.
***********************************************************************************/

/* CFGTMR user initialize config */
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
        .autoClrTrigger   = true,              /* Automatic clear of the trigger */
        .syncPoint        = CFGTMR_UPDATE_NOW,   /* Synchronization point */
    },
    .cfgtmrMode        = CFGTMR_MODE_OUTPUT_COMPARE,    /* Mode of operation for CFGTMR */
    .BDMMode         = CFGTMR_BDM_MODE_11,            /* CFGTMR debug mode */
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_64,           /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK,    /* CFGTMR clock source */
    .isEnableInitTrg = false,                       /* Initialization trigger */
    .isEnableInterrupt = false,                     /* Interrupt state */
};

/* CFGTMR the output compare channels config */
CFGTMR_OUTPUT_CMP_CHAN_PARAM_T g_cfgtmr1OCChannelsConfig[CFGTMR_OC_CHANNEL_NUMBER] =
{
    {
        .hwChannelId = 0,                       /* Physical hardware channel ID */
        .cmpValue = 0U,                         /* The compared value. CFGTMR cpunter matches the compare value, channel output is changed. */
        .chnMatchMode = CFGTMR_SET_ON_MATCH,   /* Channel output mode */
        .isExternalTrigger = false
    },
   {
        .hwChannelId = 1,                       /* Physical hardware channel ID */
        .cmpValue = 7500,                       /* The compared value */
        .chnMatchMode = CFGTMR_SET_ON_MATCH,   /* Channel output mode */
        .isExternalTrigger = false
    },
    {
        .hwChannelId = 2,                       /* Physical hardware channel ID */
        .cmpValue = 15000,                      /* The compared value */
        .chnMatchMode = CFGTMR_SET_ON_MATCH,   /* Channel output mode */
        .isExternalTrigger = false
    }
};

/* CFGTMR output compare initialize config */
CFGTMR_OUTPUT_CMP_PARAM_T g_cfgtmr1OCInitConfig =
{
    .numOfOutputChannels = CFGTMR_OC_CHANNEL_NUMBER, /* Number of output compare channels */
    .maxCountVal = 24000,                          /* Maximum count value in ticks, it's will change the signal frequency */
    .mode = CFGTMR_MODE_OUTPUT_COMPARE,              /* CFGTMR operation mode, Configured as output compare mode */
    .outputChnCfg = g_cfgtmr1OCChannelsConfig        /* Configuration for output compare channels */
};

/**@} end of group CFGTMR1_OCMatchSet_Variables */
/**@} end of group CFGTMR1_OCMatchSet */
/**@} end of group Examples */
