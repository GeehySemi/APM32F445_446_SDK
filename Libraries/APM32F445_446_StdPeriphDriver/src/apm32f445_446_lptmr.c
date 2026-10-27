/*!
 * @file        apm32f445_446_lptmr.c
 *
 * @brief       This file provides all the LPTMR firmware functions
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
#include "apm32f445_446_lptmr.h"
#include "apm32f445_446_clock.h"

/** @addtogroup  APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPTMR_Driver LPTMR Driver
  @{
*/

/** @defgroup LPTMR_Marcos Marcos
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define LPTMR_INSTANCE_COUNT             (1U)
#define ASSERT_INSTANCE(__instance__)    ({if(__instance__ >= LPTMR_INSTANCE_COUNT) {while(1);}})

/**@} end of group LPTMR_Marcos*/

/** @defgroup LPTMR_Variables Variables
  @{
*/

/* Table of lptmrBase addresses for LPTMR instances */
static LPTMR_T *const g_lptmrBase[LPTMR_INSTANCE_COUNT] = { LPTMR };

/**@} end of group LPTMR_Variables*/

/** @defgroup LPTMR_Functions Functions
  @{
*/

/*!
 * @details
 * Timer Mode: Prescaler settings calculations
 * --------------------------------------------
 *
 * Timer Mode configuration takes a period (timeout) value expressed in
 * micro-seconds. To convert this to LPTMR prescaler (and compare value)
 * settings, the closest match must be found.
 * For best precision, the lowest prescaler that allows the corresponding
 * compare value to fit in the 16-bit register will be chosen.
 *
 * Algorithm for choosing prescaler and compare values:
 * =============================================================================
 * In: tper_us (period in microseconds), fclk (input clock frequency in Hertz)
 * Out: nticks (timer ticks), p (prescaler coefficient, 2^p = prescaler value)
 * ---
 * 1) Compute time = tper_us * fclk / 1000000
 * 2) for p = 0..16
 *  2.1) nticks = time / 2^p
 *  2.2) if nticks < 0x10000
 *      2.2.1) STOP, found nticks and p
 * 3) nticks = 0xFFFF, p = 16
 * =============================================================================
 *
 * A few names used throughout the static functions affecting Timer mode:
 *  time: total number of timer ticks (undivided, unprescaled) that is necessary
 *      for a particular timeout.
 *      time = (tper_us * fclk) / 1000000 = nticks * npresc
 *
 *  tper_us: a period (or timeout) expressed in microsecond units. In most
 *      functions will be denoted as 'us' for microseconds.
 *
 *  nticks: number of timer ticks that is necessary for a particular timeout,
 *      after prescaling
 *
 *  npresc: prescaler value (1, 2, 4 ... 65536)
 *
 *  p: prescaler coefficient, 2^p = npresc
 *
 *  fclk: input clock frequency, in Hertz. In most function will be denoted as
 *      'clkfreq'.
 */

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static uint8_t LPTMR_SettingToValue( const LPTMR_PRESCALER_T psc, const bool bypass);
static uint64_t LPTMR_UsToUndividedTime(const uint32_t clkfreq, const uint32_t us);
static uint64_t LPTMR_ComputeNticks(uint64_t time, uint8_t psc);
static bool LPTMR_NticksToCompareTicks(uint64_t nticks, uint16_t* ticks);
static uint32_t LPTMR_ReadClkFreq(const LPTMR_CLOCKSOURCE_T clkSrc, const uint32_t instance);

static bool LPTMR_TicksToUs(
    const uint32_t clkfreq,
    const LPTMR_PRESCALER_T psc,
    const bool bypass,
    const uint16_t ticks,
    uint32_t *const us);

static bool LPTMR_UsToTicks(
    const uint32_t clkfreq,
    const LPTMR_PRESCALER_T psc,
    const bool bypass,
    const uint32_t us,
    uint16_t *const ticks);

