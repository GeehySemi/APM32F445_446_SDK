/*!
 * @file        apm32f445_446_cfgio_uart.c
 *
 * @brief       CFGIO_UART driver functions
 *
 * @version     V1.0.0
 *
 * @date        2026-01-31
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
#include "apm32f445_446_cfgio_uart.h"


/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_UART_Driver CFGIO_UART Driver
  @{
*/

/** @defgroup CFGIO_UART_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
/**
 * Shifters/Timers used for UART simulation. Parameter x means the index value
 * of the driver instance.
 */
#define TIMER_INDEX_RX(x)       (x)
#define TIMER_INDEX_TX(x)       (x)
#define SHIFTER_INDEX_RX(x)     (x)
#define SHIFTER_INDEX_TX(x)     (x)

/* Used for baudrate calculation */
#define MIN_DIV     0
#define MAX_DIV     0xFF

/**@} end of group CFGIO_UART_Macros*/

/** @defgroup CFGIO_UART_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

static void CFGIO_UART_ConfigureRx(
    CFGIO_UART_STATE_T *states,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    uint32_t clockInput);

static void CFGIO_UART_ConfigureTx(
    CFGIO_UART_STATE_T *states,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    uint32_t clockInput);

static void CFGIO_UART_CalculateBaudrateDivider(
    uint32_t baudrate,
    uint16_t *divider,
    uint32_t clockInput);

static void CFGIO_UART_CheckStatus(void *statePtr);
static void CFGIO_UART_CheckRxStatus(void *statePtr);
static void CFGIO_UART_CheckTxStatus(void *statePtr);
static void CFGIO_UART_ReadData(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_WriteData(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_EnableTransfer(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_StopTransfer(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_EndTransfer(CFGIO_UART_STATE_T *states);

static STATUS_T CFGIO_UART_WaitTransferComplete(
    CFGIO_UART_STATE_T *states,
    uint32_t timeout);

static void CFGIO_UART_StartDmaRxTransfer(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_StartDmaTxTransfer(CFGIO_UART_STATE_T *states);
static void CFGIO_UART_EndDmaRxTransfer(void *statePtr, DMA_CHANNEL_STATUS_T status);
static void CFGIO_UART_EndDmaTxTransfer(void *statePtr, DMA_CHANNEL_STATUS_T status);
static uint32_t CFGIO_UART_CalcDmaRxRegAddr(const CFGIO_UART_STATE_T *states);
static uint32_t CFGIO_UART_CalcDmaTxRegAddr(const CFGIO_UART_STATE_T *states);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the CFGIO_UART driver
 *
 * @param instance  CFGIO peripheral instance number
 * @param configPtr Pointer to CFGIO_UART user configuration structure.
 *                  The function reads configuration data from this structure
 *                  and initializes the driver accordingly. The application may
 *                  free this structure after the function returns.
 * @param states    Pointer to CFGIO_UART driver state structure
 *                  The driver uses this memory area for its internal logic.
 *                  The application must make no assumptions about the content
 *                  of this structure, and must not free this memory until the
 *                  driver is reset using CFGIO_UART_Deinit().
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_Init(
    uint32_t instance,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    CFGIO_UART_STATE_T *states)
{
    STATUS_T result;
    uint32_t clockInput;
    uint8_t dmaRequest;

    /* Get protocol clock frequency */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[instance], &clockInput);

    /* Inform the resource allocator that we need one shifter/timer */
    states->commonState.resCount = 1U;

    /* Common CFGIO driver initialization */
    result = CFGIO_InitDriver(instance, &(states->commonState));
    if (result != STATUS_SUCCESS)
    {
        return result;
    }

    /* Initialize the semaphore */
    if (configPtr->transferType != CFGIO_USE_POLLING)
    {
        (void)OSIF_SemCreate(&(states->sem), 0U);
    }

    /* Initialize driver internal states */
    states->transferType = configPtr->transferType;
    states->dir = configPtr->direction;
    states->numBits = configPtr->numBits;
    states->callback = configPtr->callback;
    states->callbackParam = configPtr->callbackParam;
    states->driverStatus = STATUS_SUCCESS;
    states->isIdle = true;
    states->isBlocking = false;
    states->txBuffer = NULL;
    states->rxBuffer = NULL;
    states->bytesLeft = 0U;

    /* Configure device for Rx or Tx */
    if (states->dir == CFGIO_UART_DIR_RX)
    {
        CFGIO_UART_ConfigureRx(states, configPtr, clockInput);
    }
    else
    {
        CFGIO_UART_ConfigureTx(states, configPtr, clockInput);
    }

    /* Configure the transfer engine */
    if (states->transferType == CFGIO_USE_DMA)
    {
        /* Store DMA channel number */
        states->dmaChannel = configPtr->dmaChannel;

        /* Configure DMA request source */
        dmaRequest = g_cfgioDmaSrc[instance][SHIFTER_INDEX_TX(states->commonState.resIndex)];
        DMA_ConfigChannelRequestAndTrigger(configPtr->dmaChannel, dmaRequest, false);

        /* For Tx we will need interrupt to signal end of transfer */
        if (states->dir == CFGIO_UART_DIR_TX)
        {
            states->commonState.isr = CFGIO_UART_CheckTxStatus;
        }
    }
    else if (states->transferType == CFGIO_USE_INTERRUPTS)
    {
        if (states->dir == CFGIO_UART_DIR_RX)
        {
            states->commonState.isr = CFGIO_UART_CheckRxStatus;
        }
        else
        {
            states->commonState.isr = CFGIO_UART_CheckTxStatus;
        }
    }
    else
    {
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief   De-initialize the CFGIO_UART driver
 *
 * @param   states Pointer to CFGIO_UART driver state structure
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_Deinit(CFGIO_UART_STATE_T *states)
{
    STATUS_T result = STATUS_BUSY;

    if (states->isIdle)
    {
        if (states->transferType != CFGIO_USE_POLLING)
        {
            (void)OSIF_SemDestroy(&(states->sem));
        }
        result = CFGIO_DeinitDriver(&(states->commonState));
    }
    return result;
}

/*!
 * @brief   Get default configuration for CFGIO_UART
 *
 * @param   configPtr Pointer to CFGIO_UART user configuration structure
 */
void CFGIO_UART_DefaultConfig(CFGIO_UART_USER_CONFIG_T *configPtr)
{
    configPtr->cfgioPin = 0U;
    configPtr->baudrate = 115200U;
    configPtr->numBits = 8U;
    configPtr->direction = CFGIO_UART_DIR_TX;
    configPtr->transferType = CFGIO_USE_INTERRUPTS;
    configPtr->dmaChannel = 255U;
    configPtr->callback = NULL;
    configPtr->callbackParam = NULL;
}

/*!
 * @brief   Set the baudrate and bit width
 * @details This function sets the baudrate and bit width for the UART driver.
 *          Note that due to module limitation not any baudrate can be achieved.
 *          The driver will set a baudrate as close as possible to the requested
 *          baudrate, but there may still be substantial differences, for example
 *          if requesting a high baudrate while using a low frequency CFGIO
 *          clock. The application should call CFGIO_UART_GetBaudrate() after
 *          CFGIO_UART_SetBaudrate() to check what baudrate was actually set.
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param baudrate  Desired baudrate
 * @param numBits  Number of bits per word
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_SetBaudrate(
    CFGIO_UART_STATE_T *states,
    uint32_t baudrate,
    uint8_t numBits)
{
    STATUS_T result = STATUS_BUSY;
    uint32_t clockInput;
    uint16_t div;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    if (states->isIdle)
    {
        /* Get the protocol clock frequency */
        CLOCK_SYS_ReadFreq(g_cfgioClockSrc[states->commonState.instance],
                           &clockInput);

        /* Calculate divider */
        CFGIO_UART_CalculateBaudrateDivider(baudrate, &div, clockInput);

        /* Configure Tx/Rx timer */
        CFGIO_HW_SetTimerCompare(
            base,
            TIMER_INDEX_TX(index),
            (uint16_t)((uint16_t)(((uint16_t)((uint16_t)numBits << 1U) - 1U) << 8U) + div));

        states->numBits = numBits;
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief   Get the current configured baudrate
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param baudrate  The current baudrate
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_GetBaudrate(CFGIO_UART_STATE_T *states, uint32_t *baudrate)
{
    uint16_t timerCompare;
    uint16_t divider;
    uint32_t clockInput;
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Get protocol clock frequency and currently configured divider */
    CLOCK_SYS_ReadFreq(g_cfgioClockSrc[states->commonState.instance], &clockInput);
    timerCompare = CFGIO_HW_GetTimerCompare(base, TIMER_INDEX_TX(index));
    divider = (uint16_t)(timerCompare & 0x00FFU);

    /**
     * Calculate baudrate: ClockInput / (2 * (divider + 1))
     * Round to nearest integer.
     */
    *baudrate = (clockInput + (uint32_t)divider + 1U)
              / (2U * ((uint32_t)divider + 1U));

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform blocking UART reception
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param rxBuffer  Pointer to the data buffer
 * @param rxLength  Length in bytes of the data to be received
 * @param timeout   Timeout in milliseconds
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_RxDataBlocking(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength,
    uint32_t timeout)
{
    STATUS_T result = STATUS_BUSY;

    if (states->isIdle)
    {
        /* Mark the transfer as blocking */
        if (states->transferType != CFGIO_USE_POLLING)
        {
            states->isBlocking = true;

            /* Dummy wait to ensure the semaphore is 0 */
            (void)OSIF_SemWait(&(states->sem), 0);
        }

        /**
         * Call CFGIO_UART_RxDataNonBlocking() to start transfer. We already
         * did the busy check so no need to check return code.
         */
        (void)CFGIO_UART_RxDataNonBlocking(states, rxBuffer, rxLength);

        /* Wait until the transfer is completed */
        result = CFGIO_UART_WaitTransferComplete(states, timeout);
    }
    return result;
}

/*!
 * @brief Perform non-blocking UART reception
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param txBuffer  Pointer to the data buffer
 * @param txLength  Length in bytes of the data to be received
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_RxDataNonBlocking(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;
    STATUS_T result = STATUS_BUSY;

    if (states->isIdle)
    {
        states->isIdle = false;
        states->driverStatus = STATUS_BUSY;
        states->rxBuffer = rxBuffer;
        states->bytesLeft = rxLength;

        /* Enable the shifters and timers */
        CFGIO_UART_EnableTransfer(states);

        /* Enable the transfer engine */
        if (states->transferType == CFGIO_USE_DMA)
        {
            CFGIO_UART_StartDmaRxTransfer(states);
        }
        else if (states->transferType == CFGIO_USE_POLLING)
        {
            CFGIO_UART_CheckStatus(states);
        }
        else if (states->transferType ==  CFGIO_USE_INTERRUPTS)
        {
            /* Enable the interrupts for Rx shifter */
            CFGIO_HW_SetShifterInterrupt(
                base, (uint8_t)(1U << SHIFTER_INDEX_RX(index)), true);
            CFGIO_HW_SetShifterErrorInterrupt(
                base, (uint8_t)(1U << SHIFTER_INDEX_RX(index)), true);
        }
        else
        {
        }
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief   Set the buffer for receiving data
 * @details This function can be used to provide a new buffer for receiving
 *          data to the driver. It can be called from the user callback when
 *          the event UART_EVENT_RX_BUFFER_FULL is reported. This way the
 *          reception will continue without interruption.
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param rxBuffer  Pointer to the data buffer
 * @param rxLength  Length in bytes of the data buffer
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_SetRxBuffer(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength)
{
    states->rxBuffer = rxBuffer;
    states->bytesLeft = rxLength;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Perform blocking UART transmission
 * @details This function sends a block of data and returns when the
 *          transmission is complete.
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param txBuffer  Pointer to the data buffer
 * @param txLength  Length in bytes of the data to be transferred
 * @param timeout   Timeout in milliseconds
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_TxDataBlocking(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
    uint32_t txLength,
    uint32_t timeout)
{
    STATUS_T result = STATUS_BUSY;

    if (states->isIdle)
    {
        /* Mark the transfer as blocking */
        if (states->transferType != CFGIO_USE_POLLING)
        {
            states->isBlocking = true;

            /* Dummy wait to ensure the semaphore is 0 */
            (void)OSIF_SemWait(&(states->sem), 0);
        }

        /**
         * Call CFGIO_UART_TxDataNonBlocking() to start transfer. We already
         * did the busy check so no need to check return code.
         */
        (void)CFGIO_UART_TxDataNonBlocking(states, txBuffer, txLength);

        /* Wait until the transfer is completed */
        result = CFGIO_UART_WaitTransferComplete(states, timeout);
    }
    return result;
}

/*!
 * @brief   Perform non-blocking UART transmission
 * @details This function sends a block of data and returns immediately.
 *          The rest of the transmission is handled by the interrupt service
 *          routine (if the driver is initialized in interrupt mode) or by the
 *          CFGIO_UART_GetStatus() function (if the driver is
 *          initialized in polling mode).
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param txBuffer  Pointer to the data buffer
 * @param txLength  Length in bytes of the data to be transferred
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_TxDataNonBlocking(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
    uint32_t txLength)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;
    STATUS_T result = STATUS_BUSY;

    if (states->isIdle)
    {
        states->isIdle = false;
        states->driverStatus = STATUS_BUSY;
        states->txBuffer = txBuffer;
        states->bytesLeft = txLength;

        /**
         * Number of bytes to flush after the last byte is copied in the
         * Tx shifter buffer.
         */
        states->endBytes = (uint8_t)((txLength == 1U) ? 1U : 2U);

        /* Enable the shifters and timers */
        CFGIO_UART_EnableTransfer(states);

        /* Start transfer */
        if (states->transferType == CFGIO_USE_DMA)
        {
            CFGIO_UART_StartDmaTxTransfer(states);
        }
        else if (states->transferType == CFGIO_USE_POLLING)
        {
            /* Send the first byte */
            CFGIO_UART_CheckStatus(states);
        }
        else if (states->transferType == CFGIO_USE_INTERRUPTS)
        {
            /* Enable the interrupts for Tx shifter */
            CFGIO_HW_SetShifterInterrupt(
                base, (uint8_t)(1U << SHIFTER_INDEX_TX(index)), true);
            CFGIO_HW_SetShifterErrorInterrupt(
                base, (uint8_t)(1U << SHIFTER_INDEX_TX(index)), true);
        }
        else
        {
        }
        result = STATUS_SUCCESS;
    }
    return result;
}

/*!
 * @brief   Set the buffer for transmitting data
 * @details This function can be used to provide a new buffer for transmitting
 *          data to the driver. It can be called from the user callback when
 *          event UART_EVENT_TX_BUFFER_EMPTY is reported. This way the
 *          transmission will continue without interruption.
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param txBuffer  Pointer to the data buffer
 * @param txLength  Length in bytes of the data buffer
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_SetTxBuffer(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
     uint32_t txLength)
{
    /* There are always 2 bytes to flush in case of continuous transmission */
    states->endBytes = 2;
    states->txBuffer = txBuffer;
    states->bytesLeft = txLength;
    return STATUS_SUCCESS;
}

