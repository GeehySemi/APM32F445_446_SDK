/*!
 * @file        apm32f445_446_adc.c
 *
 * @brief       This file provides all the ADC firmware functions
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



/* Includes */
#include "apm32f445_446_adc.h"
#include "apm32f445_446_clock.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup ADC_Driver ADC Driver
  @{
*/

/** @defgroup ADC_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define ADC_BASE_PTRS    { ADC0, ADC1 }
/* The reset value for CFG2[SMPLTS] */
#define ADC_RESET_SAMPLE_TIME_VALUE (12U)

/**@} end of group ADC_Macros*/

/** @defgroup ADC_Variables Variables
  @{
*/
#define ADC_INSTANCE_VALIDITY(__instance__)    if(__instance__ > ADC_INSTANCE_COUNT){while(1);}

/* Table of base addresses for ADC instances. */
ADC_T * const g_adcBase[ADC_INSTANCE_COUNT] = ADC_BASE_PTRS;

/**@} end of group ADC_Variables*/


/** @defgroup ADC_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Configure the converter using the given configuration structure
 * @details This function configures the ADC with a given configuration set
 *
 * @param   instanceNum : instance number
 * @param   config      : the configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigConverter(const uint32_t instanceNum, const ADC_CONV_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);

    ADC_T * const baseAddr = g_adcBase[instanceNum];
    CLOCK_NAMES_T adcClocks[ADC_INSTANCE_COUNT] = ADC_CLOCKS;
    uint32_t adcFrequency = 0U;
    STATUS_T clkStatus = CLOCK_SYS_ReadFreq(adcClocks[instanceNum], &adcFrequency);
    (void) clkStatus;

    adcFrequency = adcFrequency / (uint32_t)(1UL << ((uint32_t)(config->clockDivision)));
    ADC_HW_SetClockDivide(baseAddr, config->clockDivision);
    ADC_HW_SetSampleTime(baseAddr, config->adcSampleTime);
    ADC_HW_SetResolution(baseAddr, config->resolution);
    ADC_HW_SetInputClock(baseAddr, config->inputClock);
    ADC_HW_SetTriggerMode(baseAddr, config->trigger);
    ADC_HW_SetPretriggerSelect(instanceNum, config->selPretrigger);
    ADC_HW_SetTriggerSelect(instanceNum, config->selTrigger);
    ADC_HW_SetDMAEnableFlag(baseAddr, config->dmaEnable);
    ADC_HW_SetVoltageReference(baseAddr, config->voltRef);
    ADC_HW_SetContinuousConvFlag(baseAddr, config->contConvEnable);

    if(instanceNum == 0U)
    {
        SIM_T * const simBaseAddr = SIM;
        ADC_HW_SetSupplyMonitoringEnableFlag(simBaseAddr, config->supplyMonEnable);
    }
}


/*!
 * @brief   Resets the ADC converter
 * @details This function resets all the internal ADC registers to reset values.
 *
 * @param   instanceNum : instance number
 *
 * @retval  None.
 */
void ADC_Reset(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);

    ADC_T * const adcBaseAddr = g_adcBase[instanceNum];
    uint8_t index = 0U;

    for(index = 0U; index < ADC_CSTS1_COUNT; index++)
    {
        adcBaseAddr->CSTS1[index].reg = ((uint32_t)ADC_INPUT_CHANNEL_DIS) | (((uint32_t)(((uint32_t)(0x00U))<<6U))&0x40U);
    }

    adcBaseAddr->CFG1.reg = (((uint32_t)(((uint32_t)(ADC_INPUT_CLK_1))<<0U))&0x3U) |
                            (((uint32_t)(((uint32_t)(ADC_RESOLUTION_RATIO_8BIT))<<2U))&0xCU) |
                            (((uint32_t)(((uint32_t)(ADC_CLK_DIVISION_1))<<5U))&0x60U);
    adcBaseAddr->CFG2.reg = (((uint32_t)(((uint32_t)(ADC_DEFAULT_SAMPLE_TIME))<<0U))&0xFFU);

    for(index = 0U; index < ADC_COMPVAL_COUNT; index++)
    {
        adcBaseAddr->COMPVAL[index].reg = (((uint32_t)(((uint32_t)(0U))<<0U))&0xFFFFU);
    }

    adcBaseAddr->CSTS2.reg = (((uint32_t)(((uint32_t)(ADC_VOLT_REF_VREF))<<0U))&0x3U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<2U))&0x4U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<3U))&0x8U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<4U))&0x10U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<5U))&0x20U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<6U))&0x40U);

    adcBaseAddr->CSTS3.reg = (((uint32_t)(((uint32_t)(ADC_HW_AVERAGE_4))<<0U))&0x3U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<2U))&0x4U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<3U))&0x8U) |
                             (((uint32_t)(((uint32_t)(0x00U))<<7U))&0x80U);

    adcBaseAddr->UOEC.reg = (((uint32_t)(((uint32_t)(0U))<<0U))&0xFFU);
    adcBaseAddr->UGEC.reg = (((uint32_t)(((uint32_t)(ADC_DEFAULT_USER_GAIN))<<0U))&0x3FFU);

#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    for(index = 0U; index < ADC_ACSTS1_COUNT; index++)
    {
        adcBaseAddr->ACSTS1[index].reg = ((uint32_t)ADC_INPUT_CHANNEL_DIS) | (((uint32_t)(((uint32_t)(0x00U))<<6U))&0x40U);
    }
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */

    ADC_HW_SetPretriggerSelect(instanceNum, ADC_PDU_PRE_TRIGGER);
    ADC_HW_SetTriggerSelect(instanceNum, ADC_PDU_TRIGGER);
    ADC_SetSwPretrigger(instanceNum, ADC_SW_PRE_TRIGGER_DIS);

    /* Reset ADC Supply Monitoring (ADC0 only) */
    if(instanceNum == 0U)
    {
        SIM_T * const simBaseAddr = SIM;
        ADC_HW_SetSupplyMonitoringEnableFlag(simBaseAddr, false);

        (simBaseAddr->CHIPCTRL.reg) &= ~(0x70000U);
    }
}


/*!
 * @brief   Configure the ADC hardware comparison funcion
 * @details Configures the Hardware Compare feature with the given configuration
 *          structure
 *
 * @param   instanceNum : instance number
 * @param   config      : the configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigHwCompare(const uint32_t instanceNum, const ADC_COMP_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T * const baseAddr = g_adcBase[instanceNum];
    ADC_HW_SetHwCompareEnableFlag(baseAddr, config->compEnable);
    ADC_HW_SetHwCompareGtEnableFlag(baseAddr, config->compGreaterThanEnable);
    ADC_HW_SetHwCompareRangeEnableFlag(baseAddr, config->compRangeFuncEnable);
    ADC_HW_SetHwCompareComp1Value(baseAddr, config->compValue1);
    ADC_HW_SetHwCompareComp2Value(baseAddr, config->compValue2);
}


/*!
 * @brief   Configure the ADC hardware Average funcion
 * @details Configures the Hardware Average feature with the given configuration
 *          structure
 *
 * @param   instanceNum : instance number
 * @param   config      : the configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigHwAverage(const uint32_t instanceNum, const ADC_AVG_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T   * const baseAddr = g_adcBase[instanceNum];
    ADC_HW_SetHwAverageEnableFlag(baseAddr, config->hwAvgEnable);
    ADC_HW_SetHwAverageMode(baseAddr, config->hwAverage);
}



/*!
 * @brief   Configures the selected control channel with the given
 *          configuration structure
 *
 * @details When triggered by software,the channel index should be 0.Configuring any
 *          channel during conversion will terminate the conversion
 *
 * @param   instanceNum  : instance number
 * @param   channelIndex : the control channel index
 * @param   config       : the configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigChan(const uint32_t instanceNum, const uint8_t channelIndex, const ADC_CHAN_CONFIG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T * const baseAddr = g_adcBase[instanceNum];
    /* ADC_INPUTCHAN_SUPPLY_ can only be used with ADC 0,
     * If used, the feature must be enabled separately via supplyMonEnable flag in ADC_CONV_CFG_T. */
    ADC_HW_SetInputChannel(baseAddr, channelIndex, config->channel, config->interruptEnable);
}


