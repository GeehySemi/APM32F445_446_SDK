/*!
 * @file        apm32f445_446_lin.c
 *
 * @brief       This file provides all the LIN firmware functions
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
#include "apm32f445_446_lin.h"
#include "apm32f445_446_lpuart.h"


/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LIN_Driver LIN Driver
  @{
*/

/** @defgroup LIN_Marcos Marcos
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/* Accept Master baudrate deviation from the slave baudrate to be 2% */
#define AUTOBAUD_BAUDRATE_TOLERANCE (uint32_t)2U
#define BIT_RATE_TOLERANCE_UNSYNC   (uint32_t)14U

/* Calculate range of one bit time with baudrate 2400 by formula {1000000/2400*(100 + AUTOBAUD_BAUDRATE_TOLERANCE))/100} */
#define BIT_DUR_MAX_2400    (uint32_t)(100000U * (100U + AUTOBAUD_BAUDRATE_TOLERANCE) / 24U)
#define BIT_DUR_MIN_2400    (uint32_t)(100000U * (100U - AUTOBAUD_BAUDRATE_TOLERANCE) / 24U)
/* Calculate range of one bit time with baudrate 4800 by formula {1000000/4800*(100 + AUTOBAUD_BAUDRATE_TOLERANCE))/100} */
#define BIT_DUR_MAX_4800    (uint32_t)(100000U * (100U + AUTOBAUD_BAUDRATE_TOLERANCE) / 48U)
#define BIT_DUR_MIN_4800    (uint32_t)(100000U * (100U - AUTOBAUD_BAUDRATE_TOLERANCE) / 48U)
/* Calculate range of one bit time with baudrate 9600 by formula {1000000/9600*(100 + AUTOBAUD_BAUDRATE_TOLERANCE))/100} */
#define BIT_DUR_MAX_9600    (uint32_t)(100000U * (100U + AUTOBAUD_BAUDRATE_TOLERANCE) / 96U)
#define BIT_DUR_MIN_9600    (uint32_t)(100000U * (100U - AUTOBAUD_BAUDRATE_TOLERANCE) / 96U)
/* Calculate range of one bit time with baudrate 14400 by formula {1000000/14400*(100 + AUTOBAUD_BAUDRATE_TOLERANCE))/100} */
#define BIT_DUR_MAX_14400   (uint32_t)(100000U * (100U + AUTOBAUD_BAUDRATE_TOLERANCE) / 144U)
#define BIT_DUR_MIN_14400   (uint32_t)(100000U * (100U - AUTOBAUD_BAUDRATE_TOLERANCE) / 144U)
/* Calculate range of one bit time with baudrate 19200 by formula {1000000/19200*(100 + AUTOBAUD_BAUDRATE_TOLERANCE))/100} */
#define BIT_DUR_MAX_19200   (uint32_t)(100000U * (100U + AUTOBAUD_BAUDRATE_TOLERANCE) / 192U)
#define BIT_DUR_MIN_19200   (uint32_t)(100000U * (100U - AUTOBAUD_BAUDRATE_TOLERANCE) / 192U)

/* Calculate range of two bit time with baudrate 2400 */
#define TWO_BIT_DUR_MAX_2400   (2U * BIT_DUR_MAX_2400)
#define TWO_BIT_DUR_MIN_2400   (2U * BIT_DUR_MIN_2400)
/* Calculate range of two bit time with baudrate 4800 */
#define TWO_BIT_DUR_MAX_4800   (2U * BIT_DUR_MAX_4800)
#define TWO_BIT_DUR_MIN_4800   (2U * BIT_DUR_MIN_4800)
/* Calculate range of two bit time with baudrate 9600 */
#define TWO_BIT_DUR_MAX_9600   (2U * BIT_DUR_MAX_9600)
#define TWO_BIT_DUR_MIN_9600   (2U * BIT_DUR_MIN_9600)
/* Calculate range of two bit time with baudrate 14400 */
#define TWO_BIT_DUR_MAX_14400  (2U * BIT_DUR_MAX_14400)
#define TWO_BIT_DUR_MIN_14400  (2U * BIT_DUR_MIN_14400)
/* Calculate range of two bit time with baudrate 19200 */
#define TWO_BIT_DUR_MAX_19200  (2U * BIT_DUR_MAX_19200)
#define TWO_BIT_DUR_MIN_19200  (2U * BIT_DUR_MIN_19200)

/* Calculate range of 13 bit time minimum with baudrate 19200 for autobaud feature */
#define AUTOBAUD_BRK_TIME_MIN     (13U * BIT_DUR_MIN_19200)

/* Return bit B in byte A */
#define BIT(A, B) ((A >> B) & 0x01U)

/* Actions to check of make parity */
#define LIN_MAKE_PARITY  0U
#define LIN_CHECK_PARITY 1U

/**@} end of group LIN_Marcos*/

/** @defgroup LIN_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

#if (LPUART_INSTANCE_CNT > 0U)
void LIN_LPUART0_RxTx_IRQHandler(void);
#ifdef LPUART_ERR_IRQS
void LIN_LPUART0_ERR_IRQHandler(void);
#endif
#endif /* LPUART_INSTANCE_CNT > 0U */

#if (LPUART_INSTANCE_CNT > 1U)
void LIN_LPUART1_RxTx_IRQHandler(void);
#ifdef LPUART_ERR_IRQS
void LIN_LPUART1_ERR_IRQHandler(void);
#endif
#endif /* LPUART_INSTANCE_CNT > 1U */

#if (LPUART_INSTANCE_CNT > 2U)
void LIN_LPUART2_RxTx_IRQHandler(void);
#ifdef LPUART_ERR_IRQS
void LIN_LPUART2_ERR_IRQHandler(void);
#endif
#endif /* LPUART_INSTANCE_CNT > 2U */

/* ISR array for LIN LPUART driver */
ISR_T g_linLpuartISRs[] =
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART0_RxTx_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 1U)
    LIN_LPUART1_RxTx_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 2U)
    LIN_LPUART2_RxTx_IRQHandler,
#endif
};

#ifdef LPUART_ERR_IRQS
void LIN_LPUART0_ERR_IRQHandler(void);
void LIN_LPUART1_ERR_IRQHandler(void);
void LIN_LPUART2_ERR_IRQHandler(void);

ISR_T g_linLpuartErrISRs[LPUART_INSTANCE_CNT] =
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART0_ERR_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 1U)
    LIN_LPUART1_ERR_IRQHandler,
#endif
#if (LPUART_INSTANCE_CNT > 2U)
    LIN_LPUART2_ERR_IRQHandler,
#endif
};
#endif /* LPUART_ERR_IRQS */

/* Base addresses for LPUART instances */
LPUART_T *const g_linLpuartBase[LPUART_INSTANCE_CNT] =
{
    LPUART0,
    LPUART1,
    LPUART2
};

/* LPUART IRQ enumeration numbers */
const IRQn_Type g_linLpuartRxTxIrqNum[LPUART_INSTANCE_CNT] =
{
    LPUART0_RxTx_IRQn,
    LPUART1_RxTx_IRQn,
    LPUART2_RxTx_IRQn
};

#ifdef LPUART_ERR_IRQS
/* LPUART_ERR IRQ enumeration numbers */
const IRQn_Type g_linLpuartErrIrqId[LPUART_INSTANCE_CNT] = LPUART_ERR_IRQS;
#endif

/* LPUART state structure pointers */
LIN_STATE_T *g_linState[LPUART_INSTANCE_CNT] = {NULL};

/* LIN user config structure pointers */
LIN_CFG_T *g_linUserConfig[LPUART_INSTANCE_CNT] = {NULL};

static const CLOCK_NAMES_T g_linLpuartClkName[LPUART_INSTANCE_CNT] = LPUART_CLOCK_NAMES;

static uint8_t g_cntMeasure[LPUART_INSTANCE_CNT] = {0U};
static uint8_t g_wakeupSignal[LPUART_INSTANCE_CNT] = {0U};
static uint32_t g_timeMeasure[LPUART_INSTANCE_CNT] = {0U};
static uint32_t g_lastTwoBitTime[LPUART_INSTANCE_CNT] = {0U};

/**@} end of group LIN_Variables*/

