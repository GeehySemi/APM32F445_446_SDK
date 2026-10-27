/*!
 * @file        apm32f445_446_lptmr.h
 *
 * @brief       This file contains all the functions prototypes for the LPTMR firmware library.
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

#ifndef APM32F445_446_LPTMR_H
#define APM32F445_446_LPTMR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

/** @addtogroup  APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPTMR_Driver LPTMR Driver
  @{
*/

/** @defgroup LPTMR_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/* Takes into consideration that LPTMR compare events take place
 * when "the CNT equals the value of the COMP and increments" */
#define LPTMR_MAX_COMP_NTICKS                (0x10000U)
#define LPTMR_MAX_PRESCALER                  (1U << 4U)

/**@} end of group LPTMR_Macros*/

/** @defgroup LPTMR_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief Pulse Counter input selection
 */
typedef enum
{
    LPTMR_PINSELECT_TRGMUX  = 0x00U, /* Count pulses from TRGMUX trigger */
#if FEATURE_LPTMR_WITH_INPUT_ALT1_SELECTION
    LPTMR_PINSELECT_ALT1    = 0x01U, /* Count pulses from pin alternative 1 */
#endif
    LPTMR_PINSELECT_ALT2    = 0x02U, /* Count pulses from pin alternative 2 */
    LPTMR_PINSELECT_ALT3    = 0x03U  /* Count pulses from pin alternative 3 */
} LPTMR_PINSELECT_T;

/*!
 * @brief Pulse counter input polarity
 */
typedef enum
{
    LPTMR_PINPOLARITY_RISING    = 0U, /* Count pulse on rising edge */
    LPTMR_PINPOLARITY_FALLING   = 1U  /* Count pulse on falling edge */
} LPTMR_PINPOLARITY_T;

/*!
 * @brief Work mode
 */
typedef enum
{
    LPTMR_WORKMODE_TIMER        = 0U, /* Work as timer */
    LPTMR_WORKMODE_PULSECOUNTER = 1U  /* Work as Pulse counter */
} LPTMR_WORKMODE_T;

/*!
 * @brief Prescaler selection
 */
typedef enum
{
    LPTMR_PRESCALE_2                        = 0x00U, /* Timer mode: prescaler 2, Glitch filter mode: invalid */
    LPTMR_PRESCALE_4_GLITCHFILTER_2         = 0x01U, /* Timer mode: prescaler 4, Glitch filter mode: 2 clocks */
    LPTMR_PRESCALE_8_GLITCHFILTER_4         = 0x02U, /* Timer mode: prescaler 8, Glitch filter mode: 4 clocks */
    LPTMR_PRESCALE_16_GLITCHFILTER_8        = 0x03U, /* Timer mode: prescaler 16, Glitch filter mode: 8 clocks */
    LPTMR_PRESCALE_32_GLITCHFILTER_16       = 0x04U, /* Timer mode: prescaler 32, Glitch filter mode: 16 clocks */
    LPTMR_PRESCALE_64_GLITCHFILTER_32       = 0x05U, /* Timer mode: prescaler 64, Glitch filter mode: 32 clocks */
    LPTMR_PRESCALE_128_GLITCHFILTER_64      = 0x06U, /* Timer mode: prescaler 128, Glitch filter mode: 64 clocks */
    LPTMR_PRESCALE_256_GLITCHFILTER_128     = 0x07U, /* Timer mode: prescaler 256, Glitch filter mode: 128 clocks */
    LPTMR_PRESCALE_512_GLITCHFILTER_256     = 0x08U, /* Timer mode: prescaler 512, Glitch filter mode: 256 clocks */
    LPTMR_PRESCALE_1024_GLITCHFILTER_512    = 0x09U, /* Timer mode: prescaler 1024, Glitch filter mode: 512 clocks */
    LPTMR_PRESCALE_2048_GLITCHFILTER_1024   = 0x0AU, /* Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks */
    LPTMR_PRESCALE_4096_GLITCHFILTER_2048   = 0x0BU, /* Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks */
    LPTMR_PRESCALE_8192_GLITCHFILTER_4096   = 0x0CU, /* Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks */
    LPTMR_PRESCALE_16384_GLITCHFILTER_8192  = 0x0DU, /* Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks */
    LPTMR_PRESCALE_32768_GLITCHFILTER_16384 = 0x0EU, /* Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks */
    LPTMR_PRESCALE_65536_GLITCHFILTER_32768 = 0x0Fu  /* Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks */
} LPTMR_PRESCALER_T;

/*!
 * @brief Clock source selection
 */
typedef enum
{
    LPTMR_CLOCKSOURCE_LSICLKDIV2 = 0x00U, /* LSI clock */
    LPTMR_CLOCKSOURCE_1KHZ_LPO   = 0x01U, /* 1kHz LPO clock */
    LPTMR_CLOCKSOURCE_RTC        = 0x02U, /* RTC clock */
    LPTMR_CLOCKSOURCE_PCM        = 0x03u  /* PCC configured clock */
} LPTMR_CLOCKSOURCE_T;

/*!
 * @brief Defines the LPTMR counter units available for configuring or reading the timer compare value.
 */
typedef enum
{
    LPTMR_COUNTER_UNITS_TICKS           = 0x00U, /* counter units as ticks */
    LPTMR_COUNTER_UNITS_MICROSECONDS    = 0x01U  /* counter units as microseconds */
} LPTMR_COUNTER_UNITS_T;

/**@} end of group LPTMR_Enumerations*/

