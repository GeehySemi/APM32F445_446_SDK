/*!
 * @file        apm32f445_446_pdu.c
 *
 * @brief       This file provides all the PDU firmware functions
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
#include "apm32f445_446_pdu.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup PDU_Driver PDU Driver
  @{
*/

/** @defgroup PDU_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/* Number of instances of the PDU module. */
#define PDU_INSTANCE_COUNT            (2u)
#define ASSERT_INSTANCE(__instance__) ({if(__instance__ >= PDU_INSTANCE_COUNT) {while(1);}})

/**@} end of group PDU_Macros*/

/** @defgroup PDU_Variables Variables
  @{
*/

/* PDU module register base address */
static PDU_T *const g_pduBase[PDU_INSTANCE_COUNT] = { PDU0, PDU1 };

/* PDU module IRQ number */
static const IRQn_Type g_pduIrqId[PDU_INSTANCE_COUNT] = { PDU0_IRQn, PDU1_IRQn };

/**@} end of group PDU_Variables*/

/** @defgroup PDU_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initializes triggers input source and the PDU counter.
 *
 * @param instance: PDU instance number.
 * @param timerConfig: User configuration structure for the PDU driver.
 *
 * @retval None
 */
void PDU_Init(const uint32_t instance, const PDU_TIMER_CONFIG_T *const timerConfig)
{
    ASSERT_INSTANCE(instance);

    /* PDU clock names array */
#if (PDU_INSTANCE_COUNT == 1U)
    static const CLOCK_NAMES_T s_pduClkNames[PDU_INSTANCE_COUNT] = {PDU0_CLK};
#elif (PDU_INSTANCE_COUNT == 2U)
    static const CLOCK_NAMES_T s_pduClkNames[PDU_INSTANCE_COUNT] = {PDU0_CLK, PDU1_CLK};
#endif

    CLOCK_NAMES_T instanceClkName = s_pduClkNames[instance];
    PDU_T *pduBase = g_pduBase[instance];

    if (CLOCK_SYS_ReadFreq(instanceClkName, NULL) != STATUS_SUCCESS)
    {
        return;
    }

    PDU_HW_ResetRegister(pduBase);
    PDU_HW_ConfigTimer(pduBase, timerConfig);

#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
    /* Enable the instance back to back mode between PDU0 CH0 and PDU1 CH0 pre-triggers.
     * Note1: This configuration is common between all PDU instances.
     * Note2: Must not be enabled simultaneously with inter-channel back to back on any PDU instance.
     */
    if (!timerConfig->instanceBackToBackEn)
    {
        PDU_HW_SIM_DisableInstanceBackToBack();
    }
    else
    {
        PDU_HW_SIM_EnableInstanceBackToBack();
    }
#endif

    /* PDU interrupt configuration */
    if ((!timerConfig->intEn) && (!timerConfig->seqErrIntEn))
    {
        /* Disable PDU interrupt in the interrupt manager */
        INT_SYS_DisableIRQ(g_pduIrqId[instance]);
    }
    else
    {
        /* Enable PDU interrupt in the interrupt manager */
        INT_SYS_EnableIRQ(g_pduIrqId[instance]);
    }
}

/*!
 * @brief De-Initialize the PDU module, disable all the PDU interrupts.
 *
 * @param instance: PDU instance number.
 *
 * @retval None
 *
 * @note Instance back to back configuration is common between PDU instances
 *       0 and 1 (configures the same register even if configured for either PDU instance).
 *       This function disables it, so affects all other instances.
 */
void PDU_DeInit(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    INT_SYS_DisableIRQ(g_pduIrqId[instance]);

#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
    PDU_HW_SIM_DisableInstanceBackToBack();
#endif

    /* Disable PDU */
    pduBase->CSTS.bit.PDUEN = PDU_CSTS_PDUEN_0;
}

/*!
 * @brief Return PDU the default configuration.
 *
 * @param timerConfig: Pointer to user configuration structure for PDU driver.
 *
 * @retval None
 */