static bool LPTMR_ChooseClkConfig(
    const uint32_t clkfreq,
    const uint32_t us,
    LPTMR_PRESCALER_T *const psc,
    bool *const bypass,
    uint16_t *const ticks);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief  Initialize a LPTMR instance based on the input configuration structure.
 *
 * @details When (cntUnits == LPTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'psc' and 'bypassPsc' will be ignored
 *: their values will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * LPTMR_COUNTER_UNITS_MICROSECONDS may only be used for LPTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (cntUnits == LPTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'psc' and 'bypassPsc' provided in the input configuration structure.
 *
 * When (cntUnits == LPTMR_COUNTER_UNITS_TICKS), 'compareValue' must be lower
 * than 0xFFFFu. Only the least significant 16bits of 'compareValue' will be used.
 * When (cntUnits == LPTMR_COUNTER_UNITS_MICROSECONDS), 'compareValue'
 * may take any 32bits unsigned value.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param config: LPTMR_CONFIG_T pointer
 * @param startCounter: true/false
 *
 * @retval None
 */
void LPTMR_Init(const uint32_t instance,
    const LPTMR_CONFIG_T *const config,
    const bool startCounter)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_ConfigSetting(instance, config);

    /* Start the counter if requested */
    if (startCounter)
    {
        LPTMR_HW_Enable(lptmrBase);
    }
}

/*!
 * @brief Reset the LPTMR.
 *
 * @param instance: instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_DeInit(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    LPTMR_HW_Disable(lptmrBase);

    LPTMR_HW_ConfigForReset(lptmrBase);
}

/*!
 * @brief Configures a LPTMR instance based on the input configuration structure.
 *
 * @details When (cntUnits == LPTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'psc' and 'bypassPsc' will be ignored
 *: their values will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * LPTMR_COUNTER_UNITS_MICROSECONDS may only be used for LPTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (cntUnits == LPTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'psc' and 'bypassPsc' provided in the input configuration structure.
 *
 * When (cntUnits == LPTMR_COUNTER_UNITS_TICKS), 'compareValue' must be lower
 * than 0xFFFFu. Only the least significant 16bits of 'compareValue' will be used.
 * When (cntUnits == LPTMR_COUNTER_UNITS_MICROSECONDS), 'compareValue'
 * may take any 32bits unsigned value.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param config: LPTMR_CONFIG_T pointer
 *
 * @retval None
 */
void LPTMR_ConfigSetting(const uint32_t instance, const LPTMR_CONFIG_T *const config)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    LPTMR_PRESCALER_T prescVal = config->psc;
    LPTMR_WORKMODE_T configWorkMode = config->workMode;
    LPTMR_COUNTER_UNITS_T configCounterUnits = config->cntUnits;
    bool prescBypass = config->bypassPsc;
    uint32_t configCmpValue = config->compareValue;
    uint16_t cmpValueTicks = 0U;

    if(configWorkMode != LPTMR_WORKMODE_TIMER)
    {
        cmpValueTicks = (uint16_t)(configCmpValue & 0xFFFFU);
    }
    else
    {
        /* A valid clock must be selected when used in Timer Mode. */
        uint32_t clkFreq = LPTMR_ReadClkFreq(config->clkSelect, instance);

        if(configCounterUnits != LPTMR_COUNTER_UNITS_MICROSECONDS)
        {
            cmpValueTicks = (uint16_t)(configCmpValue & 0xFFFFU);
        }
        else
        {
            /* When workmode is set to Timer Mode and compare value is provided in microseconds,
             * then the input parameters for prescale value and prescaleBypass are ignored.
             * The prescaleValue, prescaleBypass and cmpValue in ticks, are calculated to best fit
             * the input configCmpValue (in us) for the current operating clk frequency.  */
            bool chooseClkConfigStatus =
                LPTMR_ChooseClkConfig(clkFreq, configCmpValue, &prescVal, &prescBypass, &cmpValueTicks);
            (void) chooseClkConfigStatus;
        }
    }

    /* Initialize and write configuration parameters. */
    LPTMR_HW_ConfigForReset(lptmrBase);

    LPTMR_HW_ConfigWorkMode(lptmrBase, configWorkMode);
    LPTMR_HW_ConfigClockSelect(lptmrBase, config->clkSelect);
    LPTMR_HW_ConfigPrescaler(lptmrBase, prescVal);
    LPTMR_HW_ConfigBypass(lptmrBase, prescBypass);
    LPTMR_HW_ConfigFreeRunning(lptmrBase, config->freeRun);
    LPTMR_HW_ConfigDmaRequest(lptmrBase, config->dmaReq);
    LPTMR_HW_ConfigInterrupt(lptmrBase, config->intEnable);
    LPTMR_HW_ConfigCompareValue(lptmrBase, cmpValueTicks);
    LPTMR_HW_ConfigPinSelect(lptmrBase, config->pinSelect);
    LPTMR_HW_ConfigPinPolarity(lptmrBase, config->pinPolarity);
}

/*!
 * @brief Initialize a configuration structure with default values.
 *
 * @param config: LPTMR_CONFIG_T pointer
 *
 * @retval None
 */
void  LPTMR_DefaultConfig(LPTMR_CONFIG_T *const config)
{
    /* Pulse Counter specific parameters */
    config->pinSelect       = LPTMR_PINSELECT_TRGMUX;
    config->pinPolarity     = LPTMR_PINPOLARITY_RISING;

    /* Counter parameters */
    config->clkSelect       = LPTMR_CLOCKSOURCE_LSICLKDIV2;
    config->psc             = LPTMR_PRESCALE_2;
    config->bypassPsc       = false;
    config->compareValue    = 0u;
    config->cntUnits        = LPTMR_COUNTER_UNITS_TICKS;

    /* General parameters */
    config->dmaReq          = false;
    config->intEnable       = false;
    config->freeRun         = false;
    config->workMode        = LPTMR_WORKMODE_TIMER;
}

/*!
 * @brief Read the current configuration of the LPTMR instance.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param config: LPTMR_CONFIG_T pointer
 *
 * @retval None
 */
void LPTMR_ReadConfig(const uint32_t instance, LPTMR_CONFIG_T *const config)
{
    ASSERT_INSTANCE(instance);

    const LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    /* Read current configuration */
    config->dmaReq       = LPTMR_HW_ReadDmaRequest(lptmrBase);
    config->intEnable    = LPTMR_HW_ReadInterruptEnable(lptmrBase);
    config->freeRun      = LPTMR_HW_ReadFreeRunning(lptmrBase);
    config->workMode     = LPTMR_HW_ReadWorkMode(lptmrBase);
    config->psc          = LPTMR_HW_ReadPrescaler(lptmrBase);
    config->bypassPsc    = LPTMR_HW_ReadBypass(lptmrBase);
    config->clkSelect    = LPTMR_HW_ReadClockSelect(lptmrBase);
    config->compareValue = LPTMR_HW_ReadCompareValue(lptmrBase);
    config->cntUnits     = LPTMR_COUNTER_UNITS_TICKS;
    config->pinSelect    = LPTMR_HW_ReadPinSelect(lptmrBase);
    config->pinPolarity  = LPTMR_HW_ReadPinPolarity(lptmrBase);
}

/*!
 * @brief Configures the compare value in counter tick units.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param compareValueByCount: the compare value
 *
 * @retval - STATUS_SUCCESS: completed successfully
 *         - STATUS_ERROR: cannot reconfigure compare value (TCF not set)
 *         - STATUS_TIMEOUT: compare value is smaller than current counter value
 */