/*!
 * @brief   This function is used to set software pre triggering
 *
 * @details This function only affects the first four channels
 *
 * @param   instanceNum  : instance number
 * @param   swPretrigger : The software to be enabled is pre-triggered
 *
 * @retval  None.
 */
void ADC_SetSwPretrigger(const uint32_t instanceNum, const ADC_SW_PRE_TRIGGER_T swPretrigger)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    SIM_T * const simBaseAddr = SIM;
    uint32_t medianValue = 0U;
    uint32_t mask[ADC_INSTANCE_COUNT] = {0U};
#if (ADC_INSTANCE_COUNT == 1U)
    mask[0] = 0xEU;
#elif (ADC_INSTANCE_COUNT == 2U)
    mask[0] = 0xEU;
    mask[1] = 0xE00U;
#endif
        medianValue = simBaseAddr->ADCOPT.reg & (~ mask[instanceNum]);
    if(instanceNum == 0U)
    {
        medianValue |= (uint32_t)((swPretrigger << 1U) & 0xEU);
    }
    else if(instanceNum == 1U)
    {
        medianValue |= (uint32_t)((swPretrigger << 9U) & 0xE00U);
    }
    simBaseAddr->ADCOPT.reg = medianValue;
}


/*!
 * @brief   Waiting for conversion to complete
 *
 * @details This function waits for the conversion to complete by reading the flag bit
 *
 * @param   instanceNum  : instance number
 *
 * @retval  None.
 */
void ADC_WaitConvDone(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    while (ADC_HW_ReadConvActiveFlag(baseAddr) == true)
    {
        /* wait for the conversion to complete */
    }
}


/*!
 * @brief   This function is used for automatic calibration and is called before conversion
 *
 * @details  this function will check and reset clock divide based the adc frequency.
 *           an error will be displayed if adc_freq too big
 *           this function will set satisfy clock divide,start calibration.
 *           final this function: restore adc clock divide,hardware average and trigger settings.
 *
 * @param   instanceNum  : instance number
 *
 * @retval  None.
 */
void ADC_AutoCalibration(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T   * const baseAddr = g_adcBase[instanceNum];
    /* set the hardware average to the maximum and set the software trigger*/
    bool hwAverageEna = ADC_HW_ReadHwAverageEnableFlag(baseAddr);
    ADC_AVERAGE_T hwAverage = ADC_HW_ReadHwAverageMode(baseAddr);
    ADC_TRIGGER_T trigger = ADC_HW_ReadTriggerMode(baseAddr);
    uint8_t adcSampleTime = ADC_HW_ReadSampleTime(baseAddr);

    ADC_HW_SetHwAverageMode(baseAddr, ADC_HW_AVERAGE_32);
    ADC_HW_SetHwAverageEnableFlag(baseAddr, true);
    ADC_HW_SetTriggerMode(baseAddr, ADC_SOFTWARE_TRIGGER);
    /* set the sampling time to a reset value */
    ADC_HW_SetSampleTime(baseAddr, ADC_RESET_SAMPLE_TIME_VALUE);
    baseAddr->GCALS.reg=0x00U;
    baseAddr->PSGCAL3.reg =0x00U;
    baseAddr->PSGCAL2.reg =0x00U;
    baseAddr->PSGCAL1.reg =0x00U;
    baseAddr->PSGCAL0.reg =0x00U;
    baseAddr->PSGCALX.reg =0x00U;
    baseAddr->PSGCAL9.reg =0x00U;
    /*Set clock divider */
    CLOCK_NAMES_T adcClocks[ADC_INSTANCE_COUNT] = ADC_CLOCKS;
    uint32_t adcFrequency = 0U;
    ADC_CLK_DIVISION_T adcClkDivideRes = ADC_HW_ReadClockDivide(baseAddr);
    ADC_CLK_DIVISION_T adcClkDivide = ADC_CLK_DIVISION_1;
    STATUS_T clkStatus = CLOCK_SYS_ReadFreq(adcClocks[instanceNum], &adcFrequency);
    (void) clkStatus;

    if ((adcFrequency / (uint32_t)(1UL << ((uint32_t)(adcClkDivideRes)))) <= (ADC_CLOCK_RUNTIME_MAX_FREQ / 2U))
    {
        /* no action */
    }
    else
    {
        if ((adcFrequency / 2U) <= (ADC_CLOCK_RUNTIME_MAX_FREQ / 2U))
        {
            adcClkDivide = ADC_CLK_DIVISION_2;
        }
        else if ((adcFrequency / 4U) <= (ADC_CLOCK_RUNTIME_MAX_FREQ / 2U))
        {
            adcClkDivide = ADC_CLK_DIVISION_4;
        }
        else if ((adcFrequency / 8U) <= (ADC_CLOCK_RUNTIME_MAX_FREQ / 2U))
        {
            adcClkDivide = ADC_CLK_DIVISION_8;
        }

        ADC_HW_SetClockDivide(baseAddr, adcClkDivide);
    }
    /* start calibration */
    ADC_HW_SetCalibrationActiveFlag(baseAddr, true);
    while (ADC_HW_ReadCalibrationActiveFlag(baseAddr))
    {
        /* Wait for calibration */
    }

    /* restore adc clock divide*/
    ADC_HW_SetClockDivide(baseAddr, adcClkDivideRes);
    /* restore hardware average and trigger settings*/
    ADC_HW_SetHwAverageEnableFlag(baseAddr, hwAverageEna);
    ADC_HW_SetHwAverageMode(baseAddr, hwAverage);
    ADC_HW_SetTriggerMode(baseAddr, trigger);
    ADC_HW_SetSampleTime(baseAddr, adcSampleTime);
}


/*!
 * @brief   Configure user calibration function
 *
 * @details Configure user calibration functionality using the given configuration set
 *
 * @param   instanceNum  : instance number
 * @param   config       : the configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigUserCalibration(const uint32_t instanceNum, const ADC_CALIBRATION_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T   * const baseAddr = g_adcBase[instanceNum];
    ADC_HW_SetUserGainValue(baseAddr, config->userGain);
    ADC_HW_SetUserOffsetValue(baseAddr, config->userOffset);
}


/*!
 * @brief   Clear all pending latch triggers
 *
 * @details This function must be called after the ADC hardware trigger source is turned off
 *
 * @param   instanceNum  : instance number
 * @param   clearMode    : The clearing method for the latched triggers
 *
 * @retval  None.
 */
void ADC_ClearLatchedTriggers(const uint32_t instanceNum, const ADC_LATCH_CLEAR_T clearMode)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T* const baseAddr = g_adcBase[instanceNum];
    if (clearMode == ADC_LATCH_CLEAR_FORCE)
    {
        ADC_HW_ClearLatchTriggers(baseAddr);
    }

    while (ADC_HW_ReadTriggerLatchFlags(baseAddr) != 0U)
    {
        /* Trigger waiting to process latch */
    }
}


/*!
 * @brief   Clear all latch trigger error
 *
 * @details This function clears all trigger error flags of the ADC instance.
 *
 * @param   instanceNum  : instance number
 *
 * @retval  None.
 */
void ADC_ClearTriggerErrors(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    ADC_T * const baseAddr = g_adcBase[instanceNum];
    baseAddr->CSTS2.reg   |= 0xF000000U;
}