/** @defgroup LIN_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

void LIN_LPUART_IRQHandler(uint32_t instance);
STATUS_T LIN_LPUART_EnableIRQ(uint32_t instance);
STATUS_T LIN_LPUART_DisableIRQ(uint32_t instance);

static STATUS_T LIN_LPUART_Init(uint32_t instance, LIN_CFG_T *userCfg, LIN_STATE_T *linState);
static void LIN_LPUART_DeInit(uint32_t instance);
static void LIN_LPUART_DefaultConfig(bool isMaster, LIN_CFG_T *userCfg);
static STATUS_T LIN_LPUART_TxDataBlocking(uint32_t instance, const uint8_t *txBuffer, uint8_t txSize, uint32_t timeout);
static STATUS_T LIN_LPUART_TxData(uint32_t instance,const uint8_t *txBuffer, uint8_t txSize);
static STATUS_T LIN_LPUART_GetTxStatus(uint32_t instance, uint8_t *bytesRemain);
static STATUS_T LIN_LPUART_CancelTx(uint32_t instance);
static STATUS_T LIN_LPUART_RxDataBlocking(uint32_t instance, uint8_t *rxBuffer, uint8_t rxSize, uint32_t timeout);
static STATUS_T LIN_LPUART_RxData(uint32_t instance, uint8_t *rxBuffer, uint8_t rxSize);
static STATUS_T LIN_LPUART_GetRxStatus(uint32_t instance, uint8_t *bytesRemain);
static STATUS_T LIN_LPUART_EnterSleepMode(uint32_t instance);
static STATUS_T LIN_LPUART_EnterIdleState(uint32_t instance);
static STATUS_T LIN_LPUART_TxWakeupSignal(uint32_t instance);
static LIN_NODE_STATE_T LIN_LPUART_GetNodeState(uint32_t instance);
static LIN_CALLBACK_T LIN_LPUART_InstallCallback(uint32_t instance, LIN_CALLBACK_T function);
static void LIN_LPUART_TimeoutProcess(uint32_t instance);
static void LIN_LPUART_SetTimeoutCnt(uint32_t instance, uint32_t timeout);
static STATUS_T LIN_LPUART_MasterTxHeader(uint32_t instance, uint8_t id);
static STATUS_T LIN_LPUART_AutoBaudCapture(uint32_t instance);
static uint8_t LIN_LPUART_MakeChecksumByte(uint32_t ins, const uint8_t *buff, uint8_t bufSize, uint8_t pid);
static STATUS_T LIN_LPUART_WaitComplete(uint32_t ins, uint32_t timeout);
static void LIN_LPUART_ProcessBreakDetect(uint32_t ins);
static void LIN_LPUART_CheckWakeupSignal(uint32_t ins);
static void LIN_LPUART_ProcessFrame(uint32_t ins, uint8_t tempValue);
static void LIN_LPUART_ProcessFrameHeader(uint32_t ins, uint8_t tempValue);
static void LIN_LPUART_ProcessRxData(uint32_t ins, uint8_t tempValue);
static void LIN_LPUART_ProcessTxData(uint32_t ins, uint8_t tempValue);
static void LIN_LPUART_AutobaudTimerValEval(uint32_t ins, uint32_t twoBitTime);
static void LIN_LPUART_EvalTwoBitTimeLength(uint32_t ins, uint32_t twoBitTime);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initialize LIN instance
 *
 * @param ins: LIN instance number
 * @param userCfg: user configuration structure of type
 * @param linState: pointer to the LIN  driver state structure
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Operation was successful.
 *         - STATUS_ERROR   : Operation failed due to semaphores initialize error.
 */
STATUS_T LIN_Init(uint32_t ins, LIN_CFG_T *userCfg, LIN_STATE_T *linState)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_Init(ins, userCfg, linState);
#endif

    return result;
}

/*!
 * @brief Reset LIN
 *
 * @param ins: LIN instance number
 *
 * @retval void
 */
void LIN_Deinit(uint32_t ins)
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART_DeInit(ins);
#endif
}

/*!
 * @brief Receives frame data from the LIN  using non-blocking method.
 * @details This function will check the checksum byte. If users use
 *          LIN_TimeoutProcess in a timer interrupt handler, then before
 *          using this function, users have to set timeout counter to an
 *          appropriate value by using LIN_SetTimeoutCounter(). The timeout
 *          value should be big enough to complete the reception.
 *
 * @param ins: LIN instance number
 * @param rxBuffer: buffer storing 8-bit received data
 * @param rxSize: the number of bytes to receive
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Receives frame successfully
 *         - STATUS_TIMEOUT : Checksum incorrect
 *         - STATUS_BUSY    : Bus busy
 *         - STATUS_ERROR   : Operation failed due to timeout has occurred
 */
STATUS_T LIN_RxData(uint32_t ins, uint8_t *rxBuffer, uint8_t rxSize)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_RxData(ins, rxBuffer, rxSize);
#endif

    return result;
}

/*!
 * @brief Receives frame data from the LIN using blocking method.
 *
 * @param ins LIN instance number
 * @param rxBuffer  buffer storing 8-bit received data
 * @param rxSize the number of bytes to receive
 * @param timeout timeout value in milliseconds
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : receives frame data is successful
 *         - STATUS_TIMEOUT : checksum is not correct
 *         - STATUS_BUSY    : Bus is busy
 *         - STATUS_ERROR   : Operation failed due to timeout has occurred
 */
STATUS_T LIN_RxDataBlocking(
    uint32_t ins,
    uint8_t *rxBuffer,
    uint8_t rxSize,
    uint32_t timeout)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_RxDataBlocking(ins, rxBuffer, rxSize, timeout);
#endif

    return result;
}

/*!
 * @brief Get status of an on-going non-blocking reception.
 *
 * @param ins: LIN instance number
 * @param bytesRemain: Number of bytes still needed to receive
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The reception finished.
 *         - STATUS_TIMEOUT : The reception isn't finished.
 *         - STATUS_BUSY    : The reception is going on
 */
STATUS_T LIN_GetRxStatus(uint32_t ins, uint8_t * bytesRemain)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_GetRxStatus(ins, bytesRemain);
#endif

    return result;
}

/*!
 * @brief Sends frame data out through the LIN using non-blocking method
 * @details If users use LIN_TimeoutProcess in a timer interrupt handler, then
 *          before using this function, users have to set timeout counter to
 *          an appropriate value by using LIN_SetTimeoutCounter(). The timeout
 *          value should be big enough to complete the transmission.
 *
 * @param ins: LIN instance number
 * @param txBuffer: source buffer containing 8-bit data chars to send
 * @param txSize: the number of bytes to send
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Transmission is successful.
 *         - STATUS_BUSY    : Transmission failed due to bus busy.
 *         - STATUS_ERROR   : Transmission failed due to condition anomaly
 */
STATUS_T LIN_TxData(uint32_t ins, const uint8_t *txBuffer, uint8_t txSize)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_TxData(ins, txBuffer, txSize);
#endif

    return result;
}

/*!
 * @brief Sends Frame data out through the LIN using blocking method
 *
 * @param ins: LIN instance number
 * @param txBuffer: source buffer containing 8-bit data chars to send
 * @param txSize: the number of bytes to send
 * @param timeout: timeout value in milliseconds
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Transmission is successful.
 *         - STATUS_TIMEOUT : Transmission isn't successful.
 *         - STATUS_ERROR : Transmission condition anomaly.
 */
STATUS_T LIN_TxDataBlocking(
    uint32_t ins,
    const uint8_t *txBuffer,
    uint8_t txSize,
    uint32_t timeout)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_TxDataBlocking(ins, txBuffer, txSize, timeout);
#endif

    return result;
}

/*!
 * @brief Get status of an on-going non-blocking transmission
 *
 * @param ins: LIN instance number
 * @param bytesRemain: Number of bytes still needed to transmit
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The transmission is successful
 *         - STATUS_BUSY    : The transmission is sending
 *         - STATUS_TIMEOUT : Operation failed due to timeout has occurred
 */
STATUS_T LIN_GetTxStatus(uint32_t ins, uint8_t *bytesRemain)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_GetTxStatus(ins, bytesRemain);
#endif

    return result;
}

/*!
 * @brief Cancel an on-going non-blocking tx/rx
 *
 * @param ins LIN instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_CancelTx(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_CancelTx(ins);
#endif

    return result;
}

/*!
 * @brief Sends frame header out through the LIN  using non-blocking method.
 *
 * @param ins: LIN instance number
 * @param id: Frame Identifier
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Transmission is successful
 *         - STATUS_BUSY    : Bus busy
 *         - STATUS_ERROR   : The interface isn't Master or id isn't in range from
 *                            0 to 0x3F or node's current state is in SLEEP mode.
 */
STATUS_T LIN_MasterTxHeader(uint32_t ins, uint8_t id)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_MasterTxHeader(ins, id);
#endif

    return result;
}

/*!
 * @brief Puts current LIN node to sleep mode
 *
 * @param ins LIN instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_EnterSleepMode(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_EnterSleepMode(ins);
#endif

    return result;
}

/*!
 * @brief Puts current LIN node to idle state
 *
 * @param ins: LIN instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_EnterIdleState(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_EnterIdleState(ins);
#endif

    return result;
}

/*!
 * @brief Sends a wakeup signal through the LIN
 *
 * @param ins: LIN instance number
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Bus busy flag is false.
 *         - STATUS_BUSY    : Bus busy flag is true.
 */
STATUS_T LIN_SendWakeupSignal(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_TxWakeupSignal(ins);
#endif

    return result;
}

/*!
 * @brief Get LIN node state
 *
 * @param ins: LIN instance number
 *
 * @retval current LIN node state
 */
LIN_NODE_STATE_T LIN_GetNodeState(uint32_t ins)
{
    LIN_NODE_STATE_T result = LIN_NODE_UNINIT;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_GetNodeState(ins);
#endif

    return result;
}

/*!
 * @brief Captures time interval to calculate baudrate automatically
 * @details When enable autobaud feature, this function should only be used in
 *          Slave, the timer should be in input capture mode of both rising
 *          and falling edges, the timer input capture pin should be externally
 *          connected to RXD pin.
 *
 * @param ins: LIN instance number
 *
 * @retval operation status:
 *        - STATUS_SUCCESS: Operation successful.
 *        - STATUS_BUSY:    Bus busy
 *        - STATUS_ERROR:   Operation failed due to break char incorrect,
 *                          wakeup signal incorrect or calculate baudrate failed.
 */
STATUS_T LIN_AutoBaudCapture(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_AutoBaudCapture(ins);
#endif

    return result;
}

/*!
 * @brief Makes or checks parity bits.
 *
 * @param PID: PID byte in case of checking parity bits or ID byte in case of making parity bits
 * @param action: 1 for Checking parity bits, 0 for making parity bits
 *
 * @retval Value has 8 bit:
 *         - 0xFF : Parity bits are incorrect,
 *         - ID   : Checking parity bits are correct.
 *         - PID  : action is making parity bits.
 */
