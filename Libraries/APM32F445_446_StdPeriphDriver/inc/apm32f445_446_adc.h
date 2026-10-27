/*!
 * @file        apm32f445_446_adc.h
 *
 * @brief       This file contains all the functions prototypes for the ADC firmware library.
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


#ifndef APM32F445_446_ADC_H
#define APM32F445_446_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup ADC_Driver ADC Driver
  @{
*/

/** @defgroup ADC_Variables Variables
  @{
*/

/* Table of base addresses for ADC instances. */
extern ADC_T           * const g_adcBase[ADC_INSTANCE_COUNT];

/**@} end of group ADC_Variables*/


/** @defgroup ADC_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* Clock Divider selection */
typedef enum
{
    ADC_CLK_DIVISION_1 = 0x00U,   /* clock division 1 */
    ADC_CLK_DIVISION_2 = 0x01U,   /* clock division 2 */
    ADC_CLK_DIVISION_4 = 0x02U,   /* clock division 4 */
    ADC_CLK_DIVISION_8 = 0x03U    /* clock division 8 */
} ADC_CLK_DIVISION_T;

/* Conversion resolution selection */
typedef enum
{
    ADC_RESOLUTION_RATIO_8BIT  = 0x00U,   /* 8-bit resolution */
    ADC_RESOLUTION_RATIO_12BIT = 0x01U,   /* 12-bit resolution */
    ADC_RESOLUTION_RATIO_10BIT = 0x02U    /* 10-bit resolution */
} ADC_RESOLUTION_RATIO_T;

/* Select the input clock source */
typedef enum
{
    ADC_INPUT_CLK_1 = 0x00U,  /* Input clock 1 */
    ADC_INPUT_CLK_2 = 0x01U,  /* Input clock 2 */
    ADC_INPUT_CLK_3 = 0x02U,  /* Input clock 3 */
    ADC_INPUT_CLK_4 = 0x03U   /* Input clock 4 */
} ADC_INPUT_CLK_T;

/* Select trigger type */
typedef enum
{
    ADC_SOFTWARE_TRIGGER = 0x00U,   /* Software trigger */
    ADC_HARDWARE_TRIGGER = 0x01U    /* Hardware trigger */
} ADC_TRIGGER_T;

/* Select a pre-trigger type */
typedef enum
{
    ADC_PDU_PRE_TRIGGER = 0x00U,   /* PDU pretrigger selected */
    ADC_TMC_PRE_TRIGGER = 0x01U,   /* TMC pretrigger selected */
    ADC_SW_PRE_TRIGGER  = 0x02U    /* Software pretrigger selected */
} ADC_PRE_TRIGGER_T;

/* Selective trigger source */
typedef enum
{
    ADC_PDU_TRIGGER = 0x00U,   /* PDU trigger selected */
    ADC_TMC_TRIGGER = 0x01U    /* TMC trigger selected */
} ADC_SEL_TRIGGER_T;

/* You can set the software pre-trigger */
typedef enum
{
    ADC_SW_PRE_TRIGGER_DIS  = 0x00U,   /* SW pretrigger disabled */
    ADC_SW_PRE_TRIGGER_0    = 0x04U,   /* SW pretrigger 0 */
    ADC_SW_PRE_TRIGGER_1    = 0x05U,   /* SW pretrigger 1 */
    ADC_SW_PRE_TRIGGER_2    = 0x06U,   /* SW pretrigger 2 */
    ADC_SW_PRE_TRIGGER_3    = 0x07U    /* SW pretrigger 3 */
} ADC_SW_PRE_TRIGGER_T;

/* select Voltage reference */
typedef enum
{
    ADC_VOLT_REF_VREF = 0x00U,    /* VrefH and VrefL as Voltage reference */
    ADC_VOLT_REF_VALT = 0x01U     /* ValtH and ValtL as Voltage reference */
} ADC_VOLT_REF_T;

/* select Hardware average */
typedef enum
{
    ADC_HW_AVERAGE_4  = 0x00U,  /* Hardware average of 4 samples */
    ADC_HW_AVERAGE_8  = 0x01U,  /* Hardware average of 8 samples */
    ADC_HW_AVERAGE_16 = 0x02U,  /* Hardware average of 16 samples */
    ADC_HW_AVERAGE_32 = 0x03U   /* Hardware average of 32 samples */
} ADC_AVERAGE_T;