/** @defgroup LPTMR_Structures Structures
  @{
*/

/*******************************************************************************
 *                              TYPE DEFINES
 ******************************************************************************/

/*!
 * @brief Defines the configuration structure for LPTMR.
 */
typedef struct
{
    /* General parameters */
    LPTMR_WORKMODE_T workMode;                /* Time/Pulse Counter Mode */
    bool dmaReq;                              /* Enable/Disable DMA requests */
    bool intEnable;                           /* Enable/Disable Interrupt */
    bool freeRun;                             /* Enable/Disable Free Running Mode */
    /* Counter parameters */
    LPTMR_CLOCKSOURCE_T clkSelect;            /* Clock selection for Timer/Glitch filter */
    LPTMR_PRESCALER_T psc;                    /* Prescaler Selection */
    bool bypassPsc;                           /* Enable/Disable prescaler bypass */
    uint32_t compareValue;                    /* Compare value */
    LPTMR_COUNTER_UNITS_T cntUnits;           /* Compare value units */
    /* Pulse Counter specific parameters */
    LPTMR_PINSELECT_T pinSelect;              /* Pin selection for Pulse-Counter */
    LPTMR_PINPOLARITY_T pinPolarity;          /* Pin Polarity for Pulse-Counter */
} LPTMR_CONFIG_T;

/**@} end of group LPTMR_Structures*/

/** @defgroup LPTMR_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
void LPTMR_Init(const uint32_t instance,
    const LPTMR_CONFIG_T *const config,
    const bool startCounter);

void LPTMR_DeInit(const uint32_t instance);
void LPTMR_ConfigSetting(const uint32_t instance, const LPTMR_CONFIG_T *const config);
void LPTMR_DefaultConfig(LPTMR_CONFIG_T *const config);
void LPTMR_ReadConfig(const uint32_t instance, LPTMR_CONFIG_T *const config);

STATUS_T LPTMR_ConfigCompareValueByCount(
    const uint32_t instance,
    const uint16_t compareValueByCount);

void LPTMR_ReadCompareValueByCount(const uint32_t instance, uint16_t *const compareValueByCount);
STATUS_T LPTMR_ConfigCompareValueByUs(const uint32_t instance, const uint32_t compareValueUs);
void LPTMR_ReadCompareValueByUs(const uint32_t instance, uint32_t *const compareValueUs);

bool LPTMR_ReadCompareFlag(const uint32_t instance);
void LPTMR_ClearCompareFlag(const uint32_t instance);

bool LPTMR_ReadRunningState(const uint32_t instance);

void LPTMR_EnableInterrupt(const uint32_t instance);
void LPTMR_DisableInterrupt(const uint32_t instance);

uint16_t LPTMR_ReadCounterValueByCount(const uint32_t instance);
void LPTMR_StartCounter(const uint32_t instance);
void LPTMR_StopCounter(const uint32_t instance);

void LPTMR_ConfigPinSetting(
    const uint32_t instance,
    const LPTMR_PINSELECT_T pinSelect,
    const LPTMR_PINPOLARITY_T pinPolarity);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
void LPTMR_HW_ConfigForReset(LPTMR_T *const lptmrBase);
bool LPTMR_HW_ReadDmaRequest(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigDmaRequest(LPTMR_T *const lptmrBase, bool enable);
bool LPTMR_HW_ReadCompareFlagState(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ClearCompareFlag(LPTMR_T *const lptmrBase);
bool LPTMR_HW_ReadInterruptEnable(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigInterrupt(LPTMR_T *const lptmrBase, bool enable);
LPTMR_PINSELECT_T LPTMR_HW_ReadPinSelect(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigPinSelect(LPTMR_T *const lptmrBase, const LPTMR_PINSELECT_T pinsel);
LPTMR_PINPOLARITY_T LPTMR_HW_ReadPinPolarity(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigPinPolarity(LPTMR_T *const lptmrBase, const LPTMR_PINPOLARITY_T pol);
bool LPTMR_HW_ReadFreeRunning(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigFreeRunning(LPTMR_T *const lptmrBase, const bool enable);
LPTMR_WORKMODE_T LPTMR_HW_ReadWorkMode(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigWorkMode(LPTMR_T *const lptmrBase, const LPTMR_WORKMODE_T mode);
bool LPTMR_HW_ReadEnable(const LPTMR_T *const lptmrBase);
void LPTMR_HW_Enable(LPTMR_T *const lptmrBase);
void LPTMR_HW_Disable(LPTMR_T *const lptmrBase);
LPTMR_PRESCALER_T LPTMR_HW_ReadPrescaler(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigPrescaler(LPTMR_T *const lptmrBase, const LPTMR_PRESCALER_T prescale);
bool LPTMR_HW_ReadBypass(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigBypass(LPTMR_T *const lptmrBase, const bool enable);
LPTMR_CLOCKSOURCE_T LPTMR_HW_ReadClockSelect(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigClockSelect(LPTMR_T *const lptmrBase, const LPTMR_CLOCKSOURCE_T clocksel);
uint16_t LPTMR_HW_ReadCompareValue(const LPTMR_T *const lptmrBase);
void LPTMR_HW_ConfigCompareValue(LPTMR_T *const lptmrBase, const uint16_t compval);
uint16_t LPTMR_HW_ReadCounterValue(LPTMR_T *const lptmrBase);

/**@} end of group LPTMR_Functions*/
/**@} end of group LPTMR_Driver*/
/**@} end of group  APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif /* APM32F445_446_LPTMR_H */