void PDU_DefaultConfig(PDU_TIMER_CONFIG_T *const timerConfig)
{
    /* Load mode selection */
    timerConfig->loadValMode = PDU_LOAD_VAL_IMMEDIATELY;
    /* Prescaler divider selection */
    timerConfig->clkPscDiv = PDU_CLK_PSCDIV1;
    /* Multiplication factor of the prescaler selection */
    timerConfig->clkPscMultFactor = PDU_CLK_PSCMULT_FACTOR_AS_1;
    /* Trigger input source selection */
    timerConfig->triggerInput = PDU_TRIGGER_IN0;
    /* Disable sequence error interrupt */
    timerConfig->seqErrIntEn = false;
    /* Disable PDU module interrupt */
    timerConfig->intEn = false;
    /* Disable continuous run mode, select One-shot mode */
    timerConfig->continuousModeEn = false;
    /* Disable DMA */
    timerConfig->dmaEn = false;
#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
    /* Disable back to back mode */
    timerConfig->instanceBackToBackEn = false;
#endif
}

/*!
 * @brief Enables the PDU module.
 *
 * @param instance: PDU instance number.
 *
 * @retval None
 */
void PDU_Enable(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];
    PDU_HW_Enable(pduBase);
}

/*!
 * @brief Disables the PDU module.
 *
 * @param instance: PDU instance number.
 *
 * @retval None
 */
void PDU_Disable(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];
    PDU_HW_Disable(pduBase);
}

/*!
 * @brief Enable PDU software trigger.
 *
 * @param instance: PDU instance number.
 *
 * @retval None
 */
void PDU_EnableSoftTrigger(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];
    PDU_HW_EnableSoftTrigger(pduBase);
}

/*!
 * @brief Reads the current value of the PDU counter.
 *
 * @param instance: PDU instance number.
 *
 * @retval Current PDU counter value.
 */
uint32_t PDU_ReadTimerValue(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    uint32_t temp;
    PDU_T const *pduBase = g_pduBase[instance];

    temp = PDU_HW_ReadTimerValue(pduBase);
    return temp;
}

/*!
 * @brief Reads the PDU interrupt flag.
 *
 * @param instance: PDU instance number.
 *
 * @retval Interrupt flag status.
 *         true: interrupt flag enable
 *         false: interrupt flag disable
 */
bool PDU_ReadTimerIntFlag(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T const *pduBase = g_pduBase[instance];
    bool temp;

    temp = PDU_HW_ReadTimerIntFlag(pduBase);
    return temp;
}

/*!
 * @brief Clears the interrupt flag.
 *
 * @param instance: PDU instance number.
 *
 * @retval None
 */
void PDU_ClearTimerIntFlag(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ClearTimerIntFlag(pduBase);
}

/*!
 * @brief Executes the command of loading values.
 *
 * @param instance PDU instance number.
 *
 * @retval None
 */
void PDU_LoadValuesCmd(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigLoadValuesCmd(pduBase);
}

/*!
 * @brief Enable/Disable the CMP pulse out in the PDU module.
 *
 * @param instance: PDU instance number.
 * @param pulseChnMask: Pulse channel mask.
 * @param enable: Ebable/disable the CMP pulse out.
 *
 * @retval None
 */
void PDU_ConfigCmpPulseOutEnable(
    const uint32_t instance,
    const uint32_t pulseChnMask,
    const bool enable)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigCmpPulseOutEnable(pduBase, pulseChnMask, enable);
}

