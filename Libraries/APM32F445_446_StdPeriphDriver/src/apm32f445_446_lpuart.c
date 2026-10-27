/*!
 * @file        apm32f445_446_lpuart.c
 *
 * @brief       This file provides all the LPUART firmware functions
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
#include "apm32f445_446_lpuart.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPUART_Driver LPUART Driver
  @{
*/

/** @defgroup LPUART_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/* Table of base addresses for lpuart inss */
static LPUART_T *const g_lpuartBase[LPUART_INSTANCE_CNT] =
{
    LPUART0,
    LPUART1,
    LPUART2
};

/* Table to save LPUART enum numbers defined in CMSIS files */
static const IRQn_Type g_lpuartIrqNum[LPUART_INSTANCE_CNT] =
{
    LPUART0_RxTx_IRQn,
    LPUART1_RxTx_IRQn,
    LPUART2_RxTx_IRQn
};

/* Pointer to lpuart runtime state structure */
static LPUART_STATE_T * g_lpuartStatus[LPUART_INSTANCE_CNT] = {NULL};

/* Table to save LPUART clock names as defined in clock manager */
static const CLOCK_NAMES_T g_lpuartClockNames[LPUART_INSTANCE_CNT] = LPUART_CLOCK_NAMES;

#if (LPUART_INSTANCE_CNT > 0U)
void LPUART0_RxTx_IRQHandler(void);
#endif

#if (LPUART_INSTANCE_CNT > 1U)
void LPUART1_RxTx_IRQHandler(void);
#endif

#if (LPUART_INSTANCE_CNT > 2U)
void LPUART2_RxTx_IRQHandler(void);
#endif

/* Array storing references to LPUART irq handlers */
ISR_T g_lpuartIsr[LPUART_INSTANCE_CNT] =
{
#if (LPUART_INSTANCE_CNT > 0U)
    LPUART0_RxTx_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 1U)
    LPUART1_RxTx_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 2U)
    LPUART2_RxTx_IRQHandler,
#endif
};

/**@} end of group LPUART_Variables*/

/** @defgroup LPUART_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

void LPUART_IRQHandler(uint32_t ins);
void LPUART_RxIrqHandler(uint32_t ins);
void LPUART_TxEmptyIrqHandler(uint32_t ins);
void LPUART_TxCompleteIrqHandler(uint32_t ins);
void LPUART_ErrorIrqHandler(uint32_t ins);

static STATUS_T LPUART_StartTxDataUsingInt(uint32_t ins, const uint8_t *txBuf, uint32_t txLen);
static void LPUART_FinishTxDataUsingInt(uint32_t ins);
static STATUS_T LPUART_StartRxDataUsingInt(uint32_t ins, uint8_t *rxBuf, uint32_t rxLen);
static void LPUART_FinishRxDataUsingInt(uint32_t ins);
static void LPUART_ReadData(uint32_t ins);
static void LPUART_WriteData(uint32_t ins);
static STATUS_T LPUART_StartRxDataUsingDma(uint32_t ins, uint8_t *rxBuf, uint32_t rxLen);
static STATUS_T LPUART_StartTxDataUsingDma(uint32_t ins, const uint8_t *txBuf, uint32_t txLen);
static void LPUART_StopRxDma(uint32_t ins);
static void LPUART_StopTxDma(uint32_t ins);
static void LPUART_RxDmaCallback(void *param, DMA_CHANNEL_STATUS_T status);
static void LPUART_TxDmaCallback(void *param, DMA_CHANNEL_STATUS_T status);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initialize LPUART instance
 * @details The caller provides memory for the driver state structures during
 *          initialization. The user must select the LPUART clock source in
 *          the application to initialize the LPUART.
 *
 * @param ins LPUART instance number
 * @param lpuartConfig User configuration structure
 * @param lpuartState Pointer to the LPUART driver state structure
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_ERROR Error occurred
 */
STATUS_T LPUART_Init(
    uint32_t ins,
    LPUART_STATE_T *lpuartState,
    const LPUART_CFG_T *lpuartCfg)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint32_t index;
    uint32_t srcClk;
    STATUS_T txSem;
    STATUS_T rxSem;
    LPUART_T *base = g_lpuartBase[ins];
    CLOCK_NAMES_T clkName = g_lpuartClockNames[ins];

    /* Get the LPUART clock as configured in the clock manager */
    (void)CLOCK_SYS_ReadFreq(clkName, &srcClk);

    /* Clear the state struct for this ins */
    uint8_t *clearStructPtr = (uint8_t *)lpuartState;
    for (index = 0; index < sizeof(LPUART_STATE_T); index++)
    {
        clearStructPtr[index] = 0;
    }

    /* Save runtime structure pointer */
    g_lpuartStatus[ins] = lpuartState;
    lpuartState->txDmaChannel = lpuartCfg->txDmaChannel;
    lpuartState->rxDmaChannel = lpuartCfg->rxDmaChannel;

    /* Save the transfer information for runtime retrieval */
    lpuartState->transferType = lpuartCfg->transferType;
    lpuartState->dataBits = lpuartCfg->dataBits;

        /* Initialize driver operation status */
    lpuartState->rxStatus = STATUS_SUCCESS;
    lpuartState->txStatus = STATUS_SUCCESS;

    /* Initialize the LPUART instance */
    LPUART_HW_Init(base);

    /* Initialize the parameters of the LPUART config structure */
    (void)LPUART_SetBaudrate(ins, lpuartCfg->baudrate);

    if (lpuartCfg->parityMode == LPUART_PARITY_DISABLED)
    {
        LPUART_HW_SetDataBits(base, lpuartCfg->dataBits, false);
    }
    else
    {
        LPUART_HW_SetDataBits(base, lpuartCfg->dataBits, true);
    }

    LPUART_HW_SetStopBitCnt(base, lpuartCfg->stopBits);
    LPUART_HW_SetParityMode(base, lpuartCfg->parityMode);

    /* Create the synchronization objects */
    rxSem = OSIF_SemCreate(&lpuartState->rxComplete, 0);
    txSem = OSIF_SemCreate(&lpuartState->txComplete, 0);
    if ((rxSem == STATUS_ERROR) || (txSem == STATUS_ERROR))
    {
        return STATUS_ERROR;
    }

    /* Install IRQ handler */
    INT_SYS_InstallHandler(g_lpuartIrqNum[ins], g_lpuartIsr[ins], (ISR_T*) 0);

    /* Enable LPUART interrupt */
    INT_SYS_EnableIRQ(g_lpuartIrqNum[ins]);
    return STATUS_SUCCESS;
}

/*!
 * @brief Resets LPUART by disabling interrupts and transmitter/receiver
 *
 * @param ins LPUART instance number
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_ERROR Error occurred.
 */
STATUS_T LPUART_DeInit(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LPUART_T *base = g_lpuartBase[ins];
    CLOCK_NAMES_T clkName = g_lpuartClockNames[ins];
    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    uint32_t srcClk;

    (void)CLOCK_SYS_ReadFreq(clkName, &srcClk);

    /* Wait until the data is completely shifted out of shift register */
    while (!LPUART_HW_GetStatusFlag(base, LPUART_TX_COMPLETE)) {}

    /* Destroy the synchronization objects */
    (void)OSIF_SemDestroy(&states->txComplete);
    (void)OSIF_SemDestroy(&states->rxComplete);

    /* Disable LPUART interrupt */
    INT_SYS_DisableIRQ(g_lpuartIrqNum[ins]);

    /* Restore default handler */
    INT_SYS_InstallHandler(g_lpuartIrqNum[ins], DefaultISR, (ISR_T*) 0);

    /* Clear our saved pointer to the state structure */
    g_lpuartStatus[ins] = NULL;
    return STATUS_SUCCESS;
}

/*!
 * @brief Initializes the LPUART configuration structure with default values
 *
 * @param lpuartCfg LPUART configuration structure
 *
 * @retval None
 */
void LPUART_DefaultConfig(LPUART_CFG_T *lpuartCfg)
{
    lpuartCfg->rxDmaChannel = 0U;
    lpuartCfg->txDmaChannel = 0U;
    lpuartCfg->baudrate = 9600U;
    lpuartCfg->dataBits = LPUART_8_BITS_DATA;
    lpuartCfg->parityMode = LPUART_PARITY_DISABLED;
    lpuartCfg->stopBits = LPUART_ONE_STOP_BIT;
    lpuartCfg->transferType = LPUART_USE_INTERRUPTS;
}

/*!
 * @brief Set the LPUART baudrate
 * @details This function configures the LPUART baudrate.
 *          In some LPUART instances the user must disable the transmitter/receiver
 *          before calling this function.
 *          Generally, this may be applied to all LPUARTs to ensure safe operation.
 *
 * @param ins LPUART instance number
 * @param expectedBaudrate LPUART baudrate
 *
 * @retval STATUS_BUSY if called during an on-going transfer
 *         STATUS_SUCCESS Successful
 */
