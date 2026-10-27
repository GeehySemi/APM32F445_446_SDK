/*!
 * @file        apm32f445_446_cfgio_common.c
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

/* Includes */

#include "apm32f445_446_cfgio_common.h"
#include "apm32f445_446_interrupt.h"
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_COMMON_Driver CFGIO_COMMON Driver
  @{
*/

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
CFGIO_DEVICE_STATE_T *g_cfgioDevState[CFGIO_INSTANCE_COUNT] = {NULL};

/* Clock sources for getting the input clock frequency */
const CLOCK_NAMES_T g_cfgioClockSrc[CFGIO_INSTANCE_COUNT] = {CFGIO_CLK};

/* IRQ numbers */
const IRQn_Type g_cfgioIrq[CFGIO_INSTANCE_COUNT] = CFGIO_IRQS;

/* DMA request sources */
const uint8_t g_cfgioDmaSrc[CFGIO_INSTANCE_COUNT][FEATURE_CFGIO_MAX_SHIFTER_COUNT] = {
    {
        (uint8_t)FEATURE_CFGIO_DMA_REQUEST_0,
        (uint8_t)FEATURE_CFGIO_DMA_REQUEST_1,
        (uint8_t)FEATURE_CFGIO_DMA_REQUEST_2,
        (uint8_t)FEATURE_CFGIO_DMA_REQUEST_3
    }
};

/* Base addresses for CFGIO instances */
CFGIO_T * const g_cfgioBase[CFGIO_INSTANCE_COUNT] = CFGIO_BASE_PTRS;

/**@} end of group CFGIO_COMMON_Variables*/

/** @defgroup CFGIO_COMMON_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

static void CFGIO_InitResources(
    uint32_t instance,
    const CFGIO_COMMON_STATE_T *driverState);

static STATUS_T CFGIO_AllocateResources(
    uint32_t instance,
    CFGIO_COMMON_STATE_T *driverState);

static void CFGIO_DeallocateResources(
    uint32_t instance,
    const CFGIO_COMMON_STATE_T *driverState);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the CFGIO device
 * @details This function resets the CFGIO device, enables interrupts in
 *          interrupt manager and enables the device.
 *
 * @param instance      CFGIO instance number
 * @param deviceState   Pointer to the CFGIO device context structure.
 *                      The driver uses this memory area for its internal logic.
 *                      The application must make no assumptions about the content
 *                      of this structure, and must not free this memory until the
 *                      device is de-initialized using CFGIO_DeinitDevice().
 *
 * @retval Error status
 */
STATUS_T CFGIO_InitDevice(uint32_t instance, CFGIO_DEVICE_STATE_T *deviceState)
{
    CFGIO_T *base;

    if (g_cfgioDevState[instance] != NULL)
    {
        return STATUS_SUCCESS;
    }

    base = g_cfgioBase[instance];

    /* Create resource allocation mutex */
    (void)OSIF_MutexCreate(&(deviceState->resMutex));

    /* Reset the CFGIO module */
    CFGIO_HW_Init(base);

    /* Enable CFGIO interrupt in the interrupt manager */
    INT_SYS_EnableIRQ(g_cfgioIrq[instance]);

    /* Enable the CFGIO module */
    CFGIO_HW_SetModuleEnable(base, true);

    /* Initialize device structure */
    deviceState->resAlloc = 0U;

    /* Clear state array */
    for (uint8_t cnt = 0U; cnt < FEATURE_CFGIO_MAX_SHIFTER_COUNT; cnt++)
    {
        deviceState->commonStates[cnt] = NULL;
    }

    g_cfgioDevState[instance] = deviceState;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Deinitialize the CFGIO device
 *
 * @param instance  CFGIO instance number
 *
 * @retval Error status
 */
STATUS_T CFGIO_DeinitDevice(uint32_t instance)
{
    CFGIO_T *base = g_cfgioBase[instance];

    /* Reset the CFGIO module */
    CFGIO_HW_Init(base);

    /* Disable CFGIO interrupt in the interrupt manager */
    INT_SYS_DisableIRQ(g_cfgioIrq[instance]);

    /* Destroy the resource allocation mutex */
    (void)OSIF_MutexDestroy(&(g_cfgioDevState[instance]->resMutex));

    g_cfgioDevState[instance] = NULL;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Reset the CFGIO device
 *
 * @param instance  CFGIO instance number
 *
 * @retval Error status
 */
STATUS_T CFGIO_ResetDevice(uint32_t instance)
{
    CFGIO_T *base = g_cfgioBase[instance];
    CFGIO_HW_Init(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Initialize an instance of CFGIO driver
 *
 * @param instance      CFGIO instance number
 * @param driverState   Pointer to driver state
 *
 * @retval Error status
 */
STATUS_T CFGIO_InitDriver(uint32_t instance, CFGIO_COMMON_STATE_T *driverState)
{
    STATUS_T result;
    CFGIO_COMMON_STATE_T **commonStates;

    /* Allocate times and shifters for the driver */
    result = CFGIO_AllocateResources(instance, driverState);
    if (result == STATUS_SUCCESS)
    {
        /* Get driver list for this device instance */
        commonStates = g_cfgioDevState[instance]->commonStates;

        /* Find an empty state structure slot for the driver */
        for (uint16_t cnt = 0U; cnt < FEATURE_CFGIO_MAX_SHIFTER_COUNT; cnt++)
        {
            if (commonStates[cnt] == NULL)
            {
                /* Place the new driver struct here */
                commonStates[cnt] = driverState;
                break;
            }
        }

        /* Initialize the allocated resources */
        CFGIO_InitResources(instance, driverState);

        /* Initialize the rest of the common state structure */
        driverState->instance = instance;
        driverState->isr = NULL;
    }
    return result;
}

/*!
 * @brief   Deinitialize an instance of CFGIO driver
 *
 * @param driverState   Pointer to driver state
 *
 * @retval Error status
 */
STATUS_T CFGIO_DeinitDriver(const CFGIO_COMMON_STATE_T *driverState)
{
    uint32_t instance = driverState->instance;
    CFGIO_COMMON_STATE_T **commonStates;

    /* Get driver list for this device instance */
    commonStates = g_cfgioDevState[instance]->commonStates;

    /* Find the driver in the list */
    for (uint16_t cnt = 0U; cnt < FEATURE_CFGIO_MAX_SHIFTER_COUNT; cnt++)
    {
        if (commonStates[cnt] == driverState)
        {
            /* Found it and remove it */
            commonStates[cnt] = NULL;
            break;
        }
    }

    /* Deallocate timers and shifters for the driver */
    CFGIO_DeallocateResources(instance, driverState);
    return STATUS_SUCCESS;
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the resources for the driver
 *
 * @param instance      CFGIO instance number
 * @param driverState   Pointer to driver structure
 *
 * @retval None
 */
static void CFGIO_InitResources(
    uint32_t instance,
    const CFGIO_COMMON_STATE_T *driverState)
{
    uint8_t resCount = driverState->resCount;
    uint8_t resIndex = driverState->resIndex;
    CFGIO_T *base = g_cfgioBase[instance];

    for (uint8_t i = resIndex; i < (resIndex + resCount); i++)
    {
        /* Ensure all shifters/timers are disabled */
        CFGIO_HW_SetTimerMode(base, i, CFGIO_TMR_MODE_DISABLED);
        CFGIO_HW_SetShifterMode(base, i, CFGIO_SHIFT_MODE_DISABLED);

        /* Ensure all interrupts and DMA requests are disabled */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << i), false);
        CFGIO_HW_SetShifterInterrupt(base, (uint8_t)(1U << i), false);
        CFGIO_HW_SetShifterErrorInterrupt(base, (uint8_t)(1U << i), false);
        CFGIO_HW_SetShifterDmaRequest(base, (uint8_t)(1U << i), false);

        /* Clear any left over flags */
        CFGIO_HW_ClearTimerStatus(base, i);
        CFGIO_HW_ClearShifterStatus(base, i);
        CFGIO_HW_ClearShifterErrorStatus(base, i);
    }
}

/*!
 * @brief   Allocate timers and shifters for a new driver instance
 *
 * @param instance      CFGIO instance number
 * @param driverState   Pointer to driver structure
 *
 * @retval Error status
 */
static STATUS_T CFGIO_AllocateResources(
    uint32_t instance,
    CFGIO_COMMON_STATE_T *driverState)
{
    STATUS_T result = STATUS_ERROR;
    const CFGIO_T *base = g_cfgioBase[instance];

    uint8_t n = 0;      /* Used to iterate through resources (shifters/timers) */
    uint8_t shifterNum; /* Total number of resources */
    uint8_t resCount;   /* How many resources are needed for this driver */
    uint8_t resAlloc;   /* Current resource allocation map */
    uint8_t resMask;    /* Bitmask corresponding to current resources */

    /**
     * Find free resources for a new driver. Drivers may need one or two
     * adjacent shifters and timers.
     */
    shifterNum = CFGIO_HW_GetShifterNumber(base);
    resCount = driverState->resCount;
    resAlloc = g_cfgioDevState[instance]->resAlloc;
    resMask = (uint8_t)((1U << resCount) - 1U);

    (void)OSIF_MutexLock(&(g_cfgioDevState[instance]->resMutex),
                         OSIF_WAIT_FOREVER);

    /* Find available shifters and timers for the driver */
    while (  (result == STATUS_ERROR)
          && (((uint8_t)(n + resCount)) <= shifterNum))
    {
        if ((resAlloc & resMask) == 0U)
        {
            /* Resources found, mark resources as busy */
            driverState->resIndex = n;
            g_cfgioDevState[instance]->resAlloc |= resMask;
            result = STATUS_SUCCESS;
        }
        /* Continue searching */
        n += 1U;
        resMask <<= 1;
    }

    (void)OSIF_MutexUnlock(&(g_cfgioDevState[instance]->resMutex));
    return result;
}

/*!
 * @brief   Deallocate timers and shifters for a driver instance
 *
 * @param instance      CFGIO instance number
 * @param driverState   Pointer to driver structure
 *
 * @retval None
 */
static void CFGIO_DeallocateResources(
    uint32_t instance,
    const CFGIO_COMMON_STATE_T *driverState)
{
    uint8_t mask = (uint8_t)((1U << driverState->resCount) - 1U);
    mask <<= driverState->resIndex;

    (void)OSIF_MutexLock(&(g_cfgioDevState[instance]->resMutex),
                         OSIF_WAIT_FOREVER);

    g_cfgioDevState[instance]->resAlloc &= (uint8_t)~mask;

    (void)OSIF_MutexUnlock(&(g_cfgioDevState[instance]->resMutex));
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

#if (CFGIO_INSTANCE_COUNT > 0U)
/*!
 * @brief Initialize all CFGIO registers to their reset value
 *
 * @param base  CFGIO instance base address
 *
 * @retval None
 */
 void CFGIO_HW_Init(CFGIO_T *base)
{
    /* Use software reset bit to reset the module */
    CFGIO_HW_SoftwareReset(base, true);

    /* The control register is not affected by software reset */
    base->CTRL.reg = 0x0U;
}

/*!
 * @brief   Perform software reset of the CFGIO module
 * @details Calling this function with enable parameter set to true will reset
 *          all registers except the CFGIO Control Register. This register
 *          bit will remain set until cleared by software.
 *
 * @param base      CFGIO instance base address
 * @param enable    Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SoftwareReset(CFGIO_T *base, bool enable)
{
    base->CTRL.bit.SWRSTEN = enable;
}

/*!
 * @brief Enable or disable the CFGIO module
 *
 * @param base      CFGIO instance base address
 * @param enable    Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SetModuleEnable(CFGIO_T *base, bool enable)
{
    base->CTRL.bit.CFGIOEN = enable;
}

/*!
 * @brief Enable or disable CFGIO in debug mode
 *
 * @param base      CFGIO instance base address
 * @param enable    Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SetDebugMode(CFGIO_T *base, bool enable)
{
    base->CTRL.bit.DBGEN = enable;
}

/*!
 * @brief Return the number of shifters implemented
 *
 * @param base  CFGIO instance base address
 *
 * @retval Number of shifters
 */
uint8_t CFGIO_HW_GetShifterNumber(const CFGIO_T *base)
{
    return  (uint8_t)(base->PARA.bit.SHINUMI);
}

/*!
 * @brief Return the input data on each of the CFGIO pins
 *
 * @param base  CFGIO instance base address
 *
 * @retval Pin data
 */
uint8_t CFGIO_HW_GetPinData(const CFGIO_T *base)
{
    return (uint8_t)(base->PIDATA.bit.PIDATA);
}

/*!
 * @brief Set timers configurations
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param start     Timer start options
 * @param stop      Timer stop options
 * @param enable    Timer enable options
 * @param disable   Timer disable options
 * @param reset     Timer reset options
 * @param decrement Timer decrement options
 * @param output    Timer initial output
 *
 * @retval None
 */
void CFGIO_HW_SetTimerConfig(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_START_T start,
    CFGIO_TMR_STOP_T stop,
    CFGIO_TMR_ENABLE_T enable,
    CFGIO_TMR_DISABLE_T disable,
    CFGIO_TMR_RESET_T reset,
    CFGIO_TMR_DEC_T decrement,
    CFGIO_TMR_OUTPUT_T output)
{
    base->TMRCFG[timer].bit.TMRSTAEN = start;
    base->TMRCFG[timer].bit.TMRSTOPFSEL = stop;
    base->TMRCFG[timer].bit.TMRENCFG = enable;
    base->TMRCFG[timer].bit.TMRDISCFG = disable;
    base->TMRCFG[timer].bit.TMRRSTCFG = reset;
    base->TMRCFG[timer].bit.TMRDCFG = decrement;
    base->TMRCFG[timer].bit.TMROCFG = output;
}

/*!
 * @brief   Configure the condition that causes the timer to be enabled and
 *          start decrementing
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param enable    Timer enable options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerEnable(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_ENABLE_T enable)
{
    base->TMRCFG[timer].bit.TMRENCFG = enable;
}

/*!
 * @brief   Configure the condition that causes the timer to be disabled and
 *          stop decrementing
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param disable   Timer disable options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerDisable(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_DISABLE_T disable)
{
    base->TMRCFG[timer].bit.TMRDISCFG = disable;
}

/*!
 * @brief   Configure the timer start options
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param start     Timer start options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerStart(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_START_T start)
{
    base->TMRCFG[timer].bit.TMRSTAEN = start;
}

/*!
 * @brief   Configure the timer stop options
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param stop      Timer stop bit options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerStop(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_STOP_T stop)
{
    base->TMRCFG[timer].bit.TMRSTOPFSEL = stop;
}

/*!
 * @brief   Configure the timer reset options
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param reset     Timer reset options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerReset(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_RESET_T reset)
{
    base->TMRCFG[timer].bit.TMRRSTCFG = reset;
}

/*!
 * @brief   Configure the source of the timer decrement and the source of the
 *          shift clock
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param decrement Timer decrement options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerDecrement(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_DEC_T decrement)
{
    base->TMRCFG[timer].bit.TMRDCFG = decrement;
}

/*!
 * @brief   Configure the initial state of the timer output and whether it is
 *          affected by the timer reset
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param output    Timer initial output options
 *
 * @retval None
 */
void CFGIO_HW_SetTimerInitialOutput(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_OUTPUT_T output)
{
    base->TMRCFG[timer].bit.TMROCFG = output;
}

/*!
 * @brief   Configure the control settings for the timer
 *
 * @param base              CFGIO instance base address
 * @param timer             Timer index
 * @param triggerSelect     Trigger select
 * @param triggerPolarity   Trigger polarity
 * @param triggerSource     Trigger source
 * @param pinSelect         Pin select
 * @param pinPolarity       Pin polarity
 * @param pinConfig         Pin configuration
 * @param mode              Timer mode
 *
 * @retval None
 */
void CFGIO_HW_SetTimerControl(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t triggerSelect,
    CFGIO_TRG_POLARITY_T triggerPolarity,
    CFGIO_TRG_SRC_T triggerSource,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig,
    CFGIO_TMR_MODE_T mode)
{
    base->TMRCTRL[timer].bit.TRGVSEL =  triggerSelect;
    base->TMRCTRL[timer].bit.TRGPOLCFG = triggerPolarity;
    base->TMRCTRL[timer].bit.TRGSSEL = triggerSource;
    base->TMRCTRL[timer].bit.TMRUPSEL = pinSelect;
    base->TMRCTRL[timer].bit.TMRPPOLCFG = pinPolarity;
    base->TMRCTRL[timer].bit.TMROPCFG = pinConfig;
    base->TMRCTRL[timer].bit.TMRMCFG = mode;
}

/*!
 * @brief   Configure timer pin
 *
 * @param base              CFGIO instance base address
 * @param timer             Timer index
 * @param pinSelect         Pin select
 * @param pinPolarity       Pin polarity
 * @param pinConfig         Pin configuration
 *
 * @retval None
 */
void CFGIO_HW_SetTimerPin(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig)
{
    base->TMRCTRL[timer].bit.TMRUPSEL = pinSelect;
    base->TMRCTRL[timer].bit.TMRPPOLCFG = pinPolarity;
    base->TMRCTRL[timer].bit.TMROPCFG = pinConfig;
}

/*!
 * @brief   Configure timer trigger
 *
 * @param base              CFGIO instance base address
 * @param timer             Timer index
 * @param triggerSelect     Trigger select
 * @param triggerPolarity   Trigger polarity
 * @param triggerSource     Trigger source
 *
 * @retval None
 */
void CFGIO_HW_SetTimerTrigger(
    CFGIO_T *base,
    uint8_t timer,
    uint8_t triggerSelect,
    CFGIO_TRG_POLARITY_T triggerPolarity,
    CFGIO_TRG_SRC_T triggerSource)
{
    base->TMRCTRL[timer].bit.TRGVSEL = triggerSelect;
    base->TMRCTRL[timer].bit.TMRPPOLCFG = triggerPolarity;
    base->TMRCTRL[timer].bit.TRGSSEL = triggerSource;
}

/*!
 * @brief   Configure timer mode
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param mode      Timer mode
 *
 * @retval None
 */
void CFGIO_HW_SetTimerMode(
    CFGIO_T *base,
    uint8_t timer,
    CFGIO_TMR_MODE_T mode)
{
    base->TMRCTRL[timer].bit.TMRMCFG = mode;
}

/*!
 * @brief   Set timer compare value
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 * @param value     Timer compare value
 *
 * @retval None
 */
void CFGIO_HW_SetTimerCompare(CFGIO_T *base, uint8_t timer, uint16_t value)
{
    base->TMRCOMP[timer].reg = (uint32_t)value;
}

/*!
 * @brief   Get the timer compare value
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 *
 * @retval None
 */
uint16_t CFGIO_HW_GetTimerCompare(const CFGIO_T *base, uint8_t timer)
{
    return (uint16_t)(base->TMRCOMP[timer].reg);
}

/*!
 * @brief   Enable or disable timer interrupts
 * @details The interrupt mask must contain a bit of 1 for each timer who's
 *          interrupt must be enabled or disabled.
 *
 * @param base          CFGIO instance base address
 * @param interruptMask Interrupt mask
 * @param enable        Interrupt enable bit
 *
 * @retval None
 */
void CFGIO_HW_SetTimerInterrupts(CFGIO_T *base, uint8_t interruptMask, bool enable)
{
    uint32_t tempValue = base->TMRSTSIEN.reg;

    if (enable)
    {
        tempValue |= interruptMask;
    }
    else
    {
        tempValue &= ~interruptMask;
    }
    base->TMRSTSIEN.reg = tempValue;
}

/*!
 * @brief   Get the state of the interrupt for all timers
 * @details Each bit in the returned value specifies the interrupt state for
 *          one timer, starting with timer 0 from least significant bit.
 *
 * @param base  CFGIO instance base address
 *
 * @retval State of the interrupt for all timers
 */
uint32_t CFGIO_HW_GetTimerInterruptStates(const CFGIO_T *base)
{
    return base->TMRSTSIEN.reg;
}

/*!
 * @brief   Get the status of all timers
 *
 * @param base  CFGIO instance base address
 *
 * @retval Status of all timers
 */
uint32_t CFGIO_HW_GetAllTimerStatus(const CFGIO_T *base)
{
    return base->TMRSTS.reg;
}

/*!
 * @brief   Get the status of the timer
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 *
 * @retval Timer status
 */
bool CFGIO_HW_GetTimerStatus(const CFGIO_T *base, uint8_t timer)
{
    uint32_t regValue = (uint32_t)base->TMRSTS.reg;
    regValue = (regValue >> timer) & 1U;
    return (bool)regValue;
}

/*!
 * @brief   Clear the status flag for the timer
 *
 * @param base      CFGIO instance base address
 * @param timer     Timer index
 *
 * @retval None
 */
void CFGIO_HW_ClearTimerStatus(CFGIO_T *base, uint8_t timer)
{
    base->TMRSTS.reg = 1UL << timer;
}

/*!
 * @brief   Set shifter configurations
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param start     Shifter start bit options
 * @param stop      Shifter stop bit options
 * @param source    Shifter input source options
 *
 * @retval None
 */
void CFGIO_HW_SetShifterConfig(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_START_T start,
    CFGIO_SHIFT_STOP_T stop,
    CFGIO_SHIFT_SRC_T source)
{
    base->SCFG[shifter].bit.ISRCSEL = source;
    base->SCFG[shifter].bit.SSTAFSEL = start;
    base->SCFG[shifter].bit.SSTOPFSEL = stop;
}

/*!
 * @brief   Set shifter input source
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param source    Shifter input source
 *
 * @retval None
 */
void CFGIO_HW_SetShifterInputSource(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_SRC_T source)
{
    base->SCFG[shifter].bit.ISRCSEL = source;
}

/*!
 * @brief   Set shifter start bit options
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param start     Shifter start bit options
 *
 * @retval None
 */
void CFGIO_HW_SetShifterStartBit(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_START_T start)
{
    base->SCFG[shifter].bit.SSTAFSEL = start;
}

/*!
 * @brief   Set shifter stop bit options
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param stop      Shifter stop bit options
 *
 * @retval None
 */
void CFGIO_HW_SetShifterStopBit(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_STOP_T stop)
{
    base->SCFG[shifter].bit.SSTOPFSEL = stop;
}

/*!
 * @brief   Set shifter control configurations
 *
 * @param base              CFGIO instance base address
 * @param shifter           Shifter index
 * @param mode              Shifter mode
 * @param pinSelect         Pin select
 * @param pinPolarity       Pin polarity
 * @param pinConfig         Pin configuration
 * @param timerSelect       Timer select
 * @param timerPolarity     Timer polarity
 *
 * @retval None
 */
void CFGIO_HW_SetShifterControl(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_MODE_T mode,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig,
    uint8_t timerSelect,
    CFGIO_TMR_POLARITY_T timerPolarity)
{
    base->SCTRL[shifter].bit.TMRSEL = timerSelect;
    base->SCTRL[shifter].bit.SCLKLCFG = timerPolarity;
    base->SCTRL[shifter].bit.SUPSEL = pinSelect;
    base->SCTRL[shifter].bit.SPPOLCFG = pinPolarity;
    base->SCTRL[shifter].bit.SOPCFG = pinConfig;
    base->SCTRL[shifter].bit.SMCFG = mode;
}

/*!
 * @brief   Set shifter timer configurations
 *
 * @param base              CFGIO instance base address
 * @param shifter           Shifter index
 * @param timerSelect       Timer select
 * @param timerPolarity     Timer polarity
 *
 * @retval None
 */
void CFGIO_HW_SetShifterTimer(
    CFGIO_T *base,
    uint8_t shifter,
    uint8_t timerSelect,
    CFGIO_TMR_POLARITY_T timerPolarity)
{
    base->SCTRL[shifter].bit.TMRSEL = timerSelect;
    base->SCTRL[shifter].bit.SPPOLCFG = timerPolarity;
}

/*!
 * @brief   Set shifter pin configurations
 *
 * @param base              CFGIO instance base address
 * @param shifter           Shifter index
 * @param pinSelect         Pin select
 * @param pinPolarity       Pin polarity
 * @param pinConfig         Pin configuration
 *
 * @retval None
 */
void CFGIO_HW_SetShifterPin(
    CFGIO_T *base,
    uint8_t shifter,
    uint8_t pinSelect,
    CFGIO_PIN_POLARITY_T pinPolarity,
    CFGIO_PIN_CONFIG_T pinConfig)
{
    base->SCTRL[shifter].bit.SUPSEL = pinSelect;
    base->SCTRL[shifter].bit.SPPOLCFG = pinPolarity;
    base->SCTRL[shifter].bit.SOPCFG = pinConfig;
}

/*!
 * @brief   Set shifter pin configuration
 *
 * @param base              CFGIO instance base address
 * @param shifter           Shifter index
 * @param pinConfig         Pin configuration
 *
 * @retval None
 */
void CFGIO_HW_SetShifterPinConfig(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_PIN_CONFIG_T pinConfig)
{
    base->SCTRL[shifter].bit.SOPCFG = pinConfig;
}

/*!
 * @brief   Set shifter mode
 *
 * @param base              CFGIO instance base address
 * @param shifter           Shifter index
 * @param mode              Shifter mode
 *
 * @retval None
 */
void CFGIO_HW_SetShifterMode(
    CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_MODE_T mode)
{
    base->SCTRL[shifter].bit.SMCFG = mode;
}

/*!
 * @brief   Get the status of all shifters
 *
 * @param base  CFGIO instance base address
 *
 * @retval Status of all shifters
 */
uint32_t CFGIO_HW_GetAllShifterStatus(const CFGIO_T *base)
{
    return base->SSTS.reg;
}

/*!
 * @brief   Get the status of the shifter
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 *
 * @retval Shifter status
 */
bool CFGIO_HW_GetShifterStatus(const CFGIO_T *base, uint8_t shifter)
{
    uint32_t reg = (uint32_t)base->SSTS.reg;
    reg = (reg >> shifter) & 1U;
    return (bool)reg;
}

/*!
 * @brief   Clear the status for the shifter
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 *
 * @retval None
 */
void CFGIO_HW_ClearShifterStatus(CFGIO_T *base, uint8_t shifter)
{
    base->SSTS.reg = 1UL << shifter;
}

/*!
 * @brief   Get the error status of all shifters
 *
 * @param base  CFGIO instance base address
 *
 * @retval Error status of all shifters
 */
uint32_t CFGIO_HW_GetAllShifterErrorStatus(const CFGIO_T *base)
{
    return base->SERRFLG.reg;
}

/*!
 * @brief   Get the error status of the shifter
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 *
 * @retval Shifter error status
 */
bool CFGIO_HW_GetShifterErrorStatus(const CFGIO_T *base, uint8_t shifter)
{
    uint32_t regValue = (uint32_t)base->SERRFLG.reg;
    regValue = (regValue >> shifter) & 1U;
    return (bool)regValue;
}

/*!
 * @brief   Clear the error status for the shifter
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 *
 * @retval None
 */
void CFGIO_HW_ClearShifterErrorStatus(CFGIO_T *base, uint8_t shifter)
{
    base->SERRFLG.reg = 1UL << shifter;
}

/*!
 * @brief   Get the interrupt status of all shifters
 *
 * @param base  CFGIO instance base address
 *
 * @retval Interrupt status of all shifters
 */
uint32_t CFGIO_HW_GetAllShifterInterrupt(const CFGIO_T *base)
{
    return base->SSTSIEN.reg;
}

/*!
 * @brief   Enable or disable specified shifter interrupts
 * @details The interrupt mask must contain a bit of 1 for each shifter who's
 *          interrupt must be enabled or disabled.
 *
 * @param base              CFGIO instance base address
 * @param interruptMask     Interrupt mask
 * @param enable            Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SetShifterInterrupt(CFGIO_T *base, uint8_t interruptMask, bool enable)
{
    uint32_t tempValue = base->SSTSIEN.reg;

    if (enable)
    {
        tempValue |= interruptMask;
    }
    else
    {
        tempValue &= ~interruptMask;
    }
    base->SSTSIEN.reg = tempValue;
}

/*!
 * @brief   Get the error interrupt status of all shifters
 *
 * @param base  CFGIO instance base address
 *
 * @retval Error interrupt status of all shifters
 */
uint32_t CFGIO_HW_GetAllShifterErrorInterrupt(const CFGIO_T *base)
{
    return base->SEFIEN.reg;
}

/*!
 * @brief   Enable or disable specified shifter error interrupts
 * @details The interrupt mask must contain a bit of 1 for each shifter who's
 *          interrupt must be enabled or disabled.
 *
 * @param base              CFGIO instance base address
 * @param interruptMask     Interrupt mask
 * @param enable            Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SetShifterErrorInterrupt(CFGIO_T *base, uint8_t interruptMask, bool enable)
{
    uint32_t tempValue = base->SEFIEN.reg;

    if (enable)
    {
        tempValue |= interruptMask;
    }
    else
    {
        tempValue &= ~interruptMask;
    }
    base->SEFIEN.reg = tempValue;
}

/*!
 * @brief   Read data from the specified shifter buffer
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param mode      Read/Write mode for shifter buffer
 *
 * @retval Shifter data
 */
uint32_t CFGIO_HW_ReadShifterBuffer(
    const CFGIO_T *base,
    uint8_t shifter,
    CFGIO_SHIFT_BUFFER_MODE_T mode)
{
    uint32_t value = 0;

    if (mode == CFGIO_SHIFT_RW_MODE_NORMAL)
    {
        value = base->SBUF[shifter].reg;
    }
    else if (mode == CFGIO_SHIFT_RW_MODE_BIT_SWAP)
    {
        value = base->SBUFBITSWA[shifter].reg;
    }
    else
    {
        value = 0;
    }
    return value;
}

/*!
 * @brief   Write data into the specified shifter buffer
 *
 * @param base      CFGIO instance base address
 * @param shifter   Shifter index
 * @param value     The value to be written
 * @param mode      Read/Write mode for shifter buffer
 *
 * @retval None
 */
void CFGIO_HW_WriteShifterBuffer(
    CFGIO_T *base,
    uint8_t shifter,
    uint32_t value,
    CFGIO_SHIFT_BUFFER_MODE_T mode)
{
    if (mode == CFGIO_SHIFT_RW_MODE_NORMAL)
    {
        base->SBUF[shifter].reg = value;
    }
    else if (mode == CFGIO_SHIFT_RW_MODE_BIT_SWAP)
    {
        base->SBUFBITSWA[shifter].reg = value;
    }
    else
    {
    }
}

/*!
 * @brief   Enable or disable shifter DMA requests
 * @details The request mask must contain a bit of 1 for each shifter who's DMA
 *          requests must be enabled or disabled.
 *
 * @param base          CFGIO instance base address
 * @param requestMask   Request mask
 * @param enable        Enable or disable
 *
 * @retval None
 */
void CFGIO_HW_SetShifterDmaRequest(CFGIO_T *base, uint8_t requestMask, bool enable)
{
    uint32_t tempValue = base->DMAREQEN.reg;

    if (enable)
    {
        tempValue |= requestMask;
    }
    else
    {
        tempValue &= ~requestMask;
    }
    base->DMAREQEN.reg = tempValue;
}

/*!
 * @brief   CFGIO interrupt handler
 */
void CFGIO_IRQHandler(void)
{
    CFGIO_COMMON_STATE_T * const *driverList;
    CFGIO_COMMON_STATE_T *driverState;
    uint32_t interruptsEnabled;
    uint32_t shifterEvents;
    uint32_t shifterErrorEvents;
    uint32_t timerEvents;
    uint32_t resMask;
    uint32_t index = 0U;
    const CFGIO_T *base = g_cfgioBase[index];

    /* Get shifter events */
    interruptsEnabled = CFGIO_HW_GetAllShifterInterrupt(base);
    shifterEvents = CFGIO_HW_GetAllShifterStatus(base) & interruptsEnabled;

    /* Get shifter error events */
    interruptsEnabled = CFGIO_HW_GetAllShifterErrorInterrupt(base);
    shifterErrorEvents = CFGIO_HW_GetAllShifterErrorStatus(base) & interruptsEnabled;

    /* Get timer events */
    interruptsEnabled = CFGIO_HW_GetTimerInterruptStates(base);
    timerEvents = CFGIO_HW_GetAllTimerStatus(base) & interruptsEnabled;

    /* Get the driver list for current instance */
    driverList = g_cfgioDevState[index]->commonStates;

    for (uint16_t n = 0U; n < FEATURE_CFGIO_MAX_SHIFTER_COUNT; n++)
    {
        driverState = driverList[n];

        /* Check if the driver is initialized and uses interrupts */
        if ((driverState != NULL) && (driverState->isr != NULL))
        {
            /* Calculate mask of shifters/timers used by this driver */
            resMask = ((1UL << driverState->resCount) - 1U) << driverState->resIndex;

            /* Check if this instance has pending events */
            if (   ((shifterEvents & resMask) != 0U)
                || ((shifterErrorEvents & resMask) != 0U)
                || ((timerEvents & resMask) != 0U))
            {
                /* Call the isr to process the event */
                (driverState->isr)(driverState);
            }
        }
    }
}
#endif /* CFGIO_INSTANCE_COUNT > 0U */

/**@} end of group CFGIO_COMMON_Functions*/
/**@} end of group CFGIO_COMMON_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
