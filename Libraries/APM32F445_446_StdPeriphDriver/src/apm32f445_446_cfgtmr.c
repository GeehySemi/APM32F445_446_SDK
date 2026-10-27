/*!
 * @file        apm32f445_446_cfgtmr.c
 *
 * @brief       This file provides all the CFGTMR firmware functions
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
#include "apm32f445_446_cfgtmr.h"
#include "apm32f445_446_clock.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGTMR_Driver CFGTMR Driver
  @{
*/

/** @defgroup CFGTMR_Macros Macros
  @{
*/
#if defined (APM32F445)
/* CFGTMR peripheral IRQn Macros */
#define CFGTMR_IRQS            { { CFGTMR0_Ch0_Ch1_IRQn, CFGTMR0_Ch0_Ch1_IRQn, CFGTMR0_Ch2_Ch3_IRQn, CFGTMR0_Ch2_Ch3_IRQn, \
                                 CFGTMR0_Ch4_Ch5_IRQn, CFGTMR0_Ch4_Ch5_IRQn, CFGTMR0_Ch6_Ch7_IRQn, CFGTMR0_Ch6_Ch7_IRQn }, \
                               { CFGTMR1_Ch0_Ch1_IRQn, CFGTMR1_Ch0_Ch1_IRQn, CFGTMR1_Ch2_Ch3_IRQn, CFGTMR1_Ch2_Ch3_IRQn, \
                                 CFGTMR1_Ch4_Ch5_IRQn, CFGTMR1_Ch4_Ch5_IRQn, CFGTMR1_Ch6_Ch7_IRQn, CFGTMR1_Ch6_Ch7_IRQn }, \
                               { CFGTMR2_Ch0_Ch1_IRQn, CFGTMR2_Ch0_Ch1_IRQn, CFGTMR2_Ch2_Ch3_IRQn, CFGTMR2_Ch2_Ch3_IRQn, \
                                 CFGTMR2_Ch4_Ch5_IRQn, CFGTMR2_Ch4_Ch5_IRQn, CFGTMR2_Ch6_Ch7_IRQn, CFGTMR2_Ch6_Ch7_IRQn }, \
                               { CFGTMR3_Ch0_Ch1_IRQn, CFGTMR3_Ch0_Ch1_IRQn, CFGTMR3_Ch2_Ch3_IRQn, CFGTMR3_Ch2_Ch3_IRQn, \
                                 CFGTMR3_Ch4_Ch5_IRQn, CFGTMR3_Ch4_Ch5_IRQn, CFGTMR3_Ch6_Ch7_IRQn, CFGTMR3_Ch6_Ch7_IRQn } }


#define CFGTMR_Fault_IRQS      { CFGTMR0_Fault_IRQn, CFGTMR1_Fault_IRQn, CFGTMR2_Fault_IRQn, CFGTMR3_Fault_IRQn }
#define CFGTMR_Overflow_IRQS   { CFGTMR0_Ovf_Reload_IRQn, CFGTMR1_Ovf_Reload_IRQn, CFGTMR2_Ovf_Reload_IRQn, CFGTMR3_Ovf_Reload_IRQn }
#define CFGTMR_Reload_IRQS     { CFGTMR0_Ovf_Reload_IRQn, CFGTMR1_Ovf_Reload_IRQn, CFGTMR2_Ovf_Reload_IRQn, CFGTMR3_Ovf_Reload_IRQn }

/* Table of CFGTMR base address */
#define CFGTMR_BASE_PTRS       { CFGTMR0, CFGTMR1, CFGTMR2, CFGTMR3 }

#elif defined (APM32F446)
/* CFGTMR peripheral IRQn Macros */
#define CFGTMR_IRQS            { { CFGTMR0_Ch0_Ch1_IRQn, CFGTMR0_Ch0_Ch1_IRQn, CFGTMR0_Ch2_Ch3_IRQn, CFGTMR0_Ch2_Ch3_IRQn, \
                                 CFGTMR0_Ch4_Ch5_IRQn, CFGTMR0_Ch4_Ch5_IRQn, CFGTMR0_Ch6_Ch7_IRQn, CFGTMR0_Ch6_Ch7_IRQn }, \
                               { CFGTMR1_Ch0_Ch1_IRQn, CFGTMR1_Ch0_Ch1_IRQn, CFGTMR1_Ch2_Ch3_IRQn, CFGTMR1_Ch2_Ch3_IRQn, \
                                 CFGTMR1_Ch4_Ch5_IRQn, CFGTMR1_Ch4_Ch5_IRQn, CFGTMR1_Ch6_Ch7_IRQn, CFGTMR1_Ch6_Ch7_IRQn }, \
                               { CFGTMR2_Ch0_Ch1_IRQn, CFGTMR2_Ch0_Ch1_IRQn, CFGTMR2_Ch2_Ch3_IRQn, CFGTMR2_Ch2_Ch3_IRQn, \
                                 CFGTMR2_Ch4_Ch5_IRQn, CFGTMR2_Ch4_Ch5_IRQn, CFGTMR2_Ch6_Ch7_IRQn, CFGTMR2_Ch6_Ch7_IRQn }, \
                               { CFGTMR3_Ch0_Ch1_IRQn, CFGTMR3_Ch0_Ch1_IRQn, CFGTMR3_Ch2_Ch3_IRQn, CFGTMR3_Ch2_Ch3_IRQn, \
                                 CFGTMR3_Ch4_Ch5_IRQn, CFGTMR3_Ch4_Ch5_IRQn, CFGTMR3_Ch6_Ch7_IRQn, CFGTMR3_Ch6_Ch7_IRQn }, \
                                { CFGTMR4_Ch0_Ch1_IRQn, CFGTMR4_Ch0_Ch1_IRQn, CFGTMR4_Ch2_Ch3_IRQn, CFGTMR4_Ch2_Ch3_IRQn, \
                                 CFGTMR4_Ch4_Ch5_IRQn, CFGTMR4_Ch4_Ch5_IRQn, CFGTMR4_Ch6_Ch7_IRQn, CFGTMR4_Ch6_Ch7_IRQn }, \
                                { CFGTMR5_Ch0_Ch1_IRQn, CFGTMR5_Ch0_Ch1_IRQn, CFGTMR5_Ch2_Ch3_IRQn, CFGTMR5_Ch2_Ch3_IRQn, \
                                 CFGTMR5_Ch4_Ch5_IRQn, CFGTMR5_Ch4_Ch5_IRQn, CFGTMR5_Ch6_Ch7_IRQn, CFGTMR5_Ch6_Ch7_IRQn } }


#define CFGTMR_Fault_IRQS      { CFGTMR0_Fault_IRQn, CFGTMR1_Fault_IRQn, CFGTMR2_Fault_IRQn, CFGTMR3_Fault_IRQn, CFGTMR4_Fault_IRQn, CFGTMR5_Fault_IRQn }
#define CFGTMR_Overflow_IRQS   { CFGTMR0_Ovf_Reload_IRQn, CFGTMR1_Ovf_Reload_IRQn, CFGTMR2_Ovf_Reload_IRQn, CFGTMR3_Ovf_Reload_IRQn, CFGTMR4_Ovf_Reload_IRQn, CFGTMR5_Ovf_Reload_IRQn }
#define CFGTMR_Reload_IRQS     { CFGTMR0_Ovf_Reload_IRQn, CFGTMR1_Ovf_Reload_IRQn, CFGTMR2_Ovf_Reload_IRQn, CFGTMR3_Ovf_Reload_IRQn, CFGTMR4_Ovf_Reload_IRQn, CFGTMR5_Ovf_Reload_IRQn }

/* Table of CFGTMR base address */
#define CFGTMR_BASE_PTRS       { CFGTMR0, CFGTMR1, CFGTMR2, CFGTMR3, CFGTMR4, CFGTMR5}
#endif
/**@} end of group CFGTMR_Macros*/

/** @defgroup CFGTMR_Variables Variables
  @{
*/

/* Table of CFGTMR base address */
static CFGTMR_T *const g_cfgtmrBaseAddress[CFGTMR_INSTANCE_COUNT] = CFGTMR_BASE_PTRS;

/* Interrupt vectors for the CFGTMR peripheral */
static const IRQn_Type g_cfgtmrIrqId[CFGTMR_INSTANCE_COUNT][FEATURE_CFGTMR_CHANNEL_NUM] = CFGTMR_IRQS;
static const IRQn_Type g_cfgtmrFaultIrqId[CFGTMR_INSTANCE_COUNT] = CFGTMR_Fault_IRQS;
static const IRQn_Type g_cfgtmrOverflowIrqId[CFGTMR_INSTANCE_COUNT] = CFGTMR_Overflow_IRQS;
static const IRQn_Type g_cfgtmrReloadIrqId[CFGTMR_INSTANCE_COUNT] = CFGTMR_Reload_IRQS;

/* Select external clock pin or clock source for peripheral */
#if defined (APM32F445)
static const CLOCK_NAMES_T g_cfgtmrExtClockSel[CFGTMR_INSTANCE_COUNT] = { CFGTMR0_CLK, CFGTMR1_CLK, CFGTMR2_CLK, CFGTMR3_CLK};
#elif defined (APM32F446)
static const CLOCK_NAMES_T g_cfgtmrExtClockSel[CFGTMR_INSTANCE_COUNT] = { CFGTMR0_CLK, CFGTMR1_CLK, CFGTMR2_CLK, CFGTMR3_CLK, CFGTMR4_CLK, CFGTMR5_CLK};
#endif

/* Pointer to the CFGTMR peripheral runtime state structure */
static CFGTMR_STATE_T *g_cfgtmrStatusPtr[CFGTMR_INSTANCE_COUNT] = { NULL };

/**@} end of group CFGTMR_Variables*/

/** @defgroup CFGTMR_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static void CFGTMR_IRQHandler(uint32_t ins, uint8_t channelPair);
static void CFGTMR_InputCaptureHandler(uint32_t ins, uint8_t channelPair);

static STATUS_T CFGTMR_ConfigSignalMeasurementType(
    uint32_t ins,
    uint8_t channelNum,
    bool enableContinuousMode,
    CFGTMR_SIGNAL_MEASUREMENT_T measureType);

static void CFGTMR_ConfigPwmIndependentChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam);
static void CFGTMR_ConfigPwmCombinedChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam);
static STATUS_T CFGTMR_ConfigPwmDutyCycleChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam);

/*******************************************************************************
 *                          CFGTMR COMMON PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initializes the CFGTMR driver and get the clock frequency value
 * which select one of three possible clock sources for the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 * @param userConfig: CFGTMR_USER_CONFIG_T pointer
 * @param cfgtmrState: CFGTMR_STATE_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_Init(uint32_t ins, const CFGTMR_USER_CONFIG_T *userConfig, CFGTMR_STATE_T *cfgtmrState)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t index = 0U;
    STATUS_T returnStatus = STATUS_SUCCESS;

    /* Check if this ins is already initialized */
    if ((true == POINTER_IS_NULL(g_cfgtmrStatusPtr[ins])) && \
        ((userConfig->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_EXTERNALCLK) || \
         (userConfig->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_FIXEDCLK) || \
         (userConfig->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_SYSTEMCLK)))
    {
        /* Configure cfgtmr state structure. */
        cfgtmrState->cfgtmrPeriod = 0U;
        cfgtmrState->cfgtmrClkSrc = userConfig->cfgtmrClkSrc;
        cfgtmrState->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;
        g_cfgtmrStatusPtr[ins] = cfgtmrState;

        for (index = 0U; index < FEATURE_CFGTMR_CHANNEL_NUM; index++)
        {
            cfgtmrState->enableNotification[index] = false;
            cfgtmrState->measurementResults[index] = 0U;
            cfgtmrState->channelsCallbacks[index] = NULL;
            cfgtmrState->channelsCallbacksParams[index] = NULL;
        }

        /* The reset operation doesn't care about write protection. CFGTMR_Reset will
         * disable this protection.*/
        CFGTMR_HW_Reset(cfgtmrBaseAddress);
        CFGTMR_HW_ConfigModule(cfgtmrBaseAddress, userConfig->cfgtmrPsc);
        /* Get clock name used to configure the CFGTMR module */
        cfgtmrState->cfgtmrClkSrcFreq = CFGTMR_ReadFrequency(ins);
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    if (STATUS_SUCCESS == returnStatus)
    {
        /* Check if the mode operation in PWM mode */
        switch (userConfig->cfgtmrMode)
        {
            case CFGTMR_MODE_EDGE_ALIGNED_PWM:
            case CFGTMR_MODE_CEN_ALIGNED_PWM:
            case CFGTMR_MODE_OUTPUT_COMPARE:
            case CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE:
                /* Configure sync for between registers and buffers */
                returnStatus = CFGTMR_ConfigSync(ins, &(userConfig->syncMethod));
                break;
            default:
                break;
        }

        /* Enable the generation of initialization trigger on chip module */
        CFGTMR_HW_ConfigInitTriggerCmd(cfgtmrBaseAddress, userConfig->isEnableInitTrg);
        CFGTMR_HW_ConfigBdmMode(cfgtmrBaseAddress, userConfig->BDMMode);

        /* Check if enable interrupt in counter mode */
        if (userConfig->isEnableInterrupt)
        {
            CFGTMR_HW_ConfigTimerOverflowInt(cfgtmrBaseAddress, true);
            INT_SYS_EnableIRQ(g_cfgtmrOverflowIrqId[ins]);
        }
        else
        {
            CFGTMR_HW_ConfigTimerOverflowInt(cfgtmrBaseAddress, false);
            INT_SYS_DisableIRQ(g_cfgtmrOverflowIrqId[ins]);
        }
    }

    return returnStatus;
}

/*!
 * @brief De-Initializes the CFGTMR driver.
 *
 * @details This function will disables the CFGTMR module. The CFGTMR_Init function must
 * be called before this function can be called.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_Deinit(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    /* Reset all CFGTMR register */
    CFGTMR_HW_Reset(cfgtmrBaseAddress);
    g_cfgtmrStatusPtr[ins] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Read the default configuration values in the structure which is used as a common use-case.
 *
 * @param cfg: CFGTMR_USER_CONFIG_T pointer.
 *
 * @retval None.
 */
void CFGTMR_DefaultConfig(CFGTMR_USER_CONFIG_T * const cfg)
{
    cfg->syncMethod.hwSync0          = false;
    cfg->syncMethod.hwSync1          = false;
    cfg->syncMethod.hwSync2          = false;
    cfg->syncMethod.softSync         = true;
    cfg->syncMethod.minLoadingPoint  = false;
    cfg->syncMethod.maxLoadingPoint  = false;
    cfg->syncMethod.initCounterSync  = CFGTMR_SYSTEM_CLOCK;
    cfg->syncMethod.inverterSync     = CFGTMR_SYSTEM_CLOCK;
    cfg->syncMethod.maskRegSync      = CFGTMR_SYSTEM_CLOCK;
    cfg->syncMethod.outRegSync       = CFGTMR_SYSTEM_CLOCK;
    cfg->syncMethod.syncPoint        = CFGTMR_UPDATE_NOW;
    cfg->syncMethod.autoClrTrigger   = true;
    cfg->cfgtmrMode                    = CFGTMR_MODE_EDGE_ALIGNED_PWM;
    cfg->cfgtmrClkSrc                  = CFGTMR_CLOCK_SRC_SYSTEMCLK;
    cfg->cfgtmrPsc                     = CFGTMR_CLOCK_DIV_1;
    cfg->BDMMode                     = CFGTMR_BDM_MODE_11;
    cfg->isEnableInitTrg           = false;
    cfg->isEnableInterrupt             = false;
}

/*!
 * @brief Mask the output of the channels and at match events will be ignored by the masked channels.
 *
 * @param ins: CFGTMR instance number
 * @param channelsMask: Channel mask. The selected channel will ignore match events.
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_MaskOutputChannels(uint32_t ins, uint32_t channelsMask, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigOutmaskReg(cfgtmrBaseAddress, channelsMask);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the counter initial value. The counter will get this value after an overflow event.
 *
 * @param ins: CFGTMR instance number
 * @param counterVal: Initial value of counter
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigCounterInitailValue(uint32_t ins, uint16_t counterVal, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, counterVal);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the value of the counter which will generates an reload point.
 *
 * @param ins: CFGTMR instance number
 * @param reloadPoint: The value for the half cycle reload register
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigHalfCycleReloadPoint(uint32_t ins, uint16_t reloadPoint, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigHalfCycleValue(cfgtmrBaseAddress, reloadPoint);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config to force the output value of a channel to a specific value.
 *
 * @details Before using this function it's mandatory to mask the match events using
 * CFGTMR_MaskOutputChannels and to enable software output control using
 * CFGTMR_ConfigSoftwareOutputChannelControl.
 *
 * @param ins: CFGTMR instance number
 * @param channelsValues: The channel software output control value
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigSoftOutChnValue(uint32_t ins, uint8_t channelsValues, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    CFGTMR_HW_ConfigAllChnSoftwareCtrlVal(cfgtmrBaseAddress, channelsValues ^ (uint8_t)cfgtmrBaseAddress->CHPOL.reg);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config which output channel can be software controlled.
 *
 * @param ins: CFGTMR instance number
 * @param channelsMask: The mask which will configure the channels which can be software controlled
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigSoftwareOutputChannelControl(uint32_t ins, uint8_t channelsMask, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    CFGTMR_HW_ConfigAllChnSoftwareCtrlCmd(cfgtmrBaseAddress, channelsMask);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config to control list of channels by software to force the output to specified value.
 *
 * @details  Despite the odd channels are configured as HIGH/LOW, they will be inverted in the following
 * configuration: COMP bit = 1 and CHSWOCTRLVSEL(n) and CHSWOCTRLVSEL(n+1) are HIGH. Please check Software
 * output control behavior chapter from RM.
 *
 * @param ins: CFGTMR instance number.
 * @param chnMask: The mask which will configure the channels which can be software controlled.
 * @param chnValueMask: The values which will be software configured for channels.
 * @param softTrigger: Enable/Disable software trigger.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigAllChnSoftwareOutputControl(
    uint32_t ins,
    uint8_t chnMask,
    uint8_t chnValueMask,
    bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint16_t value = 0U;

    value = (uint16_t)(((uint16_t)chnValueMask ^ (uint16_t)cfgtmrBaseAddress->CHPOL.reg) << (uint16_t)8U) | (uint16_t)chnMask;

    /* Enable and force the software control of channels output */
    CFGTMR_HW_ConfigSoftwareOutputControl(cfgtmrBaseAddress, value);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config if the second channel of a pair will be inverted or not.
 *
 * @param ins: CFGTMR instance number
 * @param channelsPair: It will configure which channel pair will invert the second channel.
 * @param softTrigger: Enable/Disable software trigger.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigInvertingControl(uint32_t ins, uint8_t channelsPair, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigInvctrlReg(cfgtmrBaseAddress, channelsPair);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the maximum counter value.
 *
 * @param ins: CFGTMR instance number
 * @param counterVal: Maximum counter value, the value to be set to the timer modulo.
 * @param softTrigger: Enable/Disable software trigger.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigModuloCounterValue(uint32_t ins, uint16_t counterVal, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, counterVal);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the channel edge or level on the selection of the channel mode.
 *
 * @param ins: CFGTMR instance number
 * @param channel: Channel number
 * @param level: Value of the channel mode
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigOutputlevel(uint32_t ins, uint8_t channel, uint8_t level)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    /* Sets the channel edge or level selection */
    CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channel, level);

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the synchronization mechanism for some CFGTMR registers (MOD, INITV, HC, CHnV, OMASK, ICTRL, SWOCTRL).
 *
 * @details This function configure the synchronization for PWM register (CHnV, MOD, INITV, HC, OMASK).
 * If this function is used whit wrong parameters it's possible to generate wrong waveform. Registers
 * synchronization need to be configured for PWM and output compare mode.
 *
 * @param ins: CFGTMR instance number
 * @param pwmParam: CFGTMR_PWM_SYNC_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigSync(uint32_t ins, const CFGTMR_PWM_SYNC_T *pwmParam)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    bool hwSync = pwmParam->hwSync0 || pwmParam->hwSync1 || pwmParam->hwSync2;
    STATUS_T returnStatus = STATUS_SUCCESS;

    /* Software and hardware triggers are not allowed in the same time */
    if ((pwmParam->softSync && hwSync) || (false == (pwmParam->softSync || hwSync)))
    {
        returnStatus = STATUS_ERROR;
    }
    else if (false == pwmParam->softSync)
    {
        /* Configure sync for OMASK register */
        CFGTMR_HW_ConfigOutmaskHardwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for ICTRL register */
        CFGTMR_HW_ConfigInvctrlHardwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for SWOCTRL register */
        CFGTMR_HW_ConfigSwoctrlHardwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for MOD, HC, INITV, and CHnV registers */
        CFGTMR_HW_ConfigModCntinCvHardwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure synchronization method (waiting next loading point or now) */
        CFGTMR_HW_ConfigCounterHardwareSyncModeCmd(cfgtmrBaseAddress, (bool)pwmParam->syncPoint);
    }
    else
    {
        /* Configure sync for OMASK register */
        CFGTMR_HW_ConfigOutmaskSoftwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for ICTRL register */
        CFGTMR_HW_ConfigInvctrlSoftwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for SWOCTRL register */
        CFGTMR_HW_ConfigSwoctrlSoftwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure sync for MOD, HC, INITV, and CHnV registers */
        CFGTMR_HW_ConfigModCntinCvSoftwareSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure synchronization method (waiting next loading point or now) */
        CFGTMR_HW_ConfigCounterSoftwareSyncModeCmd(cfgtmrBaseAddress, pwmParam->syncPoint);
    }

    if (STATUS_ERROR != returnStatus)
    {
        /* Enhanced PWM sync is used */
        CFGTMR_HW_ConfigPwmSyncModeCmd(cfgtmrBaseAddress, true);
        /* Configure trigger source for sync */
        CFGTMR_HW_ConfigHardwareSyncTriggerSrc(cfgtmrBaseAddress, 2U, pwmParam->hwSync2);
        CFGTMR_HW_ConfigHardwareSyncTriggerSrc(cfgtmrBaseAddress, 1U, pwmParam->hwSync1);
        CFGTMR_HW_ConfigHardwareSyncTriggerSrc(cfgtmrBaseAddress, 0U, pwmParam->hwSync0);
        /* Configure loading points */
        CFGTMR_HW_ConfigMinLoadingCmd(cfgtmrBaseAddress, pwmParam->minLoadingPoint);
        CFGTMR_HW_ConfigMaxLoadingCmd(cfgtmrBaseAddress, pwmParam->maxLoadingPoint);
        /* Configure sync for OMASK register */
        CFGTMR_HW_ConfigOutmaskPwmSyncModeCmd(cfgtmrBaseAddress, (bool)pwmParam->maskRegSync);
        /* Configure sync for ICTRL register */
        CFGTMR_HW_ConfigInvctrlPwmSyncModeCmd(cfgtmrBaseAddress, pwmParam->inverterSync);
        /* Configure sync for SWOCTRL register */
        CFGTMR_HW_ConfigSwoctrlPwmSyncModeCmd(cfgtmrBaseAddress, pwmParam->outRegSync);
        /* Configure sync for MOD, HC, INITV, and CHnV registers */
        CFGTMR_HW_ConfigCntinPwmSyncModeCmd(cfgtmrBaseAddress, pwmParam->initCounterSync);
        /* Configure if CFGTMR clears HWTnSEL (n=0,1,2) when the hardware trigger n is detected. */
        CFGTMR_HW_ConfigHwTriggerSyncModeCmd(cfgtmrBaseAddress, pwmParam->autoClrTrigger);
    }

    return returnStatus;
}