STATUS_T LPUART_SetBaudrate(uint32_t ins, uint32_t expectedBaudrate)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint16_t i;
    uint16_t sbr;
    uint32_t osr;
    uint16_t tempSbr;
    uint32_t tempDiff;
    uint32_t diffBaud;
    uint32_t calBaud;
    uint32_t srcClk;
    LPUART_T *base = g_lpuartBase[ins];
    CLOCK_NAMES_T clkName = g_lpuartClockNames[ins];
    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if ((states != NULL) && (states->rxBusy || states->txBusy))
    {
        return STATUS_BUSY;
    }

    /* Get the LPUART clock */
    (void)CLOCK_SYS_ReadFreq(clkName, &srcClk);

    /**
     * This LPUART instantiation uses a slightly different baudrate calculation.
     * The idea is to use the best OSR (over-sampling rate) possible.
     * Note: osr is typically hard-set to 16 in other LPUART instantiations.
     * First calculate the baudrate using the minimum OSR possible (4).
     */
    osr = 4;
    sbr = (uint16_t)(srcClk / (expectedBaudrate * osr));
    calBaud = (srcClk / (osr * sbr));
    if (calBaud <= expectedBaudrate)
    {
        diffBaud = expectedBaudrate - calBaud;
    }
    else
    {
        diffBaud = calBaud - expectedBaudrate;
    }

    for (i = 5U; i <= 32U; i++)
    {
        /* Calculate the temporary sbr value */
        tempSbr = (uint16_t)(srcClk / (expectedBaudrate * i));

        /* Calculate the baudrate based on the temporary osr and sbr values */
        calBaud = (srcClk / (i * tempSbr));
        if (calBaud <= expectedBaudrate)
        {
            tempDiff = expectedBaudrate - calBaud;
        }
        else
        {
            tempDiff = calBaud - expectedBaudrate;
        }

        if (tempDiff <= diffBaud)
        {
            diffBaud = tempDiff;
            /* Update store the best sbr value calculated */
            sbr = tempSbr;
            /* Update and store the best osr value calculated */
            osr = i;
        }
    }

    /**
     * Check if osr is between 4x and 7x oversampling.
     * If so, then "BOTHEDGE" sampling must be turned on.
     */
    if (osr < 8U)
    {
        LPUART_HW_EnableBothEdgeSamplingCmd(base);
    }

    /* Program the osr value (bit value is one less than actual value) */
    LPUART_HW_SetOversamplingRatio(base, (osr - 1U));

    /* Write the sbr value to the BAUD registers */
    LPUART_HW_SetBaudrateDivisor(base, sbr);
    return STATUS_SUCCESS;
}

/*!
 * @brief Get the configured baudrate
 *
 * @param ins LPUART instance number
 * @param baudrate LPUART configured baudrate
 *
 * @retval None
 */
void LPUART_GetBaudrate(uint32_t ins, uint32_t *baudrate)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LPUART_T *base = g_lpuartBase[ins];
    CLOCK_NAMES_T clkName = g_lpuartClockNames[ins];
    uint32_t srcClk;
    uint16_t sbr;
    uint8_t osr;

    (void)CLOCK_SYS_ReadFreq(clkName, &srcClk);
    sbr = LPUART_HW_GetBaudrateDivisor(base);
    osr = LPUART_HW_GetOversamplingRatio(base);
    *baudrate = (srcClk / ((osr + 1UL) * sbr));
}

/*!
 * @brief   Set the internal driver reference to the rx buffer
 * @details This function can be called from the rx callback to provide the
 *          driver with a new buffer, for continuous reception.
 *
 * @param ins LPUART instance number
 * @param rxBuffer Destination buffer
 * @param rxLen The number of bytes to receive
 *
 * @retval STATUS_SUCCESS
 */
STATUS_T LPUART_SetRxBuffer(uint32_t ins, uint8_t *rxBuf, uint32_t rxLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    states->rxLen = rxLen;
    states->rxBuffer = rxBuf;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Set the internal driver reference to the tx buffer
 * @details This function can be called from the tx callback to provide the
 *          driver with a new buffer, for continuous transmission.
 *
 * @param ins LPUART instance number
 * @param txBuffer Source buffer
 * @param txLen Nhe number of bytes to send
 *
 * @retval STATUS_SUCCESS
 */
STATUS_T LPUART_SetTxBuffer(uint32_t ins, const uint8_t *txBuf, uint32_t txLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    states->txLen = txLen;
    states->txBuffer = txBuf;
    return STATUS_SUCCESS;
}

/*!
 * @brief Receive data from the LPUART module by using a blocking method
 *
 * @param ins LPUART instance number
 * @param rxBuf The data buffer pointer
 * @param rxLen Size of data need to be received in bytes
 * @param timeout Timeout in milliseconds
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_TIMEOUT if the timeout was reached
 *         STATUS_BUSY Resource is busy
 *         STATUS_UART_FRAMING_ERROR if a framing error occurred
 *         STATUS_UART_NOISE_ERROR if a noise error occurred
 *         STATUS_UART_PARITY_ERROR if a parity error occurred
 *         STATUS_UART_RX_OVERRUN Overrun error occurred
 *         STATUS_ERROR if a DMA error occurred
 */
STATUS_T LPUART_RxDataBlocking(
    uint32_t ins,
    uint8_t *rxBuf,
    uint32_t rxLen,
    uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    STATUS_T syncState;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Indicates this is a blocking transaction */
    states->rxBlocking = true;

    if (states->transferType != LPUART_USE_INTERRUPTS)
    {
        /* Start the reception process using DMA */
        result = LPUART_StartRxDataUsingDma(ins, rxBuf, rxLen);
    }
    else
    {
         /* Start the reception process using interrupts */
         result = LPUART_StartRxDataUsingInt(ins, rxBuf, rxLen);
    }

    if (STATUS_SUCCESS == result)
    {
        /* Wait until the receive is complete */
        syncState = OSIF_SemWait(&states->rxComplete, timeout);

        /* Finish the reception if timeout expired */
        if (STATUS_TIMEOUT == syncState)
        {
            states->rxStatus = STATUS_TIMEOUT;
            states->rxBlocking = false;

            if (states->transferType != LPUART_USE_INTERRUPTS)
            {
                LPUART_StopRxDma(ins);
            }
            else
            {
                LPUART_FinishRxDataUsingInt(ins);
            }
        }
    }
    return states->rxStatus;
}

/*!
 * @brief Receive bytes using polling method
 *
 * @param ins LPUART instance number
 * @param rxBuf The data buffer pointer
 * @param rxLen Size of data need to be received in bytes
 *
 * @retval  STATUS_SUCCESS Successful
 *          STATUS_BUSY Resource is busy
 *          STATUS_UART_RX_OVERRUN Overrun error occurred
 */
STATUS_T LPUART_RxDataPolling(uint32_t ins, uint8_t *rxBuf,uint32_t rxLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint8_t temp;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    STATUS_T result = STATUS_SUCCESS;
    STATUS_T tempState = STATUS_SUCCESS;

    if (states->rxBusy)
    {
        return STATUS_BUSY;
    }

    /* Enable the LPUART receiver */
    LPUART_HW_EnableRx((LPUART_T *)base);

    while (rxLen > 0U)
    {
        while (!LPUART_HW_GetStatusFlag(base, LPUART_RX_DATA_REG_FULL)) {}

        states->rxBuffer = rxBuf;
        LPUART_ReadData(ins);

        if (states->dataBits != LPUART_8_BITS_DATA)
        {
            rxLen -= 2U;
            ++rxBuf;
            ++rxBuf;
        }
        else
        {
            --rxLen;
            ++rxBuf;
        }

        /* Check errors on received data */
        if (LPUART_HW_GetStatusFlag(base, LPUART_NOISE_DETECT))
        {
            tempState = STATUS_UART_NOISE_ERROR;
            LPUART_HW_DisableRx((LPUART_T *)base);
            (void)LPUART_HW_ClearStatusFlag(base, LPUART_NOISE_DETECT);
            break;
        }
        if (LPUART_HW_GetStatusFlag(base, LPUART_FRAME_ERR))
        {
            tempState = STATUS_UART_FRAMING_ERROR;
            LPUART_HW_DisableRx((LPUART_T *)base);
            (void)LPUART_HW_ClearStatusFlag(base, LPUART_FRAME_ERR);
            break;
        }
        if (LPUART_HW_GetStatusFlag(base, LPUART_RX_OVERRUN))
        {
            tempState = STATUS_UART_RX_OVERRUN;
            LPUART_HW_DisableRx((LPUART_T *)base);
            (void)LPUART_HW_ClearStatusFlag(base, LPUART_RX_OVERRUN);
            break;
        }
        if (LPUART_HW_GetStatusFlag(base, LPUART_PARITY_ERR))
        {
            tempState = STATUS_UART_PARITY_ERROR;
            LPUART_HW_DisableRx((LPUART_T *)base);
            (void)LPUART_HW_ClearStatusFlag(base, LPUART_PARITY_ERR);
            break;
        }
    }

    /* Update received status */
    if ((tempState == STATUS_UART_RX_OVERRUN) && (rxLen == 0U))
    {
        result = STATUS_SUCCESS;
    }
    else
    {
        result = tempState;
    }

    if (result == STATUS_SUCCESS)
    {
        LPUART_HW_DisableRx((LPUART_T *)base);
    }

    /* Read dummy to clear the RDRF flag */
    LPUART_HW_RxChar(base, &temp);
    return result;
}

/*!
 * @brief Receive data using non-blocking method
 *
 * @param ins LPUART instance number
 * @param rxBuf The data buffer pointer
 * @param rxLen Size of data need to be received in bytes
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Resource is busy
 */
STATUS_T LPUART_RxData(uint32_t ins, uint8_t *rxBuf, uint32_t rxLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Indicates this is a non-blocking transaction */
    states->rxBlocking = false;

    if (states->transferType == LPUART_USE_DMA)
    {
        /* Start the reception process using DMA */
        result = LPUART_StartRxDataUsingDma(ins, rxBuf, rxLen);
    }
    else
    {
        /* Start the reception process using interrupts */
        result = LPUART_StartRxDataUsingInt(ins, rxBuf, rxLen);
    }
    return result;
}

/*!
 * @brief Returns whether the previous receive is complete
 *
 * @param ins LPUART instance number
 * @param bytesRemain   Pointer to value that is filled  with the number of bytes
 *                      that still need to be received in the active transfer.
 *                      In DMA mode, this parameter may not be accurate, in case
 *                      the transfer completes right after calling this function.
 *                      In this edge-case, the parameter will reflect the initial
 *                      transfer size, due to automatic reloading of the major loop
 *                      count in the DMA transfer descriptor.
 *
 * @retval STATUS_SUCCESS the receive has completed successfully
 *         STATUS_BUSY the receive is still in progress
 *         STATUS_UART_ABORTED The receive was aborted
 *         STATUS_TIMEOUT A timeout was reached
 *         STATUS_UART_RX_OVERRUN, STATUS_UART_FRAMING_ERROR, STATUS_UART_PARITY_ERROR,
           or STATUS_UART_NOISE_ERROR, STATUS_ERROR An error occurred during reception.
 */
STATUS_T LPUART_GetRxStatus(uint32_t ins,uint32_t *bytesRemain)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (bytesRemain != NULL)
    {
        if (!states->rxBusy)
        {
            *bytesRemain = 0;
        }
        else
        {
            /* Fill in the bytes transferred */
            if (states->transferType == LPUART_USE_DMA)
            {
                /* Remaining bytes are retrieved from the current DMA major loop count */
                *bytesRemain = DMA_ReadRemainingMajorIterationsCount(states->rxDmaChannel);
            }
            else
            {
                /* Remaining bytes are retrieved from the state structure */
                *bytesRemain = states->rxLen;
            }
        }
    }
    return states->rxStatus;
}

