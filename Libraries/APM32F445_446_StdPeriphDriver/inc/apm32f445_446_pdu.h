/*!
 * @file        apm32f445_446_pdu.h
 *
 * @brief       This file contains all the functions prototypes for the PDU firmware library.
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

#ifndef APM32F445_446_PDU_H
#define APM32F445_446_PDU_H

#if defined(__cplusplus)
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup PDU_Driver PDU Driver
  @{
*/

/** @defgroup PDU_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief PDU counter value load mode selection.
 *
 */
typedef enum
{
    /* Loaded immediately after load operation. */
    PDU_LOAD_VAL_IMMEDIATELY                       = 0U,

    /* Loaded when counter hits the modulo after load operation. */
    PDU_LOAD_VAL_AT_MODULO_COUNTER                 = 1U,

    /* Loaded when detecting an input trigger after load operation. */
    PDU_LOAD_VAL_AT_NEXT_TRIGGER                   = 2U,

    /* Loaded when counter hits the modulo or detecting an input trigger after load operation. */
    PDU_LOAD_VAL_AT_MODULO_COUNTER_OR_NEXT_TRIGGER = 3U
} PDU_LOAD_VAL_MODE_T;

/*!
 * @brief PDU counter clock prescaler divider selection.
 *
 */
typedef enum
{
    PDU_CLK_PSCDIV1   = 0U, /* ( Prescaler multiplication factor ) x ( Counter divided by 1 ) */
    PDU_CLK_PSCDIV2   = 1U, /* ( Prescaler multiplication factor ) x ( Counter divided by 2 ) */
    PDU_CLK_PSCDIV4   = 2U, /* ( Prescaler multiplication factor ) x ( Counter divided by 4 ) */
    PDU_CLK_PSCDIV8   = 3U, /* ( Prescaler multiplication factor ) x ( Counter divided by 8 ) */
    PDU_CLK_PSCDIV16  = 4U, /* ( Prescaler multiplication factor ) x ( Counter divided by 16 ) */
    PDU_CLK_PSCDIV32  = 5U, /* ( Prescaler multiplication factor ) x ( Counter divided by 32 ) */
    PDU_CLK_PSCDIV64  = 6U, /* ( Prescaler multiplication factor ) x ( Counter divided by 64 ) */
    PDU_CLK_PSCDIV128 = 7U  /* ( Prescaler multiplication factor ) x ( Counter divided by 128 ) */
} PDU_CLK_PSCDIV_T;

/*!
 * @brief PDU trigger source.
 *
 * PDU module trigger input source selection. The trigger input source can
 * selected internal or the software trigger.
 */
typedef enum
{
    PDU_TRIGGER_IN0         = 0U,    /* Source trigger comes from TRGMUX. */
    PDU_SOFTWARE_TRIGGER    = 15U    /* Select software trigger. */
} PDU_TRIGGER_SRC_T;

/*!
 * @brief PDU multiplication source mode.
 *
 * The PDU counter clock multiplication factor selection.
 */
typedef enum
{
    PDU_CLK_PSCMULT_FACTOR_AS_1  = 0U, /* Multiplication factor is 1. */
    PDU_CLK_PSCMULT_FACTOR_AS_10 = 1U, /* Multiplication factor is 10. */
    PDU_CLK_PSCMULT_FACTOR_AS_20 = 2U, /* Multiplication factor is 20. */
    PDU_CLK_PSCMULT_FACTOR_AS_40 = 3U  /* Multiplication factor is 40. */
} PDU_CLK_PSCMULT_FACTOR_T;

/**@} end of group PDU_Enumerations*/

/** @defgroup PDU_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief PDU user configuration structure.
 *
 */
typedef struct
{
    PDU_LOAD_VAL_MODE_T    loadValMode;           /* Load mode selection */
    PDU_CLK_PSCDIV_T         clkPscDiv;             /* Prescaler divider selection */
    PDU_CLK_PSCMULT_FACTOR_T clkPscMultFactor;      /* Multiplication factor of the prescaler selection */
    PDU_TRIGGER_SRC_T        triggerInput;          /* Trigger input source selection */
    bool                     seqErrIntEn;           /* Enable/disable PDU Sequence Error Interrupt */

    /* Enable/disable the interrupt for timer.
     * Note: interrupt is generated only if DMA is disabled. */
    bool                     intEn;

    bool                     continuousModeEn;      /* Enable/disable the continuous mode */
    bool                     dmaEn;                 /* Enable/disable the dma for timer */

#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
    /* Enable the instance back to back mode between PDU0 CH0 and PDU1 CH0 pre-triggers,
    * forming a ring (PDU0_CH0_pretrigger7 -> PDU1_CH0_pretrigger0 and
    * PDU1_CH0_pretrigger7 -> PDU0_CH0_pretrigger0). */
    bool                     instanceBackToBackEn;
#endif /* FEATURE_PDU_WITH_INSTANCE_BACKTOBACK */
} PDU_TIMER_CONFIG_T;

