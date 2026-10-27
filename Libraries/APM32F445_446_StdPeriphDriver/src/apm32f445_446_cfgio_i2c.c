/*!
 * @file        apm32f445_446_cfgio_i2c.c
 *
 * @brief       CFGIO_I2C driver functions
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
#include "apm32f445_446_cfgio_i2c.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_I2C_Driver CFGIO_I2C Driver
  @{
*/

/** @defgroup CFGIO_I2C_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

#define CFGIO_SCFG_SSTOPFSEL_MASK     0x30U
#define CFGIO_SCFG_SSTOPFSEL_SHIFT    4U
#define CFGIO_SCFG_SSTOPFSEL(x)       (((uint32_t)(((uint32_t)(x)) << CFGIO_SCFG_SSTOPFSEL_SHIFT)) & CFGIO_SCFG_SSTOPFSEL_MASK)

/**
 * Shifters/Timers used for I2C simulation.
 * The parameter x is the index value of the driver instance.
 */
#define TIMER_INDEX_CTRL(x)     (uint8_t)((x) + 1U)
#define TIMER_INDEX_SCL(x)      (x)
#define SHIFTER_INDEX_RX(x)     (uint8_t)((x) + 1U)
#define SHIFTER_INDEX_TX(x)     (x)

/* Used for baudrate calculation */
#define MAX_DIVIDER_VALUE  0xFF
#define MIN_DIVIDER_VALUE  1

/**@} end of group CFGIO_I2C_Macros*/

/** @defgroup CFGIO_I2C_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

static void CFGIO_I2C_MasterCFGIOConfig(
    const CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t inputClock,
    uint32_t baudrate);

static void CFGIO_I2C_MasterSetNumberClocks(
    CFGIO_T *base,
    CFGIO_I2C_MASTER_STATE_T *states);

static void CFGIO_I2C_MasterCalcBaudrateDivider(
    uint32_t baudrate,
    uint16_t *divider,
    uint32_t inputClock);

static uint32_t CFGIO_I2C_MasterCalcRxRegAddr(const CFGIO_I2C_MASTER_STATE_T *states);
static uint32_t CFGIO_I2C_MasterCalcTxRegAddr(const CFGIO_I2C_MASTER_STATE_T *states);

static void CFGIO_I2C_MasterSendSlaveAddr(
    CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states);

static void CFGIO_I2C_MasterWriteData(CFGIO_I2C_MASTER_STATE_T *states);
static void CFGIO_I2C_MasterReadData(CFGIO_I2C_MASTER_STATE_T *states);

static void CFGIO_I2C_MasterEnableTimerShifter(CFGIO_I2C_MASTER_STATE_T *states);

static STATUS_T CFGIO_I2C_MasterStartTransfer(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t size,
    bool sendStop,
    CFGIO_I2C_DIR_T dir);

static void CFGIO_I2C_MasterStopTransfer(CFGIO_I2C_MASTER_STATE_T *states);
static void CFGIO_I2C_MasterEndTransfer(CFGIO_I2C_MASTER_STATE_T *states);

static STATUS_T CFGIO_I2C_MasterWaitTransferEnd(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t timeout);

static void CFGIO_I2C_MasterCheckStatus(void *statePtr);

static bool CFGIO_I2C_MasterBusIsBusy(
    const CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states);

static bool CFGIO_I2C_MasterCheckNack(
    const CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states);

static void CFGIO_I2C_MasterDmaRxConfig(
    CFGIO_I2C_MASTER_STATE_T *states,
    DMA_SOFTWARE_TCD_T *stcdBase);

static void CFGIO_I2C_MasterDmaTxConfig(
    CFGIO_I2C_MASTER_STATE_T *states,
    DMA_SOFTWARE_TCD_T *stcdBase);

static void CFGIO_I2C_MasterDmaTransferStart(CFGIO_I2C_MASTER_STATE_T *states);
static void CFGIO_I2C_MasterDmaTransferEnd(void *statePtr);

static void CFGIO_I2C_MasterTriggerDmaBlock(
    DMA_SOFTWARE_TCD_T *stcdBase,
    uint8_t blockNo);

static void CFGIO_I2C_MasterTerminateDmaBlock(
    DMA_SOFTWARE_TCD_T *stcdBase,
    uint8_t blockNo);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Initialize the CFGIO_I2C master mode
 *
 * @param instance  CFGIO instance number
 * @param configPtr Pointer to the CFGIO_I2C master user configuration.
 *                  The function reads configuration data from this structure
 *                  and initializes the driver accordingly. The application may
 *                  free this structure after the function returns.
 * @param states    Pointer to master context structure. The driver
 *                  uses this memory area for its internal logic. The application
 *                  must make no assumptions about the content of this structure,
 *                  and must not free this memory until the driver is reset using
 *                  CFGIO_I2C_MasterDeinit().
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterInit(
    uint32_t instance,
    const CFGIO_I2C_MASTER_USER_CONFIG_T *configPtr,
    CFGIO_I2C_MASTER_STATE_T *states)
{
    STATUS_T result;
    uint32_t inputClock;
    uint8_t dmaTxRequest;
    uint8_t dmaRxRequest;

    /* Get protocol clock frequency */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[instance], &inputClock);

    /* Instruct the resource allocator that we need two shifters/timers */
    states->commonState.resCount = 2U;

    /* Common CFGIO driver initialization */
    result = CFGIO_InitDriver(instance, &(states->commonState));
    if (result != STATUS_SUCCESS)
    {
        return result;
    }

    /* Initialize the semaphore */
    if (configPtr->transferType != CFGIO_USE_POLLING)
    {
        (void)OSIF_SemCreate(&(states->idleSemaphore), 0U);
    }

    /* Initialize driver-specific context structure */
    states->sdaPin = configPtr->sdaPin;
    states->sclPin = configPtr->sclPin;
    states->transferType = configPtr->transferType;
    states->slaveAddr = configPtr->slaveAddr;
    states->callback = configPtr->callback;
    states->callbackParam = configPtr->callbackParam;
    states->isIdle = true;
    states->isBlocking = false;
    states->driverStatus = STATUS_SUCCESS;

    /* Configure the device for I2C mode */
    CFGIO_I2C_MasterCFGIOConfig(states, inputClock, configPtr->baudrate);

    /* Set up transfer engine */
    switch (states->transferType)
    {
    case CFGIO_USE_DMA:
        /* Store DMA channel numbers */
        states->txDmaChannel = configPtr->txDmaChannel;
        states->rxDmaChannel = configPtr->rxDmaChannel;

        /* Configure DMA request sources */
        dmaTxRequest = g_cfgioDmaSrc[instance][SHIFTER_INDEX_TX(states->commonState.resIndex)];
        dmaRxRequest = g_cfgioDmaSrc[instance][SHIFTER_INDEX_RX(states->commonState.resIndex)];
         (void)DMA_ConfigChannelRequestAndTrigger(configPtr->txDmaChannel, dmaTxRequest, false);
         (void)DMA_ConfigChannelRequestAndTrigger(configPtr->rxDmaChannel, dmaRxRequest, false);

        /* Use timer interrupts to signal transfer end */
        states->commonState.isr = CFGIO_I2C_MasterDmaTransferEnd;
        break;
    case CFGIO_USE_POLLING:
        /* Nothing to do here, CFGIO_I2C_MasterGetStatus() will handle the transfer */
        break;
    case CFGIO_USE_INTERRUPTS:
        states->commonState.isr = CFGIO_I2C_MasterCheckStatus;
        break;
    default:
        /* Nothing to do */
        break;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief   Deinitialize the CFGIO_I2C master mode driver
 * @details This function deinitialize the CFGIO_I2C driver. The driver cannot
 *          be used again until initialized again. The context structure is not
 *          needed by the driver and can be freed after calling this function.
 *
 * @param states    Pointer to master context structure
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterDeinit(CFGIO_I2C_MASTER_STATE_T *states)
{
    if (states->isIdle)
    {
        if (states->transferType != CFGIO_USE_POLLING)
        {
            (void)OSIF_SemDestroy(&(states->idleSemaphore));
        }
        return CFGIO_DeinitDriver(&(states->commonState));
    }
    return STATUS_BUSY;
}

/*!
 * @brief Return the default configuration
 *
 * @param configPtr Pointer to the CFGIO_I2C user configuration
 *
 * @retval None
 */
void CFGIO_I2C_DefaultConfig(CFGIO_I2C_MASTER_USER_CONFIG_T *configPtr)
{
    configPtr->transferType = CFGIO_USE_INTERRUPTS;
    configPtr->sdaPin = 0U;
    configPtr->sclPin = 1U;
    configPtr->slaveAddr = 32U;
    configPtr->baudrate = 100000U;
    configPtr->txDmaChannel = 255U;
    configPtr->rxDmaChannel = 255U;
    configPtr->callback = NULL;
    configPtr->callbackParam = NULL;
}

/*!
 * @brief Get the currently configured baudrate
 *
 * @param states    Pointer to master context structure
 * @param baudrate  Current baudrate in Hertz
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterGetBaudrate(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t *baudrate)
{
    uint32_t inputClock;
    uint16_t timerCompare;
    uint16_t divider;
    uint8_t index = states->commonState.resIndex;
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    /* Get protocol clock frequency */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[states->commonState.instance],
                       &inputClock);

    /* Read the currently configured divider */
    timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));
    divider = (uint16_t)(timerCompare & 0x00FFU);

    /* Baudrate = InputClock / (2 * (divider + 2)). Round to nearest integer */
    *baudrate = (inputClock + divider + 2U) / (2U * ((uint32_t)divider + 2U));

    return STATUS_SUCCESS;
}

