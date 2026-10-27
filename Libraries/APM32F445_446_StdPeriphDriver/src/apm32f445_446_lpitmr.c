/*!
 * @file        apm32f445_446_lpitmr.c
 *
 * @brief       This file provides all the LPITMR firmware functions
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
#include "apm32f445_446_lpitmr.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPITMR_Driver LPITMR Driver
  @{
*/

/** @defgroup LPITMR_Macros Macros
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

#define LPITMR_INSTANCE_VALIDITY(__instance__) if(__instance__ >= LPITMR_INSTANCE_COUNT){while(1);}

/**@} end of group LPITMR_Macros*/

/** @defgroup LPITMR_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/* Table of base addresses for LPITMR instances */
LPITMR_T * const g_lpitmrBase[] = LPITMR_BASE_PTRS;
/* Table to save LPITMR indexes in PCC register map for clock configuration */
const CLOCK_NAMES_T g_lpitmrClkNames[LPITMR_INSTANCE_COUNT] = LPITMR_CLOCK_NAMES;
/* LPITMR functional clock variable which will be updated in some driver functions */
uint32_t g_lpitmrSourceClockFrequency[LPITMR_INSTANCE_COUNT] = {0};

/**@} end of group LPITMR_Variables*/

/** @defgroup LPITMR_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Reads the default LPITMR configuration
 *
 * @param userConfig The configuration structure
 *
 * @retval None
 */
void LPITMR_DefaultConfig(LPITMR_USER_CONFIG_T *const userConfig)
{
    userConfig->runInDozeEn = false;
    userConfig->runInDebugEn = false;
}

/*!
 * @brief Resets the LPITMR module.
 *
 * @param ins LPITMR module instance number
 *
 * @retval None
 */
void LPITMR_DeInit(uint32_t ins)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase;

    lpitmrBase = g_lpitmrBase[ins];
    /* Disables LPITMR module functional clock*/
    LPITMR_HW_Disable(lpitmrBase);
}

/*!
 * @brief Configures the LPITMR module.
 *
 * @details This function resets LPITMR module, enables the LPITMR module,
 * configures LPITMR module operation in Debug and DOZE mode.
 * The LPITMR configuration structure shall be passed as arguments.
 * This configuration structure affects all timer channels.
 * This function should be called before calling any other LPITMR driver function.
 *
 * @param ins LPITMR module instance number.
 * @param userConfig Pointer to LPITMR configuration structure.
 *
 * @retval None
 */
void LPITMR_Init(uint32_t ins, const LPITMR_USER_CONFIG_T *userConfig)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T *lpitmrBase;
    STATUS_T status;

    /* Reads current functional clock frequency of LPITMR ins */
    status = CLOCK_SYS_ReadFreq(g_lpitmrClkNames[ins], &g_lpitmrSourceClockFrequency[ins]);
    /* Checks the functional clock of LPITMR module */
    (void)status;

    /* When resetting the LPITMR module, a delay of 4 peripheral clock cycles
     * must be ensured. This peripheral clock and the core clock running the
     * code could be very different, two distinct cases are identified:
     *  - core_clk > peripheral_clk. This requires a delay loop to be implemented,
     *     and the delay value based on the ratio between the two frequencies.
     *  - core_clk <= peripheral_clk. This requires a short delay, which is usually
     *     below the delay caused naturally by the read-modify-write operation.
     */
    uint32_t coreFreq = 0u;
    status = CLOCK_SYS_ReadFreq(CORE_CLK, &coreFreq);
    (void)status;
    uint32_t lpitrmFreq = g_lpitmrSourceClockFrequency[ins];
    uint32_t coreToPerClockRatio = (coreFreq + (lpitrmFreq >> 1u)) / lpitrmFreq;
    lpitmrBase = g_lpitmrBase[ins];
    /* Resets LPITMR module */
    LPITMR_HW_Reset(lpitmrBase, coreToPerClockRatio);
    /* Enables functional clock of LPITMR module*/
    LPITMR_HW_Enable(lpitmrBase, coreToPerClockRatio);
    /* Sets LPITMR operation in Debug and DOZE mode*/
    LPITMR_HW_ConfigTimerRunInDebugCmd(lpitmrBase, userConfig->runInDebugEn);
    LPITMR_HW_ConfigTimerRunInDozeCmd(lpitmrBase, userConfig->runInDozeEn);
}

/*!
 * @brief Gets the default timer channel configuration
 *
 * @param userConfig The channel configuration structure
 *
 * @retval None
 */
void LPITMR_DefaultChannelConfig(LPITMR_USER_CHANNEL_CONFIG_T *const userConfig)
{
    userConfig->triggerSrc = LPITMR_TRIG_SRC_EXTERNAL;
    userConfig->triggerSelect = 0U;
    userConfig->timerMode = LPITMR_PERIODIC_COUNTER;
    userConfig->reloadOnTriggerEn = false;
    userConfig->startOnTriggerEn = false;
    userConfig->channelChainEn = false;
    userConfig->stopOnInterruptEn = false;
    userConfig->interruptEn = true;
    userConfig->periodUnits = LPIT_PERIOD_UNITS_MICROSECONDS;
    userConfig->period = 1000000U;
}