uint8_t LIN_ProcessParity(uint8_t PID, uint8_t action)
{
    uint8_t result;
    uint8_t parity;

    parity = (uint8_t)(
             ((0xFFU & (BIT(PID, 0U) ^ BIT(PID, 1U) ^ BIT(PID, 2U) ^ BIT(PID, 4U))) << 6U)
           | ((0xFFU ^ (BIT(PID, 1U) ^ BIT(PID, 3U) ^ BIT(PID, 4U) ^ BIT(PID, 5U))) << 7U));

     /* If action is checking parity bits */
    if (action == LIN_CHECK_PARITY)
    {
        if (parity == (PID & 0xC0U))
        {
            /* parity bits correct */
            result = (uint8_t)(PID & 0x3FU);
        }
        else
        {
            /* Parity bits incorrect */
            result = 0xFFU;
        }
    }
    else
    {
        /* Return PID in case of making parity bits */
        result = (uint8_t)(PID | parity);
    }
    return result;
}

/*!
 * @brief Makes the checksum byte for a frame
 *
 * @param buffer: Pointer to Tx buffer
 * @param size: Number of bytes that are contained in the buffer
 * @param PID: Protected Identifier byte
 *
 * @retval the checksum byte
 */
uint8_t LIN_CalculateChecksum(const uint8_t *buffer, uint8_t size, uint8_t PID)
{
    uint16_t checksum = 0U;
    uint8_t len;

    if ((PID == 0x3CU) || (PID == 0x7DU) || (PID == 0xFEU) || (PID == 0xBFU))
    {
        /* Do not add PID in checksum calculation */
        checksum = 0U;
    }
    else
    {
        /* Add PID in checksum calculation */
        checksum = PID;
    }

    for (len = size; 0U < len; len--)
    {
        checksum += *buffer;
        buffer++;
        if (checksum > 0xFFU)
        {
            checksum -= 0xFFU;
        }
    }

    /* Return reversed checksum */
    return (uint8_t)(~checksum);
}

/*!
 * @brief Callback function for Timer Interrupt Handler
 *
 * @param ins: LIN instance number
 *
 * @retval void
 */
void LIN_TimeoutProcess(uint32_t ins)
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART_TimeoutProcess(ins);
#endif
}

/*!
 * @brief Configure value for timeout counter which is used in LIN_TimeoutProcess
 *
 * @param ins: LIN instance number
 * @param timeout: Timeout value to be set
 *
 * @retval void
 */
void LIN_SetTimeoutCounter(uint32_t ins, uint32_t timeout)
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART_SetTimeoutCnt(ins, timeout);
#endif
}

/*!
 * @brief Initializes the LIN user configuration structure with default values
 *
 * @param isMaster: true if node is MASTER, false if node is SLAVE
 * @param userCfg: the default configuration

 * @retval void
 */
void LIN_DefaultConfig(bool isMaster, LIN_CFG_T *userCfg)
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART_DefaultConfig(isMaster, userCfg);
#endif
}

/*!
 * @brief Installs callback function that is used for LIN_IRQHandler
 *
 * @param ins: LIN instance number
 * @param function: the LIN receive callback function
 *
 * @retval Former LIN callback function pointer
 *
 * @note After a callback is installed, it bypasses part of the LIN  IRQHandler logic.
 *       Therefore, the callback needs to handle the indexes of txBuffer and txSize.
 */
LIN_CALLBACK_T LIN_InstallCallback(uint32_t ins, LIN_CALLBACK_T callback)
{
    LIN_CALLBACK_T result = NULL;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_InstallCallback(ins, callback);
#endif

    return result;
}

/*!
 * @brief Enables LIN hardware interrupts
 *
 * @param ins: LIN instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_EnableIRQ(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_EnableIRQ(ins);
#endif

    return result;
}

/*!
 * @brief Disables LIN hardware interrupts
 *
 * @param ins: LIN instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_DisableIRQ(uint32_t ins)
{
    STATUS_T result = STATUS_UNSUPPORTED;

#if (LPUART_INSTANCE_CNT > 0U)
    result = LIN_LPUART_DisableIRQ(ins);
#endif

    return result;
}

/*!
 * @brief Interrupt handler for LIN
 *
 * @param ins: LIN instance number
 *
 * @retval void
 */
void LIN_IRQHandler(uint32_t ins)
{
#if (LPUART_INSTANCE_CNT > 0U)
    LIN_LPUART_IRQHandler(ins);
#endif
}

/*!
 * @brief LIN_LPUART_IRQ_Handler
 */
#if (LPUART_INSTANCE_CNT > 0U)
void LIN_LPUART0_RxTx_IRQHandler(void)
{
    LIN_IRQHandler(0U);
}

#ifdef LPUART_ERR_IRQS
void LIN_LPUART0_ERR_IRQHandler(void)
{
    LIN_IRQHandler(0U);
}
#endif
#endif /* LPUART_INSTANCE_CNT > 0U */

#if (LPUART_INSTANCE_CNT > 1U)
void LIN_LPUART1_RxTx_IRQHandler(void)
{
    LIN_IRQHandler(1U);
}

#ifdef LPUART_ERR_IRQS
void LIN_LPUART1_ERR_IRQHandler(void)
{
    LIN_IRQHandler(1U);
}
#endif
#endif /* LPUART_INSTANCE_CNT > 1U */

#if (LPUART_INSTANCE_CNT > 2U)
void LIN_LPUART2_RxTx_IRQHandler(void)
{
    LIN_IRQHandler(2U);
}
#ifdef LPUART_ERR_IRQS
void LIN_LPUART2_ERR_IRQHandler(void)
{
    LIN_IRQHandler(2U);
}
#endif
#endif /* LPUART_INSTANCE_CNT > 2U */

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enables LIN_LPUART hardware interrupts.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_LPUART_EnableIRQ(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    const LIN_STATE_T *states = g_linState[ins];

    if (states->nodeState != LIN_NODE_SLP_MODE)
    {
        /* Enable RX complete interrupt */
        LPUART_HW_EnableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

        /* Enable frame error interrupt */
        LPUART_HW_EnableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);

        /* Enable LIN break detect interrupt */
        LPUART_HW_EnableIntMode(base, LPUART_INT_LIN_BRK_DETECT);
    }
    else
    {
        /* Enable RX Input Active Edge interrupt */
        LPUART_HW_EnableIntMode(base, LPUART_INT_RX_ACTIVE_EDGE);
    }

    /* Enable LPUART interrupts */
    INT_SYS_EnableIRQ(g_linLpuartRxTxIrqNum[ins]);

#ifdef LPUART_ERR_IRQS
    INT_SYS_EnableIRQ(g_linLpuartErrIrqId[ins]);
#endif

    return STATUS_SUCCESS;
}

/*!
 * @brief Disables LIN_LPUART hardware interrupts.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
STATUS_T LIN_LPUART_DisableIRQ(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    const LIN_STATE_T *states = g_linState[ins];

    if (states->nodeState != LIN_NODE_SLP_MODE)
    {
        /* Disable RX complete interrupt */
        LPUART_HW_DisableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

        /* Disable frame error interrupt */
        LPUART_HW_DisableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);

        /* Disable frame error interrupt */
        LPUART_HW_DisableIntMode(base, LPUART_INT_LIN_BRK_DETECT);
    }
    else
    {
        /* Disable RX Input Active Edge interrupt */
        LPUART_HW_DisableIntMode(base, LPUART_INT_RX_ACTIVE_EDGE);
    }

    /* Disable LPUART interrupts */
    INT_SYS_DisableIRQ(g_linLpuartRxTxIrqNum[ins]);

#ifdef LPUART_ERR_IRQS
    INT_SYS_DisableIRQ(g_linLpuartErrIrqId[ins]);
#endif

    return STATUS_SUCCESS;
}

/*!
 * @brief LIN_LPUART interrupt handler for RX_TX and Error interrupts.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval None
 */
void LIN_LPUART_IRQHandler(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint8_t tempValue = 0U;
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    /* Check RX Input Active Edge interrupt enable */
    bool edgeIntState = LPUART_HW_GetIntMode(base, LPUART_INT_RX_ACTIVE_EDGE);

    if (!LPUART_HW_GetStatusFlag(base,LPUART_LIN_BREAK_DETECT))
    {
        /* If LPUART_RX Pin Active Edge has been detected */
        if (! (LPUART_HW_GetStatusFlag(base, LPUART_RX_ACTIVE_EDGE_DETECT)
            && edgeIntState))
        {
            /* If Framing Error has been detected */
            if (!LPUART_HW_GetStatusFlag(base, LPUART_FRAME_ERR))
            {
                if (LPUART_HW_GetStatusFlag(base, LPUART_RX_DATA_REG_FULL))
                {
                    /* Get data from Data Register & Clear LPUART_RX_DATA_REG_FULL flag */
                    LPUART_HW_RxChar(base, &tempValue);

                    /* Process data in Data Register while receive, send data */
                    LIN_LPUART_ProcessFrame(ins, tempValue);
                }
            }
            else
            {
                /* Clear Framing Error Interrupt Flag */
                LPUART_HW_ClearStatusFlag(base, LPUART_FRAME_ERR);

                /* Read dummy to clear LPUART_RX_DATA_REG_FULL flag */
                LPUART_HW_RxChar(base, &tempValue);

                states->eventId = LIN_EVENT_FRAME_ERROR;

                /* Check if LIN current node state is LIN_NODE_TX_DATA */
                if (states->nodeState == LIN_NODE_TX_DATA)
                {
                    /* Callback function to handle Framing Error Event */
                    if (states->callback != NULL)
                    {
                        states->callback(ins, states);
                    }
                }
                else
                {
                    /* Check if LIN current node state is LIN_NODE_RX_DATA */
                    if (states->nodeState == LIN_NODE_RX_DATA)
                    {
                        /* Callback function to handle Framing Error Event */
                        if (states->callback != NULL)
                        {
                            states->callback(ins, states);
                        }
                    }
                }
                LIN_LPUART_EnterIdleState(ins);
            }
        }
        else
        {
            /* Clear LPUART_RX Pin Active Edge Interrupt Flag */
            LPUART_HW_ClearStatusFlag(base, LPUART_RX_ACTIVE_EDGE_DETECT);

            /* Check if a wakeup signal has been received */
            LIN_LPUART_CheckWakeupSignal(ins);
        }
    }
    else
    {
        /* If LIN break character has been detected */
        LIN_LPUART_ProcessBreakDetect(ins);
    }

    /* Get status RX overrun flag */
    if (LPUART_HW_GetStatusFlag(base, LPUART_RX_OVERRUN))
    {
        /* Clear overrun flag */
        LPUART_HW_ClearStatusFlag(base, LPUART_RX_OVERRUN);

        /* Set current event id to LIN_EVENT_RX_OVERRUN */
        states->eventId = LIN_EVENT_RX_OVERRUN;

        /* Callback function to handle RX Overrun Event */
        if (states->callback != NULL)
        {
            states->callback(ins, states);
        }
    }
}