/*!
 * @brief   Set the I2C baudrate
 * @details This function sets the baudrate (SCL frequency) for the I2C master.
 *          Note that due to module limitation not any baudrate can be achieved.
 *          The driver will set a baudrate as close as possible to the requested
 *          baudrate, but there may still be substantial differences, for example
 *          if requesting a high baudrate while using a low-frequency CFGIO clock.
 *          The application should call CFGIO_I2C_MasterGetBaudrate() after
 *          CFGIO_I2C_MasterSetBaudrate() to check what baudrate was actually set.
 *
 * @param states    Pointer to master context structure
 * @param baudrate  Desired baudrate in Hertz
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterSetBaudrate(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t baudrate)
{
    uint32_t inputClock;
    uint16_t timerCompare;
    uint16_t divider;
    uint8_t index = states->commonState.resIndex;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    if (states->isIdle)
    {
        /* Get protocol clock frequency */
        CLOCK_SYS_ReadFreq(g_cfgioClockSrc[states->commonState.instance],
                           &inputClock);

        /* Calculate divider */
        CFGIO_I2C_MasterCalcBaudrateDivider(baudrate, &divider, inputClock);

        /* Configure timer divider */
        timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));
        timerCompare = (uint16_t)((timerCompare & 0xFF00U) | divider);
        CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), timerCompare);

        return STATUS_SUCCESS;
    }
    return STATUS_BUSY;
}

/*!
 * @brief Set the I2C slave address
 *
 * @param states    Pointer to master context structure
 * @param address   Slave address 7-bit
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterSetSlaveAddr(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint16_t address)
{
    if (states->isIdle)
    {
        states->slaveAddr = address;
        return STATUS_SUCCESS;
    }
    return STATUS_BUSY;
}

/*!
 * @brief   Perform non-blocking send
 * @details This function starts the transmission of a block of data to the
 *          currently configured slave address and returns immediately. The
 *          rest of the transmission is handled by the interrupt service
 *          routine (if the driver is initialized in interrupt mode) or by
 *          the CFGIO_I2C_MasterGetStatus function (if the driver is
 *          initialized in polling mode). Use CFGIO_I2C_MasterGetStatus()
 *          to check the progress of the transmission.
 *
 * @param states    Pointer to master context structure
 * @param txBuf     Pointer to the data to be transferred
 * @param txLen     Length in bytes of the data to be transferred
 * @param sendStop  Whether or not generate stop condition after the transmission
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterWriteNonBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    const uint8_t *txBuf,
    uint32_t txLen,
    bool sendStop)
{
    if (states->isIdle)
    {
        states->txBuf = txBuf;
        return CFGIO_I2C_MasterStartTransfer(states, txLen, sendStop, CFGIO_I2C_TX);
    }
    return STATUS_BUSY;
}

/*!
 * @brief   Perform blocking send
 * @details This function sends a block of data to the currently configured
 *          slave address, and returns when the transmission is completed.
 *
 * @param states    Pointer to master context structure
 * @param txBuf     Pointer to the data to be transferred
 * @param txLen     Length in bytes of the data to be transferred
 * @param sendStop  Whether or not generate stop condition after the transmission
 * @param timeout   Timeout for the transfer in milliseconds
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterWriteBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    const uint8_t *txBuf,
    uint32_t txLen,
    bool sendStop,
    uint32_t timeout)
{
    STATUS_T status;

    if (!states->isIdle)
    {
        return STATUS_BUSY;
    }

    /* Mark the transfer as blocking */
    if (states->transferType != CFGIO_USE_POLLING)
    {
        states->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(states->idleSemaphore), 0);
    }

    /* Start the transfer */
    states->txBuf = txBuf;
    status = CFGIO_I2C_MasterStartTransfer(states, txLen, sendStop, CFGIO_I2C_TX);
    if (status != STATUS_SUCCESS)
    {
        states->isBlocking = false;
        return status;
    }

    /* Wait until the transfer is completed */
    return CFGIO_I2C_MasterWaitTransferEnd(states, timeout);
}

/*!
 * @brief   Perform non-blocking receive
 * @details This function starts the reception of a block of data from the
 *          currently configured slave address and returns immediately. The
 *          rest of the transmission is handled by the interrupt service
 *          routine (if the driver is initialized in interrupt mode) or by
 *          the CFGIO_I2C_MasterGetStatus function (if the driver is
 *          initialized in polling mode). Use CFGIO_I2C_MasterGetStatus()
 *          to check the progress of the reception.
 *
 * @param states    Pointer to master context structure
 * @param rxBuf     Pointer to the buffer where to store received data
 * @param rxLen     Length in bytes of the data to be transferred
 * @param sendStop  Whether or not to generate stop condition after the reception
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterReadNonBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint8_t *rxBuf,
    uint32_t rxLen,
    bool sendStop)
{
    if (states->isIdle)
    {
        states->rxBuf = rxBuf;
        return CFGIO_I2C_MasterStartTransfer(states, rxLen, sendStop, CFGIO_I2C_RX);
    }
    return STATUS_BUSY;
}

/*!
 * @brief   Perform blocking receive
 * @details This function receives a block of data from the currently configured
 *          slave address, and returns when the transmission is completed.
 *
 * @param states    Pointer to master context structure
 * @param rxBuf     Pointer to the buffer where to store received data
 * @param rxLen     Length in bytes of the data to be transferred
 * @param sendStop  Whether or not generate stop condition after the reception
 * @param timeout   Timeout for the transfer in milliseconds
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterReadBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint8_t *rxBuf,
    uint32_t rxLen,
    bool sendStop,
    uint32_t timeout)
{
    STATUS_T status;

    if (!states->isIdle)
    {
        return STATUS_BUSY;
    }

    /* Mark the transfer as blocking */
    if (states->transferType != CFGIO_USE_POLLING)
    {
        states->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(states->idleSemaphore), 0);
    }

    /* Start the transfer */
    states->rxBuf = rxBuf;
    status = CFGIO_I2C_MasterStartTransfer(states, rxLen, sendStop, CFGIO_I2C_RX);
    if (status != STATUS_SUCCESS)
    {
        states->isBlocking = false;
        return status;
    }

    /* Wait until the transfer is completed */
    return CFGIO_I2C_MasterWaitTransferEnd(states, timeout);
}