/*!
 * @brief Configures the CMP pulse out delay value for high.
 *
 * @param instance: PDU instance number.
 * @param pulseChannel: Pulse channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_ConfigCmpPulseOutDelayForHigh(
    const uint32_t instance,
    const uint32_t pulseChannel,
    const uint32_t value)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigCmpPulseOutDelayForHigh(pduBase, pulseChannel, value);
}

/*!
 * @brief Configures the CMP pulse out delay value for low.
 *
 * @param instance: PDU instance number.
 * @param pulseChannel: Pulse channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_ConfigCmpPulseOutDelayForLow(
    const uint32_t instance,
    const uint32_t pulseChannel,
    const uint32_t value)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigCmpPulseOutDelayForLow(pduBase, pulseChannel, value);
}

/*!
 * @brief Configures the value of timer modulus.
 *
 * @param instance: PDU instance number.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_ConfigPeriodCountValue(const uint32_t instance, const uint16_t value)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigPeriodCountValue(pduBase, value);
}

/*!
 * @brief Configures timer interrupt delay value.
 *
 * @param instance: PDU instance number.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_ConfigTimerIntDelayValue(const uint32_t instance, const uint16_t value)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigTimerIntDelayValue(pduBase, value);
}

/*!
 * @brief Configures the ADC pre-trigger.
 *
 * @param instance: PDU instance number.
 * @param channel: PDU channel.
 * @param cfgPtr: Pointer to ADC's pre_trigger configuration structure.
 *
 * @retval None
 *
 * @note Any pretrigger which is enabled and has the trigger output enabled (preTriggerOutputEn
 * and preTriggerEn both true) must have the corresponding delay value set to a non-zero
 * value by calling "PDU_HW_ConfigAdcPreTriggerDelayValue" function.
 */
void PDU_ConfigAdcPreTrigger(
    const uint32_t instance,
    const uint32_t channel,
    const PDU_ADC_PRETRIGGER_CONFIG_T *cfgPtr)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_EnableAdcPreTrigger(
        pduBase,
        channel,
        ((uint32_t)1U << (cfgPtr->adcPreTriggerIndex)),
        cfgPtr->preTriggerEn);

    PDU_HW_EnableAdcPreTriggerOutput(
        pduBase,
        channel,
        ((uint32_t)1U << (cfgPtr->adcPreTriggerIndex)),
        cfgPtr->preTriggerOutputEn);

    PDU_HW_EnableAdcPreTriggerBackToBackMode(
        pduBase,
        channel,
        ((uint32_t)1U << (cfgPtr->adcPreTriggerIndex)),
        cfgPtr->preTriggerBackToBackEn);
}

/*!
 * @brief Reads the ADC pre-trigger flag.
 *
 * @param instance: PDU instance number.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval ADC pre-trigger flag.
 */
uint32_t PDU_ReadAdcPreTriggerFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    ASSERT_INSTANCE(instance);

    PDU_T const *pduBase = g_pduBase[instance];
    uint32_t temp;

    temp = PDU_HW_ReadAdcPreTriggerFlags(pduBase, channel, preChnMask);
    return temp;
}

/*!
 * @brief Clears the ADC pre-trigger flag.
 *
 * @param instance: PDU instance number.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval None
 */
void PDU_ClearAdcPreTriggerFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ClearAdcPreTriggerFlags(pduBase, channel, preChnMask);
}

/*!
 * @brief Reads the ADC pre-trigger sequence error flag.
 *
 * @param instance: PDU instance number.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval Assertion of indicated flag.
 */
uint32_t PDU_ReadAdcPreTriggerSeqErrFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    ASSERT_INSTANCE(instance);

    PDU_T const *pduBase = g_pduBase[instance];
    uint32_t temp;

    temp = PDU_HW_ReadAdcPreTriggerSeqErrFlags(pduBase, channel, preChnMask);
    return temp;
}

/*!
 * @brief Clears the ADC pre-trigger sequence error flag.
 *
 * @param instance: PDU instance number.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval None
 */
void PDU_ClearAdcPreTriggerSeqErrFlags(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ClearAdcPreTriggerSeqErrFlags(pduBase, channel, preChnMask);
}

/*!
 * @brief Configures the ADC pre-trigger delay value.
 *
 * @param instance: PDU instance number.
 * @param channel: ADC channel.
 * @param preChn: ADC pre_channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_ConfigAdcPreTriggerDelayValue(
    const uint32_t instance,
    const uint32_t channel,
    const uint32_t preChn,
    const uint32_t value)
{
    ASSERT_INSTANCE(instance);

    PDU_T *pduBase = g_pduBase[instance];

    PDU_HW_ConfigAdcPreTriggerDelayValue(pduBase, channel, preChn, value);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enable the PDU module.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_Enable(PDU_T *const pduBase)
{
    pduBase->CSTS.bit.PDUEN = PDU_CSTS_PDUEN_1;
}

/*!
 * @brief Disable the PDU module.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_Disable(PDU_T *const pduBase)
{
    pduBase->CSTS.bit.PDUEN = PDU_CSTS_PDUEN_0;
}

/*!
 * @brief Enable PDU software trigger.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_EnableSoftTrigger(PDU_T *const pduBase)
{
    pduBase->CSTS.bit.SWTRGSEL = PDU_CSTS_SWTRGSEL_1;
}

/*!
 * @brief Reads the PDU interrupt flag.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval Interrupt flag status.
 *           - true: interrupt flag enable
 *           - false: interrupt flag disable
 */