/*!
 * @brief Cancel a non-blocking receive
 *
 * @param ins LPUART instance number
 *
 * @retval Whether the receiving was successful or not
 */
STATUS_T LPUART_CancelRx(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (states->rxBusy)
    {
        /* Update the rx status */
        states->rxStatus = STATUS_UART_ABORTED;

        /* Stop the running transfer */
        if (states->transferType == LPUART_USE_DMA)
        {
            LPUART_StopRxDma(ins);
        }
        else
        {
            LPUART_FinishRxDataUsingInt(ins);
        }
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Sends data out through the LPUART module using a blocking method
 * @details The function does not return until the transmission is complete.
 *
 * @param ins LPUART instance number
 * @param txBuf The data buffer pointer
 * @param txLen Size of data need to be sent in bytes
 * @param timeout Timeout value in milliseconds
 *
 * @retval STATUS_SUCCESS   Successful
 *         STATUS_TIMEOUT   Timeout was reached
 *         STATUS_BUSY      Resource is busy
 *         STATUS_ERROR     Error occurred
 */
STATUS_T LPUART_TxDataBlocking(
    uint32_t ins,
    const uint8_t *txBuffer,
    uint32_t txLen,
    uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T syncState;
    STATUS_T result = STATUS_SUCCESS;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Indicates this is a blocking transaction */
    states->txBlocking = true;

    if (states->transferType == LPUART_USE_DMA)
    {
        /* Start the transmission process using DMA */
        result = LPUART_StartTxDataUsingDma(ins, txBuffer, txLen);
    }
    else
    {
        /* Start the transmission process using interrupts */
        result = LPUART_StartTxDataUsingInt(ins, txBuffer, txLen);
    }

    if (STATUS_SUCCESS == result)
    {
        /* Wait until the transmit is complete */
        syncState = OSIF_SemWait(&states->txComplete, timeout);

        /* Finish the transmission if timeout expired */
        if (STATUS_TIMEOUT ==syncState)
        {
            states->txBlocking = false;
            states->txStatus = STATUS_TIMEOUT;
            if (states->transferType == LPUART_USE_DMA)
            {
                LPUART_StopTxDma(ins);
            }
            else
            {
                LPUART_FinishTxDataUsingInt(ins);
            }
        }
    }
    return states->txStatus;
}

/*!
 * @brief Receive multiple bytes of data using polling method
 *
 * @param ins LPUART instance number
 * @param txBuf The data buffer pointer
 * @param txLen Size of data need to be sent in bytes
 *
 * @retval  STATUS_SUCCESS Successful
 *          STATUS_BUSY Resource is busy
 *          STATUS_UART_RX_OVERRUN Overrun error occurred
 */
STATUS_T LPUART_TxDataPolling(uint32_t ins, const uint8_t *txBuf, uint32_t txLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (!states->txBusy)
    {
        /* Enable the LPUART transmitter */
        LPUART_HW_EnableTx(base);

        while (txLen > 0U)
        {
            while (!LPUART_HW_GetStatusFlag(base, LPUART_TX_DATA_REG_EMPTY)) {}

            states->txBuffer = txBuf;
            LPUART_WriteData(ins);

            if (states->dataBits != LPUART_8_BITS_DATA)
            {
                ++txBuf;
                ++txBuf;
                txLen -= 2U;
            }
            else
            {
                ++txBuf;
                --txLen;
            }
        }
        /* Disable the LPUART transmitter */
        LPUART_HW_DisableTx(base);
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief Sends data using non-blocking method
 * @details This enables an async method for transmitting data. When used with
 *          a non-blocking receive, the LPUART can perform a full duplex
 *          operation. The function returns immediately. The application has to
 *          get the transmit status to know when the transmit is complete.
 *
 * @param ins LPUART instance number
 * @param txBuf The data buffer pointer
 * @param txLen Size of data need to be sent in bytes
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Resource is busy
 */
STATUS_T LPUART_TxData(uint32_t ins, const uint8_t *txBuf, uint32_t txLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    states->txBlocking = false;

    if (states->transferType == LPUART_USE_DMA)
    {
        /* Start the transmission process using DMA */
        result = LPUART_StartTxDataUsingDma(ins, txBuf, txLen);
    }
    else
    {
        /* Start transmission process using interrupts */
        result = LPUART_StartTxDataUsingInt(ins, txBuf, txLen);
    }
    return result;
}

/*!
 * @brief Returns whether the previous transmit is complete
 *
 * @param ins LPUART instance number
 * @param bytesRemain Pointer to value that is populated with the number of
 *                    bytes that have been sent in the active transfer.
 *                    Note: In DMA mode, this parameter may not be accurate,
 *                    in case the transfer completes right after calling this
 *                    function. In this edge-case, the parameter will reflect
 *                    the initial transfer size, due to automatic reloading of
 *                    the major loop count in the DMA transfer descriptor.
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY The transmit is still in progress
 *         STATUS_UART_ABORTED The transmit was aborted
 *         STATUS_TIMEOUT A timeout was reached
 *         STATUS_ERROR An error occurred
 *
 */
STATUS_T LPUART_GetTxStatus(uint32_t ins, uint32_t *bytesRemain)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (bytesRemain == NULL)
    {
        return states->txStatus;
    }

    if (!states->txBusy)
    {
        *bytesRemain = 0;
    }
    else
    {
        /* Fill in the bytes not transferred yet */
        if (states->transferType != LPUART_USE_INTERRUPTS)
        {
            /* Remaining bytes are retrieved from the current DMA major loop count */
            *bytesRemain = DMA_ReadRemainingMajorIterationsCount(states->txDmaChannel);
        }
        else
        {
            /* Remaining bytes are retrieve from the state structure */
            *bytesRemain = states->txLen;;
        }
    }
    return states->txStatus;
}

/*!
 * @brief Cancel a non-blocking transmission early
 *
 * @param ins LPUART instance number
 *
 * @retval Whether the aborting is successful or not
 */
STATUS_T LPUART_CancelTx(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (states->txBusy)
    {
        /* Stop the running transfer */
        if (states->transferType != LPUART_USE_INTERRUPTS)
        {
            LPUART_StopTxDma(ins);
        }
        else
        {
            LPUART_FinishTxDataUsingInt(ins);
        }
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Installs callback function for the LPUART receive
 * @details After a callback is installed, it bypasses part of the LPUART
 *          IRQHandler logic. Therefore, the callback needs to handle the
 *          indexes of txBuffer and txLen.
 *
 * @param ins LPUART instance number
 * @param function The LPUART receive callback function
 * @param callbackParam The LPUART receive callback parameter pointer
 *
 * @retval Former LPUART receive callback function pointer.
 */
UART_CALLBACK_T LPUART_InstallRxCallback(
    uint32_t ins,
    UART_CALLBACK_T function,
    void *callbackParam)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    UART_CALLBACK_T callback = states->rxCallback;

    states->rxCallback = function;
    states->rxCallbackParam = callbackParam;
    return callback;
}

/*!
 * @brief Installs callback function for the LPUART transmit.
 * @details After a callback is installed, it bypasses part of the LPUART
 *          IRQHandler logic. Therefore, the callback needs to handle the
 *          indexes of txBuffer and txLen.
 *
 * @param ins LPUART instance number
 * @param function The LPUART transmit callback function
 * @param callbackParam The LPUART transmit callback parameter pointer
 *
 * @retval Former LPUART transmit callback function pointer.
 */
UART_CALLBACK_T LPUART_InstallTxCallback(
    uint32_t ins,
    UART_CALLBACK_T function,
    void *callbackParam)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    UART_CALLBACK_T callback = states->txCallback;

    states->txCallback = function;
    states->txCallbackParam = callbackParam;
    return callback;
}

#if (LPUART_INSTANCE_CNT > 0U)
/* Implementation of LPUART0 handler named in startup code */
void LPUART0_RxTx_IRQHandler(void)
{
    LPUART_IRQHandler(0);
}
#endif

#if (LPUART_INSTANCE_CNT > 1U)
/* Implementation of LPUART1 handler named in startup code */
void LPUART1_RxTx_IRQHandler(void)
{
    LPUART_IRQHandler(1);
}
#endif

#if (LPUART_INSTANCE_CNT > 2U)
/* Implementation of LPUART2 handler named in startup code */
void LPUART2_RxTx_IRQHandler(void)
{
    LPUART_IRQHandler(2);
}
#endif

/*!
 * @brief Interrupt handler for LPUART
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
void LPUART_IRQHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LPUART_T *base = g_lpuartBase[ins];

    LPUART_ErrorIrqHandler(ins);

    /* Handle receive data full interrupt */
    if (  (LPUART_HW_GetIntMode(base, LPUART_INT_RX_DATA_REG_FULL))
       && (LPUART_HW_GetStatusFlag(base, LPUART_RX_DATA_REG_FULL)))
    {
        LPUART_RxIrqHandler(ins);
    }

    /* Handle transmission complete interrupt */
    if (  (LPUART_HW_GetIntMode(base, LPUART_INT_TX_COMPLETE))
       && (LPUART_HW_GetStatusFlag(base, LPUART_TX_COMPLETE)))
    {
        LPUART_TxCompleteIrqHandler(ins);
    }

    /* Handle transmitter data register empty interrupt */
    if (  (LPUART_HW_GetIntMode(base, LPUART_INT_TX_DATA_REG_EMPTY))
       && (LPUART_HW_GetStatusFlag(base, LPUART_TX_DATA_REG_EMPTY)))
    {
        LPUART_TxEmptyIrqHandler(ins);
    }
}

/*!
 * @brief Error interrupt handler for LPUART
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
void LPUART_ErrorIrqHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Handle noise error interrupt */
    if (LPUART_HW_GetStatusFlag(base, LPUART_NOISE_DETECT))
    {
        /* Update the internal status */
        states->rxStatus = STATUS_UART_NOISE_ERROR;

        if (states->transferType == LPUART_USE_DMA)
        {
            LPUART_StopRxDma(ins);
        }
        else
        {
            LPUART_FinishRxDataUsingInt(ins);
        }

        /* Invoke callback if there is one */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_ERROR,
                               states->rxCallbackParam);
        }
        LPUART_HW_ClearErrFlags(base);
    }

    /* Handle receive overrun error interrupt */
    if (LPUART_HW_GetStatusFlag(base, LPUART_RX_OVERRUN))
    {
        if (states->transferType == LPUART_USE_DMA)
        {
            LPUART_StopRxDma(ins);
        }
        else
        {
            LPUART_FinishRxDataUsingInt(ins);
        }

        /* Invoke callback if there is one */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_ERROR,
                               states->rxCallbackParam);
        }
        LPUART_HW_ClearErrFlags(base);
    }

    /* Handle framing error interrupt */
    if (LPUART_HW_GetStatusFlag(base, LPUART_FRAME_ERR))
    {
        /* Update the internal status */
        states->rxStatus = STATUS_UART_FRAMING_ERROR;

        if (states->transferType == LPUART_USE_DMA)
        {
            LPUART_StopRxDma(ins);
        }
        else
        {
            LPUART_FinishRxDataUsingInt(ins);
        }

        /* Invoke callback if there is one */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_ERROR,
                               states->rxCallbackParam);
        }
        LPUART_HW_ClearErrFlags(base);
    }

    /* Handle parity error interrupt */
    if (LPUART_HW_GetStatusFlag(base, LPUART_PARITY_ERR))
    {
        /* Update the internal status */
        states->rxStatus = STATUS_UART_PARITY_ERROR;

        if (states->transferType == LPUART_USE_DMA)
        {
            LPUART_StopRxDma(ins);
        }
        else
        {
            LPUART_FinishRxDataUsingInt(ins);
        }

        /* Invoke callback if there is one */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_ERROR,
                               states->rxCallbackParam);
        }
        LPUART_HW_ClearErrFlags(base);
    }
}