/*!
 * @brief ADC's pre_trigger configuration structure.
 *
 */
typedef struct
{
    uint32_t adcPreTriggerIndex; /* ADC pre_trigger's index */

    /* Enable/disable the back to back mode for ADC pre_trigger. */
    bool preTriggerBackToBackEn;

    bool preTriggerEn;           /* Enable/disable the pre_trigger */
    bool preTriggerOutputEn;     /* Enable/disable the pre_trigger output */
} PDU_ADC_PRETRIGGER_CONFIG_T;

/**@} end of group PDU_Structures*/

/** @defgroup PDU_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void PDU_Init(const uint32_t instance, const PDU_TIMER_CONFIG_T *const timerConfig);
void PDU_DeInit(const uint32_t instance);
void PDU_DefaultConfig(PDU_TIMER_CONFIG_T *const timerConfig);
void PDU_Enable(const uint32_t instance);
void PDU_Disable(const uint32_t instance);
void PDU_EnableSoftTrigger(const uint32_t instance);
uint32_t PDU_ReadTimerValue(const uint32_t instance);
bool PDU_ReadTimerIntFlag(const uint32_t instance);
void PDU_ClearTimerIntFlag(const uint32_t instance);
void PDU_LoadValuesCmd(const uint32_t instance);

void PDU_ConfigCmpPulseOutEnable(
    const uint32_t instance,
    const uint32_t pulseChnMask,
    const bool enable);

void PDU_ConfigCmpPulseOutDelayForHigh(
    const uint32_t instance,
    const uint32_t pulseChannel,
    const uint32_t value);

void PDU_ConfigCmpPulseOutDelayForLow(
    const uint32_t instance,
    const uint32_t pulseChannel,
    const uint32_t value);

void PDU_ConfigPeriodCountValue(const uint32_t instance, const uint16_t value);
void PDU_ConfigTimerIntDelayValue(const uint32_t instance, const uint16_t value);

void PDU_ConfigAdcPreTrigger(
    const uint32_t instance,
    const uint32_t channel,
    const PDU_ADC_PRETRIGGER_CONFIG_T *configPtr);

uint32_t PDU_ReadAdcPreTriggerFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_ClearAdcPreTriggerFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask);

uint32_t PDU_ReadAdcPreTriggerSeqErrFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_ClearAdcPreTriggerSeqErrFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_ConfigAdcPreTriggerDelayValue(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChn,
    const uint32_t value);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void PDU_HW_Enable(PDU_T *const pduBase);
void PDU_HW_Disable(PDU_T *const pduBase);
void PDU_HW_EnableSoftTrigger(PDU_T *const pduBase);
bool PDU_HW_ReadTimerIntFlag(PDU_T const *const pduBase);
void PDU_HW_ClearTimerIntFlag(PDU_T * const pduBase);
void PDU_HW_ConfigLoadValuesCmd(PDU_T *const pduBase);
void PDU_HW_ConfigPeriodCountValue(PDU_T *const pduBase, uint16_t value);
uint32_t PDU_HW_ReadTimerValue(PDU_T const *const pduBase);
void PDU_HW_ConfigTimerIntDelayValue(PDU_T *const pduBase, uint16_t value);

uint32_t PDU_HW_ReadAdcPreTriggerFlags(
    PDU_T const *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_HW_ClearAdcPreTriggerFlags(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask);

uint32_t PDU_HW_ReadAdcPreTriggerSeqErrFlags(
    PDU_T const *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_HW_ClearAdcPreTriggerSeqErrFlags(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask);

void PDU_HW_ConfigAdcPreTriggerDelayValue(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChn,
    const uint32_t value);

void PDU_HW_ConfigCmpPulseOutEnable(
    PDU_T *const pduBase,
    const uint32_t pulseChnMask,
    const bool enable);

void PDU_HW_ConfigCmpPulseOutDelayForHigh(
    PDU_T *const pduBase,
    const uint32_t pulseChannel,
    const uint32_t value);

void PDU_HW_ConfigCmpPulseOutDelayForLow(
    PDU_T *const pduBase,
    const uint32_t pulseChannel,
    const uint32_t value);

#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
void PDU_HW_SIM_EnableInstanceBackToBack(void);
void PDU_HW_SIM_DisableInstanceBackToBack(void);
#endif /* FEATURE_PDU_WITH_INSTANCE_BACKTOBACK */

void PDU_HW_ResetRegister(PDU_T *const pduBase);
void PDU_HW_ConfigTimer(PDU_T *const pduBase, const PDU_TIMER_CONFIG_T *timerConfig);

void PDU_HW_EnableAdcPreTriggerBackToBackMode(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable);

void PDU_HW_EnableAdcPreTriggerOutput(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable);

void PDU_HW_EnableAdcPreTrigger(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable);

/**@} end of group PDU_Functions*/
/**@} end of group PDU_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined(__cplusplus)
}
#endif

#endif /* APM32F445_446_PDU_H */