bool PDU_HW_ReadTimerIntFlag(PDU_T const *const pduBase)
{
    return (PDU_CSTS_IFLG_1 == (pduBase->CSTS.bit.IFLG)) ? true : false;
}

/*!
 * @brief Clears the interrupt flag.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_ClearTimerIntFlag(PDU_T * const pduBase)
{
    volatile uint32_t dummyRead = 0;
    pduBase->CSTS.bit.IFLG = PDU_CSTS_IFLG_0;

    dummyRead = pduBase->CSTS.reg;
    (void)dummyRead;
}

/*!
 * @brief Loads the delay registers value for the PDU module.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_ConfigLoadValuesCmd(PDU_T *const pduBase)
{
    pduBase->CSTS.bit.LDSUS = PDU_CSTS_LDSUS_1;
}

/*!
 * @brief Configures PDU period count value.
 *
 * @param pduBase: PDU instance base address.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_HW_ConfigPeriodCountValue(PDU_T *const pduBase, uint16_t value)
{
    pduBase->CNTPRD.bit.CNTPRD = (uint32_t)value;
}

/*!
 * @brief Read the PDU counter value of PDU timer.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval PDU module current counter value.
 */
uint32_t PDU_HW_ReadTimerValue(PDU_T const *const pduBase)
{
    return pduBase->CNTCUR.bit.CNTCUR;
}

/*!
 * @brief Configures timer interrupt delay value.
 *
 * @param pduBase: PDU instance base address.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_HW_ConfigTimerIntDelayValue(PDU_T *const pduBase, uint16_t value)
{
    pduBase->SINTDLY.bit.SINTDLY = (uint32_t)value;
}

/*!
 * @brief Reads the ADC pre-trigger flag in the PDU module.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval Return ADC pre-trigger flag.
 */
uint32_t PDU_HW_ReadAdcPreTriggerFlags(
    PDU_T const *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    return (preChnMask & (pduBase->CH[channel].STS.bit.CHFLG));
}

/*!
 * @brief Clears the ADC pre-trigger flag in the PDU module.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval None
 */
void PDU_HW_ClearAdcPreTriggerFlags(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    pduBase->CH[channel].STS.bit.CHFLG &= ~preChnMask;
}

/*!
 * @brief Reads the ADC pre-trigger sequence error flag in the PDU module.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval Return ADC pre-trigger sequence error flag.
 */
uint32_t PDU_HW_ReadAdcPreTriggerSeqErrFlags(
    PDU_T const *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    return (preChnMask & (pduBase->CH[channel].STS.bit.CHPTEFLG));
}

/*!
 * @brief Clears the flag which indicates that a sequence error has been detected.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU channel.
 * @param preChnMask: ADC pre-trigger channels mask.
 *
 * @retval None
 */
void PDU_HW_ClearAdcPreTriggerSeqErrFlags(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChnMask)
{
    volatile uint32_t dummyRead = 0;

    pduBase->CH[channel].STS.bit.CHPTEFLG &= ~preChnMask;

    dummyRead = pduBase->CH[channel].STS.reg;
    (void)dummyRead;
}

/*!
 * @brief Configures the ADC pre-trigger delay value in the PDU module.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: ADC channel.
 * @param preChn: ADC pre_channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_HW_ConfigAdcPreTriggerDelayValue(
    PDU_T *const pduBase,
    const uint32_t channel,
    const uint32_t preChn,
    const uint32_t value)
{
    pduBase->CH[channel].D[preChn].bit.CHPTDLY = value;
}

/*!
 * @brief Enable/Disable the CMP pulse out in the PDU module.
 *
 * @param pduBase: PDU instance base address.
 * @param pulseChnMask: Pulse channel mask.
 * @param enable: Enable/Disable selection.
 *
 * @retval None
 */
