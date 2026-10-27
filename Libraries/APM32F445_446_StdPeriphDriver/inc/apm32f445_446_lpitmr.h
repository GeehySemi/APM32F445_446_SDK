/*!
 * @file        apm32f445_446_lpitmr.h
 *
 * @brief       This file contains all the functions prototypes for the LPITMR firmware library.
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

#ifndef APM32F445_446_LPITMR_H
#define APM32F445_446_LPITMR_H

/* Includes */
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPITMR_Driver LPITMR Driver
  @{
*/

/** @defgroup LPITMR_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
/*! @brief Max period in count of all operation mode except for dual 16 bit periodic counter mode */
#define MAX_PERIOD_COUNT (0xFFFFFFFFU)
/*! @brief Max period in count of dual 16 bit periodic counter mode */
#define MAX_PERIOD_COUNT_IN_DUAL_16BIT_MODE (0x1FFFEU)
/*! @brief Max count of 16 bit */
#define MAX_PERIOD_COUNT_16_BIT (0xFFFFU)

/**@} end of group LPITMR_Macros*/

/** @defgroup LPITMR_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief Mode options available for the LPITMR timer
 */
typedef enum
{
    LPITMR_PERIODIC_COUNTER      = 0x00U,  /* 32-bit Periodic Counter      */
    LPITMR_DUAL_PERIODIC_COUNTER = 0x01U,  /* Dual 16-bit Periodic Counter */
    LPITMR_TRIGGER_ACCUMULATOR   = 0x02U,  /* 32-bit Trigger Accumulator   */
    LPITMR_INPUT_CAPTURE         = 0x03U   /* 32-bit Trigger Input Capture */
} LPITMR_TIMER_MODES_T;

/*!
 * @brief Trigger source options.
 *
 * This is used for both internal and external trigger sources. The actual trigger
 * options available is SoC specific, user should refer to the reference manual.
 */
typedef enum
{
    LPITMR_TRIG_SRC_EXTERNAL = 0x00U, /* Use external trigger */
    LPITMR_TRIG_SRC_INTERNAL = 0x01U  /* Use internal trigger */
}  LPITMR_TRIGGER_SOURCE_T;

/*!
 * @brief Unit options for LPITMR period
 */
typedef enum
{
    LPIT_PERIOD_UNITS_COUNTS        = 0x00U, /* Period value unit is count */
    LPIT_PERIOD_UNITS_MICROSECONDS  = 0x01U  /* Period value unit is microsecond */
} LPIT_PERIOD_UNITS_T;

/**@} end of group LPITMR_Enumerations*/

/** @defgroup LPITMR_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief LPITMR configuration structure
 */
typedef struct
{
    bool runInDozeEn;         /* True:  Timer channels continue to run in doze mode
                              False: Timer channels stop in doze mode             */
    bool runInDebugEn;        /* True:  Timer channels continue to run in debug mode
                              False: Timer channels stop in debug mode            */
} LPITMR_USER_CONFIG_T;

/*!
 * @brief Structure to configure the channel timer
 */
typedef struct
{
    LPITMR_TIMER_MODES_T timerMode;        /* Operation mode of timer channel                          */
    LPIT_PERIOD_UNITS_T periodUnits;       /* Timer period value units                                 */
    uint32_t period;                       /* Period of timer channel                                  */
    LPITMR_TRIGGER_SOURCE_T triggerSrc;    /* Selects between internal and external trigger sources    */
    uint32_t triggerSelect;                /* Selects one trigger from the internal trigger sources
                                              this field makes sense if trigger source is internal     */
    bool channelChainEn;                   /* Channel chaining enable                                  */
    bool interruptEn;                      /* Timer channel interrupt generation enable                */
    bool reloadOnTriggerEn;                /* True: Timer channel will reload on selected trigger
                                              False: Timer channel will not reload on selected trigger */
    bool startOnTriggerEn;                 /* True: Timer channel starts to decrement when rising edge
                                              on selected trigger is detected.
                                              False: Timer starts to decrement immediately based on
                                              restart condition                                        */
    bool stopOnInterruptEn;                /* True: Timer will stop after timeout
                                              False: Timer channel does not stop after timeout         */
} LPITMR_USER_CHANNEL_CONFIG_T;

/**@} end of group LPITMR_Structures*/

/** @defgroup LPITMR_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/* Table of base addresses for LPITMR instances */
extern LPITMR_T * const g_lpitmrBase[];
/* Table to save LPITMR indexes in PCC register map for clock configuration */
extern const CLOCK_NAMES_T g_lpitmrClkNames[LPITMR_INSTANCE_COUNT];
/* LPITMR functional clock variable which will be updated in some driver functions */
extern uint32_t g_lpitmrSourceClockFrequency[LPITMR_INSTANCE_COUNT];

/**@} end of group LPITMR_Variables*/

/** @defgroup LPITMR_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void LPITMR_DefaultConfig(LPITMR_USER_CONFIG_T * const userConfig);
void LPITMR_DeInit(uint32_t ins);
void LPITMR_Init(uint32_t ins, const LPITMR_USER_CONFIG_T *userConfig);
void LPITMR_DefaultChannelConfig(LPITMR_USER_CHANNEL_CONFIG_T * const userConfig);
STATUS_T LPITMR_ChannelInit(uint32_t ins, uint32_t channelNumber, const LPITMR_USER_CHANNEL_CONFIG_T * userChannelConfig);
void LPITMR_StartTimerChannels(uint32_t ins, uint32_t mask);
void LPITMR_StopTimerChannels(uint32_t ins, uint32_t mask);
STATUS_T LPITMR_ConfigTimerPeriodByUs(uint32_t ins, uint32_t channel, uint32_t periodUs);
STATUS_T LPITMR_ConfigTimerPeriodInDual16ModeByUs(uint32_t ins, uint32_t channel, uint16_t periodHigh, uint16_t periodLow);
uint64_t LPITMR_ReadTimerPeriodByUs(uint32_t ins, uint32_t channel);
uint64_t LPITMR_ReadCurrentTimerUs(uint32_t ins, uint32_t channel);
void LPITMR_ConfigTimerPeriodByCount(uint32_t ins, uint32_t channel, uint32_t count);
void LPITMR_ConfigTimerPeriodInDual16ModeByCount(uint32_t ins, uint32_t channel, uint16_t periodHigh, uint16_t periodLow);
uint32_t LPITMR_ReadTimerPeriodByCount(uint32_t ins, uint32_t channel);
uint32_t LPITMR_ReadCurrentTimerCount(uint32_t ins, uint32_t channel);
void LPITMR_EnableTimerChannelInterrupt(uint32_t ins, uint32_t mask);
void LPITMR_DisableTimerChannelInterrupt(uint32_t ins, uint32_t mask);
uint32_t LPITMR_ReadInterruptFlagTimerChannels(uint32_t ins, uint32_t mask);
void LPITMR_ClearInterruptFlagTimerChannels(uint32_t ins, uint32_t mask);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void LPITMR_HW_Enable(LPITMR_T * const base, volatile uint32_t delay);
void LPITMR_HW_Disable(LPITMR_T * const base);
void LPITMR_HW_Reset(LPITMR_T * const base, volatile uint32_t delay);
void LPITMR_HW_StartTimerChannels(LPITMR_T * const base, uint32_t mask);
void LPITMR_HW_StopTimerChannels(LPITMR_T * const base, uint32_t mask);
void LPITMR_HW_ConfigTimerPeriodByCount(LPITMR_T * const base, uint32_t channel, uint32_t count);
uint32_t LPITMR_HW_ReadTimerPeriodByCount(const LPITMR_T * base, uint32_t channel);
uint32_t LPITMR_HW_ReadCurrentTimerCount(const LPITMR_T * base, uint32_t channel);
void LPITMR_HW_EnableInterruptTimerChannels(LPITMR_T * const base, uint32_t mask);
void LPITMR_HW_DisableInterruptTimerChannels(LPITMR_T * const base, uint32_t mask);
uint32_t LPITMR_HW_ReadInterruptFlagTimerChannels(const LPITMR_T * base, uint32_t mask);
void LPITMR_HW_ClearInterruptFlagTimerChannels(LPITMR_T * const base, uint32_t mask);
void LPITMR_HW_ConfigTimerChannelModeCmd(LPITMR_T * const base, uint32_t channel, LPITMR_TIMER_MODES_T mode);
LPITMR_TIMER_MODES_T LPITMR_HW_ReadTimerChannelModeCmd(const LPITMR_T * base, uint32_t channel);
void LPITMR_HW_ConfigTriggerSelectCmd(LPITMR_T * const base, uint32_t channel, uint32_t triggerChannelSelect);
void LPITMR_HW_ConfigTriggerSourceCmd(LPITMR_T * const base, uint32_t channel, LPITMR_TRIGGER_SOURCE_T triggerSrc);
void LPITMR_HW_ConfigReloadOnTriggerCmd(LPITMR_T * const base, uint32_t channel, bool isReloadOnTrigger);
void LPITMR_HW_ConfigStopOnInterruptCmd(LPITMR_T * const base, uint32_t channel, bool isStopOnInterrupt);
void LPITMR_HW_ConfigStartOnTriggerCmd(LPITMR_T * const base, uint32_t channel, bool isStartOnTrigger);
void LPITMR_HW_ConfigTimerChannelChainCmd(LPITMR_T * const base, uint32_t channel, bool isChannelChained);
void LPITMR_HW_ConfigTimerRunInDebugCmd(LPITMR_T * const base, bool isRunInDebug);
void LPITMR_HW_ConfigTimerRunInDozeCmd(LPITMR_T * const base, bool isRunInDoze);

/**@} end of group LPITMR_Functions*/
/**@} end of group LPITMR_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_LPITMR_H */