/*!
 * @brief Config a trigger source for CFGTMR instance.
 *
 * @details This allow a hardware trigger input which can be used in PWM synchronization.
 * Note that the hardware trigger is implemented only on trigger 1 for each instance.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_EnableHardwareTrigger(uint32_t ins)
{
    SIM_T *simeBaseAddress = SIM;

    CFGTMR_HW_ConfigSyncBit(simeBaseAddress, ins, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Enable specified CFGTMR interrupts.
 *
 * @param ins: CFGTMR instance number
 * @param interrupts: Specifies the CFGTMR interrupts sources. The parameter can be any combination
 * of members of the enumeration CFGTMR_INTERRUPT_T.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_EnableInterrupts(uint32_t ins, uint32_t interrupts)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint32_t channelInts = (interrupts & 0x000000FFU);

    for (uint8_t i = 0U; i < 3U; i++)
    {
        switch (i)
        {
            case 0:
                /* Enable the fault interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_FAULT_INT))
                {
                    CFGTMR_HW_ConfigFaultInt(cfgtmrBaseAddress, true);
                    INT_SYS_EnableIRQ(g_cfgtmrFaultIrqId[ins]);
                }
                break;
            case 1:
                /* Enable the timer overflow interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_TIME_OVERFLOW_INT))
                {
                    CFGTMR_HW_ConfigTimerOverflowInt(cfgtmrBaseAddress, true);
                    INT_SYS_EnableIRQ(g_cfgtmrOverflowIrqId[ins]);
                }
                break;
            case 2:
                /* Enable the reload interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_RELOAD_INT))
                {
                    CFGTMR_HW_ConfigReIntCmd(cfgtmrBaseAddress, true);
                    INT_SYS_EnableIRQ(g_cfgtmrReloadIrqId[ins]);
                }
                break;
            default:
                break;
        }
    }

    /* Enable the channel interrupts */
    for (uint8_t channel = 0U; channelInts != 0U; channel++)
    {
        if (0x0U != (channelInts & 0x1U))
        {
            CFGTMR_HW_EnableChnInt(cfgtmrBaseAddress, channel);
            INT_SYS_EnableIRQ(g_cfgtmrIrqId[ins][channel]);
        }
        channelInts = channelInts >> 1U;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable specified CFGTMR interrupts.
 *
 * @param ins: CFGTMR instance number
 * @param interrupts: Specifies the CFGTMR interrupts sources. The parameter can be any combination
 * of members of the enumeration CFGTMR_INTERRUPT_T.
 *
 * @retval None.
 */
void CFGTMR_DisableInterrupts(uint32_t ins, uint32_t interrupts)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint32_t channelInts = (interrupts & 0x000000FFU);

    for (uint8_t i = 0U; i < 3U; i++)
    {
        switch (i)
        {
            case 0:
                /* Disable the fault interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_FAULT_INT))
                {
                    CFGTMR_HW_ConfigFaultInt(cfgtmrBaseAddress, false);
                    INT_SYS_DisableIRQ(g_cfgtmrFaultIrqId[ins]);
                }
                break;
            case 1:
                /* Disable the timer overflow interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_TIME_OVERFLOW_INT))
                {
                    CFGTMR_HW_ConfigTimerOverflowInt(cfgtmrBaseAddress, false);
                    INT_SYS_DisableIRQ(g_cfgtmrOverflowIrqId[ins]);
                }
                break;
            case 2:
                /* Disable the reload interrupt */
                if (0x0U != (interrupts & (uint32_t)CFGTMR_RELOAD_INT))
                {
                    CFGTMR_HW_ConfigReIntCmd(cfgtmrBaseAddress, false);
                    INT_SYS_DisableIRQ(g_cfgtmrReloadIrqId[ins]);
                }
                break;
            default:
                break;
        }
    }
    /* Disable the channel interrupts */
    for (uint8_t channel = 0U; channelInts != 0U; channel++)
    {
        if (0x0U != (channelInts & 0x1U))
        {
            CFGTMR_HW_DisableChnInt(cfgtmrBaseAddress, channel);
            INT_SYS_DisableIRQ(g_cfgtmrIrqId[ins][channel]);
        }
        channelInts = channelInts >> 1U;
    }
}

/*!
 * @brief Read the enabled CFGTMR interrupts.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Enabled interrupts Value.
 *         It can be the value of any members of the CFGTMR_INTERRUPT_T enumeration type.
 */
uint32_t CFGTMR_ReadEnabledInterrupts(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T const * cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint32_t interrupts = 0U;

    for (uint8_t i = 0U; i < 3U; i++)
    {
        switch (i)
        {
            case 0:
                /* Check if fault interrupt is enabled */
                if (false != CFGTMR_HW_ReadFaultIntStatus(cfgtmrBaseAddress))
                {
                    interrupts |= (uint32_t)CFGTMR_FAULT_INT;
                }
                break;
            case 1:
                /* Check if timer overflow interrupt is enabled */
                if (false != CFGTMR_HW_ReadOverflowIntStatus(cfgtmrBaseAddress))
                {
                    interrupts |= (uint32_t)CFGTMR_TIME_OVERFLOW_INT;
                }
                break;
            case 2:
                /* Check if the reload interrupt is enabled */
                if (false != CFGTMR_HW_ReadReloadIntStatus(cfgtmrBaseAddress))
                {
                    interrupts |= (uint32_t)CFGTMR_RELOAD_INT;
                }
                break;
            default:
                break;
        }
    }

    /* Check if the channel interrupts are enabled */
    for (uint8_t channel = FEATURE_CFGTMR_CHANNEL_NUM; channel > 0U; channel--)
    {
        if (false != CFGTMR_HW_ReadChnIntStatus(cfgtmrBaseAddress, channel))
        {
            interrupts |= (1UL << (uint32_t)channel);
        }
    }

    return interrupts;
}

/*!
 * @brief Read the CFGTMR status flags.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval CFGTMR status flags.
 *         It can be the value of any members of the CFGTMR_FLAG_T enumeration type.
 */
uint32_t CFGTMR_ReadStatusFlags(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T const * cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint32_t retStatusFlags = 0U;

    for (uint8_t i = 0U; i < 4U; i++)
    {
        switch (i)
        {
            case 0:
                /* Check fault flag */
                if (false != CFGTMR_HW_ReadDetectedFaultInput(cfgtmrBaseAddress))
                {
                    retStatusFlags |= (uint32_t)CFGTMR_FAULT_FLAG;
                }
                break;
            case 1:
                /* Check the timer flag */
                if (false != CFGTMR_HW_ReadTimerOverflowedStatus(cfgtmrBaseAddress))
                {
                    retStatusFlags |= (uint32_t)CFGTMR_TIME_OVERFLOW_FLAG;
                }
                break;
            case 2:
                /* Check reload flag */
                if (false != CFGTMR_HW_ReadReloadFlag(cfgtmrBaseAddress))
                {
                    retStatusFlags |= (uint32_t)CFGTMR_RELOAD_FLAG;
                }
                break;
            case 3:
                /* Check channel trigger flag */
                if (false != CFGTMR_HW_ReadChnTriggerGenerated(cfgtmrBaseAddress))
                {
                    retStatusFlags |= (uint32_t)CFGTMR_CHANNEL_TRG_FLAG;
                }
                break;
            default:
                break;
        }
    }

    /* Lower 8 bits contain the channel status flags */
    for (uint8_t channel = 0U; channel < FEATURE_CFGTMR_CHANNEL_NUM; channel++)
    {
        if (false != CFGTMR_HW_ReadChnEventHasOccurred(cfgtmrBaseAddress, channel))
        {
            retStatusFlags |= (1UL << (uint32_t)channel);
        }
    }

    return retStatusFlags;
}

/*!
 * @brief Clear the CFGTMR status flags.
 *
 * @param ins: CFGTMR instance number
 * @param flags: Specifies the CFGTMR status flag. The parameter can be any combination
 * of members of the enumeration CFGTMR_FLAG_T.
 *
 * @retval None.
 */
void CFGTMR_ClearStatusFlags(uint32_t ins, uint32_t flg)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint32_t channelValue = (flg & 0x000000FFU);

    for (uint8_t i = 0U; i < 4U; i++)
    {
        switch (i)
        {
            case 0:
                /* Clear fault flag by writing a 0 to the bit while it is set */
                if (0x0U != (flg & (uint32_t)CFGTMR_FAULT_FLAG))
                {
                    CFGTMR_HW_ClearFaultsIsr(cfgtmrBaseAddress);
                }
                break;
            case 1:
                /* Clear the timer overflow flag by writing a 0 to the bit while it is set */
                if (0x0U != (flg & (uint32_t)CFGTMR_TIME_OVERFLOW_FLAG))
                {
                    CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
                }
                break;
            case 2:
                /* Check reload flag by writing a 0 to the bit while it is set */
                if (0x0U != (flg & (uint32_t)CFGTMR_RELOAD_FLAG))
                {
                    CFGTMR_HW_ClearReloadFlag(cfgtmrBaseAddress);
                }
                break;
            case 3:
                /* Clear channel trigger flag */
                if (0x0U != (flg & (uint32_t)CFGTMR_CHANNEL_TRG_FLAG))
                {
                    CFGTMR_HW_ClearChnTriggerFlag(cfgtmrBaseAddress);
                }
                break;
            default:
                break;
        }
    }

    /* Clear the channel status flags by writing a 0 to the bit */
    for (uint8_t channelID = 0U; channelID < FEATURE_CFGTMR_CHANNEL_NUM; channelID++)
    {
        if (0x0U != (channelValue & 0x00000001U))
        {
            CFGTMR_HW_ClearChnEventStatus(cfgtmrBaseAddress, channelID);
        }
        channelValue = channelValue >> 1U;
    }
}

/*!
 * @brief Read the frequency of the clock source feeding the CFGTMR counter.
 *
 * @details Function will return a 0 if no clock source is selected and the CFGTMR counter is disabled.
 * The returned value is clock sources for the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval The frequency of the clock source running the CFGTMR counter.
 */
uint32_t CFGTMR_ReadFrequency(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T const *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    const CFGTMR_STATE_T *cfgtmrState = g_cfgtmrStatusPtr[ins];
    uint8_t clkPsc = (uint8_t)(1U << CFGTMR_HW_ReadClockPs(cfgtmrBaseAddress));
    uint32_t freq = 0U;
    STATUS_T statusCode = STATUS_SUCCESS;

    if (cfgtmrState->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_EXTERNALCLK)
    {
        statusCode = CLOCK_SYS_ReadFreq(g_cfgtmrExtClockSel[ins], &freq);
    }
    else if (cfgtmrState->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_FIXEDCLK)
    {
        /* Get the clock frequency value */
        statusCode = CLOCK_SYS_ReadFreq(SIM_RTCCLK_CLK, &freq);
    }
    else if (cfgtmrState->cfgtmrClkSrc == CFGTMR_CLOCK_SRC_SYSTEMCLK)
    {
        /* Get the clock frequency value */
        statusCode = CLOCK_SYS_ReadFreq(CORE_CLK, &freq);
    }

    (void)statusCode;

    return (uint32_t)(freq / clkPsc);
}

/*!
 * @brief Converts the input parameters representing frequency in Hz to a period value
 * in ticks needed by the hardware timer.
 *
 * @param ins: CFGTMR instance number
 * @param freqHz: Frequency value in Hz.
 *
 * @retval Period Ticks Value.
 */
uint16_t CFGTMR_ConvertFreqToPeriodTicks(uint32_t ins, uint32_t freqHz)
{
    uint32_t uCFGTMRhz;

    CFGTMR_INSTANCE_VALIDITY(ins);

    const CFGTMR_STATE_T *status = g_cfgtmrStatusPtr[ins];
    uCFGTMRhz = status->cfgtmrClkSrcFreq;

    return (uint16_t)(uCFGTMRhz / freqHz);
}

/*!
 * @brief Restart the counter to its initial counting value in the register.
 *
 * @param ins: CFGTMR instance number
 * @param softTrigger: Selects the software trigger or hardware trigger to update COUNT register.
 *          @arg true: select software trigger to update register
 *          @arg false: select hardware trigger and waiting an external trigger for updating register.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ResetCounter(uint32_t ins, bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    /* Updates the counter with its initial value */
    CFGTMR_HW_ConfigCounter(cfgtmrBaseAddress, 0U);
    /* Set a software trigger or waiting a hardware trigger */
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Init the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 * @param timer: CFGTMR_TIMER_PARAM_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_InitCounter(uint32_t ins, const CFGTMR_TIMER_PARAM_T *timer)
{
    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint8_t channelNum    = 0U;
    STATUS_T returnStatus = STATUS_SUCCESS;

    CFGTMR_INSTANCE_VALIDITY(ins);

    if ((false == POINTER_IS_NULL(cfgtmrStatus)) && (CFGTMR_MODE_NOT_INITIALIZED == cfgtmrStatus->cfgtmrMode))
    {
        /* Disable counter clock */
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
        /* Clear the overflow flag */
        CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
        /* Set counter initial and maximum values */
        CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, timer->initValue);
        CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, timer->finalValue);
        /* Disable the quadrature decoder mode */
        CFGTMR_HW_ConfigQuadDecoderCmd(cfgtmrBaseAddress, false);
        /* Use CFGTMR as counter, disable all the channels */
        for (channelNum = 0U; channelNum < FEATURE_CFGTMR_CHANNEL_NUM; channelNum++)
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, 0U);
        }

        /* Check the CFGTMR counter modes */
        if (CFGTMR_MODE_UP_DOWN_TIMER == timer->mode)
        {
            CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, true);
        }
        else if (CFGTMR_MODE_UP_TIMER == timer->mode)
        {
            CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, false);
        }
        else
        {}

        cfgtmrStatus->cfgtmrMode = timer->mode;
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief Starts the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_StartCounter(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    const CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    STATUS_T returnStatus = STATUS_SUCCESS;

    /* Check the clock source is available for CFGTMR counter */
    if(cfgtmrStatus->cfgtmrClkSrcFreq > 0U)
    {
        /* Enable counter clock */
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, cfgtmrStatus->cfgtmrClkSrc);
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief Stops the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_StopCounter(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    /* Stop the CFGTMR counter */
    CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);

    return STATUS_SUCCESS;
}

/*!
 * @brief Reads back the current value of the CFGTMR counter.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval The current value.
 */
uint32_t CFGTMR_ReadCounter(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T const *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    return CFGTMR_HW_ReadCounterValue(cfgtmrBaseAddress);
}

/*!
 * @brief Read the timer parameter default configuration values in timer mode.
 *
 * @param cfg: CFGTMR_TIMER_PARAM_T pointer.
 *
 * @retval None.
 */
void CFGTMR_ReadTimerDefaultConfig(CFGTMR_TIMER_PARAM_T * const cfg)
{
    cfg->initValue    = 0U;
    cfg->finalValue   = 65535U;
    cfg->mode         = CFGTMR_MODE_UP_TIMER;
}

/*******************************************************************************
 *                          CFGTMR PWM PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initialize duty cycle and frequency and starts outputting.
 *
 * @param ins: CFGTMR instance number
 * @param pwmParam: CFGTMR_PWM_PARAM_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_InitPWM(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint32_t tempInstance    = ins;
    uint8_t faultChannelID   = 0U;
    uint8_t faultChannelNum  = 0U;
    STATUS_T returnCode      = STATUS_SUCCESS;

    if ((false == POINTER_IS_NULL(cfgtmrStatus)) && (CFGTMR_MODE_NOT_INITIALIZED == cfgtmrStatus->cfgtmrMode))
    {
        /* Disable counter clock */
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
        /* Clear the overflow flag */
        CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
        /* Disable write protection */
        CFGTMR_HW_ConfigWriteProtectionCmd(cfgtmrBaseAddress, false);
        /* Configure CFGTMR mode */
        cfgtmrStatus->cfgtmrMode = pwmParam->mode;
        /* Configure independent PWM channels */
        CFGTMR_ConfigPwmIndependentChannel(ins, pwmParam);
        /* Configure combined PWM channels */
        CFGTMR_ConfigPwmCombinedChannel(ins, pwmParam);
        /* Set enable outputs to be set to Initial/default value */
        CFGTMR_HW_InitChnOutputCmd(cfgtmrBaseAddress, true);
        /* Enable faults (if faults were configured) */
        if ((pwmParam->pwmFaultCfg)->faultMode != CFGTMR_FAULT_CONTROL_DISABLED)
        {
            /* Configure PWM Output behavior */
            CFGTMR_HW_ConfigPwmFaultBehavior(cfgtmrBaseAddress, ((pwmParam->pwmFaultCfg)->outputPINStateOnFault) ? true : false);
            /* Configure fault filter value */
            CFGTMR_HW_ConfigFaultInputFilterVal(cfgtmrBaseAddress, ((pwmParam->pwmFaultCfg)->faultFilterVal));
            /* Check the CFGTMR instances */
            if (tempInstance > 3U)
            {
                faultChannelNum = (uint8_t)(FEATURE_CFGTMR_FAULT_CHANNELS >> 1U);
            }
            else
            {
                faultChannelNum = (uint8_t)FEATURE_CFGTMR_FAULT_CHANNELS;
            }

            for (faultChannelID = 0U; faultChannelID < faultChannelNum; faultChannelID++)
            {
                if (true == (pwmParam->pwmFaultCfg)->faultChnParam[faultChannelID].isFaultChannel)
                {
                    /* Enable fault channel */
                    CFGTMR_HW_ConfigFaultInputCmd(cfgtmrBaseAddress, faultChannelID, true);
                    /* Configure fault filter */
                    CFGTMR_HW_ConfigFaultInputFilterCmd(
                        cfgtmrBaseAddress,
                        faultChannelID,
                        ((pwmParam->pwmFaultCfg)->faultChnParam[faultChannelID].isFaultFilter) ? true : false);
                    /* Configure fault outputs */
                    CFGTMR_HW_ConfigChnFaultInputPolarityCmd(
                        cfgtmrBaseAddress, faultChannelID,
                        (((pwmParam->pwmFaultCfg)->faultChnParam[faultChannelID].faultPinPolarity  == CFGTMR_POLARITY_HIGH) ? true : false));
                }
            }

            if (true == ((pwmParam->pwmFaultCfg)->isPWMFaultInterrupt))
            {
                CFGTMR_HW_ConfigFaultInt(cfgtmrBaseAddress, true);
            }

            CFGTMR_HW_ConfigFaultControlMode(cfgtmrBaseAddress, (uint32_t)(pwmParam->pwmFaultCfg)->faultMode);
        }

        /* Configure PWM mode: edge or center aligned */
        CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, (pwmParam->mode == CFGTMR_MODE_CEN_ALIGNED_PWM) ? true : false);
        /* Calculate frequency of the give CFGTMR hardware module - all channels will run at the same frequency */
        cfgtmrStatus->cfgtmrPeriod = CFGTMR_ConvertFreqToPeriodTicks(ins, pwmParam->uFreqHZ);
        /* Based on reference manual, in PWM mode INITV is to be set 0 */
        CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, 0U);
        /* Write MOD register with the value of the period */
        if (pwmParam->mode != CFGTMR_MODE_CEN_ALIGNED_PWM)
        {
            CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, (uint16_t)(cfgtmrStatus->cfgtmrPeriod - 1U));
        }
        else
        {
            CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, (uint16_t)(cfgtmrStatus->cfgtmrPeriod >> 1U));
        }

        /* Update the duty cycle */
        returnCode = CFGTMR_ConfigPwmDutyCycleChannel(ins, pwmParam);

        if (STATUS_SUCCESS != returnCode)
        {
            /* Restore CFGTMR mode if initialization fails */
            cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;
        }
        else
        {
            /* Configure dead time for combine mode */
            CFGTMR_HW_ConfigDeadtimeCount(cfgtmrBaseAddress, pwmParam->deadTimeVal);
            CFGTMR_HW_ConfigDeadtimePrescale(cfgtmrBaseAddress, pwmParam->deadTimePsc);
            CFGTMR_HW_Enable(cfgtmrBaseAddress, true);
            CFGTMR_HW_ConfigPwmSyncMode(cfgtmrBaseAddress, true);
            /* Set clock source to start counter */
            CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, cfgtmrStatus->cfgtmrClkSrc);
        }
    }
    else
    {
        returnCode = STATUS_ERROR;
    }

    return returnCode;
}