STATUS_T LPTMR_ConfigCompareValueByCount(
    const uint32_t instance,
    const uint16_t compareValueByCount)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase  = g_lptmrBase[instance];
    STATUS_T status = STATUS_SUCCESS;

    bool lptmrEnabled = LPTMR_HW_ReadEnable(lptmrBase);
    bool compareFlag  = LPTMR_HW_ReadCompareFlagState(lptmrBase);

    uint16_t counterVal = 0;

    /* Check if a valid clock is selected for the timer/glitch filter */
    bool bypass = LPTMR_HW_ReadBypass(lptmrBase);
    LPTMR_WORKMODE_T workMode = LPTMR_HW_ReadWorkMode(lptmrBase);

    uint32_t clkfreq = LPTMR_ReadClkFreq(LPTMR_HW_ReadClockSelect(lptmrBase), instance);

    if((clkfreq != 0u) || (bypass && (workMode == LPTMR_WORKMODE_PULSECOUNTER)))
    {
        /* The compare value can only be written if counter is disabled or the compare flag is set. */
        if (lptmrEnabled && !compareFlag)
        {
            status = STATUS_ERROR;
        }
        else
        {
            /* Check if new value is below the current counter value */
            LPTMR_HW_ConfigCompareValue(lptmrBase, compareValueByCount);
            counterVal = LPTMR_HW_ReadCounterValue(lptmrBase);
            if (counterVal >= compareValueByCount)
            {
                status = STATUS_TIMEOUT;
            }
        }
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Read the compare value of timer in ticks units.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param compareValueByCount: the compare value pointer
 *
 * @retval None
 */
void LPTMR_ReadCompareValueByCount(const uint32_t instance, uint16_t *const compareValueByCount)
{
    ASSERT_INSTANCE(instance);

    const LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    *compareValueByCount = LPTMR_HW_ReadCompareValue(lptmrBase);
}

/*!
 * @brief Configures the compare value for Timer Mode in microseconds.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param compareValueUs: the compare value
 *
 * @retval - STATUS_SUCCESS: completed successfully
 *         - STATUS_ERROR: cannot reconfigure compare value
 *         - STATUS_TIMEOUT: compare value greater then current counter value
 *
 * @note Can be used only in Timer Mode
 */
STATUS_T LPTMR_ConfigCompareValueByUs(const uint32_t instance, const uint32_t compareValueUs)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase  = g_lptmrBase[instance];
    bool lptmrEnabled = false;
    bool compareFlag = false;
    STATUS_T status = STATUS_SUCCESS;

    LPTMR_CLOCKSOURCE_T clkSrc = LPTMR_CLOCKSOURCE_LSICLKDIV2;
    uint32_t clkFreq = 0U;
    uint16_t cmpValTicks = 0U;
    uint16_t currentCounterVal = 0U;
    LPTMR_PRESCALER_T prescVal = LPTMR_PRESCALE_2;
    bool prescBypass = false;
    bool conversionStatus = false;

    /* This function can only be used if LPTMR is configured in Timer Mode. */
    if(LPTMR_HW_ReadWorkMode(lptmrBase) != LPTMR_WORKMODE_TIMER)
    {
        status = STATUS_ERROR;
    }
    else
    {
        lptmrEnabled = LPTMR_HW_ReadEnable(lptmrBase);
        compareFlag = LPTMR_HW_ReadCompareFlagState(lptmrBase);
        /* The compare value can only be written if counter is disabled or the compare flag is set. */
        if (lptmrEnabled && !compareFlag)
        {
            status = STATUS_ERROR;
        }
        else
        {
            clkSrc = LPTMR_HW_ReadClockSelect(lptmrBase);
            clkFreq = LPTMR_ReadClkFreq(clkSrc, instance);

            /* Get prescaler value and prescaler bypass state.*/
            prescVal = LPTMR_HW_ReadPrescaler(lptmrBase);
            prescBypass = LPTMR_HW_ReadBypass(lptmrBase);
            /* Convert new compare value from microseconds to ticks. */
            conversionStatus =
                LPTMR_UsToTicks(clkFreq, prescVal, prescBypass, compareValueUs, &cmpValTicks);
            (void) conversionStatus;

            /* Write value and check if written successfully */
            LPTMR_HW_ConfigCompareValue(lptmrBase, cmpValTicks);
            currentCounterVal = LPTMR_HW_ReadCounterValue(lptmrBase);

            if (currentCounterVal >= cmpValTicks)
            {
                status = STATUS_TIMEOUT;
            }
        }
    }

    return status;
}

/*!
 * @brief Read the compare value in microseconds representation.
 *
 * @param instance: instance of LPTMR lptmrBase
 * @param compareValueUs: the compare value pointer
 *
 * @retval None
 */
void LPTMR_ReadCompareValueByUs(const uint32_t instance, uint32_t *const compareValueUs)
{
    ASSERT_INSTANCE(instance);

    const LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    LPTMR_CLOCKSOURCE_T clkSrc = LPTMR_CLOCKSOURCE_LSICLKDIV2;
    uint32_t clkFreq = 0U;
    uint16_t cmpValTicks = 0U;
    LPTMR_PRESCALER_T prescVal = LPTMR_PRESCALE_2;
    bool prescBypass = false;

    /* This function can only be used if LPTMR is configured in Timer Mode. */
    if(LPTMR_HW_ReadWorkMode(lptmrBase) == LPTMR_WORKMODE_TIMER)
    {
        clkSrc = LPTMR_HW_ReadClockSelect(lptmrBase);
        clkFreq = LPTMR_ReadClkFreq(clkSrc, instance);

        /* Get prescaler value and prescaler bypass state.*/
        prescVal = LPTMR_HW_ReadPrescaler(lptmrBase);
        prescBypass = LPTMR_HW_ReadBypass(lptmrBase);
        cmpValTicks = LPTMR_HW_ReadCompareValue(lptmrBase);

        /* Convert current compare value from ticks to microseconds. */
        LPTMR_TicksToUs(clkFreq, prescVal, prescBypass, cmpValTicks, compareValueUs);
    }
}

/*!
 * @brief Read the current state of the Compare Flag.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval State of the Compare Flag.
 *         - true
 *         - false
 */