/*!
 * @brief Abort a non-blocking UART transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_AbortTransfer(CFGIO_UART_STATE_T *states)
{
    if (!states->isIdle)
    {
        states->driverStatus = STATUS_UART_ABORTED;
        CFGIO_UART_StopTransfer(states);
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief   Get the status of the current non-blocking transfer
 * @details This function returns the current status of a non-blocking UART
 *          transfer. A return code of STATUS_BUSY means the transfer is still
 *          in progress. Otherwise the function returns a status reflecting the
 *          outcome of the last transfer. When the driver is initialized in
 *          polling mode this function also advances the transfer by checking
 *          and handling the transmit and receive events, so it must be called
 *          frequently to avoid overflows or underflows.
 *
 * @param states        Pointer to CFGIO_UART driver state structure
 * @param bytesLeft     The remaining number of bytes to be transferred.
 *                      In DMA mode, this parameter may not be accurate, in
 *                      case the transfer completes right after calling this
 *                      function. In this edge-case, the parameter will
 *                      reflect the initial transfer size, due to automatic
 *                      reloading of the major loop count in the DMA transfer
 *                      descriptor.
 *
 * @retval  Error status
 */
STATUS_T CFGIO_UART_GetStatus(CFGIO_UART_STATE_T *states, uint32_t *bytesLeft)
{
    uint32_t bytesCount = states->bytesLeft;

    if (!states->isIdle)
    {
        if (states->transferType == CFGIO_USE_DMA)
        {
            /**
             * In DMA mode just update the remaining count.
             * DO NOT write states->bytesLeft directly!
             */
            bytesCount = DMA_ReadRemainingMajorIterationsCount(states->dmaChannel);
        }
        else if (states->transferType == CFGIO_USE_POLLING)
        {
            /* In polling mode advance the UART transfer here */
            CFGIO_UART_CheckStatus(states);
        }
        else
        {
        }
    }

    if (bytesLeft != NULL)
    {
        *bytesLeft = bytesCount;
    }
    return states->isIdle ? states->driverStatus : STATUS_BUSY;
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configure for UART Rx
 *
 * @param states        Pointer to CFGIO_UART driver state structure
 * @param configPtr     Pointer to CFGIO_UART user configuration structure
 * @param clockInput    Input clock
 *
 * @retval  None
 */
static void CFGIO_UART_ConfigureRx(
    CFGIO_UART_STATE_T *states,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    uint32_t clockInput)
{
    uint16_t dataBits;
    uint16_t divider;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    CFGIO_UART_CalculateBaudrateDivider(
        configPtr->baudrate, &divider, clockInput);
    dataBits = configPtr->numBits;

    /* Configure the Rx shifter */
    CFGIO_HW_SetShifterConfig(
        base,
        SHIFTER_INDEX_RX(index),
        CFGIO_SHIFT_START_BIT_0,
        CFGIO_SHIFT_STOP_BIT_1,
        CFGIO_SHIFT_SRC_PIN);
    CFGIO_HW_SetShifterControl(
        base,
        SHIFTER_INDEX_RX(index),
        CFGIO_SHIFT_MODE_DISABLED,
        configPtr->cfgioPin,
        CFGIO_PIN_POLARITY_HIGH,
        CFGIO_PIN_CFG_DISABLED,
        TIMER_INDEX_RX(index),
        CFGIO_TMR_POLARITY_NEG_EDGE);

    /* Configure the Rx timer */
    CFGIO_HW_SetTimerCompare(
        base,
        TIMER_INDEX_RX(index),
        (uint16_t)((((uint16_t)(dataBits << 1U) - 1U) << 8U) + divider));

    CFGIO_HW_SetTimerConfig(
        base,
        TIMER_INDEX_RX(index),
        CFGIO_TMR_START_BIT_ENABLED,
        CFGIO_TMR_STOP_BIT_TIM_DIS,
        CFGIO_TMR_ENABLE_PIN_POS_EDGE,
        CFGIO_TMR_DISABLE_TIM_CMP,
        CFGIO_TMR_RESET_PIN_RISING,
        CFGIO_TMR_DEC_CLK_SHIFT_TMR,    /* Decrement on CFGIO clock */
        CFGIO_TMR_OUTPUT_ONE_RESET);
    CFGIO_HW_SetTimerControl(
        base,
        TIMER_INDEX_RX(index),
        0U,                              /* Trigger unused */
        CFGIO_TRG_POLARITY_HIGH,
        CFGIO_TRG_SRC_EXTERNAL,
        configPtr->cfgioPin,           /* Input from Rx pin */
        CFGIO_PIN_POLARITY_LOW,
        CFGIO_PIN_CFG_DISABLED,
        CFGIO_TMR_MODE_DISABLED);
}

/*!
 * @brief Configure for UART Tx
 *
 * @param states        Pointer to CFGIO_UART driver state structure
 * @param configPtr     Pointer to CFGIO_UART user configuration structure
 * @param clockInput    Input clock
 *
 * @retval  None
 */
static void CFGIO_UART_ConfigureTx(
    CFGIO_UART_STATE_T *states,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    uint32_t clockInput)
{
    uint16_t dataBits;
    uint16_t divider;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    CFGIO_UART_CalculateBaudrateDivider(
        configPtr->baudrate, &divider, clockInput);
    dataBits = configPtr->numBits;

    /* Configure Tx shifter */
    CFGIO_HW_SetShifterConfig(
        base,
        SHIFTER_INDEX_TX(index),
        CFGIO_SHIFT_START_BIT_0,
        CFGIO_SHIFT_STOP_BIT_1,
        CFGIO_SHIFT_SRC_PIN);
    CFGIO_HW_SetShifterControl(
        base,
        SHIFTER_INDEX_TX(index),
        CFGIO_SHIFT_MODE_TRANSMIT,
        configPtr->cfgioPin,          /* Output on Tx pin */
        CFGIO_PIN_POLARITY_HIGH,
        CFGIO_PIN_CFG_OUTPUT,
        TIMER_INDEX_TX(index),
        CFGIO_TMR_POLARITY_POS_EDGE);

    /* Configure Tx timer */
    CFGIO_HW_SetTimerCompare(
        base,
        TIMER_INDEX_TX(index),
        (uint16_t)((((uint16_t)(dataBits << 1U) - 1U) << 8U) + divider));

    CFGIO_HW_SetTimerConfig(
        base,
        TIMER_INDEX_TX(index),
        CFGIO_TMR_START_BIT_ENABLED,
        CFGIO_TMR_STOP_BIT_TIM_DIS,
        CFGIO_TMR_ENABLE_TRG_HIGH,    /* Enable when Tx data is available */
        CFGIO_TMR_DISABLE_TIM_CMP,
        CFGIO_TMR_RESET_NEVER,
        CFGIO_TMR_DEC_CLK_SHIFT_TMR,  /* Decrement on CFGIO clock */
        CFGIO_TMR_OUTPUT_ONE);
    CFGIO_HW_SetTimerControl(
        base,
        TIMER_INDEX_TX(index),
        /* Trigger on Tx shifter status flag */
        (uint8_t)((uint8_t)(SHIFTER_INDEX_TX(index) << 2U) + 1U),
        CFGIO_TRG_POLARITY_LOW,
        CFGIO_TRG_SRC_INTERNAL,
        0U,     /* Pin unused */
        CFGIO_PIN_POLARITY_HIGH,
        CFGIO_PIN_CFG_DISABLED,
        CFGIO_TMR_MODE_DISABLED);
}

/*!
 * @brief   Calculate the baudrate divider for a target baudrate
 *
 * @param baudrate      Baudrate
 * @param divider       Divider
 * @param clockInput    Input clock
 *
 * @retval  None
 */
static void CFGIO_UART_CalculateBaudrateDivider(
    uint32_t baudrate,
    uint16_t *divider,
    uint32_t clockInput)
{
    /**
     * Calculate divider: ((ClockInput / baudrate) / 2) - 1
     * Round to nearest integer.
     */
    int32_t tmpDivider = (((int32_t)clockInput + (int32_t)baudrate)
                           / (2 * (int32_t)baudrate)) - 1;

    if (tmpDivider > MAX_DIV)
    {
        tmpDivider = MAX_DIV;
    }
    if (tmpDivider < MIN_DIV)
    {
        tmpDivider = MIN_DIV;
    }
    *divider = (uint16_t)tmpDivider;
}

/*!
 * @brief   Check status of the UART transfer
 * @details This function can be called either in an interrupt routine or
 *          directly in polling mode to advance the transfer.
 *
 * @param   statesPtr   Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_CheckStatus(void *statePtr)
{
    const CFGIO_UART_STATE_T *states = (CFGIO_UART_STATE_T *)statePtr;

    if (states->dir == CFGIO_UART_DIR_RX)
    {
        CFGIO_UART_CheckRxStatus(statePtr);
    }
    else
    {
        CFGIO_UART_CheckTxStatus(statePtr);
    }
}

/*!
 * @brief   Check status of the UART reception
 * @details This function can be called either in an interrupt routine or
 *          directly in polling mode to advance the transfer.
 *
 * @param   statesPtr   Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_CheckRxStatus(void *statePtr)
{
    CFGIO_UART_STATE_T *states = (CFGIO_UART_STATE_T *)statePtr;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    if (CFGIO_HW_GetShifterErrorStatus(base, SHIFTER_INDEX_RX(index)))
    {
        /* Shifter error */
        states->driverStatus = STATUS_UART_RX_OVERRUN;
        CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_RX(index));
        states->bytesLeft = 0U;
    }
    else if (CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_RX(index)))
    {
        /* Data received */
        CFGIO_UART_ReadData(states);

        /* Buffer is full, call the callback to allow user to provide a new buffer */
        if ((states->callback != NULL) && (states->bytesLeft == 0U))
        {
            states->callback(states,
                             UART_EVENT_RX_BUFFER_FULL,
                             states->callbackParam);
        }
    }
    else
    {
    }

    if (states->bytesLeft == 0U)
    {
        /* Transfer is completed, record success if there was no error */
        if (states->driverStatus == STATUS_BUSY)
        {
            states->driverStatus = STATUS_SUCCESS;
        }

        /* Discard any leftover Rx data and end the transfer */
        CFGIO_HW_ClearShifterStatus(base, SHIFTER_INDEX_RX(index));
        CFGIO_UART_StopTransfer(states);

        /* Notify the transfer complete event to the user */
        if (states->callback != NULL)
        {
            states->callback(states,
                             UART_EVENT_TRANSFER_COMPLETE,
                             states->callbackParam);
        }
    }
}