/*!
 * @brief De-initialize all PWM channels.
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_DeInitPWM(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint8_t channel = 0U;
    uint8_t channelPairNum = 0U;

    /* Stop the CFGTMR counter */
    CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
    for (channel = 0U; channel < FEATURE_CFGTMR_CHANNEL_NUM; channel++)
    {
        channelPairNum = (uint8_t)(channel >> 1U);
        /* Disable PWM mode in hardware */
        CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, channel, 0U);
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channel, 0U);
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channel, 0U);
        CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, false);
        /* Configure polarity bit */
        CFGTMR_HW_ConfigChnOutputPolarityCmd(cfgtmrBaseAddress, channel, false);
        CFGTMR_HW_DisablePwmChannelOutputs(cfgtmrBaseAddress, channel);
        /* Clear the PWM synchronization */
        CFGTMR_HW_ConfigDualChnPwmSyncCmd(cfgtmrBaseAddress, channelPairNum, false);
        /* Clear combination for each pair of channels */
        CFGTMR_HW_ConfigDualChnMofCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualChnDeadtimeCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualChnFaultCmd(cfgtmrBaseAddress, channelPairNum, false);
    }

    /* Clear the dead-time pre-scaler and value */
    CFGTMR_HW_ConfigExtDeadtimeValue(cfgtmrBaseAddress, 0U);
    CFGTMR_HW_ConfigDeadtimePrescale(cfgtmrBaseAddress, CFGTMR_DEADTIME_DIV_1);
    CFGTMR_HW_ConfigDeadtimeCount(cfgtmrBaseAddress, 0U);
    /* Clear fault control register */
    CFGTMR_HW_ClearFaultControl(cfgtmrBaseAddress);
    /* Disable fault interrupt */
    CFGTMR_HW_ConfigFaultInt(cfgtmrBaseAddress, false);
    /* Disable fault control */
    CFGTMR_HW_ConfigFaultControlMode(cfgtmrBaseAddress, (uint32_t)CFGTMR_FAULT_CONTROL_DISABLED);
    /* Clear the module value of the registers */
    CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, 0U);
    CFGTMR_HW_ConfigCounter(cfgtmrBaseAddress, 0U);
    cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;

    return STATUS_SUCCESS;
}

/*!
 * @brief Update the duty cycle of PWM output.
 *
 * @param ins: CFGTMR instance number
 * @param channelID: The channel number. If in combined mode, this parameter can be ignored,
 *                 and the code finds the channel number.
 * @param updateType: PWM update type.
                        @arg CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE: The type of PWM update in duty cycle/pulse or also use in frequency update.
                        @arg CFGTMR_PWM_UPDATE_IN_TICKS: The type of PWM update in ticks
 * @param firstEdge: Duty cycle or first edge time for PWM mode. Can take value between
 *                   0 - CFGTMR_MAX_DUTY_CYCLE(0 = 0% from period  and CFGTMR_MAX_DUTY_CYCLE = 100% from period)
 *                   Or value in ticks for the first of the PWM mode in which can have value between 0
 *                   and cfgtmrPeriod is stored in the state structure.
 * @param secondEdge: Second edge time - only for combined mode. Can take value between
 *                    0 - CFGTMR_MAX_DUTY_CYCLE(0 = 0% from period  and CFGTMR_MAX_DUTY_CYCLE = 100% from period).
 *                    Or value in ticks for the second of the PWM mode in which can have value between 0
 *                    and cfgtmrPeriod is stored in the state structure.
 * @param softTrigger: Enable/Disable software trigger to update PWM parameters.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_UpdatePwmChannel(
    uint32_t ins,
    uint8_t channelID,
    CFGTMR_PWM_UPDATE_TYPE_T updateType,
    uint16_t firstEdge,
    uint16_t secondEdge,
    bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];

    uint16_t cfgtmrPeriod    = 0U;
    uint16_t hwFirstEdge   = 0U;
    uint16_t hwSecondEdge  = 0U;
    uint8_t channelPairNum = (uint8_t)(channelID >> 1U);
    STATUS_T returnStatus  = STATUS_SUCCESS;

    /* Read the newest period in the MOD register */
    cfgtmrPeriod = CFGTMR_HW_ReadMod(cfgtmrBaseAddress);
    /* Check the mode operation in CFGTMR module */
    switch (cfgtmrStatus->cfgtmrMode)
    {
        case CFGTMR_MODE_CEN_ALIGNED_PWM:
            cfgtmrPeriod = (uint16_t)(cfgtmrPeriod << 1U);
            break;
        case CFGTMR_MODE_EDGE_ALIGNED_PWM:
        case CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE:
            cfgtmrPeriod = (uint16_t)(cfgtmrPeriod + 1U);
            break;
        default:
            returnStatus = STATUS_ERROR;
            break;
    }

    /* Check the type of update for PWM */
    if (CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE != updateType)
    {
        if ((firstEdge <= cfgtmrPeriod) && (secondEdge <= cfgtmrPeriod))
        {
            hwFirstEdge = firstEdge;
            hwSecondEdge = secondEdge;
        }
        else
        {
            returnStatus = STATUS_ERROR;
        }
    }
    else
    {
        if ((firstEdge <= CFGTMR_MAX_DUTY_CYCLE) && (secondEdge <= CFGTMR_MAX_DUTY_CYCLE))
        {
            /* Calculate DutyCycle based of the previously calculated frequency*/
            /* For greater resolution the DutyCycle values are in the range [0. CFGTMR_MAX_DUTY_CYCLE]
             *  where 0 = 0% or PWM always at Low and CFGTMR_MAX_DUTY_CYCLE = 100% or PWM always HIGH;
             *  a value of 0x4000 is equivalent of 50% DutyCycle. */
            hwFirstEdge = (uint16_t)((cfgtmrPeriod * firstEdge) >> CFGTMR_DUTY_TO_TICKS_SHIFT);
            hwSecondEdge = (uint16_t)((cfgtmrPeriod * secondEdge) >> CFGTMR_DUTY_TO_TICKS_SHIFT);
            /* adjust DutyCycle if 100% value is to be achieved. */
            if (CFGTMR_MAX_DUTY_CYCLE == firstEdge)
            {
                /* If expected duty is 100% then increase by 1 the value that is to be written
                 *  to Hardware so it will exceed value of period */
                hwFirstEdge = (uint16_t)(hwFirstEdge + 1U);
            }
        }
        else
        {
            returnStatus = STATUS_ERROR;
        }
    }

    if (STATUS_SUCCESS == returnStatus)
    {
        if (false == CFGTMR_HW_ReadDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum))
        {
            /* Channel value is divided by 2 for up down counter mode to keep same duty */
            if (true == CFGTMR_HW_ReadCpwmsMode(cfgtmrBaseAddress))
            {
                CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, channelID, (uint16_t)(hwFirstEdge >> 1U));
            }
            else
            {
                CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, channelID, hwFirstEdge);
            }
        }
        else
        {
            if (false == CFGTMR_HW_ReadDualChnMofCombineCmd(cfgtmrBaseAddress, channelPairNum))
            {
                CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, (uint8_t)(channelPairNum * 2U), hwFirstEdge);
            }
            else
            {
                /* Check the clock source for CFGTMR counter is disabled or not */
                if (CFGTMR_HW_ReadClockSource(cfgtmrBaseAddress) == 0U)
                {
                    CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, (uint8_t)(channelPairNum * 2U), hwFirstEdge);
                }
            }

            /* Modify the initial value in the channel (n+1) match edge */
            CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, (uint8_t)((channelPairNum * 2U) + 1U), hwSecondEdge);
        }

        /* Software trigger is generated to change Channel Value registers */
        /* Before this please configure sync mechanism to use software trigger */
        CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

        /* Store the PWM period in the status structure */
        cfgtmrStatus->cfgtmrPeriod = cfgtmrPeriod;
    }

    return returnStatus;
}

/*!
 * @brief Config to update the new period in the frequency or in the counter value
 * into mode register which modify the period of PWM signal on the channel output.
 *
 * @details If the type of update in the duty cycle which is reused in CFGTMR_UpdatePwmChannel
 * function to convert the newValue parameters representing frequency in Hz to a period
 * value to update the MOD register. The newValue parameter must be value between 1U and
 * maximum is the frequency of the CFGTMR counter.
 * If the type of update in ticks, this function will get value in counting to
 * the MOD register. The newValue parameter must be value between 1U and 0xFFFFU
 *
 * @param ins: CFGTMR instance number
 * @param updateType: The type of PWM update is a period in Hz or in ticks.
 *                      @arg CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE: It which reuse in CFGTMR_UpdatePwmChannel function will update in Hz.
 *                      @arg CFGTMR_PWM_UPDATE_IN_TICKS: It will update in ticks.
 * @param newValue: The frequency or the counter value which will select with modified value for PWM signal.
 *                  If the type of update in the duty cycle, the newValue parameter must be value
 *                  between 1U and maximum is the frequency of the CFGTMR counter.
 *                  If the type of update in ticks, the newValue parameter must be value between 1U and 0xFFFFU.
 * @param softTrigger: Enable/Disable software trigger to update PWM parameters.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_UpdatePwmPeriod(
    uint32_t ins,
    CFGTMR_PWM_UPDATE_TYPE_T updateType,
    uint32_t newValue,
    bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];

    uint32_t cfgtmrPeriod   = 0U;
    STATUS_T returnStatus = STATUS_SUCCESS;

    /* Check the type of update for period in PWM mode */
    if (CFGTMR_PWM_UPDATE_IN_TICKS != updateType)
    {
        if (newValue <= cfgtmrStatus->cfgtmrClkSrcFreq)
        {
            cfgtmrPeriod = (uint32_t)CFGTMR_ConvertFreqToPeriodTicks(ins, newValue);
        }
        else
        {
            returnStatus = STATUS_ERROR;
        }
    }
    else
    {
        cfgtmrPeriod = newValue;
    }

    if (STATUS_SUCCESS == returnStatus)
    {
        if (CFGTMR_MODE_CEN_ALIGNED_PWM != cfgtmrStatus->cfgtmrMode)
        {
            cfgtmrPeriod = (cfgtmrPeriod - 1U);
        }
        else
        {
            cfgtmrPeriod = (cfgtmrPeriod >> 1U);
        }
        /* Store the PWM period in the status structure */
        cfgtmrStatus->cfgtmrPeriod = (uint16_t)cfgtmrPeriod;
        /* Set the new modulo value into MOD register */
        CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, (uint16_t)(cfgtmrPeriod));
        /* Software trigger is generated to change register SYN[SWTSEL] */
        CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);
    }

    return returnStatus;
}

/*!
 * @brief Config to update the duty cycle of PWM output for multiple channels.
 *
 * @details The main differences between this function and CFGTMR_UpdatePwmChannel is the execution speed.
 * This feature makes this function ideal for applications like motor controlling.
 * The downside is the low configuration of the parameters (this function accept only updates in ticks).
 *
 * @param ins: CFGTMR instance number
 * @param numOfChannels: The number of channels which should be updated.
 * @param channelIDArray: The list of channels which should be updated.
 * @param dutyArray: The list of duty cycles for selected channels.
 * @param softTrigger: Enable/Disable software trigger to update PWM parameters.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_UpdatePwmMultipleChannels(
    uint32_t ins,
    uint8_t numOfChannels,
    const uint8_t *channelIDArray,
    const uint16_t *dutyArray,
    bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t idx = 0U;

    for (idx = 0U; idx < numOfChannels; idx++)
    {
        ((cfgtmrBaseAddress)->CONTROLS[channelIDArray[idx]].CHnV.reg) = (uint32_t)dutyArray[idx];
    }

    if (softTrigger)
    {
       cfgtmrBaseAddress->SYN.bit.SWTSEL = CFGTMR_SYN_SWTSEL_1;
    }
    else
    {
        cfgtmrBaseAddress->SYN.bit.SWTSEL = CFGTMR_SYN_SWTSEL_0;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Control the channel output enable/disable.
 *
 * @param ins: CFGTMR instance number
 * @param channel: CFGTMR hardware channel number, it is used in PWM mode.
 * @param enable: Enable/Disable channel output.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ControlChannelOutput(uint32_t ins, uint8_t channel, bool enable)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    if (enable == false)
    {
        /* Disable the channel output */
        CFGTMR_HW_DisablePwmChannelOutputs(cfgtmrBaseAddress, channel);
    }
    else
    {
        /* Enable the channel output */
        CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, channel);
    }

    return STATUS_SUCCESS;
}

#if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
/*!
 * @brief Update the Pwm Period Dither.
 *
 * @param ins: CFGTMR instance number
 * @param newMMVVal: This value is added to an internal accumulator at the end of each PWM period.
 * @param isSoftwareTrig: software trigger or not
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_UpdatePwmPeriodDither(uint32_t ins, uint8_t newMMVVal, bool isSoftwareTrig)
{
    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigMMVVal(cfgtmrBaseAddress, newMMVVal);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, isSoftwareTrig);

    return STATUS_SUCCESS;
}

/*!
 * @brief Update the Pwm Edge Channel Dither.
 *
 * @param ins: CFGTMR instance number
 * @param channel: CFGTMR hardware channel number, it is used in PWM mode.
 * @param newMMVVal: This value is added to the channel (n) internal accumulator at the end of each PWM period.
 * @param isSoftwareTrig: software trigger or not
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_UpdatePwmEdgeChannelDither(uint32_t ins, uint8_t channel, uint8_t newMMVVal, bool isSoftwareTrig)
{
    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];

    CFGTMR_HW_ConfigMCHMVal(cfgtmrBaseAddress, channel, newMMVVal);
    CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, isSoftwareTrig);

    return STATUS_SUCCESS;
}
#endif

/*!
 * @brief The CFGTMR PWM IRQ handler.
 *
 * @details This function must be used in the CFGTMR Overflow Flag interrupt handler when a fault
 * is detected to  ensure that the outputs return to the value configured by CFGTMR_SWOCTRL.
 *
 * @param ins: CFGTMR instance number.
 * @param chnOutCtrlVal: Channel output control value. The value of some channel outputs
 *        are affected by software output control.
 *
 * @retval None.
 */
void CFGTMR_PWM_IRQHandler(uint32_t ins, uint32_t chnOutCtrlVal)
{
    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    static bool faultDetection = false;

    /* 1. Check the value of CFGTMR_FMSTS[FCDFLG] */
    if (CFGTMR_HW_ReadDetectedFaultInput(cfgtmrBaseAddress) == true)
    {
        faultDetection = true;
        /* 2. Write the CFGTMR_OMASK register to set the bit */
        CFGTMR_HW_ConfigOutmaskReg(cfgtmrBaseAddress, chnOutCtrlVal);
        /* 3. Clear fault conditions by reading the CFGTMR_FMSTS register and then writing CFGTMR_FMSTS with all zeroes */
        CFGTMR_HW_ClearFaultsIsr(cfgtmrBaseAddress);
        /* 4. Clear the CFGTMR_CSTS[CFGTMROFLG] bit by reading the CFGTMR_CSTS register, then writing a 0 to CFGTMR_CSTS[CFGTMROFLG] */
        CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
        /* 5. Exit the interrupt handler to skip following steps */
    }
    else if (faultDetection == true)
    {
        /* 6. Clear the CFGTMR_SWOCTRL by writing all zeroes to it */
        CFGTMR_HW_ConfigAllChnSoftwareCtrlCmd(cfgtmrBaseAddress, 0x00U);
        CFGTMR_HW_ConfigAllChnSoftwareCtrlVal(cfgtmrBaseAddress, 0x00U);
        /* 7. Write CFGTMR_SWOCTRL with the desired value again */
        uint8_t u8chnOutCtrlVal = (uint8_t)(chnOutCtrlVal & 0xFFu);
        CFGTMR_HW_ConfigAllChnSoftwareCtrlCmd(cfgtmrBaseAddress, u8chnOutCtrlVal);
        CFGTMR_HW_ConfigAllChnSoftwareCtrlVal(cfgtmrBaseAddress, u8chnOutCtrlVal);
        /* 8. Clear the CFGTMR_OMASK bits that were set in step 2 */
        CFGTMR_HW_ConfigOutmaskReg(cfgtmrBaseAddress, 0x00U);
        /* 9. Clear the fault variable that was set in step 1 when the fault condition was originally detected */
        faultDetection = false;
        /* 10. Clear the CFGTMR_CSTS[CFGTMROFLG] bit by reading the CFGTMR_CSTS register, then writing a 0 to CFGTMR_CSTS[CFGTMROFLG] */
        CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
    }
    else
    {}
}

/*******************************************************************************
 *                          CFGTMR OUTPUT COMPARE PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Config the CFGTMR to generate timed pulses in output mode.
 *
 * @param ins: CFGTMR instance number.
 * @param param: CFGTMR_OUTPUT_CMP_PARAM_T pointer.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_InitOutputCompare(uint32_t ins, const CFGTMR_OUTPUT_CMP_PARAM_T *param)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    STATUS_T returnStatus    = STATUS_SUCCESS;
    uint8_t hardwareChannel  = 0U;
    uint8_t channelPairNum   = 0U;
    uint8_t index            = 0U;

    if ((false == POINTER_IS_NULL(cfgtmrStatus)) && (CFGTMR_MODE_NOT_INITIALIZED == cfgtmrStatus->cfgtmrMode))
    {
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
        CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, false);
        /* Clear the overflow flag */
        CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
        CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, 0U);
        CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, param->maxCountVal);
        CFGTMR_HW_ConfigCounter(cfgtmrBaseAddress, 0U);
        CFGTMR_HW_ConfigQuadDecoderCmd(cfgtmrBaseAddress, false);
        if(param->numOfOutputChannels > 0)
        {
            /* Use CFGTMR as counter, disable all the channels */
            for (index = 0U; index < param->numOfOutputChannels; index++)
            {
                hardwareChannel = param->outputChnCfg[index].hwChannelId;
                channelPairNum =  (uint8_t)(hardwareChannel >> 1U);
                CFGTMR_HW_ConfigDualChnMofCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
                CFGTMR_HW_ConfigDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
                CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, false);
                /* Set Channel Output mode */
                CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, hardwareChannel, (uint8_t)(param->outputChnCfg[index].chnMatchMode));
                /* Enter counter mode for all configured channels */
                CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, hardwareChannel, 1U);
                /* Write initial count value for all channels */
                CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, hardwareChannel, param->outputChnCfg[index].cmpValue);
                /* Enable channel output */
                CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, hardwareChannel);
                /* Enable the generation a trigger on chip module */
                CFGTMR_HW_ConfigChnTriggerCmd(cfgtmrBaseAddress, hardwareChannel, param->outputChnCfg[index].isExternalTrigger);
            }

            /* Set software trigger */
            CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, true);
            cfgtmrStatus->cfgtmrMode = param->mode;
            /* Set clock source to start the counter */
            CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, cfgtmrStatus->cfgtmrClkSrc);
        }
        else
        {
            returnStatus = STATUS_ERROR;
        }
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief De-initialize compare match output control and clears CFGTMR timer configuration
 *
 * @param ins: CFGTMR instance number
 * @param param: CFGTMR_OUTPUT_CMP_PARAM_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_DeinitOutputCompare(uint32_t ins, const CFGTMR_OUTPUT_CMP_PARAM_T *param)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint8_t hardwareChannel  = 0U;
    uint8_t index = 0U;

    /* Stop the CFGTMR counter */
    CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
    /* Clear the overflow flag */
    CFGTMR_HW_ClearTimerOverflow(cfgtmrBaseAddress);
    CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, 0U);
    for (index = 0U; index < param->numOfOutputChannels; index++)
    {
        hardwareChannel = param->outputChnCfg[index].hwChannelId;
        /* Disable Channel Output mode */
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, hardwareChannel, (uint8_t)0U);
        /* Write initial count value for all channels to 0xFFFF */
        CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, hardwareChannel, 0U);
        /* Disable channel output */
        CFGTMR_HW_DisablePwmChannelOutputs(cfgtmrBaseAddress, hardwareChannel);
    }

    /* Clear out the registers */
    CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, 0U);
    CFGTMR_HW_ConfigCounter(cfgtmrBaseAddress, 0U);
    cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;

    return STATUS_SUCCESS;
}

/*!
 * @brief Config the next compare match value on the given channel starting from the current counter value.
 *
 * @param ins: CFGTMR instance number
 * @param channelID: the output compare channel number
 * @param nextCmpMatchValue: the next compare match value
 * @param updateType: CFGTMR input capture type of the next output compare value
 *                @arg: CFGTMR_RELATIVE_VALUE: it will be appended to current counter value.
 *                @arg: CFGTMR_ABSOLUTE_VALUE: it will be written in counter register as it is.
 * @param softTrigger: Enable/Disable software trigger
 *
 * @retval Status code
 */