bool LPTMR_ReadCompareFlag(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    const LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    bool compareFlag = LPTMR_HW_ReadCompareFlagState(lptmrBase);

    return compareFlag;
}

/*!
 * @brief Clear the Compare Flag.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_ClearCompareFlag(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_HW_ClearCompareFlag(lptmrBase);
}

/*!
 * @brief Read the running state.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval - true: Timer/Counter started
 *         - false: Timer/Counter stopped
 */
bool LPTMR_ReadRunningState(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    const LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    bool runningState = LPTMR_HW_ReadEnable(lptmrBase);

    return runningState;
}

/*!
 * @brief Enables the LPTMR interrupt.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_EnableInterrupt(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_HW_ConfigInterrupt(lptmrBase, true);
}

/*!
 * @brief Disables the LPTMR interrupt.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_DisableInterrupt(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_HW_ConfigInterrupt(lptmrBase, false);
}

/*!
 * @brief Read the current Counter Value in timer ticks representation.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval The Current Counter Value.
 */
uint16_t LPTMR_ReadCounterValueByCount(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    uint16_t counterVal = LPTMR_HW_ReadCounterValue(lptmrBase);

    return counterVal;
}

/*!
 * @brief Start the counter.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_StartCounter(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];
    bool bypass = LPTMR_HW_ReadBypass(lptmrBase);
    LPTMR_WORKMODE_T workMode = LPTMR_HW_ReadWorkMode(lptmrBase);
    uint32_t clkFreq = LPTMR_ReadClkFreq(LPTMR_HW_ReadClockSelect(lptmrBase), instance);

    /* Check if a valid clock is selected for the timer/glitch filter */
    if(( clkFreq != 0u) || (bypass && (workMode == LPTMR_WORKMODE_PULSECOUNTER)))
    {
        LPTMR_HW_Enable(lptmrBase);
    }
}

/*!
 * @brief Stop the counter.
 *
 * @param instance: Instance of LPTMR lptmrBase
 *
 * @retval None
 */
void LPTMR_StopCounter(const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_HW_Disable(lptmrBase);
}

/*!
 * @brief Configures the Input Pin configuration for Pulse Counter mode.
 *
 * @param instance: Instance of LPTMR lptmrBase
 * @param pinSelect: LPTMR_PINSELECT_T Value
 * @param pinPolarity: LPTMR_PINPOLARITY_T Value
 *
 * @retval None
 */
void LPTMR_ConfigPinSetting(
    const uint32_t instance,
    const LPTMR_PINSELECT_T pinSelect,
    const LPTMR_PINPOLARITY_T pinPolarity)
{
    ASSERT_INSTANCE(instance);

    LPTMR_T *const lptmrBase = g_lptmrBase[instance];

    LPTMR_HW_ConfigPinSelect(lptmrBase, pinSelect);
    LPTMR_HW_ConfigPinPolarity(lptmrBase, pinPolarity);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configures all registers of the LPTMR instance to reset value.
 *
 * @param lptmrBase: LPTMR lptmrBase pointer.
 *
 * @retval None
 */
void LPTMR_HW_ConfigForReset(LPTMR_T *const lptmrBase)
{
    /* First, disable the module so we can write the registers */
    lptmrBase->CSTS.reg = 0U;
    lptmrBase->PSC.reg = 0U;
    lptmrBase->COM.reg = 0U;
}

/*!
 * @brief Read the DMA request enable
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval DMA Request enable
 *         - true: enable DMA Request
 *         - false: disable DMA Request
 */
bool LPTMR_HW_ReadDmaRequest(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->CSTS.bit.DMAREN == LPTMR_CSTS_DMAREN_1) ? true : false);
}

/*!
 * @brief Configures the DMA Request Enable
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param enable: The new state of the DMA Request Enable Flag
 *        - true: enable DMA Request
 *        - false: disable DMA Request
 *
 * @retval None
 */
void LPTMR_HW_ConfigDmaRequest(LPTMR_T *const lptmrBase, bool enable)
{
    lptmrBase->CSTS.bit.DMAREN = (enable ? LPTMR_CSTS_DMAREN_1 : LPTMR_CSTS_DMAREN_0);
}

/*!
 * @brief Read the Compare flag state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The Compare Flag state
 *         - true: Compare Match/Interrupt Pending asserted
 *         - false: Compare Match/Interrupt Pending not asserted
 */
bool LPTMR_HW_ReadCompareFlagState(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->CSTS.bit.COMFLG == LPTMR_CSTS_COMFLG_1) ? true : false);
}

/*!
 * @brief Clear the Compare flag
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval None
 */
void LPTMR_HW_ClearCompareFlag(LPTMR_T *const lptmrBase)
{
    lptmrBase->CSTS.bit.COMFLG = LPTMR_CSTS_COMFLG_1;

    /* Read-after-write sequence to guarantee required serialization of memory operations */
    (void)lptmrBase->CSTS.reg;
}

/*!
 * @brief Read the interrupt enable state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval Interrupt Enable state
 *         - true: Interrupt enabled
 *         - false: Interrupt disabled
 */
bool LPTMR_HW_ReadInterruptEnable(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->CSTS.bit.IEN == LPTMR_CSTS_IEN_1) ? true : false);
}

/*!
 * @brief Configures the Interrupt Enable state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param enable: The new state for the interrupt
 *                - true: enable Interrupt
 *                - false: disable Interrupt
 *
 * @retval None
 */
void LPTMR_HW_ConfigInterrupt(LPTMR_T *const lptmrBase, bool enable)
{
    lptmrBase->CSTS.bit.IEN = (enable ? LPTMR_CSTS_IEN_1 : LPTMR_CSTS_IEN_0);
}