/*!
 * @brief Tx Empty interrupt handler for LPUART
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
void LPUART_TxEmptyIrqHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Check if there are any more bytes to send */
    if (states->txLen <= 0U)
    {
        return;
    }

    /* Transmit the data */
    LPUART_WriteData(ins);

    /* Update internal state */
    if (states->dataBits != LPUART_8_BITS_DATA)
    {
        states->txBuffer = &states->txBuffer[2];
        states->txLen -= 2U;
    }
    else
    {
        --states->txLen;
        ++states->txBuffer;
    }

    /* Check if this was the last byte in the current buffer */
    if (states->txLen == 0U)
    {
        /**
         * Invoke callback if there is one (callback may reset the tx buffer
         * for continuous transmission).
         */
        if (states->txCallback != NULL)
        {
            states->txCallback(states,
                               UART_EVENT_TX_BUFFER_EMPTY,
                               states->txCallbackParam);
        }

        /**
         * If there's no new data, disable tx empty interrupt and enable
         * transmission complete interrupt.
         */
        if (states->txLen == 0U)
        {
            LPUART_HW_DisableIntMode(base, LPUART_INT_TX_DATA_REG_EMPTY);
            LPUART_HW_EnableIntMode(base, LPUART_INT_TX_COMPLETE);
        }
    }
}

/*!
 * @brief Rx interrupt handler
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
void LPUART_RxIrqHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Get data and put in receive buffer  */
    LPUART_ReadData(ins);

    /* Update internal state */
    if (states->dataBits != LPUART_8_BITS_DATA)
    {
        states->rxBuffer = &states->rxBuffer[2];
        states->rxLen -= 2U;
    }
    else
    {
        --states->rxLen;
        ++states->rxBuffer;
    }

    /* Check if this was the last byte in the current buffer */
    if (states->rxLen == 0U)
    {
        /**
         * Invoke callback if there is one (callback may reset the rx buffer
         * for continuous reception).
         */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_RX_BUFFER_FULL,
                               states->rxCallbackParam);
        }
    }

    /* Finish reception if this was the last byte received */
    if (states->rxLen == 0U)
    {
        LPUART_FinishRxDataUsingInt(ins);

        /* Invoke callback if there is one */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states,
                               UART_EVENT_TRANSFER_COMPLETE,
                               states->rxCallbackParam);
        }
    }
}