STATUS_T CFGTMR_UpdateOutputCompareChannel(
    uint32_t ins,
    uint8_t channelID,
    uint16_t nextCmpMatchValue,
    CFGTMR_OC_UPDATE_TYPE_T updateType,
    bool softTrigger)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint16_t cmpValue = 0U;
    uint16_t cntValue = CFGTMR_HW_ReadCounterValue(cfgtmrBaseAddress);
    uint16_t maxCntValue = CFGTMR_HW_ReadMod(cfgtmrBaseAddress);
    STATUS_T returnStatus = STATUS_SUCCESS;

    if(nextCmpMatchValue <= maxCntValue)
    {
        if (updateType == CFGTMR_RELATIVE_VALUE)
        {
            /* Configure channel compare register */
            if (nextCmpMatchValue <= (maxCntValue - cntValue))
            {
                cmpValue = (uint16_t)(cntValue + nextCmpMatchValue);
            }
            else
            {
                cmpValue = (uint16_t)(nextCmpMatchValue - (maxCntValue - cntValue));
            }
        }
        else
        {
            cmpValue = nextCmpMatchValue;
        }

        /* Set CH(n)V value and use software trigger for sync */
        CFGTMR_HW_ConfigChnCountVal(cfgtmrBaseAddress, channelID, cmpValue);
        CFGTMR_HW_ConfigSoftwareTriggerCmd(cfgtmrBaseAddress, softTrigger);

        returnStatus =  STATUS_SUCCESS;
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*******************************************************************************
 *                          CFGTMR INPUT CAPTURE PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initialize input capture mode.
 *
 * @details Config the channel in the input capture mode for either getting
 * time-stamps on edge detection or on signal measurement. If the filterVal parameter
 * is 0, the filter is disable. The filter function is available only on channels 0/1/2/3.
 *
 * @param ins: CFGTMR instance number
 * @param param: CFGTMR_INPUT_PARAM_T pointer.
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_InitInputCapture(uint32_t ins, const CFGTMR_INPUT_PARAM_T *param)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    STATUS_T returnStatus    = STATUS_SUCCESS;
    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    CFGTMR_SIGNAL_MEASUREMENT_T measureType;

    uint8_t hardwareChannel = 0U;
    uint8_t channelPairNum  = 0U;
    uint8_t index           = 0U;
    bool enableContMode;

    if ((false == POINTER_IS_NULL(cfgtmrStatus)) && \
        ((CFGTMR_MODE_NOT_INITIALIZED == cfgtmrStatus->cfgtmrMode) || \
        (CFGTMR_MODE_EDGE_ALIGNED_PWM == cfgtmrStatus->cfgtmrMode) || \
        (CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE == cfgtmrStatus->cfgtmrMode)))
    {
        if ((CFGTMR_MODE_EDGE_ALIGNED_PWM == cfgtmrStatus->cfgtmrMode) || \
            (CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE == cfgtmrStatus->cfgtmrMode))
        {
            /* In this case clock source is configured from PWM init. */
            cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE;
        }
        else if (cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_NOT_INITIALIZED)
        {
            CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
            CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, 0U);
            CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, param->maxCountVal);
            CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, false);
        }

        /* Get curent MOD value */
        cfgtmrStatus->cfgtmrModVal = CFGTMR_HW_ReadMod(cfgtmrBaseAddress);

        for (index = 0U; index < param->numOfChannels; index++)
        {
            hardwareChannel = param->inputChnConfig[index].hwChannelId;
            channelPairNum =  (uint8_t)(hardwareChannel >> 1U);
            /* Save in status structure user define handlers */
            cfgtmrStatus->channelsCallbacks[hardwareChannel] = param->inputChnConfig[index].channelsCallbacks;
            cfgtmrStatus->channelsCallbacksParams[hardwareChannel] =  param->inputChnConfig[index].channelsCallbacksParams;
            cfgtmrStatus->enableNotification[hardwareChannel] = true;

            /* Enable filtering for input channels */
            if (hardwareChannel < CHAN4_IDX)
            {
                if (true == param->inputChnConfig[index].enableFilter)
                {
                    CFGTMR_HW_ConfigChnInputCaptureFilter(cfgtmrBaseAddress, hardwareChannel, (uint8_t)param->inputChnConfig[index].filterVal);
                }
                else
                {
                    CFGTMR_HW_ConfigChnInputCaptureFilter(cfgtmrBaseAddress, hardwareChannel, 0U);
                }
            }

            if (CFGTMR_SIGNAL_MEASUREMENT == param->inputChnConfig[index].inputCaptureMode)
            {
                measureType = param->inputChnConfig[index].measurementMode;
                enableContMode = param->inputChnConfig[index].enableContinuousMode;
                /* Configure the signal measurement type */
                returnStatus = CFGTMR_ConfigSignalMeasurementType(ins, hardwareChannel, enableContMode, measureType);
            }
            else if (CFGTMR_EDGE_DETECT == param->inputChnConfig[index].inputCaptureMode)
            {
                /* Disable the dual edge mode */
                CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, false);
                /* Set input capture mode */
                CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, hardwareChannel, 0U);
                /* Set the event which will generate the interrupt */
                CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, hardwareChannel, (uint8_t)param->inputChnConfig[index].edgeMode);
                /* Enable interrupt request for the current channel */
                CFGTMR_HW_EnableChnInt(cfgtmrBaseAddress, hardwareChannel);
                INT_SYS_EnableIRQ(g_cfgtmrIrqId[ins][hardwareChannel]);
            }

            if (STATUS_SUCCESS != returnStatus)
            {
                break;
            }
        }

        if (STATUS_SUCCESS == returnStatus)
        {
            /* Update mode. */
            if (cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_NOT_INITIALIZED)
            {
                cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_INPUT_CAPTURE;
                /* Set clock source to start the counter. */
                CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, cfgtmrStatus->cfgtmrClkSrc);
            }
        }
        else
        {
            if (cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE)
            {
                cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_EDGE_ALIGNED_PWM;
            }
        }
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief De-initialize Channel Input Capture.
 *
 * @param ins: CFGTMR instance number
 * @param param: CFGTMR_INPUT_PARAM_T pointer
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_DeinitInputCapture(uint32_t ins, const CFGTMR_INPUT_PARAM_T *param)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint8_t hardwareChannel  = 0U;
    uint8_t channelPairNum   = 0U;
    uint8_t index            = 0U;

    /* Remain configuration for PWM */
    if (cfgtmrStatus->cfgtmrMode != CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE)
    {
        /* CFGTMR counter is disabled */
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
        CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, 0U);
        CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, 0xFFFFU);
        CFGTMR_HW_ConfigCpwmsMode(cfgtmrBaseAddress, false);
    }

    for (index = 0U; index < param->numOfChannels; index++)
    {
        hardwareChannel = param->inputChnConfig[index].hwChannelId;
        channelPairNum =  (uint8_t)(hardwareChannel >> 1U);
        /* Disable filtering for input channels */
        if (hardwareChannel < CHAN4_IDX)
        {
            CFGTMR_HW_ConfigChnInputCaptureFilter(cfgtmrBaseAddress, hardwareChannel, 0U);
        }

        CFGTMR_HW_ConfigDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, hardwareChannel, (uint8_t)0U);
        CFGTMR_HW_DisableChnInt(cfgtmrBaseAddress, hardwareChannel);
        CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, hardwareChannel);

        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (hardwareChannel | 1U), (uint8_t)0U);
        CFGTMR_HW_DisableChnInt(cfgtmrBaseAddress, (hardwareChannel | 1U));
        CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, (hardwareChannel | 1U));
    }

    if (cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE)
    {
        cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_EDGE_ALIGNED_PWM;
    }
    else
    {
        cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;
    }

    /* Clear Callbacks function from the status structure */
    for (index = 0U; index < FEATURE_CFGTMR_CHANNEL_NUM; index++)
    {
        cfgtmrStatus->channelsCallbacksParams[index] =  NULL;
        cfgtmrStatus->channelsCallbacks[index] = NULL;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Calculate the measurement and/or time stamps values which are read from
 * the channel value registers and stored to the static buffers.
 *
 * @param ins: CFGTMR instance number
 * @param channelNum: CFGTMR hardware channel number
 *
 * @retval The measurement value.
 */
uint16_t CFGTMR_ReadInputCaptureMeasurement(uint32_t ins, uint8_t channelNum)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    const CFGTMR_STATE_T *status = g_cfgtmrStatusPtr[ins];
    uint16_t returnValue = 0U;

    if (status != NULL)
    {
        returnValue = status->measurementResults[channelNum];
    }

    return returnValue;
}

/*!
 * @brief Start new Signal Measurements on a dual input compare channelNum
 * that is configured as single-shot measurement.
 *
 * @param ins: CFGTMR instance number
 * @param channelNum: CFGTMR hardware channel number
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_StartNewSignalMeasurement(uint32_t ins, uint8_t channelNum)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    STATUS_T returnStatus = STATUS_SUCCESS;

    /* Clear CH(n)F and CH(n+1)F  flags and Set DECAP bit */
    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t channelPairNum  = (uint8_t)(channelNum >> 1U);

    /* Get channelNum mode */
    if (FEATURE_CFGTMR_INPUT_CAPTURE_SINGLE_SHOT == CFGTMR_HW_ReadChnMode(cfgtmrBaseAddress, channelNum))
    {
        if (CFGTMR_HW_ReadDualEdgeCaptureBit(cfgtmrBaseAddress, channelPairNum))
        {
            /* Clear event flags for channelNum n and n + 1 */
            CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U));
            CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, channelNum);
            /* Set DECAP bit to start measurement */
            CFGTMR_HW_ConfigDualChnDecapCmd(cfgtmrBaseAddress, channelPairNum, true);
        }
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief Config mode operation for channel in the input capture mode.
 *
 * @details This function will change the channel mode at run time or when stopping channel,
 * The channel mode is selected in the CFGTMR_IC_OP_MODE_T enumeration type.
 *
 * @param ins: CFGTMR instance number
 * @param channelNum: CFGTMR hardware channel number
 * @param inputCaptureMode: The channel of measurement type for input capture mode
 * @param enableContinuousCapture: Enable/Disable continuous capture
 *
 * @retval Status code.
 */
STATUS_T CFGTMR_ConfigChannelInputCaptureMode(
    uint32_t ins,
    uint8_t channelNum,
    CFGTMR_IC_OP_MODE_T inputCaptureMode,
    bool enableContinuousCapture)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t channelPair = (uint8_t)(channelNum >> 1U);
    uint8_t level = 1U;
    uint8_t nextChannelLevel = 1U;

    if ((inputCaptureMode == CFGTMR_TIMESTAMP_RISING_EDGE) || \
        (inputCaptureMode == CFGTMR_TIMESTAMP_FALLING_EDGE) || \
        (inputCaptureMode == CFGTMR_TIMESTAMP_BOTH_EDGES))
    {
        /* Disable the dual edge mode */
        CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPair, false);
        /* Set input capture mode */
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelNum, 0U);
        /* Set the event which will generate the interrupt */
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, (uint8_t)inputCaptureMode);
        /* Enable interrupt request for the current channel */
        CFGTMR_HW_EnableChnInt(cfgtmrBaseAddress, channelNum);
    }
    else if (inputCaptureMode == CFGTMR_DISABLE_OPERATION)
    {
        /* Set the edge level to disable operation on the channel input */
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, 0x0U);
    }
    else
    {
        /* Enable the dual edge mode */
        CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPair, true);
        /* Enable dual edge input capture */
        CFGTMR_HW_ConfigDualChnDecapCmd(cfgtmrBaseAddress, channelPair, true);
        /* If continuous mode is set */
        if (false == enableContinuousCapture)
        {
            /* Clear MSnA and Set MSnB bit */
            CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelNum, 2U);
        }
        else
        {
             /* Set MSnA and MSnB bit*/
            CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelNum, 3U);
        }

        /* Check If want to measure a pulse width or period of the signal */
        if ((CFGTMR_MEASURE_RISING_EDGE_PERIOD == inputCaptureMode) || (CFGTMR_MEASURE_PULSE_HIGH == inputCaptureMode))
        {
            if (CFGTMR_MEASURE_PULSE_HIGH == inputCaptureMode)
            {
                /* Measure time between rising and falling edge - positive duty */
                nextChannelLevel = 2U;
            }
        }
        else
        {
            level = 2U;
            if (CFGTMR_MEASURE_FALLING_EDGE_PERIOD == inputCaptureMode)
            {
                /* If channel (n) is configured to capture rising edges ( CHMSEL1(n):CHMSEL0(n) = 1:0 ) than
                 * channel (n+1) is setup to capture also raising edges ( CHMSEL1(n+1):CHMSEL0(n+1) = 1:0 */
                nextChannelLevel = 2U;
            }
        }

        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, level);
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U), nextChannelLevel);

        /* Disable interrupt request for the current channel */
        CFGTMR_HW_DisableChnInt(cfgtmrBaseAddress, channelNum);
        CFGTMR_HW_EnableChnInt(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U));
    }

    return STATUS_SUCCESS;
}

/*******************************************************************************
 *                          CFGTMR Quadrature Decoder PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Config the parameters needed and activates quadrature
 *
 * @param ins: CFGTMR instance number
 * @param cfg: CFGTMR_QUAD_DECODE_CONFIG_T pointer
 *
 * @retval Status code.
 *
 * @note This function can only be used with CFGTMR1 and CFGTMR2.
 */
STATUS_T CFGTMR_ConfigQuadDecodeStart(uint32_t ins, const CFGTMR_QUAD_DECODE_CONFIG_T *cfg)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    STATUS_T returnStatus    = STATUS_SUCCESS;
    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];

    if ((false == POINTER_IS_NULL(cfgtmrStatus)) && (CFGTMR_MODE_NOT_INITIALIZED == cfgtmrStatus->cfgtmrMode))
    {
        /* Disable Quadrature Decoder */
        CFGTMR_HW_ConfigQuadDecoderCmd(cfgtmrBaseAddress, false);
        CFGTMR_HW_ConfigClockSource(cfgtmrBaseAddress, CFGTMR_CLOCK_SRC_NONE);
        /* Configure Quadrature Decoder */
        /* Set decoder mode Speed and direction or Phase A and Phase B encoding */
        CFGTMR_HW_ConfigQuadMode(cfgtmrBaseAddress, (uint8_t)cfg->mode);

        /* Set filter state for Phase B (enable/disable) */
        CFGTMR_HW_ConfigQuadPhaseBFilterCmd(cfgtmrBaseAddress, cfg->phaseBCfg.isPhaseInputFilter);
        /* Set Phase B filter value if phase filter is enabled */
        if (cfg->phaseBCfg.isPhaseInputFilter)
        {
            CFGTMR_HW_ConfigChnInputCaptureFilter(cfgtmrBaseAddress, CHAN1_IDX, cfg->phaseBCfg.phaseFilterVal);
        }

        /* Set filter state for Phase A (enable/disable) */
        CFGTMR_HW_ConfigQuadPhaseAFilterCmd(cfgtmrBaseAddress, cfg->phaseACfg.isPhaseInputFilter);
        /* Set Phase A filter value if phase filter is enabled */
        if (cfg->phaseACfg.isPhaseInputFilter)
        {
            CFGTMR_HW_ConfigChnInputCaptureFilter(cfgtmrBaseAddress, CHAN0_IDX, cfg->phaseACfg.phaseFilterVal);
        }

        /* Set polarity for Phase A and Phase B */
        CFGTMR_HW_ConfigQuadPhaseBPolarity(cfgtmrBaseAddress, (uint8_t)cfg->phaseBCfg.phasePolarity);
        CFGTMR_HW_ConfigQuadPhaseAPolarity(cfgtmrBaseAddress, (uint8_t)cfg->phaseACfg.phasePolarity);
        /* Configure counter (initial value and maximum value) */
        CFGTMR_HW_InitCounterValue(cfgtmrBaseAddress, cfg->initVal);
        CFGTMR_HW_ConfigMod(cfgtmrBaseAddress, cfg->maxVal);
        CFGTMR_HW_ConfigCounter(cfgtmrBaseAddress, cfg->initVal);
        /* Enable Quadrature Decoder */
        CFGTMR_HW_ConfigQuadDecoderCmd(cfgtmrBaseAddress, true);
        cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_QUADRATURE_DECODER;
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief Stop quadrature decoder mode
 *
 * @param ins: CFGTMR instance number
 *
 * @retval Status code.
 *
 * @note This function can only be used with CFGTMR1 and CFGTMR2.
 */
STATUS_T CFGTMR_ConfigQuadDecodeStop(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];

    /* Disable Quadrature decoder */
    CFGTMR_HW_ConfigQuadDecoderCmd(cfgtmrBaseAddress, false);
    cfgtmrStatus->cfgtmrMode = CFGTMR_MODE_NOT_INITIALIZED;

    return STATUS_SUCCESS;
}

/*!
 * @brief Read the current quadrature decoder Status
 *
 * @param ins: CFGTMR instance number
 *
 * @retval The current state of quadrature decoder

 * @note This function can only be used with CFGTMR1 and CFGTMR2.
 */
CFGTMR_QUAD_DECODE_STATE_T CFGTMR_ReadQuadDecoderStatus(uint32_t ins)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T const *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    CFGTMR_QUAD_DECODE_STATE_T cfgtmrQDState;

    cfgtmrQDState.counterVal   = CFGTMR_HW_ReadCounterValue(cfgtmrBaseAddress);
    cfgtmrQDState.overflowDir  = CFGTMR_HW_ReadQuadTimerOverflowDir(cfgtmrBaseAddress);
    cfgtmrQDState.overflowFlag = CFGTMR_HW_ReadTimerOverflowedStatus(cfgtmrBaseAddress);
    cfgtmrQDState.counterDir   = CFGTMR_HW_ReadQuadDir(cfgtmrBaseAddress);

    return cfgtmrQDState;
}

/*!
 * @brief Read the default configuration values.
 *
 * @param cfg: CFGTMR_QUAD_DECODE_CONFIG_T pointer.
 *
 * @retval None.
 */
void CFGTMR_ReadQuadDecoderDefaultConfig(CFGTMR_QUAD_DECODE_CONFIG_T * const cfg)
{
    cfg->initVal                    = 0U;
    cfg->maxVal                     = 65535U;
    cfg->mode                       = CFGTMR_QUAD_COUNT_AND_DIR;
    cfg->phaseACfg.phaseFilterVal   = 0U;
    cfg->phaseACfg.isPhaseInputFilter = false;
    cfg->phaseACfg.phasePolarity    = CFGTMR_QUAD_PHASE_NORMAL;
    cfg->phaseBCfg.phaseFilterVal   = 0U;
    cfg->phaseBCfg.isPhaseInputFilter = false;
    cfg->phaseBCfg.phasePolarity    = CFGTMR_QUAD_PHASE_NORMAL;
}

/*******************************************************************************
 *                          CFGTMR IRQ Handler FUNCTIONS
 ******************************************************************************/

/*!
 * @brief CFGTMR0 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR0_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(0U, 0U);
}

/*!
 * @brief CFGTMR0 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR0_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(0U, 1U);
}

/*!
 * @brief CFGTMR0 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR0_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(0U, 2U);
}

/*!
 * @brief CFGTMR0 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR0_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(0U, 3U);
}

/*!
 * @brief CFGTMR1 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR1_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(1U, 0U);
}

/*!
 * @brief CFGTMR1 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR1_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(1U, 1U);
}

/*!
 * @brief CFGTMR1 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR1_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(1U, 2U);
}

/*!
 * @brief CFGTMR1 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR1_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(1U, 3U);
}

/*!
 * @brief CFGTMR2 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR2_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(2U, 0U);
}

/*!
 * @brief CFGTMR2 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR2_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(2U, 1U);
}

/*!
 * @brief CFGTMR2 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR2_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(2U, 2U);
}

/*!
 * @brief CFGTMR2 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR2_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(2U, 3U);
}

/*!
 * @brief CFGTMR3 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR3_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(3U, 0U);
}

/*!
 * @brief CFGTMR3 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR3_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(3U, 1U);
}

/*!
 * @brief CFGTMR3 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR3_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(3U, 2U);
}

/*!
 * @brief CFGTMR3 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR3_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(3U, 3U);
}

#if (CFGTMR_INSTANCE_COUNT > 4U)
/*!
 * @brief CFGTMR4 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR4_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(4U, 0U);
}

/*!
 * @brief CFGTMR4 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR4_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(4U, 1U);
}

/*!
 * @brief CFGTMR4 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR4_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(4U, 2U);
}

/*!
 * @brief CFGTMR4 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR4_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(4U, 3U);
}

/*!
 * @brief CFGTMR5 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR5_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(5U, 0U);
}

/*!
 * @brief CFGTMR5 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR5_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(5U, 1U);
}

/*!
 * @brief CFGTMR5 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR5_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(5U, 2U);
}

/*!
 * @brief CFGTMR5 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR5_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(5U, 3U);
}
#endif

#if (CFGTMR_INSTANCE_COUNT > 6U)
/*!
 * @brief CFGTMR6 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR6_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(6U, 0U);
}

/*!
 * @brief CFGTMR6 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR6_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(6U, 1U);
}

/*!
 * @brief CFGTMR6 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR6_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(6U, 2U);
}

/*!
 * @brief CFGTMR6 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR6_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(6U, 3U);
}

/*!
 * @brief CFGTMR7 Channel 0 and 1 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR7_Ch0_Ch1_IRQHandler(void)
{
    CFGTMR_IRQHandler(7U, 0U);
}

/*!
 * @brief CFGTMR7 Channel 2 and 3 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR7_Ch2_Ch3_IRQHandler(void)
{
    CFGTMR_IRQHandler(7U, 1U);
}

/*!
 * @brief CFGTMR7 Channel 4 and 5 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR7_Ch4_Ch5_IRQHandler(void)
{
    CFGTMR_IRQHandler(7U, 2U);
}

/*!
 * @brief CFGTMR7 Channel 6 and 7 IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void CFGTMR7_Ch6_Ch7_IRQHandler(void)
{
    CFGTMR_IRQHandler(7U, 3U);
}
#endif

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Interrupt handler for the CFGTMR.
 *
 * @param ins: CFGTMR instance number
 * @param channelPair: The CFGTMR peripheral channel pair number
 *
 * @retval None.
 */
static void CFGTMR_IRQHandler(uint32_t ins, uint8_t channelPair)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    const CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    if ((cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_INPUT_CAPTURE) || \
        (cfgtmrStatus->cfgtmrMode == CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE))
    {
        CFGTMR_InputCaptureHandler(ins, channelPair);
    }
    else
    {
        /* Nothing to do */
    }
}

/*!
 * @brief Interrupt handler for the CFGTMR in input capture mode.
 *
 * @param ins: CFGTMR instance number
 * @param channelPair: The CFGTMR peripheral channel pair number
 *
 * @retval None.
 */
static void CFGTMR_InputCaptureHandler(uint32_t ins, uint8_t channelPair)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress  = g_cfgtmrBaseAddress[ins];
    CFGTMR_STATE_T *cfgtmrStatus = g_cfgtmrStatusPtr[ins];
    uint8_t channelNum = (uint8_t)(channelPair << 1U);

    /* Verify the mode for current pair of channels */
    if (false == CFGTMR_HW_ReadDualEdgeCaptureBit(cfgtmrBaseAddress, channelPair))
    {
        /* To get the channelNum interrupt source the both channels flag must be checked */
        if (false == CFGTMR_HW_ReadChnEventHasOccurred(cfgtmrBaseAddress, channelNum))
        {
            channelNum++;
        }

        /* Get the time stamp of the event */
        cfgtmrStatus->measurementResults[channelNum] = CFGTMR_HW_ReadChnCountVal(cfgtmrBaseAddress, channelNum);
        /* Clear the flag for C(n+1) channelNum */
        CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, channelNum);
    }
    else
    {
        /* Dual edge input capture case */
        uint16_t first_event_time = CFGTMR_HW_ReadChnCountVal(cfgtmrBaseAddress, channelNum);
        uint16_t second_event_time = CFGTMR_HW_ReadChnCountVal(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U));
        if (second_event_time <= first_event_time)
        {
            /* Measurement when overflow occurred */
            cfgtmrStatus->measurementResults[channelNum] = \
                (uint16_t)(second_event_time + (cfgtmrStatus->cfgtmrModVal - first_event_time));
        }
        else
        {
            /* Measurement when overflow doesn't occurred */
            cfgtmrStatus->measurementResults[channelNum] = (uint16_t)(second_event_time - first_event_time);
        }

        /* Clear flags for channels n and n+1 */
        CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, channelNum);
        CFGTMR_HW_ClearChnEventFlag(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U));
    }

    /* Re-update MOD value */
    cfgtmrStatus->cfgtmrModVal = CFGTMR_HW_ReadMod(cfgtmrBaseAddress);

    /* If the callback is defined to use it */
    if ((POINTER_IS_NULL((cfgtmrStatus->channelsCallbacks[channelNum])) == false) && \
        (cfgtmrStatus->enableNotification[channelNum] == true))
    {
        cfgtmrStatus->channelsCallbacks[channelNum](
            IC_EVENT_MEASUREMENT_COMPLETE,
            cfgtmrStatus->channelsCallbacksParams[channelNum]);
    }
}