/*!
 * @brief   Check status of the UART transmission
 * @details This function can be called either in an interrupt routine or
 *          directly in polling mode to advance the transfer.
 *
 * @param   statesPtr   Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_CheckTxStatus(void *statePtr)
{
    CFGIO_UART_STATE_T *states = (CFGIO_UART_STATE_T *)statePtr;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    if (  (states->bytesLeft > 0U)
       && CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_TX(index)))
    {
        /* There are more data, send it now. */
        CFGIO_UART_WriteData(states);

        /* Now the buffer is empty, call callback to allow user to provide a new buffer */
        if ((states->callback != NULL) && (states->bytesLeft == 0U))
        {
            states->callback(states,
                             UART_EVENT_TX_BUFFER_EMPTY,
                             states->callbackParam);
        }

        /**
         * The transmission will stop after the last bytes are sent. The timer
         * event will indicate the send is complete.
         */
        if (states->bytesLeft == 0U)
        {
            CFGIO_HW_ClearTimerStatus(base, TIMER_INDEX_TX(index));

            if (states->transferType == CFGIO_USE_INTERRUPTS)
            {
                /* Transmission is completed, disable interrupt */
                CFGIO_HW_SetShifterInterrupt(
                    base, (uint8_t)(1U << SHIFTER_INDEX_TX(index)), false);

                /* Enable the timer interrupt to ensure that transfer is completed */
                CFGIO_HW_SetTimerInterrupts(
                    base, (uint8_t)(1U << TIMER_INDEX_TX(index)), true);
            }
        }
    }
    else if (  (states->bytesLeft == 0U)
            && (CFGIO_HW_GetTimerStatus(base, TIMER_INDEX_TX(index))))
    {
        /**
         * Transfer is completed, clear timer status. No need to check for Tx
         * underflow since the timer is controlled by the shifter status flag.
         */
        CFGIO_HW_ClearTimerStatus(base, TIMER_INDEX_TX(index));

        states->endBytes--;
        if (states->endBytes == 0U)
        {
            if (states->driverStatus == STATUS_BUSY)
            {
                states->driverStatus = STATUS_SUCCESS;
            }

            /* Done flushing the Tx buffer, stop the transfer. */
            CFGIO_UART_StopTransfer(states);

            /* Notify the end transfer event to the user */
            if (states->callback != NULL)
            {
                states->callback(states,
                                 UART_EVENT_TRANSFER_COMPLETE,
                                 states->callbackParam);
            }
        }
        else if (CFGIO_HW_GetShifterStatus(base, SHIFTER_INDEX_TX(index)))
        {
            /**
             * The last byte was already transferred from buffer to shifter.
             * There is a danger that the transmission is over and we end up
             * never reporting the end event. To avoid this, send one extra
             * dummy byte. Set start bit to 1 and send 0xFF byte, this way the
             * line will appear idle.
             */
            CFGIO_HW_SetShifterStartBit(base,
                                          SHIFTER_INDEX_TX(index),
                                          CFGIO_SHIFT_START_BIT_1);
            CFGIO_HW_WriteShifterBuffer(base,
                                          SHIFTER_INDEX_TX(index),
                                          0xFFFFFFFFUL,
                                          CFGIO_SHIFT_RW_MODE_NORMAL);
        }
        else
        {
            /**
             * The last byte was not yet transferred from buffer to shifter.
             * No need to do anything, just wait for the next timer event.
             */
        }
    }
    else
    {
    }
}