void PDU_HW_ConfigCmpPulseOutEnable(
    PDU_T *const pduBase,
    const uint32_t pulseChnMask,
    const bool enable)
{
    if (!enable)
    {
        pduBase->POEN.bit.POEN &= ~pulseChnMask;
    }
    else
    {
        pduBase->POEN.bit.POEN |= pulseChnMask;
    }
}

/*!
 * @brief Configures the CMP pulse out delay value for high.
 *
 * @param pduBase: PDU instance base address.
 * @param pulseChannel: Pulse channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_HW_ConfigCmpPulseOutDelayForHigh(
    PDU_T *const pduBase,
    const uint32_t pulseChannel,
    const uint32_t value)
{
    pduBase->PODLY[pulseChannel].bit.PODLY1 = (uint16_t)value;
}

/*!
 * @brief Configures the CMP pulse out delay value for low.
 *
 * @param pduBase: PDU instance base address.
 * @param pulseChannel: Pulse channel.
 * @param value: Specified setting value.
 *
 * @retval None
 */
void PDU_HW_ConfigCmpPulseOutDelayForLow(
    PDU_T *const pduBase,
    const uint32_t pulseChannel,
    const uint32_t value)
{
    pduBase->PODLY[pulseChannel].bit.PODLY2 = (uint16_t)value;
}

#if FEATURE_PDU_WITH_INSTANCE_BACKTOBACK
/*!
 * @brief Enable PDU instance back to back mode between PDU0 CH0 and PDU1 CH0 pre-triggers
 *
 * @param None
 *
 * @retval None
 */
void PDU_HW_SIM_EnableInstanceBackToBack(void)
{
    SIM->CHIPCTRL.bit.PDUBBMSEL = SIM_CHIPCTRL_PDUBBMSEL_1;
}

/*!
 * @brief Disable PDU instance back to back mode between PDU0 CH0 and PDU1 CH0 pre-triggers
 *
 * @param None
 *
 * @retval None
 */
void PDU_HW_SIM_DisableInstanceBackToBack(void)
{
    SIM->CHIPCTRL.bit.PDUBBMSEL = SIM_CHIPCTRL_PDUBBMSEL_0;
}
#endif /* FEATURE_PDU_WITH_INSTANCE_BACKTOBACK */

/*!
 * @brief Resets the PDU registers to default state.
 *
 * @param pduBase: PDU instance base address.
 *
 * @retval None
 */
void PDU_HW_ResetRegister(PDU_T *const pduBase)
{
    uint32_t channel = 0U;
    uint32_t preChn = 0U;

    pduBase->CSTS.reg = 0U;
    pduBase->CSTS.bit.PDUEN = PDU_CSTS_PDUEN_1;
    pduBase->CNTPRD.reg  = 0xFFFFU;
    pduBase->SINTDLY.reg = 0xFFFFU;

    /* ADC trigger configuration */
    for (channel = 0U; channel < PDU_CH_COUNT; channel++)
    {
        pduBase->CH[channel].CTRL1.reg = 0U;
        pduBase->CH[channel].STS.reg   = 0U;
        for (preChn = 0U; preChn < PDU_CH_DELAY_COUNT; preChn++)
        {
            /* Config the ADC pre-trigger delay value is 0 */
            pduBase->CH[channel].D[preChn].bit.CHPTDLY = 0U;
        }
    }

    /* Pulse out trigger configuration */
    pduBase->POEN.reg = 0U;
    for (channel = 0U; channel < PDU_PODLY_COUNT; channel++)
    {
        pduBase->PODLY[channel].reg = 0U;
    }

    /* Load the setting value */
    PDU_HW_ConfigLoadValuesCmd(pduBase);
    pduBase->CSTS.bit.PDUEN = PDU_CSTS_PDUEN_0;
}