/*!
 * @brief Config the measurement type in the input capture mode.
 *
 * @param ins: CFGTMR instance number
 * @param channelNum: CFGTMR hardware channel number
 * @param enableContinuousMode: Enable/Disable continuous measurement
 * @param measureType: CFGTMR input capture measurement type
 *
 * @retval Status code.
 */
static STATUS_T CFGTMR_ConfigSignalMeasurementType(
    uint32_t ins,
    uint8_t channelNum,
    bool enableContinuousMode,
    CFGTMR_SIGNAL_MEASUREMENT_T measureType)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint8_t channelPairNum = (uint8_t)(channelNum >> 1U);

    /* Enable the dual edge mode */
    CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, true);
    /* Enable dual edge input capture */
    CFGTMR_HW_ConfigDualChnDecapCmd(cfgtmrBaseAddress, channelPairNum, true);

    /* If continuous mode is set */
    if (false == enableContinuousMode)
    {
        /* Clear Channel (n) Mode Select 2/3 bit */
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelNum, 2U);
    }
    else
    {
        /* Set Channel (n) Mode Select 2/3 bit */
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelNum, 3U);
    }

    /* Check If want to measure a pulse width or period of the signal */
    if ((CFGTMR_PERIOD_ON_MEASUREMENT == measureType) || (CFGTMR_RISING_EDGE_PERIOD_MEASUREMENT== measureType))
    {
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, 1U);
        if (CFGTMR_RISING_EDGE_PERIOD_MEASUREMENT == measureType)
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U), 1U);
        }
        else
        {
            /* Measure time between rising and falling edge - positive duty */
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U), 2U);
        }
    }
    else if ((CFGTMR_PERIOD_OFF_MEASUREMENT == measureType) || (CFGTMR_FALLING_EDGE_PERIOD_MEASUREMENT == measureType))
    {
        CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelNum, 2U);
        if (CFGTMR_FALLING_EDGE_PERIOD_MEASUREMENT == measureType)
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U), 2U);
        }
        else
        {
            /* Measure time between falling and rising edge - negative duty */
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U), 1U);
        }
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    /* Enable the interrupt request for the channelNum which will indicate that the measurement is done. */
    CFGTMR_HW_EnableChnInt(cfgtmrBaseAddress, (uint8_t)(channelNum + 1U));
    INT_SYS_EnableIRQ(g_cfgtmrIrqId[ins][channelNum]);

    return returnStatus;
}

/*!
 * @brief Config the PWM signal for the independent channel.
 *
 * @param ins: CFGTMR instance number
 * @param pwmParam: CFGTMR_PWM_PARAM_T pointer.
 *
 * @retval None.
 */
static void CFGTMR_ConfigPwmIndependentChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t channelId = 0U;
    uint8_t channelPairNum = 0U;
    uint8_t index = 0U;

    /* Configure independent PWM channels */
    for (index = 0U; index < pwmParam->numOfIndependentPwmChns; index++)
    {
        channelId = pwmParam->pwmIndependentChnCfg[index].hwChannelId;
        channelPairNum =  (uint8_t)(channelId >> 1U);
        /* Configure POL bits for fail safe state */
        CFGTMR_HW_ConfigChnOutputPolarityCmd(cfgtmrBaseAddress, channelId, (bool)pwmParam->pwmIndependentChnCfg[index].safeState);

        /* Configure polarity of the PWM signal taking into consideration POL and ELSA/ELSB */
        if ((uint32_t)(pwmParam->pwmIndependentChnCfg[index].safeState) != (uint32_t)(pwmParam->pwmIndependentChnCfg[index].polarity))
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId, (uint8_t)2U);
        }
        else
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId, (uint8_t)1U);
        }

        if (false == (pwmParam->pwmIndependentChnCfg[index].enableSecondChnOutput))
        {
            CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, false);
        }
        else
        {
            /* Configure dead time, and enable complementary channel. */
            CFGTMR_HW_ConfigDualChnDeadtimeCmd(cfgtmrBaseAddress, channelPairNum, pwmParam->pwmIndependentChnCfg[index].enableDeadTime);
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId + 1U, (uint8_t)2U);
            CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, true);
            if (pwmParam->pwmIndependentChnCfg[index].secondChnPolarity != CFGTMR_MAIN_INVERTED)
            {
                CFGTMR_HW_ConfigChnOutputPolarityCmd(cfgtmrBaseAddress, channelId + 1U, !((bool)pwmParam->pwmIndependentChnCfg[index].safeState));
            }
            else
            {
                CFGTMR_HW_ConfigChnOutputPolarityCmd(cfgtmrBaseAddress, channelId + 1U, (bool)pwmParam->pwmIndependentChnCfg[index].safeState);
            }
        }

        /* Disable combined mode. */
        CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
        CFGTMR_HW_ConfigDualChnMofCombineCmd(cfgtmrBaseAddress, channelPairNum, false);
        /* Set MSB and MSA bits*/
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelId, 3U);
        /* Configure fault mode */
        CFGTMR_HW_ConfigDualChnFaultCmd(cfgtmrBaseAddress, channelPairNum, ((pwmParam->pwmFaultCfg)->faultMode != CFGTMR_FAULT_CONTROL_DISABLED) ? true : false);
        /* Enable sync control for channels*/
        CFGTMR_HW_ConfigDualChnPwmSyncCmd(cfgtmrBaseAddress, channelPairNum, true);
        /* Enable the generation a trigger on chip module */
        CFGTMR_HW_ConfigChnTriggerCmd(cfgtmrBaseAddress, channelId, pwmParam->pwmIndependentChnCfg[index].isExternalTrigger);

        /* Write CFGTMRn_PWMLOAD register to enable synchronized loading points for the given channel */
        CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, channelId);
        if (pwmParam->pwmIndependentChnCfg[index].enableSecondChnOutput)
        {
            CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, channelId + 1U);
        }
    }
}

/*!
 * @brief Config the PWM signal for the combined channel.
 *
 * @param ins: CFGTMR instance number
 * @param pwmParam: CFGTMR_PWM_PARAM_T pointer.
 *
 * @retval None.
 */
static void CFGTMR_ConfigPwmCombinedChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    CFGTMR_T *cfgtmrBaseAddress = g_cfgtmrBaseAddress[ins];
    uint8_t channelId = 0U;
    uint8_t channelPairNum = 0U;
    uint8_t index = 0U;

    /* Configure combined PWM channels */
    for (index = 0U; index < pwmParam->numOfCombinedPwmChns; index++)
    {
        channelId = pwmParam->pwmCombinedChnCfg[index].hwChannelId;
        channelPairNum = (uint8_t)(channelId >> 1U);

        /* Configure POL bits for fail safe state */
        CFGTMR_HW_ConfigChnOutputPolarityCmd(
            cfgtmrBaseAddress,
            channelId,
            (bool)pwmParam->pwmCombinedChnCfg[index].mainChnSafeState);

        /* Configure polarity of the PWM signal taking into consideration POL and ELSA/ELSB */
        if ((uint32_t)(pwmParam->pwmCombinedChnCfg[index].mainChnSafeState) != \
            (uint32_t)(pwmParam->pwmCombinedChnCfg[index].mainChnPolarity))
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId, (uint8_t)2U);
        }
        else
        {
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId, (uint8_t)1U);
        }

        CFGTMR_HW_ConfigDualEdgeCaptureCmd(cfgtmrBaseAddress, channelPairNum, false);
        /* Set MSB and MSA bits */
        CFGTMR_HW_ConfigChnMSnBAMode(cfgtmrBaseAddress, channelId, 3U);

        /* Enable channel (n) output */
        CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, channelId);
        /* Configure channel n+1 if it necessary. */
        if (false != (pwmParam->pwmCombinedChnCfg[index].enableSecondChnOutput))
        {
            channelId = channelId + 1U;
            /* Configure POL bits for fail safe state */
            CFGTMR_HW_ConfigChnOutputPolarityCmd(
                cfgtmrBaseAddress,
                channelId,
                (bool)pwmParam->pwmCombinedChnCfg[index].secondChnSafeState);
            CFGTMR_HW_ConfigChnEdgeLevel(cfgtmrBaseAddress, channelId, (uint8_t)2U);

            /* Configure polarity of the second channel relative to main channel polarity. */
            if (pwmParam->pwmCombinedChnCfg[index].secondChnSafeState != \
                pwmParam->pwmCombinedChnCfg[index].mainChnSafeState)
            {
                if(pwmParam->pwmCombinedChnCfg[index].secondChnPolarity != CFGTMR_MAIN_DUPLICATED)
                {
                    CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, false);
                }
                else
                {
                    CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, true);
                }
            }
            else
            {
                if(pwmParam->pwmCombinedChnCfg[index].secondChnPolarity != CFGTMR_MAIN_DUPLICATED)
                {
                    CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, true);
                }
                else
                {
                    CFGTMR_HW_ConfigDualChnCompCmd(cfgtmrBaseAddress, channelPairNum, false);
                }
            }
            /* Enable channel (n+1) output */
            CFGTMR_HW_EnablePwmChannelOutputs(cfgtmrBaseAddress, (uint8_t)(channelId));
        }

        /* Set fault control for the channel */
        CFGTMR_HW_ConfigDualChnFaultCmd(
            cfgtmrBaseAddress,
            channelPairNum,
            ((pwmParam->pwmFaultCfg)->faultMode != CFGTMR_FAULT_CONTROL_DISABLED) ? true : false);
        /* Enable sync control for channels */
        CFGTMR_HW_ConfigDualChnPwmSyncCmd(cfgtmrBaseAddress, channelPairNum, true);
        /* Enable the combine mode */
        CFGTMR_HW_ConfigDualChnCombineCmd(cfgtmrBaseAddress, channelPairNum, true);
        /* Configure the modified combine mode */
        CFGTMR_HW_ConfigDualChnMofCombineCmd(
            cfgtmrBaseAddress,
            channelPairNum,
            pwmParam->pwmCombinedChnCfg[index].enableModifiedCombine);
        /* Configure dead time */
        CFGTMR_HW_ConfigDualChnDeadtimeCmd(cfgtmrBaseAddress, channelPairNum, pwmParam->pwmCombinedChnCfg[index].enableDeadTime);
        /* Enable the generation a trigger on the channel (n) */
        channelId = (uint8_t)(channelPairNum << 1U);
        CFGTMR_HW_ConfigChnTriggerCmd(cfgtmrBaseAddress, channelId, pwmParam->pwmCombinedChnCfg[index].isExternalTrigger);
        /* Enable the generation a trigger on the channel (n+1) */
        channelId = channelId + 1U;
        CFGTMR_HW_ConfigChnTriggerCmd(
            cfgtmrBaseAddress,
            channelId,
            pwmParam->pwmCombinedChnCfg[index].isExternalTriggerOnNextChn);
    }
}

/*!
 * @brief Config the duty cycle for the PWM signal.
 *
 * @param ins: CFGTMR instance number
 * @param pwmParam: CFGTMR_PWM_PARAM_T pointer.
 *
 * @retval Status code.
 */
static STATUS_T CFGTMR_ConfigPwmDutyCycleChannel(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam)
{
    CFGTMR_INSTANCE_VALIDITY(ins);

    STATUS_T returnValue = STATUS_SUCCESS;
    uint8_t hardwareChannel = 0U;
    uint8_t index = 0U;

    for (index = 0U; index < pwmParam->numOfIndependentPwmChns; index++)
    {
        hardwareChannel = pwmParam->pwmIndependentChnCfg[index].hwChannelId;
        /* Write channel value registers and setup duty cycle and phase values */
        returnValue = CFGTMR_UpdatePwmChannel(ins,
                                            hardwareChannel,
                                            CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE,
                                            pwmParam->pwmIndependentChnCfg[index].dutyCycle,
                                            0U,
                                            false);
    }

    for (index = 0U; index < pwmParam->numOfCombinedPwmChns; index++)
    {
        hardwareChannel = pwmParam->pwmCombinedChnCfg[index].hwChannelId;
        /* Write channel value registers and setup duty cycle and phase values */
        returnValue = CFGTMR_UpdatePwmChannel(ins,
                                            hardwareChannel,
                                            CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE,
                                            pwmParam->pwmCombinedChnCfg[index].firstEdge,
                                            pwmParam->pwmCombinedChnCfg[index].secondEdge,
                                            false);
    }

    return returnValue;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Config the value for the half cycle reload register.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @param value The 16 bit counter value
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigHalfCycleValue(CFGTMR_T *const cfgtmrBase, uint16_t value)
{
    ((cfgtmrBase)->HC.bit.HCV) = (uint32_t)value;
}

/*!
 * @brief Config the CFGTMR clock source.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param clock The CFGTMR peripheral clock selection
 *            - 00: No clock
 *            - 01: system clock
 *            - 10: fixed clock
 *            - 11: External clock
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigClockSource(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_SRC_T clock)
{
    ((cfgtmrBase)->CSTS.bit.CCLKSSEL) = (uint32_t)clock;
}

/*!
 * @brief Config the CFGTMR clock divider.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param ps The CFGTMR peripheral clock pre-scale divider
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigClockPs(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_PSC_T ps)
{
    ((cfgtmrBase)->CSTS.bit.CLKDFSEL) = (uint32_t)ps;
}

/*!
 * @brief Read the CFGTMR clock divider.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The CFGTMR clock pre-scale divider
 */
uint8_t CFGTMR_HW_ReadClockPs(const CFGTMR_T *cfgtmrBase)
{
    return (uint8_t)((cfgtmrBase)->CSTS.bit.CLKDFSEL);
}

/*!
 * @brief  Config the CFGTMR peripheral timer overflow interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param state - true : Overflow interrupt enabled
 *                  - false: Overflow interrupt disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigTimerOverflowInt(CFGTMR_T *const cfgtmrBase, bool state)
{
    ((cfgtmrBase)->CSTS.bit.CFGTMROIEN) = (uint32_t)state;
}

/*!
 * @brief Enable PWM channel Outputs.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR channel(0-7)
 *
 * @retval None.
 */
void CFGTMR_HW_EnablePwmChannelOutputs(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    switch(channel)
    {
        case 0U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH0) = CFGTMR_CSTS_PWMOENCH0_1;
            break;
        case 1U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH1) = CFGTMR_CSTS_PWMOENCH1_1;
            break;
        case 2U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH2) = CFGTMR_CSTS_PWMOENCH2_1;
            break;
        case 3U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH3) = CFGTMR_CSTS_PWMOENCH3_1;
            break;
        case 4U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH4) = CFGTMR_CSTS_PWMOENCH4_1;
            break;
        case 5U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH5) = CFGTMR_CSTS_PWMOENCH5_1;
            break;
        case 6U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH6) = CFGTMR_CSTS_PWMOENCH6_1;
            break;
        case 7U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH7) = CFGTMR_CSTS_PWMOENCH7_1;
            break;
        default:
            break;
    }
}

/*!
 * @brief Disable PWM channel Outputs.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR channel(0-7)
 *
 * @retval None.
 */
void CFGTMR_HW_DisablePwmChannelOutputs(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    switch(channel)
    {
        case 0U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH0) = CFGTMR_CSTS_PWMOENCH0_0;
            break;
        case 1U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH1) = CFGTMR_CSTS_PWMOENCH1_0;
            break;
        case 2U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH2) = CFGTMR_CSTS_PWMOENCH2_0;
            break;
        case 3U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH3) = CFGTMR_CSTS_PWMOENCH3_0;
            break;
        case 4U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH4) = CFGTMR_CSTS_PWMOENCH4_0;
            break;
        case 5U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH5) = CFGTMR_CSTS_PWMOENCH5_0;
            break;
        case 6U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH6) = CFGTMR_CSTS_PWMOENCH6_0;
            break;
        case 7U:
            ((cfgtmrBase)->CSTS.bit.PWMOENCH7) = CFGTMR_CSTS_PWMOENCH7_0;
            break;
        default:
            break;
    }
}

/*!
 * @brief Clear the timer overflow interrupt flag.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 *
 * @retval None.
 */
void CFGTMR_HW_ClearTimerOverflow(CFGTMR_T *const cfgtmrBase)
{
    uint32_t temp;
    /* Read-after-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->CSTS.reg;
    (void)temp;

    ((cfgtmrBase)->CSTS.bit.CFGTMROFLG) = CFGTMR_CSTS_CFGTMROFLG_0;
}

/*!
 * @brief Read the bit that controls enabling the CFGTMR timer overflow interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval State of Timer Overflow Interrupt
 *         - true : Overflow interrupt is enabled
 *         - false: Overflow interrupt is disabled
 */
bool CFGTMR_HW_ReadOverflowIntStatus(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->CSTS.bit.CFGTMROIEN != CFGTMR_CSTS_CFGTMROIEN_0) ? true : false;
}

/*!
 * @brief Read the CFGTMR peripheral timer overflow interrupt flag.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval State of Timer Overflow Flag
 *         - true : CFGTMR counter has overflowed
 *         - false: CFGTMR counter has not overflowed
 */
bool CFGTMR_HW_ReadTimerOverflowedStatus(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->CSTS.bit.CFGTMROFLG != CFGTMR_CSTS_CFGTMROFLG_0) ? true : false;
}

/*!
 * @brief Config the CFGTMR count direction bit.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode The Center-Aligned PWM selection
 *                 - 1U: Up counting mode
 *                 - 0U: Up down counting mode
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigCpwmsMode(CFGTMR_T *const cfgtmrBase, bool mode)
{
    cfgtmrBase->CSTS.bit.CMSEL = (uint32_t)mode;
}

/*!
 * @brief Read the CFGTMR count direction bit.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The Center-Aligned PWM selection
 *         - 1U: Up counting mode
 *         - 0U: Up down counting mode
 */
bool CFGTMR_HW_ReadCpwmsMode(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->CSTS.bit.CMSEL != CFGTMR_CSTS_CMSEL_0) ? true : false;
}

/*!
 * @brief Config the CFGTMR reload interrupt enable.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable - true : Reload interrupt is  enabled
 *                   - false: Reload interrupt is disabled
 */
void CFGTMR_HW_ConfigReIntCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->CSTS.bit.RPIEN = (uint32_t)enable;
}

/*!
 * @brief Read the state whether the CFGTMR counter reached a reload point.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval State of reload point
 *         - true : CFGTMR counter reached a reload point
 *         - false: CFGTMR counter did not reach a reload point
 */
bool CFGTMR_HW_ReadReloadFlag(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->CSTS.bit.RFLG != CFGTMR_CSTS_RFLG_0) ? true : false;
}

/*!
 * @brief Clear the reload flag bit.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_ClearReloadFlag(CFGTMR_T *const cfgtmrBase)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->CSTS.reg;
    (void)temp;

    cfgtmrBase->CSTS.bit.RFLG = CFGTMR_CSTS_RFLG_0;
}

/*!
 * @brief Read true/false whether the reload interrupt was enabled or not
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval State of reload interrupt
 *         - true : reload interrupt is enable
 *         - false: reload interrupt is disable
 */
bool CFGTMR_HW_ReadReloadIntStatus(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->CSTS.bit.RPIEN != CFGTMR_CSTS_RPIEN_0) ? true : false;
}

/*!
 * @brief Reads the CFGTMR clock source.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The CFGTMR clock source selection
 *          - 00: No clock
 *          - 01: system clock
 *          - 10: fixed clock
 *          - 11: External clock
 */
uint8_t CFGTMR_HW_ReadClockSource(const CFGTMR_T *cfgtmrBase)
{
    return (uint8_t)cfgtmrBase->CSTS.bit.CCLKSSEL;
}

/*!
 * @brief Config the CFGTMR peripheral current counter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value The CFGTMR timer counter value to be set
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigCounter(CFGTMR_T *const cfgtmrBase, uint16_t value)
{
    cfgtmrBase->CNT.reg = (uint32_t)value;
}

/*!
 * @brief Config the CFGTMR peripheral timer modulo value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value The value to be set to the timer modulo
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigMod(CFGTMR_T *const cfgtmrBase, uint16_t value)
{
    cfgtmrBase->MOD.reg = (uint32_t)(value & 0xFFFFU);
}

/*!
 * @brief Config the CFGTMR peripheral timer counter with a initial value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value initial value to be set
 *
 * @retval None.
 */
void CFGTMR_HW_InitCounterValue(CFGTMR_T *const cfgtmrBase, uint16_t value)
{
    cfgtmrBase->INITV.reg = (uint32_t)(value & 0xFFFFU);
}

/*!
 * @brief Config the CFGTMR peripheral timer channel mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param selection The mode to be set valid value MSnB:MSnA :00, 01, 10, 11
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnMSnBAMode(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t selection)
{
    /* write CHMSEL2 bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL2 = (uint32_t)selection & 0x01U;

    /* write CHMSEL3 bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL3 = ((uint32_t)selection & 0x02U) >> 1U;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel edge level.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param level ELSnB:ELSnA :00, 01, 10, 11
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnEdgeLevel(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t level)
{
    /* write CHMSEL0 bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL0 = (uint32_t)level & 0x01U;

    /* write CHMSEL1 bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL1 = ((uint32_t)level & 0x02U) >> 1U;
}

/*!
 * @brief Enables the CFGTMR peripheral timer channel(n) interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval None.
 */
void CFGTMR_HW_EnableChnInt(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHIEN = CFGTMR_CHnCSTS_CHIEN_1;
}

/*!
 * @brief Disables the CFGTMR peripheral timer channel(n) interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval None.
 */
void CFGTMR_HW_DisableChnInt(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHIEN = CFGTMR_CHnCSTS_CHIEN_0;
}

/*!
 * @brief Clear the channel flag by writing a 0 to the CHF bit.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval None.
 */
void CFGTMR_HW_ClearChnEventFlag(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->CONTROLS[channel].CHnCSTS.reg;
    (void)temp;

    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHEVFLG = CFGTMR_CHnCSTS_CHEVFLG_0;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel counter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param value Counter value to be set
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnCountVal(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint16_t value)
{
    ((cfgtmrBase)->CONTROLS[channel].CHnV.bit.CHV) = (uint32_t)(value & 0xFFFFU);
}

/*!
 * @brief Read CFGTMR channel(n) interrupt enabled or not.
 * @param cfgtmrBase CFGTMR module base address
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval channel(n) interrupt status
 *         - true : Enable channel (n) interrupt
 *         - false: Disable channel (n) interrupt
 */
bool CFGTMR_HW_ReadChnIntStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase)->CONTROLS[channel].CHnCSTS.bit.CHIEN != CFGTMR_CHnCSTS_CHIEN_0) ? true : false;
}