/*!
 * @brief Read received data
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_ReadData(CFGIO_UART_STATE_T *states)
{
    uint32_t rxValue;
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Read data from the shifter buffer */
    rxValue = CFGIO_HW_ReadShifterBuffer(base,
                                           SHIFTER_INDEX_RX(index),
                                           CFGIO_SHIFT_RW_MODE_NORMAL);
    rxValue >>= 32U - (uint32_t)(states->numBits);

    if (states->numBits > 8U)
    {
        /* More than 8 bits per word, 2 bytes are needed */
        *(uint16_t *)states->rxBuffer = (uint16_t)rxValue;

        /* Update the Rx buffer pointer and remaining bytes */
        states->rxBuffer = &states->rxBuffer[2U];
        states->bytesLeft -= 2U;

    }
    else
    {
        *(uint8_t *)states->rxBuffer = (uint8_t)rxValue;

        /* Update the Rx buffer pointer and remaining bytes */
        states->rxBuffer ++;
        states->bytesLeft -= 1U;
    }
}

/*!
 * @brief Write data to be transmitted
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_WriteData(CFGIO_UART_STATE_T *states)
{
    uint32_t txValue;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Read data from user buffer, update Tx buffer pointer and remaining bytes */
    if (states->numBits > 8U)
    {
        /* More than 8 bits per word, 2 bytes are needed */
        txValue = (uint32_t)(*(uint16_t *)states->txBuffer);
        states->txBuffer = &states->txBuffer[2U];
        states->bytesLeft -= 2U;
    }
    else
    {
        txValue = (uint32_t)(*(uint8_t *)states->txBuffer);
        states->txBuffer++;
        states->bytesLeft -= 1U;

    }

    CFGIO_HW_WriteShifterBuffer(base,
                                  SHIFTER_INDEX_TX(index),
                                  txValue,
                                  CFGIO_SHIFT_RW_MODE_NORMAL);
}