/*!
 * @brief Read the Pin select for Counter Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval Input pin selection
 *         - LPTMR_PINSELECT_TRGMUX: count pulses from TRGMUX output
 *         - LPTMR_PINSELECT_ALT1: count pulses from pin alt 1
 *         - LPTMR_PINSELECT_ALT2: count pulses from pin alt 2
 *         - LPTMR_PINSELECT_ALT3: count pulses from pin alt 3
 */
LPTMR_PINSELECT_T LPTMR_HW_ReadPinSelect(const LPTMR_T *const lptmrBase)
{
    return (LPTMR_PINSELECT_T)(lptmrBase->CSTS.bit.TPISEL);
}

/*!
 * @brief Configures the Pin selection for Pulse Counter Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param pinsel: Pin selection
 *                - LPTMR_PINSELECT_TRGMUX: count pulses from TRGMUX output
 *                - LPTMR_PINSELECT_ALT1: count pulses from pin alt 1
 *                - LPTMR_PINSELECT_ALT2: count pulses from pin alt 2
 *                - LPTMR_PINSELECT_ALT3: count pulses from pin alt 3
 *
 * @retval None
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigPinSelect(LPTMR_T *const lptmrBase, const LPTMR_PINSELECT_T pinsel)
{
    lptmrBase->CSTS.bit.TPISEL = (uint32_t)pinsel;
}

/*!
 * @brief Read Pin polarity for Pulse Counter Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The pin polarity for Pulse Counter Mode
 *         - LPTMR_PINPOLARITY_RISING: count pulse on Rising Edge
 *         - LPTMR_PINPOLARITY_FALLING: count pulse on Falling Edge
 */
LPTMR_PINPOLARITY_T LPTMR_HW_ReadPinPolarity(const LPTMR_T *const lptmrBase)
{
    return (LPTMR_PINPOLARITY_T)((lptmrBase->CSTS.bit.TPIMSEL == LPTMR_CSTS_TPIMSEL_0)
        ? LPTMR_PINPOLARITY_RISING : LPTMR_PINPOLARITY_FALLING);
}

/*!
 * @brief Configures Pin polarity for Pulse Counter Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param pol: The pin polarity to count in Pulse Counter Mode
 *             - LPTMR_PINPOLARITY_RISING: count pulse on Rising Edge
 *             - LPTMR_PINPOLARITY_FALLING: count pulse on Falling Edge
 *
 * @retval None
 */
void LPTMR_HW_ConfigPinPolarity(LPTMR_T *const lptmrBase, const LPTMR_PINPOLARITY_T pol)
{
    lptmrBase->CSTS.bit.TPIMSEL = (uint32_t)pol;
}

/*!
 * @brief Read the Free Running state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval Free running mode state
 *         - true: Free Running Mode enabled. Reset counter on 16-bit overflow
 *         - false: Free Running Mode disabled. Reset counter on Compare Match.
 */
bool LPTMR_HW_ReadFreeRunning(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->CSTS.bit.FRCNT == LPTMR_CSTS_FRCNT_1) ? true : false);
}

/*!
 * @brief Configures the Free Running state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param enable: The new Free Running state
 *                - true: Free Running Mode enabled. Reset counter on 16-bit overflow
 *                - false: Free Running Mode disabled. Reset counter on Compare Match.
 *
 * @retval None
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigFreeRunning(LPTMR_T *const lptmrBase, const bool enable)
{
    lptmrBase->CSTS.bit.FRCNT = (enable ? (uint32_t)1u : (uint32_t)0u);
}

/*!
 * @brief Read current Work Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval Work Mode
 *         - LPTMR_WORKMODE_TIMER: LPTMR is in Timer Mode
 *         - LPTMR_WORKMODE_PULSECOUNTER: LPTMR is in Pulse Counter Mode
 */
LPTMR_WORKMODE_T LPTMR_HW_ReadWorkMode(const LPTMR_T *const lptmrBase)
{
    return (LPTMR_WORKMODE_T)((lptmrBase->CSTS.bit.TMRMCFG == LPTMR_CSTS_TMRMCFG_0)
        ? LPTMR_WORKMODE_TIMER : LPTMR_WORKMODE_PULSECOUNTER);
}

/*!
 * @brief Configures the Work Mode
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param mode: New Work Mode
 *              - LPTMR_WORKMODE_TIMER: LPTMR set to Timer Mode
 *              - LPTMR_WORKMODE_PULSECOUNTER: LPTMR set to Pulse Counter Mode
 *
 * @retval None
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigWorkMode(LPTMR_T *const lptmrBase, const LPTMR_WORKMODE_T mode)
{
    lptmrBase->CSTS.bit.TMRMCFG = (uint32_t)mode;
}

/*!
 * @brief Read the Enable state.
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The state of the LPTMR
 *         - true: LPTMR enabled
 *         - false: LPTMR disabled
 */
bool LPTMR_HW_ReadEnable(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->CSTS.bit.LPTMREN == LPTMR_CSTS_LPTMREN_1) ? true : false);
}

/*!
 * @brief Enable the LPTMR
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval None
 */
void LPTMR_HW_Enable(LPTMR_T *const lptmrBase)
{
    lptmrBase->CSTS.bit.LPTMREN = LPTMR_CSTS_LPTMREN_1;
}

/*!
 * @brief Disable the LPTMR
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval None
 */
void LPTMR_HW_Disable(LPTMR_T *const lptmrBase)
{
    lptmrBase->CSTS.bit.LPTMREN = LPTMR_CSTS_LPTMREN_0;
}