/* Enumeration of control input channels */
typedef enum
{
    ADC_INPUT_CHANNEL_EXT0      = 0x00U,    /* External input channel 0 */
    ADC_INPUT_CHANNEL_EXT1      = 0x01U,    /* External input channel 1 */
#if (FEATURE_ADC_WITH_CHANNEL_2)
    ADC_INPUT_CHANNEL_EXT2      = 0x02U,    /* External input channel 2 */
#endif
    ADC_INPUT_CHANNEL_EXT3      = 0x03U,    /* External input channel 3 */
    ADC_INPUT_CHANNEL_EXT4      = 0x04U,    /* External input channel 4 */
    ADC_INPUT_CHANNEL_EXT5      = 0x05U,    /* External input channel 5 */
    ADC_INPUT_CHANNEL_EXT6      = 0x06U,    /* External input channel 6 */
    ADC_INPUT_CHANNEL_EXT7      = 0x07U,    /* External input channel 7 */
#if (FEATURE_ADC_WITH_CHANNEL_8)
    ADC_INPUT_CHANNEL_EXT8      = 0x08U,    /* External input channel 8 */
#endif
    ADC_INPUT_CHANNEL_EXT9      = 0x09U,    /* External input channel 9 */
    ADC_INPUT_CHANNEL_EXT10     = 0x0AU,    /* External input channel 10 */
    ADC_INPUT_CHANNEL_EXT11     = 0x0BU,    /* External input channel 11 */
    ADC_INPUT_CHANNEL_EXT12     = 0x0CU,    /* External input channel 12 */
    ADC_INPUT_CHANNEL_EXT13     = 0x0DU,    /* External input channel 13 */
    ADC_INPUT_CHANNEL_EXT14     = 0x0EU,    /* External input channel 14 */
#if (FEATURE_ADC_EXT_CHNS_MAX_NUM <= 16)
    ADC_INPUT_CHANNEL_EXT15     = 0x0FU,    /* External input channel 15 */
#if (FEATURE_ADC_EXT_CHNS_MAX_NUM > 16)
    ADC_INPUT_CHANNEL_EXT16     = 0x20U,    /* External input channel 16 */
    ADC_INPUT_CHANNEL_EXT17     = 0x21U,    /* External input channel 17 */
    ADC_INPUT_CHANNEL_EXT18     = 0x22U,    /* External input channel 18 */
    ADC_INPUT_CHANNEL_EXT19     = 0x23U,    /* External input channel 19 */
    ADC_INPUT_CHANNEL_EXT20     = 0x24U,    /* External input channel 20 */
    ADC_INPUT_CHANNEL_EXT21     = 0x25U,    /* External input channel 21 */
    ADC_INPUT_CHANNEL_EXT22     = 0x26U,    /* External input channel 22 */
    ADC_INPUT_CHANNEL_EXT23     = 0x27U,    /* External input channel 23 */
#if (FEATURE_ADC_EXT_CHNS_MAX_NUM > 24)
    ADC_INPUT_CHANNEL_EXT24     = 0x28U,    /* External input channel 24 */
    ADC_INPUT_CHANNEL_EXT25     = 0x29U,    /* External input channel 25 */
    ADC_INPUT_CHANNEL_EXT26     = 0x2AU,    /* External input channel 26 */
    ADC_INPUT_CHANNEL_EXT27     = 0x2BU,    /* External input channel 27 */
    ADC_INPUT_CHANNEL_EXT28     = 0x2CU,    /* External input channel 28 */
    ADC_INPUT_CHANNEL_EXT29     = 0x2DU,    /* External input channel 29 */
    ADC_INPUT_CHANNEL_EXT30     = 0x2EU,    /* External input channel 30 */
    ADC_INPUT_CHANNEL_EXT31     = 0x2FU,    /* External input channel 31 */
#endif /* (FEATURE_ADC_EXT_CHNS_MAX_NUM > 24) */
#endif /* (FEATURE_ADC_EXT_CHNS_MAX_NUM > 16) */
#endif /* (FEATURE_ADC_EXT_CHNS_MAX_NUM <= 16) */

#if (FEATURE_ADC_EXT_CHNS_MAX_NUM <= 16)
    ADC_INPUT_CHANNEL_DIS       = 0x1FU,    /* input Channel disabled */
#elif (FEATURE_ADC_EXT_CHNS_MAX_NUM > 16)
    ADC_INPUT_CHANNEL_DIS       = 0x3FU,    /* input Channel disabled */
#endif
    ADC_INPUT_CHANNEL_INT0      = 0x15U,               /* Internal input channel 0 */
    ADC_INPUT_CHANNEL_INT1      = 0x16U,               /* Internal input channel 1 */
    ADC_INPUT_CHANNEL_INT2      = 0x17U,               /* Internal input channel 2 */
    ADC_INPUT_CHANNEL_INT3      = 0x1CU,               /* Internal input channel 3 */

    ADC_INPUT_CHANNEL_TEMP      = 0x1AU,               /* Temperature Sensor */
    ADC_INPUT_CHANNEL_BANDGAP   = 0x1BU,               /* Band Gap */
    ADC_INPUT_CHANNEL_VREFSH    = 0x1DU,               /* Voltage Reference Select High */
    ADC_INPUT_CHANNEL_VREFSL    = 0x1EU,               /* Voltage Reference Select Low */

    ADC_INPUT_CHANNEL_SUPPLY_VDD            = 0xF00U,   /* Monitor internal supply 5 V input VDD supply */
    ADC_INPUT_CHANNEL_SUPPLY_VDDA           = 0xF01U,   /* Monitor internal supply 5 V input analog supply */
    ADC_INPUT_CHANNEL_SUPPLY_VREFH          = 0xF02U,   /* Monitor internal supply ADC reference supply */
    ADC_INPUT_CHANNEL_SUPPLY_VDD_3V         = 0xF03U,   /* Monitor internal supply 3.3 V oscillator regulator output */
    ADC_INPUT_CHANNEL_SUPPLY_VDD_FLASH_3V   = 0xF04U,   /* Monitor internal supply 3.3 V flash regulator output */
    ADC_INPUT_CHANNEL_SUPPLY_VDD_LV         = 0xF05U    /* Monitor internal supply 1.2 V core regulator output */
} ADC_INPUT_CHANNEL_T;