/*!
 * @brief Initializes an LIN_LPUART ins for LIN Network.
 *
 * @param ins LIN_LPUART instance number
 * @param userCfg user configuration structure of type #LIN_CFG_T
 * @param linState pointer to the LIN_LPUART driver state structure
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Operation was successful.
 *         - STATUS_ERROR   : Operation failed due to semaphores initialize error.
 *
 */
static STATUS_T LIN_LPUART_Init(uint32_t ins, LIN_CFG_T *userCfg, LIN_STATE_T *states)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint32_t sourceClockFreq = 0U;
    LPUART_T *base = g_linLpuartBase[ins];

    /* Get the LPUART clock configured in the clock manager */
    (void)CLOCK_SYS_ReadFreq(g_linLpuartClkName[ins], &sourceClockFreq);

    g_linState[ins] = states;
    g_linUserConfig[ins] = userCfg;
    states->sourceClockFreq = sourceClockFreq;

    LPUART_HW_Init(base);

    /* Create tx completed and rx completed semaphores as counting, with init value is 0 */
    if (  (OSIF_SemCreate(&states->rxCompleted, 0) != STATUS_SUCCESS)
       || (OSIF_SemCreate(&states->txCompleted, 0) != STATUS_SUCCESS))
    {
        return STATUS_ERROR;
    }

    /* if autobaud is enabled */
    if ((userCfg->autobaudEnable) && (userCfg->nodeType == LIN_SLAVE))
    {
        /**
         * Setting Slave's baudrate to 19200 will help Slave node always detect
         * LIN Break from Master.
         */
        userCfg->baudrate = 19200U;
        g_cntMeasure[ins] = 0U;
        g_timeMeasure[ins] = 0U;
        g_lastTwoBitTime[ins] = 0U;
        states->fallingEdgeCnt = 0U;
        states->enableBaudrateEval = true;
    }

    /* Set LPUART baudrate */
    (void)LPUART_SetBaudrate(ins, userCfg->baudrate);
    LPUART_HW_SetDataBits(base, LPUART_8_BITS_DATA, false);
    LPUART_HW_SetParityMode(base, LPUART_PARITY_DISABLED);
    LPUART_HW_SetStopBitCnt(base, LPUART_ONE_STOP_BIT);

    /* Check if the current node is MASTER */
    if (userCfg->nodeType == LIN_MASTER)
    {
        /* Set Break char length as 13 bits minimum */
        LPUART_HW_SetBreakCharTxLen(base, LPUART_BRK_CHAR_13_BIT_MIN);
    }

    /* Set Break char detect length as 13 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_13_BIT_MIN);

    /* Enable RX complete interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

    /* Enable frame error interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);

    /* Enable LIN break detect interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_LIN_BRK_DETECT);

    /* Install LIN_RxTx ISR for LPUART ins */
    INT_SYS_InstallHandler(g_linLpuartRxTxIrqNum[ins],
                           g_linLpuartISRs[ins],
                           (ISR_T *)0);

    /* Enable LPUART interrupts */
    INT_SYS_EnableIRQ(g_linLpuartRxTxIrqNum[ins]);

#ifdef LPUART_ERR_IRQS
    /* Install LIN_ERR ISR for LPUART instance */
    INT_SYS_InstallHandler(g_linLpuartErrIrqId[ins],
                           g_linLpuartErrISRs[ins],
                           (ISR_T *)0);
    /* Enable LPUART error interrupts */
    INT_SYS_EnableIRQ(g_linLpuartErrIrqId[ins]);
#endif

    states->busBusy = false;
    states->txBusy = false;
    states->rxBusy = false;
    states->txBlocking = false;
    states->rxBlocking = false;
    states->timeoutCntFlag = false;
    states->timeoutCnt = 0U;
    states->nodeState = LIN_NODE_IDLE;

    /**
     * Assign wakeup signal to satisfy LIN Specifications specifies that wakeup
     * signal shall be in range from 250us to 5ms.
     */
    if (userCfg->baudrate <= 10000U)
    {
        /* Wakeup signal will be range from 400us to 4ms depend on baudrate */
        g_wakeupSignal[ins] = 0xF8U;
    }
    else
    {
        /* Wakeup signal will be range from 400us to 800us depend on baudrate */
        g_wakeupSignal[ins] = 0x80U;
    }

    if ( !((userCfg->autobaudEnable)
        && (userCfg->nodeType == LIN_SLAVE)))
    {
        /* Enable the LPUART transmitter and receiver */
        LPUART_HW_EnableTx(base);
        LPUART_HW_EnableRx(base);
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief Shuts down the LIN_LPUART by disabling interrupts and transmitter/receiver.
 *
 * @param ins: LIN_LPUART instance number
 *
 * @retval None
 */
static void LIN_LPUART_DeInit(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    /* Wait until the data is completely shifted out of shift register */
    while (!LPUART_HW_GetStatusFlag(base, LPUART_TX_COMPLETE)) {}

    /* Disable the LPUART transmitter and receiver */
    LPUART_HW_DisableTx(base);
    LPUART_HW_DisableRx(base);

    /* Destroy TX and RX semaphores */
    (void)OSIF_SemDestroy(&states->txCompleted);
    (void)OSIF_SemDestroy(&states->rxCompleted);

    /* Disable LPUART interrupts */
    INT_SYS_DisableIRQ(g_linLpuartRxTxIrqNum[ins]);

#ifdef LPUART_ERR_IRQS
    INT_SYS_DisableIRQ(g_linLpuartErrIrqId[ins]);
#endif

    states->nodeState = LIN_NODE_UNINIT;
    g_linState[ins] = NULL;
}

/*!
 * @brief Read the LIN user configuration structure with default values.
 *
 * @param isMaster Node function:
                - true if node is MASTER
                - false if node is SLAVE
 * @param userCfg the default configuration
 *
 * @retval void
 */
static void LIN_LPUART_DefaultConfig(bool isMaster, LIN_CFG_T *userCfg)
{
    userCfg->baudrate = 19200U;
    userCfg->nodeType = isMaster ? LIN_MASTER : LIN_SLAVE;
    userCfg->getTimeIntervalCallback = NULL;
    userCfg->autobaudEnable = false;
}

/*!
 * @brief Installs callback function that is used for LIN_LPUART_IRQHandler.
 * @details After a callback is installed, it bypasses part of the LIN_LPUART
 *          IRQHandler logic. Therefore, the callback needs to handle the
 *          indexes of txBuffer and txSize.
 *
 * @param ins The LIN_LPUART instance number.
 * @param function The LIN_LPUART receive callback function.
 *
 * @retval Former LIN callback function pointer.
 */
static LIN_CALLBACK_T LIN_LPUART_InstallCallback(uint32_t ins, LIN_CALLBACK_T function)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];
    LIN_CALLBACK_T oldCallback = states->callback;
    states->callback = function;
    return oldCallback;
}

/*!
 * @brief Part of Interrupt handler for receiving and sending data.
 *        Receive Header, Data and Send Data.
 *
 * @param ins LIN_LPUART instance number
 * @param tmpByte transfer data
 *
 * @retval None
 */
static void LIN_LPUART_ProcessFrame(uint32_t ins, uint8_t tempValue)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LIN_STATE_T *states = g_linState[ins];

    if (states->nodeState == LIN_NODE_RX_SYNC)
    {
        LIN_LPUART_ProcessFrameHeader(ins, tempValue);
    }
    else if (states->nodeState == LIN_NODE_TX_PID)
    {
        LIN_LPUART_ProcessFrameHeader(ins, tempValue);
    }
    else if (states->nodeState == LIN_NODE_RX_PID)
    {
        LIN_LPUART_ProcessFrameHeader(ins, tempValue);
    }
    else if (states->nodeState == LIN_NODE_RX_DATA)
    {
        LIN_LPUART_ProcessRxData(ins, tempValue);
    }
    else if (states->nodeState == LIN_NODE_TX_DATA)
    {
        LIN_LPUART_ProcessTxData(ins, tempValue);
    }
}

/*!
 * @brief Part of Interrupt handler for receiving and sending data.
 *        Receive Sync byte, PID and Send PID.
 *
 * @param ins LIN_LPUART instance number
 * @param tmpByte transfer data
 *
 * @retval None
 */