/*!
 * @brief Read Prescaler/Glitch Filter divider value
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The Prescaler/Glitch filter value
 *         - LPTMR_PRESCALE_2: Timer mode: prescaler 2, Glitch filter mode: invalid
 *         - LPTMR_PRESCALE_4_GLITCHFILTER_2: Timer mode: prescaler 4, Glitch filter mode: 2 clocks
 *         - LPTMR_PRESCALE_8_GLITCHFILTER_4: Timer mode: prescaler 8, Glitch filter mode: 4 clocks
 *         - LPTMR_PRESCALE_16_GLITCHFILTER_8: Timer mode: prescaler 16, Glitch filter mode: 8 clocks
 *         - LPTMR_PRESCALE_32_GLITCHFILTER_16: Timer mode: prescaler 32, Glitch filter mode: 16 clocks
 *         - LPTMR_PRESCALE_64_GLITCHFILTER_32: Timer mode: prescaler 64, Glitch filter mode: 32 clocks
 *         - LPTMR_PRESCALE_128_GLITCHFILTER_64: Timer mode: prescaler 128, Glitch filter mode: 64 clocks
 *         - LPTMR_PRESCALE_256_GLITCHFILTER_128: Timer mode: prescaler 256, Glitch filter mode: 128 clocks
 *         - LPTMR_PRESCALE_512_GLITCHFILTER_256: Timer mode: prescaler 512, Glitch filter mode: 256 clocks
 *         - LPTMR_PRESCALE_1024_GLITCHFILTER_512: Timer mode: prescaler 1024, Glitch filter mode: 512 clocks
 *         - LPTMR_PRESCALE_2048_GLITCHFILTER_1024: Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks
 *         - LPTMR_PRESCALE_4096_GLITCHFILTER_2048: Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks
 *         - LPTMR_PRESCALE_8192_GLITCHFILTER_4096: Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks
 *         - LPTMR_PRESCALE_16384_GLITCHFILTER_8192: Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks
 *         - LPTMR_PRESCALE_32768_GLITCHFILTER_16384: Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks
 *         - LPTMR_PRESCALE_65536_GLITCHFILTER_32768: Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks
 */
LPTMR_PRESCALER_T LPTMR_HW_ReadPrescaler(const LPTMR_T *const lptmrBase)
{
    return (LPTMR_PRESCALER_T)(lptmrBase->PSC.bit.PVCFG);
}

/*!
 * @brief Configures the Prescaler/Glitch Filter divider value
 *
 * @param lptmrBase : LPTMR lptmrBase pointer
 * @param prescale: The new Prescaler value
 *         : LPTMR_PRESCALE_2: Timer mode: prescaler 2, Glitch filter mode: invalid
 *         : LPTMR_PRESCALE_4_GLITCHFILTER_2: Timer mode: prescaler 4, Glitch filter mode: 2 clocks
 *         : LPTMR_PRESCALE_8_GLITCHFILTER_4: Timer mode: prescaler 8, Glitch filter mode: 4 clocks
 *         : LPTMR_PRESCALE_16_GLITCHFILTER_8: Timer mode: prescaler 16, Glitch filter mode: 8 clocks
 *         : LPTMR_PRESCALE_32_GLITCHFILTER_16: Timer mode: prescaler 32, Glitch filter mode: 16 clocks
 *         : LPTMR_PRESCALE_64_GLITCHFILTER_32: Timer mode: prescaler 64, Glitch filter mode: 32 clocks
 *         : LPTMR_PRESCALE_128_GLITCHFILTER_64: Timer mode: prescaler 128, Glitch filter mode: 64 clocks
 *         : LPTMR_PRESCALE_256_GLITCHFILTER_128: Timer mode: prescaler 256, Glitch filter mode: 128 clocks
 *         : LPTMR_PRESCALE_512_GLITCHFILTER_256: Timer mode: prescaler 512, Glitch filter mode: 256 clocks
 *         : LPTMR_PRESCALE_1024_GLITCHFILTER_512: Timer mode: prescaler 1024, Glitch filter mode: 512 clocks
 *         : LPTMR_PRESCALE_2048_GLITCHFILTER_1024: Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks
 *         : LPTMR_PRESCALE_4096_GLITCHFILTER_2048: Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks
 *         : LPTMR_PRESCALE_8192_GLITCHFILTER_4096: Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks
 *         : LPTMR_PRESCALE_16384_GLITCHFILTER_8192: Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks
 *         : LPTMR_PRESCALE_32768_GLITCHFILTER_16384: Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks
 *         : LPTMR_PRESCALE_65536_GLITCHFILTER_32768: Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigPrescaler(LPTMR_T *const lptmrBase, const LPTMR_PRESCALER_T prescale)
{
    lptmrBase->PSC.bit.PVCFG = (uint32_t)prescale;
}

/*!
 * @brief Read the Prescaler/Glitch Filter Bypass enable state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The Prescaler Bypass state
 *         - true: Prescaler/Glitch Filter is enabled
 *         - false: Prescaler/Glitch Filter is bypassed
 */
bool LPTMR_HW_ReadBypass(const LPTMR_T *const lptmrBase)
{
    return ((lptmrBase->PSC.bit.PBYPEN == LPTMR_PSC_PBYPEN_1) ? true : false);
}

/*!
 * @brief Configures the Prescaler/Glitch Filter Bypass enable state
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param enable: The new Prescaler/Glitch Filter Bypass state
 *        - true: Prescaler/Glitch Filter Bypass enabled
 *        - false: Prescaler/Glitch Filter Bypass disabled
 *
 * @retval None
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigBypass(LPTMR_T *const lptmrBase, const bool enable)
{
    lptmrBase->PSC.bit.PBYPEN = (enable ? (uint32_t)1u : (uint32_t)0u);
}

/*!
 * @brief Read the LPTMR input Clock selection
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The Clock source
 *         - LPTMR_CLOCKSOURCE_LSICLKDIV2: clock from LSICLK DIV2
 *         - LPTMR_CLOCKSOURCE_1KHZ_LPO: clock from 1kHz LPO
 *         - LPTMR_CLOCKSOURCE_RTC: clock from RTC
 *         - LPTMR_CLOCKSOURCE_PCM: clock from PCM
 */