/*!
 * @brief   initialize the ADC converter configuration structure
 *
 * @details This function initializes the members of the ADC_CONV_CFG_T structure
 *          to the default values in the reference manual, and to ensure safety,
 *          this function is called before calling ADC_ConfigConverter(),
 *          and the user only needs to modify the member values.
 *
 * @param   config  : configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigConverterStruct(ADC_CONV_CFG_T * const config)
{
    config->contConvEnable  = false;
    config->supplyMonEnable = false;
    config->clockDivision   = ADC_CLK_DIVISION_1;
    config->adcSampleTime   = (uint8_t)ADC_DEFAULT_SAMPLE_TIME;
    config->resolution      = ADC_RESOLUTION_RATIO_8BIT;
    config->inputClock      = ADC_INPUT_CLK_1;
    config->trigger         = ADC_SOFTWARE_TRIGGER;
    config->selPretrigger   = ADC_PDU_PRE_TRIGGER;
    config->selTrigger      = ADC_PDU_TRIGGER;
    config->dmaEnable       = false;
    config->voltRef         = ADC_VOLT_REF_VREF;
}


/*!
 * @brief   initialize the ADC hardware comparison configuration structure
 *
 * @details This function initializes the members of the ADC_COMP_CFG_T structure
 *          to the default values in the reference manual, and to ensure safety,
 *          this function is called before calling ADC_ConfigHwCompare(),
 *          and the user only needs to modify the member values.
 *
 * @param   config  : configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigHwCompareStruct(ADC_COMP_CFG_T * const config)
{
    config->compValue1 = 0U;
    config->compValue2 = 0U;
    config->compGreaterThanEnable = false;
    config->compRangeFuncEnable = false;
    config->compEnable = false;
}


/*!
 * @brief   initialize the ADC hardware averaging configuration structure
 *
 * @details This function initializes the members of the ADC_AVG_CFG_T structure
 *          to the default values in the reference manual, and to ensure safety,
 *          this function is called before calling ADC_ConfigHwAverage(),
 *          and the user only needs to modify the member values.
 *
 * @param   config  : configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigHwAverageStruct(ADC_AVG_CFG_T * const config)
{
    config->hwAverage = ADC_HW_AVERAGE_4;
    config->hwAvgEnable = false;
}


/*!
 * @brief   initialize the ADC control channel configuration structure
 *
 * @details This function initializes the members of the ADC_CHAN_CONFIG_T structure
 *          to the default values in the reference manual, and to ensure safety,
 *          this function is called before calling ADC_ConfigChan(),
 *          and the user only needs to modify the member values.
 *
 * @param   config  : configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigChanStruct(ADC_CHAN_CONFIG_T * const config)
{
    config->channel = ADC_INPUT_CHANNEL_DIS;
    config->interruptEnable = false;
}



/*!
 * @brief   initialize the ADC user calibration configuration structure
 *
 * @details This function initializes the members of the ADC_CALIBRATION_T structure
 *          to the default values in the reference manual, and to ensure safety,
 *          this function is called before calling ADC_ConfigUserCalibration(),
 *          and the user only needs to modify the member values.
 *
 * @param   config  : configuration structure
 *
 * @retval  None.
 */
void ADC_ConfigUserCalibrationStruct(ADC_CALIBRATION_T * const config)
{
    config->userOffset = (uint16_t)0U;
    config->userGain = (uint16_t)ADC_DEFAULT_USER_GAIN;
}



/*!
 * @brief   Get the current ADC converter configuration
 *
 * @details This function returns the configuration of the ADC converter as a configuration structure.
 *
 * @param   instanceNum  : instance number
 * @param   config       : configuration structure
 *
 * @retval  None.
 */
void ADC_ReadConverterConfig(const uint32_t instanceNum, ADC_CONV_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);

    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    config->contConvEnable = ADC_HW_ReadContinuousConvFlag(baseAddr);
    config->voltRef = ADC_HW_ReadVoltageReference(baseAddr);
    config->dmaEnable = ADC_HW_ReadDMAEnableFlag(baseAddr);
    config->selPretrigger = ADC_HW_ReadPretriggerSelect(instanceNum);
    config->selTrigger = ADC_HW_ReadTriggerSelect(instanceNum);
    config->trigger = ADC_HW_ReadTriggerMode(baseAddr);
    config->inputClock = ADC_HW_ReadInputClock(baseAddr);
    config->resolution = ADC_HW_ReadResolution(baseAddr);
    config->adcSampleTime = ADC_HW_ReadSampleTime(baseAddr);
    config->clockDivision = ADC_HW_ReadClockDivide(baseAddr);

    /* Power monitoring is only available for ADC 0. */
    if(instanceNum != 0U)
    {
        config->supplyMonEnable = false;
    }
    else
    {
        const SIM_T * const simBaseAddr = SIM;
        config->supplyMonEnable = simBaseAddr->CHIPCTRL.bit.ADCISMEN != 0U ? true : false;
    }
}


/*!
 * @brief   Get the current ADC hardware comparison configuration
 *
 * @details This function returns the configuration of the ADC hardware comparison function.
 *
 * @param   instanceNum  : instance number
 * @param   config       : configuration structure
 *
 * @retval  None.
 */
void ADC_ReadHwCompareConfig(const uint32_t instanceNum, ADC_COMP_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    config->compValue2 = ADC_HW_ReadHwCompareComp2Value(baseAddr);
    config->compValue1 = ADC_HW_ReadHwCompareComp1Value(baseAddr);
    config->compRangeFuncEnable = ADC_HW_ReadHwCompareRangeEnableFlag(baseAddr);
    config->compGreaterThanEnable = ADC_HW_ReadHwCompareGtEnableFlag(baseAddr);
    config->compEnable = ADC_HW_ReadHwCompareEnableFlag(baseAddr);
}



/*!
 * @brief   Get the current ADC hardware average configuration
 *
 * @details This function returns the configuration of the ADC hardware averaging feature.
 *
 * @param   instanceNum  : instance number
 * @param   config       : configuration structure
 *
 * @retval  None.
 */
void ADC_ReadHwAverageConfig(const uint32_t instanceNum, ADC_AVG_CFG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    config->hwAverage = ADC_HW_ReadHwAverageMode(baseAddr);
    config->hwAvgEnable = ADC_HW_ReadHwAverageEnableFlag(baseAddr);
}



/*!
 * @brief   Get the selected current ADC control channel configuration channel index
 *
 * @details This function returns the configuration of the ADC control channel
 *
 * @param   instanceNum   : instance number
 * @param   channelIndex  : controls channel indexing
 * @param   config        : configuration structure
 *
 * @retval  None.
 */
void ADC_ReadChanConfig(const uint32_t instanceNum, const uint8_t channelIndex, ADC_CHAN_CONFIG_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    config->channel = ADC_HW_ReadInputChannel(baseAddr, channelIndex);
    config->interruptEnable = ADC_HW_ReadChanInterruptEnableFlag(baseAddr, channelIndex);
}


/*!
 * @brief   Gets the control channel transition completion flag status
 *
 * @details True flag when conversion is complete or when a condition generated
 *          by the hardware comparison feature is true.
 *
 * @param   instanceNum   : instance number
 * @param   channelIndex  : controls channel indexing
 *
 * @retval  Transition Complete flag status
 *
 */
bool ADC_ReadConvCompleteFlag(const uint32_t instanceNum, const uint8_t channelIndex)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];

#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    uint32_t temp = baseAddr->ACSTS1[channelIndex].reg;
    temp = (temp & 0x80U) >> 7U;
#else
    uint32_t temp = baseAddr->CSTS1[channelIndex].reg;
    temp = (temp & 0x80U) >> 7U;
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */

    return (temp != 0U) ? true : false;

}