/*!
 * @brief   Abort the last non-blocking I2C master transaction
 * @details Warning: An ongoing transfer can't be aborted safely due to device
 *          limitation, there is no way to know the exact stage of the transfer,
 *          and if we disable the module during the ACK bit (transmit) or during
 *          a 0 data bit (receive) the slave will hold the SDA line low forever
 *          and block the I2C bus. NACK reception is the only exception, as
 *          there is no slave to hold the line low. Therefore this function
 *          should only be used in extreme conditions, and the application must
 *          have a way to reset the I2C slave.
 * @param states    Pointer to master context structure
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterAbortTransfer(CFGIO_I2C_MASTER_STATE_T *states)
{
    if (states->isIdle)
    {
        return STATUS_SUCCESS;
    }

    states->driverStatus = STATUS_I2C_ABORTED;
    CFGIO_I2C_MasterStopTransfer(states);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Get the status of the current non-blocking I2C master transaction
 * @details This function returns the current status of a non-blocking I2C
 *          master transaction. A return code of STATUS_BUSY means the transfer
 *          is still in progress. Otherwise the function returns a status
 *          reflecting the outcome of the last transfer. When the driver is
 *          initialized in polling mode this function also advances the transfer
 *          by checking and handling the transmit and receive events, so it must
 *          be called frequently to avoid overflows or underflows.
 *
 * @param states     Pointer to master context structure
 * @param bytesLeft  The remaining number of bytes to be transferred
 *
 * @retval  Error status
 */
STATUS_T CFGIO_I2C_MasterGetStatus(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t *bytesLeft)
{
    /**
     * Use rxBytesLeft even for transmit. byte is not transmitted
     * until Rx shifter confirms the ACK.
     */
    uint32_t remainingBytes = states->rxBytesLeft;

    if (!states->isIdle)
    {
        if (states->transferType == CFGIO_USE_DMA)
        {
            /**
             * In DMA mode just update the remaining count.
             * DO NOT write states->rxBytesLeft directly!
             */
            remainingBytes = DMA_ReadRemainingMajorIterationsCount(states->rxDmaChannel);
        }
        else if (states->transferType == CFGIO_USE_POLLING)
        {
            /* Advance the I2C transfer in polling mode */
            CFGIO_I2C_MasterCheckStatus(states);
        }
        else
        {
        }
    }

    if (bytesLeft != NULL)
    {
        *bytesLeft = remainingBytes;
    }
    return states->isIdle ? states->driverStatus : STATUS_BUSY;
}

/*!
 *
 * @brief Get the status whether SDA or SCL line is low or high
 *
 * @param states    Pointer to master context structure
 * @param line      Get status of the selected line
 *
 * @retval true:  Pin selected is high
 *         false: Pin selected is low
 */
bool CFGIO_I2C_GetBusStatus(
    const CFGIO_I2C_MASTER_STATE_T *states,
    CFGIO_I2C_BUS_LINE_T line)
{
    uint8_t mask;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    if (line == CFGIO_I2C_SDA_LINE)
    {
        mask = (uint8_t)((1U << states->sdaPin));
    }
    else
    {
        mask = (uint8_t)((1U << states->sclPin));
    }

    /* Check pin mask */
    if ((CFGIO_HW_GetPinData(base) & mask) == mask)
    {
        /* SDA/SCL line is high */
        return true;
    }
    else
    {
        /* SDA/SCL line is low */
        return false;
    }
}

/*!
 * @brief   Generate nine clock on SCL line to free SDA line
 * @details This function should be called when SDA line be stuck in low.
 *
 * @param states    Pointer to master context structure
 *
 * @retval STATUS_BUSY:     Driver is transferring data
 *         STATUS_SUCCESS:  Function started generating clock
 */
STATUS_T CFGIO_I2C_GenerateNineClock(CFGIO_I2C_MASTER_STATE_T *states)
{
    uint16_t timerCompare;
    uint8_t index = states->commonState.resIndex;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    if (states->isIdle)
    {
        /* Set the number of ticks in high part of timer compare register */
        timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));

        /* Set the ticks in high part of timer compare register to generated nine clock*/
        timerCompare = (uint16_t)((timerCompare & 0x00FFU) | (uint16_t)(((16U) & 0xFFU) << 8U));
        CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), timerCompare);

        /* Disable the timer on timer compare */
        CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_TIM_CMP);
        CFGIO_HW_SetTimerStop(base, TIMER_INDEX_SCL(index), CFGIO_TMR_STOP_BIT_DISABLED);

        /* Enable timers and shifters */
        CFGIO_HW_SetShifterMode(base, SHIFTER_INDEX_TX(index), CFGIO_SHIFT_MODE_TRANSMIT);
        CFGIO_HW_SetTimerMode(base, TIMER_INDEX_SCL(index), CFGIO_TMR_MODE_8BIT_BAUD);

        /* Enable the interrupt SLC timer */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << TIMER_INDEX_SCL(index)), true);

        /* Write any value to trigger the timer */
        CFGIO_HW_WriteShifterBuffer(base,
                                      SHIFTER_INDEX_TX(index),
                                      (uint32_t)0x00U,
                                      CFGIO_SHIFT_RW_MODE_BIT_SWAP);
        return STATUS_SUCCESS;
    }
    return STATUS_BUSY;
}

/*!
 *
 * @brief Indicate the generation nine clock is done or not
 *
 * @param states    Pointer to master context structure
 *
 * @retval STATUS_BUSY:     Clock generation not done yet
 *         STATUS_SUCCESS:  Device finished generating nine clock
 */