/*!
 * @brief Enable timers and shifters to start a transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_EnableTransfer(CFGIO_UART_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Enable the shifters and timers */
    if (states->dir == CFGIO_UART_DIR_RX)
    {
        /* Discard any leftover Rx data */
        CFGIO_HW_ClearShifterStatus(base, SHIFTER_INDEX_RX(index));
        CFGIO_HW_SetShifterMode(base,
                                  SHIFTER_INDEX_RX(index),
                                  CFGIO_SHIFT_MODE_RECEIVE);
    }
    CFGIO_HW_SetTimerMode(base,
                            TIMER_INDEX_TX(index),
                            CFGIO_TMR_MODE_8BIT_BAUD);
}

/*!
 * @brief Force stop the current transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_StopTransfer(CFGIO_UART_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Disable and re-enable shifters and timers to reset them */
    CFGIO_HW_SetTimerMode(base,
                            TIMER_INDEX_TX(index),
                            CFGIO_TMR_MODE_DISABLED);
    CFGIO_HW_SetShifterMode(base,
                              SHIFTER_INDEX_TX(index),
                              CFGIO_SHIFT_MODE_DISABLED);

    /* Clear any leftover error flags and end the transfer */
    CFGIO_HW_ClearShifterErrorStatus(base, SHIFTER_INDEX_TX(index));
    CFGIO_UART_EndTransfer(states);

    /* Re-enable shifter for Tx, to ensure correct idle state */
    if (states->dir == CFGIO_UART_DIR_TX)
    {
        /* Restore start bit in case it was changed for end of transmission detection */
        CFGIO_HW_SetShifterStartBit(base,
                                      SHIFTER_INDEX_TX(index),
                                      CFGIO_SHIFT_START_BIT_0);
        CFGIO_HW_SetShifterMode(base,
                                  SHIFTER_INDEX_TX(index),
                                  CFGIO_SHIFT_MODE_TRANSMIT);
    }
}