/* Defines the trigger latch clear method */
typedef enum
{
    ADC_LATCH_CLEAR_WAIT, /* Clear by waiting for the trigger to process all latches */
    ADC_LATCH_CLEAR_FORCE /* Process current trigger and clear all locks */
} ADC_LATCH_CLEAR_T;


/**@} end of group ADC_Enumerations*/

/** @defgroup ADC_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/
/* Defines the converter configuration */
typedef struct
{
    bool supplyMonEnable;                  /* Only available for ADC 0. Enable internal supply monitoring
                                              - enables measurement of ADC_INPUTCHAN_SUPPLY_ sources. */
    bool contConvEnable;                   /* Enable Continuous conversions */
    ADC_VOLT_REF_T voltRef;                /* Voltage reference used */
    bool dmaEnable;                        /* Enable DMA for the ADC */
    ADC_SEL_TRIGGER_T selTrigger;          /* Trigger source selected from Trigger Latching and Arbitration Unit */
    ADC_PRE_TRIGGER_T selPretrigger;       /* Pretrigger source selected from Trigger Latching and Arbitration Unit
                                              - affects only the first 4 control channels */
    ADC_TRIGGER_T trigger;                 /* ADC trigger type (software, hardware)
                                              - affects only the first control channel */
    ADC_INPUT_CLK_T inputClock;            /* Input clock source */
    ADC_RESOLUTION_RATIO_T resolution;     /* ADC resolution (8,10,12 bit) */
    uint8_t adcSampleTime;                 /* Sample time in AD Clocks */
    ADC_CLK_DIVISION_T clockDivision;      /* Divider of the input clock for the ADC */
} ADC_CONV_CFG_T;


/* Defines hardware comparison configuration */
typedef struct
{
    uint16_t compValue2;        /* Second Compare Value */
    uint16_t compValue1;        /* First Compare Value */
    bool compRangeFuncEnable;   /* Enable Range functionality */
    bool compGreaterThanEnable; /* Enable Greater-Than functionality */
    bool compEnable;            /* Enable the compare feature */
} ADC_COMP_CFG_T;