/*!
 * @brief Tx Complete interrupt handler for LPUART
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
void LPUART_TxCompleteIrqHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (states->txLen != 0U)
    {
        return;
    }

    if (states->transferType == LPUART_USE_DMA)
    {
        LPUART_StopTxDma(ins);
    }
    else
    {
        LPUART_FinishTxDataUsingInt(ins);
    }

    /* Call the callback if there is one */
    if (states->txCallback != NULL)
    {
        states->txCallback(states,
                           UART_EVENT_TRANSFER_COMPLETE,
                           states->txCallbackParam);
    }
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Start a transmit process of sending data and enabling the interrupt
 *
 * @param ins LPUART instance number
 * @param txBuf Buffer containing data chars to send
 * @param txLen Number of bytes to send
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Busy status
 */
static STATUS_T LPUART_StartTxDataUsingInt(
    uint32_t ins,
    const uint8_t *txBuf,
    uint32_t txLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    LPUART_T *base = g_lpuartBase[ins];

    if (!states->txBusy)
    {
        states->txBusy = true;
        states->txStatus = STATUS_BUSY;
        states->txBuffer = txBuf;
        states->txLen = txLen;

        /* Enable the LPUART transmitter and tx empty interrupt */
        LPUART_HW_EnableTx(base);
        LPUART_HW_EnableIntMode(base, LPUART_INT_TX_DATA_REG_EMPTY);

        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief Finish the process of sending data and disabling the interrupt
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_FinishTxDataUsingInt(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    LPUART_T *base = g_lpuartBase[ins];

    if (states->txStatus != STATUS_BUSY)
    {
        LPUART_HW_DisableIntMode(base, LPUART_INT_TX_DATA_REG_EMPTY);
    }
    else
    {
        /* Transfer is completed */
        states->txStatus = STATUS_SUCCESS;
    }

    /* Disable transmission complete interrupt and transmitter */
    LPUART_HW_DisableIntMode(base, LPUART_INT_TX_COMPLETE);
    LPUART_HW_DisableTx(base);

    /* Update the internal busy flag */
    states->txBusy = false;

    /* Signal the synchronous completion object */
    if (states->txBlocking)
    {
        (void)OSIF_SemPost(&states->txComplete);
    }
}

/*!
 * @brief Start the process of receiving data and enabling the interrupt
 *
 * @param ins LPUART instance number
 * @param rxBuf Buffer containing 8-bit data chars to receive
 * @param rxLen Number of bytes to receive
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Resource is busy
 */
static STATUS_T LPUART_StartRxDataUsingInt(
    uint32_t ins,
    uint8_t *rxBuf,
    uint32_t rxLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    LPUART_T *base = g_lpuartBase[ins];

    if (!states->rxBusy)
    {
        states->rxBusy = true;
        states->rxStatus = STATUS_BUSY;
        states->rxBuffer = rxBuf;
        states->rxLen = rxLen;

        /* Enable the receiver, error interrupts, receive data full interrupt */
        LPUART_HW_EnableRx(base);
        LPUART_HW_EnableErrInterrupts(base);
        LPUART_HW_EnableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief Finish the process of receiving data and disabling the interrupt
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_FinishRxDataUsingInt(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint8_t temp;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    /* Disable receiver and error interrupts */
    LPUART_HW_DisableRx(base);
    LPUART_HW_DisableErrInterrupts(base);

    /* Read dummy to clear RDRF flag */
    LPUART_HW_RxChar(base, &temp);

    /* Disable receive data full and rx overrun interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

    /* Signal the synchronous completion object */
    if (states->rxBlocking)
    {
        (void)OSIF_SemPost(&states->rxComplete);
        states->rxBlocking = false;
    }

    states->rxBusy = false;
    if (states->rxStatus == STATUS_BUSY)
    {
        states->rxStatus = STATUS_SUCCESS;
    }
}

/*!
 * @brief Write data to the buffer register
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_WriteData(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint16_t value;
    LPUART_T *base = g_lpuartBase[ins];
    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    const uint8_t *txBuf = states->txBuffer;

    if (states->dataBits == LPUART_9_BITS_DATA)
    {
        /* Create a 16-bits integer from two bytes */
        value = (uint16_t)(*txBuf);
        ++txBuf;
        value |= (uint16_t)(((uint16_t)(*txBuf)) << 8U);
        LPUART_HW_TxChar9(base, value);
    }
    else if (states->dataBits == LPUART_8_BITS_DATA)
    {
        LPUART_HW_TxChar(base, *txBuf);
    }
    else
    {
        /* Create a 16-bits integer from two bytes */
        value = (uint16_t)(*txBuf);
        ++txBuf;
        value |= (uint16_t)(((uint16_t)(*txBuf)) << 8U);
        LPUART_HW_TxChar10(base, value);
    }
}

/*!
 * @brief Read data from the buffer register
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_ReadData(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint16_t value;
    const LPUART_T *base = g_lpuartBase[ins];
    const LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];
    uint8_t *rxBuf = states->rxBuffer;

    if (states->dataBits == LPUART_9_BITS_DATA)
    {
        LPUART_HW_RxChar9(base, &value);

        /* Write the least significant bits to the receive buffer */
        *rxBuf = (uint8_t)(value & 0xFFU);
        ++rxBuf;
        /* Write the ninth bit to the subsequent byte in the rx buffer */
        *rxBuf = (uint8_t)(value >> 8U);
    }
    else if (states->dataBits == LPUART_8_BITS_DATA)
    {
        LPUART_HW_RxChar(base, rxBuf);
    }
    else
    {
        LPUART_HW_RxChar10(base, &value);

        /* Write the least significant bits to the receive buffer */
        *rxBuf = (uint8_t)(value & 0xFFU);
        ++rxBuf;
        /* Write the ninth and tenth bits to the subsequent byte in the rx buffer */
        *rxBuf = (uint8_t)(value >> 8U);
    }
}

/*!
 * @brief Start the process of receiving data using DMA
 *
 * @param ins LPUART instance number
 * @param rxBuf Receive buffer
 * @param rxLen Number of bytes to receive
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Resource is busy
 */