/*!
 * @brief Read whether any event for the CFGTMR peripheral timer channel has occurred.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval State of channel flag
 *         - true : Event occurred
 *         - false: No event occurred.
 */
bool CFGTMR_HW_ReadChnEventHasOccurred(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase)->CONTROLS[channel].CHnCSTS.bit.CHEVFLG != CFGTMR_CHnCSTS_CHEVFLG_0) ? true : false;
}

/*!
 * @brief Read the CFGTMR peripheral timer channel mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval The MSnB:MSnA mode value, will be 00, 01, 10, 11
 *
 */
uint8_t CFGTMR_HW_ReadChnMode(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    uint32_t retValue;
    retValue = cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL2;

    retValue |= ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL3) << 1U);

    return (uint8_t)retValue;
}

/*!
 * @brief Config the provided value to the OUTMASK register.
 *
 * This function will mask/unmask multiple channels.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param regVal Value to be written to the register
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigOutmaskReg(CFGTMR_T *const cfgtmrBase, uint32_t regVal)
{
    ((cfgtmrBase)->OMASK.reg) = regVal;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel output polarity.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param polarity The polarity to be set
 *            - true : The channel polarity is active low
 *            - false  : The channel polarity is active high
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnOutputPolarityCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool polarity)
{
    if (true != polarity)
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->CHPOL.bit.CHPOL0 = CFGTMR_CHPOL_CHPOL0_0;
                break;
            case 1U:
                (cfgtmrBase)->CHPOL.bit.CHPOL1 = CFGTMR_CHPOL_CHPOL1_0;
                break;
            case 2U:
                (cfgtmrBase)->CHPOL.bit.CHPOL2 = CFGTMR_CHPOL_CHPOL2_0;
                break;
            case 3U:
                (cfgtmrBase)->CHPOL.bit.CHPOL3 = CFGTMR_CHPOL_CHPOL3_0;
                break;
            case 4U:
                (cfgtmrBase)->CHPOL.bit.CHPOL4 = CFGTMR_CHPOL_CHPOL4_0;
                break;
            case 5U:
                (cfgtmrBase)->CHPOL.bit.CHPOL5 = CFGTMR_CHPOL_CHPOL5_0;
                break;
            case 6U:
                (cfgtmrBase)->CHPOL.bit.CHPOL6 = CFGTMR_CHPOL_CHPOL6_0;
                break;
            case 7U:
                (cfgtmrBase)->CHPOL.bit.CHPOL7 = CFGTMR_CHPOL_CHPOL7_0;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->CHPOL.bit.CHPOL0 = CFGTMR_CHPOL_CHPOL0_1;
                break;
            case 1U:
                (cfgtmrBase)->CHPOL.bit.CHPOL1 = CFGTMR_CHPOL_CHPOL1_1;
                break;
            case 2U:
                (cfgtmrBase)->CHPOL.bit.CHPOL2 = CFGTMR_CHPOL_CHPOL2_1;
                break;
            case 3U:
                (cfgtmrBase)->CHPOL.bit.CHPOL3 = CFGTMR_CHPOL_CHPOL3_1;
                break;
            case 4U:
                (cfgtmrBase)->CHPOL.bit.CHPOL4 = CFGTMR_CHPOL_CHPOL4_1;
                break;
            case 5U:
                (cfgtmrBase)->CHPOL.bit.CHPOL5 = CFGTMR_CHPOL_CHPOL5_1;
                break;
            case 6U:
                (cfgtmrBase)->CHPOL.bit.CHPOL6 = CFGTMR_CHPOL_CHPOL6_1;
                break;
            case 7U:
                (cfgtmrBase)->CHPOL.bit.CHPOL7 = CFGTMR_CHPOL_CHPOL7_1;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel fault input polarity.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param faultChannelID The CFGTMR peripheral channel number
 * @param polarity The polarity to be set
 *                - false : The fault input polarity is active high
 *                - true: The fault input polarity is active low
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnFaultInputPolarityCmd(CFGTMR_T *const cfgtmrBase, uint8_t faultChannelID, bool polarity)
{
    if (true == polarity)
    {
         switch(faultChannelID)
        {
            case 0U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL0 = CFGTMR_FIPOLSEL_FIPOLSEL0_0;
                break;
            case 1U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL1 = CFGTMR_FIPOLSEL_FIPOLSEL1_0;
                break;
            case 2U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL2 = CFGTMR_FIPOLSEL_FIPOLSEL2_0;
                break;
            case 3U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL3 = CFGTMR_FIPOLSEL_FIPOLSEL3_0;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(faultChannelID)
        {
            case 0U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL0 = CFGTMR_FIPOLSEL_FIPOLSEL0_1;
                break;
            case 1U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL1 = CFGTMR_FIPOLSEL_FIPOLSEL1_1;
                break;
            case 2U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL2 = CFGTMR_FIPOLSEL_FIPOLSEL2_1;
                break;
            case 3U:
                (cfgtmrBase)->FIPOLSEL.bit.FIPOLSEL3 = CFGTMR_FIPOLSEL_FIPOLSEL3_1;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer fault interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param state Timer fault interrupt state
 *            - true : Fault control interrupt is enable
 *            - false: Fault control interrupt is disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigFaultInt(CFGTMR_T *const cfgtmrBase, bool state)
{
    (cfgtmrBase)->FMSEL.bit.FCTRLIEN = (uint32_t)state;
}

/*!
 * @brief Return true/false whether the Fault interrupt was enabled or not
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval - true : Fault control interrupt is enable
 *         - false: Fault control interrupt is disabled.
 */
bool CFGTMR_HW_ReadFaultIntStatus(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->FMSEL.bit.FCTRLIEN != CFGTMR_FMSEL_FCTRLIEN_0) ? true : false;
}

/*!
 * @brief Clears all fault interrupt flags that are active.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_ClearFaultsIsr(CFGTMR_T *const cfgtmrBase)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->FMSTS.reg;
    (void)temp;

    (cfgtmrBase)->FMSTS.bit.FICTRL = CFGTMR_FMSTS_FICTRL_0;
    (cfgtmrBase)->FMSTS.bit.FIDFLG0 = CFGTMR_FMSTS_FIDFLG0_0;

    (cfgtmrBase)->FMSTS.bit.FICTRL = CFGTMR_FMSTS_FICTRL_0;
    (cfgtmrBase)->FMSTS.bit.FIDFLG1 = CFGTMR_FMSTS_FIDFLG1_0;

    (cfgtmrBase)->FMSTS.bit.FICTRL = CFGTMR_FMSTS_FICTRL_0;
    (cfgtmrBase)->FMSTS.bit.FIDFLG2 = CFGTMR_FMSTS_FIDFLG2_0;

    (cfgtmrBase)->FMSTS.bit.FICTRL = CFGTMR_FMSTS_FICTRL_0;
    (cfgtmrBase)->FMSTS.bit.FIDFLG3 = CFGTMR_FMSTS_FIDFLG3_0;
}

/*!
 * @brief Config the CFGTMR fault control mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode Fault control mode value
 * - CFGTMR_FAULT_CONTROL_DISABLED: Fault control disabled
 * - CFGTMR_FAULT_CONTROL_MAN_EVEN: Fault control enabled for even channel (0, 2, 4, 6) and manual fault clearing.
 * - CFGTMR_FAULT_CONTROL_MAN_ALL : Fault control enabled for all channels and manual fault clearing is enabled.
 * - CFGTMR_FAULT_CONTROL_AUTO_ALL: Fault control enabled for all channels and automatic fault clearing is enabled.
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigFaultControlMode(CFGTMR_T *const cfgtmrBase, uint32_t mode)
{
    cfgtmrBase->FMSEL.bit.FCTRLMSEL = mode;
}

/*!
 * @brief Initializes the channels output.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable Initialize the channels output
 *                   - true : The channels output is initialized according to the state of OUTINIT reg
 *                   - false: No effect
 *
 * @retval None.
 */
void CFGTMR_HW_InitChnOutputCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->FMSEL.bit.INITSTS = (uint32_t)enable;
}

/*!
 * @brief Read the CFGTMR detected fault input.
 *
 * This function reads the status for all fault inputs
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The fault byte
 *         - 0 : No fault condition was detected.
 *         - 1 : A fault condition was detected.
 *
 * @retval None.
 */
bool CFGTMR_HW_ReadDetectedFaultInput(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase->FMSTS.bit.FCDFLG !=  CFGTMR_FMSTS_FCDFLG_0) ? true : false);
}

/*!
 * @brief Config the CFGTMR write protection.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable The CFGTMR write protection selection
 *                   - true : Write-protection is enabled
 *                   - false: Write-protection is disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigWriteProtectionCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        cfgtmrBase->FMSTS.bit.WPEN = CFGTMR_FMSTS_WPEN_1;
    }
    else
    {
        cfgtmrBase->FMSEL.bit.WPUNLOCK = CFGTMR_FMSEL_WPUNLOCK_1;
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer group.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable CFGTMR mode selection
 *                   - true : All registers including CFGTMR-specific registers are available
 *                   - false: Only the TPM-compatible registers are available
 *
 * @retval None.
 */
void CFGTMR_HW_Enable(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->FMSEL.bit.CFGTMREN = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR peripheral timer sync mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable PWM synchronization mode
 *                   - false: No restriction both software and hardware triggers can be used
 *                   - true : Software trigger can only be used for MOD and CnV synchronization,
 *                            hardware trigger only for OUTMASK and CFGTMR counter synchronization.
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPwmSyncMode(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->FMSEL.bit.PWMSMCFG = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR peripheral timer software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 * @param enable Software trigger selection
 *                   - true : Software trigger is selected
 *                   - false: Software trigger is not selected
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigSoftwareTriggerCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYN.bit.SWTSEL = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR hardware synchronization trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param trigger_num Number of trigger
 *                        - 0U: trigger 0
 *                        - 1U: trigger 1
 *                        - 2U: trigger 2
 * @param enable State of trigger
 *                   - true : Enable hardware trigger from field trigger_num for PWM synchronization
 *                   - false: Disable hardware trigger from field trigger_num for PWM synchronization
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigHardwareSyncTriggerSrc(CFGTMR_T *const cfgtmrBase, uint8_t trigger_num, bool enable)
{
    if (enable)
    {
        switch(trigger_num)
        {
            case 0U:
                cfgtmrBase->SYN.bit.HWT0SEL = CFGTMR_SYN_HWT0SEL_1;
                break;
            case 1U:
                cfgtmrBase->SYN.bit.HWT1SEL = CFGTMR_SYN_HWT0SEL_1;
                break;
            case 2U:
                cfgtmrBase->SYN.bit.HWT2SEL = CFGTMR_SYN_HWT0SEL_1;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(trigger_num)
        {
            case 0U:
                cfgtmrBase->SYN.bit.HWT0SEL = CFGTMR_SYN_HWT0SEL_0;
                break;
            case 1U:
                cfgtmrBase->SYN.bit.HWT1SEL = CFGTMR_SYN_HWT0SEL_0;
                break;
            case 2U:
                cfgtmrBase->SYN.bit.HWT2SEL = CFGTMR_SYN_HWT0SEL_0;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Determines when the OUTMASK register is updated with the value of its buffer.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable Output Mask synchronization selection
 *                   - true : OUTMASK register is updated only by PWM synchronization
 *                   - false: OUTMASK register is updated in all rising edges of the system clock
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigOutmaskPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYN.bit.OVSMSEL = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR peripheral timer maximum loading points.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable Maximum loading point selection
 *                   - true : To enable maximum loading point
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigMaxLoadingCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYN.bit.MAXLPEN = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR peripheral timer minimum loading points.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable Minimum loading point selection
 *                   - true : To enable minimum loading point
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigMinLoadingCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYN.bit.MINLPEN = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel modified combine mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel pair outputs modified combine
 *                   - true : To enable modified combine
 *                   - false: To disable modified combine
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnMofCombineCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.MCM0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.MCM1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.MCM2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.MCM3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.MCM0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.MCM1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.MCM2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.MCM3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel pair fault control.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel pair fault control
 *                   - true : To enable fault control
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnFaultCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.FCTRLEN3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel pair counter PWM sync.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel pair counter PWM sync
 *                   - true : To enable PWM synchronization
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnPwmSyncCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.PWMSYNEN3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel pair deadtime insertion.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel pair deadtime insertion
 *                   - true : To enable deadtime insertion
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnDeadtimeCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DTIEN0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DTIEN1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DTIEN2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DTIEN3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DTIEN0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DTIEN1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DTIEN2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DTIEN3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel dual edge capture.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel dual edge capture
 *                   - true : To enable dual edge capture mode
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnDecapCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DECSTS0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DECSTS1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DECSTS2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DECSTS3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DECSTS0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DECSTS1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DECSTS2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DECSTS3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer dual edge capture mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of dual edge capture mode
 *                   - true : To enable dual edge capture
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualEdgeCaptureCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DECMEN0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DECMEN1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DECMEN2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DECMEN3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.DECMEN0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.DECMEN1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.DECMEN2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.DECMEN3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Read the CFGTMR peripheral timer dual edge capture mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 *
 * @retval Dual edge capture mode status
 *         - true : To enable dual edge capture
 *         - false: To disable
 */
bool CFGTMR_HW_ReadDualEdgeCaptureBit(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum)
{
    bool tempValue;

    switch(chnlPairNum)
    {
        case 0U:
            tempValue = ((cfgtmrBase)->FLCH.bit.DECMEN0 != 0U) ? true : false;
            break;
        case 1U:
            tempValue = ((cfgtmrBase)->FLCH.bit.DECMEN1 != 0U) ? true : false;
            break;
        case 2U:
            tempValue = ((cfgtmrBase)->FLCH.bit.DECMEN2 != 0U) ? true : false;
            break;
        case 3U:
            tempValue = ((cfgtmrBase)->FLCH.bit.DECMEN3 != 0U) ? true : false;
            break;
        default:
            break;
    }
    return tempValue;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel pair output complement mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param polarity State of channel pair output complement mode
 *            - true : The channel (n+1) output is the complement of the channel (n) output
 *            - false: The channel (n+1) output is the same as the channel (n) output
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnCompCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool polarity)
{
    if (polarity == true)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.CCHO0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.CCHO1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.CCHO2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.CCHO3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.CCHO0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.CCHO1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.CCHO2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.CCHO3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel pair output combine mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel pair output combine mode
 *                   - true : Channels pair are combined
 *                   - false: Channels pair are independent
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnCombineCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if (enable == true)
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.CHCM0 = 1U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.CHCM1 = 1U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.CHCM2 = 1U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.CHCM3 = 1U;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(chnlPairNum)
        {
            case 0U:
                (cfgtmrBase)->FLCH.bit.CHCM0 = 0U;
                break;
            case 1U:
                (cfgtmrBase)->FLCH.bit.CHCM1 = 0U;
                break;
            case 2U:
                (cfgtmrBase)->FLCH.bit.CHCM2 = 0U;
                break;
            case 3U:
                (cfgtmrBase)->FLCH.bit.CHCM3 = 0U;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Verify if an channels pair is used in combine mode or not.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 *
 * @retval Channel pair output combine mode status
 *         - true : Channels pair are combined
 *         - false: Channels pair are independent
 */
bool CFGTMR_HW_ReadDualChnCombineCmd(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum)
{
   bool tempValue;

    switch(chnlPairNum)
    {
        case 0U:
            tempValue = ((cfgtmrBase)->FLCH.bit.CHCM0 != 0U) ? true : false;
            break;
        case 1U:
            tempValue = ((cfgtmrBase)->FLCH.bit.CHCM1 != 0U) ? true : false;
            break;
        case 2U:
            tempValue = ((cfgtmrBase)->FLCH.bit.CHCM2 != 0U) ? true : false;
            break;
        case 3U:
            tempValue = ((cfgtmrBase)->FLCH.bit.CHCM3 != 0U) ? true : false;
            break;
        default:
            break;
    }
    return tempValue;
}

/*!
 * @brief Verify if an channels pair is used in the modified combine mode or not.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 *
 * @retval Channel pair output combine mode status
 *         - true : Channels pair are combined
 *         - false: Channels pair are independent
 */
bool CFGTMR_HW_ReadDualChnMofCombineCmd(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum)
{
    bool tempValue;

    switch(chnlPairNum)
    {
        case 0U:
            tempValue = ((cfgtmrBase)->FLCH.bit.MCM0 != 0U) ? true : false;
            break;
        case 1U:
            tempValue = ((cfgtmrBase)->FLCH.bit.MCM1 != 0U) ? true : false;
            break;
        case 2U:
            tempValue = ((cfgtmrBase)->FLCH.bit.MCM2 != 0U) ? true : false;
            break;
        case 3U:
            tempValue = ((cfgtmrBase)->FLCH.bit.MCM3 != 0U) ? true : false;
            break;
        default:
            break;
    }
    return tempValue;
}

/*!
 * @brief Config the CFGTMR extended dead-time value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value The CFGTMR peripheral extend pre-scale divider
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigExtDeadtimeValue(CFGTMR_T *const cfgtmrBase, uint8_t value)
{
    (cfgtmrBase)->DCFG.bit.EDTV = (uint32_t)value;
}

/*!
 * @brief Config the CFGTMR dead time divider.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param divider The CFGTMR peripheral pre-scaler divider
 *                    - CFGTMR_DEADTIME_DIV_1 : Divide by 1
 *                    - CFGTMR_DEADTIME_DIV_4 : Divide by 4
 *                    - CFGTMR_DEADTIME_DIV_16: Divide by 16
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDeadtimePrescale(CFGTMR_T *const cfgtmrBase, CFGTMR_DEADTIME_PSC_T divider)
{
    (cfgtmrBase)->DCFG.bit.DPSCCFG = (uint32_t)divider;
}

/*!
 * @brief Config the CFGTMR deadtime value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param count The CFGTMR peripheral pre-scaler divider
 *                  - 0U : no counts inserted
 *                  - 1U : 1 count is inserted
 *                  - 2U : 2 count is inserted
 *                  - ... up to a possible 63 counts
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDeadtimeCount(CFGTMR_T *const cfgtmrBase, uint8_t count)
{
    (cfgtmrBase)->DCFG.bit.DTVSEL = (uint32_t)count;
}

/*!
 * @brief Config the generation of the trigger when the CFGTMR counter is equal
 * to the CNTIN register.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of initialization trigger
 *                   - true : To enable
 *                   - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigInitTriggerCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    (cfgtmrBase)->EXTTRG.bit.INITTRGEN = (uint32_t)enable;
}

/*!
 * @brief Read whether any channel trigger event has occurred.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval Channel trigger status
 *         - true : If there is a channel trigger event
 *         - false: If not.
 */
bool CFGTMR_HW_ReadChnTriggerGenerated(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->EXTTRG.bit.CHTRGFLG != CFGTMR_EXTTRG_CHTRGFLG_0) ? true : false;
}

/*!
 * @brief Clear the channel trigger flag.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_ClearChnTriggerFlag(CFGTMR_T *const cfgtmrBase)
{
    (cfgtmrBase)->EXTTRG.bit.CHTRGFLG = CFGTMR_EXTTRG_CHTRGFLG_0;
}

/*!
 * @brief Config the channel quadrature decoder.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of channel quadrature decoder
 *                  - true : To enable
 *                  - false: To disable
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadDecoderCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->QDCSTS.bit.QDMEN = CFGTMR_QDCSTS_QDMEN_1;
    }
    else
    {
        (cfgtmrBase)->QDCSTS.bit.QDMEN = CFGTMR_QDCSTS_QDMEN_0;
    }
}

/*!
 * @brief Config the phase A input filter.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of phase A input filter
 *                   - true : Enables the phase input filter
 *                   - false: Disables the filter
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadPhaseAFilterCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->QDCSTS.bit.PAIFEN = CFGTMR_QDCSTS_PAIFEN_1;
    }
    else
    {
        (cfgtmrBase)->QDCSTS.bit.PAIFEN = CFGTMR_QDCSTS_PAIFEN_0;
    }
}

/*!
 * @brief Config to select polarity for the quadrature decode phase A input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode Phase A input polarity selection
 *                - 0U: Normal polarity
 *                - 1U: Inverted polarity
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadPhaseAPolarity(CFGTMR_T *const cfgtmrBase, uint8_t mode)
{
    (cfgtmrBase)->QDCSTS.bit.PAISPOL = (uint32_t)mode;
}

/*!
 * @brief Config to select polarity for the quadrature decode phase B input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode Phase B input polarity selection
 *                - 0U: Normal polarity
 *                - 1U: Inverted polarity
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadPhaseBPolarity(CFGTMR_T *const cfgtmrBase, uint8_t mode)
{
    (cfgtmrBase)->QDCSTS.bit.PBISPOL = (uint32_t)mode;
}

/*!
 * @brief Config the encoding mode used in quadrature decoding mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param quadMode Quadrature decoder mode selection
 *                     - 0U: Phase A and Phase B encoding mode
 *                     - 1U: Count and direction encoding mode
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadMode(CFGTMR_T *const cfgtmrBase, uint8_t quadMode)
{
    (cfgtmrBase)->QDCSTS.bit.EMSEL = (uint32_t)quadMode;
}

/*!
 * @brief Read the CFGTMR counter direction in quadrature mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The counting direction
 *         - true: counting direction is increasing
 *         - false: counting direction is decreasing
 */
bool CFGTMR_HW_ReadQuadDir(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->QDCSTS.bit.CNTDIR != CFGTMR_QDCSTS_CNTDIR_0) ? true : false;
}

/*!
 * @brief Read the Timer overflow direction in quadrature mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The timer overflow direction
 *         - true: TOF bit was set on the top of counting
 *         - false: TOF bit was set on the bottom of counting
 */
bool CFGTMR_HW_ReadQuadTimerOverflowDir(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->QDCSTS.bit.TMRODSEL != CFGTMR_QDCSTS_TMRODSEL_0) ? true : false;
}

/*!
 * @brief Config the phase B input filter.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of phase B input filter
 *                   - true : Enables the phase input filter
 *                   - false: Disables the filter
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigQuadPhaseBFilterCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->QDCSTS.bit.PBIFEN = CFGTMR_QDCSTS_PBIFEN_1;
    }
    else
    {
        (cfgtmrBase)->QDCSTS.bit.PBIFEN = CFGTMR_QDCSTS_PBIFEN_0;
    }
}

/*!
 * @brief Config the fault input filter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value Fault input filter value
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigFaultInputFilterVal(CFGTMR_T *const cfgtmrBase, uint32_t value)
{
    (cfgtmrBase)->FCTRL.bit.FIFVSEL = value;
}

/*!
 * @brief Config the fault input filter.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param inputNum Fault input to be configured, valid value 0, 1, 2, 3
 * @param enable State of fault input filter
 *                   - true : To enable fault input filter
 *                   - false: To disable fault input filter
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigFaultInputFilterCmd(CFGTMR_T *const cfgtmrBase, uint8_t inputNum, bool enable)
{
    switch(inputNum)
    {
        case 0U:
            (cfgtmrBase)->FCTRL.bit.FIFEN0 = (uint32_t)enable;
            break;
        case 1U:
            (cfgtmrBase)->FCTRL.bit.FIFEN1 = (uint32_t)enable;
            break;
        case 2U:
            (cfgtmrBase)->FCTRL.bit.FIFEN2 = (uint32_t)enable;
            break;
        case 3U:
            (cfgtmrBase)->FCTRL.bit.FIFEN3 = (uint32_t)enable;
            break;
        default:
            break;
    }
}

/*!
 * @brief Clears the entire content value of the Fault control register.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_ClearFaultControl(CFGTMR_T *const cfgtmrBase)
{
    (cfgtmrBase)->FCTRL.reg =  0U;
}

/*!
 * @brief Config the fault input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param inputNum fault input to be configured, valid value 0, 1, 2, 3
 * @param enable State of fault input
 *                   - true : To enable fault input
 *                   - false: To disable fault input
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigFaultInputCmd(CFGTMR_T *const cfgtmrBase, uint8_t inputNum, bool enable)
{
    switch(inputNum)
    {
        case 0U:
            (cfgtmrBase)->FCTRL.bit.FIEN0 = (uint32_t)enable;
            break;
        case 1U:
            (cfgtmrBase)->FCTRL.bit.FIEN1 = (uint32_t)enable;
            break;
        case 2U:
            (cfgtmrBase)->FCTRL.bit.FIEN2 = (uint32_t)enable;
            break;
        case 3U:
            (cfgtmrBase)->FCTRL.bit.FIEN3 = (uint32_t)enable;
            break;
        default:
            break;
    }
}

/*!
 * @brief Config the behavior of the PWM outputs when a fault is detected
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of fault output
 *                   - true : Output pins are set tri-state,
 *                   - false: Pins are set to a safe state determined by POL bits
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPwmFaultBehavior(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->FCTRL.bit.FEOSTS = CFGTMR_FCTRL_FEOSTS_1;
    }
    else
    {
        (cfgtmrBase)->FCTRL.bit.FEOSTS = CFGTMR_FCTRL_FEOSTS_0;
    }
}

/*!
 * @brief Config the provided value to the Inverting control register, inverting control on multiple channel pairs.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param regVal Value to be written to the register
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigInvctrlReg(CFGTMR_T *const cfgtmrBase, uint32_t regVal)
{
    (cfgtmrBase)->ICTRL.reg = regVal;
}

/*!
 * @brief Config the channel software output control.
 *
 * @details The main difference between this function and CFGTMR_HAL_SetChnSoftwareCtrlCmd
 * is that this can configure all the channels in the same time.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channelsCfg, Channels to be enabled or disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigAllChnSoftwareCtrlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channelsCfg)
{
    (cfgtmrBase)->SWOCTRL.reg = (((cfgtmrBase)->SWOCTRL.reg) & (~(0xFFU))) | (uint32_t)channelsCfg;
}

/*!
 * @brief Config the channel software output control value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 * @param channelsValues The values which will overwrite the output channels
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigAllChnSoftwareCtrlVal(CFGTMR_T *const cfgtmrBase, uint8_t channelsValues)
{
    (cfgtmrBase)->SWOCTRL.reg = (((cfgtmrBase)->SWOCTRL.reg) & (~(0xFF00U))) | (uint32_t)(channelsValues << 8U);
}

/*!
 * @brief Config and Force the software control of channels output at once.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 * @param value The values which enables and force the software control of channels output
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigSoftwareOutputControl(CFGTMR_T *const cfgtmrBase, uint16_t value)
{
   (cfgtmrBase)->SWOCTRL.reg = (uint32_t)value;
}

/*!
 * @brief Config the BDM mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param val The CFGTMR behavior in BDM mode
 *                - CFGTMR_BDM_MODE_00: CFGTMR counter stopped, CH(n)F bit can be set, CFGTMR channels
 *                                   in functional mode, writes to MOD,CNTIN and C(n)V registers bypass
 *                                   the register buffers
 *                - CFGTMR_BDM_MODE_01: CFGTMR counter stopped, CH(n)F bit is not set, CFGTMR channels
 *                                   outputs are forced to their safe value , writes to MOD,CNTIN and
 *                                   C(n)V registers bypass the register buffers
 *                - CFGTMR_BDM_MODE_10: CFGTMR counter stopped, CH(n)F bit is not set, CFGTMR channels
 *                                   outputs are frozen when chip enters in BDM mode, writes to MOD,
 *                                   CNTIN and C(n)V registers bypass the register buffers
 *                - CFGTMR_BDM_MODE_11: CFGTMR counter in functional mode, CH(n)F bit can be set,
 *                                   CFGTMR channels in functional mode, writes to MOD,CNTIN and C(n)V
 *                                   registers is in fully functional mode
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigBdmMode(CFGTMR_T *const cfgtmrBase, CFGTMR_BDM_MODE_T val)
{
    cfgtmrBase->CFG.bit.DBG = (uint32_t)val;
}

/*!
 * @brief Config the sync mode for the CFGTMR SWOCTRL register when using a hardware trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of software output control synchronization
 *                   - true : The hardware trigger activates SWOCTRL register sync
 *                   - false: The hardware trigger does not activate SWOCTRL register sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigSwoctrlHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTASWO = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR INVCTRL register when using a hardware trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of inverting control synchronization
 *                   - true : The hardware trigger activates INVCTRL register sync
 *                   - false: The hardware trigger does not activate INVCTRL register sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigInvctrlHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTAICTRL = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR OUTMASK register when using a hardware trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of output mask synchronization
 *                   - true : The hardware trigger activates OUTMASK register sync
 *                   - false: The hardware trigger does not activate OUTMASK register sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigOutmaskHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTAOM = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR MOD, CNTIN and CV registers when using a hardware trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of registers synchronization
 *                   - true : The hardware trigger activates  MOD, HCR, CNTIN, and CV registers sync
 *                   - false: The hardware trigger does not activate MOD, HCR, CNTIN, and CV registers sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigModCntinCvHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTAWRBUF = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR counter register when using a hardware trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of CFGTMR counter synchronization
 *                   - true : The hardware trigger activates CFGTMR counter sync
 *                   - false: The hardware trigger does not activate CFGTMR counter sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigCounterHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTAC = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR SWOCTRL register when using a software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of software output control synchronization
 *                   - true : The software trigger activates SWOCTRL register sync
 *                   - false: The software trigger does not activate SWOCTRL register sync
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigSwoctrlSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.SWTASWO = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR INVCTRL register when using a software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of State of inverting control synchronization
 *                   - true : The software trigger activates INVCTRL register sync
 *                   - false: The software trigger does not activate INVCTRL register sync
 *
 * @retval None
 */
void CFGTMR_HW_ConfigInvctrlSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.SWTAICTRL = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR OUTMASK register when using a software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of output mask synchronization
 *                   - true : The software trigger activates OUTMASK register sync
 *                   - false: The software trigger does not activate OUTMASK register sync
 *
 * @retval None
 */
void CFGTMR_HW_ConfigOutmaskSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.SWTAOM = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR MOD, CNTIN and CV registers when using a software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of registers synchronization
 *                   - true : The software trigger activates CFGTMR MOD, CNTIN and CV registers sync
 *                   - false: The software trigger does not activate CFGTMR MOD, CNTIN and CV registers sync
 *
 * @retval None
 */
void CFGTMR_HW_ConfigModCntinCvSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.SWTAWRBUF = (uint32_t)enable;
}

/*!
 * @brief Config hardware trigger mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of hardware trigger mode
 *                   - true : CFGTMR does not clear the TRIGx bit when the hardware trigger j is detected
 *                   - false: CFGTMR clears the TRIGx bit when the hardware trigger j is detected
 *
 * @retval None
 */
void CFGTMR_HW_ConfigHwTriggerSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->SYNCFG.bit.HWTRGD = (uint32_t)enable;
}

/*!
 * @brief Config sync mode for CFGTMR counter register when using a software trigger.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param update_mode State of CFGTMR counter synchronization
 *                   - CFGTMR_UPDATE_NOW : The software trigger activates CFGTMR counter sync
 *                   - CFGTMR_WAIT_LOADING_POINTS: The software trigger does not activate CFGTMR counter sync
 *
 * @retval None
 */
void CFGTMR_HW_ConfigCounterSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_PWM_SYNC_MODE_T update_mode)
{
    cfgtmrBase->SYNCFG.bit.SWTAC = (uint32_t)update_mode;
}

/*!
 * @brief Config the PWM synchronization mode to enhanced or legacy.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode State of synchronization mode
 *                   - true : Enhanced PWM synchronization is selected
 *                   - false: Legacy PWM synchronization is selected
 *
 * @retval None
 */
void CFGTMR_HW_ConfigPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool mode)
{
    cfgtmrBase->SYNCFG.bit.SYNMSEL = (uint32_t)mode;
}

/*!
 * @brief Config the SWOCTRL register PWM synchronization mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode State of register synchronization
 *                   - CFGTMR_PWM_SYNC : SWOCTRL register is updated by PWM sync
 *                   - CFGTMR_SYSTEM_CLOCK: SWOCTRL register is updated at all rising edges of system clock
 *
 * @retval None
 */
void CFGTMR_HW_ConfigSwoctrlPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode)
{
    cfgtmrBase->SYNCFG.bit.SWOCTRLSYN = (uint32_t)mode;
}