STATUS_T CFGIO_I2C_StatusGenerateNineClock(CFGIO_I2C_MASTER_STATE_T *states)
{
    uint32_t timerMask;
    uint32_t timerState;
    uint8_t index = states->commonState.resIndex;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    /* Check enable SCL timer */
    timerMask = (1UL << (uint32_t)TIMER_INDEX_SCL(index));
    timerState = CFGIO_HW_GetTimerInterruptStates(base);

    if ((timerState & timerMask) != 0U)
    {
        /* Generation of nine clock have not done yet */
        return STATUS_BUSY;
    }
    else
    {
        /* Generation of nine clock was done */
        return STATUS_SUCCESS;
    }
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configure CFGIO module as I2C master
 *
 * @param states        Pointer to master context structure
 * @param clockInput    Input clock for I2C
 * @param baudrate      Baudrate of I2C
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterCFGIOConfig(
    const CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t clockInput,
    uint32_t baudrate)
{
    uint16_t divider;
    uint8_t index = states->commonState.resIndex;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    /* Calculate the divider */
    CFGIO_I2C_MasterCalcBaudrateDivider(baudrate, &divider, clockInput);

    /* Configure the Tx shifter */
    CFGIO_HW_SetShifterConfig(
        base,
        SHIFTER_INDEX_TX(index),
        CFGIO_SHIFT_START_BIT_0,
        CFGIO_SHIFT_STOP_BIT_1,
        CFGIO_SHIFT_SRC_PIN);

    /**
     * Shifter disabled and pin enabled causes the pin to be held low.
     * So disable pin too, will be enabled at transfer time.
     */
    CFGIO_HW_SetShifterControl(
        base,
        SHIFTER_INDEX_TX(index),
        CFGIO_SHIFT_MODE_DISABLED,
        states->sdaPin,        /* Output on SDA pin */
        CFGIO_PIN_POLARITY_LOW,
        CFGIO_PIN_CFG_DISABLED,
        TIMER_INDEX_CTRL(index),  /* Use control timer to drive the shifter */
        CFGIO_TMR_POLARITY_POS_EDGE);

    /* Configure the Rx shifter */
    CFGIO_HW_SetShifterConfig(
        base,
        SHIFTER_INDEX_RX(index),
        CFGIO_SHIFT_START_BIT_DISABLED,
        CFGIO_SHIFT_STOP_BIT_0,
        CFGIO_SHIFT_SRC_PIN);
    CFGIO_HW_SetShifterControl(
        base,
        SHIFTER_INDEX_RX(index),
        CFGIO_SHIFT_MODE_DISABLED,
        states->sdaPin,        /* Input from SDA pin */
        CFGIO_PIN_POLARITY_HIGH,
        CFGIO_PIN_CFG_DISABLED,
        TIMER_INDEX_CTRL(index),  /* Use control timer to drive the shifter */
        CFGIO_TMR_POLARITY_NEG_EDGE);

    /* Configure the SCL timer */
    CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), divider);
    CFGIO_HW_SetTimerConfig(
        base,
        TIMER_INDEX_SCL(index),
        CFGIO_TMR_START_BIT_ENABLED,
        CFGIO_TMR_STOP_BIT_TIM_DIS,
        CFGIO_TMR_ENABLE_TRG_HIGH,    /* Enable when Tx data is available */
        CFGIO_TMR_DISABLE_TIM_CMP,
        CFGIO_TMR_RESET_PIN_OUT,      /* Reset if output equals pin (for clock stretching) */
        CFGIO_TMR_DEC_CLK_SHIFT_TMR,  /* Decrement on CFGIO clock */
        CFGIO_TMR_OUTPUT_ZERO);
    CFGIO_HW_SetTimerControl(
        base,
        TIMER_INDEX_SCL(index),
        (uint8_t)((uint8_t)(SHIFTER_INDEX_TX(index) << 2U) + 1U), /* Trigger on Tx shifter status flag */
        CFGIO_TRG_POLARITY_LOW,
        CFGIO_TRG_SRC_INTERNAL,
        states->sclPin,             /* Output on SCL pin */
        CFGIO_PIN_POLARITY_HIGH,
        CFGIO_PIN_CFG_OPEN_DRAIN, /* Enable output */
        CFGIO_TMR_MODE_DISABLED);

    /* Configure the control timer for shifters */
    CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_CTRL(index), 0x000FU);
    CFGIO_HW_SetTimerConfig(
        base,
        TIMER_INDEX_CTRL(index),
        CFGIO_TMR_START_BIT_ENABLED,
        CFGIO_TMR_STOP_BIT_TIM_CMP,
        CFGIO_TMR_ENABLE_TIM_ENABLE,      /* Enable on SCL timer enable */
        CFGIO_TMR_DISABLE_TIM_DISABLE,    /* Disable on SCL timer disable */
        CFGIO_TMR_RESET_NEVER,
        CFGIO_TMR_DEC_PIN_SHIFT_PIN,      /* Decrement on SCL pin input */
        CFGIO_TMR_OUTPUT_ONE);
    CFGIO_HW_SetTimerControl(
        base,
         TIMER_INDEX_CTRL(index),
         (uint8_t)((uint8_t)(SHIFTER_INDEX_TX(index) << 2U) + 1U), /* Trigger on Tx shifter status flag */
         CFGIO_TRG_POLARITY_LOW,
         CFGIO_TRG_SRC_INTERNAL,
         states->sclPin,      /* Use SCL pin as input */
         CFGIO_PIN_POLARITY_LOW,
         CFGIO_PIN_CFG_DISABLED,
         CFGIO_TMR_MODE_DISABLED);
}

/*!
 * @brief Set the number of SCL clocks needed for the entire transmission
 *
 * @param base      Base address of CFGIO instance
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterSetNumberClocks(
    CFGIO_T *base,
    CFGIO_I2C_MASTER_STATE_T *states)
{
    uint32_t edgeNumber;    /* Total number of clock edges */
    uint32_t reloadNumber;  /* Number of timer reloads */
    uint32_t edgeCounter;   /* Edge counter per timer reload */
    uint16_t timerCompare;
    uint8_t index = states->commonState.resIndex;

    /* Calculate number of SCL edges including address */
    edgeNumber   = (states->txBytesLeft * 18U) + 2U;
    reloadNumber = (uint32_t)((edgeNumber + 255U) / 256U);
    edgeCounter  = (uint32_t)((uint32_t)(edgeNumber + (reloadNumber - 1))
                 / (uint32_t)reloadNumber);

    /* Set the number of ticks in high part of timer compare register */
    timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));
    timerCompare = (uint16_t)((timerCompare & 0x00FFU)
                 | (uint16_t)(((edgeCounter - 1U) & 0xFFU) << 8U));

    CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), timerCompare);

    /* Store the reload information */
    states->eventCount =(uint16_t)reloadNumber;
    states->reloadCounter = (uint8_t)(edgeNumber
                          - ((reloadNumber - 1U) * edgeCounter) - 1U);

    /* Handle no reload case */
    if (reloadNumber == 1U)
    {
        CFGIO_HW_SetTimerDisable(base,
                                   TIMER_INDEX_SCL(index),
                                   CFGIO_TMR_DISABLE_TIM_CMP);
    }
}

/*!
 * @brief Calculate the baudrate divider for target baudrate
 *
 * @param baudrate      Baudrate of I2C
 * @param divider       Clock divider
 * @param clockInput    Input clock for I2C
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterCalcBaudrateDivider(
    uint32_t baudrate,
    uint16_t *divider,
    uint32_t clockInput)
{
    int32_t tempDivider;

    /**
     * Divider = ((InputClock / baudrate) / 2) - 1 - 1
     * The extra -1 is from the timer reset setting used for clock stretching.
     * Round to nearest integer.
     */
    tempDivider = (((int32_t)clockInput + (int32_t)baudrate) / (2 * (int32_t)baudrate)) - 2;

    /* Check upper/lower limits */
    if (tempDivider > MAX_DIVIDER_VALUE)
    {
        tempDivider = MAX_DIVIDER_VALUE;
    }
    if (tempDivider < MIN_DIVIDER_VALUE)
    {
        tempDivider = MIN_DIVIDER_VALUE;
    }

    *divider = (uint16_t)tempDivider;
}

/*!
 * @brief Calculate the address of the register used for DMA Rx transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static uint32_t CFGIO_I2C_MasterCalcRxRegAddr(const CFGIO_I2C_MASTER_STATE_T *states)
{
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t shifter = SHIFTER_INDEX_RX(states->commonState.resIndex);
    uint32_t regAddr = (uint32_t)(&(base->SBUFBITSWA[shifter]));
    return regAddr;
}

/*!
 * @brief Calculate the address of the register used for DMA Tx transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static  uint32_t CFGIO_I2C_MasterCalcTxRegAddr(const CFGIO_I2C_MASTER_STATE_T *states)
{
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t shifter = SHIFTER_INDEX_TX(states->commonState.resIndex);
    uint32_t regAddr = (uint32_t)(&(base->SBUFBITSWA[shifter])) + (sizeof(uint32_t) - 1U);
    return regAddr;
}

/*!
 * @brief Send slave address byte
 *
 * @param base      Base address of CFGIO instance
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterSendSlaveAddr(
    CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states)
{
    uint8_t index = states->commonState.resIndex;
    uint8_t addrByte;

    /* Address byte: slave 7-bit address + D = 0 (transmit) or 1 (receive) */
    addrByte = (uint8_t)((uint8_t)(states->slaveAddr << 1U)
             + (uint8_t)((states->transferDir == CFGIO_I2C_RX) ? 1U : 0U));

    CFGIO_HW_WriteShifterBuffer(base,
                                  SHIFTER_INDEX_TX(index),
                                  (uint32_t)addrByte << 24U,
                                  CFGIO_SHIFT_RW_MODE_BIT_SWAP);
}