/*!
 * @brief End the current transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_EndTransfer(CFGIO_UART_STATE_T *states)
{
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Disable the transfer engine */
    if (states->transferType == CFGIO_USE_DMA)
    {
        /* For Tx we need to disable timer interrupt */
        CFGIO_HW_SetTimerInterrupts(base,
                                      (uint8_t)(1U << TIMER_INDEX_TX(index)),
                                      false);

        /* Stop DMA channel */
        DMA_StopChannel(states->dmaChannel);

        /* Disable CFGIO DMA request */
        CFGIO_HW_SetShifterDmaRequest(base,
                                        (uint8_t)(1U << SHIFTER_INDEX_TX(index)),
                                        false);
    }
    else if (states->transferType ==  CFGIO_USE_INTERRUPTS)
    {
        /* Disable interrupts for Rx/Tx shifter */
        CFGIO_HW_SetShifterInterrupt(base,
                                       (uint8_t)(1U << SHIFTER_INDEX_TX(index)),
                                       false);
        CFGIO_HW_SetShifterErrorInterrupt(base,
                                            (uint8_t)(1U << SHIFTER_INDEX_TX(index)),
                                            false);

        /* Disable timer interrupt */
        CFGIO_HW_SetTimerInterrupts(base,
                                      (uint8_t)(1U << TIMER_INDEX_TX(index)),
                                      false);
    }
    else
    {
    }

    states->bytesLeft = 0U;
    states->isIdle = true;

    if (states->isBlocking)
    {
        /* Signal transfer end for blocking transfer */
        (void)OSIF_SemPost(&(states->sem));
    }
}