/*!
 * @brief Config the INVCTRL register PWM synchronization mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode State of register synchronization
 *                   - CFGTMR_PWM_SYNC : INVCTRL register is updated by PWM sync
 *                   - CFGTMR_SYSTEM_CLOCK: INVCTRL register is updated at all rising edges of system clock
 *
 * @retval None
 */
void CFGTMR_HW_ConfigInvctrlPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode)
{
    cfgtmrBase->SYNCFG.bit.ICTRLSYN = (uint32_t)mode;
}

/*!
 * @brief Config the CNTIN register PWM synchronization mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param mode State of register synchronization
 *                   - CFGTMR_PWM_SYNC : CNTIN register is updated by PWM sync
 *                   - CFGTMR_SYSTEM_CLOCK: CNTIN register is updated at all rising edges of system clock
 *
 * @retval None
 */
void CFGTMR_HW_ConfigCntinPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode)
{
    cfgtmrBase->SYNCFG.bit.INITVSYN = (uint32_t)mode;
}



#if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
/*!
 * @brief Config the modulo fractional value in the PWM dithering.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param value The value to be set to the fractional value for the modulo
 */
void CFGTMR_HW_ConfigMMVVal(CFGTMR_T * const cfgtmrBase, uint8_t value)
{
    if ((cfgtmrBase == CFGTMR1) || (cfgtmrBase == CFGTMR2))
    {
        cfgtmrBase->MMV.reg = ((cfgtmrBase->MMV.reg) & ~(0xF800U)) | (((uint32_t)(((uint32_t)(value))<<11U))&0xF800U);
    }
}

/*!
 * @brief Config the channel (n) match fractional value.
 *
 * @param[in] cfgtmrBase The CFGTMR base address pointer
 * @param[in] channel The CFGTMR peripheral channel (n)
 * @param[in] value The value to be set to the channel (n) match fractional value
 */
void CFGTMR_HW_ConfigMCHMVal(CFGTMR_T * const cfgtmrBase, uint8_t channel, uint8_t value)
{
    if ((cfgtmrBase == CFGTMR1) || (cfgtmrBase == CFGTMR2))
    {
        cfgtmrBase->MCHM[channel].reg = ((cfgtmrBase->MCHM[channel].reg) & ~(0xF800U)) | (((uint32_t)(((uint32_t)(value))<<11U))&0xF800U);
    }
}
#endif



/*!
 * @brief Config the sync bit as a hardware trigger source for CFGTMR instance.
 *
 * @param simBase The SIM base address pointer.
 * @param ins The instance number of the CFGTMR module.
 * @param enable Enable/Disable the sync bit.
 *
 * @retval None
 */
void CFGTMR_HW_ConfigSyncBit(SIM_T * const simBase, uint32_t ins, bool enable)
{
    uint32_t instTemp = ins;

    if (true == enable)
    {
        switch(instTemp)
        {
            case 0U:
                simBase->CFGTMROPT1.bit.CFGTMR0SYNC = 1U;
                break;
            case 1U:
                simBase->CFGTMROPT1.bit.CFGTMR1SYNC = 1U;
                break;
            case 2U:
                simBase->CFGTMROPT1.bit.CFGTMR2SYNC = 1U;
                break;
            case 3U:
                simBase->CFGTMROPT1.bit.CFGTMR3SYNC = 1U;
                break;
#if (CFGTMR_INSTANCE_COUNT > 4)
            case 4U:
                simBase->CFGTMROPT1.bit.CFGTMR4SYNC = 1U;
                break;
            case 5U:
                simBase->CFGTMROPT1.bit.CFGTMR5SYNC = 1U;
                break;
#endif
            default:
                break;
        }
    }
    else
    {
        switch(instTemp)
        {
            case 0U:
                simBase->CFGTMROPT1.bit.CFGTMR0SYNC = 0U;
                break;
            case 1U:
                simBase->CFGTMROPT1.bit.CFGTMR1SYNC = 0U;
                break;
            case 2U:
                simBase->CFGTMROPT1.bit.CFGTMR2SYNC = 0U;
                break;
            case 3U:
                simBase->CFGTMROPT1.bit.CFGTMR3SYNC = 0U;
                break;
#if (CFGTMR_INSTANCE_COUNT > 4)
            case 4U:
                simBase->CFGTMROPT1.bit.CFGTMR4SYNC = 0U;
                break;
            case 5U:
                simBase->CFGTMROPT1.bit.CFGTMR5SYNC = 0U;
                break;
#endif
            default:
                break;
        }
    }
}


/*!
 * @brief Config the filter Pre-scaler divider.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param filterPrescale The CFGTMR peripheral clock pre-scale divider
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigClockFilterPs(CFGTMR_T *const cfgtmrBase, uint8_t filterPrescale)
{
    cfgtmrBase->CSTS.bit.FPSCSEL = (uint32_t)filterPrescale;
}

/*!
 * @brief Read the CFGTMR filter clock divider.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The CFGTMR filter clock pre-scale divider
 */
uint8_t CFGTMR_HW_ReadClockFilterPs(const CFGTMR_T *cfgtmrBase)
{
    return (uint8_t)(cfgtmrBase->CSTS.bit.FPSCSEL);
}

/*!
 * @brief Read the CFGTMR peripheral current counter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval The current CFGTMR timer counter value
 */
uint16_t CFGTMR_HW_ReadCounterValue(const CFGTMR_T *cfgtmrBase)
{
    return (uint16_t)((cfgtmrBase)->CNT.bit.CNT);
}

/*!
 * @brief Returns the CFGTMR peripheral counter modulo value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval CFGTMR timer modulo value
 */
uint16_t CFGTMR_HW_ReadMod(const CFGTMR_T *cfgtmrBase)
{
    return (uint16_t)((cfgtmrBase)->MOD.reg);
}

/*!
 * @brief Returns the CFGTMR peripheral counter initial value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval CFGTMR timer counter initial value
 */
uint16_t CFGTMR_HW_ReadCounterInitVal(const CFGTMR_T *cfgtmrBase)
{
    return (uint16_t)((cfgtmrBase)->INITV.reg);
}

/*!
 * @brief Clears the content of Channel (n) Status And Control.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel  The CFGTMR peripheral channel number
 *
 * @retval None.
 */
void CFGTMR_HW_ClearChSC(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->CONTROLS[channel].CHnCSTS.reg;
    (void)temp;

    ((cfgtmrBase)->CONTROLS[channel].CHnCSTS.reg) = 0U;
}

/*!
 * @brief Read the CFGTMR peripheral timer channel edge level.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval The ELSnB:ELSnA mode value, will be 00, 01, 10, 11
 */
uint8_t CFGTMR_HW_ReadChnEdgeLevel(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    uint32_t retValue;

    retValue = cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL0;

    retValue |= ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHMSEL1) << 1U);

    return (uint8_t)retValue;
}


/*!
 * @brief Config the feature of CFGTMR counter reset by the selected input capture event.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param enable Enable the CFGTMR counter reset
 *                   - true : CFGTMR counter is reset
 *                   - false: CFGTMR counter is not reset
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnIcrstCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    /* Write CRSTCTRL bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CRSTCTRL = (uint32_t)enable;
}

/*!
 * @brief Read whether the CFGTMR CFGTMR counter is reset.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @retval State of the CFGTMR peripheral timer channel CRSTCTRL
 *         - true : Enabled the CFGTMR counter reset
 *         - false: Disabled the CFGTMR counter reset
 */
bool CFGTMR_HW_ReadChnIcrstStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CRSTCTRL) != CFGTMR_CHnCSTS_CRSTCTRL_0) ? true : false;
}

/*!
 * @brief Config the CFGTMR peripheral timer channel DMA.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param enable Enable DMA transfers for the channel
 *                   - true : Enabled DMA transfers
 *                   - false: Disabled DMA transfers
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnDmaCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    /* Write DMAEN bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.DMAEN = (uint32_t)enable;
}

/*!
 * @brief Read whether the CFGTMR peripheral timer channel DMA is enabled.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @retval State of the CFGTMR peripheral timer channel DMA
 *         - true : Enabled DMA transfers
 *         - false: Disabled DMA transfers
 */
bool CFGTMR_HW_ReadChnDmaStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.DMAEN) != CFGTMR_CHnCSTS_DMAEN_0) ? true : false;
}

/*!
 * @brief Config the trigger generation on CFGTMR channel outputs.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param enable Trigger mode control
 *                   - false : Enable PWM output without generating a pulse
 *                   - true  : Disable a trigger generation on channel output
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigTrigModeControlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    /* Write CHOTMCTRL bit */
    cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHOTMCTRL = (uint32_t)enable;
}

/*!
 * @brief Read whether the trigger mode is enabled.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @retval State of the channel outputs
 *         - true : Enabled a trigger generation on channel output
 *         - false: PWM outputs without generating a pulse
 */
bool CFGTMR_HW_ReadTriggerControled(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHOTMCTRL) != CFGTMR_CHnCSTS_CHOTMCTRL_0) ? true : false;
}

/*!
 * @brief Read the state of channel input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @retval State of the channel inputs
 *         - true : The channel input is one
 *         - false: The channel input is zero
 */
bool CFGTMR_HW_ReadChInputStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHISTS) != CFGTMR_CHnCSTS_CHISTS_0) ? true : false;
}

/*!
 * @brief Read the value of channel output.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @retval Value of the channel outputs
 *         - true : The channel output is one
 *         - false: The channel output is zero
 */
bool CFGTMR_HW_ReadChOutputValue(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return ((cfgtmrBase->CONTROLS[channel].CHnCSTS.bit.CHOLSEL) != CFGTMR_CHnCSTS_CHOLSEL_0) ? true : false;
}

/*!
 * @brief Read the CFGTMR peripheral timer channel counter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval Channel counter value
 */
uint16_t CFGTMR_HW_ReadChnCountVal(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    return (uint16_t)(cfgtmrBase->CONTROLS[channel].CHnV.reg);
}

/*!
 * @brief Read the CFGTMR peripheral timer  channel event status.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval Channel event status
 *         - true  : A channel event has occurred
 *         - false : No channel event has occurred
 */
bool CFGTMR_HW_ReadChnEventStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    bool retValue = false;

    switch(channel)
    {
        case 0U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG0 != CFGTMR_CCSTS_CHEVFLG0_0) ? true : false;
            break;
        case 1U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG1 != CFGTMR_CCSTS_CHEVFLG1_0) ? true : false;
            break;
        case 2U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG2 != CFGTMR_CCSTS_CHEVFLG2_0) ? true : false;
            break;
        case 3U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG3 != CFGTMR_CCSTS_CHEVFLG3_0) ? true : false;
            break;
        case 4U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG4 != CFGTMR_CCSTS_CHEVFLG4_0) ? true : false;
            break;
        case 5U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG5 != CFGTMR_CCSTS_CHEVFLG5_0) ? true : false;
            break;
        case 6U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG6 != CFGTMR_CCSTS_CHEVFLG6_0) ? true : false;
            break;
        case 7U:
            retValue = ((cfgtmrBase)->CCSTS.bit.CHEVFLG7 != CFGTMR_CCSTS_CHEVFLG7_0) ? true : false;
            break;
        default:
            break;
    }
    return retValue;
}

/*!
 * @brief Read the CFGTMR peripheral timer status information for all channels.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval Channel event status value
 */
uint32_t CFGTMR_HW_ReadEventStatus(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->CCSTS.reg) & (0xFFU);
}

/*!
 * @brief Clears the CFGTMR peripheral timer all channel event status.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 *
 * @retval None.
 */