static STATUS_T LPUART_StartRxDataUsingDma(
    uint32_t ins,
    uint8_t *rxBuf,
    uint32_t rxLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (!states->rxBusy)
    {
        states->rxStatus = STATUS_BUSY;
        states->rxBusy = true;
        states->rxBuffer = rxBuf;
        states->rxLen = 0U;

        (void)DMA_ConfigMultiBlockTransfer(
            states->rxDmaChannel,
            DMA_TRANSFER_PERIPH2MEM,
            (uint32_t)(&(base->DATA.reg)),
            (uint32_t)rxBuf,
            DMA_TRANSFER_SIZE_1B,
            1U,
            rxLen,
            true);

        /* Call driver function to end the reception when the DMA transfer is done */
        (void)DMA_RegisterCallback(
            states->rxDmaChannel,
            (DMA_CALLBACK_T)(LPUART_RxDmaCallback),
            (void*)(ins));

        (void)DMA_StartChannel(states->rxDmaChannel);

        /* Enable the receiver and error interrupts */
        LPUART_HW_EnableRx(base);
        LPUART_HW_EnableErrInterrupts(base);

        LPUART_HW_EnableRxDma(base);
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief Start the process of sending data using DMA transfers
 *
 * @param ins LPUART instance number
 * @param txBuf Buffer containing 8-bit data chars to send
 * @param txLen Number of bytes to send
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_BUSY Busy status
 */
static STATUS_T LPUART_StartTxDataUsingDma(
    uint32_t ins,
    const uint8_t *txBuf,
    uint32_t txLen)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (!states->txBusy)
    {
        states->txStatus = STATUS_BUSY;
        states->txBusy = true;
        states->txBuffer = txBuf;
        states->txLen = 0U;

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)DMA_ConfigMultiBlockTransfer(
            states->txDmaChannel,
            DMA_TRANSFER_MEM2PERIPH,
            (uint32_t)txBuf,
            (uint32_t)(&(base->DATA)),
            DMA_TRANSFER_SIZE_1B,
            1U,
            txLen,
            true);

        /* Call driver function to end the transmission when the DMA transfer is done */
        (void)DMA_RegisterCallback(
            states->txDmaChannel,
            (DMA_CALLBACK_T)(LPUART_TxDmaCallback),
            (void*)(ins));

        (void)DMA_StartChannel(states->txDmaChannel);

        LPUART_HW_EnableTx(base);
        LPUART_HW_EnableTxDma(base);
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief Stop DMA reception
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_StopRxDma(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint8_t temp;
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    LPUART_HW_DisableRx(base);
    LPUART_HW_DisableErrInterrupts(base);
    LPUART_HW_DisableRxDma(base);

    /* Read dummy to clear RDRF flag */
    LPUART_HW_RxChar(base, &temp);

    /* Stop the DMA channel */
    (void)DMA_StopChannel(states->rxDmaChannel);

    /* Signal the synchronous completion object */
    if (states->rxBlocking)
    {
        (void)OSIF_SemPost(&states->rxComplete);
        states->rxBlocking = false;
    }

    /* Update the internal driver status */
    if (states->rxStatus == STATUS_BUSY)
    {
        states->rxStatus = STATUS_SUCCESS;
    }
    states->rxBusy = false;
}

/*!
 * @brief Stop DMA transmission
 *
 * @param ins LPUART instance number
 *
 * @retval None
 */
static void LPUART_StopTxDma(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    LPUART_HW_DisableTxDma(base);

    /* Stop the dma channel */
    (void)DMA_StopChannel(states->txDmaChannel);

    /* Disable transmission complete interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_TX_COMPLETE);

    /* Disable transmitter */
    LPUART_HW_DisableTx(base);

    /* Signal the synchronous completion object */
    if (states->txBlocking)
    {
        (void)OSIF_SemPost(&states->txComplete);
    }

    if (states->txStatus == STATUS_BUSY)
    {
        states->txStatus = STATUS_SUCCESS;
    }
    states->txBusy = false;
}

/*!
 * @brief   DMA rx callback
 * @details Finish up a receive by completing the process of receiving data
 *          and disabling the DMA requests. This is a callback for DMA major
 *          loop completion, so it must match the DMA callback signature.
 */
static void LPUART_RxDmaCallback(void *param, DMA_CHANNEL_STATUS_T state)
{
    uint32_t ins = ((uint32_t)param);
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (state == DMA_CHANNEL_ERROR)
    {
        states->rxStatus = STATUS_ERROR;
        LPUART_StopRxDma(ins);

        /* Notify the application that an error occurred */
        if (states->rxCallback != NULL)
        {
            states->rxCallback(states, UART_EVENT_ERROR, states->rxCallbackParam);
        }
        LPUART_HW_ClearErrFlags(base);
    }

    /* Return if an error occurred; error cases are treated by the interrupt handler */
    if (states->rxStatus == STATUS_BUSY)
    {
        if (states->rxCallback != NULL)
        {
            /**
             * Allow the user to provide a new buffer inside the callback
             * to continue the reception.
             */
            states->rxCallback(states,
                               UART_EVENT_RX_BUFFER_FULL,
                               states->rxCallbackParam);
        }

        /**
         * If the callback has updated the rx buffer, update the DMA descriptor
         * to continue the transfer. Otherwise, stop the current transfer.
         */
        if (states->rxLen <= 0U)
        {
            /* Stop the reception */
            LPUART_StopRxDma(ins);

            if (states->rxCallback != NULL)
            {
                states->rxCallback(states,
                                   UART_EVENT_TRANSFER_COMPLETE,
                                   states->rxCallbackParam);
            }
            LPUART_HW_ClearErrFlags(base);
        }
        else
        {
            /* Set source address and number of minor loops (bytes to be transferred) */
            DMA_ConfigDestAddr(states->rxDmaChannel,
                               (uint32_t)(states->rxBuffer));
            DMA_ConfigMajorLoopIterationCount(states->rxDmaChannel,
                                              states->rxLen);

            /* Now the rx is set up, clear remaining bytes count */
            states->rxLen = 0U;

            /* Restart the channel */
            (void)DMA_StartChannel(states->rxDmaChannel);
        }
    }
}

/*!
 * @brief   DMA tx callback
 * @details Finish up a transmit by completing the process of sending
 *          data and disabling the DMA requests. This is a callback for DMA
 *          major loop completion, so it must match the DMA callback signature.
 *
 * @retval None
 */
static void LPUART_TxDmaCallback(void *param, DMA_CHANNEL_STATUS_T state)
{
    uint32_t ins = ((uint32_t)param);
    LPUART_T *base = g_lpuartBase[ins];
    LPUART_STATE_T *states = (LPUART_STATE_T *)g_lpuartStatus[ins];

    if (state == DMA_CHANNEL_NORMAL)
    {
        if (states->txCallback != NULL)
        {
            /* Allow the user to provide a new buffer for continuous transmission */
            states->txCallback(states,
                               UART_EVENT_TX_BUFFER_EMPTY,
                               states->txCallbackParam);
        }

        /**
         * If the callback has updated the tx buffer, update the DMA descriptor
         * to continue the transfer. Otherwise, stop the current transfer.
         */
        if (states->txLen <= 0U)
        {
            /* Enable transmission complete interrupt */
            LPUART_HW_EnableIntMode(base, LPUART_INT_TX_COMPLETE);
        }
        else
        {
            /* Set the source address and the number of minor loops (bytes to be transferred) */
            DMA_ConfigSrcAddr(states->txDmaChannel,
                              (uint32_t)(states->txBuffer));
            DMA_ConfigMajorLoopIterationCount(states->txDmaChannel,
                                              states->txLen);

            /* The tx is set up, clear remaining bytes count */
            states->txLen = 0U;

            /* Re-start the channel */
            (void)DMA_StartChannel(states->txDmaChannel);
        }
    }
    else
    {
        /* MA transfer completed with errors */
        states->txStatus = STATUS_ERROR;
        LPUART_StopTxDma(ins);

        /* Notify the application that an error occurred */
        if (states->txCallback != NULL)
        {
            states->txCallback(states,
                               UART_EVENT_ERROR,
                                states->txCallbackParam);
        }
    }
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Config the LPUART controller
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_Init(LPUART_T *base)
{
    /* Set the default oversampling ratio (16) and baud-rate divider (4) */
    base->BR.reg = ((uint32_t)(((0x0FUL) << 24u) | ((0x04UL) << 0u)));

    /* Clear the error/interrupt flags */
    base->STS.reg = (0xC01FC000U);
    base->CTRL.reg = 0x00000000;
    base->MADDR.reg = 0x00000000;
    base->MIR.reg = 0x00000000;
    /* Reset FIFO feature */
    base->FIFO.reg = (0x0003C000U);
    base->WMKR.reg = 0x00000000;
}

/*!
 * @brief Enable the LPUART transmitter
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_EnableTx(LPUART_T *base)
{
    base->CTRL.bit.TXEN = LPUART_CTRL_TXEN_1;
    while(base->CTRL.bit.TXEN != LPUART_CTRL_TXEN_1) {}
}

/*!
 * @brief Disable the LPUART transmitter
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_DisableTx(LPUART_T *base)
{
    base->CTRL.bit.TXEN = LPUART_CTRL_TXEN_0;
    while(base->CTRL.bit.TXEN != LPUART_CTRL_TXEN_0) {}
}

/*!
 * @brief Enable the LPUART receiver
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_EnableRx(LPUART_T *base)
{
    base->CTRL.bit.RXEN = LPUART_CTRL_RXEN_1;
    while(base->CTRL.bit.RXEN != LPUART_CTRL_RXEN_1) {}
}

/*!
 * @brief Disable the LPUART receiver
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_DisableRx(LPUART_T *base)
{
    base->CTRL.bit.RXEN = LPUART_CTRL_RXEN_0;
    while(base->CTRL.bit.RXEN != LPUART_CTRL_RXEN_0) {}
}

/*!
 * @brief Assign the LPUART baudrate modulo divisor
 *
 * @param base LPUART base pointer
 * @param divisor The baudrate modulo division "SBR"
 *
 * @retval None
 *
 */
void LPUART_HW_SetBaudrateDivisor(LPUART_T *base, uint32_t divisor)
{
    base->BR.bit.BRMD = divisor & 0x1FFFU;
}

/*!
 * @brief Read the LPUART baudrate modulo divisor
 *
 * @param base LPUART base pointer
 *
 * @retval The baudrate modulo division "SBR"
 *
 */
uint16_t LPUART_HW_GetBaudrateDivisor(const LPUART_T *base)
{
    return ((uint16_t)base->BR.bit.BRMD);
}

/*!
 * @brief Assign the LPUART baudrate oversampling ratio
 *
 * @param base LPUART base pointer
 * @param ratio The oversampling ratio "OSR"
 *
 * @retval None
 *
 */
void LPUART_HW_SetOversamplingRatio(LPUART_T *base, uint32_t ratio)
{
    base->BR.bit.OSRCFG = ratio;
}

/*!
 * @brief Read the LPUART baudrate oversampling ratio
 *
 * @param base LPUART base pointer
 *
 * @retval The oversampling ratio "OSR"
 *
 */
uint8_t LPUART_HW_GetOversamplingRatio(const LPUART_T *base)
{
    return ((uint8_t)(base->BR.bit.OSRCFG));
}

/*!
 * @brief Configures the LPUART baudrate both edge sampling
 *
 * @param base LPUART base pointer
 * @param enable   Enable (1) or Disable (0) Both Edge Sampling
 *
 * @retval None
 */
void LPUART_HW_EnableBothEdgeSamplingCmd(LPUART_T *base)
{
    base->BR.bit.BEDSMPSEL = LPUART_BR_BEDSMPSEL_1;
}

/*!
 * @brief Configures the data bits in the LPUART controller
 *
 * @param base LPUART base pointer
 * @param dataBits Number of bits per char (8, 9, or 10)
 * @param parity Specifies whether parity bit is enabled
 *
 * @retval None
 *
 */
void LPUART_HW_SetDataBits(
    LPUART_T *base,
    LPUART_BIT_COUNT_PER_CHAR_T dataBits,
    bool parity)
{
    uint32_t tempDataBits = (uint32_t)dataBits;

    if (parity)
    {
        tempDataBits += 1U;
    }

    if (tempDataBits == (uint32_t)LPUART_10_BITS_DATA)
    {
        base->BR.bit.SELM10 = LPUART_BR_SELM10_1;
    }
    else
    {
        /* config 8-bit (M=0) or 9-bits (M=1) */
        base->CTRL.reg = (base->CTRL.reg & ~(0x10U)) | (tempDataBits << 4U);

        /* clear M10 to make sure not 10-bit mode */
        base->BR.bit.SELM10 = LPUART_BR_SELM10_0;
    }
}

/*!
 * @brief Configures parity mode in the LPUART controller
 *
 * @param base LPUART base pointer
 * @param parity Parity mode (enabled, disable, odd, even)
 *
 * @retval None
 *
 */
void LPUART_HW_SetParityMode(LPUART_T *base, LPUART_PARITY_MODE_T parity)
{
    base->CTRL.reg = (base->CTRL.reg & ~(0x2U)) | (((uint32_t)parity >> 1U) << 1U);
    base->CTRL.reg = (base->CTRL.reg & ~(0x1U)) | (((uint32_t)parity & 1U) << 0U);
}

/*!
 * @brief Configures the number of stop bits in the LPUART controller
 *
 * @param base LPUART base pointer
 * @param stopBits Number of stop bits (1 or 2)
 *
 * @retval None
 *
 */
void LPUART_HW_SetStopBitCnt(LPUART_T *base, LPUART_STOP_BIT_COUNT_T stopBits)
{
    base->BR.bit.STOPBSEL = stopBits;
}

/*!
 * @brief Enable the LPUART module interrupts
 *
 * @param base LPUART module base pointer
 * @param intSrc LPUART interrupt configuration data
 *
 * @retval None
 *
 */
void LPUART_HW_EnableIntMode(LPUART_T *base, LPUART_INTERRUPT_T intSrc)
{
    uint32_t eventId = (uint32_t)(intSrc) >> LPUART_SHIFT;
    uint32_t intRegShift = (uint16_t)(intSrc);

    if (eventId == LPUART_REG_ID_BAUDRATE)
    {
        base->BR.reg = (base->BR.reg & ~(1UL << intRegShift))
                     | (1U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_CONTROL)
    {
        base->CTRL.reg = (base->CTRL.reg & ~(1UL << intRegShift))
                       | (1U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_MODEM)
    {
        base->MIR.reg = (base->MIR.reg & ~(1UL << intRegShift))
                      | (1U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_FIFO)
    {
       base->FIFO.reg = (base->FIFO.reg & (~(0x00030000U) & ~(1UL << intRegShift)))
                      | (1U << intRegShift);
    }
}

/*!
 * @brief Disable the LPUART module interrupts
 *
 * @param base LPUART module base pointer
 * @param intSrc LPUART interrupt configuration data
 *
 * @retval None
 *
 */
void LPUART_HW_DisableIntMode(LPUART_T *base, LPUART_INTERRUPT_T intSrc)
{
    uint32_t eventId = (uint32_t)(intSrc) >> LPUART_SHIFT;
    uint32_t intRegShift = (uint16_t)(intSrc);

    if (eventId == LPUART_REG_ID_BAUDRATE)
    {
        base->BR.reg = (base->BR.reg & ~(1UL << intRegShift))
                     | (0U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_CONTROL)
    {
        base->CTRL.reg = (base->CTRL.reg & ~(1UL << intRegShift))
                       | (0U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_MODEM)
    {
        base->MIR.reg = (base->MIR.reg & ~(1UL << intRegShift))
                      | (0U << intRegShift);
    }
    else if (eventId == LPUART_REG_ID_FIFO)
    {
       base->FIFO.reg = (base->FIFO.reg & (~0x00030000U & ~(1UL << intRegShift)))
                      | (0U << intRegShift);
    }
}

/*!
 * @brief Returns LPUART module interrupts state
 *
 * @param base LPUART module base pointer
 * @param intSrc LPUART interrupt configuration data
 *
 * @retval  true: enable
 *          false: disable
 *
 */
bool LPUART_HW_GetIntMode(const LPUART_T *base, LPUART_INTERRUPT_T intSrc)
{
    uint32_t eventId = (uint32_t)(intSrc) >> LPUART_SHIFT;
    bool result = false;

    if (eventId == LPUART_REG_ID_BAUDRATE)
    {
        result = (((base->BR.reg >> (uint16_t)(intSrc)) & 1U) > 0U);
    }
    else if (eventId == LPUART_REG_ID_CONTROL)
    {
        result = (((base->CTRL.reg >> (uint16_t)(intSrc)) & 1U) > 0U);
    }
    else if (eventId == LPUART_REG_ID_MODEM)
    {
        result = (((base->MIR.reg >> (uint16_t)(intSrc)) & 1U) > 0U);
    }
    else if (eventId == LPUART_REG_ID_FIFO)
    {
        result = (((base->FIFO.reg >> (uint16_t)(intSrc)) & 1U) > 0U);
    }

    return result;
}

/*!
 * @brief Sends the LPUART 8-bit character
 *
 * @param base LPUART instance
 * @param data Data to send (8-bit)
 *
 * @retval None
 */
void LPUART_HW_TxChar(LPUART_T *base, uint8_t data)
{
    volatile uint8_t * dataReg = (volatile uint8_t *)(&(base->DATA.reg));
    dataReg[0] = data;
}

/*!
 * @brief Sends the LPUART 9-bit character
 *
 * @param base LPUART Instance
 * @param data Data to send (9-bit)
 *
 * @retval None
 *
 */
void LPUART_HW_TxChar9(LPUART_T *base, uint16_t data)
{
    uint8_t ninthDataBit;
    volatile uint8_t * dataReg = (volatile uint8_t *)(&(base->DATA.reg));

    ninthDataBit = (uint8_t)((data >> 8U) & 0x1U);

    /* Write to ninth data bit T8 (where T[0:7]=8-bits, T8=9th bit) */
    base->CTRL.reg = (base->CTRL.reg & ~0x40000000U)
                   | ((uint32_t)(ninthDataBit) << 30U);

    /* Write 8-bits to the data register */
    dataReg[0] = (uint8_t)data;
}

/*!
 * @brief Sends the LPUART 10-bit character
 *
 * @param base LPUART Instance
 * @param data Data to send (10-bit)
 *
 * @retval None
 */
void LPUART_HW_TxChar10(LPUART_T *base, uint16_t data)
{
    uint8_t ninthDataBit;
    uint8_t tenthDataBit;
    uint32_t ctrlRegValue;
    volatile uint8_t * dataReg = (volatile uint8_t *)(&(base->DATA.reg));

    ninthDataBit = (uint8_t)((data >> 8U) & 0x1U);
    tenthDataBit = (uint8_t)((data >> 9U) & 0x1U);

    /* Write to ninth/tenth data bit (T[0:7]=8-bits, T8=9th bit, T9=10th bit) */
    ctrlRegValue = base->CTRL.reg;
    ctrlRegValue = (ctrlRegValue & ~0x40000000U) | ((uint32_t)ninthDataBit << 30U);
    ctrlRegValue = (ctrlRegValue & ~0x80000000U) | ((uint32_t)tenthDataBit << 31U);
    base->CTRL.reg = ctrlRegValue;

    /* Write 8-bits to the data register */
    dataReg[0] = (uint8_t)data;
}

/*!
 * @brief Read the LPUART 8-bit character
 *
 * @param base LPUART base pointer
 * @param readData Data read from receive (8-bit)
 *
 * @retval None
 */
void LPUART_HW_RxChar(const LPUART_T *base, uint8_t *readData)
{
    *readData = (uint8_t)base->DATA.reg;
}

/*!
 * @brief Read the LPUART 9-bit character
 *
 * @param base LPUART base pointer
 * @param readData Data read from receive (9-bit)
 *
 * @retval None
 */
void LPUART_HW_RxChar9(const LPUART_T *base, uint16_t *readData)
{
    /* Get ninth bit from lpuart data register */
    *readData = (uint16_t)(((base->CTRL.reg >> 31U) & 1U) << 8);

    /* Get 8-bit data from the lpuart data register */
    *readData |= (uint8_t)base->DATA.reg;
}

/*!
 * @brief Read the LPUART 10-bit character
 *
 * @param base LPUART base pointer
 * @param readData Data read from receive (10-bit)
 *
 * @retval None
 */
void LPUART_HW_RxChar10(const LPUART_T *base, uint16_t *readData)
{
    /* Get tenth data bit */
    *readData = (uint16_t)(((base->CTRL.reg >> 30U) & 1U) << 9);

    /* Get ninth data bit */
    *readData |= (uint16_t)(((base->CTRL.reg >> 31U) & 1U) << 8);

    /* Get 8-bit data */
    *readData |= (uint8_t)base->DATA.reg;
}

/*!
 * @brief  LPUART get status flag
 *
 * @param base LPUART base pointer
 * @param status The status flag to query
 *
 * @retval Whether the current status flag is set (true) or not (false)
 *
 */
bool LPUART_HW_GetStatusFlag(const LPUART_T *base, LPUART_STATUS_FLG_T status)
{
    uint32_t stsId = (uint32_t)(status) >> LPUART_SHIFT;
    bool result = false;

    if (stsId == LPUART_REG_ID_STATUS)
    {
        result = (((base->STS.reg >> (uint16_t)(status)) & 1U) > 0U);
    }
    else if (stsId == LPUART_REG_ID_DATA)
    {
        result = (((base->DATA.reg >> (uint16_t)(status)) & 1U) > 0U);
    }
    else if (stsId == LPUART_REG_ID_MODEM)
    {
        result = (((base->MIR.reg >> (uint16_t)(status)) & 1U) > 0U);
    }
    else if (stsId == LPUART_REG_ID_FIFO)
    {
        result = (((base->FIFO.reg >> (uint16_t)(status)) & 1U) > 0U);
    }

    return result;
}

/*!
 * @brief LPUART clears an individual status flag
 *
 * @param base LPUART base pointer
 * @param status Desired LPUART status flag to clear
 *
 * @retval STATUS_SUCCESS Successful
 *         STATUS_ERROR Error occurred
 *
 */
STATUS_T LPUART_HW_ClearStatusFlag(LPUART_T *base,LPUART_STATUS_FLG_T status)
{
    STATUS_T result = STATUS_SUCCESS;

    /**
     * These flags are cleared automatically by other lpuart operations
     * and cannot be manually cleared, return error code.
     */
    if (status == LPUART_TX_DATA_REG_EMPTY)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_TX_COMPLETE)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_RX_DATA_REG_FULL)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_RX_ACTIVE)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_IDLE_LINE_DETECT)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x100000U;
    }
    else if (status == LPUART_RX_OVERRUN)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x80000U;
    }
    else if (status == LPUART_NOISE_DETECT)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x40000U;
    }
    else if (status == LPUART_FRAME_ERR)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x20000U;
    }
    else if (status == LPUART_PARITY_ERR)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x10000U;
    }
    else if (status == LPUART_LIN_BREAK_DETECT)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x80000000U;
    }
    else if (status == LPUART_RX_ACTIVE_EDGE_DETECT)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x40000000U;
    }
    else if (status == LPUART_NOISE_IN_CURRENT_WORD)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_PARITY_ERR_IN_CURRENT_WORD)
    {
        result = STATUS_ERROR;
    }
    else if (status == LPUART_MATCH_ADDR_ONE)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x8000U;
    }
    else if (status == LPUART_MATCH_ADDR_TWO)
    {
        base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | 0x4000U;
    }
    if (status == LPUART_FIFO_TX_OVERFLOW)
    {
        base->FIFO.reg = (base->FIFO.reg & (~(0x00030000U))) | 0x20000U;
    }
    else if (status == LPUART_FIFO_RX_UNDERFLOW)
    {
        base->FIFO.reg = (base->FIFO.reg & (~(0x00030000U))) | 0x10000U;
    }
    else
    {
        result = STATUS_ERROR;
    }
    return result;
}