/*!
 * @brief   Wait for the end of a blocking transfer
 *
 * @param states    Pointer to CFGIO_UART driver state structure
 * @param timeout   Timeout for the transfer in milliseconds
 *
 * @retval  Error status
 */
static STATUS_T CFGIO_UART_WaitTransferComplete(
    CFGIO_UART_STATE_T *states,
    uint32_t timeout)
{
    STATUS_T osResult = STATUS_SUCCESS;

    if (states->transferType == CFGIO_USE_DMA)
    {
        /* Wait until transfer completion to be signaled by the DMA or IRQ */
        osResult = OSIF_SemWait(&(states->sem), timeout);
    }
    else if (states->transferType == CFGIO_USE_POLLING)
    {
        /* Call CFGIO_UART_GetStatus() to do the transfer */
        while (CFGIO_UART_GetStatus(states, NULL) == STATUS_BUSY) {}
    }
    else if (states->transferType == CFGIO_USE_INTERRUPTS)
    {
        /* Wait until transfer to be completed by the IRQ */
        osResult = OSIF_SemWait(&(states->sem), timeout);
    }
    else
    {
    }

    /* Blocking transfer is completed */
    states->isBlocking = false;

    if (osResult == STATUS_TIMEOUT)
    {
        /* Timeout, stop the current transfer */
        states->driverStatus = STATUS_TIMEOUT;
        CFGIO_UART_StopTransfer(states);
    }
    return states->driverStatus;
}

/*!
 * @brief Start DMA Rx transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_StartDmaRxTransfer(CFGIO_UART_STATE_T *states)
{
    uint32_t numBytes;
    DMA_TRANSFER_SIZE_T transferSize;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Configure the transfer control descriptor for the previously allocated channel */
    if (states->numBits > 8U)
    {
        numBytes = 2U;
        transferSize = DMA_TRANSFER_SIZE_2B;
    }
    else
    {
        numBytes = 1U;
        transferSize = DMA_TRANSFER_SIZE_1B;
    }

    DMA_ConfigMultiBlockTransfer(
        states->dmaChannel,
        DMA_TRANSFER_PERIPH2MEM,
        CFGIO_UART_CalcDmaRxRegAddr(states),
        (uint32_t)(states->rxBuffer),
        transferSize,
        numBytes,
        states->bytesLeft / numBytes,
        true);

    /* Now the transfer is set up, clear remaining bytes count */
    states->bytesLeft = 0U;

    /* Setup callback for DMA Rx transfer end */
    DMA_RegisterCallback(
        states->dmaChannel,
        (DMA_CALLBACK_T)(CFGIO_UART_EndDmaRxTransfer),
        (void*)(states));

    /* Start Tx DMA channel */
    DMA_StartChannel(states->dmaChannel);

    /* Enable CFGIO DMA requests */
    CFGIO_HW_SetShifterDmaRequest(
        base,
        (uint8_t)(1U << SHIFTER_INDEX_RX(index)),
        true);
}

/*!
 * @brief Start DMA Tx transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static void CFGIO_UART_StartDmaTxTransfer(CFGIO_UART_STATE_T *states)
{
    uint32_t numBytes;
    DMA_TRANSFER_SIZE_T transferSize;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t index = states->commonState.resIndex;

    /* Configure the transfer control descriptor for the previously allocated channel */
    if (states->numBits > 8U)
    {
        numBytes = 2U;
        transferSize = DMA_TRANSFER_SIZE_2B;
    }
    else
    {
        numBytes = 1U;
        transferSize = DMA_TRANSFER_SIZE_1B;
    }

    DMA_ConfigMultiBlockTransfer(
        states->dmaChannel,
        DMA_TRANSFER_MEM2PERIPH,
        (uint32_t)(states->txBuffer),
        CFGIO_UART_CalcDmaTxRegAddr(states),
        transferSize,
        numBytes,
        states->bytesLeft / numBytes,
        true);

    /* Now the transfer is set up, clear remaining bytes count */
    states->bytesLeft = 0U;

    /* Setup callback for DMA Tx transfer end */
    DMA_RegisterCallback(
        states->dmaChannel,
        (DMA_CALLBACK_T)(CFGIO_UART_EndDmaTxTransfer),
        (void*)(states));

    /* Start Tx DMA channel */
    DMA_StartChannel(states->dmaChannel);

    /* Enable CFGIO DMA requests */
    CFGIO_HW_SetShifterDmaRequest(
        base,
        (uint8_t)(1U << SHIFTER_INDEX_TX(index)),
        true);
}