static void LIN_LPUART_ProcessFrameHeader(uint32_t ins, uint8_t tempValue)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    const LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LIN_STATE_T *states = g_linState[ins];

    if (states->nodeState == LIN_NODE_RX_PID)
    {
        if (userCfg->nodeType == LIN_SLAVE)
        {
            /* Check the received PID */
            states->currentId = LIN_ProcessParity(tempValue, LIN_CHECK_PARITY);
            states->currentPid = tempValue;
            if (states->currentId == 0xFFU)
            {
                /* Set current event ID to PID ERROR */
                states->eventId = LIN_EVENT_PID_ERROR;

                /* Callback function to handle event PID incorrect */
                if (states->callback != NULL)
                {
                    states->callback(ins, states);
                }

                /* Change node's current state to IDLE */
                LIN_LPUART_EnterIdleState(ins);
            }
            else
            {
                /* Set current event ID to PID correct */
                states->eventId = LIN_EVENT_PID_OK;

                /* Check receiving data is blocking */
                if (!states->rxBlocking)
                {
                    states->busBusy = false;

                    /* Callback function to handle event PID correct */
                    if (states->callback != NULL)
                    {
                        states->callback(ins, states);
                    }
                }
                else
                {
                    /* Starting receive data blocking */
                    states->rxBusy = true;
                    states->busBusy = true;
                    states->nodeState = LIN_NODE_RX_DATA;

                    /* Set Break char detect length as 10 bits minimum */
                    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);
                }
            }
        }
        /* If the node is MASTER */
        else
        {
            /**
             * In case of errors during header transmission, it is up to the implementer
             * how to handle these errors (stop/continue transmission) and to decide if the
             * corresponding response is valid or not. By default, LIN Driver set busBusy
             * to false, and change node's state to IDLE.
             */
            if (tempValue != states->currentPid)
            {
                /* Set current event ID to PID incorrect */
                states->eventId = LIN_EVENT_PID_ERROR;
                states->busBusy = false;
                states->nodeState = LIN_NODE_IDLE;

                /* Callback function to handle event MASTER SENT PID ERROR */
                if (states->callback != NULL)
                {
                    states->callback(ins, states);
                }
            }
            /* Check if master node sent PID correctly */
            else
            {
                /* Set current event ID to PID correct */
                states->eventId = LIN_EVENT_PID_OK;

                /* Check receiving data is blocking */
                if (!states->rxBlocking)
                {
                    /* Clear Bus bus flag */
                    states->busBusy = false;

                    /* Callback function to handle correct PID */
                    if (states->callback != NULL)
                    {
                        states->callback(ins, states);
                    }
                }
                else
                {
                    /* Starting receive data blocking */
                    states->rxBusy = true;
                    states->busBusy = true;
                    states->nodeState = LIN_NODE_RX_DATA;

                    /* Set Break char detect length as 10 bits minimum */
                    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);
                }
            }
        }
    }
    /* If current state is MASTER SENDING PID */
    else if (states->nodeState == LIN_NODE_TX_PID)
    {
        if (tempValue == 0x55U)
        {
            states->nodeState = LIN_NODE_RX_PID;

            /* Send the current PID byte */
            LPUART_HW_TxChar(base, states->currentPid);
        }
        /**
         * In case of errors during header transmission, it is up to the implementer
         * how to handle these errors (stop/continue transmission) and to decide if the
         * corresponding response is valid or not. By default, LIN Driver set busBusy
         * to false, and change node's state to IDLE.
         */
        else
        {
            /* Set current event ID to Sync byte is incorrect */
            states->eventId = LIN_EVENT_SYNC_ERROR;
            states->busBusy = false;
            states->nodeState = LIN_NODE_IDLE;

            /* Callback function to handle event SENT SYNC BYTE ERROR */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }
        }
    }
    /* If current state is RECEIVE SYNC FIELD */
    else if (states->nodeState == LIN_NODE_RX_SYNC)
    {
        if (tempValue == 0x55U)
        {
            /* Set current event ID to Sync byte is correct */
            states->eventId = LIN_EVENT_SYNC_OK;
            states->nodeState = LIN_NODE_RX_PID;
        }
        else
        {
            /* Set current event ID to Sync byte is incorrect */
            states->eventId = LIN_EVENT_SYNC_ERROR;

            /* Callback function to handle event RECEIVED SYNC FIELD ERROR */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }
            LIN_LPUART_EnterIdleState(ins);
        }
    }
}

/*!
 * @brief Part of Interrupt handler for receiving.
 *
 * @param ins LIN_LPUART instance number
 * @param tmpByte transfer data
 *
 * @retval None
 */
static void LIN_LPUART_ProcessRxData(uint32_t ins, uint8_t tempValue)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];
    if (states->rxSize <= (states->byteCount + 1U))
    {
        if ((states->rxSize - states->byteCount) == 1U)
        {
            states->checkSum = tempValue;
        }
    }
    else
    {
        *(states->rxBuffer) = tempValue;
        states->rxBuffer++;
    }

    states->byteCount++;
    if (states->byteCount == states->rxSize)
    {
        /* Restore rxBuffer pointer */
        states->rxBuffer -= states->rxSize - 1U;
        if (LIN_LPUART_MakeChecksumByte(ins,
                                        states->rxBuffer,
                                        states->rxSize - 1U,
                                        states->currentPid)
           != states->checkSum)
        {
            states->eventId = LIN_EVENT_CHKSUM_ERROR;

            /* Callback function to handle checksum error */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }
            states->rxBusy = false;
            LIN_LPUART_EnterIdleState(ins);
        }
        else
        {
            states->eventId = LIN_EVENT_RX_FINISHED;
            states->nodeState = LIN_NODE_RX_DATA_FINISHED;

            /* Callback function to handle RX COMPLETED */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }

            /* Check if the reception is non-blocking */
            if (states->rxBlocking)
            {
                /* Post Semaphore to signal Rx Completed */
                (void)OSIF_SemPost(&states->rxCompleted);
            }
            else
            {
                states->busBusy = false;
                states->rxBusy = false;

                /**
                 * In case of receiving a go to sleep request, after callback
                 * node is in SLEEP MODE. In this case, node is in SLEEP MODE state.
                 */
                if (states->nodeState != LIN_NODE_SLP_MODE)
                {
                    LIN_LPUART_EnterIdleState(ins);
                }
            }
        }
    }
}

/*!
 * @brief Part of Interrupt handler for sending data.
 *
 * @param ins LIN_LPUART instance number
 * @param tmpByte transfer data
 *
 * @retval None
 */
static void LIN_LPUART_ProcessTxData(uint32_t ins, uint8_t tempValue)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint8_t tempSize;
    bool txFlag = true;
    bool tempBufAndSize;
    bool tempChecksumAndSize;
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    /* Check Tx data register empty flag */
    if (LPUART_HW_GetStatusFlag(base, LPUART_TX_DATA_REG_EMPTY))
    {
        tempSize = (uint8_t)(states->txSize - states->byteCount);
        tempChecksumAndSize =  (tempSize == 1U)
                            && (states->checkSum != tempValue);
        tempBufAndSize =  (*states->txBuffer != tempValue)
                       && (tempSize != 1U);

        if ((tempBufAndSize) || (tempChecksumAndSize))
        {
            states->eventId = LIN_EVENT_READBACK_ERROR;

            /* Callback function to handle Readback error */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }

            /* Check if the transmission is non-blocking */
            if (!states->txBlocking)
            {
                states->txBusy = false;
                LIN_LPUART_EnterIdleState(ins);
            }
            txFlag = false;
        }
        else
        {
            states->byteCount++;
            states->txBuffer++;
        }
    }
    else
    {
        states->eventId = LIN_EVENT_READBACK_ERROR;

        /* Callback function to handle Readback error */
        if (states->callback != NULL)
        {
            states->callback(ins, states);
        }

        /* Check if the transmission is non-blocking */
        if (states->txBlocking == false)
        {
            states->txBusy = false;
            LIN_LPUART_EnterIdleState(ins);
        }
        txFlag = false;
    }

    if (txFlag)
    {
        if (states->byteCount >= states->txSize)
        {
            states->nodeState = LIN_NODE_TX_DATA_FINISHED;
            states->eventId = LIN_EVENT_TX_FINISHED;

            LPUART_HW_DisableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

            /* Callback function to handle event TX COMPLETED */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }

            /* Check if the transmission is non-blocking */
            if (states->txBlocking)
            {
                /* Post Semaphore to signal Tx Completed*/
                (void)OSIF_SemPost(&states->txCompleted);
            }
            else
            {
                /* Clear Tx busy flag */
                states->txBusy = false;

                /* In this case, node is in SLEEP MODE state */
                if (states->nodeState != LIN_NODE_SLP_MODE)
                {
                    LIN_LPUART_EnterIdleState(ins);
                }
            }
        }
        else
        {
            /* Send checksum byte */
            if ((states->txSize - states->byteCount) == 1U)
            {
                LPUART_HW_TxChar(base, states->checkSum);
            }
            /* Send data bytes */
            else
            {
                LPUART_HW_TxChar(base, *states->txBuffer);
            }
        }
    }
}

/*!
 * @brief This function calculate LIN bus baudrate and set slave's baudrate accordingly.
 *
 * @details Autobaud process runs only once after reset.
 *          After setting slave's baudrate to LIN bus baudrate,
 *          slave does not evaluate LIN bus baudrate anymore.
 *          This is not a public API as it is called from other driver functions.
 *
 * @param ins LIN_LPUART instance number
 * @param twoBitTime two bit time with baudrate
 *
 * @retval None
 */