/*!
 * @brief Initializes the LPITMR channelNumber.
 *
 * @details This function initializes the LPITMR timers by using a channelNumber, this function
 * configures timer channelNumber chaining, timer channelNumber mode, timer channelNumber period,
 * interrupt generation, trigger source, trigger select, reload on trigger,
 * stop on interrupt and start on trigger.
 * The timer channelNumber number and its configuration structure shall be passed as arguments.
 * Timer channels do not start counting by default after calling this function.
 * The function LPITMR_StartTimerChannels must be called to start the timer channelNumber counting.
 * In order to re-configures the period, call the LPITMR_ConfigTimerPeriodByUs or
 * LPITMR_ConfigTimerPeriodByCount.
 *
 * @param ins LPITMR module instance number
 * @param channelNumber Timer channel number
 * @param userChannelConfig Pointer to LPITMR channel configuration structure
 *
 * @retval Operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - STATUS_ERROR: The channel 0 is chained.
 *         - STATUS_ERROR: The input period is invalid.
 */
STATUS_T LPITMR_ChannelInit(uint32_t ins,
                            uint32_t channelNumber,
                            const LPITMR_USER_CHANNEL_CONFIG_T *userChannelConfig)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    STATUS_T retVal = STATUS_SUCCESS;
    LPITMR_T * lpitmrBase;
    const IRQn_Type lpitmrIrqnId[] = LPITMR_IRQS;

    lpitmrBase = g_lpitmrBase[ins];

    if ((channelNumber == 0U) && (userChannelConfig->channelChainEn))
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        /* Setups the timer channel chaining  */
        LPITMR_HW_ConfigTimerChannelChainCmd(lpitmrBase,
                                             channelNumber,
                                             userChannelConfig->channelChainEn);
        /*  Setups the timer channel operation mode */
        LPITMR_HW_ConfigTimerChannelModeCmd(lpitmrBase, channelNumber, userChannelConfig->timerMode);
        if (userChannelConfig->periodUnits != LPIT_PERIOD_UNITS_MICROSECONDS)
        {
            /* Setups timer channel period in count unit */
            LPITMR_ConfigTimerPeriodByCount(ins, channelNumber, userChannelConfig->period);
        }
        else
        {
            /* Setups timer channel period in microsecond unit */
            retVal = LPITMR_ConfigTimerPeriodByUs(ins, channelNumber, userChannelConfig->period);
        }

        if (retVal == STATUS_SUCCESS)
        {
            /* Setups the timer channel trigger source, trigger select, reload on trigger,
            stop on timeout, start on trigger and channel chaining */
            LPITMR_HW_ConfigTriggerSelectCmd(lpitmrBase,
                                             channelNumber,
                                             userChannelConfig->triggerSelect);
            LPITMR_HW_ConfigTriggerSourceCmd(lpitmrBase,
                                             channelNumber,
                                             userChannelConfig->triggerSrc);
            LPITMR_HW_ConfigReloadOnTriggerCmd(lpitmrBase,
                                               channelNumber,
                                               userChannelConfig->reloadOnTriggerEn);
            LPITMR_HW_ConfigStartOnTriggerCmd(lpitmrBase,
                                              channelNumber,
                                              userChannelConfig->startOnTriggerEn);
            LPITMR_HW_ConfigStopOnInterruptCmd(lpitmrBase,
                                               channelNumber,
                                               userChannelConfig->stopOnInterruptEn);
            /* Setups interrupt generation for timer channel */
            if (userChannelConfig->interruptEn == false)
            {
                /* Disables interrupt generation */
                LPITMR_HW_DisableInterruptTimerChannels(lpitmrBase, (uint32_t)1U << channelNumber);
                /* Only disable channel interrupt globally if each channel has a separate interrupt line */
#if (FEATURE_LPITMR_IRQS_NUM_CHANS == LPITMR_COUNT)
                INT_SYS_DisableIRQ(lpitmrIrqnId[channelNumber]);
#endif
            }
            else
            {
                /* Enables interrupt generation */
                LPITMR_HW_EnableInterruptTimerChannels(lpitmrBase, (uint32_t)1U << channelNumber);
                INT_SYS_EnableIRQ(lpitmrIrqnId[channelNumber]);
            }
        }
    }
    return retVal;
}

/*!
 * @brief Starts the timer channel counting.
 *
 * @details This function allows starting timer channels simultaneously.
 * After calling this function, timer channels are going operate depend on mode
 * and control bits which controls timer channel start, reload and restart.
 *
 * @param ins LPITMR module instance number
 * @param mask Timer channels starting mask that decides which channels
 *             will be started
 *             - For example:
 *               - with mask = 0x01U then channel 0 will be started
 *               - with mask = 0x02U then channel 1 will be started
 *               - with mask = 0x03U then channel 0 and channel 1 will be started
 *
 * @retval None
 */
void LPITMR_StartTimerChannels(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * baseAddr;

    baseAddr = g_lpitmrBase[ins];
    /* Starts timer channel counting */
    LPITMR_HW_StartTimerChannels(baseAddr, mask);
}

/*!
 * @brief Stops the timer channel counting.
 *
 * @details This function allows stop timer channels simultaneously from counting.
 * Timer channels reload their periods respectively after the next time
 * they call the LPITMR_StartTimerChannels.
 *
 * @param ins LPITMR module instance number
 * @param mask Timer channels stopping mask that decides which channels
 *             will be stopped
 *             - For example:
 *               - with mask = 0x01U then channel 0 will be stopped
 *               - with mask = 0x02U then channel 1 will be stopped
 *               - with mask = 0x03U then channel 0 and channel 1 will be stopped
 *
 * @retval None
 *
 * @note In 32-bit Trigger Accumulator mode, the counter will load on
 * the first trigger rising edge.
 */