/*!
 * @brief Configure the PDU timer.
 *
 * @param pduBase: PDU instance base address.
 * @param timerConfig: Pointer to user configuration structure for the PDU driver.
 *
 * @retval None
 */
void PDU_HW_ConfigTimer(PDU_T *const pduBase, const PDU_TIMER_CONFIG_T *timerConfig)
{
    pduBase->CSTS.bit.CMODEN    = PDU_CSTS_CMODEN_0;
    pduBase->CSTS.bit.MULPSCCFG = PDU_CSTS_MULPSCCFG_00;
    pduBase->CSTS.bit.IEN       = PDU_CSTS_IEN_0;
    pduBase->CSTS.bit.TRGINTSEL = PDU_CSTS_TRGINTSEL_0000;
    pduBase->CSTS.bit.PSCDIVCFG = PDU_CSTS_PSCDIVCFG_000;
    pduBase->CSTS.bit.DMAEN     = PDU_CSTS_DMAEN_0;
    pduBase->CSTS.bit.SEQEIEN   = PDU_CSTS_SEQEIEN_0;
    pduBase->CSTS.bit.LDMODSEL  = PDU_CSTS_LDMODSEL_00;

    pduBase->CSTS.bit.LDMODSEL = (uint32_t)(timerConfig->loadValMode);
    if (timerConfig->seqErrIntEn)
    {
        pduBase->CSTS.bit.SEQEIEN = PDU_CSTS_SEQEIEN_1;
    }

    pduBase->CSTS.bit.PSCDIVCFG = (uint32_t)(timerConfig->clkPscDiv);
    pduBase->CSTS.bit.TRGINTSEL = (uint32_t)(timerConfig->triggerInput);
    pduBase->CSTS.bit.MULPSCCFG = (uint32_t)(timerConfig->clkPscMultFactor);
    if (timerConfig->continuousModeEn)
    {
        pduBase->CSTS.bit.CMODEN = PDU_CSTS_CMODEN_1;
    }
    if (timerConfig->intEn)
    {
        pduBase->CSTS.bit.IEN = PDU_CSTS_IEN_1;
    }
    if (timerConfig->dmaEn)
    {
        pduBase->CSTS.bit.DMAEN = PDU_CSTS_DMAEN_1;
    }
}

/*!
 * @brief Switches to enable the pre-trigger back-to-back mode.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU instance index for trigger.
 * @param preChnMask: ADC channel group index mask for trigger.
 * @param enable: Enable/Disable selection.
 *
 * @retval None
 */
void PDU_HW_EnableAdcPreTriggerBackToBackMode(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable)
{
    if (!enable)
    {
        pduBase->CH[channel].CTRL1.bit.PTBBEN &= ~preChnMask;
    }
    else
    {
        pduBase->CH[channel].CTRL1.bit.PTBBEN |= preChnMask;
    }
}

/*!
 * @brief Switches to enable the pre-trigger output.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU instance index for trigger.
 * @param preChnMask: ADC channel group index mask for trigger.
 * @param enable: Enable/Disable selection.
 *
 * @retval None
 */
void PDU_HW_EnableAdcPreTriggerOutput(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable)
{
    if (!enable)
    {
        pduBase->CH[channel].CTRL1.bit.CHPTOSEL &= ~preChnMask;
    }
    else
    {
        pduBase->CH[channel].CTRL1.bit.CHPTOSEL |= preChnMask;
    }
}

/*!
 * @brief Switches to enable the pre-trigger.
 *
 * @param pduBase: PDU instance base address.
 * @param channel: PDU instance index for trigger.
 * @param preChnMask: ADC channel group index mask for trigger.
 * @param enable: Enable/Disable selection.
 *
 * @retval None
 */
void PDU_HW_EnableAdcPreTrigger(
    PDU_T *const pduBase,
    uint32_t channel,
    uint32_t preChnMask,
    bool enable)
{
    if (!enable)
    {
        pduBase->CH[channel].CTRL1.bit.CHPTOEN &= ~preChnMask;
    }
    else
    {
        pduBase->CH[channel].CTRL1.bit.CHPTOEN |= preChnMask;
    }
}

/**@} end of group PDU_Functions*/
/**@} end of group PDU_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