/*!
 * @brief Enable the LPUART error interrupts
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_EnableErrInterrupts(LPUART_T *base)
{
    LPUART_HW_EnableIntMode(base, LPUART_INT_RX_OVERRUN);
    LPUART_HW_EnableIntMode(base, LPUART_INT_PARITY_ERR_FLAG);
    LPUART_HW_EnableIntMode(base, LPUART_INT_NOISE_ERR_FLAG);
    LPUART_HW_EnableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);
}

/*!
 * @brief Disable the LPUART error interrupts
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_DisableErrInterrupts(LPUART_T *base)
{
    LPUART_HW_DisableIntMode(base, LPUART_INT_RX_OVERRUN);
    LPUART_HW_DisableIntMode(base, LPUART_INT_PARITY_ERR_FLAG);
    LPUART_HW_DisableIntMode(base, LPUART_INT_NOISE_ERR_FLAG);
    LPUART_HW_DisableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);
}

/*!
 * @brief Clears the error flags treated by the driver
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_ClearErrFlags(LPUART_T *base)
{
    uint32_t mark = 0x80000U | 0x40000U | 0x20000U | 0x10000U;
    base->STS.reg = (base->STS.reg & (~(0xC01FC000U))) | mark;
}

/*!
 * @brief Returns whether the receive data is inverted or not
 * @details This function returns the polarity of the receive data.
 *
 * @param base LPUART base pointer
 *
 * @retval true: Rx data polarity inverted
 *         false: Not inverted
 */