void LPITMR_StopTimerChannels(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * baseAddr;

    baseAddr = g_lpitmrBase[ins];
    /* Stops timer channel from counting */
    LPITMR_HW_StopTimerChannels(baseAddr, mask);
}

/*!
 * @brief Sets the timer channel period in microseconds.
 *
 * @details This function sets the timer channel period in microseconds
 * when timer channel mode is 32 bit periodic or dual 16 bit counter mode.
 * The period range depends on the frequency of the LPITMR functional clock and
 * operation mode of timer channel.
 * If the required period is out of range, use the suitable mode if applicable.
 * This function is only valid for one single channel.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 * @param periodUs Timer channel period in microseconds
 *
 * @retval Operation status
 *         - STATUS_SUCCESS: Input period of timer channel is valid.
 *         - STATUS_ERROR: Input period of timer channel is invalid.
 */
STATUS_T LPITMR_ConfigTimerPeriodByUs(uint32_t ins, uint32_t channel, uint32_t periodUs)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase;
    LPITMR_TIMER_MODES_T timerMode;
    STATUS_T clockStatus;
    STATUS_T retVal = STATUS_SUCCESS;
    uint64_t count;

    /* Reads current functional clock frequency of LPITMR ins */
    clockStatus = CLOCK_SYS_ReadFreq(g_lpitmrClkNames[ins], &g_lpitmrSourceClockFrequency[ins]);
    /* Checks the functional clock of LPITMR module */
    (void)clockStatus;

    lpitmrBase = g_lpitmrBase[ins];
    /* Calculates the count value, assign it to timer channel counter register.*/
    count = ((uint64_t)periodUs) * g_lpitmrSourceClockFrequency[ins];
    count = (count / 1000000U) - 1U;
    /* Reads current timer channel operation mode */
    timerMode = LPITMR_HW_ReadTimerChannelModeCmd(lpitmrBase, channel);
    /* Checks whether the count is valid with timer channel operation mode */
    if (count > MAX_PERIOD_COUNT)
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        if (timerMode == LPITMR_DUAL_PERIODIC_COUNTER)
        {
            if (count <= MAX_PERIOD_COUNT_IN_DUAL_16BIT_MODE)
            {
                if (count > MAX_PERIOD_COUNT_16_BIT)
                {
                    /* Calculates the count value for dual 16 bit periodic counter mode */
                    count = ((count - (MAX_PERIOD_COUNT_16_BIT + 1U)) << 16U)
                            | (MAX_PERIOD_COUNT_16_BIT);
                }
            }
            else
            {
                retVal = STATUS_ERROR;
            }
        }
    }
    if (retVal == STATUS_SUCCESS)
    {
        /* Sets the timer channel period in count unit */
        LPITMR_HW_ConfigTimerPeriodByCount(lpitmrBase, channel, (uint32_t)count);
    }
    return retVal;
}

/*!
 * @brief Sets the timer channel period in microseconds.
 *
 * @details This function sets the timer channel period in microseconds
 * when timer channel mode is dual 16 bit periodic counter mode.
 * The period range depends on the frequency of the LPITMR functional clock and
 * operation mode of timer channel.
 * If the required period is out of range, use the suitable mode if applicable.
 * This function is only valid for one single channel.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 * @param periodHigh Period of higher 16 bit in microseconds
 * @param periodLow Period of lower 16 bit in microseconds
 *
 * @retval Operation status
 *         - STATUS_SUCCESS: Input period of timer channel is valid.
 *         - STATUS_ERROR: Input period of timer channel is invalid.
 */
STATUS_T LPITMR_ConfigTimerPeriodInDual16ModeByUs(uint32_t ins,
                                                  uint32_t channel,
                                                  uint16_t periodHigh,
                                                  uint16_t periodLow)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    uint64_t periodHighCount;
    uint64_t periodLowCount;
    uint64_t periodCount;
    STATUS_T retVal = STATUS_SUCCESS;
    LPITMR_T * lpitmrBase;
    STATUS_T clockStatus;

    /* Reads current functional clock frequency of LPITMR ins */
    clockStatus = CLOCK_SYS_ReadFreq(g_lpitmrClkNames[ins], &g_lpitmrSourceClockFrequency[ins]);
    (void)clockStatus;
    if ((clockStatus == STATUS_SUCCESS) && (g_lpitmrSourceClockFrequency[ins] > 0U))
    {
        lpitmrBase = g_lpitmrBase[ins];

        /* Calculates the count value of 16 bit higher period.*/
        periodHighCount = ((uint64_t)periodHigh) * g_lpitmrSourceClockFrequency[ins];
        periodHighCount = (periodHighCount / 1000000U) - 1U;

        /* Calculates the count value of 16 bit lower period.*/
        periodLowCount = ((uint64_t)periodLow) * g_lpitmrSourceClockFrequency[ins];
        periodLowCount = (periodLowCount / 1000000U) - 1U;

        /* Checks whether the count is valid */
        if ((periodHighCount <= MAX_PERIOD_COUNT_16_BIT) && (periodLowCount <= MAX_PERIOD_COUNT_16_BIT))
        {
            periodCount = (periodHighCount << 16U) | periodLowCount;
            LPITMR_HW_ConfigTimerPeriodByCount(lpitmrBase, channel, (uint32_t)periodCount);
        }
        else
        {
            retVal = STATUS_ERROR;
        }
    }
    else
    {
        retVal = STATUS_ERROR;
    }
    return retVal;
}