/*!
 * @brief Handle the data transmission
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterWriteData(CFGIO_I2C_MASTER_STATE_T *states)
{
    uint32_t data;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    if (states->txBytesLeft > 0U)
    {
        states->txBytesLeft--;
        if (states->txBytesLeft == 0U)
        {
            /* Transmission is done, generate stop condition or not */
            data = states->sendStop ? 0x00U : 0xFFU;
        }
        else if (states->transferDir == CFGIO_I2C_RX)
        {
            /* Transmit 0xFF to leave the line free while receiving */
            data = 0xFFU;
        }
        else
        {
            /* Read data from user buffer */
            data =  *(states->txBuf);
            states->txBuf++;
        }

        /**
         * Shift data before bit swapping it, to get the relevant bits in the lower
         * part of the shifter.
         */
        data <<= 24U;
        CFGIO_HW_WriteShifterBuffer(base,
                                      SHIFTER_INDEX_TX(index),
                                      data,
                                      CFGIO_SHIFT_RW_MODE_BIT_SWAP);
    }
}

/*!
 * @brief Handle the data reception
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterReadData(CFGIO_I2C_MASTER_STATE_T *states)
{
    uint8_t data;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Read data from Rx shifter */
    data = (uint8_t)CFGIO_HW_ReadShifterBuffer(base,
                                                 SHIFTER_INDEX_RX(index),
                                                 CFGIO_SHIFT_RW_MODE_BIT_SWAP);
    if (states->addrReceived)
    {
        states->rxBytesLeft--;
        if (states->transferDir == CFGIO_I2C_RX)
        {
            /* Put data in user buffer */
            *(states->rxBuf) = data;
            states->rxBuf++;
        }
    }
    else
    {
        /* This is the address byte */
        states->addrReceived = true;
        if (states->transferDir == CFGIO_I2C_RX)
        {
            /* Send ACK from now on */
            CFGIO_HW_SetShifterStopBit(base,
                                         SHIFTER_INDEX_TX(index),
                                         CFGIO_SHIFT_STOP_BIT_0);
        }
    }

    if (   (states->transferDir == CFGIO_I2C_RX)
        && (states->rxBytesLeft == 1U))
    {
        /* Send NACK for the last byte */
        CFGIO_HW_SetShifterStopBit(base,
                                     SHIFTER_INDEX_TX(index),
                                     CFGIO_SHIFT_STOP_BIT_1);

        /* Instruct Rx shifter to expect NACK */
        CFGIO_HW_SetShifterStopBit(base,
                                     SHIFTER_INDEX_RX(index),
                                     CFGIO_SHIFT_STOP_BIT_1);
    }
}

/*!
 * @brief Enable timers and shifters to start a transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterEnableTimerShifter(CFGIO_I2C_MASTER_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Enable timers and shifters */
    CFGIO_HW_SetShifterMode(base, SHIFTER_INDEX_TX(index), CFGIO_SHIFT_MODE_TRANSMIT);
    CFGIO_HW_SetShifterMode(base, SHIFTER_INDEX_RX(index), CFGIO_SHIFT_MODE_RECEIVE);
    CFGIO_HW_SetTimerMode(base, TIMER_INDEX_SCL(index), CFGIO_TMR_MODE_8BIT_BAUD);
    CFGIO_HW_SetTimerMode(base, TIMER_INDEX_CTRL(index), CFGIO_TMR_MODE_16BIT);

    /* Enable Tx pin */
    CFGIO_HW_SetShifterPinConfig(base,
                                   SHIFTER_INDEX_TX(index),
                                   CFGIO_PIN_CFG_OPEN_DRAIN);
}

/*!
 * @brief Perform a send or receive transaction on the I2C bus
 *
 * @param states    Pointer to master context structure
 * @param size      Number of bytes
 * @param sendStop  Send stop or not
 * @param receive   Receive or not
 *
 * @retval STATUS_BUSY:     Driver is busy transferring data
 *         STATUS_SUCCESS:  Function started generating clock
 */
static STATUS_T CFGIO_I2C_MasterStartTransfer(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t size,
    bool sendStop,
    CFGIO_I2C_DIR_T dir)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    if (CFGIO_I2C_MasterBusIsBusy(base, states))
    {
        return STATUS_I2C_BUS_BUSY;
    }

    /* One extra byte for stop condition */
    states->driverStatus = STATUS_BUSY;
    states->isIdle = false;
    states->addrReceived = false;
    states->transferDir = dir;
    states->txBytesLeft = size + 1U;
    states->rxBytesLeft = size;
    states->sendStop = sendStop;

    /* Configure the device for requested number of bytes, keep the existing baudrate */
    CFGIO_I2C_MasterSetNumberClocks(base, states);
    CFGIO_I2C_MasterEnableTimerShifter(states);

    /* Enable the transfer engine */
    switch (states->transferType)
    {
    case CFGIO_USE_DMA:
        CFGIO_I2C_MasterDmaTransferStart(states);
        /* Enable error interrupt for Rx shifter, for NACK detection */
        CFGIO_HW_SetShifterErrorInterrupt(base,
                                            (uint8_t)(1U << SHIFTER_INDEX_RX(index)),
                                            true);
        /* Enable interrupt for SCL timer, for end of transfer detection */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << TIMER_INDEX_SCL(index)), true);
        /* Disable system interrupt */
        INT_SYS_DisableIRQGlobal();
        /* Send address to start transfer */
        CFGIO_I2C_MasterSendSlaveAddr(base, states);
        /* Enable CFGIO DMA requests for both shifters */
        CFGIO_HW_SetShifterDmaRequest(base,
                                        (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                  (1U << SHIFTER_INDEX_RX(index))),
                                        true);
        /*Enable system interrupt*/
        INT_SYS_EnableIRQGlobal();
        break;
    case CFGIO_USE_INTERRUPTS:
        /* Send address to start transfer */
        CFGIO_I2C_MasterSendSlaveAddr(base, states);
        /* Enable interrupt for Tx and Rx shifters */
        CFGIO_HW_SetShifterInterrupt(base,
                                       (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                 (1U << SHIFTER_INDEX_RX(index))),
                                       true);
        CFGIO_HW_SetShifterErrorInterrupt(base,
                                            (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                      (1U << SHIFTER_INDEX_RX(index))),
                                            true);
        /* Enable interrupt for SCL timer */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << TIMER_INDEX_SCL(index)), true);
        break;
    case CFGIO_USE_POLLING:
        /* Send address to start transfer */
        CFGIO_I2C_MasterSendSlaveAddr(base, states);
        /* Nothing to do here, CFGIO_I2C_MasterGetStatus() will handle the transfer */
        break;
    default:
        /* Nothing to do */
        break;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Force stop the current transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterStopTransfer(CFGIO_I2C_MASTER_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Disable Tx pin */
    CFGIO_HW_SetShifterPinConfig(base, SHIFTER_INDEX_TX(index), CFGIO_PIN_CFG_DISABLED);

    /* Disable and reenable timers and shifters to reset them */
    CFGIO_HW_SetShifterMode(base, SHIFTER_INDEX_TX(index), CFGIO_SHIFT_MODE_DISABLED);
    CFGIO_HW_SetShifterMode(base, SHIFTER_INDEX_RX(index), CFGIO_SHIFT_MODE_DISABLED);
    CFGIO_HW_SetTimerMode(base, TIMER_INDEX_SCL(index), CFGIO_TMR_MODE_DISABLED);
    CFGIO_HW_SetTimerMode(base, TIMER_INDEX_CTRL(index), CFGIO_TMR_MODE_DISABLED);

    /* Clear any leftover error flags */
    CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_TX(index));
    CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_RX(index));

    /* Discard any leftover Rx data */
    CFGIO_HW_ClearShifterStatus(base, SHIFTER_INDEX_RX(index));

    /* Clear timer status */
    CFGIO_HW_ClearTimerStatus(base, TIMER_INDEX_SCL(index));

    /* End the transfer */
    CFGIO_I2C_MasterEndTransfer(states);

    /* Check Rx overflow */
    if (   (states->rxBytesLeft != 0U)
        && (states->driverStatus == STATUS_SUCCESS)
        && (states->transferDir == CFGIO_I2C_RX)
        && (states->transferType != CFGIO_USE_DMA))
    {
        states->driverStatus = STATUS_I2C_RX_OVERRUN_ERROR;
    }
}