/*!
 * @brief   Get the last result for the selected ADC channel
 *
 * @details This function obtains the conversion result from the ADC channel
 *          and stores it in the formal parameter.
 *
 * @param   instanceNum   : instance number
 * @param   channelIndex  : controls channel indexing
 * @param   resultVal     : the result raw value
 *
 * @retval  None.
 */
void ADC_ReadChanResult(const uint32_t instanceNum, const uint8_t channelIndex, uint16_t * const resultVal)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];

#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    uint32_t temp = baseAddr->ADATA[channelIndex].reg;
    temp = (temp & 0xFFFU) >> 0U;
#else
    uint32_t temp = baseAddr->DATA[channelIndex].reg;
    temp = (temp & 0xFFFU) >> 0U;
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */

    *resultVal = (uint16_t)temp;
}


/*!
 * @brief   Get the current user calibration configuration
 *
 * @details This function returns the current user calibration register value.
 *
 * @param   instanceNum   : instance number
 * @param   config        : configuration structure
 *
 * @retval  None.
 */
void ADC_ReadUserCalibration(const uint32_t instanceNum, ADC_CALIBRATION_T * const config)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];
    config->userOffset = ADC_HW_ReadUserOffsetValue(baseAddr);
    config->userGain = ADC_HW_ReadUserGainValue(baseAddr);
}


/*!
 * @brief   Returns the interrupt number of the ADC instance.
 *
 * @details This function returns the interrupt number for the specified ADC instance.
 *
 * @param   instanceNum   : instance number
 *
 * @retval  The number of interrupts of the ADC instance, which is used to configure interrupts
 *
 */
IRQn_Type ADC_ReadInterruptNumber(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    static const IRQn_Type adcIrqIdNumber[ADC_INSTANCE_COUNT] = ADC_IRQS;
    IRQn_Type irqIdNumber = adcIrqIdNumber[instanceNum];

    return irqIdNumber;
}


/*!
 * @brief   get the trigger error flag bit of the ADC instance
 *
 * @details This function returns the bit that triggered the mislabeled ADC instance.
 *
 * @param   instanceNum    : instance number
 *
 * @retval  trigErrorFlags : trigger error flag bitmask
 *
 */
uint32_t ADC_ReadTriggerErrorFlags(const uint32_t instanceNum)
{
    ADC_INSTANCE_VALIDITY(instanceNum);
    const ADC_T * const baseAddr = g_adcBase[instanceNum];

    uint32_t triggerErrors = ((baseAddr->CSTS2.reg) & 0xF000000U) >> 24U;

    return triggerErrors;
}


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   get the conversion Active flag
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  Conversion Active Flag
 *
 */
bool ADC_HW_ReadConvActiveFlag(const ADC_T * const adcBaseAddr)
{
     return (bool)(adcBaseAddr->CSTS2.bit.CFLAG);
}


/*!
 * @brief   Get the current ADC clock divider configuration.
 *
 * @details This function returns the clock divider bit field value configured for the ADC instance.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  clock divider value. Possible values:
 *          - ADC_CLK_DIVISION_1 : The divider is set to 1.
 *          - ADC_CLK_DIVISION_2 : The divider is set to 2.
 *          - ADC_CLK_DIVISION_4 : The divider is set to 4.
 *          - ADC_CLK_DIVISION_8 : The divider is set to 8.
 */
ADC_CLK_DIVISION_T ADC_HW_ReadClockDivide(const ADC_T * const adcBaseAddr)
{
    return (ADC_CLK_DIVISION_T)(adcBaseAddr->CFG1.bit.CLKDIVCFG);
}


/*!
 * @brief   Set the ADC clock divider configuration.
 *
 * @details This feature configures the ADC instance clock divider.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 * @param   clock divider  : - ADC_CLK_DIVISION_1 : The divider is set to 1.
 *                           - ADC_CLK_DIVISION_2 : The divider is set to 2.
 *                           - ADC_CLK_DIVISION_4 : The divider is set to 4.
 *                           - ADC_CLK_DIVISION_8 : The divider is set to 8.
 * @retval  None.
 */
void ADC_HW_SetClockDivide(ADC_T * const adcBaseAddr,
                           const ADC_CLK_DIVISION_T adcClockDivide)
{
    uint32_t temp = adcBaseAddr->CFG1.reg;
    temp &= ~(0x60U);
    temp |= (((uint32_t)(((uint32_t)(adcClockDivide))<<5U))&0x60U);
        adcBaseAddr->CFG1.reg = temp;
}

/*!
 * @brief   Set the ADC clock divider configuration.
 *
 * @details This feature gets the sample time configured for the ADC in AD clocks.
 *          You can choose from 2 to 256 ADCKs. The value returned by this function is
 *          the sample time minus 1. The sampling time is not supported.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  sampling time in AD clock
 */
uint8_t ADC_HW_ReadSampleTime(const ADC_T * const adcBaseAddr)
{
    return (uint8_t)(adcBaseAddr->CFG2.bit.SMPTCFG);
}


/*!
 * @brief   Set the sampling time in AD clock cycles
 *
 * @details This function configures the sampling time of the ADC in ADCK clocks.
 *          The actual sampling time is the value provided plus 1.
 *          You can choose from 2 to 256 ADCKs. The actual sample time of 1 is not supported
 *          (parameter value 0 is automatically changed to 1).
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  None.
 */
void ADC_HW_SetSampleTime(ADC_T * const adcBaseAddr,uint8_t adcSampleTime)
{
    /* Clip the sampling time to the minimum value */
    uint8_t rSampleTime = (uint8_t)((adcSampleTime > 0U) ? adcSampleTime : 1U);
    uint32_t temp = adcBaseAddr->CFG2.reg;
    temp &= ~(0xFFU);
    temp |= (((uint32_t)(((uint32_t)(rSampleTime))<<0U))&0xFFU);
    adcBaseAddr->CFG2.reg = temp;
}



/*!
 * @brief   get the resolution mode configuration
 *
 * @details This function returns the resolution mode configured for the ADC.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  ADC resolution mode. Possible values:
 *          - ADC_RESOLUTION_RATIO_8BIT  : 8-bit resolution mode.
 *          - ADC_RESOLUTION_RATIO_10BIT : 10-bit resolution mode.
 *          - ADC_RESOLUTION_RATIO_12BIT : 12-bit resolution mode.
 */
ADC_RESOLUTION_RATIO_T ADC_HW_ReadResolution(const ADC_T * const adcBaseAddr)
{
    ADC_RESOLUTION_RATIO_T returnValue;
    if(adcBaseAddr->CFG1.bit.CMODSEL == ADC_CFG1_CMODSEL_00)
    {
        returnValue = ADC_RESOLUTION_RATIO_8BIT;
    }
    else if(adcBaseAddr->CFG1.bit.CMODSEL == ADC_CFG1_CMODSEL_01)
    {
        returnValue = ADC_RESOLUTION_RATIO_12BIT;
    }
    else if(adcBaseAddr->CFG1.bit.CMODSEL == ADC_CFG1_CMODSEL_10)
    {
        returnValue = ADC_RESOLUTION_RATIO_10BIT;
    }
    else
        returnValue = ADC_RESOLUTION_RATIO_8BIT;

    return returnValue;
}


/*!
 * @brief   set resolution mode configuration
 *
 * @details This feature configures the ADC resolution mode.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 * @param   resolutionMode : ADC resolution mode
 *
 * @retval  None.
 */
void ADC_HW_SetResolution(ADC_T * const adcBaseAddr,const ADC_RESOLUTION_RATIO_T resolutionMode)
{
    uint32_t temp = adcBaseAddr->CFG1.reg;
    temp &= ~(0xCU);
    temp |= (((uint32_t)(((uint32_t)(resolutionMode))<<2U))&0xCU);
    adcBaseAddr->CFG1.reg = temp;
}