/*!
 * @brief Reads the timer channel period in microseconds.
 *
 * @details This function gets the timer channel period in microseconds.
 * The returned period here makes sense if the operation mode of timer channel
 * is 32 bit periodic counter or dual 16 bit periodic counter.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 *
 * @retval Timer channel period in microseconds
 */
uint64_t LPITMR_ReadTimerPeriodByUs(uint32_t ins, uint32_t channel)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    const LPITMR_T * lpitmrBase;
    uint64_t currentPeriod;
    STATUS_T clockStatus;
    LPITMR_TIMER_MODES_T timerMode;

    /* Reads current functional clock frequency of LPITMR ins */
    clockStatus = CLOCK_SYS_ReadFreq(g_lpitmrClkNames[ins], &g_lpitmrSourceClockFrequency[ins]);
    /* Checks the functional clock of LPITMR module */
    (void)clockStatus;
    if ((clockStatus == STATUS_SUCCESS) && (g_lpitmrSourceClockFrequency[ins] > 0U))
    {
        lpitmrBase = g_lpitmrBase[ins];
        /* Reads current timer channel period in count.*/
        currentPeriod = LPITMR_HW_ReadTimerPeriodByCount(lpitmrBase, channel);
        /* Reads current timer channel operation mode */
        timerMode = LPITMR_HW_ReadTimerChannelModeCmd(lpitmrBase, channel);

        if (timerMode != LPITMR_DUAL_PERIODIC_COUNTER)
        {
            /* Converts period from count unit to microseconds unit for other modes */
            currentPeriod = ((currentPeriod + 1U) * 1000000U) / g_lpitmrSourceClockFrequency[ins];
        }
        else
        {
            if (currentPeriod <= MAX_PERIOD_COUNT_16_BIT)
            {
                /* Converts period from count unit to microseconds unit for other modes */
                currentPeriod = ((currentPeriod + 1U) * 1000000U) / g_lpitmrSourceClockFrequency[ins];
            }
            else
            {
                currentPeriod = (((currentPeriod >> 16U) + (currentPeriod & MAX_PERIOD_COUNT_16_BIT) + 2U) * 1000000U)
                                    / g_lpitmrSourceClockFrequency[ins];
            }
        }
    }
    return currentPeriod;
}

/*!
 * @brief Gets the current timer channel counting value in microseconds.
 *
 * @details This function returns an absolute time stamp in microseconds.
 * One common use of this function is to measure the running time of a part of code.
 * Call this function at both the beginning and end of code.
 * The time difference between these two time stamps is the running time.
 * The return counting value here makes sense if the operation mode of timer channel
 * is 32 bit periodic counter or dual 16 bit periodic counter or 32-bit trigger input capture.
 * Need to make sure the running time will not exceed the timer channel period.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 *
 * @retval Current timer channel counting value in microseconds
 */
uint64_t LPITMR_ReadCurrentTimerUs(uint32_t ins, uint32_t channel)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    const LPITMR_T * lpitmrBase;
    uint64_t currentTime = 0U;
    STATUS_T clockStatus;
    LPITMR_TIMER_MODES_T timerMode;

    /* Reads current functional clock frequency of LPITMR ins */
    clockStatus = CLOCK_SYS_ReadFreq(g_lpitmrClkNames[ins], &g_lpitmrSourceClockFrequency[ins]);
    (void)clockStatus;
    if ((clockStatus == STATUS_SUCCESS) && (g_lpitmrSourceClockFrequency[ins] > 0U))
    {
        lpitmrBase = g_lpitmrBase[ins];
        /* Reads current timer channel counting value */
        currentTime = LPITMR_HW_ReadCurrentTimerCount(lpitmrBase, channel);
        /* Reads current timer channel operation mode */
        timerMode = LPITMR_HW_ReadTimerChannelModeCmd(lpitmrBase, channel);

        if (timerMode != LPITMR_DUAL_PERIODIC_COUNTER)
        {
            /* Converts counting value to microseconds unit for other modes */
            currentTime = (currentTime * 1000000U) / g_lpitmrSourceClockFrequency[ins];
        }
        else
        {
            currentTime = (((currentTime >> 16U) + (currentTime & MAX_PERIOD_COUNT_16_BIT)) * 1000000U)
                              / g_lpitmrSourceClockFrequency[ins];
        }
    }
    return currentTime;
}

/*!
 * @brief Sets the timer channel period in count unit.
 *
 * @details This function sets the timer channel period in count unit.
 * The counter period of a running timer channel can be modified by first setting
 * a new load value, the value will be loaded after the timer channel expires.
 * To abort the current cycle and start a timer channel period with the new value,
 * the timer channel must be disabled and enabled again.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 * @param count Timer channel period in count unit
 *
 * @retval None
 */
void LPITMR_ConfigTimerPeriodByCount(uint32_t ins, uint32_t channel, uint32_t count)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase;

    lpitmrBase = g_lpitmrBase[ins];
    LPITMR_HW_ConfigTimerPeriodByCount(lpitmrBase, channel, count);

}

/*!
 * @brief Sets the timer channel period in count unit.
 *
 * @details This function sets the timer channel period in count unit
 * when timer channel mode is dual 16 periodic counter mode.
 * The counter period of a running timer channel can be modified by first setting
 * a new load value, the value will be loaded after the timer channel expires.
 * To abort the current cycle and start a timer channel period with the new value,
 * the timer channel must be disabled and enabled again.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 * @param periodHigh Period of higher 16 bit in count unit
 * @param periodLow Period of lower 16 bit in count unit
 *
 * @retval None
 */