LPTMR_CLOCKSOURCE_T LPTMR_HW_ReadClockSelect(const LPTMR_T *const lptmrBase)
{
    return (LPTMR_CLOCKSOURCE_T)(lptmrBase->PSC.bit.PCLKSEL);
}

/*!
 * @brief Configures the LPTMR input clock source selection
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param clocksel: New Clock Source
 *        - LPTMR_CLOCKSOURCE_LSICLKDIV2: clock from LSICLK DIV2
 *        - LPTMR_CLOCKSOURCE_1KHZ_LPO: clock from 1kHz LPO
 *        - LPTMR_CLOCKSOURCE_RTC: clock from RTC
 *        - LPTMR_CLOCKSOURCE_PCM: clock from PCM
 *
 * @retval None
 *
 * @note This feature can be configured only when the LPTMR is disabled.
 */
void LPTMR_HW_ConfigClockSelect(LPTMR_T *const lptmrBase, const LPTMR_CLOCKSOURCE_T clocksel)
{
    lptmrBase->PSC.bit.PCLKSEL = (uint32_t)clocksel;
}

/*!
 * @brief Read the Compare Value
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The current compare value
 */
uint16_t LPTMR_HW_ReadCompareValue(const LPTMR_T *const lptmrBase)
{
    return (uint16_t)(lptmrBase->COM.bit.COMVAL);
}

/*!
 * @brief Configures the Compare Value
 *
 * @details If set to 0, the Compare Match event and the hardware trigger assert
 *          and remain asserted until the timer is disabled.
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 * @param compval: The new Compare Value
 *
 * @retval None
 */
void LPTMR_HW_ConfigCompareValue(LPTMR_T *const lptmrBase, const uint16_t compval)
{
    lptmrBase->COM.bit.COMVAL = (uint32_t)compval;
}

/*!
 * @brief Read the current Counter Value
 *
 * @param lptmrBase: LPTMR lptmrBase pointer
 *
 * @retval The Counter Value
 */
uint16_t LPTMR_HW_ReadCounterValue(LPTMR_T *const lptmrBase)
{
    /* Write dummy value before reading register */
    lptmrBase->CNT.bit.CNTVAL = 0U;
    return (uint16_t)lptmrBase->CNT.bit.CNTVAL;
}

/*******************************************************************************
 *                          PRIVATE FUNCTION FUNCTIONS
 ******************************************************************************/

/*!
* @brief Transform prescaler settings to prescaler coefficient value.
*
* @param psc: LPTMR prescale value
* @param bypass: true or false
*
* @retval The Prescaler Coefficient Value
*/
static uint8_t LPTMR_SettingToValue(const LPTMR_PRESCALER_T psc, const bool bypass)
{
    uint8_t p = 0U;

    if (!bypass)
    {
        p = (uint8_t)(((uint8_t)psc) + 1u);
    }

    return p;
}

/*!
* @brief Transform microseconds to undivided (unprescaled) timer units
*
* @details time = (us * clkfreq + 500000) / 1000000.
*
* @param clkfreq: The frequency in Hertz
* @param us: Microseconds value
*
* @retval The value of undivided (unprescaled) timer units.
*/
static uint64_t LPTMR_UsToUndividedTime(const uint32_t clkfreq, const uint32_t us)
{
    /* Approximate the timeout in undivided (unprescaled) timer ticks.
       - us is the timeout in microseconds (1/10^6 seconds)
       - clkfreq is the frequency in Hertz
        Operation:
        time = (us/1000000) * clkfreq
        In C:
        For better precision, first to the multiplication (us * clkfreq)
        To overcome the truncation of the div operator in C, add half of the
        denominator before the division. Hence:
        time = (us * clkfreq + 500000) / 1000000
    */
    /* There is no risk of overflow since us is 32-bit wide and clkfreq can be
       a theoretical maximum of ~100 MHz (platform maximum), which is over the
       maximum input of the LPTMR anyway
     */
    uint64_t time = (uint64_t)( (uint64_t)us * (uint64_t)clkfreq );
    time = (time + 500000u) / 1000000u;
    return time;
}

/*!
* @brief Compute total number of divided (prescaled) timer ticks
*
* @param time: The value of undivided (unprescaled) timer units
* @param psc: prescaler coefficient value
*
* @retval The total number of divided (prescaled) timer ticks.
*/
static uint64_t LPTMR_ComputeNticks(uint64_t time, uint8_t psc)
{
    uint64_t npresc = (uint64_t) 1u << psc;

    /* integer division */
    uint64_t nticks = ((time + (npresc >> 1u)) / npresc);

    return nticks;
}

/*!
* @brief Transform the value of divided (prescaled) timer ticks(nticks) to a 16-bit value.
*
* @param nticks: The value of divided (prescaled) timer ticks
* @param ticks: COMP register address
*
* @retval - true: no underflow or overflow detected
*         - false: value written was capped, underflow or overflow detected
*/
static bool LPTMR_NticksToCompareTicks(uint64_t nticks, uint16_t *ticks)
{
    bool success = true;

    /* if nticks fits, write the value to ticks */
    if (nticks > LPTMR_MAX_COMP_NTICKS)
    {
        /* timeout period (us) too high for prescaler settings */
        *ticks = LPTMR_MAX_COMP_NTICKS & 0x1U;
        success = false;
    }
    else
    {
        if (nticks != 0u)
        {
            /* According to RM, the LPTMR compare events take place when
             * "the CNT equals the value of the COMP and increments".
             * The additional increment is compensated here by decrementing the
             * calculated compare value with 1, before being written to COMP. */
            *ticks = (uint16_t)(nticks - 0x01u);
        }
        else
        {
            /* timeout period (us) too low for prescaler settings */
            *ticks = 0u;
            success = false;
        }
    }

    return success;
}

