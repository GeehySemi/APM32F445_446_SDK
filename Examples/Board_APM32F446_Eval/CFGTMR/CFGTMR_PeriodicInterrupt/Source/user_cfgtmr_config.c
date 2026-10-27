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

/** @addtogroup CFGTMR_PeriodicInterrupt
  @{
  */

/** @defgroup CFGTMR_PeriodicInterrupt_Variables Variables
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
        .inverterSync     = CFGTMR_SYSTEM_CLOCK, /* Update mode for ICTRL register */
        .outRegSync       = CFGTMR_SYSTEM_CLOCK, /* Update mode for SWOCTRL register */
        .maskRegSync      = CFGTMR_SYSTEM_CLOCK, /* Update mode for OMASK register */
        .initCounterSync  = CFGTMR_SYSTEM_CLOCK, /* Update mode for INITV register */
        .autoClrTrigger   = false,             /* Automatic clear of the trigger*/
        .syncPoint        = CFGTMR_UPDATE_NOW,   /* Synchronization point */
    },
    .cfgtmrMode        = CFGTMR_MODE_UP_TIMER,       /* Mode of operation for CFGTMR */
    .BDMMode         = CFGTMR_BDM_MODE_11,         /* CFGTMR debug mode */
    .cfgtmrPsc         = CFGTMR_CLOCK_DIV_1,         /* CFGTMR clock prescaler */
    .cfgtmrClkSrc      = CFGTMR_CLOCK_SRC_SYSTEMCLK, /* CFGTMR clock source */
    .isEnableInitTrg = false,                    /* Initialization trigger */
    .isEnableInterrupt = true,                   /* Interrupt state */
};

/* CFGTMR timer parameter config */
CFGTMR_TIMER_PARAM_T g_cfgtmr0TimerParamConfig =
{
    .initValue    = 0,                  /* Initial counter value */
    .finalValue   = 48000,              /* Final counter value */
    .mode         = CFGTMR_MODE_UP_TIMER  /* Counter mode */
};

/**@} end of group CFGTMR_PeriodicInterrupt_Variables */
/**@} end of group CFGTMR_PeriodicInterrupt */
/**@} end of group Examples */
