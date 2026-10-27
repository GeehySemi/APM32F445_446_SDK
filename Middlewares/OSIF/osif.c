/*!
 * @file        osif.c
 *
 * @brief       This file provides all the OSIF functions
 *
 * @version     V1.0.0
 *
 * @date        2023-08-30
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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
#include "osif.h"
#include "device.h"
#include "apm32f445_446_clock.h"

/** @addtogroup RTOS
  @{
*/

/** @addtogroup OSIF
  @{
*/

/** @defgroup OSIF_Macros Macros
  @{
*/

/* Convert milliseconds to ticks, 1 tick = 1 millisecond */
#define MILLISECONDS_TO_TICK(msec)  (msec)

/**@} end of group OSIF_Macros*/

#if (FEATURE_OSIF_USE_SYSTICK != 0) || (FEATURE_OSIF_USE_PIT != 0)
static volatile uint32_t g_osifTickCount = 0U;
#endif

/*******************************************************************************
 *                              FUNCTIONS
 ******************************************************************************/

/** @defgroup OSIF_Functions Functions
  @{
*/

#if FEATURE_OSIF_USE_SYSTICK
/*!
 * @brief System clock update
 */
static void OSIF_UpdateSysTickCfg(void)
{
    uint32_t coreFreq = 0u;
    static bool isFirstInit = true;

    /* Read the frequency of the core clock */
    (void)CLOCK_SYS_ReadFreq(CORE_CLK, &coreFreq);

    /**
     * For Cortex-M0 devices the systick counter is initialized with an
     * undefined value, make sure to initialize it to 0 before starting.
     */
    APM32_SysTick->CSR.reg &= (uint32_t)(APM32_SysTick_CSR_ENABLE_0 & 0x01U);
    APM32_SysTick->RVR.reg = coreFreq / 1000u;

    if (isFirstInit)
    {
        /* Only initialize CVR on the first entry, to not cause time drift */
        isFirstInit = false;
        APM32_SysTick->CVR.reg = 0U;
    }

    APM32_SysTick->CSR.reg |= (uint32_t)((APM32_SysTick_CSR_CLKSOURCE_1 << 2U)
                                     | (APM32_SysTick_CSR_TICKINT_1 << 1U)
                                     | (APM32_SysTick_CSR_ENABLE_1 << 0U));
}
#endif /* FEATURE_OSIF_USE_SYSTICK */

#if (FEATURE_OSIF_USE_SYSTICK != 0) || (FEATURE_OSIF_USE_PIT != 0)
static uint32_t OSIF_GetCurrentTickCount(void)
{
    return g_osifTickCount;
}

void OSIF_Tick(void)
{
    g_osifTickCount++;
}
#endif /* (FEATURE_OSIF_USE_SYSTICK != 0) || (FEATURE_OSIF_USE_PIT != 0) */

/*!
 * @brief Block execution for a number of milliseconds
 *
 * @param delay Delay time in milliseconds
 *
 * @retval None
 */
void OSIF_TimeDelay(const uint32_t delay)
{
    OSIF_UpdateSysTickCfg();

    uint32_t delayTicks = MILLISECONDS_TO_TICK(delay);
    uint32_t startTick = OSIF_GetCurrentTickCount();
    uint32_t currentTick = OSIF_GetCurrentTickCount();
    uint32_t tickCount = currentTick - startTick;

    while (tickCount < delayTicks)
    {
        currentTick = OSIF_GetCurrentTickCount();
        tickCount = currentTick - startTick;
    }
}

/*!
 * @brief   Return the number of miliseconds elapsed since starting the internal timer
 * @details Note: Please make sure the timer is initialized before calling this
 *          function. To initialize the internal timer (systick) in bare-metal,
 *          call either OSIF_TimeDelay or OSIF_SemWait functions. Calling
 *          OSIF_TimeDelay(0) will initialize the timer without any
 *          side-effects (no delay).
 */
uint32_t OSIF_GetMilliseconds(void)
{
    /* Assumes that 1 tick = 1 millisecond */
    return OSIF_GetCurrentTickCount();
}

/*!
 * @brief Create a mutex (mock operation in bare-metal case)
 */