void LPITMR_ConfigTimerPeriodInDual16ModeByCount(uint32_t ins,
                                                 uint32_t channel,
                                                 uint16_t periodHigh,
                                                 uint16_t periodLow)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase = g_lpitmrBase[ins];
    uint32_t period = ((uint32_t)periodHigh << 16U) | periodLow;

    LPITMR_HW_ConfigTimerPeriodByCount(lpitmrBase, channel, period);

}

/*!
 * @brief Gets the current timer channel period in count unit.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 *
 * @retval Timer channel period in count unit
 */
uint32_t LPITMR_ReadTimerPeriodByCount(uint32_t ins, uint32_t channel)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    const LPITMR_T * lpitmrBase = g_lpitmrBase[ins];
    LPITMR_TIMER_MODES_T timerMode;
    uint32_t currentPeriod;

    /* Read mode */
    timerMode = LPITMR_HW_ReadTimerChannelModeCmd(lpitmrBase, channel);
    /* Read period */
    currentPeriod = LPITMR_HW_ReadTimerPeriodByCount(lpitmrBase, channel);

    if (timerMode == LPITMR_DUAL_PERIODIC_COUNTER)
    {
        /* Calculates the period for dual 16 bit periodic counter mode */
        currentPeriod = (currentPeriod >> 16U);
        currentPeriod += (currentPeriod & MAX_PERIOD_COUNT_16_BIT);
    }
    return currentPeriod;
}

/*!
 * @brief Gets the current timer channel counting value in count.
 *
 * @details This function returns the real-time timer channel counting value,
 * the value in a range from 0 to timer channel period.
 * Need to make sure the running time does not exceed the timer channel period.
 *
 * @param ins LPITMR module instance number
 * @param channel Timer channel number
 *
 * @retval Current timer channel counting value in count
 */
uint32_t LPITMR_ReadCurrentTimerCount(uint32_t ins, uint32_t channel)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    const LPITMR_T * lpitmrBase  = g_lpitmrBase[ins];
    LPITMR_TIMER_MODES_T timerMode;
    uint32_t currentTime;

    /* Reads current timer channel operation mode */
    timerMode = LPITMR_HW_ReadTimerChannelModeCmd(lpitmrBase, channel);
    /* Reads current timer channel counting value */
    currentTime = LPITMR_HW_ReadCurrentTimerCount(lpitmrBase, channel);

    if (timerMode == LPITMR_DUAL_PERIODIC_COUNTER)
    {
        /* Calculates the current counting value for dual 16 bit periodic counter mode */
        currentTime = (currentTime >> 16U);
        currentTime += (currentTime & MAX_PERIOD_COUNT_16_BIT);
    }
    return currentTime;
}

/*!
 * @brief Enables the interrupt generation of timer channel.
 *
 * @details This function allows enabling interrupt generation of timer channel
 * when timeout occurs or input trigger occurs.
 *
 * @param ins LPITMR module instance number.
 * @param mask The mask that decides which channels will be enabled interrupt.
 *             - For example:
 *               - with mask = 0x01u then the interrupt of channel 0 will be enabled
 *               - with mask = 0x02u then the interrupt of channel 1 will be enabled
 *               - with mask = 0x03u then the interrupt of channel 0 and channel 1 will be enabled
 *
 * @retval None
 */
void LPITMR_EnableTimerChannelInterrupt(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase = g_lpitmrBase[ins];

    LPITMR_HW_EnableInterruptTimerChannels(lpitmrBase, mask);
}

/*!
 * @brief Disables the interrupt generation of timer channel.
 *
 * @details This function allows disabling interrupt generation of timer channel
 * when timeout occurs or input trigger occurs.
 *
 * @param ins LPITMR module instance number
 * @param mask The mask that decides which channels will be disable interrupt.
 *             - For example:
 *               - with mask = 0x01u then the interrupt of channel 0 will be disable
 *               - with mask = 0x02u then the interrupt of channel 1 will be disable
 *               - with mask = 0x03u then the interrupt of channel 0 and channel 1 will be disable
 *
 * @retval None
 */
void LPITMR_DisableTimerChannelInterrupt(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase = g_lpitmrBase[ins];

    LPITMR_HW_DisableInterruptTimerChannels(lpitmrBase, mask);
}

/*!
 * @brief Gets the current interrupt flag of timer channels.
 *
 * @details This function gets the current interrupt flag of timer channels.
 * In compare modes, the flag sets to 1 at the end of the timer period.
 * In capture modes, the flag sets to 1 when the trigger asserts.
 *
 * @param ins LPITMR module instance number.
 * @param mask The interrupt flag getting mask that decides which channels will
 *             be got interrupt flag.
 *             - For example:
 *               - with mask = 0x01u then the interrupt flag of channel 0 only will be got
 *               - with mask = 0x02u then the interrupt flag of channel 1 only will be got
 *               - with mask = 0x03u then the interrupt flags of channel 0 and channel 1 will be got
 *
 * @retval Current the interrupt flag of timer channels
 */
uint32_t LPITMR_ReadInterruptFlagTimerChannels(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    const LPITMR_T * lpitmrBase = g_lpitmrBase[ins];
    uint32_t retVal;

    retVal = LPITMR_HW_ReadInterruptFlagTimerChannels(lpitmrBase, mask);
    return retVal;
}