/* Defines hardware average configuration */
typedef struct
{
    ADC_AVERAGE_T hwAverage; /* Selection for number of samples used for averaging */
    bool hwAvgEnable;        /* Enable averaging functionality */
} ADC_AVG_CFG_T;

/* Defines control channel configuration */
typedef struct
{
    ADC_INPUT_CHANNEL_T channel; /* Selection of input channel for measurement */
    bool interruptEnable;        /* Enable interrupts for this channel */
} ADC_CHAN_CONFIG_T;

/* Defines the user calibration configuration */
typedef struct
{
    uint16_t userOffset;  /* User-configurable Offset (2's complement, subtracted from result) */
    uint16_t userGain;    /* User-configurable gain */
} ADC_CALIBRATION_T;

/**@} end of group ADC_Structures*/




/** @defgroup ADC_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
void ADC_ConfigConverterStruct(ADC_CONV_CFG_T * const config);
void ADC_ConfigConverter(const uint32_t instanceNum, const ADC_CONV_CFG_T * const config);
void ADC_ReadConverterConfig(const uint32_t instanceNum, ADC_CONV_CFG_T * const config);
void ADC_ConfigHwCompareStruct(ADC_COMP_CFG_T * const config);
void ADC_ConfigHwCompare(const uint32_t instanceNum, const ADC_COMP_CFG_T * const config);
void ADC_ReadHwCompareConfig(const uint32_t instanceNum, ADC_COMP_CFG_T * const config);
void ADC_ConfigHwAverageStruct(ADC_AVG_CFG_T * const config);
void ADC_ConfigHwAverage(const uint32_t instanceNum, const ADC_AVG_CFG_T * const config);
void ADC_ReadHwAverageConfig(const uint32_t instanceNum, ADC_AVG_CFG_T * const config);
void ADC_ConfigChanStruct(ADC_CHAN_CONFIG_T * const config);
void ADC_ConfigChan(const uint32_t instanceNum, const uint8_t channelIndex, const ADC_CHAN_CONFIG_T * const config);
void ADC_ReadChanConfig(const uint32_t instanceNum, const uint8_t channelIndex, ADC_CHAN_CONFIG_T * const config);
void ADC_SetSwPretrigger(const uint32_t instanceNum, const ADC_SW_PRE_TRIGGER_T swPretrigger);
void ADC_Reset(const uint32_t instanceNum);
void ADC_WaitConvDone(const uint32_t instanceNum);
bool ADC_ReadConvCompleteFlag(const uint32_t instanceNum, const uint8_t channelIndex);
void ADC_ReadChanResult(const uint32_t instanceNum, const uint8_t channelIndex,  uint16_t * const resultVal);
void ADC_AutoCalibration(const uint32_t instanceNum);
void ADC_ConfigUserCalibrationStruct(ADC_CALIBRATION_T * const config);
void ADC_ConfigUserCalibration(const uint32_t instanceNum, const ADC_CALIBRATION_T * const config);
void ADC_ReadUserCalibration(const uint32_t instanceNum, ADC_CALIBRATION_T * const config);
IRQn_Type ADC_ReadInterruptNumber(const uint32_t instanceNum);
void ADC_ClearLatchedTriggers(const uint32_t instanceNum, const ADC_LATCH_CLEAR_T clearMode);
void ADC_ClearTriggerErrors(const uint32_t instanceNum);
uint32_t ADC_ReadTriggerErrorFlags(const uint32_t instanceNum);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
bool ADC_HW_ReadConvActiveFlag(const ADC_T * const adcBaseAddr);
ADC_CLK_DIVISION_T ADC_HW_ReadClockDivide(const ADC_T * const adcBaseAddr);
void ADC_HW_SetClockDivide(ADC_T * const adcBaseAddr, const ADC_CLK_DIVISION_T adcClockDivide);
uint8_t ADC_HW_ReadSampleTime(const ADC_T * const adcBaseAddr);
void ADC_HW_SetSampleTime(ADC_T * const adcBaseAddr, uint8_t adcSampleTime);
ADC_RESOLUTION_RATIO_T ADC_HW_ReadResolution(const ADC_T * const adcBaseAddr);
void ADC_HW_SetResolution(ADC_T * const adcBaseAddr, const ADC_RESOLUTION_RATIO_T resolutionMode);
ADC_INPUT_CLK_T ADC_HW_ReadInputClock(const ADC_T * const adcBaseAddr);
void ADC_HW_SetInputClock(ADC_T * const adcBaseAddr, const ADC_INPUT_CLK_T adcInputClock);
ADC_TRIGGER_T ADC_HW_ReadTriggerMode(const ADC_T * const adcBaseAddr);
void ADC_HW_SetTriggerMode(ADC_T * const adcBaseAddr, const ADC_TRIGGER_T triggerMode);
ADC_PRE_TRIGGER_T ADC_HW_ReadPretriggerSelect(const uint32_t adcInstance);
void ADC_HW_SetPretriggerSelect(const uint32_t adcInstance, const ADC_PRE_TRIGGER_T preTrigger);
ADC_SEL_TRIGGER_T ADC_HW_ReadTriggerSelect(const uint32_t adcInstance);
void ADC_HW_SetTriggerSelect(const uint32_t adcInstance, const ADC_SEL_TRIGGER_T adcTrigger);
bool ADC_HW_ReadDMAEnableFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetDMAEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
ADC_VOLT_REF_T ADC_HW_ReadVoltageReference(const ADC_T * const adcBaseAddr);
void ADC_HW_SetVoltageReference(ADC_T * const adcBaseAddr, const ADC_VOLT_REF_T adcVoltageRef);
bool ADC_HW_ReadContinuousConvFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetContinuousConvFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
void ADC_HW_SetSupplyMonitoringEnableFlag(SIM_T * const simBaseAddr, const bool stateFlag);
bool ADC_HW_ReadHwCompareEnableFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwCompareEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
bool ADC_HW_ReadHwCompareGtEnableFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwCompareGtEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
bool ADC_HW_ReadHwCompareRangeEnableFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwCompareRangeEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
uint16_t ADC_HW_ReadHwCompareComp1Value(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwCompareComp1Value(ADC_T * const adcBaseAddr, const uint16_t value);
uint16_t ADC_HW_ReadHwCompareComp2Value(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwCompareComp2Value(ADC_T * const adcBaseAddr, const uint16_t value);
bool ADC_HW_ReadHwAverageEnableFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwAverageEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
ADC_AVERAGE_T ADC_HW_ReadHwAverageMode(const ADC_T * const adcBaseAddr);
void ADC_HW_SetHwAverageMode(ADC_T * const adcBaseAddr, const ADC_AVERAGE_T hwAverageMode);
bool ADC_HW_ReadCalibrationActiveFlag(const ADC_T * const adcBaseAddr);
void ADC_HW_SetCalibrationActiveFlag(ADC_T * const adcBaseAddr, const bool stateFlag);
uint16_t ADC_HW_ReadUserGainValue(const ADC_T * const adcBaseAddr);
void ADC_HW_SetUserGainValue(ADC_T * const adcBaseAddr, const uint16_t value);
uint16_t ADC_HW_ReadUserOffsetValue(const ADC_T * const adcBaseAddr);
void ADC_HW_SetUserOffsetValue(ADC_T * const adcBaseAddr,const uint16_t value);
bool ADC_HW_ReadChanInterruptEnableFlag(const ADC_T * const adcBaseAddr, const uint8_t channelIndex);
ADC_INPUT_CHANNEL_T ADC_HW_ReadInputChannel(const ADC_T * const adcBaseAddr, const uint8_t channelIndex);
void ADC_HW_SetInputChannel(ADC_T * const adcBaseAddr, const uint8_t channelIndex, const ADC_INPUT_CHANNEL_T inputChannel, const bool stateFlag);
void ADC_HW_ClearLatchTriggers(ADC_T * const adcBaseAddr);
uint32_t ADC_HW_ReadTriggerLatchFlags(const ADC_T * const adcBaseAddr);


/**@} end of group ADC_Functions*/
/**@} end of group ADC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif

#endif /* APM32F445_446_ADC_H */

