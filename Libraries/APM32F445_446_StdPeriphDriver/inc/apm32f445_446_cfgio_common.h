/*!
 * @file        apm32f445_446_cfgio_common.h
 *
 * @brief       CFGIO common functions
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

#ifndef APM32F445_446_CFGIO_COMMON_H
#define APM32F445_446_CFGIO_COMMON_H

/* Includes */
#include "device.h"
#include "apm32f445_446_dma.h"
#include "osif.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_COMMON_Driver CFGIO_COMMON Driver
  @{
*/

/** @defgroup CFGIO_COMMON_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* CFGIO driver type */
typedef enum
{
    CFGIO_USE_INTERRUPTS  = 0U,   /* Driver uses interrupts for data transfers */
    CFGIO_USE_POLLING     = 1U,   /* Driver is based on polling */
    CFGIO_USE_DMA         = 2U,   /* Driver uses DMA for data transfers */
} CFGIO_DRIVER_TYPE_T;

/* Pin configuration */
typedef enum
{
    CFGIO_PIN_CFG_DISABLED    = 0x00U,    /* Shifter pin output disabled */
    CFGIO_PIN_CFG_OPEN_DRAIN  = 0x01U,    /* Shifter pin open drain or bidirectional output enable */
    CFGIO_PIN_CFG_BI_OUTPUT   = 0x02U,    /* Shifter pin bidirectional output data */
    CFGIO_PIN_CFG_OUTPUT      = 0x03U,    /* Shifter pin output */
} CFGIO_PIN_CONFIG_T;

/* Pin polarity */
typedef enum
{
    CFGIO_PIN_POLARITY_HIGH = 0x00U,  /* Pin is active high */
    CFGIO_PIN_POLARITY_LOW  = 0x01U,  /* Pin is active low  */
} CFGIO_PIN_POLARITY_T;

/* Shifter input source */
typedef enum
{
    CFGIO_SHIFT_SRC_PIN     = 0x00U,  /* Input source is selected pin */
    CFGIO_SHIFT_SRC_SHIFTER = 0x01U,  /* Input source is shifter N+1 output */
} CFGIO_SHIFT_SRC_T;

/* Shifter mode */
typedef enum
{
    CFGIO_SHIFT_MODE_DISABLED         = 0x00U,    /* Shifter disabled */
    CFGIO_SHIFT_MODE_RECEIVE          = 0x01U,    /* Receive mode */
    CFGIO_SHIFT_MODE_TRANSMIT         = 0x02U,    /* Transmit mode */
    CFGIO_SHIFT_MODE_MATCH_STORE      = 0x04U,    /* Match Store mode */
    CFGIO_SHIFT_MODE_MATCH_CONTINUOUS = 0x05U,    /* Match Continuous mode */
} CFGIO_SHIFT_MODE_T;

/* Read/Write mode for shifter buffer */
typedef enum
{
    CFGIO_SHIFT_RW_MODE_NORMAL    = 0x00U,    /* Normal shifter buffer read/write */
    CFGIO_SHIFT_RW_MODE_BIT_SWAP  = 0x01U,    /* Data is bit-swapped */
} CFGIO_SHIFT_BUFFER_MODE_T;

/* Timer start bit options for Transmit, Receive or Match Store modes only */
typedef enum
{
    CFGIO_SHIFT_START_BIT_DISABLED    = 0x00U,    /* Start bit disabled, transmitter loads data on enable */
    CFGIO_SHIFT_START_BIT_DISABLED_SH = 0x01U,    /* Start bit disabled, transmitter loads data on first shift */
    CFGIO_SHIFT_START_BIT_0           = 0x02U,    /* Transmit/expect start bit value 0 */
    CFGIO_SHIFT_START_BIT_1           = 0x03U,    /* Transmit/expect start bit value 1 */
} CFGIO_SHIFT_START_T;

/* Timer stop bit options for Transmit, Receive or Match Store modes only */
typedef enum
{
    CFGIO_SHIFT_STOP_BIT_DISABLED = 0x00U,    /* Stop bit disabled */
    CFGIO_SHIFT_STOP_BIT_0        = 0x02U,    /* Transmit/expect stop bit value 0 */
    CFGIO_SHIFT_STOP_BIT_1        = 0x03U,    /* Transmit/expect stop bit value 1 */
} CFGIO_SHIFT_STOP_T;

/* Trigger source */
typedef enum
{
    CFGIO_TRG_SRC_EXTERNAL    = 0x00U,    /* External trigger selected */
    CFGIO_TRG_SRC_INTERNAL    = 0x01U,    /* Internal trigger selected */
} CFGIO_TRG_SRC_T;

/* Trigger polarity */
typedef enum
{
    CFGIO_TRG_POLARITY_HIGH   = 0x00U,    /* Trigger is active high */
    CFGIO_TRG_POLARITY_LOW    = 0x01U,    /* Trigger is active low  */
} CFGIO_TRG_POLARITY_T;

/* Shift clock polarity */
typedef enum
{
    CFGIO_TMR_POLARITY_POS_EDGE = 0x00U,  /* Shift on positive edge of shift clock */
    CFGIO_TMR_POLARITY_NEG_EDGE = 0x01U,  /* Shift on negative edge of shift clock */
} CFGIO_TMR_POLARITY_T;

/* Timer mode */
typedef enum
{
    CFGIO_TMR_MODE_DISABLED   = 0x00U,    /* Timer disabled */
    CFGIO_TMR_MODE_8BIT_BAUD  = 0x01U,    /* Dual 8-bit counters baud/bit mode */
    CFGIO_TMR_MODE_8BIT_PWM   = 0x02U,    /* Dual 8-bit counters PWM mode */
    CFGIO_TMR_MODE_16BIT      = 0x03U,    /* Single 16-bit counter mode */
} CFGIO_TMR_MODE_T;

/* Timer initial output options */
typedef enum
{
    CFGIO_TMR_OUTPUT_ONE        = 0x00U,  /* Timer output is logic one when enabled, unaffected by timer reset */
    CFGIO_TMR_OUTPUT_ZERO       = 0x01U,  /* Timer output is logic zero when enabled, unaffected by timer reset */
    CFGIO_TMR_OUTPUT_ONE_RESET  = 0x02U,  /* Timer output is logic one when enabled and on timer reset */
    CFGIO_TMR_OUTPUT_ZERO_RESET = 0x03U,  /* Timer output is logic zero when enabled and on timer reset */
} CFGIO_TMR_OUTPUT_T;

/* Timer decrement options */
typedef enum
{
    CFGIO_TMR_DEC_CLK_SHIFT_TMR = 0x00U, /* Decrement counter on CFGIO clock, Shift clock equals Timer output */
    CFGIO_TMR_DEC_TRG_SHIFT_TMR = 0x01U, /* Decrement counter on Trigger input (both edges), Shift clock equals Timer output */
    CFGIO_TMR_DEC_PIN_SHIFT_PIN = 0x02U, /* Decrement counter on Pin input (both edges), Shift clock equals Pin input */
    CFGIO_TMR_DEC_TRG_SHIFT_TRG = 0x03U, /* Decrement counter on Trigger input (both edges), Shift clock equals Trigger input */
} CFGIO_TMR_DEC_T;

/* Timer reset options   */
typedef enum
{
    CFGIO_TMR_RESET_NEVER         = 0x00U, /* Timer never reset */
    CFGIO_TMR_RESET_PIN_OUT       = 0x02U, /* Timer reset on Timer Pin equal to Timer Output */
    CFGIO_TMR_RESET_TRG_OUT       = 0x03U, /* Timer reset on Timer Trigger equal to Timer Output */
    CFGIO_TMR_RESET_PIN_RISING    = 0x04U, /* Timer reset on Timer Pin rising edge */
    CFGIO_TMR_RESET_TRG_RISING    = 0x06U, /* Timer reset on Trigger rising edge */
    CFGIO_TMR_RESET_TRG_BOTH      = 0x07U, /* Timer reset on Trigger rising or falling edge */
} CFGIO_TMR_RESET_T;

/* Timer enable options */
typedef enum
{
    CFGIO_TMR_ENABLE_ALWAYS                   = 0x00U,  /* Timer always enabled */
    CFGIO_TMR_ENABLE_TIM_ENABLE               = 0x01U,  /* Timer enabled on Timer N-1 enable */
    CFGIO_TMR_ENABLE_TRG_HIGH                 = 0x02U,  /* Timer enabled on Trigger high */
    CFGIO_TMR_ENABLE_TRG_PIN_HIGH             = 0x03U,  /* Timer enabled on Trigger high and Pin high */
    CFGIO_TMR_ENABLE_PIN_POS_EDGE             = 0x04U,  /* Timer enabled on Pin rising edge */
    CFGIO_TMR_ENABLE_PIN_POS_EDGE_TRG_HIGH    = 0x05U,  /* Timer enabled on Pin rising edge and Trigger high */
    CFGIO_TMR_ENABLE_TRG_POS_EDGE             = 0x06U,  /* Timer enabled on Trigger rising edge */
    CFGIO_TMR_ENABLE_TRG_EDGE                 = 0x07U,  /* Timer enabled on Trigger rising or falling edge */
} CFGIO_TMR_ENABLE_T;

/* Timer disable options */
typedef enum
{
    CFGIO_TMR_DISABLE_NEVER           = 0x00U,  /* Timer never disabled */
    CFGIO_TMR_DISABLE_TIM_DISABLE     = 0x01U,  /* Timer disabled on Timer N-1 disable */
    CFGIO_TMR_DISABLE_TIM_CMP         = 0x02U,  /* Timer disabled on Timer compare */
    CFGIO_TMR_DISABLE_TIM_CMP_TRG_LOW = 0x03U,  /* Timer disabled on Timer compare and Trigger Low */
    CFGIO_TMR_DISABLE_PIN             = 0x04U,  /* Timer disabled on Pin rising or falling edge */
    CFGIO_TMR_DISABLE_PIN_TRG_HIGH    = 0x05U,  /* Timer disabled on Pin rising or falling edge provided Trigger is high */
    CFGIO_TMR_DISABLE_TRG             = 0x06U,  /* Timer disabled on Trigger falling edge */
} CFGIO_TMR_DISABLE_T;

/* Timer start bit options */
typedef enum
{
    CFGIO_TMR_START_BIT_DISABLED  = 0x00U,    /* Start bit disabled */
    CFGIO_TMR_START_BIT_ENABLED   = 0x01U,    /* Start bit enabled */
} CFGIO_TMR_START_T;

/* Timer stop bit options */
typedef enum
{
    CFGIO_TMR_STOP_BIT_DISABLED       = 0x00U,    /* Stop bit disabled */
    CFGIO_TMR_STOP_BIT_TIM_CMP        = 0x01U,    /* Stop bit is enabled on timer compare */
    CFGIO_TMR_STOP_BIT_TIM_DIS        = 0x02U,    /* Stop bit is enabled on timer disable */
    CFGIO_TMR_STOP_BIT_TIM_CMP_DIS    = 0x03U,    /* Stop bit is enabled on timer compare and disable */
} CFGIO_TMR_STOP_T;

/**@} end of group CFGIO_COMMON_Enumerations*/

/** @defgroup CFGIO_COMMON_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief   CFGIO interrupt service routine
 */
typedef void (*CFGIO_ISR_T)(void *isrParam);

/*!
 * @brief   CFGIO common context structure
 * @details This is a common structure used by all CFGIO drivers as a part of
 *          their context structure. It is needed for common operations such as
 *          interrupt handling.
 */
typedef struct
{
    uint32_t instance;  /* CFGIO device instance number */
    uint8_t resCount;   /* Number of internal resources used (shifters and timers) */
    uint8_t resIndex;   /* Index of the first used internal resource instance (shifter and timer) */
    CFGIO_ISR_T isr;  /* Interrupt handler for this driver instance */
} CFGIO_COMMON_STATE_T;

/*!
 * @brief   CFGIO device context structure
 */
typedef struct
{
    uint8_t resAlloc;   /* Mask to track allocated resources on current device */
    MUTEX_T resMutex;   /* Mutex for guarding channel allocation */

    /**
     * Pointers to runtime states. Each CFGIO instance can have at most one
     * driver instance per shifter.
     */
    CFGIO_COMMON_STATE_T *commonStates[FEATURE_CFGIO_MAX_SHIFTER_COUNT];
} CFGIO_DEVICE_STATE_T;

/**@} end of group CFGIO_COMMON_Structures*/

/** @defgroup CFGIO_COMMON_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/**
 * Pointer to device states. This structure contains the data common to all
 * drivers on one device.
 */
extern CFGIO_DEVICE_STATE_T *g_cfgioDevState[CFGIO_INSTANCE_COUNT];

/* Clock sources for getting the input clock frequency */
extern const CLOCK_NAMES_T g_cfgioClockSrc[CFGIO_INSTANCE_COUNT];

/* IRQ numbers */
extern const IRQn_Type g_cfgioIrq[CFGIO_INSTANCE_COUNT];

/* DMA request sources */
extern const uint8_t g_cfgioDmaSrc[CFGIO_INSTANCE_COUNT][FEATURE_CFGIO_MAX_SHIFTER_COUNT];

/* Base addresses for CFGIO instances */
extern CFGIO_T * const g_cfgioBase[CFGIO_INSTANCE_COUNT];

/**@} end of group CFGIO_COMMON_Variables*/

/** @defgroup CFGIO_COMMON_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/* Called by the user */
STATUS_T CFGIO_InitDevice(uint32_t instance, CFGIO_DEVICE_STATE_T *deviceState);
STATUS_T CFGIO_DeinitDevice(uint32_t instance);
STATUS_T CFGIO_ResetDevice(uint32_t instance);

/* Used by other CFGIO drivers (e.g. CFGIO_I2C, CFGIO_SPI) */
STATUS_T CFGIO_InitDriver(uint32_t instance, CFGIO_COMMON_STATE_T *commonState);
STATUS_T CFGIO_DeinitDriver(const CFGIO_COMMON_STATE_T *commonState);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void CFGIO_HW_Init(CFGIO_T *base);
void CFGIO_HW_SoftwareReset(CFGIO_T *base, bool enable);
void CFGIO_HW_SetModuleEnable(CFGIO_T *base, bool enable);
void CFGIO_HW_SetDebugMode(CFGIO_T *base, bool enable);

uint8_t CFGIO_HW_GetShifterNumber(const CFGIO_T *base);
uint8_t CFGIO_HW_GetPinData(const CFGIO_T *base);

void CFGIO_HW_SetTimerConfig(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_START_T start,
    CFGIO_TMR_STOP_T stop,
    CFGIO_TMR_ENABLE_T enable,
    CFGIO_TMR_DISABLE_T disable,
    CFGIO_TMR_RESET_T reset,
    CFGIO_TMR_DEC_T decrement,
    CFGIO_TMR_OUTPUT_T output);

void CFGIO_HW_SetTimerEnable(CFGIO_T *base, uint8_t timer, CFGIO_TMR_ENABLE_T enable);
void CFGIO_HW_SetTimerDisable(CFGIO_T *base, uint8_t timer, CFGIO_TMR_DISABLE_T disable);
void CFGIO_HW_SetTimerStart(CFGIO_T *base, uint8_t timer, CFGIO_TMR_START_T start);
void CFGIO_HW_SetTimerStop(CFGIO_T *base, uint8_t timer, CFGIO_TMR_STOP_T stop);
void CFGIO_HW_SetTimerReset(CFGIO_T *base, uint8_t timer, CFGIO_TMR_RESET_T reset);
void CFGIO_HW_SetTimerDecrement(CFGIO_T *base,uint8_t timer,CFGIO_TMR_DEC_T decrement);
void CFGIO_HW_SetTimerInitialOutput(CFGIO_T *base, uint8_t timer, CFGIO_TMR_OUTPUT_T output);

void CFGIO_HW_SetTimerControl(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t triggerSelect,
    CFGIO_TRG_POLARITY_T triggerPolarity,
    CFGIO_TRG_SRC_T triggerSource,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig,
    CFGIO_TMR_MODE_T mode);

void CFGIO_HW_SetTimerPin(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig);

void CFGIO_HW_SetTimerTrigger(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t triggerSelect,
    CFGIO_TRG_POLARITY_T triggerPolarity,
    CFGIO_TRG_SRC_T triggerSource);

void CFGIO_HW_SetTimerMode(CFGIO_T *base,uint8_t timer,CFGIO_TMR_MODE_T mode);

void CFGIO_HW_SetTimerCompare(CFGIO_T *base, uint8_t timer, uint16_t value);
uint16_t CFGIO_HW_GetTimerCompare(const CFGIO_T *base, uint8_t timer);

void CFGIO_HW_SetTimerInterrupts(CFGIO_T *base, uint8_t interruptMask, bool enable);
uint32_t CFGIO_HW_GetTimerInterruptStates(const CFGIO_T *base);

uint32_t CFGIO_HW_GetAllTimerStatus(const CFGIO_T *base);
bool CFGIO_HW_GetTimerStatus(const CFGIO_T *base, uint8_t timer);
void CFGIO_HW_ClearTimerStatus(CFGIO_T *base, uint8_t timer);

void CFGIO_HW_SetShifterConfig(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_START_T start,
    CFGIO_SHIFT_STOP_T stop,
    CFGIO_SHIFT_SRC_T source);

void CFGIO_HW_SetShifterInputSource(CFGIO_T *base, uint8_t shifter, CFGIO_SHIFT_SRC_T source);
void CFGIO_HW_SetShifterStartBit(CFGIO_T *base, uint8_t shifter, CFGIO_SHIFT_START_T start);
void CFGIO_HW_SetShifterStopBit(CFGIO_T *base, uint8_t shifter, CFGIO_SHIFT_STOP_T stop);

void CFGIO_HW_SetShifterControl(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_MODE_T mode,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig,
    uint8_t timerSelect,
    CFGIO_TMR_POLARITY_T timerPolarity);

void CFGIO_HW_SetShifterTimer(
    CFGIO_T *base,
    uint8_t shifter,
    uint8_t timerSelect,
    CFGIO_TMR_POLARITY_T timerPolarity);

void CFGIO_HW_SetShifterPin(
    CFGIO_T *base,
    uint8_t shifter,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig);

void CFGIO_HW_SetShifterPinConfig(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_PIN_CONFIG_T pinConfig);

void CFGIO_HW_SetShifterMode(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_MODE_T mode);

uint32_t CFGIO_HW_GetAllShifterStatus(const CFGIO_T *base);
bool CFGIO_HW_GetShifterStatus(const CFGIO_T *base, uint8_t shifter);
void CFGIO_HW_ClearShifterStatus(CFGIO_T *base, uint8_t shifter);

uint32_t CFGIO_HW_GetAllShifterErrorStatus(const CFGIO_T *base);
bool CFGIO_HW_GetShifterErrorStatus(const CFGIO_T *base, uint8_t shifter);
void CFGIO_HW_ClearShifterErrorStatus(CFGIO_T *base, uint8_t shifter);

uint32_t CFGIO_HW_GetAllShifterInterrupt(const CFGIO_T *base);
void CFGIO_HW_SetShifterInterrupt(CFGIO_T *base, uint8_t interruptMask, bool enable);

uint32_t CFGIO_HW_GetAllShifterErrorInterrupt(const CFGIO_T *base);
void CFGIO_HW_SetShifterErrorInterrupt(CFGIO_T *base, uint8_t interruptMask, bool enable);

uint32_t CFGIO_HW_ReadShifterBuffer(
    const CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_BUFFER_MODE_T mode);

void CFGIO_HW_WriteShifterBuffer(
    CFGIO_T *base,
    uint8_t shifter,
    uint32_t value,
    CFGIO_SHIFT_BUFFER_MODE_T mode);

void CFGIO_HW_SetShifterDmaRequest(CFGIO_T *base, uint8_t requestMask, bool enable);

/**@} end of group CFGIO_COMMMON_Functions*/
/**@} end of group CFGIO_COMMON_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_CFGIO_COMMON_H */