/*!
 * @brief End the current transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterEndTransfer(CFGIO_I2C_MASTER_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Restore Rx stop bit in case it was changed by a receive */
    CFGIO_HW_SetShifterStopBit(base, SHIFTER_INDEX_RX(index), CFGIO_SHIFT_STOP_BIT_0);

    /* Restore Tx stop bit in case it was changed by a receive */
    CFGIO_HW_SetShifterStopBit(base, SHIFTER_INDEX_TX(index), CFGIO_SHIFT_STOP_BIT_1);

    /* Clear Rx status in case there is a character left in the buffer */
    CFGIO_HW_ClearShifterStatus(base, SHIFTER_INDEX_RX(index));

    /* Disable the transfer engine */
    switch (states->transferType)
    {
    case CFGIO_USE_DMA:
        /* Disable error interrupt for Rx shifter */
        CFGIO_HW_SetShifterErrorInterrupt(base,
                                            (uint8_t)(1U << SHIFTER_INDEX_RX(index)),
                                            false);
        /* Disable interrupt for SCL timer */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << TIMER_INDEX_SCL(index)), false);
        /* Stop DMA channels */
        (void)DMA_StopChannel(states->txDmaChannel);
        (void)DMA_StopChannel(states->rxDmaChannel);
        /* Disable CFGIO DMA requests for both shifters */
        CFGIO_HW_SetShifterDmaRequest(base,
                                        (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                  (1U << SHIFTER_INDEX_RX(index))),
                                        false);
        break;
    case CFGIO_USE_INTERRUPTS:
        /* Disable interrupts for Rx and Tx shifters */
        CFGIO_HW_SetShifterInterrupt(base,
                                       (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                 (1U << SHIFTER_INDEX_RX(index))),
                                       false);
        CFGIO_HW_SetShifterErrorInterrupt(base,
                                            (uint8_t)((1U << SHIFTER_INDEX_TX(index)) |
                                                      (1U << SHIFTER_INDEX_RX(index))),
                                            false);
        /* Disable interrupt for SCL timer */
        CFGIO_HW_SetTimerInterrupts(base, (uint8_t)(1U << TIMER_INDEX_SCL(index)), false);
        break;
    case CFGIO_USE_POLLING:
        /* Nothing to do here */
        break;
    default:
        /* Nothing to do */
        break;
    }

    states->isIdle = true;

    /* Signal transfer end for blocking transfers */
    if (states->isBlocking)
    {
        (void)OSIF_SemPost(&(states->idleSemaphore));
    }
}

/*!
 * @brief Wait for the end of a blocking transfer
 *
 * @param states    Pointer to master context structure
 * @param timeout   Timeout in milliseconds
 *
 * @retval   Error status
 */
static STATUS_T CFGIO_I2C_MasterWaitTransferEnd(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t timeout)
{
    STATUS_T osResult = STATUS_SUCCESS;

    switch (states->transferType)
    {
    case CFGIO_USE_DMA:
        /* Wait for transfer to be completed by DMA IRQ */
        osResult = OSIF_SemWait(&(states->idleSemaphore), timeout);
        break;
    case CFGIO_USE_INTERRUPTS:
        /* Wait for transfer to be completed by IRQ */
        osResult = OSIF_SemWait(&(states->idleSemaphore), timeout);
        break;
    case CFGIO_USE_POLLING:
        /* Call CFGIO_I2C_MasterGetStatus() to do the transfer */
        while (CFGIO_I2C_MasterGetStatus(states, NULL) == STATUS_BUSY);
        break;
    default:
        /* Nothing to do */
        break;
    }

    /* Blocking transfer is done */
    states->isBlocking = false;

    if (osResult == STATUS_TIMEOUT)
    {
        /* Abort the current transfer */
        states->driverStatus = STATUS_TIMEOUT;
        CFGIO_I2C_MasterStopTransfer(states);
    }
    return states->driverStatus;
}

/*!
 * @brief   Check the status of I2C transfer
 * @details This function can be called either in an interrupt routine or
 *          directly in polling mode to advance the I2C transfer.
 *
 * @param statePtr  Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterCheckStatus(void *statePtr)
{
    uint16_t timerCompare;
    CFGIO_I2C_MASTER_STATE_T *states = (CFGIO_I2C_MASTER_STATE_T *)statePtr;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Check for errors */
    if (CFGIO_HW_GetShifterErrorStatus(base, SHIFTER_INDEX_TX(index)))
    {
        states->driverStatus = STATUS_I2C_TX_UNDERRUN_ERROR;
        CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_TX(index));
        /* Don't stop the transfer, continue processing events */
    }

    if (CFGIO_HW_GetShifterErrorStatus(base, SHIFTER_INDEX_RX(index)))
    {
        /**
         * Device limitation: not possible to tell the difference between NACK
         * and receive overflow.
         */
        if (CFGIO_I2C_MasterCheckNack(base, states))
        {
            states->driverStatus = STATUS_I2C_NACK_ERROR;

            /* Force stop the transfer */
            CFGIO_I2C_MasterStopTransfer(states);

            /* Notify the event to the user */
            if (states->callback != NULL)
            {
                states->callback(I2C_MASTER_EVENT_TRANSFER_COMPLETE, states->callbackParam);
            }
            return;
        }
        else
        {
            states->driverStatus = STATUS_I2C_RX_OVERRUN_ERROR;
            CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_RX(index));
            /* Don't stop the transfer, continue processing events */
        }
    }

    /* Check if data was received */
    if (CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_RX(index)))
    {
        CFGIO_I2C_MasterReadData(states);
    }

    /* Check if transmitter needs more data */
    if (CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_TX(index)))
    {
        CFGIO_I2C_MasterWriteData(states);
        if (states->txBytesLeft == 0U)
        {
            /* Done transmitting, disable Tx interrupt */
            CFGIO_HW_SetShifterInterrupt(base, (uint8_t)(1U << SHIFTER_INDEX_TX(index)), false);
        }
    }

    /* Check if the transfer is done */
    if (CFGIO_HW_GetTimerStatus(base, TIMER_INDEX_SCL(index)))
    {
        /* Reset when when generate nine clock */
        if (states->isIdle)
        {
            /* Clear timer status */
            CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_NEVER);
            CFGIO_I2C_MasterStopTransfer(states);
        }
        else /* General interrupt behavior */
        {
            states->eventCount--;

            /* Clear timer status */
            CFGIO_HW_ClearTimerStatus(base, TIMER_INDEX_SCL(index));

            if (states->eventCount == 2U)
            {
                /* Adjust number of ticks in high part of timer compare register for the last reload */
                timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));
                timerCompare = (uint16_t)((uint16_t)((uint32_t)timerCompare & 0x00FFU) |
                                          (uint16_t)(((uint32_t)(states->reloadCounter) & 0xFFU) << 8U));
                CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), timerCompare);
            }

            if (states->eventCount == 1U)
            {
                /* Timer will disable on the next countdown complete */
                CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_TIM_CMP);
            }

            if (states->eventCount == 0U)
            {
                CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_NEVER);

                /* Record success if there was no error */
                if (states->driverStatus == STATUS_BUSY)
                {
                    states->driverStatus = STATUS_SUCCESS;
                }
                /**
                 * End transfer. In case of race condition between Tx shifter
                 * and timer end events, it is possible for the clock to be
                 * restarted. So we use forced stop to avoid this.
                 */
                CFGIO_I2C_MasterStopTransfer(states);

                /* Notify the event to the user */
                if (states->callback != NULL)
                {
                    states->callback(I2C_MASTER_EVENT_TRANSFER_COMPLETE, states->callbackParam);
                }
            }
        }
    }
}