/*!
 * @brief   Get AD clock input configuration
 *
 * @details This function returns the clock input source configured for the ADC.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  input clock source. Possible values:
 *          - ADC_INPUT_CLK_1 : ADC Input Clock Source Alternative 1.
 *          - ADC_INPUT_CLK_2 : ADC Input Clock Source Alternative 2.
 *          - ADC_INPUT_CLK_3 : ADC Input Clock Source Alternative 3.
 *          - ADC_INPUT_CLK_4 : ADC Input Clock Source Alternative 4.
 */
ADC_INPUT_CLK_T ADC_HW_ReadInputClock(const ADC_T * const adcBaseAddr)
{
    return (ADC_INPUT_CLK_T)(adcBaseAddr->CFG1.bit.ICLKSSEL);
}


/*!
 * @brief   set the AD clock input configuration
 *
 * @details This function configures the ADC's clock input source.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 * @param   adcInputClock  : input clock new input clock source.
 *
 * @retval  None.
 */
void ADC_HW_SetInputClock(ADC_T * const adcBaseAddr,const ADC_INPUT_CLK_T adcInputClock)
{
    uint32_t temp = adcBaseAddr->CFG1.reg;
    temp &= ~(0x3U);
    temp |= (((uint32_t)(((uint32_t)(adcInputClock))<<0U))&0x3U);
    adcBaseAddr->CFG1.reg = temp;
}



/*!
 * @brief   read ADC trigger mode
 *
 * @details This function returns the trigger mode configured for the ADC. In software-triggered mode,
 *          the user can start the conversion by setting the input channel in ADC measurement channel A (index 0).
 *          In hardware-triggered mode, the conversion is initiated by another peripheral device, such as a PDU or TMC.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 *
 * @retval  current trigger mode. Possible values:
 *          - ADC_SOFTWARE_TRIGGER : Software triggered.
 *          - ADC_HARDWARE_TRIGGER : Hardware triggered.
 */
ADC_TRIGGER_T ADC_HW_ReadTriggerMode(const ADC_T * const adcBaseAddr)
{
    return (ADC_TRIGGER_T)(adcBaseAddr->CSTS2.bit.CTRGSEL);
}


/*!
 * @brief   Sets ADC trigger mode
 *
 * @details This feature configures the ADC trigger mode. In software-triggered mode,
 *          the user can start the conversion by setting the input channel in ADC
 *          measurement channel A (index 0). In hardware-triggered mode, the conversion is
 *          initiated by another peripheral device, such as a PDU or TMC.
 *
 * @param   adcBaseAddr    : base address of the current ADC instance.
 * @param   triggerMode    : the desired trigger mode
 *
 * @retval  None.
 */
void ADC_HW_SetTriggerMode(ADC_T * const adcBaseAddr,const ADC_TRIGGER_T triggerMode)
{
    uint32_t temp = adcBaseAddr->CSTS2.reg;
    temp &= ~(0x40U);
    temp |= (((uint32_t)(((uint32_t)(triggerMode))<<6U))&0x40U);
    adcBaseAddr->CSTS2.reg = temp;
}


/*!
 * @brief   Get the pretrigger source configured for the ADC instance
 *
 * @details This function captures the pretrigger source selected from the
 *          ADC trigger latch and arbitration unit, affecting control channels 0-3.
 *
 * @param   adcInstance    : ADC instance
 *
 * @retval  pretrigger source selected. Possible values:
 *          - ADC_PDU_PRE_TRIGGER - PDU pretrigger
 *          - ADC_TMC_PRE_TRIGGER - TMC pre-trigger
 *          - ADC_SW_PRE_TRIGGER - Software pre-trigger
 */
ADC_PRE_TRIGGER_T ADC_HW_ReadPretriggerSelect(const uint32_t adcInstance)
{
    const SIM_T * const simBaseAddr = SIM;
    uint32_t currentVal = 0U;
    ADC_PRE_TRIGGER_T returnVal = ADC_PDU_PRE_TRIGGER;
    if(adcInstance == 0U)
    {
        currentVal = (simBaseAddr->ADCOPT.bit.ADC0PTSSEL);
    }
    else if(adcInstance == 1U)
    {
        currentVal = (simBaseAddr->ADCOPT.bit.ADC1PTSSEL);
    }
    if(currentVal == ADC_PDU_PRE_TRIGGER)
    {
        returnVal = ADC_PDU_PRE_TRIGGER;
    }
    else if(currentVal == ADC_TMC_PRE_TRIGGER)
    {
        returnVal = ADC_TMC_PRE_TRIGGER;
    }
    else if(currentVal == ADC_SW_PRE_TRIGGER)
    {
        returnVal = ADC_SW_PRE_TRIGGER;
    }
    return returnVal;
}


/*!
 * @brief   Set the pretrigger selection for the ADC instance
 *
 * @details This function sets the pre-trigger source selected from the ADC trigger
            latch and arbitration unit, affecting control channels 0-3.
 *
 * @param   adcInstance    : ADC instance
 * @param   preTrigger     : the pretrigger source that will be selected
 *                           - ADC_PDU_PRE_TRIGGER  - PDU pretrigger
 *                           - ADC_TMC_PRE_TRIGGER  - TMC pretrigger
 *                           - ADC_SW_PRE_TRIGGER   - Software pretrigger
 * @retval  None.
 */
void ADC_HW_SetPretriggerSelect(const uint32_t adcInstance,const ADC_PRE_TRIGGER_T preTrigger)
{
    SIM_T * const simBaseAddr = SIM;
    if(adcInstance ==0U)
    {
        simBaseAddr->ADCOPT.bit.ADC0PTSSEL = preTrigger;
    }
    else if (adcInstance ==1U)
    {
        simBaseAddr->ADCOPT.bit.ADC1PTSSEL = preTrigger;
    }
}


/*!
 * @brief   Get the trigger source configured for the ADC instance
 *
 * @details This function captures the trigger source selected from
            the ADC trigger latch and arbitration unit.
 *
 * @param   adcInstance    : ADC instance
 *
 * @retval  trigger        : source for triggering selection
 *                           - ADC_PDU_TRIGGER - PDU trigger selected
 *                           - ADC_TMC_TRIGGER - TMC trigger
 */
ADC_SEL_TRIGGER_T ADC_HW_ReadTriggerSelect(const uint32_t adcInstance)
{
    const SIM_T * const simBaseAddr  = SIM;
    uint32_t currentValue  = 0xffU;
    ADC_SEL_TRIGGER_T returnValue = ADC_PDU_TRIGGER;
    if(adcInstance == 0U)
    {
      currentValue = simBaseAddr->ADCOPT.bit.ADC0TRGSSEL;
    }
    else if(adcInstance == 1U)
    {
      currentValue = simBaseAddr->ADCOPT.bit.ADC1TRGSSEL;
    }
    if(currentValue == SIM_ADCOPT_ADC0PTSSEL_00)
    {
      returnValue = ADC_PDU_TRIGGER;
    }
    else if (currentValue == SIM_ADCOPT_ADC0PTSSEL_01)
    {
      returnValue = ADC_TMC_TRIGGER;
    }

    return returnValue;
}


/*!
 * @brief   Set the trigger selection for the ADC instance
 *
 * @details This function sets the trigger source selected from the ADC trigger latch and arbitration unit.
 *
 * @param   adcInstance    : ADC instance
 * @param   adcTrigger     : the pretrigger source that will be selected
 *                           - ADC_PDU_TRIGGER - PDU trigger selected
 *                           - ADC_TMC_TRIGGER - TMC trigger
 * @retval  None.
 */