bool LPUART_HW_GetRxDataPolarity(const LPUART_T *base)
{
    return ((base->STS.bit.RXDIPINV) > 0);
}

/*!
 * @brief Enable receive data invert
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_EnableRxDataPolarity(LPUART_T *base)
{
    base->STS.bit.RXDIPINV = LPUART_STS_RXDIPINV_1;
}

/*!
 * @brief Disable receive data invert
 *
 * @param base LPUART base pointer
 *
 * @retval None
 *
 */
void LPUART_HW_DisableRxDataPolarity(LPUART_T *base)
{
    base->STS.bit.RXDIPINV = LPUART_STS_RXDIPINV_0;
}

/*!
 * @brief  LPUART break character transmit length configuration
 * @details This function configures the break char length.
 *          In some LPUART instances, the user should disable the transmitter
 *          before calling this function. Generally, this may be applied to
 *          all LPUARTs to ensure safe operation.
 *
 * @param base LPUART base pointer
 * @param length LPUART break character length setting
 *               0 - minimum 10-bit times (default)
 *               1 - minimum 13-bit times
 *
 * @retval None
 */
void LPUART_HW_SetBreakCharTxLen(LPUART_T *base, LPUART_BRK_CHAR_LEN_T length)
{
    base->STS.bit.BCTXLEN = length;
}

/*!
 * @brief  LPUART break character detect length configuration
 *
 * @param base LPUART base pointer
 * @param length LPUART break character length setting
 *               0 - minimum 10-bit times (default)
 *               1 - minimum 13-bit times
 *
 * @retval None
 */
void LPUART_HW_SetBreakCharDetectLen(LPUART_T *base, LPUART_BRK_CHAR_LEN_T length)
{
    base->STS.bit.LBDEN = length;
}

/*!
 * @brief  LPUART transmit sends break character configuration
 * @details This function sets break character transmission in queue mode.
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_QueueBreakField(LPUART_T *base)
{
    base->DATA.bit.FETXSC = LPUART_DATA_FETXSC_1;
}

/*!
 * @brief Enable DMA requests for LPUART Transmitter
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_EnableTxDma(LPUART_T *base)
{
    base->BR.bit.TXDMAREN = LPUART_BR_TXDMAREN_1;
}

/*!
 * @brief Disable DMA requests for LPUART Transmitter
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_DisableTxDma(LPUART_T *base)
{
    base->BR.bit.TXDMAREN = LPUART_BR_TXDMAREN_0;
}

/*!
 * @brief Enable DMA requests for LPUART Receiver
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_EnableRxDma(LPUART_T *base)
{
    base->BR.bit.RXFDMAREN = LPUART_BR_RXFDMAREN_1;
}

/*!
 * @brief Disable DMA requests for LPUART Receiver
 *
 * @param base LPUART base pointer
 *
 * @retval None
 */
void LPUART_HW_DisableRxDma(LPUART_T *base)
{
    base->BR.bit.RXFDMAREN = LPUART_BR_RXFDMAREN_0;
}

/**@} end of group LPUART_Functions*/
/**@} end of group LPUART_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