/*!
 * @brief This function is called at the end of a DMA Rx transfer
 *
 * @param statesPtr Pointer to CFGIO_UART driver state structure
 * @param dmaStatus DMA channel status
 *
 * @retval  None
 */
static void CFGIO_UART_EndDmaRxTransfer(void *statePtr, DMA_CHANNEL_STATUS_T dmaStatus)
{
    uint32_t numBytes;
    uint8_t dmaChannel;
    CFGIO_UART_STATE_T *states = (CFGIO_UART_STATE_T *)statePtr;

    if (dmaStatus != DMA_CHANNEL_ERROR)
    {
        /* Call the callback to allow user to provide a new buffer */
        if (states->callback != NULL)
        {
            states->callback(states,
                             UART_EVENT_RX_BUFFER_FULL,
                             states->callbackParam);
        }
    }
    else
    {
        states->driverStatus = STATUS_ERROR;
    }

    if (states->bytesLeft != 0U)
    {
        /* More data to transfer, restart DMA channel. Update buffer address and size. */
        dmaChannel = states->dmaChannel;
        numBytes = (states->numBits > 8U) ? 2U : 1U;

        DMA_ConfigDestAddr(dmaChannel, (uint32_t)(states->rxBuffer));
        DMA_ConfigMajorLoopIterationCount(
            dmaChannel,
            states->bytesLeft / numBytes);

        /* Now Tx is set up, clear remaining bytes count. */
        states->bytesLeft = 0U;

        /* Start the channel */
        DMA_StartChannel(dmaChannel);
    }
    else
    {
        if (states->driverStatus == STATUS_BUSY)
        {
            states->driverStatus = STATUS_SUCCESS;
        }

        /* No more data to transmit, reception stop now. */
        CFGIO_UART_StopTransfer(states);

        /* Notify the event to the user */
        if (states->callback != NULL)
        {
            states->callback(states,
                             UART_EVENT_TRANSFER_COMPLETE,
                             states->callbackParam);
        }
    }
}

/*!
 * @brief This function is called at the end of a DMA Tx transfer
 *
 * @param statesPtr Pointer to CFGIO_UART driver state structure
 * @param dmaStatus DMA channel status
 *
 * @retval  None
 */
static void CFGIO_UART_EndDmaTxTransfer(void *statePtr, DMA_CHANNEL_STATUS_T dmaStatus)
{
    uint32_t numBytes;
    uint8_t dmaChannel;
    CFGIO_UART_STATE_T *states = (CFGIO_UART_STATE_T *)statePtr;
    CFGIO_T *base = g_cfgioBase[states->commonState.instance];

    if (dmaStatus != DMA_CHANNEL_ERROR)
    {
        /* Call the callback to allow user to provide a new buffer */
        if (states->callback != NULL)
        {
            states->callback(states,
                             UART_EVENT_TX_BUFFER_EMPTY,
                             states->callbackParam);
        }

        if (states->bytesLeft != 0U)
        {
            /* More data to transfer, restart DMA channel. Update buffer address and size. */
            dmaChannel = states->dmaChannel;
            numBytes = (states->numBits > 8U) ? 2U : 1U;

            DMA_ConfigSrcAddr(dmaChannel, (uint32_t)(states->txBuffer));
            DMA_ConfigMajorLoopIterationCount(
                dmaChannel,
                states->bytesLeft / numBytes);

            /* Now Tx is set up, clear remaining bytes count. */
            states->bytesLeft = 0U;

            /* Start the channel */
            DMA_StartChannel(dmaChannel);
        }
        else
        {
            /**
             * No more data to transmit, transmission will stop. Enable timer
             * interrupt to let the IRQ to ensure that transfer is completed.
             */
            CFGIO_HW_ClearTimerStatus(
                base,
                TIMER_INDEX_TX(states->commonState.resIndex));
            CFGIO_HW_SetTimerInterrupts(
                base,
                (uint8_t)(1U << TIMER_INDEX_TX(states->commonState.resIndex)),
                true);
        }
    }
    else
    {
        states->driverStatus = STATUS_ERROR;

        /* DMA error, stop the transfer. */
        CFGIO_UART_StopTransfer(states);

        /* Notify the end transfer event to the user */
        if (states->callback != NULL)
        {
            states->callback(states,
                             UART_EVENT_TRANSFER_COMPLETE,
                             states->callbackParam);
        }
    }
}

/*!
 * @brief Calculate the address of the register used for DMA Rx transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static uint32_t CFGIO_UART_CalcDmaRxRegAddr(const CFGIO_UART_STATE_T *states)
{
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint32_t numBytes = (states->numBits > 8U) ? 2U : 1U;
    uint8_t shifterIndex = SHIFTER_INDEX_RX(states->commonState.resIndex);

    uint32_t regAddr = (uint32_t)(&(base->SBUF[shifterIndex].reg))
                     + (sizeof(uint32_t) - numBytes);
    return regAddr;
}

/*!
 * @brief Calculate the address of the register used for DMA Tx transfer
 *
 * @param states Pointer to CFGIO_UART driver state structure
 *
 * @retval  None
 */
static uint32_t CFGIO_UART_CalcDmaTxRegAddr(const CFGIO_UART_STATE_T *states)
{
    const CFGIO_T *base = g_cfgioBase[states->commonState.instance];
    uint8_t shifterIndex = SHIFTER_INDEX_TX(states->commonState.resIndex);
    uint32_t regAddr = (uint32_t)(&(base->SBUF[shifterIndex].reg));
    return regAddr;
}

/**@} end of group CFGIO_UART_Functions*/
/**@} end of group CFGIO_UART_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
