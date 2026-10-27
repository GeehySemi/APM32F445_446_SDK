/*!
 * @file        apm32f445_446_cfgio_spi.c
 *
 * @brief       This file provides all the CFGIO_SPI firmware functions
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

#include "apm32f445_446_cfgio_spi.h"
#include "apm32f445_446_cfgio_common.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_SPI_Driver CFGIO_SPI Driver
  @{
*/

/** @defgroup CFGIO_SPI_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

#define TX_SHIFTER(x)     (x)
#define RX_SHIFTER(x)     (uint8_t)((x) + 1U)
#define SCK_TIMER(x)      (x)
#define SS_TIMER(x)       (uint8_t)((x) + 1U)

/* Threshold value for baudrate calculation */
#define DIVIDER_MIN 1
#define DIVIDER_MAX 0xFF

/* Virtual data send when the user does not provide data */
#define CFGIO_SPI_VIRTUAL_DATA (0xFFFFFFFFU)

/**@} end of group CFGIO_SPI_Macros*/

/** @defgroup CFGIO_SPI_Functions Functions
  @{
*/
/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static void CFGIO_SPI_MasterConfigure(const CFGIO_SPI_MASTER_STATE_T *master, const CFGIO_SPI_MASTER_CFG_T * configPtr, uint32_t inputClock);
static void CFGIO_SPI_MasterCalculateBaudrateDivider(uint32_t baudrate, uint16_t *divider, uint32_t inputClock);
static void CFGIO_SPI_MasterEnableTransfer(CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_MasterStopTransfer(CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_MasterEndTransfer(CFGIO_SPI_MASTER_STATE_T *master);
static STATUS_T CFGIO_SPI_MasterWaitTransferEnd(CFGIO_SPI_MASTER_STATE_T *master, uint32_t timeout);
static void CFGIO_SPI_MasterStartDmaTransfer(CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_MasterEndDmaTransfer(void *statePtr, DMA_CHANNEL_STATUS_T status);
static uint32_t CFGIO_SPI_MasterCalculateTxRegisterAddr(const CFGIO_SPI_MASTER_STATE_T *master);
static uint32_t CFGIO_SPI_MasterCalculateRxRegisterAddr(const CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_MasterCheckStatus(void *statePtr);
static void CFGIO_SPI_ReadData(CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_WriteData(CFGIO_SPI_MASTER_STATE_T *master);
static void CFGIO_SPI_SlaveConfigure(const CFGIO_SPI_SLAVE_STATE_T *slave, const CFGIO_SPI_SLAVE_CFG_T * configPtr);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initializes the CFGIO SPI instance for interrupt driven master mode operation.
 *
 * @param   instance  : The peripheral instance number of CFGIO
 * @param   configPtr : Pointer to the CFGIO_SPI master user configuration structure.
 * @param   master    : User defined CFGIO_SPI mastet stauts
 *
 * @retval  Error codes
 */
STATUS_T CFGIO_SPI_MasterInit(
    uint32_t instance,
    const CFGIO_SPI_MASTER_CFG_T *configPtr,
    CFGIO_SPI_MASTER_STATE_T *master)
{
    STATUS_T result;
    uint32_t inputClock;
    uint8_t dmaRxRequest;
    uint8_t dmaTxRequest;

    /* Obtain protocol clock frequency */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[instance], &inputClock);

    /* Assign two shifters/timers */
    master->commonCFGIO.resCount = 2U;
    /* CFGIO driver initialization */
    result = CFGIO_InitDriver(instance, &(master->commonCFGIO));
    if(result == STATUS_SUCCESS)
    {
        /* Initialize of a semaphore */
        if (configPtr->cfgspiDirType != CFGIO_USE_POLLING)
        {
            (void)OSIF_SemCreate(&(master->blockSemaphore), 0U);
        }

        /* Initialize driver structure */
        master->status = STATUS_SUCCESS;
        master->callbackParam = configPtr->callbackParam;
        master->callback = configPtr->callback;
        master->firstBitOrder = configPtr->firstBitOrder;
        master->cfgspiDirType = configPtr->cfgspiDirType;
        master->transferSize = configPtr->transferSize;
        master->driverIdleStatus = true;
        master->blocking = false;
        master->master = true;

        /* Configure CFGIO_SPI device */
        CFGIO_SPI_MasterConfigure(master, configPtr, inputClock);
        /* Set up transmission engine */
        if(configPtr->cfgspiDirType == CFGIO_USE_INTERRUPTS)
        {
            master->commonCFGIO.isr = CFGIO_SPI_MasterCheckStatus;
        }
        else if(configPtr->cfgspiDirType == CFGIO_USE_DMA)
        {
            /* Storage DMA channel number */
            master->rxDMAChannel = configPtr->rxDMAChannel;
            master->txDMAChannel = configPtr->txDMAChannel;
            /* Configure DMA request sources */
            dmaTxRequest = g_cfgioDmaSrc[instance][TX_SHIFTER(master->commonCFGIO.resIndex)];
            dmaRxRequest = g_cfgioDmaSrc[instance][RX_SHIFTER(master->commonCFGIO.resIndex)];
            (void)DMA_ConfigChannelRequestAndTrigger(configPtr->txDMAChannel, dmaTxRequest, false);
            (void)DMA_ConfigChannelRequestAndTrigger(configPtr->rxDMAChannel, dmaRxRequest, false);
        }
        return STATUS_SUCCESS;
    }
    else
    {
        return result;
    }
}


/*!
 * @brief   Close the CFGIO_SPI instance
 *
 * @details This function resets the CFGIO_SPI peripheral,sets its clock,and disables interrupts to
 *          the core,it first checks whether the transmission is in progress,and if so,returns
 *          error statistics information
 *
 * @param   master    : User defined CFGIO_SPI mastet stauts
 *
 * @retval  Error codes
 *
 */
STATUS_T CFGIO_SPI_MasterDeinit(CFGIO_SPI_MASTER_STATE_T *master)
{
    /* Check the driver busy status */
    if (!master->driverIdleStatus)
    {
        return STATUS_BUSY;
    }
    /* Destroy semaphore */
    if (master->cfgspiDirType != CFGIO_USE_POLLING)
    {
        (void)OSIF_SemDestroy(&(master->blockSemaphore));
    }

    return CFGIO_DeinitDriver(&(master->commonCFGIO));
}


/*!
 * @brief   initializes the user-provided structure
 *
 * @details Initializes the structured interrupt based CFGIO_SPI transmission configuration
 *           provided by the user
 *
 * @param   configPtr : The pointer to CFGIO_SPI_MASTER_CFG_T.
 *
 */
void CFGIO_SPI_MasterDefaultConfig(CFGIO_SPI_MASTER_CFG_T * configPtr)
{
    configPtr->callbackParam = NULL;
    configPtr->callback = NULL;
    configPtr->cfgspiDirType = CFGIO_USE_INTERRUPTS;
    configPtr->firstBitOrder = CFGIO_SPI_SEND_MSB_FIRST;
    configPtr->transferSize = CFGIO_SPI_DATA_SIZE_1BYTE;
    configPtr->txDMAChannel = 255U;
    configPtr->rxDMAChannel = 255U;
    configPtr->clockPol = 0U;
    configPtr->clockPha = 0U;
    configPtr->baudrate = 100000U;
    configPtr->ssPin = 3U;
    configPtr->sckPin = 2U;
    configPtr->misoPin = 1U;
    configPtr->mosiPin = 0U;
}


/*!
 * @brief   Read configured baudrate
 *
 * @param   master      : User defined CFGIO_SPI mastet stauts
 * @param   baudrate    : Current baudrate
 *
 * @retval  Error codes
 */
STATUS_T CFGIO_SPI_MasterReadBaudrate(CFGIO_SPI_MASTER_STATE_T *master, uint32_t *baudrate)
{
    uint32_t inputClock;
    uint16_t timerCompare;
    uint16_t divider;
    const CFGIO_T *baseAddress;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    /* Get the clock frequency */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[master->commonCFGIO.instance], &inputClock);

    /* Get the configured divider */
    timerCompare = CFGIO_HW_GetTimerCompare(baseAddress, SCK_TIMER(index));
    divider = (uint16_t)(timerCompare & 0x00FFU);

    /* Calculate baudrate */
    *baudrate = (inputClock + divider + 1U) / (2U * ((uint32_t)divider + 1U));

    return STATUS_SUCCESS;
}

/*!
 * @brief   Sets the LPSPI baud rate in bits per second.
 *
 * @details Due to module limitations, no baud rate can be achieved,
 *          and only the baud rate can be set as close as possible,
 *          but there may be differences. For example, when using a
 *          low-frequency clock, CFGIO_SPI_MasterSetBaudrate should
 *          be called after CFGIO_SPI_MasterReadBaudrate to see the
 *          actual baud rate
 *
 * @param   master      : User defined CFGIO_SPI mastet stauts
 * @param   baudrate    : Current baudrate
 *
 * @retval  Error codes
 *
 */
STATUS_T CFGIO_SPI_MasterSetBaudrate(CFGIO_SPI_MASTER_STATE_T *master, uint32_t baudrate)
{
    uint32_t inputClock;
    uint16_t divider;
    uint16_t timerCompare;
    CFGIO_T *baseAddress;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    if(master->driverIdleStatus)
    {
        /* Obtain protocol clock frequency */
        CLOCK_SYS_ReadFreq(g_cfgioClockSrc[master->commonCFGIO.instance], &inputClock);

        /* Calculate divider */
        CFGIO_SPI_MasterCalculateBaudrateDivider(baudrate, &divider, inputClock);

        /* Configure the timer divider in the lower 8 bits of the register */
        timerCompare = CFGIO_HW_GetTimerCompare(baseAddress, SCK_TIMER(index));
        timerCompare = (uint16_t)((timerCompare & 0xFF00U) | divider);
        CFGIO_HW_SetTimerCompare(baseAddress, SCK_TIMER(index), timerCompare);

        return STATUS_SUCCESS;
    }
    else
    {
        return STATUS_BUSY;
    }
}

/*!
 * @brief   Perform interrupt-driven non-blocking SPI main mode transfer.
 *
 * @details This function performs a full-duplex parallel send and receive of spi,
 *          and can supply NULL for the parameter if only send or receive is required.
 *
 * @param   master   : User defined CFGIO_SPI mastet stauts
 * @param   txData   : User-sent data cache.
 * @param   rxData   : User-received data cache.
 * @param   dataSize : Length in bytes of the data to be transferred
 *
 * @retval  Error codes.
 *
 */
STATUS_T CFGIO_SPI_MasterTransferNonBlocking(
    CFGIO_SPI_MASTER_STATE_T *master,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize)
{
    STATUS_T result = STATUS_BUSY;
    CFGIO_T *baseAddress;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    /* Check the driver busy status */
    if (master->driverIdleStatus)
    {
        /* Initialize transfer data */
        master->txBytesRemaining = dataSize;
        master->rxBytesRemaining = dataSize;
        master->txData = txData;
        master->rxData = rxData;
        master->driverIdleStatus = false;
        master->status = STATUS_BUSY;

        /* Enable transfer */
        CFGIO_SPI_MasterEnableTransfer(master);

        if(master->cfgspiDirType == CFGIO_USE_POLLING)
        {
            /* Call CFGIO_SPI_MasterCheckStatus to send the first byte at once */
            CFGIO_SPI_MasterCheckStatus(master);
        }
        else if(master->cfgspiDirType == CFGIO_USE_DMA)
        {
            CFGIO_SPI_MasterStartDmaTransfer(master);
        }
        else if(master->cfgspiDirType == CFGIO_USE_INTERRUPTS)
        {
            /* Enable interrupts for Rx and Tx */
            CFGIO_HW_SetShifterInterrupt(baseAddress,
                                      (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                                      true);
            CFGIO_HW_SetShifterErrorInterrupt(baseAddress,
                                           (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                                           true);
        }
        result = STATUS_SUCCESS;
    }
    return result;
}


/*!
 * @brief   Perform interrupt-driven blocking SPI main mode transmission.
 *
 * @details This function performs a full-duplex parallel send and receive of spi,
 *          and can supply NULL for the parameter if only send or receive is required.
 *          The transfer is blocking, the function only returns when the transfer is complete.
 *
 * @param   master   : User defined CFGIO_SPI mastet stauts
 * @param   txData   : User-sent data cache.
 * @param   rxData   : User-received data cache.
 * @param   dataSize : Length in bytes of the data to be transferred
 * @param   timeout  : Set timeout.
 *
 * @retval  Error codes.
 *
 */
STATUS_T CFGIO_SPI_MasterTransferBlocking(
    CFGIO_SPI_MASTER_STATE_T *master,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize,
    uint32_t timeout)
{
    STATUS_T status = STATUS_BUSY;

    /* Check the driver busy status */
    if (master->driverIdleStatus)
    {
        /* Mark transfer as blocked*/
        if (master->cfgspiDirType != CFGIO_USE_POLLING)
        {
            master->blocking = true;

            /* wait to ensure that the semaphore is 0 without checking the result */
            (void)OSIF_SemWait(&(master->blockSemaphore), 0);
        }

        status = CFGIO_SPI_MasterTransferNonBlocking(master, txData, rxData, dataSize);
        if (status == STATUS_SUCCESS)
        {
            /* Wait for transmission to end */
            status = CFGIO_SPI_MasterWaitTransferEnd(master, timeout);
        }
        else
        {
            /* Unable to start transfer */
            master->blocking = false;
        }
    }
    return status;
}

/*!
 * @brief   Early termination of interrupt-driven asynchronous transmission
 *
 * @param   master   : User defined CFGIO_SPI mastet stauts
 *
 * @retval  Error codes.
 */
STATUS_T CFGIO_SPI_MasterAbortTransfer(CFGIO_SPI_MASTER_STATE_T *master)
{
    /* Check the driver busy status */
    if (master->driverIdleStatus)
    {
        return STATUS_SUCCESS;
    }
    master->status = STATUS_SPI_ABORTED;
    CFGIO_SPI_MasterStopTransfer(master);

    return STATUS_SUCCESS;
}


/*!
 * @brief   Read the status of the non blocking SPI master transaction
 *
 * @param   master           : User defined CFGIO_SPI mastet stauts
 * @param   bytesRemaining   : Remaining bytes count
 *
 * @retval  Error codes.
 */
STATUS_T CFGIO_SPI_MasterReadStatus(
    CFGIO_SPI_MASTER_STATE_T *master,
    uint32_t *bytesRemaining)
{
    uint32_t remainingBytes;

    /* Initialize with the remaining byte count */
    remainingBytes = master->rxBytesRemaining;

    if (!master->driverIdleStatus)
    {
        if(master->cfgspiDirType == CFGIO_USE_POLLING)
        {
            /* In polling mode, the SPI transfer is advanced here */
            CFGIO_SPI_MasterCheckStatus(master);
        }
        else if(master->cfgspiDirType == CFGIO_USE_DMA)
        {
            /* In DMA mode,simply update the remaining counts and do not write master->txBytesRemaining */
            remainingBytes = DMA_ReadRemainingMajorIterationsCount(master->rxDMAChannel);
        }
    }

    if (bytesRemaining != NULL)
    {
        *bytesRemaining = remainingBytes;
    }
    return master->status;
}


/*!
 * @brief   Initializes the CFGIO SPI instance for interrupt driven slave mode operation.
 *
 * @param   instance  : The peripheral instance number of CFGIO
 * @param   configPtr : Pointer to the CFGIO_SPI master user configuration structure.
 * @param   slave     : User defined CFGIO_SPI slave stauts
 *
 * @retval  Error codes
 */
STATUS_T CFGIO_SPI_SlaveInit(
    uint32_t instance,
    const CFGIO_SPI_SLAVE_CFG_T *configPtr,
    CFGIO_SPI_SLAVE_STATE_T *slave)
{
    uint8_t dmaRxRequest;
    uint8_t dmaTxRequest;
    STATUS_T result;

    /* Assign two shifters/timers */
    slave->commonCFGIO.resCount = 2U;
    /* CFGIO driver initialization */
    result = CFGIO_InitDriver(instance, &(slave->commonCFGIO));
    if(result == STATUS_SUCCESS)
    {
        /* Initialize of a semaphore */
        if (configPtr->cfgspiDirType != CFGIO_USE_POLLING)
        {
            (void)OSIF_SemCreate(&(slave->blockSemaphore), 0U);
        }

        /* Initialize driver structure */
        slave->callbackParam = configPtr->callbackParam;
        slave->callback = configPtr->callback;
        slave->cfgspiDirType = configPtr->cfgspiDirType;
        slave->firstBitOrder = configPtr->firstBitOrder;
        slave->transferSize = configPtr->transferSize;
        slave->driverIdleStatus = true;
        slave->blocking = false;
        slave->master = false;
        slave->status = STATUS_SUCCESS;

        /* Configure CFGIO_SPI device */
        CFGIO_SPI_SlaveConfigure(slave, configPtr);
        /* Set up transmission engine */
        if(configPtr->cfgspiDirType == CFGIO_USE_INTERRUPTS)
        {
            slave->commonCFGIO.isr = CFGIO_SPI_MasterCheckStatus;
        }
        else if(configPtr->cfgspiDirType == CFGIO_USE_DMA)
        {
            /* Storage DMA channel number */
            slave->rxDMAChannel = configPtr->rxDMAChannel;
            slave->txDMAChannel = configPtr->txDMAChannel;
            /* Configure DMA request sources */
            dmaTxRequest = g_cfgioDmaSrc[instance][TX_SHIFTER(slave->commonCFGIO.resIndex)];
            dmaRxRequest = g_cfgioDmaSrc[instance][RX_SHIFTER(slave->commonCFGIO.resIndex)];
            (void)DMA_ConfigChannelRequestAndTrigger(configPtr->txDMAChannel, dmaTxRequest, false);
            (void)DMA_ConfigChannelRequestAndTrigger(configPtr->rxDMAChannel, dmaRxRequest, false);
        }
        return STATUS_SUCCESS;
    }
    else
    {
        return result;
    }
}

/*!
 * @brief   Close the CFGIO_SPI slave mode instance
 *
 * @details This function resets the CFGIO_SPI peripheral,sets its clock,and disables interrupts to
 *          the core,it first checks whether the transmission is in progress,and if so,returns
 *          error statistics information
 *
 * @param   slave   : User defined CFGIO_SPI slave stauts
 *
 * @retval  Error codes
 *
 */
STATUS_T CFGIO_SPI_SlaveDeinit(CFGIO_SPI_SLAVE_STATE_T *slave)
{
    return CFGIO_SPI_MasterDeinit(slave);
}


/*!
 * @brief   initializes the user-provided structure
 *
 * @details Initializes the structured interrupt based CFGIO_SPI transmission configuration
 *          provided by the user
 *
 * @param   configPtr : The pointer to CFGIO_SPI_SLAVE_CFG_T.
 *
 */
void CFGIO_SPI_SlaveDefaultConfig(CFGIO_SPI_SLAVE_CFG_T * configPtr)
{

    configPtr->callbackParam = NULL;
    configPtr->callback = NULL;
    configPtr->cfgspiDirType = CFGIO_USE_INTERRUPTS;
    configPtr->firstBitOrder = CFGIO_SPI_SEND_MSB_FIRST;
    configPtr->transferSize = CFGIO_SPI_DATA_SIZE_1BYTE;
    configPtr->txDMAChannel = 255U;
    configPtr->rxDMAChannel = 255U;
    configPtr->clockPol = 0U;
    configPtr->clockPha = 0U;
    configPtr->ssPin = 3U;
    configPtr->sckPin = 2U;
    configPtr->misoPin = 1U;
    configPtr->mosiPin = 0U;
}


/*!
 * @brief   Perform interrupt-driven non-blocking SPI slave mode transfer.
 *
 * @details This function performs a full-duplex parallel send and receive of spi,
 *          and can supply NULL for the parameter if only send or receive is required.
 *
 * @param   slave    : User defined CFGIO_SPI slave stauts
 * @param   txData   : User-sent data cache.
 * @param   rxData   : User-received data cache.
 * @param   dataSize : Length in bytes of the data to be transferred
 *
 * @retval  Error codes.
 *
 */
STATUS_T CFGIO_SPI_SlaveTransferNonBlocking(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize)
{
    return CFGIO_SPI_MasterTransferNonBlocking(slave, txData, rxData, dataSize);
}


/*!
 * @brief   Perform interrupt-driven blocking SPI slave mode transmission.
 *
 * @details This function performs a full-duplex parallel send and receive of spi,
 *          and can supply NULL for the parameter if only send or receive is required.
 *          The transfer is blocking, the function only returns when the transfer is complete.
 *
 * @param   slave    : User defined CFGIO_SPI slave stauts
 * @param   txData   : User-sent data cache.
 * @param   rxData   : User-received data cache.
 * @param   dataSize : Length in bytes of the data to be transferred
 * @param   timeout  : Set timeout.
 *
 * @retval  Error codes.
 *
 */
STATUS_T CFGIO_SPI_SlaveTransferBlocking(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize,
    uint32_t timeout)
{
    return CFGIO_SPI_MasterTransferBlocking(slave, txData, rxData, dataSize, timeout);
}


/*!
 * @brief   Early termination of interrupt-driven asynchronous transmission
 *
 * @param   slave   : User defined CFGIO_SPI slave stauts
 *
 * @retval  Error codes.
 */
STATUS_T CFGIO_SPI_SlaveAbortTransfer(CFGIO_SPI_SLAVE_STATE_T *slave)
{
    return CFGIO_SPI_MasterAbortTransfer(slave);
}

/*!
 * @brief   Read the status of the non blocking SPI slave transaction
 *
 * @param   slave           : User defined CFGIO_SPI slave stauts
 * @param   bytesRemaining  : Remaining bytes count
 *
 * @retval  Error codes.
 */
STATUS_T CFGIO_SPI_SlaveReadStatus(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    uint32_t *bytesRemaining)
{
    return CFGIO_SPI_MasterReadStatus(slave, bytesRemaining);
}


/*!
 * @brief   Configure CFGIO module as SPI master module
 *
 * @param   master      : User defined CFGIO_SPI mastet stauts
 * @param   configPtr   : Pointer to the CFGIO_SPI master user configuration structure.
 * @param   inputClock  : Input clock
 *
 */
void CFGIO_SPI_MasterConfigure(
    const CFGIO_SPI_MASTER_STATE_T *master,
    const CFGIO_SPI_MASTER_CFG_T * configPtr,
    uint32_t inputClock)
{
    CFGIO_TMR_POLARITY_T rxShifterPolarity;   /* Polarity of MISO */
    CFGIO_TMR_POLARITY_T txShifterPolarity;   /* Polarity of MOSI */
    CFGIO_PIN_POLARITY_T clockPol;       /* Polarity of clock */
    CFGIO_SHIFT_START_T startBit;
    CFGIO_SHIFT_STOP_T stopBit;
    CFGIO_T *baseAddress;
    uint16_t divider;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    /* Calculate divider */
    CFGIO_SPI_MasterCalculateBaudrateDivider(configPtr->baudrate, &divider, inputClock);

    /*Adds the digits in the upper 8 digits of the divider.Formula is: TIMCMP[15:8] = (number of bits x 2) - 1 */
    divider += (uint16_t)((((uint16_t)(configPtr->transferSize) * 8U * 2U) - 1U) << 8U);

    if (configPtr->clockPol != 0U)
    {
        clockPol = CFGIO_PIN_POLARITY_LOW;
    }
    else
    {
        clockPol = CFGIO_PIN_POLARITY_HIGH;
    }

    if (configPtr->clockPha == 0U)
    {
        startBit = CFGIO_SHIFT_START_BIT_DISABLED;
        stopBit = CFGIO_SHIFT_STOP_BIT_DISABLED;
        txShifterPolarity = CFGIO_TMR_POLARITY_NEG_EDGE;
        rxShifterPolarity = CFGIO_TMR_POLARITY_POS_EDGE;
    }
    else
    {
        startBit = CFGIO_SHIFT_START_BIT_DISABLED_SH;
        stopBit = CFGIO_SHIFT_STOP_BIT_0;
        txShifterPolarity = CFGIO_TMR_POLARITY_POS_EDGE;
        rxShifterPolarity = CFGIO_TMR_POLARITY_NEG_EDGE;
    }

    /* Configure MOSI */
    CFGIO_HW_SetShifterControl(baseAddress,
                            TX_SHIFTER(index),
                            CFGIO_SHIFT_MODE_DISABLED,
                            configPtr->mosiPin,
                            CFGIO_PIN_POLARITY_HIGH,
                            CFGIO_PIN_CFG_OUTPUT,
                            SCK_TIMER(index),
                            txShifterPolarity);
    CFGIO_HW_SetShifterConfig(baseAddress,
                           TX_SHIFTER(index),
                           startBit,
                           stopBit,
                           CFGIO_SHIFT_SRC_PIN);

    /* Configure MISO */
    CFGIO_HW_SetShifterControl(baseAddress,
                            RX_SHIFTER(index),
                            CFGIO_SHIFT_MODE_DISABLED,
                            configPtr->misoPin,
                            CFGIO_PIN_POLARITY_HIGH,
                            CFGIO_PIN_CFG_DISABLED,
                            SCK_TIMER(index),
                            rxShifterPolarity);
    CFGIO_HW_SetShifterConfig(baseAddress,
                           RX_SHIFTER(index),
                           CFGIO_SHIFT_START_BIT_DISABLED,
                           CFGIO_SHIFT_STOP_BIT_DISABLED,
                           CFGIO_SHIFT_SRC_PIN);

    /* Configure sck timer, baud rate, and bit count */
    CFGIO_HW_SetTimerCompare(baseAddress, SCK_TIMER(index), divider);
    CFGIO_HW_SetTimerConfig(baseAddress,
                      SCK_TIMER(index),
                      CFGIO_TMR_START_BIT_ENABLED,
                      CFGIO_TMR_STOP_BIT_TIM_DIS,
                      CFGIO_TMR_ENABLE_TRG_HIGH,
                      CFGIO_TMR_DISABLE_TIM_CMP,
                      CFGIO_TMR_RESET_NEVER,
                      CFGIO_TMR_DEC_CLK_SHIFT_TMR,
                      CFGIO_TMR_OUTPUT_ZERO);
    CFGIO_HW_SetTimerControl(baseAddress,
                          SCK_TIMER(index),
                          (uint8_t)((uint8_t)(TX_SHIFTER(index) << 2U) + 1U), /* Tx Trigger status flag on the shifter */
                          CFGIO_TRG_POLARITY_LOW,
                          CFGIO_TRG_SRC_INTERNAL,
                          configPtr->sckPin,
                          clockPol,
                          CFGIO_PIN_CFG_OUTPUT,
                          CFGIO_TMR_MODE_DISABLED);

    /* Configure SS timer */
    CFGIO_HW_SetTimerCompare(baseAddress, SS_TIMER(index), 0xFFFFU);
    CFGIO_HW_SetTimerConfig(baseAddress,
                      SS_TIMER(index),
                      CFGIO_TMR_START_BIT_DISABLED,
                      CFGIO_TMR_STOP_BIT_DISABLED,
                      CFGIO_TMR_ENABLE_TIM_ENABLE,    /* Enable SCK timer */
                      CFGIO_TMR_DISABLE_TIM_DISABLE,  /* Disable SCK timer */
                      CFGIO_TMR_RESET_NEVER,
                      CFGIO_TMR_DEC_CLK_SHIFT_TMR,
                      CFGIO_TMR_OUTPUT_ONE);
    CFGIO_HW_SetTimerControl(baseAddress,
                          SS_TIMER(index),
                          0U,   /* No trigger is used, default values are used */
                          CFGIO_TRG_POLARITY_HIGH,
                          CFGIO_TRG_SRC_EXTERNAL,
                          configPtr->ssPin,
                          CFGIO_PIN_POLARITY_LOW,
                          CFGIO_PIN_CFG_OUTPUT,
                          CFGIO_TMR_MODE_DISABLED);
}

/*!
 * @brief   Baud rate allocator for calcuating target baud rate
 *
 * @param   baudrate    : Baudrate
 * @param   divider     : Divider
 * @param   inputClock  : Input clock
 *
 */
 void CFGIO_SPI_MasterCalculateBaudrateDivider(
    uint32_t baudrate,
    uint16_t *divider,
    uint32_t inputClock)
{
    int32_t tmpDivider;

    /* Calculate divider */
    tmpDivider = (((int32_t)inputClock + (int32_t)baudrate) / (2 * (int32_t)baudrate)) - 1;

    /* Enforce the upper/lower limit */
    if (tmpDivider > DIVIDER_MAX)
    {
        tmpDivider = DIVIDER_MAX;
    }

    if (tmpDivider < DIVIDER_MIN)
    {
        tmpDivider = DIVIDER_MIN;
    }

    *divider = (uint16_t)tmpDivider;
}


/*!
 * @brief   Enable timers and shifters to start transfer
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
 void CFGIO_SPI_MasterEnableTransfer(CFGIO_SPI_MASTER_STATE_T *master)
{
    uint8_t index;
    CFGIO_T *baseAddress;

    index = master->commonCFGIO.resIndex;
    baseAddress = g_cfgioBase[master->commonCFGIO.instance];

    /* enable timers and shifters */
    CFGIO_HW_SetShifterMode(baseAddress, TX_SHIFTER(index), CFGIO_SHIFT_MODE_TRANSMIT);
    CFGIO_HW_SetShifterMode(baseAddress, RX_SHIFTER(index), CFGIO_SHIFT_MODE_RECEIVE);

    if (master->master)
    {
        CFGIO_HW_SetTimerMode(baseAddress, SCK_TIMER(index), CFGIO_TMR_MODE_8BIT_BAUD);
        CFGIO_HW_SetTimerMode(baseAddress, SS_TIMER(index), CFGIO_TMR_MODE_16BIT);
    }
    else
    {
        CFGIO_HW_SetTimerMode(baseAddress, SCK_TIMER(index), CFGIO_TMR_MODE_16BIT);
    }
}

/*!
 * @brief   Force stop of current transmission
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
 void CFGIO_SPI_MasterStopTransfer(CFGIO_SPI_MASTER_STATE_T *master)
{
    uint8_t index;
    CFGIO_T *baseAddress;

    index = master->commonCFGIO.resIndex;
    baseAddress = g_cfgioBase[master->commonCFGIO.instance];

    /* disable and re-enable timers and shifters */
    CFGIO_HW_SetShifterMode(baseAddress, TX_SHIFTER(index), CFGIO_SHIFT_MODE_DISABLED);
    CFGIO_HW_SetShifterMode(baseAddress, RX_SHIFTER(index), CFGIO_SHIFT_MODE_DISABLED);
    CFGIO_HW_SetTimerMode(baseAddress, SCK_TIMER(index), CFGIO_TMR_MODE_DISABLED);
    CFGIO_HW_SetTimerMode(baseAddress, SS_TIMER(index), CFGIO_TMR_MODE_DISABLED);

    /* clear all remaining error flags */
    CFGIO_HW_ClearShifterErrorStatus(baseAddress, TX_SHIFTER(index));
    CFGIO_HW_ClearShifterErrorStatus(baseAddress, RX_SHIFTER(index));

    /* discard any remaining Rx data */
    CFGIO_HW_ClearShifterStatus(baseAddress, RX_SHIFTER(index));

    /* end transfer */
    CFGIO_SPI_MasterEndTransfer(master);
}


/*!
 * @brief   End transfer
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
void CFGIO_SPI_MasterEndTransfer(CFGIO_SPI_MASTER_STATE_T *master)
{
    uint8_t index;
    CFGIO_T *baseAddress;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;
    /* Disable transfer */
    if(master->cfgspiDirType == CFGIO_USE_INTERRUPTS)
    {
        /* Disable interrupts for RX and TX */
        CFGIO_HW_SetShifterInterrupt(baseAddress,
                                  (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                                  false);
        CFGIO_HW_SetShifterErrorInterrupt(baseAddress,
                                       (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                                       false);
    }
    else if(master->cfgspiDirType == CFGIO_USE_DMA)
    {
        /* Stop DMA channels */
        (void)DMA_StopChannel(master->txDMAChannel);
        (void)DMA_StopChannel(master->rxDMAChannel);

        /* Disable CFGIO DMA requests for both */
        CFGIO_HW_SetShifterDmaRequest(baseAddress,
                                   (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                                   false);
    }
    master->txBytesRemaining = 0U;
    master->rxBytesRemaining = 0U;
    master->driverIdleStatus = true;

    /* The transmitting end of a signal used to block transmission */
    if (master->blocking == true)
    {
        (void)OSIF_SemPost(&(master->blockSemaphore));
    }
}


/*!
 * @brief   Waiting for blocked transmission to end
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 * @param   timeout : Timeout
 *
 * @retval  Error codes
 */
 STATUS_T CFGIO_SPI_MasterWaitTransferEnd(CFGIO_SPI_MASTER_STATE_T *master, uint32_t timeout)
{
    STATUS_T osResult = STATUS_SUCCESS;

    if(master->cfgspiDirType == CFGIO_USE_INTERRUPTS)
    {
        /* Wait for IRQ to finish transmitting */
        osResult = OSIF_SemWait(&(master->blockSemaphore), timeout);
    }
    else if(master->cfgspiDirType == CFGIO_USE_POLLING)
    {
        /* Call CFGIO_SPI_MasterReadStatus() */
        while (CFGIO_SPI_MasterReadStatus(master, NULL) == STATUS_BUSY)
        {
        }
    }
    else if(master->cfgspiDirType == CFGIO_USE_DMA)
    {
        osResult = OSIF_SemWait(&(master->blockSemaphore), timeout);
    }
    /* Blocking transmission end */
    master->blocking = false;
    if (osResult == STATUS_TIMEOUT)
    {
        /* abort current transmission */
        master->status = STATUS_TIMEOUT;
        CFGIO_SPI_MasterStopTransfer(master);
    }
    return master->status;
}


/*!
 * @brief   Start DMA transmission
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
 void CFGIO_SPI_MasterStartDmaTransfer(CFGIO_SPI_MASTER_STATE_T *master)
{
    uint8_t index;
    CFGIO_T *baseAddress;
    uint32_t addr;

    /* Map the SPI transfer sizes to the DMA transfer size */
    const DMA_TRANSFER_SIZE_T dmaTransferSize[CFGIO_SPI_DATA_SIZE_4BYTE] =
    {
        DMA_TRANSFER_SIZE_1B,
        DMA_TRANSFER_SIZE_2B,
        DMA_TRANSFER_SIZE_4B,
        DMA_TRANSFER_SIZE_4B
    };

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    if (master->txData != NULL)
    {
        addr = (uint32_t)(master->txData);
    }
    else
    {
        /* If there is no data to transfer, use virtual data source data for DMA transmission */
        master->virtualDmaData = CFGIO_SPI_VIRTUAL_DATA;
        addr = (uint32_t)(&(master->virtualDmaData));
    }

    /* Configure the transmission control descriptor for the Tx channel */
    (void)DMA_ConfigMultiBlockTransfer(master->txDMAChannel,
                                       DMA_TRANSFER_MEM2PERIPH,
                                       addr,
                                       CFGIO_SPI_MasterCalculateTxRegisterAddr(master),
                                       dmaTransferSize[(uint32_t)master->transferSize - 1U],
                                       (uint32_t)master->transferSize,
                                       master->txBytesRemaining / (uint32_t)master->transferSize,
                                       true);

    if (master->txData == NULL)
    {
        /* If there is no data to transmit, don't increment source offset */
        DMA_ConfigSrcOffset(master->txDMAChannel, 0);
    }

    if (master->rxData == NULL)
    {
        /* If there is no data to receive, do notincrease the source offset */
        addr = (uint32_t)(&(master->virtualDmaData));
    }
    else
    {
        addr = (uint32_t)(master->rxData);
    }

    /* Configure the transmission control descriptor for the Rx channel */
    (void)DMA_ConfigMultiBlockTransfer(master->rxDMAChannel,
                                       DMA_TRANSFER_PERIPH2MEM,
                                       CFGIO_SPI_MasterCalculateRxRegisterAddr(master),
                                       addr,
                                       dmaTransferSize[(uint32_t)master->transferSize - 1U],
                                       (uint32_t)master->transferSize,
                                       master->rxBytesRemaining / (uint32_t)master->transferSize,
                                       true);
    if (master->rxData == NULL)
    {
        /* if there is no data to receive, do not increase the target offset */
        DMA_ConfigDestOffset(master->rxDMAChannel, 0);
    }

    /* DMA transfer end setting callback */
    (void)DMA_RegisterCallback(master->rxDMAChannel,
                               (DMA_CALLBACK_T)(CFGIO_SPI_MasterEndDmaTransfer),
                               (void*)(master));

    /* Start both DMA channels */
    (void)DMA_StartChannel(master->txDMAChannel);
    (void)DMA_StartChannel(master->rxDMAChannel);

    /* Enable CFGIO DMA requests */
    CFGIO_HW_SetShifterDmaRequest(baseAddress,
                               (uint8_t)((1U << TX_SHIFTER(index)) | (1U << RX_SHIFTER(index))),
                               true);
}

/*!
 * @brief   End DMA transfer
 *
 * @param   statePtr    : Pointer to the Master internal context structure
 * @param   status      : Channel status for eDMA channel
 *
 * @retval  Error codes
 *
 */
 void CFGIO_SPI_MasterEndDmaTransfer(void *statePtr, DMA_CHANNEL_STATUS_T status)
{
    CFGIO_SPI_MASTER_STATE_T *master;

    master = (CFGIO_SPI_MASTER_STATE_T *)statePtr;

    /* If there are no errors,success is recorded */
    if (status == DMA_CHANNEL_ERROR)
    {
        master->status = STATUS_ERROR;
    }
    else
    {
        master->status = STATUS_SUCCESS;
    }

    CFGIO_SPI_MasterStopTransfer(master);

    /* Call callback to announce events to the user */
    if (master->callback != NULL)
    {
        master->callback(master, SPI_EVENT_TRANSFER_COMPLETE, master->callbackParam);
    }
}


/*!
 * @brief   Calculate the address of the register used for the DMA Tx transfer
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 * @retval  uint32_t
 *
 */
 uint32_t CFGIO_SPI_MasterCalculateTxRegisterAddr(const CFGIO_SPI_MASTER_STATE_T *master)
{
    const CFGIO_T *baseAddress;
    uint32_t regAddr;
    uint8_t shifter;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    shifter = TX_SHIFTER(master->commonCFGIO.resIndex);
    if (master->firstBitOrder == CFGIO_SPI_SEND_LSB_FIRST)
    {
        regAddr = (uint32_t)(&(baseAddress->SBUF[shifter].reg));
    }
    else
    {
        regAddr = (uint32_t)(&(baseAddress->SBUFBITSWA[shifter].reg))
                + (sizeof(uint32_t) - (uint32_t)master->transferSize);
    }
    return regAddr;
}


/*!
 * @brief   Calculate the address of the register used for the DMA RX transfer
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 * @retval  uint32_t
 *
 */
 uint32_t CFGIO_SPI_MasterCalculateRxRegisterAddr(const CFGIO_SPI_MASTER_STATE_T *master)
{
    const CFGIO_T *baseAddress;
    uint32_t regAddr;
    uint8_t shifter;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    shifter = RX_SHIFTER(master->commonCFGIO.resIndex);
    if (master->firstBitOrder == CFGIO_SPI_SEND_LSB_FIRST)
    {
        regAddr = (uint32_t)(&(baseAddress->SBUF[shifter].reg))
                + (sizeof(uint32_t) - (uint32_t)master->transferSize);
    }
    else
    {
        regAddr = (uint32_t)(&(baseAddress->SBUFBITSWA[shifter].reg));
    }
    return regAddr;
}


/*!
 * @brief   Check SPI master transmission status
 *
 * @details This function can be called in the interrupt routine or
 *          directly in the polling mode before SPI transmission.
 *
 * @param   statePtr  : Pinter to state struct
 *
 */
 void CFGIO_SPI_MasterCheckStatus(void *statePtr)
{
    CFGIO_T *baseAddress;
    uint8_t index;
    CFGIO_SPI_MASTER_STATE_T *master;

    master = (CFGIO_SPI_MASTER_STATE_T *)statePtr;
    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    /* Check error */
    if (CFGIO_HW_GetShifterErrorStatus(baseAddress, TX_SHIFTER(index)))
    {
        master->status = STATUS_SPI_TX_UNDERRUN;

        /* Force transmission stop */
        CFGIO_SPI_MasterStopTransfer(master);

        /* Call callback */
        if (master->callback != NULL)
        {
            master->callback(master, SPI_EVENT_TRANSFER_COMPLETE, master->callbackParam);
        }
        return;
    }

    if (CFGIO_HW_GetShifterErrorStatus(baseAddress, RX_SHIFTER(index)))
    {
        master->status = STATUS_SPI_RX_OVERRUN;

        /* Force transmission stop */
        CFGIO_SPI_MasterStopTransfer(master);

        /* Call callback */
        if (master->callback != NULL)
        {
            master->callback(master, SPI_EVENT_TRANSFER_COMPLETE, master->callbackParam);
        }
        return;
    }

    /* Check whether data is received */
    if (CFGIO_HW_GetShifterStatus(baseAddress, RX_SHIFTER(index)))
    {
        CFGIO_SPI_ReadData(master);
    }

    /* Check if the transmitter requires more data */
    if (CFGIO_HW_GetShifterStatus(baseAddress, TX_SHIFTER(index)))
    {
        CFGIO_SPI_WriteData(master);
        if (master->txBytesRemaining == 0U)
        {
            /* RX no more data,disable tx interrupt */
            CFGIO_HW_SetShifterInterrupt(baseAddress, (uint8_t)(1U << TX_SHIFTER(index)), false);
            CFGIO_HW_SetShifterErrorInterrupt(baseAddress, (uint8_t)(1U << TX_SHIFTER(index)), false);
        }
    }

    /* Check if there is any remaining data */
    if ((master->txBytesRemaining == 0U) && (master->rxBytesRemaining == 0U))
    {
        /* If there are no errors,success is recorded */
        if (master->status == STATUS_BUSY)
        {
            master->status = STATUS_SUCCESS;
        }

        /* End transfer */
        CFGIO_SPI_MasterStopTransfer(master);

        /* Call callback */
        if (master->callback != NULL)
        {
            master->callback(master, SPI_EVENT_TRANSFER_COMPLETE, master->callbackParam);
        }
    }
}

/*!
 * @brief   Read the data received by the module
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
 void CFGIO_SPI_ReadData(CFGIO_SPI_MASTER_STATE_T *master)
{
    const CFGIO_T *baseAddress;
    uint32_t data;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    /* Reads data from the shifter buffer */
    if (master->firstBitOrder == CFGIO_SPI_SEND_LSB_FIRST)
    {
        /* For data smaller 4 bytes,our data is located in the upper part of the buffer and must be shifted */
        data = CFGIO_HW_ReadShifterBuffer(baseAddress, RX_SHIFTER(index), CFGIO_SHIFT_RW_MODE_NORMAL);
        data >>= (32U - (8U * (uint32_t)(master->transferSize)));
    }
    else
    {
        data = CFGIO_HW_ReadShifterBuffer(baseAddress, RX_SHIFTER(index), CFGIO_SHIFT_RW_MODE_BIT_SWAP);
    }

    if ((master->rxBytesRemaining > 0U) && (master->rxData != NULL))
    {
        if(master->transferSize == CFGIO_SPI_DATA_SIZE_1BYTE)
        {
            *(uint8_t *)master->rxData = (uint8_t)data;
        }
        else if(master->transferSize == CFGIO_SPI_DATA_SIZE_2BYTE)
        {
            *(uint16_t *)master->rxData = (uint16_t)data;
        }
        else if(master->transferSize == CFGIO_SPI_DATA_SIZE_4BYTE)
        {
            *(uint32_t *)master->rxData = (uint32_t)data;
        }
        /* Update the Rx buffer pointer and the remaining bytes count */
        master->rxData = &master->rxData[(master->transferSize)];
        master->rxBytesRemaining -= (uint32_t)(master->transferSize);
    }
    else
    {
        /* Nothing */
    }
}


/*!
 * @brief   Write the data transmitted by the module
 *
 * @param   master  : User defined CFGIO_SPI mastet stauts
 *
 */
 void CFGIO_SPI_WriteData(CFGIO_SPI_MASTER_STATE_T *master)
{
    uint32_t data = CFGIO_SPI_VIRTUAL_DATA;
    CFGIO_T *baseAddress;
    uint8_t index;

    baseAddress = g_cfgioBase[master->commonCFGIO.instance];
    index = master->commonCFGIO.resIndex;

    if (master->txBytesRemaining == 0U)
    {
        /* Nothing */
        return;
    }

    if ((master->txBytesRemaining > 0U) && (master->txData != NULL))
    {
        /* Read data from user buffer */
        if(master->transferSize == CFGIO_SPI_DATA_SIZE_1BYTE)
        {
            data = (uint32_t)(*(const uint8_t *)master->txData);
        }
        else if(master->transferSize == CFGIO_SPI_DATA_SIZE_2BYTE)
        {
            data = (uint32_t)(*(const uint16_t *)master->txData);
        }
        else if(master->transferSize == CFGIO_SPI_DATA_SIZE_2BYTE)
        {
            data = (uint32_t)(*(const uint32_t *)master->txData);
        }
        /* Update Tx buffer pointer and remaining bytes count */
        master->txData = &master->txData[master->transferSize];
        master->txBytesRemaining -= (uint32_t)(master->transferSize);

        /* Write data to shifter buffer */
        if (master->firstBitOrder == CFGIO_SPI_SEND_LSB_FIRST)
        {
            CFGIO_HW_WriteShifterBuffer(baseAddress, TX_SHIFTER(index), data, CFGIO_SHIFT_RW_MODE_NORMAL);
        }
        else
        {
            /* The data is shifted beford the bit swap to obtain the lower half of the bit */
            data <<= 32U - (8U * (uint32_t)(master->transferSize));
            CFGIO_HW_WriteShifterBuffer(baseAddress, TX_SHIFTER(index), data, CFGIO_SHIFT_RW_MODE_BIT_SWAP);
        }
    }
    else
    {
        /* There is nothing to send, write vritual data in the buffer */
        CFGIO_HW_WriteShifterBuffer(baseAddress, TX_SHIFTER(index), CFGIO_SPI_VIRTUAL_DATA, CFGIO_SHIFT_RW_MODE_NORMAL);
    }
}


/*!
 * @brief   Configure CFGIO module as SPI slave module
 *
 * @param   slave       : User defined CFGIO_SPI slave stauts
 * @param   configPtr   : Pointer to the CFGIO_SPI master user configuration structure.
 *
 */
 void CFGIO_SPI_SlaveConfigure(
    const CFGIO_SPI_SLAVE_STATE_T *slave,
    const CFGIO_SPI_SLAVE_CFG_T * configPtr)
{
    CFGIO_TMR_POLARITY_T rxShifterPolarity;  /* Polarity of MOSI */
    CFGIO_TMR_POLARITY_T txShifterPolarity;  /* Polarity of MISO */
    CFGIO_PIN_POLARITY_T clockPol;      /* Polarity of clock */
    CFGIO_SHIFT_START_T startBit;
    CFGIO_T *baseAddress;
    uint8_t index;

    baseAddress = g_cfgioBase[slave->commonCFGIO.instance];
    index = slave->commonCFGIO.resIndex;

    if (configPtr->clockPol == 0U)
    {
        clockPol = CFGIO_PIN_POLARITY_HIGH;
    }
    else
    {
        clockPol = CFGIO_PIN_POLARITY_LOW;
    }

    if (configPtr->clockPha == 0U)
    {
        rxShifterPolarity = CFGIO_TMR_POLARITY_POS_EDGE;
        txShifterPolarity = CFGIO_TMR_POLARITY_NEG_EDGE;
        startBit = CFGIO_SHIFT_START_BIT_DISABLED;
    }
    else
    {
        rxShifterPolarity = CFGIO_TMR_POLARITY_NEG_EDGE;
        txShifterPolarity = CFGIO_TMR_POLARITY_POS_EDGE;
        startBit = CFGIO_SHIFT_START_BIT_DISABLED_SH;
    }

    /* Configure MISO */
    CFGIO_HW_SetShifterControl(baseAddress,
                            TX_SHIFTER(index),
                            CFGIO_SHIFT_MODE_DISABLED,
                            configPtr->misoPin,
                            CFGIO_PIN_POLARITY_HIGH,
                            CFGIO_PIN_CFG_OUTPUT,
                            SCK_TIMER(index),
                            txShifterPolarity);
    CFGIO_HW_SetShifterConfig(baseAddress,
                           TX_SHIFTER(index),
                           startBit,
                           CFGIO_SHIFT_STOP_BIT_DISABLED,
                           CFGIO_SHIFT_SRC_PIN);

    /* Configure MOSI */
    CFGIO_HW_SetShifterControl(baseAddress,
                            RX_SHIFTER(index),
                            CFGIO_SHIFT_MODE_DISABLED,
                            configPtr->mosiPin,
                            CFGIO_PIN_POLARITY_HIGH,
                            CFGIO_PIN_CFG_DISABLED,
                            SCK_TIMER(index),
                            rxShifterPolarity);
    CFGIO_HW_SetShifterConfig(baseAddress,
                           RX_SHIFTER(index),
                           CFGIO_SHIFT_START_BIT_DISABLED,
                           CFGIO_SHIFT_STOP_BIT_DISABLED,
                           CFGIO_SHIFT_SRC_PIN);

    /* Configure sck timer */
    CFGIO_HW_SetTimerCompare(baseAddress, SCK_TIMER(index), 0xFFFFU);
    CFGIO_HW_SetTimerConfig(baseAddress,
                      SCK_TIMER(index),
                      CFGIO_TMR_START_BIT_DISABLED,
                      CFGIO_TMR_STOP_BIT_DISABLED,
                      CFGIO_TMR_ENABLE_TRG_POS_EDGE,  /* Enable SS pin rise edge */
                      CFGIO_TMR_DISABLE_TRG,  /* Disable SS pin fall edge */
                      CFGIO_TMR_RESET_NEVER,
                      CFGIO_TMR_DEC_PIN_SHIFT_PIN,    /* Decrement on input pin SCK */
                      CFGIO_TMR_OUTPUT_ZERO);
    CFGIO_HW_SetTimerControl(baseAddress,
                          SCK_TIMER(index),
                          (uint8_t)(configPtr->ssPin << 1U),    /* Trigger on edge of SS pin */
                          CFGIO_TRG_POLARITY_LOW,
                          CFGIO_TRG_SRC_INTERNAL,
                          configPtr->sckPin,    /* Using SCK pin as input pin */
                          clockPol,
                          CFGIO_PIN_CFG_DISABLED,
                          CFGIO_TMR_MODE_DISABLED);
}

/**@} end of group CFGIO_SPI_Functions*/
/**@} end of group CFGIO_SPI_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
