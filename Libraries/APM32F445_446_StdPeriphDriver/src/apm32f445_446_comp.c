/*!
 * @file        apm32f445_446_cmp.c
 *
 * @brief       This file provides all the COMP firmware functions
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
#include "apm32f445_446_comp.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup COMP_Driver COMP Driver
  @{
*/

/** @defgroup COMP_Variables Variables
  @{
*/

/* Table of base addresses for COMP instances. */
COMP_T * const g_compBase[] = COMP_BASE_PTRS;

/**@} end of group COMP_Variables*/

/** @defgroup COMP_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configure all comparator features with the given configuration structure
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *         STATUS_ERROR: Error occurred
 */
STATUS_T COMP_Init(const uint32_t ins, const COMP_MODULE_T* const cfg)
{
    COMP_T* base = g_compBase[ins];

    COMP_HW_SetFuncMode(base,
                        (cfg->comparator).mode,
                        (cfg->comparator).filtSmpCnt,
                        (cfg->comparator).filtSmpPeriod);

    COMP_HW_SetRoundRobinSamplesNo(base, (cfg->trigMode).samplesNumber);
    COMP_HW_SetRoundRobinChannels(base, (cfg->trigMode).roundRobinChannelsStatus);
    COMP_HW_SetRoundRobinIntStatus(base, (cfg->trigMode).roundRobinIntStatus);
    COMP_HW_SetRoundRobinStatus(base, (cfg->trigMode).roundRobinStatus);
    COMP_HW_SetPresetStatus(base, (cfg->trigMode).programedStatus);
    COMP_HW_SetFixedPm(base, (cfg->trigMode).fixedPm);
    COMP_HW_SetFixedChannel(base, (cfg->trigMode).fixedChannel);
    COMP_HW_SetPwrMode(base, (cfg->comparator).pwrMode);
    COMP_HW_SetDMATrigStatus(base, (cfg->comparator).dmaTrigStatus);
    COMP_HW_SetOutputIntTrig(base, (cfg->comparator).outputIntTrig);
    COMP_HW_SetCompOutputSrc(base, (cfg->comparator).outputSel);
    COMP_HW_SetInvertStatus(base, (cfg->comparator).invertStatus);
    COMP_HW_SetOutputPinStatus(base, (cfg->comparator).pinStatus);
    COMP_HW_SetPosPmInput(base, (cfg->mux).posPmMux);
    COMP_HW_SetNegPmInput(base, (cfg->mux).negPmMux);
    COMP_HW_SetHys(base, (cfg->comparator).hysLev);
    COMP_HW_SetVolt(base, (cfg->dac).volt);
    COMP_HW_SetVoltRef(base, (cfg->dac).voltRefSrc);
    COMP_HW_SetPlusMUXCtrl(base, (cfg->mux).posInputMux);
    COMP_HW_SetMinMUXCtrl(base, (cfg->mux).negInputMux);
    COMP_HW_SetDACStatus(base, (cfg->dac).state);

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    COMP_HW_SetFixedDACStatus(base, (cfg->dac).fixRefInputMux);
#endif

    COMP_HW_SetOffset(base, (cfg->comparator).offset);
    COMP_HW_SetInitDelay(base, (cfg->trigMode).initDelay);

    /* Clear all flags*/
    COMP_ClearOutputFlags(ins);
    CMP_ClearInputFlags(ins);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Reset all registers
 * @details This function set all COMP registers to reset values.
 *
 * @param ins Instance number
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *         STATUS_ERROR: Error occurred
 */
STATUS_T COMP_Reset(const uint32_t ins)
{
    COMP_T *base = g_compBase[ins];

    base->CTRL0.reg = 0U;
    base->CTRL1.reg = 0U;
    base->CTRL2.reg = 0U;

    return STATUS_SUCCESS;
}

/*!
 * @brief Gets a default comparator configuration
 *
 * This function returns a default configuration for the comparator as a configuration structure.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T COMP_DefaultConfig(COMP_MODULE_T* cfg)
{
#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    (cfg->dac).fixRefInputMux = false;
#endif

    (cfg->trigMode).initDelay = 0U;
    (cfg->comparator).offset = COMP_LEVEL_OFFSET_0;

    (cfg->mux).negInputMux = 1U;
    (cfg->mux).negPmMux = COMP_DAC;
    (cfg->mux).posInputMux = 1U;
    (cfg->mux).posPmMux = COMP_MUX;
    (cfg->dac).state = true;
    (cfg->dac).voltRefSrc = COMP_VOLT_REF_SRC_VIN1;
    (cfg->dac).volt = 127U;
    (cfg->trigMode).roundRobinStatus = false;
    (cfg->trigMode).roundRobinIntStatus = false;
    (cfg->trigMode).fixedPm = COMP_PLUS_FIXED;
    (cfg->trigMode).fixedChannel = 0U;
    (cfg->trigMode).samplesNumber = 0U;
    (cfg->trigMode).roundRobinChannelsStatus = 0U;
    (cfg->trigMode).programedStatus = 0U;
    (cfg->comparator).filtSmpCnt = 0U;
    (cfg->comparator).filtSmpPeriod = 0U;
    (cfg->comparator).dmaTrigStatus = false;
    (cfg->comparator).outputIntTrig = COMP_EVEN_INT_BY_BOTH_EDGES;
    (cfg->comparator).mode = COMP_CONSTANT_MODE;
    (cfg->comparator).hysLev = COMP_LEVEL_HYS_0;
    (cfg->comparator).pwrMode = COMP_SPEED_LOW;
    (cfg->comparator).invertStatus = COMP_NORMAL;
    (cfg->comparator).outputSel = COMP_COUT;
    (cfg->comparator).pinStatus = COMP_UNUSABLE;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read reset configuration for all registers
 * @details This function returns a configuration structure with reset values
 *          for all registers from comparator module.
 *
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetAllInitConfig(COMP_MODULE_T* cfg)
{
    (cfg->comparator).offset = COMP_LEVEL_OFFSET_0;

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    (cfg->dac).fixRefInputMux = false;
#endif

    (cfg->trigMode).initDelay = 0U;
    (cfg->mux).negPmMux = COMP_DAC;
    (cfg->mux).negInputMux = 0U;
    (cfg->mux).posPmMux = COMP_DAC;
    (cfg->mux).posInputMux = 0U;
    (cfg->dac).state = false;
    (cfg->dac).voltRefSrc = COMP_VOLT_REF_SRC_VIN1;
    (cfg->dac).volt = 0U;
    (cfg->trigMode).roundRobinStatus = false;
    (cfg->trigMode).roundRobinIntStatus = false;
    (cfg->trigMode).programedStatus = 0U;
    (cfg->trigMode).roundRobinChannelsStatus = 0U;
    (cfg->trigMode).fixedPm = COMP_PLUS_FIXED;
    (cfg->trigMode).samplesNumber = 0U;
    (cfg->trigMode).fixedChannel = 0U;
    (cfg->comparator).dmaTrigStatus = false;
    (cfg->comparator).outputIntTrig = COMP_NO_EVENT_NO_INT;
    (cfg->comparator).mode = COMP_DISABLED_MODE;
    (cfg->comparator).pwrMode = COMP_SPEED_LOW;
    (cfg->comparator).invertStatus = COMP_NORMAL;
    (cfg->comparator).outputSel = COMP_COUT;
    (cfg->comparator).filtSmpCnt = 0U;
    (cfg->comparator).filtSmpPeriod = 0U;
    (cfg->comparator).hysLev = COMP_LEVEL_HYS_0;
    (cfg->comparator).pinStatus = COMP_UNUSABLE;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read reset configuration for registers related with DAC
 * @details This function returns a configuration structure with reset values
 *          for features associated with DAC.
 *
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetDACInitConfig(COMP_DAC_T* cfg)
{
#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    cfg->fixRefInputMux = false;
#endif

    cfg->volt = 0U;
    cfg->state = false;
    cfg->voltRefSrc = COMP_VOLT_REF_SRC_VIN1;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read reset configuration for registers related with MUX
 * @details This function returns a configuration structure with reset values
 *          for features associated with MUX.
 *
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetMUXInitConfig(COMP_ANMUX_T* cfg)
{
    cfg->posInputMux = 0U;
    cfg->posPmMux = COMP_DAC;
    cfg->negInputMux = 0U;
    cfg->negPmMux = COMP_DAC;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read reset configuration for registers related with comparator features
 * @details This function return a configuration structure with reset values
 *          for features associated with comparator (DMA request, power mode,
 *          output select, interrupts enable, invert, offset, hysteresis).
 *
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetCompInitConfig(COMP_COMPARATOR_T* cfg)
{
    cfg->offset = COMP_LEVEL_OFFSET_0;

    cfg->pwrMode = COMP_SPEED_LOW;
    cfg->invertStatus = COMP_NORMAL;
    cfg->outputSel = COMP_COUT;
    cfg->pinStatus = COMP_UNUSABLE;
    cfg->hysLev = COMP_LEVEL_HYS_0;
    cfg->dmaTrigStatus = false;
    cfg->filtSmpCnt = 0U;
    cfg->filtSmpPeriod = 0U;
    cfg->mode = COMP_DISABLED_MODE;
    cfg->outputIntTrig = COMP_NO_EVENT_NO_INT;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read out reset configuration for registers related with Trigger Mode
 * @details This function returns a configuration structure with reset values
 *          for features associated with Trigger Mode.
 *
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetTriggerModeInitConfig(COMP_TRIG_MODE_T* cfg)
{
    cfg->initDelay = 0U;
    cfg->fixedChannel = 0U;
    cfg->samplesNumber = 0U;
    cfg->programedStatus = 0U;
    cfg->roundRobinStatus = false;
    cfg->roundRobinIntStatus = false;
    cfg->roundRobinChannelsStatus = 0U;
    cfg->fixedPm = COMP_PLUS_FIXED;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Gets the current comparator configuration
 * @details This function returns the current configuration for comparator
 *          as a configuration structure.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetAllConfig(const uint32_t ins, COMP_MODULE_T* const cfg)
{
    const COMP_T* base = g_compBase[ins];
    (cfg->trigMode).initDelay = COMP_HW_GetInitDelay(base);

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    (cfg->dac).fixRefInputMux = COMP_HW_GetFixedDACStatus(base);
#endif

    (cfg->comparator).offset = COMP_HW_GetOffset(base);

    (cfg->dac).state = COMP_HW_GetDACStatus(base);
    (cfg->dac).voltRefSrc = COMP_HW_GetVoltRef(base);
    (cfg->dac).volt = COMP_HW_GetVolt(base);
    (cfg->mux).negPmMux = COMP_HW_GetNegPmInput(base);
    (cfg->mux).posPmMux = COMP_HW_GetPosPmInput(base);
    (cfg->mux).negInputMux = COMP_HW_GetMinMUXCtrl(base);
    (cfg->mux).posInputMux = COMP_HW_GetPlusMUXCtrl(base);
    (cfg->trigMode).fixedChannel = COMP_HW_GetFixedChannel(base);
    (cfg->trigMode).samplesNumber = COMP_HW_GetRoundRobinSamplesNo(base);
    (cfg->trigMode).roundRobinChannelsStatus = COMP_HW_GetRoundRobinChannels(base);
    (cfg->trigMode).programedStatus = COMP_HW_GetLastCompareResult(base);
    (cfg->trigMode).roundRobinStatus = COMP_HW_GetRoundRobinStatus(base);
    (cfg->trigMode).roundRobinIntStatus = COMP_HW_GetRoundRobinIntStatus(base);
    (cfg->trigMode).fixedPm = COMP_HW_GetFixedPm(base);
    (cfg->comparator).outputSel = COMP_HW_GetCompOutputSrc(base);
    (cfg->comparator).pinStatus = COMP_HW_GetOutputPinStatus(base);
    (cfg->comparator).dmaTrigStatus = COMP_HW_GetDMATrigStatus(base);
    (cfg->comparator).outputIntTrig = COMP_HW_GetOutputIntTrig(base);
    (cfg->comparator).mode = COMP_HW_GetFuncMode(base);
    (cfg->comparator).filtSmpCnt = COMP_HW_GetFiltSmpCnt(base);
    (cfg->comparator).filtSmpPeriod = COMP_HW_GetFiltSmpPeriod(base);
    (cfg->comparator).hysLev = COMP_HW_GetHys(base);
    (cfg->comparator).pwrMode = COMP_HW_GetPwrMode(base);
    (cfg->comparator).invertStatus = COMP_HW_GetInvertStatus(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Return current configuration for DAC
 * @details This function returns current configuration only for DAC.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetDACConfig(const uint32_t ins, COMP_DAC_T* const cfg)
{
    const COMP_T* base = g_compBase[ins];

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    cfg->fixRefInputMux = COMP_HW_GetFixedDACStatus(base);
#endif

    cfg->volt = COMP_HW_GetVolt(base);
    cfg->state = COMP_HW_GetDACStatus(base);
    cfg->voltRefSrc = COMP_HW_GetVoltRef(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Return configuration only for the MUX component
 * @details This function returns current configuration to determine
 *          which signals go to comparator ports.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetMUXConfig(const uint32_t ins, COMP_ANMUX_T* const cfg)
{
    const COMP_T* base = g_compBase[ins];

    cfg->posPmMux = COMP_HW_GetPosPmInput(base);
    cfg->posInputMux = COMP_HW_GetPlusMUXCtrl(base);
    cfg->negPmMux = COMP_HW_GetNegPmInput(base);
    cfg->negInputMux = COMP_HW_GetMinMUXCtrl(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Return configuration for comparator from COMP module
 * @details This function return configuration for features related with
 *          comparator: DMA request, power mode, output select, interrupts
 *          enable, invert, offset, hysteresis.
 *
 * @param ins Instance number
 * @param cfg The configuration structure returned
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetCompConfig(const uint32_t ins, COMP_COMPARATOR_T *cfg)
{
    const COMP_T* base = g_compBase[ins];

    cfg->offset = COMP_HW_GetOffset(base);

    cfg->mode = COMP_HW_GetFuncMode(base);
    cfg->hysLev = COMP_HW_GetHys(base);
    cfg->pwrMode = COMP_HW_GetPwrMode(base);
    cfg->outputSel = COMP_HW_GetCompOutputSrc(base);
    cfg->pinStatus = COMP_HW_GetOutputPinStatus(base);
    cfg->filtSmpCnt = COMP_HW_GetFiltSmpCnt(base);
    cfg->invertStatus = COMP_HW_GetInvertStatus(base);
    cfg->outputIntTrig = COMP_HW_GetOutputIntTrig(base);
    cfg->dmaTrigStatus = COMP_HW_GetDMATrigStatus(base);
    cfg->filtSmpPeriod = COMP_HW_GetFiltSmpPeriod(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Read current trigger mode configuration
 * @details This function returns the current trigger mode configuration
 *          for trigger mode.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetTriggerModeConfig(const uint32_t ins, COMP_TRIG_MODE_T* const cfg)
{
    const COMP_T* base = g_compBase[ins];
    cfg->initDelay =  COMP_HW_GetInitDelay(base);
    cfg->fixedPm = COMP_HW_GetFixedPm(base);
    cfg->fixedChannel = COMP_HW_GetFixedChannel(base);
    cfg->samplesNumber = COMP_HW_GetRoundRobinSamplesNo(base);
    cfg->programedStatus = COMP_HW_GetLastCompareResult(base);
    cfg->roundRobinStatus = COMP_HW_GetRoundRobinStatus(base);
    cfg->roundRobinIntStatus = COMP_HW_GetRoundRobinIntStatus(base);
    cfg->roundRobinChannelsStatus = COMP_HW_GetRoundRobinChannels(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Configure the DAC component
 * @details This function configures the DAC with the options provided in the
 *          config structure.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_ConfigDAC(const uint32_t ins, const COMP_DAC_T* cfg)
{
    COMP_T* base = g_compBase[ins];

#if FEATURE_COMP_DAC_FIX_SELECTRLTION
    COMP_HW_SetFixedDACStatus(base, cfg->fixRefInputMux);
#endif

    COMP_HW_SetVolt(base, cfg->volt);
    COMP_HW_SetDACStatus(base, cfg->state);
    COMP_HW_SetVoltRef(base, cfg->voltRefSrc);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Configure the MUX component
 * @details This function configures the MUX with the options provided in the
 *          config structure.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_ConfigMUX(const uint32_t ins, const COMP_ANMUX_T* cfg)
{
    COMP_T* base = g_compBase[ins];

    COMP_HW_SetPosPmInput(base, cfg->posPmMux);
    COMP_HW_SetNegPmInput(base, cfg->negPmMux);
    COMP_HW_SetMinMUXCtrl(base, cfg->negInputMux);
    COMP_HW_SetPlusMUXCtrl(base, cfg->posInputMux);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Configure only comparator features
 * @details This function configure only features related with comparator:
 *          DMA request, power mode, output select, interrupts enable, invert,
 *          offset, hysteresis.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_ConfigComp(const uint32_t ins, const COMP_COMPARATOR_T *cfg)
{
    COMP_T* base = g_compBase[ins];

    COMP_HW_SetOffset(base, cfg->offset);

    COMP_HW_SetHys(base, cfg->hysLev);
    COMP_HW_SetPwrMode(base, cfg->pwrMode);
    COMP_HW_SetFiltSmpCnt(base, cfg->filtSmpCnt);
    COMP_HW_SetCompOutputSrc(base, cfg->outputSel);
    COMP_HW_SetOutputPinStatus(base, cfg->pinStatus);
    COMP_HW_SetInvertStatus(base, cfg->invertStatus);
    COMP_HW_SetDMATrigStatus(base, cfg->dmaTrigStatus);
    COMP_HW_SetOutputIntTrig(base, cfg->outputIntTrig);
    COMP_HW_SetFiltSmpPeriod(base, cfg->filtSmpPeriod);
    COMP_HW_SetFuncMode(base, cfg->mode, cfg->filtSmpCnt, cfg->filtSmpPeriod);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Configure trigger mode
 * @details This function configures the trigger mode with the options provided
 *          in the cfg structure.
 *
 * @param ins Instance number
 * @param cfg The configuration structure
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_ConfigTriggerMode(const uint32_t ins, const COMP_TRIG_MODE_T* cfg)
{
    COMP_T* base = g_compBase[ins];
    COMP_HW_SetInitDelay(base, cfg->initDelay);
    COMP_HW_SetFixedPm(base, cfg->fixedPm);
    COMP_HW_SetFixedChannel(base, cfg->fixedChannel);
    COMP_HW_SetPresetStatus(base, cfg->programedStatus);
    COMP_HW_SetRoundRobinStatus(base, cfg->roundRobinStatus);
    COMP_HW_SetRoundRobinSamplesNo(base, cfg->samplesNumber);
    COMP_HW_SetRoundRobinIntStatus(base, cfg->roundRobinIntStatus);
    COMP_HW_SetRoundRobinChannels(base, cfg->roundRobinChannelsStatus);
    return STATUS_SUCCESS;
}

/*!
 * @brief Clear comparator input channels flags
 *
 * @param ins Instance number
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_ClearInputFlags(const uint32_t ins)
{
    COMP_T *base = g_compBase[ins];

    uint32_t regValue = base->CTRL2.reg;
    regValue |= (uint32_t)0xFF0000;
    base->CTRL2.reg = regValue;
    (void)base->CTRL2.reg;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Gets input channels change flags
 * @details This function return in <flags> all input channels flags as uint8_t
 *          (1 bit for each channel flag).
 *
 * @param ins Instance number
 * @param flags Pointer to input flags
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T CMP_GetInputFlags(const uint32_t ins,  COMP_CH_LIST_T *flags)
{
    const COMP_T *base = g_compBase[ins];

    uint32_t regValue = base->CTRL2.reg;
    *flags = (COMP_CH_LIST_T)((regValue & (uint32_t)0xFF0000) >> 16U);
    return STATUS_SUCCESS;
 }

/*!
 * @brief   Clear comparator output flags
 * @details This function clear comparator output flags(rising and falling edge).
 *
 * @param ins Instance number
 *
 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T COMP_ClearOutputFlags(const uint32_t ins)
{
    COMP_T *base = g_compBase[ins];

    uint32_t regValue = base->CTRL0.reg;
    regValue &= ~(0x4000000u);
    regValue |= (((uint32_t)(((uint32_t)(1))<<26u))&0x4000000u);
    regValue &= ~(0x2000000u);
    regValue |= (((uint32_t)(((uint32_t)(1))<<25u))&0x2000000u);
    base->CTRL0.reg = regValue;
    (void)base->CTRL0.reg;
    return STATUS_SUCCESS;
 }

/*!
 * @brief   Read comparator output flags
 * @details This function returns in <flags> comparator output flags (rising
 *          and falling edge).
 *
 * @param ins Instance number
 * @param flags Pointer to output flags

 * @retval STATUS_SUCCESS: Completed successfully
 *          STATUS_ERROR: Error occurred
 */
STATUS_T COMP_GetOutputFlags(const uint32_t ins, COMP_OUTPUT_TRIG_T *flags)
{
    const COMP_T *base = g_compBase[ins];

    bool enableRising = (bool)(((base->CTRL0.reg) >> 26u) & (uint32_t)1U);
    bool enableFalling = (bool)(((base->CTRL0.reg) >> 25u) & (uint32_t)1U);

    *flags = (COMP_OUTPUT_TRIG_T) (((uint32_t)enableRising << (uint32_t)1U)
                                 | ((uint32_t)enableFalling));
    return STATUS_SUCCESS;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief check if the DAC is enabled
 * @param base COMP base pointer
 * @retval DAC state
 *  true - DAC is enabled
 *  false - DAC is disabled
 */
bool COMP_HW_GetDACStatus(const COMP_T *base)
{
    return (bool)(base->CTRL1.bit.DACEN);
}

/*!
 * @brief Set the DAC state (enabled/disabled)
 * @param base COMP base pointer
 * @param value DAC state
 *  true - DAC is enabled
 *  false - DAC is disabled
 * @retval None
 */
void COMP_HW_SetDACStatus(COMP_T *base, bool value)
{
    base->CTRL1.bit.DACEN = value;
}

/*!
 * @brief Set the volt reference
 * @param base COMP base pointer
 * @param value volt reference
 *  COMP_VOLT_REF_SRC_VIN1
 *  COMP_VOLT_REF_SRC_VIN2
 * @retval None
 */
void COMP_HW_SetVoltRef(COMP_T *base, COMP_VOLT_REF_T value)
{
    base->CTRL1.bit.SVRSSEL = value;
}

/*!
 * @brief Return the current volt reference
 * @param base COMP base pointer
 * @retval volt referece
 */
COMP_VOLT_REF_T COMP_HW_GetVoltRef(const COMP_T *base)
{
    return (COMP_VOLT_REF_T) (base->CTRL1.bit.SVRSSEL);
}

/*!
 * @brief Return the current output volt level(0-255)
 * @param base COMP base pointer
 * @retval volt level
 */
uint8_t COMP_HW_GetVolt(const COMP_T *base)
{
    return (uint8_t) (base->CTRL1.bit.DACOVCFG);
}

/*!
 * @brief Set the output volt level
 * @param base COMP base pointer
 * @param value volt level
 * @retval None
 */
void COMP_HW_SetVolt(COMP_T *base, uint8_t value)
{
    base->CTRL1.bit.DACOVCFG = value;
}

/*!
 * @brief This function read COMP functional mode based on configuration
 * @param base COMP base pointer
 * @retval COMP functional mode
 */
COMP_MODE_T COMP_HW_GetFuncMode(const COMP_T *base)
{
    COMP_MODE_T compMode = COMP_DISABLED_MODE;
    uint32_t samplePeriod = (base->CTRL0.reg & 0xff0000u) >> 16u;
    uint32_t sampleCount = (base->CTRL0.reg & 0x70u) >> 4u;
    uint32_t value;

    value = (((base->CTRL0.reg) & 0x100u) >> 8u) << 2U;
    value |= (((base->CTRL0.reg) & 0x4000u) >> 14u) << 1U;
    value |= (((base->CTRL0.reg) & 0x8000u) >> 15u);

    if (value == 4U)
    {
        if ((samplePeriod == 0U) || (sampleCount == 0U))
        {
            compMode = COMP_CONSTANT_MODE;
        }
        else if (sampleCount == 1U)
        {
            compMode = COMP_SMPED_NONFILTRED_INT_CLK_MODE;
        }
        else if (sampleCount > 1U)
        {
            compMode = COMP_SMPED_FILTRED_INT_CLK_MODE;
        }
    }
    else if (value == 5U)
    {
        if (sampleCount == 1U)
        {
            compMode = COMP_SMPED_NONFILTRED_EXT_CLK_MODE;
        }
        else if (sampleCount > 1u)
        {
            compMode = COMP_SMPED_FILTRED_EXT_CLK_MODE;
        }
    }
    else if (value == 6U)
    {

        if ((samplePeriod == 0U) || (sampleCount == 0U))
        {
            compMode = COMP_WINDOWED_MODE;
        }
        else if ((sampleCount == 1U) && (samplePeriod > 0U))
        {
            compMode = COMP_WINDOWED_RESMPED_MODE;
        }
        else if ((sampleCount > 1U) && (samplePeriod > 0U))
        {
            compMode = COMP_WINDOWED_FILTRED_MODE;
        }
    }
    else
    {
    }

    if ((value & 0x4U) == 0U)
    {
        compMode = COMP_DISABLED_MODE;
    }
    return compMode;
}

/*!
 * @brief Sets the comparator functional mode (mode, filter count, filter period)

 * @param base COMP base pointer
 * @param mode Functional mode
 * @param sampleCount Number of consecutive samplesNumber that must agree prior
 *                    to the comparator ouput filter accepting a new output state
 * @param samplePeriod Sampling period
 *
 * @retval None
 */
 void COMP_HW_SetFuncMode(
    COMP_T *base,
    COMP_MODE_T mode,
    uint8_t sampleCount,
    uint8_t samplePeriod)
 {
    uint32_t value = base->CTRL0.reg;
    value = value & (~(0x8000u & 0xff0000u & 0x70u & 0x100u & 0x4000u));
    base->CTRL0.reg = value;

    if (mode == COMP_CONSTANT_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
    }
    else if (mode == COMP_SMPED_NONFILTRED_INT_CLK_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.FOSMPCNT = 1;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
    else if (mode == COMP_SMPED_NONFILTRED_EXT_CLK_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 1;
        base->CTRL0.bit.FOSMPCNT = 1;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
    else if (mode == COMP_SMPED_FILTRED_EXT_CLK_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 1;
        base->CTRL0.bit.FOSMPCNT = sampleCount;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
    else if (mode == COMP_SMPED_FILTRED_INT_CLK_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 0;
        base->CTRL0.bit.FOSMPCNT = sampleCount;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
    else if (mode == COMP_WINDOWED_RESMPED_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 0;
        base->CTRL0.bit.WINMEN = 1;
        base->CTRL0.bit.FOSMPCNT = 1;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
    else if (mode == COMP_WINDOWED_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 0;
        base->CTRL0.bit.FOSMPCNT = 0;
        base->CTRL0.bit.WINMEN = 1;
    }
    else if (mode == COMP_WINDOWED_FILTRED_MODE)
    {
        base->CTRL0.bit.ACEN = 1;
        base->CTRL0.bit.SMPMEN = 0;
        base->CTRL0.bit.WINMEN = 1;
        base->CTRL0.bit.FOSMPCNT = sampleCount;
        base->CTRL0.bit.FSMPPCFG = samplePeriod;
    }
}

/*!
 * @brief Verify if the DMA transfer trigger is enabled
 * @param base COMP base pointer
 * @retval DMA transfer trigger state
 *  true - DMA trigger is enabled
 *  false - DAM trigger is disabled
 */
bool COMP_HW_GetDMATrigStatus(const COMP_T *base)
{
    return (bool)(base->CTRL0.bit.DMAEN);
}

/*!
 * @brief Configure the DMA transfer trigger
 * @param base COMP base pointer
 * @param value DMA transfer trigger state
 *  true - DMA trigger is enabled
 *  false - DAM trigger is disabled
 * @retval None
 */
void COMP_HW_SetDMATrigStatus(COMP_T *base, bool value)
{
    base->CTRL0.bit.DMAEN = value;
}

/*!
 * @brief Set the power mode
 * @param base COMP base pointer
 * @param value power mode
 *  COMP_SPEED_LOW
 *  COMP_SPEED_HIGH
 */
void COMP_HW_SetPwrMode(COMP_T *base, COMP_PWR_MODE_T value)
{
    base->CTRL0.bit.PMSEL = value;
}

/*!
 * @brief Return the current power mode
 * @param base COMP base pointer
 * @retval current power mode
 *  COMP_SPEED_LOW
 *  COMP_SPEED_HIGH
 */
COMP_PWR_MODE_T COMP_HW_GetPwrMode(const COMP_T *base)
{
    return (COMP_PWR_MODE_T)(base->CTRL0.bit.PMSEL);
}

/*!
 * @brief   Set the comparator output interrupts source configuration (none,
 *          rising edge, falling edge or both edges)
 * @param base COMP base pointer
 * @param value comparator output interrupts configuration
 *  COMP_NO_EVENT_NO_INT
 *  COMP_EVEN_INT_BY_FALLING_EDGE
 *  COMP_EVEN_INT_BY_RISING_EDGE
 *  COMP_EVEN_INT_BY_BOTH_EDGES
 * @retval None
 */
void COMP_HW_SetOutputIntTrig(COMP_T *base, COMP_OUTPUT_TRIG_T value)
{
    base->CTRL0.bit.ACFIEN = value&0x01;
    base->CTRL0.bit.ACRIEN = (value>>1)&0x01;
}

/*!
 * @brief   Return the comparator output interrupts source configuration(none,
            rising edge, falling edge or both edges)
 * @param base COMP base pointer
 * @retval comparator output interrupts configuration
 *  COMP_NO_EVENT_NO_INT
 *  COMP_EVEN_INT_BY_FALLING_EDGE
 *  COMP_EVEN_INT_BY_RISING_EDGE
 *  COMP_EVEN_INT_BY_BOTH_EDGES
 */
COMP_OUTPUT_TRIG_T COMP_HW_GetOutputIntTrig(const COMP_T *base)
{
    bool enableFalling = (bool)(((base->CTRL0.reg) >> 27u) & 1U);
    bool enableRising = (bool)(((base->CTRL0.reg) >> 28u) & 1U);
    return (COMP_OUTPUT_TRIG_T) (((uint32_t)enableRising << (uint32_t)1U)
                               | ((uint32_t)enableFalling));
}

/*!
 * @brief Set the filter sample period (clock cycles)
 * @param base COMP base pointer
 * @param value Number of bus cycles
 * @retval None
 */
void COMP_HW_SetFiltSmpPeriod(COMP_T *base, uint8_t value)
{
    base->CTRL0.bit.FSMPPCFG = value;
}

/*!
 * @brief Return the sample period for filter(clock cycles)
 * @param base COMP base pointer
 * @retval sampling period (in bus cycles)
 */
uint8_t COMP_HW_GetFiltSmpPeriod(const COMP_T *base)
{
    return (uint8_t) (base->CTRL0.bit.FSMPPCFG);
}

/*!
 * @brief Return the current comparator output inverter
 * @param baseAddress comp base pointer
 * @retval inverter state
 *  COMP_NORMAL
 *  COMP_INVERT
 */
COMP_INVERT_T COMP_HW_GetInvertStatus(const COMP_T *base)
{
    return (COMP_INVERT_T)(base->CTRL0.bit.INVEN);
}

/*!
 * @brief Configure the comparator output inverter mode
 * @param base COMP base pointer
 * @param value comparator output inverter mode
 *  COMP_NORMAL
 *  COMP_INVERT
 * @retval None
 */
void COMP_HW_SetInvertStatus(COMP_T *base, COMP_INVERT_T value)
{
    base->CTRL0.bit.INVEN = value;
}

/*!
 * @brief Return the current comparator output selected
 * @param base COMP base pointer
 * @retval comparator output signal source
 *  COMP_COUT
 *  COMP_COUTA
 */
COMP_OUTPUT_SEL_T COMP_HW_GetCompOutputSrc(const COMP_T *base)
{
    return (COMP_OUTPUT_SEL_T)(base->CTRL0.bit.COCFG);
}

/*!
 * @brief Select the comparator output signal source
 * @param base COMP base pointer
 * @param value comparator output signal source
 *  COMP_COUT
 *  COMP_COUTA
 * @retval vid
 */
void COMP_HW_SetCompOutputSrc(COMP_T *base, COMP_OUTPUT_SEL_T value)
{
    base->CTRL0.bit.COCFG = value;
}

/*!
 * @brief   Set the comparator output pin state (available/not available
            in a packaged pin)
 * @param base COMP base pointer
 * @param value comparator output state
 *  COMP_UNUSABLE
 *  COMP_USABLE
 * @retval None
 */
void COMP_HW_SetOutputPinStatus(COMP_T *base, COMP_OUTPUT_ENABLE_T value)
{
    base->CTRL0.bit.COPCFG = value;
}

/*!
 * @brief   Get the comparator output state (available/not available
            in a packaged pin)
 * @param base COMP base pointer
 * @retval comparator output state
 *  COMP_UNUSABLE
 *  COMP_USABLE
 */
COMP_OUTPUT_ENABLE_T COMP_HW_GetOutputPinStatus(const COMP_T *base)
{
    return (COMP_OUTPUT_ENABLE_T)(base->CTRL0.bit.COPCFG);
}

/*!
 * @brief   Set the number of consecutive samplesNumber that must agree prior
 *          to the comparator output filter accepting a new output state
 * @param base COMP base pointer
 * @param value filter sample count (min value 0, max value 7)
 * @retval None
 */
void COMP_HW_SetFiltSmpCnt(COMP_T *base, uint8_t value)
{
    base->CTRL0.bit.FOSMPCNT = value;
}

/*!
 * @brief   Return the number of consecutive samplesNumber that must agree
 *          prior to the comparator output filter accepting a new output state
 * @param base COMP base pointer
 * @retval filter sample count
 */
uint8_t COMP_HW_GetFiltSmpCnt(const COMP_T *base)
{
    return (uint8_t)(base->CTRL0.bit.FOSMPCNT);
}

/*!
 * @brief Set the offset level
 * @param base COMP base pointer
 * @param value offset level
 * @retval None
 */
void COMP_HW_SetOffset(COMP_T *base, COMP_HBOLSEL_T value)
{
    base->CTRL0.bit.HBOLSEL = value;
}

/*!
 * @brief Return the current offset level
 * @param base COMP base pointer
 * @retval offset level
 */
COMP_HBOLSEL_T COMP_HW_GetOffset(const COMP_T *base)
{
    return (COMP_HBOLSEL_T)(base->CTRL0.bit.HBOLSEL);
}

/*!
 * @brief Set the hysteresis level
 * @param base COMP base pointer
 * @param value hysteresis level
 * @retval None
 */
void COMP_HW_SetHys(COMP_T *base, COMP_HYS_T value)
{
    base->CTRL0.bit.HBHLSEL = value;
}

/*!
 * @brief Return the current hysteresis level
 * @param base COMP base pointer
 * @retval current hysteresis level
 */
COMP_HYS_T COMP_HW_GetHys(const COMP_T *base)
{
    return (COMP_HYS_T) (base->CTRL0.bit.HBHLSEL);
}

/*!
 * @brief Return the current source for negative port of the comparator
 * @param base COMP base pointer
 * @retval signal source
 */
COMP_PM_MUX_T COMP_HW_GetNegPmInput(const COMP_T *base)
{
    return (COMP_PM_MUX_T) (base->CTRL1.bit.CNISEL);
}

/*!
 * @brief Set the source for negative port of the comparator
 * @param base COMP base pointer
 * @param value signal source
 * @retval None
 */
void COMP_HW_SetNegPmInput(COMP_T *base, COMP_PM_MUX_T value)
{
    base->CTRL1.bit.CNISEL = value;
}

/*!
 * @brief Return which channels are used for round-robin checker
 * @param base COMP base pointer
 * @retval channels states, one bite for each channel state
 */
COMP_CH_LIST_T COMP_HW_GetRoundRobinChannels(const COMP_T *base)
{
    uint32_t tempValue = base->CTRL1.reg;
    tempValue =  tempValue & (uint32_t)0xFF0000;
    return(COMP_CH_LIST_T) (tempValue >> 16u);
}

/*!
 * @brief Set which channels are use for round-robin checker
 * @param base COMP base pointer
 * @param value channels states, one bite for each channel state
 * @retval None
 */
void COMP_HW_SetRoundRobinChannels(COMP_T *base, COMP_CH_LIST_T value)
{
    uint32_t tempValue = base->CTRL1.reg;
    tempValue &= ~(uint32_t)(0xFF0000);
    tempValue |= (uint32_t)value << (uint32_t)16u;
    base->CTRL1.reg = tempValue;
}

/*!
 * @brief Set the source for positive port of the comparator
 * @param baseAddress comp base pointer
 * @param value signal source
 * @retval None
 */
void COMP_HW_SetPosPmInput(COMP_T *base, COMP_PM_MUX_T value)
{
    base->CTRL1.bit.CPISEL = value;
}

/*!
 * @brief Return the current source for positive port of the comparator
 * @param base COMP base pointer
 * @retval signal source
 */
COMP_PM_MUX_T COMP_HW_GetPosPmInput(const COMP_T *base)
{
    return (COMP_PM_MUX_T)(base->CTRL1.bit.CPISEL);
}

/*!
 * @brief Select input for the plus mux
 * @param baseAddress comp base pointer
 * @param value channel for the plus mux
 * @retval None
 */
void COMP_HW_SetPlusMUXCtrl(COMP_T *base, COMP_CH_NUMBER_T value)
{
    base->CTRL1.bit.MUXPICSEL = value;
}

/*!
 * @brief Determine which input is selected for the plus mux
 * @param base COMP base pointer
 * @retval channel for the plus mux
 */
COMP_CH_NUMBER_T COMP_HW_GetPlusMUXCtrl(const COMP_T *base)
{
    return (COMP_CH_NUMBER_T) (base->CTRL1.bit.MUXPICSEL);
}

/*!
 * @brief Select input for the minus mux
 * @param base COMP base pointer
 * @param value channel for the minus mux
 * @retval None
 */
void COMP_HW_SetMinMUXCtrl(COMP_T *base, COMP_CH_NUMBER_T value)
{
    base->CTRL1.bit.MUXMICSEL = value;
}

/*!
 * @brief Determine which input is selected for the minus mux
 * @param base COMP base pointer
 * @retval channel for the minus mux
 */
COMP_CH_NUMBER_T COMP_HW_GetMinMUXCtrl(const COMP_T *base)
{
    return (COMP_CH_NUMBER_T) (base->CTRL1.bit.MUXMICSEL);
}

/*!
 * @brief Set the round robin operation state
 * @param baseAddress comp base pointer
 * @param value round robin operation state
 *  true - round robin operation is enabled
 *  false - round robin operation is disabled
 * @retval None
 */
void COMP_HW_SetRoundRobinStatus(COMP_T *base, bool value)
{
    base->CTRL2.bit.RRSEL = value;
}

/*!
 * @brief Verify if the round robin operation is enabled
 * @param base COMP base pointer
 * @retval round-robin operation state
 *  true - round robin operation is enabled
 *  false - round robin operation is disabled
 */
bool COMP_HW_GetRoundRobinStatus(const COMP_T *base)
{
    return (bool) (base->CTRL2.bit.RRSEL);
}

/*!
 * @brief Set the round robin interrupt state
 * @param base COMP base pointer
 * @param value round robin interrupt state
 *  true - round robin interrupt is enabled
 *  false - round robin interrupt is disabled
 * @retval None
 */
void COMP_HW_SetRoundRobinIntStatus(COMP_T *base, bool value)
{
    base->CTRL2.bit.RRIEN = value;
}

/*!
 * @brief Verify if the round robin interrupt is enabled
 * @param base COMP base pointer
 * @retval round-robin interrupt state
 *  true - round robin interrupt is enabled
 *  false - round robin interrupt is disabled
 */
bool COMP_HW_GetRoundRobinIntStatus(const COMP_T *base)
{
    return (bool) (base->CTRL2.bit.RRIEN);
}

/*!
 * @brief Set the fixed port for round-robin operation
 * @param base COMP base pointer
 * @param value fixed port
 * @retval None
 */
void COMP_HW_SetFixedPm(COMP_T *base, COMP_FIXED_PM_T value)
{
    base->CTRL2.bit.MUXFPSEL = value;
}

/*!
 * @brief Return the port fixed for round-robin operation
 * @param base COMP base pointer
 * @retval fixed port
 */
COMP_FIXED_PM_T COMP_HW_GetFixedPm(const COMP_T *base)
{
    return (COMP_FIXED_PM_T) (base->CTRL2.bit.MUXFPSEL);
}

/*!
 * @brief Set which channel is used as the fixed reference input for the fixed mux port
 * @param base COMP base pointer
 * @param value fixed channel
 * @retval None
 */
void COMP_HW_SetFixedChannel(COMP_T *base, COMP_CH_NUMBER_T value)
{
    base->CTRL2.bit.MUXFCHSEL = value;
}

/*!
 * @brief Return which channel is selected for fixed mux port(as fixed reference)
 * @param base COMP base pointer
 * @retval fixed channel
 */
COMP_CH_NUMBER_T COMP_HW_GetFixedChannel(const COMP_T *base)
{
    return (COMP_CH_NUMBER_T) (base->CTRL2.bit.MUXFCHSEL);
}

/*!
 * @brief Set how many round-robin clock cycles takes sampling
 * @param base COMP base pointer
 * @param value number of sample clocks(min value 0, max value 3)
 * @retval None
 */
void COMP_HW_SetRoundRobinSamplesNo(COMP_T *base, uint8_t value)
{
    base->CTRL2.bit.NSCFG = value;
}

/*!
 * @brief Return how many round-robin clock cycles takes sampling
 * @param base COMP base pointer
 * @retval number of sample clocks
 */
uint8_t COMP_HW_GetRoundRobinSamplesNo(const COMP_T *base)
{
    return (uint8_t) (base->CTRL2.bit.NSCFG);
}

/*!
 * @brief Set the comparator and  DAC initialization delay
 * @param base COMP base pointer
 * @param value delay (min value 0, max value 63)
 * @retval None
 */
void COMP_HW_SetInitDelay(COMP_T *base, uint8_t value)
{
    base->CTRL2.bit.IDCLKCFG = value;
}

/*!
 * @brief Return the comparator and DAC initialization delay
 * @param base COMP base pointer
 * @retval delay(round-robin clock period)
 */
uint8_t COMP_HW_GetInitDelay(const COMP_T *base)
{
    return (uint8_t) (base->CTRL2.bit.IDCLKCFG);
}

/*!
 * @brief Defines the pre-set state of input channels.
 * @param baseAddress comp base pointer
 * @param value state
 * @retval void
 */
void COMP_HW_SetPresetStatus(COMP_T *base, COMP_CH_LIST_T value)
{
    base->CTRL2.bit.CRETCHn = value;

}

/*!
 * @brief Return last input comparison results for all channels
 * @param base COMP base pointer
 * @retval comparison results
 */
COMP_CH_LIST_T COMP_HW_GetLastCompareResult(const COMP_T *base)
{
    return (COMP_CH_LIST_T) (base->CTRL2.bit.CRETCHn);
}

/**@} end of group COMP_Functions*/
/**@} end of group COMP_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