/*!
 * @brief   Check the busy state of I2C bus
 * @details The bus is busy if either SDA or SCL is low.
 *
 * @param base      Base address of CFGIO instance
 * @param states    Pointer to master context structure
 *
 * @retval  true:   Bus is busy
 *          false:  Bus is not busy
 */
static bool CFGIO_I2C_MasterBusIsBusy(
    const CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states)
{
    uint8_t mask = (uint8_t)((1U << states->sdaPin) | (1U << states->sclPin));

    if ((CFGIO_HW_GetPinData(base) & mask) == mask)
    {
        /* Both pins are high, bus is not busy */
        return false;
    }
    else
    {
        /* bus is busy */
        return true;
    }
}

/*!
 * @brief   Check if the current Rx shifter error is NACK or Rx overflow
 * @details If there is a Tx event active it is an indication that module was
 *          not serviced for a long time, chances are this is an overflow.
 *          It is not certain, and it is even possible to have both NACK and
 *          overflow, but there is no way to tell, so we chose the safe option
 *          (if it is an overflow and we abort the transfer we may block the
 *          I2C bus).
 *
 * @param base      Base address of CFGIO instance
 * @param states    Pointer to master context structure
 *
 * @retval    NACK or not
 */
static bool CFGIO_I2C_MasterCheckNack(
    const CFGIO_T *base,
    const CFGIO_I2C_MASTER_STATE_T *states)
{
    uint8_t index = states->commonState.resIndex;
    return !(CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_TX(index)));
}

/*!
 * @brief Configure DMA for Rx transfer
 *
 * @param states    Pointer to master context structure
 * @param stcdBase  Base to the DMA TCD
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterDmaRxConfig(
    CFGIO_I2C_MASTER_STATE_T *states,
    DMA_SOFTWARE_TCD_T *stcdBase)
{
    DMA_SCATTER_GATHER_LIST_T dmaListDest[6U];
    DMA_SCATTER_GATHER_LIST_T dmaListSrc[6U];
    uint8_t blockCount = 0U;
    uint8_t shifter;
    uint32_t tempValue;
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t dmaChannel = states->rxDmaChannel;

    if (states->transferDir == CFGIO_I2C_TX)
    {
        /**
         * When transmitting we don't need scatter-gather for receive, just
         * move read data to dummy location.
         */
        (void)DMA_ConfigMultiBlockTransfer(dmaChannel,
                                           DMA_TRANSFER_PERIPH2MEM,
                                           CFGIO_I2C_MasterCalcRxRegAddr(states),
                                           (uint32_t)(&(states->dummyDmaReceive)),
                                           DMA_TRANSFER_SIZE_1B,
                                           1U,
                                           states->rxBytesLeft + 1U,
                                           true);
        /* No data to receive, don't increment destination offset. */
        DMA_ConfigDestOffset(dmaChannel, 0);
    }
    else
    {
        /* First block: receive address byte (dummy read) */
        dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListDest[blockCount].address = (uint32_t)(&(states->dummyDmaReceive));
        dmaListDest[blockCount].length = 1U;
        dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListSrc[blockCount].address = CFGIO_I2C_MasterCalcRxRegAddr(states);
        dmaListSrc[blockCount].length = 1U;
        blockCount++;

        /* When receiving just 1 byte, skip the middle part */
        if (states->rxBytesLeft > 1U)
        {
            /* 2nd block: set tx shifter stop bit to 0 (transmit ACK) */
            shifter = SHIFTER_INDEX_TX(states->commonState.resIndex);
            tempValue = base->SCFG[shifter].reg;
            tempValue &= ~(CFGIO_SCFG_SSTOPFSEL_MASK);
            tempValue |= CFGIO_SCFG_SSTOPFSEL(CFGIO_SHIFT_STOP_BIT_0);
            states->dmaTxStop0 = (uint8_t)(tempValue & 0xFFU);

            dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListDest[blockCount].address = (uint32_t)(&(base->SCFG[shifter]));
            dmaListDest[blockCount].length = 1U;
            dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListSrc[blockCount].address = (uint32_t)(&(states->dmaTxStop0));
            dmaListSrc[blockCount].length = 1U;
            blockCount++;

            /* 3rd block: receive all but the last data byte */
            dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListDest[blockCount].address = (uint32_t)(states->rxBuf);
            dmaListDest[blockCount].length = states->rxBytesLeft - 1U;
            dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListSrc[blockCount].address = CFGIO_I2C_MasterCalcRxRegAddr(states);
            dmaListSrc[blockCount].length = states->rxBytesLeft - 1U;
            blockCount++;

            /* 4th block: set tx shifter stop bit to 1 (transmit NACK for last byte) */
            tempValue = base->SCFG[shifter].reg;
            tempValue &= ~(CFGIO_SCFG_SSTOPFSEL_MASK);
            tempValue |= CFGIO_SCFG_SSTOPFSEL(CFGIO_SHIFT_STOP_BIT_1);
            states->dmaTxStop1 = (uint8_t)(tempValue & 0xFFU);

            dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListDest[blockCount].address = (uint32_t)(&(base->SCFG[shifter]));
            dmaListDest[blockCount].length = 1U;
            dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
            dmaListSrc[blockCount].address = (uint32_t)(&(states->dmaTxStop1));
            dmaListSrc[blockCount].length = 1U;
            blockCount++;
        }

        /* 5th block: set rx shifter stop bit to 1 (expect NACK) */
        shifter = SHIFTER_INDEX_RX(states->commonState.resIndex);
        tempValue = base->SCFG[shifter].reg;
        tempValue &= ~(CFGIO_SCFG_SSTOPFSEL_MASK);
        tempValue |= CFGIO_SCFG_SSTOPFSEL(CFGIO_SHIFT_STOP_BIT_1);
        states->dmaRxStop1 = (uint8_t)(tempValue & 0xFFU);

        dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListDest[blockCount].address = (uint32_t)(&(base->SCFG[shifter]));
        dmaListDest[blockCount].length = 1U;
        dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListSrc[blockCount].address = (uint32_t)(&(states->dmaRxStop1));
        dmaListSrc[blockCount].length = 1U;
        blockCount++;

        /* 6th block: receive last byte */
        dmaListDest[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListDest[blockCount].address = (uint32_t)(&(states->rxBuf[states->rxBytesLeft - 1U]));
        dmaListDest[blockCount].length = 1U;
        dmaListSrc[blockCount].type = DMA_TRANSFER_PERIPH2MEM;
        dmaListSrc[blockCount].address = CFGIO_I2C_MasterCalcRxRegAddr(states);
        dmaListSrc[blockCount].length = 1U;
        blockCount++;

        /**
         * Use blockCount (3 or 6) STCDs for Rx.
         * Transfer size: 1 byte, 1 byte per DMA request.
         */
        (void)DMA_ConfigScatterGatherTransfer(dmaChannel,
                                              stcdBase,
                                              DMA_TRANSFER_SIZE_1B,
                                              1U,
                                              dmaListSrc,
                                              dmaListDest,
                                              blockCount);

        /* Set all config transfers to trigger immediately */
        CFGIO_I2C_MasterTriggerDmaBlock(stcdBase, 0U);

        if (states->rxBytesLeft > 1U)
        {
            CFGIO_I2C_MasterTriggerDmaBlock(stcdBase, 2U);
            CFGIO_I2C_MasterTriggerDmaBlock(stcdBase, 3U);
        }

        /* Set for last block */
        CFGIO_I2C_MasterTerminateDmaBlock(stcdBase, (uint8_t)(blockCount - 2U));
    }
}