void CFGTMR_HW_ClearChnEventStatus(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->CCSTS.reg;
    (void)temp;

    switch(channel)
    {
        case 0U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG0 = CFGTMR_CCSTS_CHEVFLG0_0;
            break;
        case 1U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG1 = CFGTMR_CCSTS_CHEVFLG1_0;
            break;
        case 2U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG2 = CFGTMR_CCSTS_CHEVFLG2_0;
            break;
        case 3U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG3 = CFGTMR_CCSTS_CHEVFLG3_0;
            break;
        case 4U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG4 = CFGTMR_CCSTS_CHEVFLG4_0;
            break;
        case 5U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG5 = CFGTMR_CCSTS_CHEVFLG5_0;
            break;
        case 6U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG6 = CFGTMR_CCSTS_CHEVFLG6_0;
            break;
        case 7U:
            (cfgtmrBase)->CCSTS.bit.CHEVFLG7 = CFGTMR_CCSTS_CHEVFLG7_0;
            break;
        default:
            break;
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel output mask.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param mask Value to set Output Mask
 *                 - true : Channel output is masked
 *                 - false: Channel output is not masked
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnOutputMask(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool mask)
{
    if(channel < 8U)
    {
        if (mask)
        {
            cfgtmrBase->OMASK.reg |= (uint32_t) 1 << channel;
        }
        else
        {
            cfgtmrBase->OMASK.reg &= ~((uint32_t) 1 << channel);
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel output initial state 0 or 1.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel number
 * @param state Initial state for channels output
 *                  - true : The initialization value is 1
 *                  - false: The initialization value is 0
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnOutputInitStateCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool state)
{
    if (state)
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->CHOI.bit.CHOISEL0 = CFGTMR_CHOI_CHOISEL0_1;
                break;
            case 1U:
                (cfgtmrBase)->CHOI.bit.CHOISEL1 = CFGTMR_CHOI_CHOISEL1_1;
                break;
            case 2U:
                (cfgtmrBase)->CHOI.bit.CHOISEL2 = CFGTMR_CHOI_CHOISEL2_1;
                break;
            case 3U:
                (cfgtmrBase)->CHOI.bit.CHOISEL3 = CFGTMR_CHOI_CHOISEL3_1;
                break;
            case 4U:
                (cfgtmrBase)->CHOI.bit.CHOISEL4 = CFGTMR_CHOI_CHOISEL4_1;
                break;
            case 5U:
                (cfgtmrBase)->CHOI.bit.CHOISEL5 = CFGTMR_CHOI_CHOISEL5_1;
                break;
            case 6U:
                (cfgtmrBase)->CHOI.bit.CHOISEL6 = CFGTMR_CHOI_CHOISEL6_1;
                break;
            case 7U:
                (cfgtmrBase)->CHOI.bit.CHOISEL7 = CFGTMR_CHOI_CHOISEL7_1;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->CHOI.bit.CHOISEL0 = CFGTMR_CHOI_CHOISEL0_0;
                break;
            case 1U:
                (cfgtmrBase)->CHOI.bit.CHOISEL1 = CFGTMR_CHOI_CHOISEL1_0;
                break;
            case 2U:
                (cfgtmrBase)->CHOI.bit.CHOISEL2 = CFGTMR_CHOI_CHOISEL2_0;
                break;
            case 3U:
                (cfgtmrBase)->CHOI.bit.CHOISEL3 = CFGTMR_CHOI_CHOISEL3_0;
                break;
            case 4U:
                (cfgtmrBase)->CHOI.bit.CHOISEL4 = CFGTMR_CHOI_CHOISEL4_0;
                break;
            case 5U:
                (cfgtmrBase)->CHOI.bit.CHOISEL5 = CFGTMR_CHOI_CHOISEL5_0;
                break;
            case 6U:
                (cfgtmrBase)->CHOI.bit.CHOISEL6 = CFGTMR_CHOI_CHOISEL6_0;
                break;
            case 7U:
                (cfgtmrBase)->CHOI.bit.CHOISEL7 = CFGTMR_CHOI_CHOISEL7_0;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Disables the CFGTMR peripheral timer fault interrupt.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_DisableFaultInt(CFGTMR_T *const cfgtmrBase)
{
    (cfgtmrBase)->FMSEL.bit.FCTRLIEN = CFGTMR_FMSEL_FCTRLIEN_0;
}

/*!
 * @brief Config the CFGTMR peripheral timer capture test mode.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable Capture Test Mode Enable
 *            - true : Capture test mode is enabled
 *            - false: Capture test mode is disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigCaptureTestCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    (cfgtmrBase)->FMSEL.bit.CTMEN = (uint32_t)enable;
}

/*!
 * @brief Read status of the CFGTMREN bit in the CFGTMR_FMSEL register.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @retval the CFGTMR Enable status
 *         - true : TPM compatibility. Free running counter and synchronization compatible with TPM
 *         - false: Free running counter and synchronization are different from TPM behavior
 */
bool CFGTMR_HW_ReadCFGtmrEnableStatus(const CFGTMR_T *cfgtmrBase)
{
    return ((cfgtmrBase)->FMSEL.bit.CFGTMREN != (uint32_t)CFGTMR_FMSEL_CFGTMREN_0) ? true : false;
}

/*!
 * @brief Determines if the CFGTMR counter is re-initialized when the selected trigger for
 * synchronization is detected.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable CFGTMR counter re-initialization selection
 *                   - true : To update CFGTMR counter when triggered
 *                   - false: To count normally
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigCountReinitSyncCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    (cfgtmrBase)->SYN.bit.CFGTMRCRINT = (uint32_t)enable;
}

/*!
 * @brief Read whether the write protection is enabled.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval Write-protection status
 *         - true : If enabled
 *         - false: If not
 */
bool CFGTMR_HW_ReadWriteProtectionEnabledStatus(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->FMSTS.bit.WPEN != CFGTMR_FMSTS_WPEN_0) ? true : false;
}

/*!
 * @brief Read whether the logic OR of the fault inputs is enabled.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval the enabled fault inputs status
 *         - true : The logic OR of the enabled fault inputs is 1
 *         - false: The logic OR of the enabled fault inputs is 0
 */
bool CFGTMR_HW_ReadFaultInputEnabledStatus(const CFGTMR_T *cfgtmrBase)
{
    return (cfgtmrBase->FMSTS.bit.FICTRL != CFGTMR_FMSTS_FICTRL_0) ? true : false;
}

/*!
 * @brief Read whether a fault condition is detected at the fault input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel
 *
 * @retval the fault condition status
 *         - true : A fault condition was detected at the fault input
 *         - false: No fault condition was detected at the fault input
 */
bool CFGTMR_HW_ReadFaultFlagDetectedStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel)
{
    uint32_t retValue = 0U;
    switch(channel)
    {
        case 0U:
            retValue = (cfgtmrBase->FMSTS.bit.FIDFLG0 != CFGTMR_FMSTS_FIDFLG0_0) ? true : false;
            break;
        case 1U:
            retValue = (cfgtmrBase->FMSTS.bit.FIDFLG1 != CFGTMR_FMSTS_FIDFLG1_0) ? true : false;
            break;
        case 2U:
            retValue = (cfgtmrBase->FMSTS.bit.FIDFLG2 != CFGTMR_FMSTS_FIDFLG2_0) ? true : false;
            break;
        case 3U:
            retValue = (cfgtmrBase->FMSTS.bit.FIDFLG3 != CFGTMR_FMSTS_FIDFLG3_0) ? true : false;
            break;
        default:
            break;
    }

    return retValue;
}

/*!
 * @brief Clear a fault condition is detected at the fault input.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel The CFGTMR peripheral channel
 *
 * @retval None.
 */
void CFGTMR_HW_ClearFaultFlagDetectedStatus(CFGTMR_T *const cfgtmrBase, uint8_t channel)
{
    uint32_t temp;
    /* Read-then-write sequence to guarantee required serialization of memory operations */
    temp = cfgtmrBase->FMSTS.reg;
    (void)temp;

    switch(channel)
    {
        case 0U:
            cfgtmrBase->FMSTS.bit.FIDFLG0 = CFGTMR_FMSTS_FIDFLG0_0;
            break;
        case 1U:
            cfgtmrBase->FMSTS.bit.FIDFLG1 = CFGTMR_FMSTS_FIDFLG1_0;
            break;
        case 2U:
            cfgtmrBase->FMSTS.bit.FIDFLG2 = CFGTMR_FMSTS_FIDFLG2_0;
            break;
        case 3U:
            cfgtmrBase->FMSTS.bit.FIDFLG3 = CFGTMR_FMSTS_FIDFLG3_0;
            break;
        default:
            break;
    }
}

/*!
 * @brief Config the channel invert for a channel pair.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param chnlPairNum The CFGTMR peripheral channel pair number
 * @param enable State of channel invert for a channel pair
 *                   - true : To enable channel inverting
 *                   - false: To disable channel inversion
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigDualChnInvertCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable)
{
    if(chnlPairNum < 4U)
    {
        if (enable)
        {
            cfgtmrBase->ICTRL.reg |= (uint32_t)(1 << chnlPairNum);
        }
        else
        {
            cfgtmrBase->ICTRL.reg &= ~(uint32_t)(1 << chnlPairNum);
        }

    }
}

/*!
 * @brief Config the channel software output control.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel Channel to be enabled or disabled
 * @param enable State of channel software output control
 *                   - true : To enable the channel output will be affected by software output control
 *                   - false: To disable the channel output is unaffected
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnSoftwareCtrlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    if(channel < 8U)
    {
        if (enable)
        {
            cfgtmrBase->SWOCTRL.reg |= (uint32_t)(1 << channel);
        }
        else
        {
            cfgtmrBase->SWOCTRL.reg &= ~(uint32_t)(1 << channel);
        }
    }
}

/*!
 * @brief Config the channel software output control value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 * @param channel Channel to be configured
 * @param enable State of software output control value
 *                   - true : to force 1 to the channel output
 *                   - false: to force 0 to the channel output
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnSoftwareCtrlVal(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    if ((uint32_t)enable != (((uint32_t)cfgtmrBase->CHPOL.reg >> (uint32_t)channel) & (uint32_t)1U))
    {
        ((cfgtmrBase)->SWOCTRL.reg) |=  (1UL << (channel + 8U));
    }
    else
    {
        ((cfgtmrBase)->SWOCTRL.reg) &=  ~(1UL << (channel + 8U));
    }
}

/*!
 * @brief Enable the global load mechanism.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 *
 * @retval None.
 */
void CFGTMR_HW_EnableGlobalLoadCmd(CFGTMR_T *const cfgtmrBase)
{
    (cfgtmrBase)->PWMLD.reg |= (uint32_t)(CFGTMR_PWMLD_GLOP_1 << 11U);
}

/*!
 * @brief Config the global load.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of the global load mechanism
 *                   - true : Global Load OK enabled
 *                   - false: Global Load OK disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigLoadCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->PWMLD.reg |= (uint32_t)((CFGTMR_PWMLD_GLOKEN_1 << 10U));
    }
    else
    {
        (cfgtmrBase)->PWMLD.reg &= (uint32_t)~((CFGTMR_PWMLD_GLOKEN_1 << 10U));
    }
}

/*!
 * @brief Config the half cycle reload.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of the half cycle match as a reload opportunity
 *                   - true : Half cycle reload is enabled
 *                   - false: Half cycle reload is disabled
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigHalfCycleCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->PWMLD.reg |= (uint32_t)((CFGTMR_PWMLD_HCREN_1 << 8U));
    }
    else
    {
        (cfgtmrBase)->PWMLD.reg &= (uint32_t)~((CFGTMR_PWMLD_HCREN_1 << 8U));
    }
}

/*!
 * @brief Config the loading of MOD, INITV and CV with values of their write buffer.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of loading updated values
 *                   - true : To enable the loading of value of their buffer
 *                   - false: To disable the loading of value of their buffer
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPwmLoadCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    if (enable)
    {
        (cfgtmrBase)->PWMLD.reg |= (uint32_t)((CFGTMR_PWMLD_LUVEN_1 << 9U));
    }
    else
    {
        (cfgtmrBase)->PWMLD.reg &= (uint32_t)~((CFGTMR_PWMLD_LUVEN_1 << 9U));
    }
}

/*!
 * @brief Config to includes or excludes the channel in the matching process.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channel Channel to be configured
 * @param enable State of channel
 *                - true : means include the channel in the matching process
 *                - false: means do not include channel in the matching process
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPwmLoadChnSelCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    if(channel < 8U)
    {
        if (enable)
        {
            cfgtmrBase->PWMLD.reg |= (uint32_t)(1U << channel);
        }
        else
        {
            cfgtmrBase->PWMLD.reg &= ~(uint32_t)(1U << channel);
        }
    }
}

/*!
 * @brief Config the CFGTMR initialization trigger on Reload Point.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable bit controls whether an initialization trigger is generated
 *                   - true : Trigger is generated when a reload point is reached
 *                   - false: Trigger is generated on counter wrap events
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigInitTrigOnReloadCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->CFG.bit.INITTRGRP = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR global time base signal generation to other CFGTMR's.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of global time base signal
 *                   - true : To enable the golobal time base generation to other CFGTMR instances
 *                   - false: To disable the golobal time base generation to other CFGTMR instances
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigGlobalTimeBaseOutputCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->CFG.bit.GTIMBSGEN = (uint32_t)enable;
}

/*!
 * @brief Config the CFGTMR timer global time base.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param enable State of global time base
 *                   - true : To enable an external global time base signal
 *                   - false: To disable an external global time base signal
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigGlobalTimeBaseCmd(CFGTMR_T *const cfgtmrBase, bool enable)
{
    cfgtmrBase->CFG.bit.EGTIMBEN = (uint32_t)enable;
}

/*!
 * @brief Config the frequency of reload points
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param val Value of the TOF bit set frequency
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigLoadFreq(CFGTMR_T *const cfgtmrBase, uint8_t val)
{
    cfgtmrBase->CFG.bit.ROCFG = (uint32_t)val;
}

/*!
 * @brief Config the CFGTMR extended dead-time value for the channel pair.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channelPair The CFGTMR peripheral channel pair (n)
 * @param value The CFGTMR peripheral extend pre-scale divider using the concatenation with the dead-time value
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigExtPairDeadtimeValue(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, uint8_t value)
{
    switch (channelPair)
    {
        case CHAN0_IDX:
            cfgtmrBase->P0DCFG.bit.EDTV = (uint32_t)value;
            break;
        case CHAN1_IDX:
            cfgtmrBase->P1DCFG.bit.EDTV = (uint32_t)value;
            break;
        case CHAN2_IDX:
            cfgtmrBase->P2DCFG.bit.EDTV = (uint32_t)value;
            break;
        case CHAN3_IDX:
            cfgtmrBase->P3DCFG.bit.EDTV = (uint32_t)value;
            break;
        default:
            /* Nothing to do */
            break;
    }
}

/*!
 * @brief Config the CFGTMR dead time divider for the channel pair.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channelPair The CFGTMR peripheral channel pair (n)
 * @param divider The CFGTMR peripheral pre-scaler divider
 *                    - CFGTMR_DEADTIME_DIV_1 : Divide by 1
 *                    - CFGTMR_DEADTIME_DIV_4 : Divide by 4
 *                    - CFGTMR_DEADTIME_DIV_16: Divide by 16
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPairDeadtimePrescale(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, CFGTMR_DEADTIME_PSC_T divider)
{
    switch (channelPair)
    {
        case CHAN0_IDX:
            cfgtmrBase->P0DCFG.bit.PSCCFG = (uint32_t)divider;
            break;
        case CHAN1_IDX:
            cfgtmrBase->P1DCFG.bit.PSCCFG = (uint32_t)divider;
            break;
        case CHAN2_IDX:
            cfgtmrBase->P2DCFG.bit.PSCCFG = (uint32_t)divider;
            break;
        case CHAN3_IDX:
            cfgtmrBase->P3DCFG.bit.PSCCFG = (uint32_t)divider;
            break;
        default:
            /* Nothing to do */
            break;
    }
}

/*!
 * @brief Config the CFGTMR dead-time value for the channel pair.
 *
 * @param cfgtmrBase The CFGTMR base address pointer
 * @param channelPair The CFGTMR peripheral channel pair (n)
 * @param count The CFGTMR peripheral selects the dead-time value
 *                  - 0U : no counts inserted
 *                  - 1U : 1 count is inserted
 *                  - 2U : 2 count is inserted
 *                  - ... up to a possible 63 counts
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigPairDeadtimeCount(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, uint8_t count)
{
    switch (channelPair)
    {
        case CHAN0_IDX:
            cfgtmrBase->P0DCFG.bit.DVSEL = (uint32_t)count;
            break;
        case CHAN1_IDX:
            cfgtmrBase->P1DCFG.bit.DVSEL = (uint32_t)count;
            break;
        case CHAN2_IDX:
            cfgtmrBase->P2DCFG.bit.DVSEL = (uint32_t)count;
            break;
        case CHAN3_IDX:
            cfgtmrBase->P3DCFG.bit.DVSEL = (uint32_t)count;
            break;
        default:
            /* Nothing to do */
            break;
    }
}

/*!
 * @brief Config the CFGTMR.
 *
 * @param base The CFGTMR base address pointer.
 *
 * @param ftmClockPrescaler The CFGTMR_CLOCK_PSC_T pointer.
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigModule(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_PSC_T ftmClockPrescaler)
{
    /* Use CFGTMR mode */
    CFGTMR_HW_Enable(cfgtmrBase, true);
    CFGTMR_HW_ConfigClockPs(cfgtmrBase, ftmClockPrescaler);
}


/*!
 * @brief All the register use in the driver should be reset to default value of each register.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 *
 * @retval None.
 */
void CFGTMR_HW_Reset(CFGTMR_T *const cfgtmrBase)
{
    uint8_t chnIndex;

    /* WPUNLOCK is set when WPEN bit is read as a 1 and then 1 is written to WPUNLOCK */
    ((cfgtmrBase)->FMSTS.reg) &= 0U;
    /* This is the reset value for MODE register. WPUNLOCK bit is set to disable write protection */
    ((cfgtmrBase)->FMSEL.reg) = 0x00000004U;
    ((cfgtmrBase)->CSTS.reg) &= 0U;
    ((cfgtmrBase)->CNT.reg) = 0U;
    ((cfgtmrBase)->MOD.reg) = 0U;
    ((cfgtmrBase)->INITV.reg) = 0U;
    ((cfgtmrBase)->CCSTS.reg) &= 0U;
    ((cfgtmrBase)->SYN.reg) = 0U;
    ((cfgtmrBase)->CHOI.reg) = 0U;
    ((cfgtmrBase)->OMASK.reg) = 0U;
    ((cfgtmrBase)->DCFG.reg) = 0U;
    ((cfgtmrBase)->EXTTRG.reg) &= 0U;
    ((cfgtmrBase)->CHPOL.reg) = 0U;
    ((cfgtmrBase)->ICFCTRL.reg) = 0U;
    ((cfgtmrBase)->FCTRL.reg) = 0U;
    ((cfgtmrBase)->QDCSTS.reg) = 0U;
    ((cfgtmrBase)->CFG.reg) = 0U;
    ((cfgtmrBase)->FIPOLSEL.reg) = 0U;
    ((cfgtmrBase)->SYNCFG.reg) = 0U;
    ((cfgtmrBase)->ICTRL.reg) = 0U;
    ((cfgtmrBase)->SWOCTRL.reg) = 0U;
    ((cfgtmrBase)->PWMLD.reg) = 0U;
    ((cfgtmrBase)->HC.reg) = 0U;

    #if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
        if ((cfgtmrBase == CFGTMR1) || (cfgtmrBase == CFGTMR2))
        {
            ((cfgtmrBase)->MMV.reg) = 0U;
        }
    #endif

    /* Set to reset value all CnV and CnSC registers */
    for (chnIndex = 0U; chnIndex < FEATURE_CFGTMR_CHANNEL_NUM; chnIndex++)
    {
        ((cfgtmrBase)->CONTROLS[chnIndex].CHnCSTS.reg) &= 0U;
        ((cfgtmrBase)->CONTROLS[chnIndex].CHnV.reg) = 0U;
        #if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
            if ((cfgtmrBase == CFGTMR1) || (cfgtmrBase == CFGTMR2))
            {
                ((cfgtmrBase)->MCHM[chnIndex].reg) = 0U;
            }
        #endif
    }
}

/*!
 * @brief Config the generation of the CFGTMR peripheral timer channel trigger when the
 * CFGTMR counter is equal to its initial value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 *
 * @param channel  0-7.
 *
 * @param enable  true or false.
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnTriggerCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable)
{
    if (true == enable)
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH0 = CFGTMR_EXTTRG_EXTTRGENCH0_1;
                break;
            case 1U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH1 = CFGTMR_EXTTRG_EXTTRGENCH1_1;
                break;
            case 2U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH2 = CFGTMR_EXTTRG_EXTTRGENCH2_1;
                break;
            case 3U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH3 = CFGTMR_EXTTRG_EXTTRGENCH3_1;
                break;
            case 4U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH4 = CFGTMR_EXTTRG_EXTTRGENCH4_1;
                break;
            case 5U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH5 = CFGTMR_EXTTRG_EXTTRGENCH5_1;
                break;
            case 6U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH6 = CFGTMR_EXTTRG_EXTTRGENCH6_1;
                break;
            case 7U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH7 = CFGTMR_EXTTRG_EXTTRGENCH7_1;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(channel)
        {
            case 0U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH0 = CFGTMR_EXTTRG_EXTTRGENCH0_0;
                break;
            case 1U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH1 = CFGTMR_EXTTRG_EXTTRGENCH1_0;
                break;
            case 2U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH2 = CFGTMR_EXTTRG_EXTTRGENCH2_0;
                break;
            case 3U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH3 = CFGTMR_EXTTRG_EXTTRGENCH3_0;
                break;
            case 4U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH4 = CFGTMR_EXTTRG_EXTTRGENCH4_0;
                break;
            case 5U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH5 = CFGTMR_EXTTRG_EXTTRGENCH5_0;
                break;
            case 6U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH6 = CFGTMR_EXTTRG_EXTTRGENCH6_0;
                break;
            case 7U:
                (cfgtmrBase)->EXTTRG.bit.EXTTRGENCH7 = CFGTMR_EXTTRG_EXTTRGENCH7_0;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief Config the CFGTMR peripheral timer channel input capture filter value.
 *
 * @param cfgtmrBase The CFGTMR base address pointer.
 *
 * @param channel  0-3.
 *
 * @param value  input capture filter value.
 *
 * @retval None.
 */
void CFGTMR_HW_ConfigChnInputCaptureFilter(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t value)
{
    switch (channel)
    {
        case CHAN0_IDX:
            (cfgtmrBase)->ICFCTRL.bit.IFVSELCH0 = (uint32_t)value;
            break;
        case CHAN1_IDX:
            (cfgtmrBase)->ICFCTRL.bit.IFVSELCH1 = (uint32_t)value;
            break;
        case CHAN2_IDX:
            (cfgtmrBase)->ICFCTRL.bit.IFVSELCH2 = (uint32_t)value;
            break;
        case CHAN3_IDX:
            (cfgtmrBase)->ICFCTRL.bit.IFVSELCH3 = (uint32_t)value;
            break;
        default:
            /* Nothing to do */
            break;
    }
}

/**@} end of group CFGTMR_Functions*/
/**@} end of group CFGTMR_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