/*!
* @brief Read the clock frequency for the selected clock source.
*
* @param clkSrc: The value of LPTMR_CLOCKSOURCE_T
*
* @param instance: COMP register address
*
* @retval: the clock frequency or 0 if the clock is invalid.
*/
static uint32_t LPTMR_ReadClkFreq(const LPTMR_CLOCKSOURCE_T clkSrc, const uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    /* LPTMR PCM clock source names, for getting the input clock frequency */
    static const CLOCK_NAMES_T lptmrClockName[LPTMR_INSTANCE_COUNT] = {LPTMR_CLK};
    CLOCK_NAMES_T inputClockName = LSICLK_CLK;
    uint32_t clkFreq = 0U;
    STATUS_T status = STATUS_SUCCESS;

    /* Get input clock name */
    switch(clkSrc)
    {
    case LPTMR_CLOCKSOURCE_LSICLKDIV2:
        inputClockName = LSICLK_CLK;
        break;
    case LPTMR_CLOCKSOURCE_1KHZ_LPO:
        inputClockName = SIM_LPO_1K_CLK;
        break;
    case LPTMR_CLOCKSOURCE_RTC:
        inputClockName = SIM_RTCCLK_CLK;
        break;
    case LPTMR_CLOCKSOURCE_PCM:
        inputClockName = lptmrClockName[instance];
        break;
    default:
        /* Invalid clock source */
        break;
    }

    /* Read input clock frequency */
    if (inputClockName == LSICLK_CLK)
    {
        status = CLOCK_SYS_ReadFreq(LSIDIV2_CLK, &clkFreq);
        (void) status;
    }
    else
    {
        status = CLOCK_SYS_ReadFreq(inputClockName, &clkFreq);
        (void) status;
    }

    return clkFreq;
}

/*!
* @brief Transform timer ticks to microseconds using the given prescaler settings.
*
* @param clkfreq: The frequency in Hertz.
* @param psc: LPTMR prescaler value.
* @param bypass: enable or disable bypass.
* @param ticks: LPTMR tick value.
* @param us: Microseconds value.
*
* @retval - true: conversion success
*         - false: conversion failed, result did not fit in 32-bit.
*/
static bool LPTMR_TicksToUs(
    const uint32_t clkfreq,
    const LPTMR_PRESCALER_T psc,
    const bool bypass,
    const uint16_t ticks,
    uint32_t *const us)
{
    bool success = true;
    uint8_t p = LPTMR_SettingToValue(psc, bypass);
    uint64_t time = ( (uint64_t)ticks + 1u ) << p;
    uint64_t us_real = (time * 1000000u) / (clkfreq);
    uint32_t us_local = 0;

    if ( us_real > (0xFFFFFFFFu) )
    {
        us_local = 0xFFFFFFFFu;
        success = false;
    }
    else
    {
        us_local = (uint32_t)us_real;
    }

    *us = us_local;
    return success;
}

/*!
* @brief Transform microseconds to timer ticks using the given prescaler settings.
*
* @param clkfreq: The frequency in Hertz.
* @param psc: LPTMR prescale value.
* @param bypass: enable or disable bypass.
* @param us: Microseconds value.
* @param ticks: LPTMR tick value.
*
* @retval: true: conversion completed successfully
*        : false: conversion failed, value did not fit in 16-bit.
*/
static bool LPTMR_UsToTicks(
    const uint32_t clkfreq,
    const LPTMR_PRESCALER_T psc,
    const bool bypass,
    const uint32_t us,
    uint16_t *const ticks)
{
    bool success = true;
    /* Transform prescaler setting to prescaler coefficient value of p */
    uint8_t p = LPTMR_SettingToValue(psc, bypass);
    /* Compute time, the number of ticks necessary for the period in microseconds
       without any prescaler */
    uint64_t time = LPTMR_UsToUndividedTime(clkfreq, us);
    /* Compute nticks, total number of ticks with prescaler */
    uint64_t nticks = LPTMR_ComputeNticks(time, p);
    /* Transform nticks to value to be written to register */
    success = LPTMR_NticksToCompareTicks(nticks, ticks);
    return success;
}

/*!
 * @brief Choose clocking configuration for the desired timeout period
 *
 * @param clkfreq: The frequency in Hertz.
 * @param us: Microseconds value.
 * @param psc: LPTMR prescale value.
 * @param bypass: enable or disable bypass.
 * @param ticks: LPTMR tick value.
 *
 * @retval - true: configuration found
 *         - false: configuration mismatch, desired timeout period is
 *                  too small or too big for the clock settings.
 */
static bool LPTMR_ChooseClkConfig(
    const uint32_t clkfreq,
    const uint32_t us,
    LPTMR_PRESCALER_T *const psc,
    bool *const bypass,
    uint16_t *const ticks)

{
    uint8_t tempPsc = 0;
    uint64_t nticks = 0ULL;
    bool success = true;

    uint64_t time = LPTMR_UsToUndividedTime(clkfreq, us);

    /* Find the lowest prescaler value that allows the compare value in 16-bits */
    for (tempPsc = 0u; tempPsc <= LPTMR_MAX_PRESCALER; tempPsc++)
    {
        nticks = LPTMR_ComputeNticks(time, tempPsc);

        if (nticks <= LPTMR_MAX_COMP_NTICKS)
        {
            /* Search finished, value will fit in the 16-bit register */
            break;
        }
    }

    success = LPTMR_NticksToCompareTicks(nticks, ticks);

    /* Convert p to prescaler configuration */
    if (tempPsc != 0u)
    {
        *bypass = false;
        /* Decrement to match LPTMR_PRESCALER_T.  */
        tempPsc--;
        *psc = (LPTMR_PRESCALER_T) tempPsc;
    }
    else
    {
        /* Prescaler value of 1 */
        *bypass = true;
        *psc = LPTMR_PRESCALE_2;
    }

    return success;
}

/**@} end of group LPTMR_Functions*/
/**@} end of group LPTMR_Driver*/
/**@} end of group  APM32F445_446_StdPeriphDriver*/
