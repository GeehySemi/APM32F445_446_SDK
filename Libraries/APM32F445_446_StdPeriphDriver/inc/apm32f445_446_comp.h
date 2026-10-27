/*!
 * @file        apm32f445_446_comp.h
 *
 * @brief       This file contains all the functions prototypes for the COMP firmware library.
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

#ifndef APM32F445_446_COMP_H
#define APM32F445_446_COMP_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */
#include "device.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup COMP_Driver COMP Driver
  @{
*/

/** @defgroup COMP_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* Comparator functional modes */
typedef enum
{
    COMP_DISABLED_MODE                  = 0U,
    COMP_CONSTANT_MODE                  = 1U,
    COMP_SMPED_NONFILTRED_INT_CLK_MODE  = 2U,
    COMP_SMPED_NONFILTRED_EXT_CLK_MODE  = 3U,
    COMP_SMPED_FILTRED_INT_CLK_MODE     = 4U,
    COMP_SMPED_FILTRED_EXT_CLK_MODE     = 5U,
    COMP_WINDOWED_MODE                  = 6U,
    COMP_WINDOWED_RESMPED_MODE          = 7U,
    COMP_WINDOWED_FILTRED_MODE          = 8U
} COMP_MODE_T;

/* Comparator Round-Robin fixed port */
typedef enum
{
    COMP_PLUS_FIXED  = 0U, /* The Plus port is fixed. Only the inputs to the Minus port are swept in each round */
    COMP_MINUS_FIXED = 1U  /* The Minus port is fixed. Only the inputs to the Plus port are swept in each round */
} COMP_FIXED_PM_T;

/* Comparator hysteresis control */
typedef enum
{
    COMP_LEVEL_HYS_0 = 0U,
    COMP_LEVEL_HYS_1 = 1U,
    COMP_LEVEL_HYS_2 = 2U,
    COMP_LEVEL_HYS_3 = 3U
} COMP_HYS_T;

/* Comparator output interrupt configuration */
typedef enum
{
    COMP_NO_EVENT_NO_INT            = 0U,   /* Comparator output interrupts are disabled OR no event occurred */
    COMP_EVEN_INT_BY_FALLING_EDGE   = 1U,   /* Comparator output interrupts will be generated only on falling edge OR only falling edge event occurred */
    COMP_EVEN_INT_BY_RISING_EDGE    = 2U,   /* Comparator output interrupts  will be generated only on rising edge OR only rising edge event occurred */
    COMP_EVEN_INT_BY_BOTH_EDGES     = 3U    /* Comparator output interrupts  will be generated on both edges OR both edges event occurred */
} COMP_OUTPUT_TRIG_T;

/* Comparator output invert selection */
typedef enum
{
    COMP_NORMAL = 0U,   /* Output signal isn't inverted */
    COMP_INVERT = 1U    /* Output signal is inverted */
} COMP_INVERT_T;

/* Port Mux Source selection */
typedef enum
{
    COMP_DAC = 0U, /* Select DAC as source for the comparator port */
    COMP_MUX = 1U  /* Select MUX8 as source for the comparator port */
} COMP_PM_MUX_T;

/* Voltage Reference selection */
typedef enum
{
    COMP_VOLT_REF_SRC_VIN1  = 0U,   /* Use Vin1 as supply reference source for DAC */
    COMP_VOLT_REF_SRC_VIN2  = 1U    /* Use Vin2 as supply reference source for DAC */
} COMP_VOLT_REF_T;

/* Power Modes selection */
typedef enum
{
    COMP_SPEED_LOW  = 0U,   /* Module in low speed mode */
    COMP_SPEED_HIGH = 1U    /* Module in high speed mode */
} COMP_PWR_MODE_T;

/* Comparator output pin enable selection */
typedef enum
{
    COMP_UNUSABLE  = 0U,    /* Comparator output isn't available to a specific pin */
    COMP_USABLE    = 1U     /* Comparator output is available to a specific pin */
} COMP_OUTPUT_ENABLE_T;

/* Comparator output select selection */
typedef enum
{
    COMP_COUT  = 0U,    /* Select COUT as comparator output signal */
    COMP_COUTA = 1U     /* Select COUTA as comparator output signal */
} COMP_OUTPUT_SEL_T;

/* Comparator hard block offset control */
typedef enum
{
    COMP_LEVEL_OFFSET_0 = 0U,
    COMP_LEVEL_OFFSET_1 = 1U
} COMP_HBOLSEL_T;

/**@} end of group COMP_Enumerations*/

/** @defgroup COMP_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/* Comparator channels list */
typedef uint8_t COMP_CH_LIST_T;

/* Number of channel */
typedef uint8_t COMP_CH_NUMBER_T;

/*
 * Defines the block configuration
 * Used to configure only comparator block module (filtering, sampling, power mode etc.)
 */
typedef struct
{
    COMP_MODE_T             mode;
    COMP_PWR_MODE_T         pwrMode;
    COMP_OUTPUT_TRIG_T      outputIntTrig;
    bool                    dmaTrigStatus;  /* True if DMA transfer trigger from comparator is enable */
    uint8_t                 filtSmpCnt;     /* Number of sample count for filtering */
    uint8_t                 filtSmpPeriod;  /* Filter sample period */
    COMP_OUTPUT_SEL_T       outputSel;
    COMP_OUTPUT_ENABLE_T    pinStatus;
    COMP_INVERT_T           invertStatus;
    COMP_HYS_T              hysLev;
    COMP_HBOLSEL_T          offset;
} COMP_COMPARATOR_T;

/*
 * Defines the analog mux
 * Used to configure the analog multiplexer to select compared signals
 */
typedef struct
{
    COMP_PM_MUX_T       negPmMux;       /* Negative port signal */
    COMP_CH_NUMBER_T    negInputMux;    /* Which channel is selected for the minus mux */
    COMP_PM_MUX_T       posPmMux;       /* Positive port signal */
    COMP_CH_NUMBER_T    posInputMux;    /* Which channel is selected for the plus mux */
} COMP_ANMUX_T;

/*
 * Defines the DAC block
 * Used to configure the DAC block in comparator module
 */
typedef struct
{
    uint8_t volt;   /* The digital value which is converted to analog signal */
    bool    state;  /* True if DAC is enabled */
    COMP_VOLT_REF_T voltRefSrc; /* COMP Voltage reference source*/

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    bool                fixRefInputMux;
#endif
} COMP_DAC_T;

/*
 * Defines the trigger mode
 * Used to configure the trigger mode when MCU enters STOP modes
 */
typedef struct
{
    COMP_FIXED_PM_T     fixedPm;
    COMP_CH_NUMBER_T    fixedChannel;               /* Which channel would be assigned to the fixed port */
    COMP_CH_LIST_T      roundRobinChannelsStatus;   /* One bite for each channel state */
    COMP_CH_LIST_T      programedStatus;            /* Pre-programmed state for comparison result */
    bool                roundRobinStatus;           /* True if Round-Robin is enabled */
    bool                roundRobinIntStatus;        /* True if Round-Robin interrupt is enabled */
    uint8_t             samplesNumber;              /* Number of round-robin clock cycles for a given channel */
    uint8_t             initDelay;                  /* DAC and comparator initialization delay (clock cycles) */
} COMP_TRIG_MODE_T;

/*
 * Defines comparator module configuration
 * Used to configure all components of comparator module
 */
typedef struct
{
    COMP_COMPARATOR_T   comparator;
    COMP_TRIG_MODE_T    trigMode;
    COMP_DAC_T          dac;
    COMP_ANMUX_T        mux;
} COMP_MODULE_T;

/**@} end of group COMP_Structures*/

/** @defgroup COMP_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC FUNCTIONS
 ******************************************************************************/

STATUS_T COMP_Init(const uint32_t ins, const COMP_MODULE_T* const cfg);
STATUS_T COMP_Reset(const uint32_t ins);

STATUS_T COMP_DefaultConfig(COMP_MODULE_T* const cfg);

STATUS_T CMP_GetAllInitConfig(COMP_MODULE_T* cfg);
STATUS_T CMP_GetDACInitConfig(COMP_DAC_T* cfg);
STATUS_T CMP_GetMUXInitConfig(COMP_ANMUX_T* cfg);
STATUS_T CMP_GetCompInitConfig(COMP_COMPARATOR_T* cfg);
STATUS_T CMP_GetTriggerModeInitConfig(COMP_TRIG_MODE_T* cfg);

STATUS_T CMP_GetAllConfig(const uint32_t ins, COMP_MODULE_T* const cfg);
STATUS_T CMP_GetDACConfig(const uint32_t ins, COMP_DAC_T* const cfg);
STATUS_T CMP_GetMUXConfig(const uint32_t ins, COMP_ANMUX_T* const cfg);
STATUS_T CMP_GetCompConfig(const uint32_t ins, COMP_COMPARATOR_T *cfg);
STATUS_T CMP_GetTriggerModeConfig(const uint32_t ins, COMP_TRIG_MODE_T* const cfg);

STATUS_T CMP_ConfigDAC(const uint32_t ins, const COMP_DAC_T* cfg);
STATUS_T CMP_ConfigMUX(const uint32_t ins, const COMP_ANMUX_T* cfg);
STATUS_T CMP_ConfigComp(const uint32_t ins, const COMP_COMPARATOR_T *cfg);
STATUS_T CMP_ConfigTriggerMode(const uint32_t ins, const COMP_TRIG_MODE_T* cfg);

STATUS_T CMP_ClearInputFlags(const uint32_t ins);
STATUS_T CMP_GetInputFlags(const uint32_t ins, COMP_CH_LIST_T *flags);

STATUS_T COMP_ClearOutputFlags(const uint32_t ins);
STATUS_T COMP_GetOutputFlags(const uint32_t ins, COMP_OUTPUT_TRIG_T *flags);

/*******************************************************************************
 *                          HARDWARE FUNCTIONS
 ******************************************************************************/

bool COMP_HW_GetDACStatus(const COMP_T *base);
void COMP_HW_SetDACStatus(COMP_T *base, bool value);

uint8_t COMP_HW_GetFiltSmpCnt(const COMP_T *base);
void COMP_HW_SetFiltSmpCnt(COMP_T *base, uint8_t value);

COMP_HYS_T COMP_HW_GetHys(const COMP_T *base);
void COMP_HW_SetHys(COMP_T *base, COMP_HYS_T value);

COMP_PM_MUX_T COMP_HW_GetNegPmInput(const COMP_T *base);
void COMP_HW_SetNegPmInput(COMP_T *base, COMP_PM_MUX_T value);

COMP_CH_LIST_T COMP_HW_GetRoundRobinChannels(const COMP_T *base);
void COMP_HW_SetRoundRobinChannels(COMP_T *base, COMP_CH_LIST_T value);

COMP_PM_MUX_T COMP_HW_GetPosPmInput(const COMP_T *base);
void COMP_HW_SetPosPmInput(COMP_T *base, COMP_PM_MUX_T value);

COMP_CH_NUMBER_T COMP_HW_GetPlusMUXCtrl(const COMP_T *base);
void COMP_HW_SetPlusMUXCtrl(COMP_T *base, COMP_CH_NUMBER_T value);

COMP_CH_NUMBER_T COMP_HW_GetMinMUXCtrl(const COMP_T *base);
void COMP_HW_SetMinMUXCtrl(COMP_T *base, COMP_CH_NUMBER_T value);

COMP_FIXED_PM_T COMP_HW_GetFixedPm(const COMP_T *base);
void COMP_HW_SetFixedPm(COMP_T *base, COMP_FIXED_PM_T value);

COMP_CH_NUMBER_T COMP_HW_GetFixedChannel(const COMP_T *base);
void COMP_HW_SetFixedChannel(COMP_T *base, COMP_CH_NUMBER_T value);

COMP_VOLT_REF_T COMP_HW_GetVoltRef(const COMP_T *base);
void COMP_HW_SetVoltRef(COMP_T *base, COMP_VOLT_REF_T value);

uint8_t COMP_HW_GetVolt(const COMP_T *base);
void COMP_HW_SetVolt(COMP_T *base, uint8_t value);

bool COMP_HW_GetDMATrigStatus(const COMP_T *base);
void COMP_HW_SetDMATrigStatus(COMP_T *base, bool value);

COMP_PWR_MODE_T COMP_HW_GetPwrMode(const COMP_T *base);
void COMP_HW_SetPwrMode(COMP_T *base, COMP_PWR_MODE_T value);

COMP_OUTPUT_TRIG_T COMP_HW_GetOutputIntTrig(const COMP_T *base);
void COMP_HW_SetOutputIntTrig(COMP_T *base, COMP_OUTPUT_TRIG_T value);

uint8_t COMP_HW_GetFiltSmpPeriod(const COMP_T *base);
void COMP_HW_SetFiltSmpPeriod(COMP_T *base, uint8_t value);

COMP_INVERT_T COMP_HW_GetInvertStatus(const COMP_T *base);
void COMP_HW_SetInvertStatus(COMP_T *base, COMP_INVERT_T value);

COMP_OUTPUT_SEL_T COMP_HW_GetCompOutputSrc(const COMP_T *base);
void COMP_HW_SetCompOutputSrc(COMP_T *base, COMP_OUTPUT_SEL_T value);

COMP_OUTPUT_ENABLE_T COMP_HW_GetOutputPinStatus(const COMP_T *base);
void COMP_HW_SetOutputPinStatus(COMP_T *base, COMP_OUTPUT_ENABLE_T value);

uint8_t COMP_HW_GetRoundRobinSamplesNo(const COMP_T *base);
void COMP_HW_SetRoundRobinSamplesNo(COMP_T *base, uint8_t value);

bool COMP_HW_GetRoundRobinStatus(const COMP_T *base);
void COMP_HW_SetRoundRobinStatus(COMP_T *base, bool value);

bool COMP_HW_GetRoundRobinIntStatus(const COMP_T *base);
void COMP_HW_SetRoundRobinIntStatus(COMP_T *base, bool value);

COMP_CH_LIST_T COMP_HW_GetLastCompareResult(const COMP_T *base);

void COMP_HW_SetPresetStatus(COMP_T *base, COMP_CH_LIST_T value);

COMP_MODE_T COMP_HW_GetFuncMode(const COMP_T *base);
void COMP_HW_SetFuncMode(
    COMP_T *base,
    COMP_MODE_T mode,
    uint8_t sampleCount,
    uint8_t samplePeriod);
void COMP_HW_SetOffset(COMP_T *base, COMP_HBOLSEL_T value);
COMP_HBOLSEL_T COMP_HW_GetOffset(const COMP_T *base);
void COMP_HW_SetInitDelay(COMP_T *base, uint8_t value);
uint8_t COMP_HW_GetInitDelay(const COMP_T *base);

/**@} end of group COMP_Functions*/
/**@} end of group COMP_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_COMP_H */