static void LIN_LPUART_AutobaudTimerValEval(
    uint32_t ins,
    uint32_t twoBitTime)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint32_t hostBaudrate = 0U;
    LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LIN_STATE_T *states = g_linState[ins];

    /* Evaluate average value against baudrate */
    LIN_LPUART_EvalTwoBitTimeLength(ins, twoBitTime);

    if (  (states->nodeState == LIN_NODE_RX_SYNC)
       && (states->fallingEdgeCnt > 4U))
    {
        if (   (twoBitTime >= TWO_BIT_DUR_MIN_19200)
            && (twoBitTime <= TWO_BIT_DUR_MAX_19200))
        {
            hostBaudrate = 19200U;
        }
        else if (  (twoBitTime >= TWO_BIT_DUR_MIN_14400)
                && (twoBitTime <= TWO_BIT_DUR_MAX_14400))
        {
            hostBaudrate = 14400U;
        }
        else if (  (twoBitTime >= TWO_BIT_DUR_MIN_9600)
                && (twoBitTime <= TWO_BIT_DUR_MAX_9600))
        {
            hostBaudrate = 9600U;
        }
        else if (  (twoBitTime >= TWO_BIT_DUR_MIN_4800)
                && (twoBitTime <= TWO_BIT_DUR_MAX_4800))
        {
                hostBaudrate = 4800U;
        }
        else if (  (twoBitTime >= TWO_BIT_DUR_MIN_2400)
                && (twoBitTime <= TWO_BIT_DUR_MAX_2400))
        {
            hostBaudrate = 2400U;
        }
        else
        {
        }

        /* Check Master Baudrate against node's current baudrate */
        if (   (hostBaudrate != 0U)
            && (userCfg->baudrate != hostBaudrate))
        {
            /* Set new baudrate */
            userCfg->baudrate = hostBaudrate;
            LPUART_SetBaudrate(ins, userCfg->baudrate);

            /**
             * Assign wakeup signal to satisfy LIN Specifications specifies
             * that wakeup signal shall be in range from 250 us to 5 ms.
             */
            if (userCfg->baudrate <= 10000U)
            {
                /* Wakeup signal ranges from 400us to 4ms depend on baudrate */
                g_wakeupSignal[ins] = 0xF8U;
            }
            else
            {
                /* Wakeup signal ranges from 400us to 800us depend on baudrate */
                g_wakeupSignal[ins] = 0x80U;
            }
        }

        /* Disable baudrate evaluation process */
        states->enableBaudrateEval = false;
        states->eventId = LIN_EVENT_BAUDRATE_ADJUSTED;

        /* Callback function to handle the event */
        if (states->callback != NULL)
        {
            states->callback(ins, states);
        }

        /* Update current state and current event */
        states->eventId = LIN_EVENT_SYNC_OK;
        states->nodeState = LIN_NODE_RX_PID;
    }
}

/*!
 * @brief This function check time of double bit in sync byte for support autobaud.
 *
 * @details This is not a public API as it is called from other driver functions.
 *
 * @param ins LIN_LPUART instance number
 * @param twoBitTime two bit time with baudrate
 *
 * @retval None
 */
static void LIN_LPUART_EvalTwoBitTimeLength(
    uint32_t ins,
    uint32_t twoBitTime)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];

    if (   (states->fallingEdgeCnt > 0U)
        && (states->fallingEdgeCnt < 5U))
    {
        if (   (twoBitTime > TWO_BIT_DUR_MAX_2400)
            || (   (twoBitTime > TWO_BIT_DUR_MAX_4800)
                && (twoBitTime < TWO_BIT_DUR_MIN_2400))
            || (   (twoBitTime > TWO_BIT_DUR_MAX_9600)
                && (twoBitTime < TWO_BIT_DUR_MIN_4800))
            || (   (twoBitTime > TWO_BIT_DUR_MAX_14400)
                && (twoBitTime < TWO_BIT_DUR_MIN_9600))
            || (   (twoBitTime > TWO_BIT_DUR_MAX_19200)
                && (twoBitTime < TWO_BIT_DUR_MIN_14400))
            ||  (twoBitTime < TWO_BIT_DUR_MIN_19200))
        {
            LIN_LPUART_EnterIdleState(ins);
            states->fallingEdgeCnt = 0U;
        }
        else
        {
            if (states->fallingEdgeCnt > 1U)
            {
                if (  (twoBitTime <
                      ((100U - BIT_RATE_TOLERANCE_UNSYNC) * g_lastTwoBitTime[ins] / 100U))
                   || (twoBitTime >
                       ((100U + BIT_RATE_TOLERANCE_UNSYNC) * g_lastTwoBitTime[ins] / 100U)))
                {
                    LIN_LPUART_EnterIdleState(ins);
                    states->fallingEdgeCnt = 0U;
                }
            }
        }
        g_lastTwoBitTime[ins] = twoBitTime;
    }
    states->fallingEdgeCnt += 1U;
}

/*!
 * @brief Sends frame data out through the LIN_LPUART module using non-blocking method.
 *
 * @param ins LIN_LPUART instance number
 * @param txBuffer  source buffer containing 8-bit data chars to send
 * @param txSize  the number of bytes to send
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The transmission is successful.
 *         - STATUS_BUSY    : Operation failed due to busBusy is currently true.
 *         - STATUS_ERROR   : Operation failed due to txSize is equal to 0 or greater than 8
 *                            or node's current state is in SLEEP mode
 */
static STATUS_T LIN_LPUART_TxData(uint32_t ins, const uint8_t *txBuffer, uint8_t txSize)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];
    LPUART_T *base = g_linLpuartBase[ins];

    if (  (LIN_NODE_SLP_MODE == states->nodeState)
       || (txSize > 8U)
       || (txSize == 0U))
    {
        return STATUS_ERROR;
    }

    if (states->busBusy)
    {
        return STATUS_BUSY;
    }

    /* Make the checksum byte */
    states->checkSum = LIN_LPUART_MakeChecksumByte(ins,
                                                   txBuffer,
                                                   txSize,
                                                   states->currentPid);

    /* Add a place for checksum byte */
    states->txSize = (uint8_t)(txSize + 1U);
    states->txBuffer = txBuffer;
    states->nodeState = LIN_NODE_TX_DATA;
    states->eventId = LIN_NO_EVENT;
    states->txBusy = true;
    states->busBusy = true;
    states->byteCount = 0U;

    /* Set Break char detect length as 10 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);

    /* Start sending data */
    LPUART_HW_TxChar(base, *states->txBuffer);
    return STATUS_SUCCESS;
}

/*!
 * @brief Read status of an on-going non-blocking transmission
 *
 * @param ins LIN_LPUART instance number
 * @param bytesRemain  Number of bytes still needed to transmit
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The transmission is successful.
 *         - STATUS_BUSY    : The transmission is sending
 *         - STATUS_TIMEOUT : Operation failed due to timeout has occurred.
 */
static STATUS_T LIN_LPUART_GetTxStatus(uint32_t ins, uint8_t *bytesRemain)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    const LIN_STATE_T *states = g_linState[ins];
    *bytesRemain = (uint8_t)(states->txSize - states->byteCount);

    /* Return status of the on-going transmission */
    if ((states->eventId == LIN_NO_EVENT) && (*bytesRemain != 0U))
    {
        result = states->timeoutCntFlag ? STATUS_TIMEOUT : STATUS_BUSY;
    }
    return result;
}

/*!
 * @brief Cancel an on-going non-blocking transmission/reception.
 * @details While performing a non-blocking transferring data, users can call
 *          this function to terminate immediately the transfer.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
static STATUS_T LIN_LPUART_CancelTx(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LIN_STATE_T *states = g_linState[ins];

    /* Change node's current state to IDLE */
    LIN_LPUART_EnterIdleState(ins);

    states->rxBusy = false;
    states->txBusy = false;
    return result;
}

/*!
 * @brief Receives frame data through the LIN_LPUART module using blocking method.
 *
 * @param ins LIN_LPUART instance number
 * @param rxBuffer  buffer containing 8-bit received data
 * @param rxSize the number of bytes to receive
 * @param timeout timeout value in milliseconds
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The receives frame data is successful.
 *         - STATUS_TIMEOUT : The checksum is incorrect.
 *         - STATUS_BUSY    : Bus busy flag is true.
 *         - STATUS_ERROR   : Operation failed due is equal to 0 or greater than 8 or
 *                            node's current state is in SLEEP mode
 */
static STATUS_T LIN_LPUART_RxDataBlocking(
    uint32_t ins,
    uint8_t *rxBuffer,
    uint8_t rxSize,
    uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LIN_STATE_T *states = g_linState[ins];

    if (  (rxSize > 8U)
       || (rxSize == 0U)
       || (LIN_NODE_SLP_MODE == states->nodeState))
    {
        result = STATUS_ERROR;
    }

    if (states->rxBusy)
    {
        return STATUS_BUSY;
    }

    /* Add a place for checksum byte */
    states->rxSize = (uint8_t)(rxSize + 1U);
    states->rxBuffer = rxBuffer;
    states->byteCount = 0U;
    states->eventId = LIN_NO_EVENT;
    states->rxBlocking = true;

    /* Wait until the reception is complete */
    result = LIN_LPUART_WaitComplete(ins, timeout);

    /**
     * In case of receiving a go to sleep request, after callback, node is in
     * SLEEP MODE. In this case, node is in SLEEP MODE state.
     */
    if (states->nodeState != LIN_NODE_SLP_MODE)
    {
        /* Update node's current state to IDLE */
        states->nodeState = LIN_NODE_IDLE;
    }

    states->rxBlocking = false;
    states->busBusy = false;
    states->rxBusy = false;
    return result;
}

