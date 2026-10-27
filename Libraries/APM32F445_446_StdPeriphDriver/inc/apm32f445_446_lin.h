/*!
 * @file        apm32f445_446_lin.h
 *
 * @brief       This file contains all the functions prototypes for the LIN firmware library.
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

#ifndef APM32F445_446_LIN_H
#define APM32F445_446_LIN_H

/* Includes */
#include "apm_core_cm4.h"
#include "device.h"
#include "apm32f445_446_interrupt.h"
#include "osif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LIN_Driver LIN Driver
  @{
*/

/** @defgroup LIN_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

 /* LIN node type */
typedef enum {
    LIN_MASTER  = 1U,
    LIN_SLAVE   = 2U
} LIN_NODE_TYPE_T;

/* LIN node state */
typedef enum {
    LIN_NODE_UNINIT             = 0x00U,    /* Uninitialized state */
    LIN_NODE_SLP_MODE           = 0x01U,    /* Sleep mode state */
    LIN_NODE_IDLE               = 0x02U,    /* Idle state */
    LIN_NODE_TX_BRK_FIELD       = 0x03U,    /* Send break field state */
    LIN_NODE_RX_SYNC            = 0x04U,    /* Receive the synchronization byte state */
    LIN_NODE_TX_PID             = 0x05U,    /* Send PID state */
    LIN_NODE_RX_PID             = 0x06U,    /* Receive PID state */
    LIN_NODE_RX_DATA            = 0x07U,    /* Receive data state */
    LIN_NODE_RX_DATA_FINISHED   = 0x08U,    /* Receive data completed state */
    LIN_NODE_TX_DATA            = 0x09U,    /* Send data state */
    LIN_NODE_TX_DATA_FINISHED   = 0x0AU     /* Send data completed state */
} LIN_NODE_STATE_T;

/* LIN events */
typedef enum {
    LIN_NO_EVENT                = 0x00U,    /* No event yet */
    LIN_EVENT_WAKEUP_SIGNAL     = 0x01U,    /* Received a wakeup signal */
    LIN_EVENT_BAUDRATE_ADJUSTED = 0x02U,    /* Indicate that baudrate was adjusted to Master's baudrate */
    LIN_EVENT_RX_BRK_FIELD_OK   = 0x03U,    /* Indicate that correct Break Field was received */
    LIN_EVENT_SYNC_OK           = 0x04U,    /* Sync byte is correct */
    LIN_EVENT_SYNC_ERROR        = 0x05U,    /* Sync byte is incorrect */
    LIN_EVENT_PID_OK            = 0x06U,    /* PID correct */
    LIN_EVENT_PID_ERROR         = 0x07U,    /* PID incorrect */
    LIN_EVENT_FRAME_ERROR       = 0x08U,    /* Framing Error */
    LIN_EVENT_READBACK_ERROR    = 0x09U,    /* Readback data is incorrect */
    LIN_EVENT_CHKSUM_ERROR      = 0x0AU,    /* Checksum byte is incorrect */
    LIN_EVENT_TX_FINISHED       = 0x0BU,    /* Sending data completed */
    LIN_EVENT_RX_FINISHED       = 0x0CU,    /* Receiving data completed */
    LIN_EVENT_RX_OVERRUN        = 0x0DU     /* RX overrun flag */
} LIN_EVENT_ID_T;

/**@} end of group LIN_Enumerations*/

/** @defgroup LIN_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/* LIN Driver callback function */
typedef void (* LIN_CALLBACK_T)(uint32_t ins, void *linState);

/* Callback function to get time interval in nanoseconds */
typedef uint32_t (* LIN_GET_TIME_INTERVAL_T)(uint32_t *nanoSeconds);

/* LIN configuration structure */
typedef struct
{
    LIN_NODE_TYPE_T nodeType;   /* Node type as Master or Slave */
    bool autobaudEnable;        /* Enable Autobaud feature */
    uint32_t baudrate;          /* baudrate of LIN Hardware Interface to configure */
    uint8_t numOfClassicPID;    /* Number of PIDs use classic checksum */
    uint8_t *classicPID;        /* List of PIDs use classic checksum */
    LIN_GET_TIME_INTERVAL_T getTimeIntervalCallback;    /* Callback function to get time interval in nanoseconds */
} LIN_CFG_T;

/**
 * Runtime state of the LIN driver. Note that the driver does not statically
 * allocate memory, so caller provides memory for the driver state structure
 * during initialization.
*/
typedef struct
{
    volatile bool rxBusy;                   /* True if the LIN interface is receiving frame data */
    volatile bool txBusy;                   /* True if the LIN interface is transmitting frame data */
    volatile bool busBusy;                  /* True if there are data, frame headers being transferred on bus */
    volatile bool rxBlocking;               /* True if receive is blocking transaction */
    volatile bool txBlocking;               /* True if transmit is blocking transaction */
    volatile uint8_t rxSize;                /* The remaining number of bytes to be received */
    volatile uint8_t txSize;                /* The remaining number of bytes to be transmitted */
    volatile uint8_t fallingEdgeCnt;        /* Falling Edge count of a sync byte */
    uint8_t checkSum;                       /* Checksum byte */
    uint8_t byteCount;                      /* To count number of bytes already transmitted or received */
    uint8_t currentId;                      /* ID */
    uint8_t currentPid;                     /* PID */
    uint8_t *rxBuffer;                      /* The buffer of received data */
    const uint8_t * txBuffer;               /* The buffer of data being sent */
    volatile LIN_EVENT_ID_T eventId;        /* ID Event */
    volatile LIN_NODE_STATE_T nodeState;    /* Current Node state */
    volatile uint32_t timeoutCnt;           /* Value of timeout counter */
    volatile bool timeoutCntFlag;           /* Timeout counter flag */
    volatile bool enableBaudrateEval;       /* Baudrate Evaluation Process Enable */
    SEMAPHORE_T rxCompleted;                /* Used to wait for LIN interface ISR to reception done*/
    SEMAPHORE_T txCompleted;                /* Used to wait for LIN interface ISR transmission done.*/
    uint32_t sourceClockFreq;               /* Frequency of the source clock for LIN */
    LIN_CALLBACK_T callback;                /* Callback function to invoke after receiving a byte or transmitting a byte */
} LIN_STATE_T;

/**@} end of group LIN_Structures*/

/** @defgroup LIN_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T LIN_Init(uint32_t ins, LIN_CFG_T *userCfg, LIN_STATE_T *linState);
void LIN_Deinit(uint32_t ins);

STATUS_T LIN_RxData(uint32_t ins, uint8_t *rxBuffer, uint8_t rxSize);
STATUS_T LIN_RxDataBlocking(
    uint32_t ins,
    uint8_t *rxBuffer,
    uint8_t rxSize,
    uint32_t timeout);
STATUS_T LIN_GetRxStatus(uint32_t ins, uint8_t *bytesRemain);

STATUS_T LIN_TxData(uint32_t ins, const uint8_t *txBuffer, uint8_t txSize);
STATUS_T LIN_TxDataBlocking(
    uint32_t ins,
    const uint8_t *txBuffer,
    uint8_t txSize,
    uint32_t timeout);
STATUS_T LIN_GetTxStatus(uint32_t ins, uint8_t *bytesRemain);
STATUS_T LIN_CancelTx(uint32_t ins);
STATUS_T LIN_MasterTxHeader(uint32_t ins, uint8_t id);

STATUS_T LIN_EnterSleepMode(uint32_t ins);
STATUS_T LIN_EnterIdleState(uint32_t ins);
STATUS_T LIN_SendWakeupSignal(uint32_t ins);
LIN_NODE_STATE_T LIN_GetNodeState(uint32_t ins);

STATUS_T LIN_AutoBaudCapture(uint32_t ins);
uint8_t LIN_ProcessParity(uint8_t PID, uint8_t action);
uint8_t LIN_CalculateChecksum(const uint8_t *buffer, uint8_t size, uint8_t PID);

void LIN_TimeoutProcess(uint32_t ins);
void LIN_SetTimeoutCounter(uint32_t ins, uint32_t timeout);

void LIN_DefaultConfig(bool isMaster, LIN_CFG_T *userCfg);
LIN_CALLBACK_T LIN_InstallCallback(uint32_t ins, LIN_CALLBACK_T callback);

STATUS_T LIN_EnableIRQ(uint32_t ins);
STATUS_T LIN_DisableIRQ(uint32_t ins);
void LIN_IRQHandler(uint32_t ins);

/**@} end of group LIN_Functions*/
/**@} end of group LIN_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_LIN_H */