STATUS_T OSIF_MutexCreate(MUTEX_T * const mutexPtr)
{
    if (POINTER_IS_NULL(mutexPtr))
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Destroys a mutex (mock operation in bare-metal case)
 */
STATUS_T OSIF_MutexDestroy(const MUTEX_T * const mutexPtr)
{
    if (POINTER_IS_NULL(mutexPtr))
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Lock a mutex (mock operation in bare-metal case)
 */
STATUS_T OSIF_MutexLock(const MUTEX_T * const mutexPtr, const uint32_t timeout)
{
    (void)timeout;

    if (POINTER_IS_NULL(mutexPtr))
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Unlock a mutex (mock operation in bare-metal case)
 */
STATUS_T OSIF_MutexUnlock(const MUTEX_T * const mutexPtr)
{
    if (POINTER_IS_NULL(mutexPtr))
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Creates a semaphore
 */
STATUS_T OSIF_SemCreate(SEMAPHORE_T * const semPtr, const uint8_t initValue)
{
    if (POINTER_IS_NULL(semPtr))
    {
        return STATUS_ERROR;
    }

    INT_SYS_DisableIRQGlobal();
    *semPtr = initValue;
    INT_SYS_EnableIRQGlobal();
    return STATUS_SUCCESS;
}

/*!
 * @brief Destroy a semaphore (mock operation in bare-metal case)
 *
 * @param semPtr The pointer to SEMAPHORE_T.
 *
 * @retval STATUS_T.
 */
STATUS_T OSIF_SemDestroy(const SEMAPHORE_T * const semPtr)
{
    if (POINTER_IS_NULL(semPtr))
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief   Perform the wait (decrement) operation on a semaphore
 * @details When timeout value is 0, it's the equivalent of TryWait, try to
 *          decrement but return immediately if it fails (counter is 0).
 */
STATUS_T OSIF_SemWait(SEMAPHORE_T * const semPtr, const uint32_t timeout)
{
    STATUS_T result = STATUS_SUCCESS;
    uint32_t startTick;
    uint32_t endTick;
    uint32_t currentTick;
    uint32_t tickCount;
    uint32_t timeoutTicks;
    uint32_t maxWaitTicks;
    bool stopWaiting;

    if (POINTER_IS_NULL(semPtr))
    {
        return STATUS_ERROR;
    }

    OSIF_UpdateSysTickCfg();

    if (timeout != 0u)
    {
        if (timeout != OSIF_WAIT_FOREVER)
        {
            timeoutTicks = MILLISECONDS_TO_TICK(timeout);
        }
        else
        {
            timeoutTicks = OSIF_WAIT_FOREVER;
        }

        startTick = OSIF_GetCurrentTickCount();
        endTick = (uint32_t)(startTick + timeoutTicks);
        maxWaitTicks = endTick - startTick;
        stopWaiting = false;

        while ((stopWaiting == false) && (*semPtr == 0U))
        {
            currentTick = OSIF_GetCurrentTickCount();
            tickCount = currentTick - startTick;

            if (timeoutTicks != OSIF_WAIT_FOREVER)
            {
                /* Timeout occurred, stop waiting */
                if (tickCount > maxWaitTicks)
                {
                    result = STATUS_TIMEOUT;
                    stopWaiting = true;
                }
            }
        }
    }
    else
    {
        /**
         * When the timeout is 0 the wait operation is equivalent to TryWait,
         * which means it return immediately with an error code.
         */
        if (*semPtr == 0u)
        {
            result = STATUS_TIMEOUT;
        }
    }

    if (result == STATUS_SUCCESS)
    {
        INT_SYS_DisableIRQGlobal();
        --(*semPtr);
        INT_SYS_EnableIRQGlobal();
    }
    return result;
}

/*!
 * @brief Performs the post (increment) operation on a semaphore
 */
STATUS_T OSIF_SemPost(SEMAPHORE_T * const semPtr)
{
    STATUS_T result = STATUS_SUCCESS;

    if (POINTER_IS_NULL(semPtr))
    {
        return STATUS_ERROR;
    }

    INT_SYS_DisableIRQGlobal();

    if (*semPtr == 255U)
    {
        result = STATUS_ERROR;
    }
    else
    {
        ++(*semPtr);
    }

    INT_SYS_EnableIRQGlobal();
    return result;
}

/**@} end of group OSIF_Functions*/
/**@} end of group OSIF*/
/**@} end of group RTOS*/