/*!
 * @brief Configure DMA transfer for Tx
 *
 * @param states    Pointer to master context structure
 * @param stcdBase  Base to the DMA TCD
 *
 * @retval    None
 */
static void CFGIO_I2C_MasterDmaTxConfig(
    CFGIO_I2C_MASTER_STATE_T *states,
    DMA_SOFTWARE_TCD_T *stcdBase)
{
    uint32_t addr;
    DMA_SCATTER_GATHER_LIST_T dmaListDest[2U];
    DMA_SCATTER_GATHER_LIST_T dmaListSrc[2U];

    /* First block: transmit data */
    if (states->transferDir == CFGIO_I2C_TX)
    {
        addr = (uint32_t)(states->txBuf);
    }
    else
    {
        /* Send 0xFF to keep the line clear if receiving */
        states->dummyDmaIdle = 0xFFU;
        addr = (uint32_t)(&(states->dummyDmaIdle));
    }
    dmaListDest[0U].type = DMA_TRANSFER_MEM2PERIPH;
    dmaListDest[0U].address = CFGIO_I2C_MasterCalcTxRegAddr(states);
    dmaListDest[0U].length = states->rxBytesLeft;
    dmaListSrc[0U].type = DMA_TRANSFER_MEM2PERIPH;
    dmaListSrc[0U].address = addr;
    dmaListSrc[0U].length = states->rxBytesLeft;

    /* 2nd block: transmit stop/repeated start */
    if (states->sendStop)
    {
        states->dummyDmaStop = 0U;
    }
    else
    {
        states->dummyDmaStop = 0xFFU;
    }
    dmaListDest[1U].type = DMA_TRANSFER_MEM2PERIPH;
    dmaListDest[1U].address = CFGIO_I2C_MasterCalcTxRegAddr(states);
    dmaListDest[1U].length = 1U;
    dmaListSrc[1U].type = DMA_TRANSFER_MEM2PERIPH;
    dmaListSrc[1U].address = (uint32_t)(&(states->dummyDmaStop));
    dmaListSrc[1U].length = 1U;

    /* Use 2 STCDs for Tx. Transfer size: 1 byte, 1 byte per DMA request. */
    (void)DMA_ConfigScatterGatherTransfer(states->txDmaChannel,
                                          stcdBase,
                                          DMA_TRANSFER_SIZE_1B,
                                          1U,
                                          dmaListSrc,
                                          dmaListDest,
                                          2U);

    if (states->transferDir == CFGIO_I2C_RX)
    {
        /* If there is no data to transmit, don't increment source offset */
        DMA_ConfigSrcOffset(states->txDmaChannel, 0);
    }
    /* Set for last block */
    CFGIO_I2C_MasterTerminateDmaBlock(stcdBase, 0U);
}

/*!
 * @brief Start DMA transfer
 *
 * @param states    Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterDmaTransferStart(CFGIO_I2C_MASTER_STATE_T *states)
{
    uint32_t alignedStcd;
    DMA_SOFTWARE_TCD_T *stcdBase;

    /* Get the aligned address to use for software TCDs */
    alignedStcd = STCD_ADDR(states->stcdBuf);
    stcdBase = (DMA_SOFTWARE_TCD_T *)(alignedStcd);

    /* Configure Tx and Rx chains */
    CFGIO_I2C_MasterDmaTxConfig(states, stcdBase);
    CFGIO_I2C_MasterDmaRxConfig(states, &stcdBase[CFGIO_I2C_DMA_TX_CHAIN_LENGTH]);

    /* Start both DMA channels */
    (void)DMA_StartChannel(states->txDmaChannel);
    (void)DMA_StartChannel(states->rxDmaChannel);
}

/*!
 * @brief End DMA transfer
 *
 * @param statePtr  Pointer to master context structure
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterDmaTransferEnd(void *statePtr)
{
    uint16_t timerCompare;
    CFGIO_I2C_MASTER_STATE_T *states = (CFGIO_I2C_MASTER_STATE_T *)statePtr;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Check DMA transfer errors */
    if (   (DMA_ReadChannelStatus(states->txDmaChannel) == DMA_CHANNEL_ERROR)
        || (DMA_ReadChannelStatus(states->rxDmaChannel) == DMA_CHANNEL_ERROR))
    {
        states->driverStatus = STATUS_ERROR;

        /* Force stop the transfer */
        CFGIO_I2C_MasterStopTransfer(states);

        /* Notify the event to the user */
        if (states->callback != NULL)
        {
            states->callback(I2C_MASTER_EVENT_TRANSFER_COMPLETE, states->callbackParam);
        }
        return;
    }

    /* Check for NACK */
    if (CFGIO_HW_GetShifterErrorStatus(base, SHIFTER_INDEX_RX(index)))
    {
        CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_RX(index));
        states->driverStatus = STATUS_I2C_NACK_ERROR;

        /* Force stop the transfer */
        CFGIO_I2C_MasterStopTransfer(states);

        /* Notify the event to the user */
        if (states->callback != NULL)
        {
            states->callback(I2C_MASTER_EVENT_TRANSFER_COMPLETE, states->callbackParam);
        }
        return;
    }

    /* Check if the transfer is done */
    if (CFGIO_HW_GetTimerStatus(base, TIMER_INDEX_SCL(index)))
    {
        states->eventCount--;

        /* Clear timer status */
        CFGIO_HW_ClearTimerStatus(base, TIMER_INDEX_SCL(index));
        if (states->eventCount == 2U)
        {
            /**
             * Adjust number of ticks in high part of timer compare register
             * for the last reload.
             */
            timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_SCL(index));
            timerCompare = (uint16_t)((uint32_t)timerCompare & 0x00FFU) |
                           (uint16_t)(((uint32_t)(states->reloadCounter) & 0xFFU) << 8U);
            CFGIO_HW_SetTimerCompare(base, TIMER_INDEX_SCL(index), timerCompare);
        }

        if (states->eventCount == 1U)
        {
            /* Timer will disable on the next countdown complete */
            CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_TIM_CMP);
        }

        if (states->eventCount == 0U)
        {
            CFGIO_HW_SetTimerDisable(base, TIMER_INDEX_SCL(index), CFGIO_TMR_DISABLE_NEVER);

            /* Record success if there was no error */
            if (states->driverStatus == STATUS_BUSY)
            {
                states->driverStatus = STATUS_SUCCESS;
            }

            /* End the transfer */
            CFGIO_I2C_MasterStopTransfer(states);

            /* Notify the event to the user */
            if (states->callback != NULL)
            {
                states->callback(I2C_MASTER_EVENT_TRANSFER_COMPLETE, states->callbackParam);
            }
        }
    }
}

/*!
 * @brief Trigger DMA transfer immediately
 *
 * @param stcdBase      Base to the DMA TCD
 * @param blockNumber   Block number
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterTriggerDmaBlock(
    DMA_SOFTWARE_TCD_T *stcdBase,
    uint8_t blockNumber)
{
    /* Set the START bit for this TCD to 1 */
    stcdBase[blockNumber].CSR |= (uint16_t)(1U);
}

/*!
 * @brief Disable the DMA request upon transfer completion
 *
 * @param stcdBase      Base to the DMA TCD
 * @param blockNumber   block number
 *
 * @retval  None
 */
static void CFGIO_I2C_MasterTerminateDmaBlock(
    DMA_SOFTWARE_TCD_T *stcdBase,
    uint8_t blockNumber)
{
    /* Set the DREQ bit for this TCD to 1 */
    stcdBase[blockNumber].CSR |= (uint16_t)(1U << 3U);
}

/**@} end of group CFGIO_I2C_Functions*/
/**@} end of group CFGIO_I2C_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