void ADC_HW_SetTriggerSelect(const uint32_t adcInstance, const ADC_SEL_TRIGGER_T adcTrigger)
{
    SIM_T * const simBaseAddr = SIM;
    uint32_t mask[ADC_INSTANCE_COUNT] = {0U};
#if (ADC_INSTANCE_COUNT == 1U)
    mask[0] = SIM_ADCOPT_ADC0TRGSEL_MASK;
#elif (ADC_INSTANCE_COUNT == 2U)
    mask[0] = 0x1U;
    mask[1] = 0x100U;
#endif
    uint32_t intermValue = 0U;
    intermValue = simBaseAddr->ADCOPT.reg & (~ mask[adcInstance]);

    if(adcInstance == 0U)
    {
        intermValue |= (uint32_t)((adcTrigger << 0U) & 0x1U);
    }
    else if(adcInstance == 1U)
    {
        intermValue |= (uint32_t)((adcTrigger << 8U) & 0x100U);
    }
    simBaseAddr->ADCOPT.reg = intermValue;
}

/*!
 * @brief   get the DMA enable flag status
 *
 * @details This function returns the status of the DMA enable flag. DMA can be used to transfer
 *          converted values from registers to RAM without CPU intervention.
 *
 * @param   adcBaseAddr    : ADC base pointer
 *
 * @retval  DMA enable flag status
 */
bool ADC_HW_ReadDMAEnableFlag(const ADC_T * const adcBaseAddr)
{
    return (adcBaseAddr->CSTS2.bit.DMAEN);
}


/*!
 * @brief   set DMA enable flag status
 *
 * @details This function returns the status of the DMA enable flag. DMA can be used to transfer
 *          converted values from registers to RAM without CPU intervention.
 *
 * @param   adcBaseAddr    : ADC base pointer
 * @param   stateFlag      : declares a new DMA enable flag status
 *
 * @retval  None.
 */
void ADC_HW_SetDMAEnableFlag(ADC_T * const adcBaseAddr,const bool stateFlag)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS2.reg;
    temp &= (uint32_t)(~(0x4U));
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<2U))&0x4U);
    adcBaseAddr->CSTS2.reg = (uint32_t)temp;
}


/*!
 * @brief   get the DMA enable flag status
 *
 * @details This function returns the reference voltage selection configured for the ADC.
 *          The reference voltage can be selected between pairs (VrefH, VrefL) and (ValtH, ValtL).
 *
 * @param   adcBaseAddr    : ADC base pointer
 *
 * @retval  reference input pair. Possible values:
 *          - ADC_VOLT_REF_VREF : VrefL and VrefH are used as voltage references.
 *          - ADC_VOLT_REF_VALT : ValtL and ValtH are used as voltage references.
 */
 ADC_VOLT_REF_T ADC_HW_ReadVoltageReference(const ADC_T * const adcBaseAddr)
{
    return (ADC_VOLT_REF_T)(adcBaseAddr->CSTS2.bit.VREFSEL);
}



/*!
 * @brief   set ADC reference voltage selection
 *
 * @details This function returns the status of the DMA enable flag. DMA can be used to transfer
 *          converted values from registers to RAM without CPU intervention.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   adcVoltageRef      : voltage reference to the new voltage reference input
 *
 * @retval  None.
 */
void ADC_HW_SetVoltageReference(ADC_T * const adcBaseAddr, const ADC_VOLT_REF_T adcVoltageRef)
{
    uint32_t temp = adcBaseAddr->CSTS2.reg;
    temp &= ~(0x3U);
    temp |= (((uint32_t)(((uint32_t)(adcVoltageRef))<<0U))&0x3U);
    adcBaseAddr->CSTS2.reg = temp;
}



/*!
 * @brief    get the continuous transition flag status
 *
 * @details  This function returns the status of the continuous transition flag.
 *           This feature can be used to continuously sample a single channel. When this feature
 *           is active, the channel cannot be changed until this feature is turned off.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   Continuous transition flag status
 */
bool ADC_HW_ReadContinuousConvFlag(const ADC_T * const adcBaseAddr)
{
    return (bool)(adcBaseAddr->CSTS3.bit.CONTCEN);
}


/*!
 * @brief   sets the continuous transition flag state
 *
 * @details This feature configures continuous conversion. This feature can be used to continuously
 *          sample a single channel. When this feature is active, the channel cannot be changed until
 *          this feature is turned off.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   stateFlag          : declares a new continuous transition flag state
 *
 * @retval  None.
 */
void ADC_HW_SetContinuousConvFlag(ADC_T * const adcBaseAddr,const bool stateFlag)
{
   uint32_t temp = (uint32_t)adcBaseAddr->CSTS3.reg;
    temp &= ~(0x8U);
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<3U))&0x8U);
    adcBaseAddr->CSTS3.reg = (uint32_t)temp;

}



/*!
 * @brief   set the power monitor enable flag state
 *
 * @details This feature configures the power monitor enable flag. Power monitoring is
 *          only available for ADC 0, and you can use the ADC_INPUTCHAN_SUPPLY_ enumeration
 *          entries to select the actual power supply to monitor.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   stateFlag          : New power monitor enables flag status
 *
 * @retval  None.
 */
void ADC_HW_SetSupplyMonitoringEnableFlag(SIM_T * const simBaseAddr, const bool stateFlag)
{
    if(stateFlag == true)
    {
        simBaseAddr->CHIPCTRL.reg |= (0x80000U);
    }
    else
    {
        simBaseAddr->CHIPCTRL.reg &= ~(0x80000U);
    }

}


/*!
 * @brief    get the continuous transition flag status
 *
 * @details  This function returns the status of the continuous transition flag.
 *           This feature can be used to continuously sample a single channel. When this feature
 *           is active, the channel cannot be changed until this feature is turned off.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   hardware comparison enable flag status
 */
bool ADC_HW_ReadHwCompareEnableFlag(const ADC_T * const adcBaseAddr)
{
    return (bool)(adcBaseAddr->CSTS2.bit.COMPEN);
}


/*!
 * @brief   set hardware comparison enable flag status
 *
 * @details This feature configures the hardware comparison enable flag.
 *          Hardware comparisons can be used to check whether ADC conversion
 *          results are within or outside the predefined range.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   stateFlag          : new hardware comparison enables flag status
 *
 * @retval  None.
 */
void ADC_HW_SetHwCompareEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS2.reg;
    temp &= (uint32_t)(~(0x20U));
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<5U))&0x20U);
    adcBaseAddr->CSTS2.reg = (uint32_t)temp;
}



/*!
 * @brief    get a hardware comparison that is greater than the enabled flag state
 *
 * @details  This function returns the "Hardware comparison is greater than enabled" flag.
 *           Using this feature, the ADC can be configured to check if the measured value
 *           is within a predefined range.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   hardware comparison is greater than the enable flag state
 */
bool ADC_HW_ReadHwCompareGtEnableFlag(const ADC_T * const adcBaseAddr)
{
    return (bool)(adcBaseAddr->CSTS2.bit.GTCOMPEN);
}



/*!
 * @brief   set hardware comparison greater than enable flag state
 *
 * @details This feature configures hardware comparison larger than the enable flag.
 *          Using this feature, the ADC can be configured to check if the measured value
 *          is within a predefined range.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   stateFlag          : new hardware comparison is greater than the enable flag state
 *
 * @retval  None.
 */
void ADC_HW_SetHwCompareGtEnableFlag(ADC_T * const adcBaseAddr,const bool stateFlag)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS2.reg;
    temp &= (uint32_t)(~(0x10U));
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<4U))&0x10U);
    adcBaseAddr->CSTS2.reg = (uint32_t)temp;
}


/*!
 * @brief    Read hardware comparison range enabled
 *
 * @details  This function returns the status of the hardware comparison range enable flag.
 *           This feature allows you to configure a range with two nonzero values or with
 *           nonzero and zero values.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   hardware comparison range Enable flag status
 */