/*!
 * @brief Receives frame data through the LIN_LPUART module using non-blocking method.
 *
 * @param ins LIN_LPUART instance number
 * @param rxBuffer  buffer containing 8-bit received data
 * @param rxSize the number of bytes to receive
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The receives frame data is successful.
 *         - STATUS_TIMEOUT : The checksum is incorrect.
 *         - STATUS_BUSY    : Bus busy flag is true.
 *         - STATUS_ERROR   : Operation failed due is equal to 0 or greater than 8 or
 *                            node's current state is in SLEEP mode
 */
static STATUS_T LIN_LPUART_RxData(uint32_t ins, uint8_t *rxBuffer, uint8_t rxSize)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LIN_STATE_T *states = g_linState[ins];
    LPUART_T *base = g_linLpuartBase[ins];

    if (  (rxSize > 8U)
       || (rxSize == 0U)
       || (LIN_NODE_SLP_MODE == states->nodeState))
    {
        result = STATUS_ERROR;
    }

    if (states->rxBusy)
    {
        return STATUS_BUSY;
    }

    /* Add a space for checksum byte */
    states->rxSize = (uint8_t)(rxSize + 1U);
    states->rxBuffer = rxBuffer;
    states->byteCount = 0U;

    /* Start receiving data */
    states->rxBusy = true;
    states->busBusy = true;
    states->rxBlocking = false;
    states->eventId = LIN_NO_EVENT;
    states->nodeState = LIN_NODE_RX_DATA;

    /* Set Break char detect length as 10 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);
    return result;
}

/*!
 * @brief Read status of an on-going non-blocking reception
 *
 * @param ins LIN_LPUART instance number
 * @param bytesRemain  Number of bytes still needed to receive
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The reception is complete.
 *         - STATUS_TIMEOUT : The reception isn't complete.
 *         - STATUS_BUSY    : The reception is on going
 *
 */
static STATUS_T LIN_LPUART_GetRxStatus(uint32_t ins, uint8_t *bytesRemain)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;

    const LIN_STATE_T *states = g_linState[ins];
    *bytesRemain = (uint8_t)(states->rxSize - states->byteCount);

    /* Return status of the on-going reception */
    if ((states->eventId == LIN_NO_EVENT) && (*bytesRemain != 0U))
    {
        result = states->timeoutCntFlag ? STATUS_TIMEOUT : STATUS_BUSY;
    }
    return result;
}

/*!
 * @brief This function puts current node to sleep mode
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
static STATUS_T LIN_LPUART_EnterSleepMode(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    states->nodeState = LIN_NODE_SLP_MODE;
    states->busBusy = false;
    states->rxBusy  = false;
    states->txBusy  = false;

    /* Clear LPUART_RX Pin Active Edge Interrupt Flag */
    LPUART_HW_ClearStatusFlag(base, LPUART_RX_ACTIVE_EDGE_DETECT);

    /* Set Receive data not inverted */
    LPUART_HW_DisableRxDataPolarity(base);

    /* Disable RX complete interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

    /* Enable RX Input Active Edge interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_RX_ACTIVE_EDGE);

    /* Disable frame error interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);

    /* Disable LIN break detect interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_LIN_BRK_DETECT);
    return STATUS_SUCCESS;
}

/*!
 * @brief Puts current LIN node enter Idle state
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval function always return STATUS_SUCCESS
 */
static STATUS_T LIN_LPUART_EnterIdleState(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    states->eventId = LIN_NO_EVENT;

    /* Set Break char detect length as 13 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_13_BIT_MIN);

    /* Set Receive data not inverted */
    LPUART_HW_DisableRxDataPolarity(base);

    /* Enable RX complete interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_RX_DATA_REG_FULL);

    /* Disable RXEDG interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_RX_ACTIVE_EDGE);

    /* Enable frame error interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_FRAME_ERR_FLAG);

    /* Enable LIN break detect interrupt */
    LPUART_HW_EnableIntMode(base, LPUART_INT_LIN_BRK_DETECT);

    states->busBusy = false;
    states->nodeState = LIN_NODE_IDLE;
    return STATUS_SUCCESS;
}

/*!
 * @brief Sends a wakeup signal through the LIN_LPUART interface
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : Bus busy flag is false.
 *         - STATUS_BUSY    : Bus busy flag is true.
 *
 */
static STATUS_T LIN_LPUART_TxWakeupSignal(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LPUART_T *base = g_linLpuartBase[ins];
    const LIN_STATE_T *states = g_linState[ins];

    /* Check if bus is not busy */
    if (states->busBusy)
    {
        return STATUS_BUSY;
    }

    /* Send a wakeup signal */
    LPUART_HW_TxChar(base, g_wakeupSignal[ins]);
    return STATUS_SUCCESS;
}

/*!
 * @brief Read the current LIN node state
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval current LIN node state
 *
 */
static LIN_NODE_STATE_T LIN_LPUART_GetNodeState(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_NODE_STATE_T result = LIN_NODE_UNINIT;
    const LIN_STATE_T *states = g_linState[ins];

    if (states != NULL)
    {
        result = states->nodeState;
    }
    return result;
}

/*!
 * @brief Callback function for Timer Interrupt Handler
 *
 * @details Users shall initialize a timer (for example CFGTMR) in Output compare mode
 *          with period of 500 micro seconds. In timer IRQ handler, call this function.
 *
 * @param ins LIN_LPUART instance number
 * @retval None
 */
static void LIN_LPUART_TimeoutProcess(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];
    LIN_NODE_STATE_T nodeStatus = states->nodeState;

    if (nodeStatus == LIN_NODE_RX_DATA)
    {
        if (states->timeoutCnt != 0U)
        {
            states->timeoutCnt--;
        }
        else
        {
            states->timeoutCntFlag = true;

            /* Check if the reception is non-blocking */
            if (states->rxBlocking == false)
            {
                /* Callback to handle timeout counter flag */
                if (states->callback != NULL)
                {
                    states->callback(ins, states);
                }

                /* Clear Rx busy flag */
                states->rxBusy = false;

                /* Change the node's current Status to IDLE */
                LIN_LPUART_EnterIdleState(ins);
            }
        }
    }
    else if (nodeStatus == LIN_NODE_TX_DATA)
    {
        if (states->timeoutCnt != 0U)
        {
            states->timeoutCnt--;
        }
        else
        {
            states->timeoutCntFlag = true;

            if (states->txBlocking == false)
            {
                /* Callback to handle timeout counter flag */
                if (states->callback != NULL)
                {
                    states->callback(ins, states);
                }

                /* Clear Tx busy flag */
                states->txBusy = false;

                /* Change the node's current Status to IDLE */
                LIN_LPUART_EnterIdleState(ins);
            }
        }
    }
}

/*!
 * @brief Config Value for Timeout Counter that is used in LIN_LPUART_TimeoutProcess
 *
 * @param ins LPUART ins number
 * @param timeout  Timeout Value to be set
 *
 * @retval None
 */
static void LIN_LPUART_SetTimeoutCnt(uint32_t ins, uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    LIN_STATE_T *states = g_linState[ins];
    states->timeoutCnt = timeout;
    states->timeoutCntFlag = false;
}

/*!
 * @brief Sends frame header out through the LIN_LPUART module using a non-blocking method
 *
 * @param ins LIN_LPUART instance number.
 * @param id Frame Identifier.
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The transmission is successful.
 *         - STATUS_BUSY    : Bus busy flag is true.
 *         - STATUS_ERROR   : The interface isn't Master or id isn't in range from 0 to 0x3F
 *                            or node's current state is in SLEEP mode.
 */
static STATUS_T LIN_LPUART_MasterTxHeader(uint32_t ins, uint8_t id)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    if (  (states->nodeState == LIN_NODE_SLP_MODE)
       || (userCfg->nodeType == LIN_SLAVE)
       || (id > 0x3FU))
    {
        return STATUS_ERROR;
    }

    if (states->busBusy)
    {
        return STATUS_BUSY;
    }

    states->currentId = id;
    states->currentPid = LIN_ProcessParity(id, LIN_MAKE_PARITY);

    /* Set LIN current state to sending Break field */
    states->busBusy = true;
    states->eventId = LIN_NO_EVENT;
    states->nodeState = LIN_NODE_TX_BRK_FIELD;

    /* Set Break char detect length as 13 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_13_BIT_MIN);
    LPUART_HW_EnableIntMode(base, LPUART_INT_LIN_BRK_DETECT);

    /* Send break char by using queue mode */
    LPUART_HW_QueueBreakField(base);
    return STATUS_SUCCESS;
}

/*!
 * @brief LIN_LPUART capture time interval to set baudrate automatically
 *
 * @details when enable autobaud feature.
 *          This function should only be used in Slave.
 *          The timer should be in input capture mode of both rising and falling edges.
 *          The timer input capture pin should be externally connected to RXD pin.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_BUSY:    Operation is running.
 *        - STATUS_ERROR:   Operation failed due to break char incorrect,
 *                          wakeup signal incorrect
 *                          or calculate baudrate failed.
 */