/*!
 * @brief Clears the interrupt flag of timer channels.
 *
 * @details This function clears the interrupt flag of timer channels after
 * their interrupt event occurred.
 *
 * @param ins LPITMR module instance number
 * @param mask The interrupt flag clearing mask that decides which channels will
 *             be cleared interrupt flag
 *             - For example:
 *               - with mask = 0x01u then the interrupt flag of channel 0 only will be cleared
 *               - with mask = 0x02u then the interrupt flag of channel 1 only will be cleared
 *               - with mask = 0x03u then the interrupt flags of channel 0 and channel 1 will be cleared
 *
 * @retval None
 */
void LPITMR_ClearInterruptFlagTimerChannels(uint32_t ins, uint32_t mask)
{
    LPITMR_INSTANCE_VALIDITY(ins);
    LPITMR_T * lpitmrBase = g_lpitmrBase[ins];

    LPITMR_HW_ClearInterruptFlagTimerChannels(lpitmrBase, mask);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enables the LPITMR module.
 *
 * @param base LPITMR peripheral base address
 * @param delay delay time
 *
 * @retval None
 *
 * @note This function should be called before setup any timer channel.
 */
void LPITMR_HW_Enable(LPITMR_T *const base, volatile uint32_t delay)
{
    base->MCTRL.reg |= 0x1U;
    /* Run this counter down to zero
     * If the delay is 0, the four clock delay between setting and clearing
     * the SW_RST bit is ensured by the read-modify-write operation.
     */
    while(delay != 0u)
    {
        /* Since we need a four cycle delay, we assume the decrement is one cycle
         * and insert three NOP instructions. The actual delay will be larger because
         * of the loop overhead and the compiler optimization.
         */
        delay--;
        NOP();
        NOP();
        NOP();
    }
}

/*!
 * @brief Disables the LPITMR module.
 *
 * @param base LPITMR peripheral base address
 *
 * @retval None
 */
void LPITMR_HW_Disable(LPITMR_T *const base)
{
    base->MCTRL.reg &= ~0x1U;
}

/*!
 * @brief Resets the LPITMR module.
 *
 * @param base LPITMR peripheral base address
 * @param delay delay
 *
 * @retval None
 */
void LPITMR_HW_Reset(LPITMR_T *const base, volatile uint32_t delay)
{
    base->MCTRL.reg |= 0x2U;
    /* Run this counter down to zero
     * If the delay is 0, the four clock delay between setting and clearing
     * the SW_RST bit is ensured by the read-modify-write operation.
     */
    while(delay != 0u)
    {
        /* Since we need a four cycle delay, we assume the decrement is one cycle
         * and insert three NOP instructions. The actual delay will be larger because
         * of the loop overhead and the compiler optimization.
         */
        delay--;
        NOP();
        NOP();
        NOP();
    }
    base->MCTRL.reg &= ~0x2U;
}

/*!
  @brief Starts the timer channel counting.
 *
 * @param base LPITMR peripheral base address
 * @param mask Timer channels starting mask that decides which channels
 *        will be started
 *        - For example:
 *          - with mask = 0x01U then channel 0 will be started
 *          - with mask = 0x02U then channel 1 will be started
 *          - with mask = 0x03U then channel 0 and channel 1 will be started
 *
 * @retval None
 */
void LPITMR_HW_StartTimerChannels(LPITMR_T *const base, uint32_t mask)
{
    base->STMREN.reg |= mask;
}

/*!
 * @brief Stops the timer channel from counting.
 *
 * @param base LPITMR peripheral base address
 * @param mask Timer channels stopping mask that decides which channels
 *        will be stopped
 *        - For example:
 *          - with mask = 0x01U then channel 0 will be stopped
 *          - with mask = 0x02U then channel 1 will be stopped
 *          - with mask = 0x03U then channel 0 and channel 1 will be stopped
 *
 * @retval None
 *
 * @note In 32-bit Trigger Accumulator mode,
 *       the counter will load on the first trigger rising edge
 */
void LPITMR_HW_StopTimerChannels(LPITMR_T *const base, uint32_t mask)
{
    base->CLRTMREN.reg |= mask;
}

/*!
 * @brief Configures the timer channel period in count unit.
 *
 * @details This function sets the timer channel period in count unit.
 * The period range depends on the frequency of the LPITMR functional clock and
 * operation mode of timer channel.
 * If the required period is out of range, use the suitable mode if applicable.
 * Timer channel begins counting from the value that is set by this function.
 * The counter period of a running timer channel can be modified by first setting
 * a new load value, the value will be loaded after the timer channel expires.
 * To abort the current cycle and start a timer channel period with the new value,
 * the timer channel must be disabled and enabled again.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param count Timer channel period in count unit
 *
 * @retval None
 */
void LPITMR_HW_ConfigTimerPeriodByCount(LPITMR_T *const base, uint32_t channel, uint32_t count)
{
    base->TMR[channel].TMRV.reg = count;
}

/*!
 * @brief Reads the timer channel period in count unit.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 *
 * @retval Timer channel period in count unit
 */
uint32_t LPITMR_HW_ReadTimerPeriodByCount(const LPITMR_T *base, uint32_t channel)
{
    return (base->TMR[channel].TMRV.reg);
}

/*!
 * @brief Reads the current timer channel counting value.
 *
 * @details This function returns the real-time timer channel counting value,
 * the value in a range from 0 to timer channel period.
 * Need to make sure the running time does not exceed the timer channel period.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 *
 * @retval Current timer channel counting value
 */
uint32_t LPITMR_HW_ReadCurrentTimerCount(const LPITMR_T *base, uint32_t channel)
{
    return base->TMR[channel].CTMRV.bit.CTMRV;
}

/*!
 * @brief Enables the interrupt generation for timer channels.
 *
 * @param base LPITMR peripheral base address
 * @param mask The interrupt enabling mask that decides which channels will
 *             be enabled interrupt.
 *             - For example:
 *               - with mask = 0x01u then will enable interrupt for channel 0 only
 *               - with mask = 0x02u then will enable interrupt for channel 1 only
 *               - with mask = 0x03u then will enable interrupt for channel 0 and channel 1
 *
 * @retval None
 */
void LPITMR_HW_EnableInterruptTimerChannels(LPITMR_T *const base, uint32_t mask)
{
    base->MIEN.reg |= mask;
}

/*!
 * @brief Disables the interrupt generation for timer channels.
 *
 * @param base LPITMR peripheral base address
 * @param mask The interrupt disabling mask that decides which channels will
 *             be disabled interrupt.
 *             - For example:
 *               - with mask = 0x01u then will disable interrupt for channel 0 only
 *               - with mask = 0x02u then will disable interrupt for channel 1 only
 *               - with mask = 0x03u then will disable interrupt for channel 0 and channel 1
 *
 * @retval None
 */
void LPITMR_HW_DisableInterruptTimerChannels(LPITMR_T *const base, uint32_t mask)
{
    base->MIEN.reg &= ~mask;
}

/*!
 * @brief Reads the interrupt flag of timer channels.
 *
 * @param base LPITMR peripheral base address
 * @param mask The interrupt flag getting mask that decides which channels will
 *             be got interrupt flag.
 *             - For example:
 *               - with mask = 0x01u then the interrupt flag of channel 0 only will be got
 *               - with mask = 0x02u then the interrupt flag of channel 1 only will be got
 *               - with mask = 0x03u then the interrupt flags of channel 0 and channel 1 will be got
 *
 * @retval The interrupt flag of timer channels.
 */
uint32_t LPITMR_HW_ReadInterruptFlagTimerChannels(const LPITMR_T *base, uint32_t mask)
{
    return (base->MSTS.reg) & mask;
}

/*!
 * @brief Clears the interrupt flag of timer channels.
 *
 * @param base LPITMR peripheral base address
 * @param mask The interrupt flag clearing mask that decides which channels will
 *             be cleared interrupt flag.
 *             - For example:
 *               - with mask = 0x01u then the interrupt flag of channel 0 only will be cleared
 *               - with mask = 0x02u then the interrupt flag of channel 1 only will be cleared
 *               - with mask = 0x03u then the interrupt flags of channel 0 and channel 1 will be cleared
 *
 * @retval None
 */
void LPITMR_HW_ClearInterruptFlagTimerChannels(LPITMR_T *const base, uint32_t mask)
{
    /* Write 1 to clear the interrupt flag. */
    base->MSTS.reg = mask;

    /* Read-after-write sequence to guarantee required serialization of memory operations */
    base->MSTS.reg;
}

/*!
 * @brief Configures operation mode of timer channel
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param mode Operation mode of timer channel
 *
 * @retval None
 */
void LPITMR_HW_ConfigTimerChannelModeCmd(LPITMR_T *const base,
                                         uint32_t channel,
                                         LPITMR_TIMER_MODES_T mode)
{
    base->TMR[channel].TMRCTRL.reg &= ~0xCU;
    base->TMR[channel].TMRCTRL.reg |=  (((uint32_t)(((uint32_t)(mode))<<2U))&0xCU);
}

/*!
 * @brief Reads current operation mode of timer channel.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 *
 * @retval Operation mode of timer channel
 */
LPITMR_TIMER_MODES_T LPITMR_HW_ReadTimerChannelModeCmd(const LPITMR_T *base, uint32_t channel)
{
    uint32_t tmp;
    LPITMR_TIMER_MODES_T mode;

    tmp = (((base->TMR[channel].TMRCTRL.reg) & 0xCU) >> 2U);

    if(tmp == 0x00U)
    {
        mode = LPITMR_PERIODIC_COUNTER;
    }
    else if(tmp == 0x01U)
    {
        mode = LPITMR_DUAL_PERIODIC_COUNTER;
    }
    else if(tmp == 0x02U)
    {
        mode = LPITMR_TRIGGER_ACCUMULATOR;
    }
    else if(tmp == 0x03U)
    {
        mode = LPITMR_INPUT_CAPTURE;
    }
    else
    {
        mode = LPITMR_PERIODIC_COUNTER;
    }

    return mode;
}

/*!
 * @brief Configures internal trigger source for timer channel
 *
 * @details This function selects one trigger from the set of internal triggers
 * that is generated by other timer channels.
 * The selected trigger is used for starting and/or reloading the timer channel.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param triggerChannelSelect Number of the channel which is selected to be trigger source
 *
 * @retval None
 */
void LPITMR_HW_ConfigTriggerSelectCmd(LPITMR_T *const base,
                                      uint32_t channel,
                                      uint32_t triggerChannelSelect)
{
    base->TMR[channel].TMRCTRL.reg &= ~0xF000000U;
    base->TMR[channel].TMRCTRL.reg |=
      (((uint32_t)(((uint32_t)(triggerChannelSelect))<<24U))&0xF000000U);
}

/*!
 * @brief Configures trigger source of timer channel.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param triggerSrc Trigger source of timer channel(internal or external source)
 *
 * @retval None
 */
void LPITMR_HW_ConfigTriggerSourceCmd(LPITMR_T *const base,
                                      uint32_t channel,
                                      LPITMR_TRIGGER_SOURCE_T triggerSrc)
{
    base->TMR[channel].TMRCTRL.reg &= ~0x800000U;
    base->TMR[channel].TMRCTRL.reg |= (((uint32_t)(((uint32_t)(triggerSrc))<<23U))&0x800000U);
}

/*!
 * @brief Configures timer channel reload on trigger.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param isReloadOnTrigger Timer channel reload on trigger
 *        - True : timer channel will reload on trigger
 *        - False : timer channel will not reload on trigger
 *
 * @retval None
 */
void LPITMR_HW_ConfigReloadOnTriggerCmd(LPITMR_T *const base, uint32_t channel, bool isReloadOnTrigger)
{
    base->TMR[channel].TMRCTRL.reg &= ~0x40000U;
    base->TMR[channel].TMRCTRL.reg |=
      (((uint32_t)(((uint32_t)(isReloadOnTrigger))<<18U))&0x40000U) ? 1UL : 0UL;
}

/*!
 * @brief Configures timer channel stop on interrupt.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param isStopOnInterrupt Timer channel stop on interrupt
 *        - True : Timer channel will stop after it times out
 *        - False : Timer channel will not stop after it times out
 *
 * @retval None
 */
void LPITMR_HW_ConfigStopOnInterruptCmd(LPITMR_T *const base, uint32_t channel, bool isStopOnInterrupt)
{
    base->TMR[channel].TMRCTRL.reg &= ~0x20000U;
    base->TMR[channel].TMRCTRL.reg |=
      (((uint32_t)(((uint32_t)(isStopOnInterrupt))<<17U))&0x20000U) ? 1UL : 0UL;
}

/*!
 * @brief Configures timer channel start on trigger.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param isStartOnTrigger Timer channel start on trigger
 *        - True : Timer channel starts to decrement when rising edge on selected trigger is detected
 *        - False : Timer channel starts to decrement immediately based on restart condition
 *                  (controlled by Timer Stop On Interrupt bit)
 *
 * @retval None
 */
void LPITMR_HW_ConfigStartOnTriggerCmd(LPITMR_T *const base, uint32_t channel, bool isStartOnTrigger)
{
    base->TMR[channel].TMRCTRL.reg &= ~0x10000U;
    base->TMR[channel].TMRCTRL.reg |=
      (((uint32_t)(((uint32_t)(isStartOnTrigger))<<16U))&0x10000U)? 1UL : 0UL;
}

/*!
 * @brief Sets timer channel chaining.
 *
 * @param base LPITMR peripheral base address
 * @param channel Timer channel number
 * @param isChannelChained Timer channel chaining
 *        - True : Timer channel is chained. Timer channel decrements on previous channel's timeout
 *        - False : Timer channel is not chained. Timer channel runs independently
 *
 * @retval None
 *
 * @note The timer channel 0 cannot be chained
 */
void LPITMR_HW_ConfigTimerChannelChainCmd(LPITMR_T *const base, uint32_t channel, bool isChannelChained)
{
    base->TMR[channel].TMRCTRL.reg &= ~0x2U;
    base->TMR[channel].TMRCTRL.reg |=
      (((uint32_t)(((uint32_t)(isChannelChained))<<1U))&0x2U)? 1UL : 0UL;
}

/*!
 * @brief Sets operation of LPITMR in debug mode.
 *
 * @details When the device enters debug mode, the timer channels may or may not be frozen,
 * based on the configuration of this function. This is intended to aid software development,
 * allowing the developer to halt the processor, investigate the current state of
 * the system (for example, the timer channel values), and continue the operation.
 *
 * @param base LPITMR peripheral base address
 * @param isRunInDebug LPITMR run in debug mode
 *        - True: LPITMR continue to run when the device enters debug mode
 *        - False: LPITMR stop when the device enters debug mode
 *
 * @retval None
 */
void LPITMR_HW_ConfigTimerRunInDebugCmd(LPITMR_T *const base, bool isRunInDebug)
{
    base->MCTRL.reg &= ~0x8U;
    base->MCTRL.reg |= (((uint32_t)(((uint32_t)(isRunInDebug))<<3U))&0x8U) ? 1UL: 0UL;
}

/*!
 * @brief Sets operation of LPITMR in DOZE mode.
 *
 * @details When the device enters debug mode, the timer channels may or may not be frozen,
 * based on the configuration of this function. The LPITMR must use an external or
 * internal clock source which remains operating during DOZE modes(low power mode).
 *
 * @param base LPITMR peripheral base address
 * @param isRunInDoze LPITMR run in DOZE mode
 *        - True: LPITMR continue to run when the device enters DOZE mode
 *        - False: LPITMR channels stop when the device enters DOZE mode
 *
 * @retval None
 */
void LPITMR_HW_ConfigTimerRunInDozeCmd(LPITMR_T *const base, bool isRunInDoze)
{
    base->MCTRL.reg &= ~0x4U;
    base->MCTRL.reg |= (((uint32_t)(((uint32_t)(isRunInDoze))<<2U))&0x4U) ? 1UL : 0UL;
}

/**@} end of group LPITMR_Functions*/
/**@} end of group LPITMR_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