bool ADC_HW_ReadHwCompareRangeEnableFlag(const ADC_T * const adcBaseAddr)
{
    return (bool)(adcBaseAddr->CSTS2.bit.RCOMPEN);
}


/*!
 * @brief   Set hardware comparison range enabled
 *
 * @details This feature configures the hardware comparison range enable flag.
 *          This feature allows you to configure a range with two nonzero values
 *          or with nonzero and zero values.
 *
 * @param   adcBaseAddr        : ADC base pointer
 * @param   stateFlag          : new hardware comparison range enables flag status
 *
 * @retval  None.
 */
void ADC_HW_SetHwCompareRangeEnableFlag(ADC_T * const adcBaseAddr,const bool stateFlag)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS2.reg;
    temp &= (uint32_t)(~(0x8U));
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<3U))&0x8U);
    adcBaseAddr->CSTS2.reg = (uint32_t)temp;

}



/*!
 * @brief    Get the comparison register 1 value
 *
 * @details  This function returns a value written to hardware comparison register 1.
             This value defines the upper or lower limit of the hardware comparison range. This value is always
             a 12-bit resolution value.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   Compare Register 1 value
 */
uint16_t ADC_HW_ReadHwCompareComp1Value(const ADC_T * const adcBaseAddr)
{
   return (uint16_t)(adcBaseAddr->COMPVAL[0U].reg);
}


/*!
 * @brief   sets the comparison register 1 value
 *
 * @details This function writes a 12-bit value in hardware comparison register 1. This value defines the upper
 *          or lower limit of the hardware comparison range. This value is always 12-bit resolution.
 *
 * @param   adcBaseAddr : ADC base pointer
 * @param   value       : of the new comparison register 1 value
 *
 * @retval  None.
 */
void ADC_HW_SetHwCompareComp1Value(ADC_T * const adcBaseAddr, const uint16_t value)
{
    adcBaseAddr->COMPVAL[0U].reg  = (((uint32_t)(((uint32_t)(value))<<0U))&0xFFFFU);
}

/*!
 * @brief    Get the comparison register 2 value
 *
 * @details  This function returns a value written to hardware comparison register 2.
 *           This value defines the upper or lower limit of the hardware comparison range.
 *           This value is always 12-bit resolution.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   compare register 2 value
 */
uint16_t ADC_HW_ReadHwCompareComp2Value(const ADC_T * const adcBaseAddr)
{
   return (uint16_t)adcBaseAddr->COMPVAL[1U].reg;
}


/*!
 * @brief   sets the comparison register 2 value
 *
 * @details This function writes a 12-bit value in hardware comparison register 2. This value defines the upper
 *          or lower limit of the hardware comparison range. This value is always 12-bit resolution
 *
 * @param   adcBaseAddr : ADC base pointer
 * @param   value       : of the new comparison register 2 value
 *
 * @retval  None.
 */
void ADC_HW_SetHwCompareComp2Value(ADC_T * const adcBaseAddr, const uint16_t value)
{

    adcBaseAddr->COMPVAL[1U].reg = (((uint32_t)(((uint32_t)(value))<<0U))&0xFFFFU);

}


/*!
 * @brief    get the hardware average enable flag status
 *
 * @details  This function returns the status of the hardware average enable flag.
 *           Hardware averaging can be used to obtain the average of multiple consecutive
 *           conversions on the same channel.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   hardware average enable flag status
 */
bool ADC_HW_ReadHwAverageEnableFlag(const ADC_T * const adcBaseAddr)
{

    return adcBaseAddr->CSTS3.bit.HAVGEN;
}



/*!
 * @brief   sets the hardware average enable flag status
 *
 * @details This feature configures the hardware average enable flag. Hardware averaging can be used to
            obtain the average of multiple consecutive conversions on the same channel.
 *
 * @param   adcBaseAddr : ADC base pointer
 * @param   stateFlag   : New hardware average enable flag status
 *
 * @retval  None.
 */
void ADC_HW_SetHwAverageEnableFlag(ADC_T * const adcBaseAddr, const bool stateFlag)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS3.reg;
    temp &= (uint32_t)(~(0x4U));
    temp |= (((uint32_t)(((uint32_t)((stateFlag ? (uint32_t)1U : (uint32_t)0U)))<<2u))&0x4u);
    adcBaseAddr->CSTS3.reg = (uint32_t)temp;
}



/*!
 * @brief    get hardware averaging mode
 *
 * @details  This function returns the configured hardware averaging mode.
 *           The mode selects the number of samples to average: 4, 8, 16, or 32.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   hardware averaging mode selection. Possible values:
 *           - ADC_HW_AVERAGE_4 : Hardware average of 4 samples.
 *           - ADC_HW_AVERAGE_8 : Hardware average of 8 samples.
 *           - ADC_HW_AVERAGE_16 : Hardware average of 16 samples.
 *           - ADC_HW_AVERAGE_32 : Hardware average of 32 samples.
 */
ADC_AVERAGE_T ADC_HW_ReadHwAverageMode(const ADC_T * const adcBaseAddr)
{
    return (ADC_AVERAGE_T)(adcBaseAddr->CSTS3.bit.HAVGCFG);
}


/*!
 * @brief   set hardware averaging mode
 *
 * @details This feature configures hardware averaging mode.
 *          The mode selects the number of samples to average: 4, 8, 16, or 32.
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   hwAverageMode : new hardware averaging mode.
 *
 * @retval  None.
 */
void ADC_HW_SetHwAverageMode(ADC_T * const adcBaseAddr, const ADC_AVERAGE_T hwAverageMode)
{
    uint32_t temp = adcBaseAddr->CSTS3.reg;
    /* Clear the affected bitfield */
    temp &= ~(0x3U);
    temp |= (((uint32_t)(((uint32_t)(hwAverageMode))<<0U))&0x3U);
    adcBaseAddr->CSTS3.reg = temp;
}


/*!
 * @brief    get calibration activity flag status
 *
 * @details  This function returns the status of the calibration activity flag.
 *           Set this flag if an auto-calibration sequence is being performed.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   calibration activity flag status
 */
bool ADC_HW_ReadCalibrationActiveFlag(const ADC_T * const adcBaseAddr)
{
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS3.reg;
    temp = (temp & 0x80U) >> 7U;

    return (temp != 0U) ? true : false;

}



/*!
 * @brief   Set calibration activity flag status
 *
 * @details This function starts or aborts the auto-calibration sequence.
 *          If this setting is set, it remains set until the sequence is complete.
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   stateFlag     : New calibration activity flag status
 *
 * @retval  None.
 */
void ADC_HW_SetCalibrationActiveFlag(ADC_T * const adcBaseAddr, const bool stateFlag)
{
    uint32_t temp = adcBaseAddr->CSTS3.reg;
    temp &= ~(0x80U);
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<7U))&0x80U);
    adcBaseAddr->CSTS3.reg = temp;
}




/*!
 * @brief    Get the user gain register value
 *
 * @details  This function returns the value in the user gain register.
 *           The value in this register is an amplification that is applied
 *           to the measurement data before writing to the result register.
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   user gain register value
 */
uint16_t ADC_HW_ReadUserGainValue(const ADC_T * const adcBaseAddr)
{
    return (uint16_t)(adcBaseAddr->UGEC.bit.UGECVAL);
}



/*!
 * @brief   set the user gain register value
 *
 * @details This function configures the user gain register. The value in this register
 *          is an amplification that is applied to the measurement data before writing to the result register.
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   value         : the new user gain register value
 *
 * @retval  None.
 */