static STATUS_T LIN_LPUART_AutoBaudCapture(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_BUSY;
    uint32_t tempTime = 0U;
    const LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    if (!states->enableBaudrateEval)
    {
        if (states->fallingEdgeCnt > 4U)
        {
            /* Enable the LPUART transmitter and receiver */
            LPUART_HW_EnableTx(base);
            LPUART_HW_EnableRx(base);
            states->fallingEdgeCnt = 0U;
        }
        result = STATUS_SUCCESS;
    }
    else
    {
        /* Calculate time between two bit (for service autobaud) */
        userCfg->getTimeIntervalCallback(&tempTime);

        /* Get two bits time length */
        g_timeMeasure[ins] += tempTime;
        g_cntMeasure[ins]++;

        if ((g_cntMeasure[ins] > 1U))
        {
            if (states->nodeState == LIN_NODE_IDLE)
            {
                /* Check break time minimum */
                if (tempTime < AUTOBAUD_BRK_TIME_MIN)
                {
                    g_cntMeasure[ins] = 0U;
                    result = STATUS_ERROR;
                }
                else
                {
                    /* Set Break char detect length as 10 bits minimum */
                    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);

                    /* Disable LIN Break Detect Interrupt */
                    LPUART_HW_DisableIntMode(base, LPUART_INT_LIN_BRK_DETECT);

                    /* Set flag LIN bus busy */
                    states->busBusy = true;

                    /* Change the node's current state to RECEIVED BREAK FIELD */
                    states->eventId = LIN_EVENT_RX_BRK_FIELD_OK;

                    /* Callback function */
                    if (states->callback != NULL)
                    {
                        states->callback(ins, states);
                    }

                    /* Change the node's current state to RECEIVING SYNC FIELD */
                    states->nodeState = LIN_NODE_RX_SYNC;

                    /* Start Autobaud Count(initialize number of measurements in sync byte) */
                    states->fallingEdgeCnt = 0U;
                    g_cntMeasure[ins] = 1U;
                }

            }
            else if (states->nodeState == LIN_NODE_SLP_MODE)
            {
                if (tempTime < 150000U)
                {
                    result = STATUS_ERROR;
                }
                else
                {
                    states->eventId = LIN_EVENT_WAKEUP_SIGNAL;

                    /* Callback to handle event: Received a wakeup signal */
                    if (states->callback != NULL)
                    {
                        states->callback(ins, states);
                    }

                    /* Change node's state to IDLE */
                    LIN_LPUART_EnterIdleState(ins);
                }
                g_cntMeasure[ins] = 0U;
            }
            /* If current state is RECEIVE SYNC */
            else
            {
                /* Calculate baudrate */
                LIN_LPUART_AutobaudTimerValEval(ins, g_timeMeasure[ins]);

                /* Reset to measure in next times */
                g_cntMeasure[ins] = 0U;
                g_timeMeasure[ins] = 0U;

                if (states->nodeState == LIN_NODE_IDLE)
                {
                    result = STATUS_ERROR;
                }
            }

        }
    }
    return result;
}

/*!
 * @brief Sends Frame data out through the LIN_LPUART module using blocking method.
 *
 * @param ins LIN_LPUART instance number
 * @param txBuffer  source buffer containing 8-bit data chars to send
 * @param txSize the number of bytes to send
 * @param timeout timeout value in milliseconds
 *
 * @retval operation status:
 *         - STATUS_SUCCESS : The transmission is successful.
 *         - STATUS_TIMEOUT : The transmission isn't successful.
 *
 */
static STATUS_T LIN_LPUART_TxDataBlocking(
    uint32_t ins,
    const uint8_t *txBuffer,
    uint8_t txSize,
    uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    if (   (LIN_NODE_SLP_MODE == states->nodeState)
        || (8U < txSize)
        || (0U == txSize))
    {
        return STATUS_ERROR;
    }

    if (states->busBusy)
    {
        return STATUS_BUSY;
    }

    /* Make the checksum byte */
    states->checkSum = LIN_LPUART_MakeChecksumByte(ins,
                                                   txBuffer,
                                                   txSize,
                                                   states->currentPid);

    /* Add a place for checksum byte */
    states->txSize = (uint8_t)(txSize + 1U);
    states->txBuffer = txBuffer;
    states->txBusy = true;
    states->busBusy = true;
    states->txBlocking = true;
    states->byteCount = 0U;
    states->eventId = LIN_NO_EVENT;

    /* Set Break char detect length as 10 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);

    states->nodeState = LIN_NODE_TX_DATA;

    /* Start sending data */
    LPUART_HW_TxChar(base, *states->txBuffer);

    /* Wait until the transmission is complete */
    result = LIN_LPUART_WaitComplete(ins, timeout);

    states->txBlocking = false;
    states->txBusy = false;

    /* Change node's current state to IDLE */
    LIN_LPUART_EnterIdleState(ins);
    return result;
}

/*!
 * @brief Installs callback function that is used for LIN_LPUART_IRQHandler.
 *
 * @param ins The LIN_LPUART instance number.
 * @param buff source buffer containing 8-bit data chars to send
 * @param bufSize the number of bytes to send
 * @param PID calculate pid for a frame
 *
 * @retval classic or enhanced checksum base on data in g_linUserConfig[ins] and input parameter.
 *
 */
static uint8_t LIN_LPUART_MakeChecksumByte(
    uint32_t ins,
    const uint8_t *buff,
    uint8_t bufSize,
    uint8_t pid)
{
    LPUART_INSTANCE_VALIDITY(ins);

    /* Read list of PIDs use classic checksum total */
    uint8_t checksum = pid;
    uint8_t result = 0U;
    const uint8_t *classicPID = g_linUserConfig[ins]->classicPID;
    const uint8_t numClassicPID = g_linUserConfig[ins]->numOfClassicPID;

    if (numClassicPID != 255U)
    {
        if (classicPID != NULL)
        {
            for (result = 0U; result < numClassicPID; result++)
            {
                if (checksum == classicPID[result])
                {
                    checksum = 0U;
                    break;
                }
            }
        }
    }
    else
    {
        /* All frame use enhanced checksum */
        checksum = 0U;
    }
    result = LIN_CalculateChecksum(buff, bufSize, checksum);
    return result;
}

/*!
 * @brief waits until transmission/reception is complete
 *
 * @param ins LIN_LPUART instance number
 * @param timeout timeout value in milliseconds
 *
 * @retval returns status of the transaction
 */
static STATUS_T LIN_LPUART_WaitComplete(uint32_t ins, uint32_t timeout)
{
    LPUART_INSTANCE_VALIDITY(ins);

    STATUS_T result = STATUS_SUCCESS;
    LIN_STATE_T *states = g_linState[ins];

    if (states->rxBlocking)
    {
        if (OSIF_SemWait(&states->rxCompleted, timeout) == STATUS_TIMEOUT)
        {
            result = STATUS_TIMEOUT;
        }
    }
    if (states->txBlocking == true)
    {
        if (OSIF_SemWait(&states->txCompleted, timeout) == STATUS_TIMEOUT)
        {
            result = STATUS_TIMEOUT;
        }
    }
    return result;
}

/*!
 * @brief process break detect for LIN communication.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval None
 */
static void LIN_LPUART_ProcessBreakDetect(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    const LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    /* Clear LIN Break Detect Interrupt Flag */
    LPUART_HW_ClearStatusFlag(base, LPUART_LIN_BREAK_DETECT);

    /* Set Break char detect length as 10 bits minimum */
    LPUART_HW_SetBreakCharDetectLen(base, LPUART_BRK_CHAR_10_BIT_MIN);

    /* Disable LIN Break Detect Interrupt */
    LPUART_HW_DisableIntMode(base, LPUART_INT_LIN_BRK_DETECT);

    if (userCfg->nodeType == LIN_SLAVE)
    {
        /* Set flag LIN bus busy */
        states->busBusy = true;

        /* Change the node's current state to RECEIVED BREAK FIELD */
        states->eventId = LIN_EVENT_RX_BRK_FIELD_OK;

        if (states->callback != NULL)
        {
            states->callback(ins, states);
        }
        /* Change the node's current state to RECEIVING SYNC FIELD */
        states->nodeState = LIN_NODE_RX_SYNC;
    }
    /* If the current node is MASTER */
    else
    {
        /* Check if LIN current node state is LIN_NODE_TX_BRK_FIELD */
        if (states->nodeState == LIN_NODE_TX_BRK_FIELD)
        {
            states->busBusy = true;

            /* Change the node's current state to SENDING PID to send PID after send SYNC */
            states->nodeState = LIN_NODE_TX_PID;

            /* Send Sync Field 0x55 */
            LPUART_HW_TxChar(base, 0x55);
        }
    }
}

/*!
 * @brief check if a dominant signal received is a wakeup signal.
 *
 * @param ins LIN_LPUART instance number
 *
 * @retval None
 */
static void LIN_LPUART_CheckWakeupSignal(uint32_t ins)
{
    LPUART_INSTANCE_VALIDITY(ins);

    uint32_t wakeupSignalSize = 0U;
    const LIN_CFG_T *userCfg = g_linUserConfig[ins];
    LPUART_T *base = g_linLpuartBase[ins];
    LIN_STATE_T *states = g_linState[ins];

    if (LPUART_HW_GetRxDataPolarity(base))
    {
        /* Set Receive Data is Not Inverted */
        LPUART_HW_DisableRxDataPolarity(base);

        /* Calculate time interval between the falling and rising edge */
        userCfg->getTimeIntervalCallback(&wakeupSignalSize);

        /* If length of the dominant signal is longer than 150us, it is a wakeup signal */
        if (wakeupSignalSize >= 150000U)
        {
            states->eventId = LIN_EVENT_WAKEUP_SIGNAL;

            /* Callback to handle event: Received a wakeup signal */
            if (states->callback != NULL)
            {
                states->callback(ins, states);
            }
            LIN_LPUART_EnterIdleState(ins);
        }
    }
    /* if LPUART_GetRxDataPolarity is 0: Receive Data is not inverted */
    else
    {
        /* Start measure time */
        userCfg->getTimeIntervalCallback(&wakeupSignalSize);

        /* Set Receive Data Inverted */
        LPUART_HW_EnableRxDataPolarity(base);
    }
}

/**@} end of group LIN_Functions*/
/**@} end of group LIN_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