void ADC_HW_SetUserGainValue(ADC_T * const adcBaseAddr,const uint16_t value)
{
    uint16_t calvalS = (uint16_t)adcBaseAddr->GCALS.bit.CALVAL;
    uint16_t calval3 = (uint16_t)adcBaseAddr->PSGCAL3.bit.CALVAL;
    uint16_t calval2 = (uint16_t)adcBaseAddr->PSGCAL2.bit.CALVAL;
    uint16_t calval1 = (uint16_t)adcBaseAddr->PSGCAL1.bit.CALVAL;
    uint16_t calval0 = (uint16_t)adcBaseAddr->PSGCAL0.bit.CALVAL;

    /* Add calval0, calval1, calval2, calval3 and calvalS */
    uint16_t dataSum = (uint16_t)(value + calval0 + calval1 + calval2 + calval3 + calvalS);

    uint16_t temp = (uint16_t)(dataSum & 0xF800U);
    if (temp != 0x0000U)
    {
        temp = 0xFFFFU;
    }
    adcBaseAddr->UGEC.reg = (uint32_t)value;
    adcBaseAddr->GEAF.reg = (uint32_t)temp;
}


/*!
 * @brief    read user offset register value
 *
 * @param    adcBaseAddr    : ADC base pointer
 *
 * @retval   user offset register value
 */
uint16_t ADC_HW_ReadUserOffsetValue(const ADC_T * const adcBaseAddr)
{
    return (uint16_t)(adcBaseAddr->UOEC.bit.UOECVAL);
}



/*!
 * @brief   Set the user offset register value
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   value         : the new user offset register value
 *
 * @retval  None.
 */
void ADC_HW_SetUserOffsetValue(ADC_T * const adcBaseAddr, const uint16_t value)
{
    adcBaseAddr->UOEC.reg = (((uint32_t)(((uint32_t)(value))<<0U))&0xFFU);

}


/*!
 * @brief    get the channel interrupt enabled status
 *
 * @details  This function returns the status of the channel interrupt enable flag.
 *           If this flag is set, an interrupt is generated when the transition of the channel is complete.
 *
 * @param    adcBaseAddr     : ADC base pointer
 * @param    channelIndex    : ADC measurement channel index
 *
 * @retval   channel interrupt Enable flag status
 */
bool ADC_HW_ReadChanInterruptEnableFlag(const ADC_T * const adcBaseAddr, const uint8_t channelIndex)
{
#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    uint32_t temp = (uint32_t)adcBaseAddr->ACSTS1[channelIndex].reg;
    temp = (temp & 0x40U) >> 6U;
#else
    uint32_t temp = (uint32_t)adcBaseAddr->CSTS1[channelIndex].reg;
    temp = (temp & 0x40U) >> 6U;
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */

    return (temp != 0U) ? true : false;
}


/*!
 * @brief   Get the configuration input channel of the selected measurement channel
 *
 * @details This function returns the configuration input channel for the measurement channel.
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   value         : the new user gain register value
 *
 * @retval the input channel selected for the measurement channel.
 */
ADC_INPUT_CHANNEL_T ADC_HW_ReadInputChannel(const ADC_T * const adcBaseAddr, const uint8_t channelIndex)
{
#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    uint32_t temp = (uint32_t) (adcBaseAddr->ACSTS1[channelIndex].reg);
    temp = (temp & ADC_INPUT_CHANNEL_DIS) >> 0U;
#else
    uint32_t temp = (uint32_t) (adcBaseAddr->CSTS1[channelIndex].reg);
    temp = (temp & ADC_INPUT_CHANNEL_DIS) >> 0U;
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */
    uint32_t supplyen = (uint32_t)SIM->CHIPCTRL.reg;
    supplyen = (supplyen & 0x80000U) >> 19U;
    if((temp == (uint32_t)ADC_INPUT_CHANNEL_INT0) && (supplyen != 0UL))
    {
        temp = (uint32_t)SIM->CHIPCTRL.reg;
        temp = (temp & 0x70000U) >> 16U;
        temp = temp + (uint32_t)ADC_INPUT_CHANNEL_SUPPLY_VDD;
    }
    return (ADC_INPUT_CHANNEL_T)(temp);

}


/*!
 * @brief   Set the configuration of the input channel and channel interrupt enabled as the measurement channel.
 *
 * @details This function configures the interrupt of the measurement channel, the input channel and status of the enable flag.
 *          In software-triggered mode, configuration channel A (index 0) immediately starts a new conversion. If this flag is set,
 *          an interrupt is generated when the transition of the channel is complete.
 *
 * @param   adcBaseAddr   : ADC base pointer
 * @param   channelIndex  : ADC measurement channel index
 * @param   inputChannel  : is the input channel selected for the measurement channel
 * @param   stateFlag     : New channel interrupt enable flag state
 *
 * @retval  None.
 */
void ADC_HW_SetInputChannel(ADC_T * const adcBaseAddr, const uint8_t channelIndex, const ADC_INPUT_CHANNEL_T inputChannel, const bool stateFlag)
{
    ADC_INPUT_CHANNEL_T inputChanUnmap;

    /* The internal supply monitoring channel requires special configuration */
    if((inputChannel >= ADC_INPUT_CHANNEL_SUPPLY_VDD) && (inputChannel <= ADC_INPUT_CHANNEL_SUPPLY_VDD_LV))
    {
        const uint32_t supplyMonitorIndex = (uint32_t)inputChannel - (uint32_t)ADC_INPUT_CHANNEL_SUPPLY_VDD;

        SIM_T * const simBaseAddr = SIM;
        simBaseAddr->CHIPCTRL.reg &= ~(0x70000U);
        simBaseAddr->CHIPCTRL.reg |= (uint32_t)((supplyMonitorIndex << 16U) & 0x70000U);

        inputChanUnmap = ADC_INPUT_CHANNEL_INT0;
    }
    else
    {
        inputChanUnmap = inputChannel;
    }

#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
{
    uint32_t temp = adcBaseAddr->ACSTS1[channelIndex].reg;
    temp &= ~(ADC_INPUT_CHANNEL_DIS);
    temp |= (((uint32_t)(((uint32_t)(inputChanUnmap))<<0U))&ADC_INPUT_CHANNEL_DIS);
    temp &= ~(0x40U);
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<6U))&0x40U);
    adcBaseAddr->ACSTS1[channelIndex].reg = temp;
}
#else
{
    uint32_t temp = adcBaseAddr->CSTS1[channelIndex].reg;
    temp &= ~(ADC_INPUT_CHANNEL_DIS);
    temp |= (((uint32_t)(((uint32_t)(inputChanUnmap))<<0U))&ADC_INPUT_CHANNEL_DIS);
    temp &= ~(0x40U);
    temp |= (((uint32_t)(((uint32_t)(stateFlag ? (uint32_t)1U : (uint32_t)0U))<<6U))&0x40U);
    adcBaseAddr->CSTS1[channelIndex].reg = temp;
}
#endif /* FEATURE_ADC_WITH_EXTRA_NUM_REGS */

}


/*!
 * @brief   Clear locked trigger
 *
 * @details This feature clears locked triggers except for triggers that are being processed.
 *          Before calling this function, ensure that the hardware trigger source for the ADC is disabled.
 *
 * @param   adcBaseAddr   : ADC base pointer
 *
 * @retval  None.
 */
void ADC_HW_ClearLatchTriggers(ADC_T * const adcBaseAddr)
{
    (adcBaseAddr->CFG1.reg) |= (((uint32_t)(((uint32_t)(0x01U))<<8U))&0x100U);
}


/*!
 * @brief   Gets the trigger latch flag bit.
 *
 * @details This function returns the trigger latch flag.
 *
 * @param   adcBaseAddr   : ADC base pointer
 *
 * @retval  Trigger latch flag
 */
 uint32_t ADC_HW_ReadTriggerLatchFlags(const ADC_T * const adcBaseAddr)
{
    uint32_t temp = adcBaseAddr->CSTS2.reg;
    temp = (temp & 0xF0000U) >> 16U;
    return temp;
}


/**@} end of group ADC_Functions*/
/**@} end of group ADC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
