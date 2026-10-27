/*
 * @file        apm32f445_446_can.h
 *
 * @brief       This file provides all the CAN firmware functions
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


#ifndef APM32F445_446_CAN_H
#define APM32F445_446_CAN_H

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "apm32f445_446_clock.h"
#include "osif.h"

#include "apm32f445_446_dma.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CAN_Driver CAN Driver
  @{
*/

/** @defgroup CAN_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define CAN_INSTANCE_COUNT  (3U)

/**@} end of group CAN_Macros*/

/** @defgroup CAN_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* CAN controller operation modes */
typedef enum {
    CAN_MODE_NORMAL,        /* Normal mode or user mode */
    CAN_MODE_LISTEN_ONLY,   /* Listen-only mode */
    CAN_MODE_LOOPBACK,      /* Loopback mode */
    CAN_MODE_FREEZE,        /* Freeze mode */
    CAN_MODE_DISABLE        /* Disable mode */
} CAN_OPERATION_MODE_T;

/* CAN message ID type */
typedef enum {
    CAN_ID_STANDARD,        /* Standard ID */
    CAN_ID_EXTENDED         /* Extended ID */
} CAN_MSG_ID_TYPE_T;

/* MB state (idle, Rx busy, Tx busy) */
typedef enum {
    CAN_MB_STATE_IDLE,      /* The MB is not used by any transfer */
    CAN_MB_STATE_RX_BUSY,   /* The MB is used for a reception */
    CAN_MB_STATE_TX_BUSY,   /* The MB is used for a transmission */
    CAN_MB_STATE_DMA_ERROR  /* The MB is used as DMA source and failed to transfer */
} CAN_MB_STATE_T;

/* CAN Rx mask type */
typedef enum {
    CAN_RX_MASK_GLOBAL,      /* Rx global mask */
    CAN_RX_MASK_INDIVIDUAL   /* Rx individual mask */
} CAN_RX_MASK_TYPE_T;

/* Event type which occurred when the callback was invoked */
typedef enum {
    CAN_EVENT_TX_COMPLETE,      /* A frame was sent from the configured Tx MB */
    CAN_EVENT_RX_COMPLETE,      /* A frame was received in the configured Rx MB */
    CAN_EVENT_RXFIFO_COMPLETE,  /* A frame was received in the RxFIFO */
    CAN_EVENT_RXFIFO_WARNING,   /* RxFIFO is almost full (5 frames) */
    CAN_EVENT_RXFIFO_OVERFLOW,  /* RxFIFO is full (incoming message was lost) */
    CAN_EVENT_WAKEUP_MATCH,     /* An wakeup event occurred due to matching */
    CAN_EVENT_WAKEUP_TIMEOUT,   /* An wakeup event occurred due to timeout */
    CAN_EVENT_DMA_COMPLETE,     /* A complete transfer occurred on DMA */
    CAN_EVENT_DMA_ERROR,        /* A DMA transfer failed due to DMA channel error */
    CAN_EVENT_ERROR
} CAN_EVENT_TYPE_T;

/* RxFIFO transfer type (interrupt or DMA) */
typedef enum {
    CAN_RXFIFO_USE_INTERRUPT,   /* Use interrupts for RxFIFO */

    CAN_RXFIFO_USE_DMA          /* Use DMA for RxFIFO */
} CAN_RXFIFO_TRANSFER_TYPE_T;

/* CAN RxFIFO filter numbers */
typedef enum {
    CAN_RXFIFO_ID_FILTERS_8   = 0x00U,
    CAN_RXFIFO_ID_FILTERS_16  = 0x01U,
    CAN_RXFIFO_ID_FILTERS_24  = 0x02U,
    CAN_RXFIFO_ID_FILTERS_32  = 0x03U,
    CAN_RXFIFO_ID_FILTERS_40  = 0x04U,
    CAN_RXFIFO_ID_FILTERS_48  = 0x05U,
    CAN_RXFIFO_ID_FILTERS_56  = 0x06U,
    CAN_RXFIFO_ID_FILTERS_64  = 0x07U,
    CAN_RXFIFO_ID_FILTERS_72  = 0x08U,
    CAN_RXFIFO_ID_FILTERS_80  = 0x09U,
    CAN_RXFIFO_ID_FILTERS_88  = 0x0AU,
    CAN_RXFIFO_ID_FILTERS_96  = 0x0BU,
    CAN_RXFIFO_ID_FILTERS_104 = 0x0CU,
    CAN_RXFIFO_ID_FILTERS_112 = 0x0DU,
    CAN_RXFIFO_ID_FILTERS_120 = 0x0EU,
    CAN_RXFIFO_ID_FILTERS_128 = 0x0FU
} CAN_RXFIFO_FILTER_NUM_T;

/* CAN RxFIFO ID formats */
typedef enum {
    CAN_RXFIFO_ID_FORMAT_A, /* One full ID (standard and extended) per ID Filter Table element */
    CAN_RXFIFO_ID_FORMAT_B, /* Two full standard IDs or two partial 14-bit (standard and extended)
                                   IDs per ID Filter Table element */
    CAN_RXFIFO_ID_FORMAT_C, /* Four partial 8-bit Standard IDs per ID Filter Table element */
    CAN_RXFIFO_ID_FORMAT_D  /* All frames rejected */
} CAN_RXFIFO_ID_ELEMENT_FORMAT_T;

/* Message buffer CODE for Rx buffers */
enum {
    CAN_CODE_RX_INACTIVE = 0x00U,   /* MB is not active */
    CAN_CODE_RX_BUSY     = 0x01U,   /* Updating MB contents, the CPU must not access the MB */
    CAN_CODE_RX_FULL     = 0x02U,   /* MB is full */
    CAN_CODE_RX_EMPTY    = 0x04U,   /* MB is active and empty */
    CAN_CODE_RX_OVERRUN  = 0x06U,   /* MB is overwritten into a full buffer */
    CAN_CODE_RX_RANSWER  = 0x0AU,   /* A frame was configured to recognize a remote request
                                           frame and transmit a response frame in return */
    CAN_CODE_RX_NOT_USED = 0x0FU    /* Not used */
};

/* Message buffer CODE for Tx buffers */
enum {
    CAN_CODE_TX_INACTIVE  = 0x08U,  /* MB is not active */
    CAN_CODE_TX_ABORT     = 0x09U,  /* MB is aborted */
    CAN_CODE_TX_DATA      = 0x0CU,  /* MB is a Tx data frame */
    CAN_CODE_TX_REMOTE    = 0x1CU,  /* MB is a Tx remote request frame */
    CAN_CODE_TX_TANSWER   = 0x0EU,  /* MB is a TX response request frame from
                                           an incoming remote request frame */
    CAN_CODE_TX_NOT_USED  = 0x0FU   /* Not used */
};

/* CAN error interrupt types */
typedef enum {
    CAN_INTERRUPT_RX_WARNING  = 0x400u,     /* RX warning interrupt */
    CAN_INTERRUPT_TX_WARNING  = 0x800u,     /* TX warning interrupt */
    CAN_INTERRUPT_ERROR       = 0x4000u,    /* Error interrupt */
    CAN_INTERRUPT_BUS_OFF     = 0x8000u,    /* Bus off interrupt */
} CAN_INTERRUPT_TYPE_T;

/* CANFD payload sizes */
typedef enum {
    CAN_FD_PAYLOAD_SIZE_8 = 0,
    CAN_FD_PAYLOAD_SIZE_16,
    CAN_FD_PAYLOAD_SIZE_32,
    CAN_FD_PAYLOAD_SIZE_64
} CAN_FD_PAYLOAD_SIZE_T;

/* Pretended Networking filtering types */
typedef enum {
    CAN_PN_FILTER_ID,                  /* Filter message ID only */
    CAN_PN_FILTER_ID_PAYLOAD,          /* Filter message ID and payload */
    CAN_PN_FILTER_ID_NTIMES,           /* Filter message ID occurring a number of times */
    CAN_PN_FILTER_ID_PAYLOAD_NTIMES    /* Filter message ID and payload occurring a number of times */
} CAN_PN_FILTER_TYPE_T;

/* Pretended Networking matching schemes */
typedef enum {
    CAN_PN_MATCH_EXACT, /* Match an exact target value */
    CAN_PN_MATCH_GE,    /* Match greater than or equal to a specified target value */
    CAN_PN_MATCH_LE,    /* Match less than or equal to a specified target value */
    CAN_PN_MATCH_RANGE  /* Match inside a range, greater than or equal to the lower
                               limit and smaller than or equal to the upper limit */
} CAN_PN_MATCH_SCHEME_T;

/* CAN PE clock sources */
typedef enum {
    CAN_CLOCK_SOURCE_OSC    = 0U,   /* CAN engine clock source is oscillator clock */
    CAN_CLOCK_SOURCE_PER    = 1U    /* CAN engine clock source is peripheral clock */
} CAN_CLK_SOURCE_T;

/**@} end of group CAN_Enumerations*/

/** @defgroup CAN_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/* CAN message buffer structure */
typedef struct {
    uint32_t cs;            /* Code and status */
    uint32_t messageId;     /* Message ID */
    uint8_t data[64];       /* Data buffer */
    uint8_t dataLen;        /* Data length in bytes */
} CAN_MSG_BUF_T;

/* Information for MB internal handling */
typedef struct {
    CAN_MSG_BUF_T *msgBuf;          /* Message buffer */
    SEMAPHORE_T sem;                    /* Semaphore used for signaling completion of a blocking transfer */
    volatile CAN_MB_STATE_T state;  /* State of the MB (idle/Rx busy/Tx busy) */
    bool isBlocking;                    /* If the transfer is blocking */
    bool isRemote;                      /* If the frame is a remote frame */
} CAN_MB_HANDLE_T;

/* Internal driver state information */
typedef struct CAN_STATE {
    /* MB information array */
    CAN_MB_HANDLE_T mbHandles[FEATURE_CAN_MAX_MB_NUM];

    /* IRQ handler callback function */
    void (*callback)(uint8_t instance,
                     CAN_EVENT_TYPE_T eventType,
                     uint32_t mbIndex,
                     struct CAN_STATE *driverState);
    /* Parameter used to pass user data when invoking the callback function */
    void *callbackParam;

    /* Error IRQ handler callback function */
    void (*errorCallback)(uint8_t instance,
                          CAN_EVENT_TYPE_T eventType,
                          struct CAN_STATE *driverState);
    /* Parameter used to pass user data when invoking the error callback function */
    void *errorCallbackParam;

    /* RxFIFO transfer type (interrupt or DMA) */
    CAN_RXFIFO_TRANSFER_TYPE_T rxFifoTransferType;
    uint8_t rxFifoDmaChannel;   /* DMA channel number used by RxFIFO */
} CAN_STATE_T;

/* CAN message buffer code and status for transmit and receive */
typedef struct {
    uint32_t code;                      /* MB code */
    CAN_MSG_ID_TYPE_T msgIdType;    /* Type of message ID (standard or extended) */
    uint32_t dataLen;                   /* Data length in bytes */
    bool fdEnable;                      /* Enable or disable CANFD */
    uint8_t fdPadding;                  /* CANFD padding value */
    bool brsEnable;                     /* Enable bitrate switch in the data phase */
} CAN_MB_CODE_STATUS_T;

/* CAN data info from user */
typedef struct {
    CAN_MSG_ID_TYPE_T msgIdType;    /* Type of message ID (standard or extended) */
    uint32_t dataLen;                   /* Data length in bytes */
    bool fdEnable;                      /* Enable or disable CANFD */
    uint8_t fdPadding;                  /* CANFD padding value */
    bool brsEnable;                     /* Enable bitrate switch in the data phase */
    bool isRemote;                      /* If this is remote frame or data frame */
} CAN_DATA_INFO_T;

/* RxFIFO ID filter table structure */
typedef struct {
    bool isExtended;        /* Extended frame */
    bool isRemote;          /* Remote frame */
    uint32_t idFilter;      /* ID filter */
} CAN_RXFIFO_ID_FILTER_T;

/* CAN bitrate settings */
typedef struct {
    uint32_t propSeg;           /* Propagation time segment */
    uint32_t phaseSeg1;         /* Phase buffer segment 1 */
    uint32_t phaseSeg2;         /* Phase buffer segment 2 */
    uint32_t preDiv;            /* Prescaler division factor */
    uint32_t resyncJumpWidth;   /* Resynchronized jump width */
} CAN_TIME_SEGMENT_T;

/* CAN controller user configurations */
typedef struct {
    uint32_t maxMbNumber;                       /* The maximum number of Message Buffers */
    CAN_TIME_SEGMENT_T bitrate;             /* Bitrate used for standard frames or for CANFD arbitration phase */
    CAN_OPERATION_MODE_T operationMode;     /* Operation mode of the CAN controller */
    bool rxFifoEnable;                                  /* RxFIFO is enabled or not */
    CAN_RXFIFO_FILTER_NUM_T rxFifoFilterNum;        /* The number of RxFIFO ID filters */
    CAN_RXFIFO_TRANSFER_TYPE_T rxFifoTransferType;  /* RxFIFO transfer type (interrupt or DMA) */
    bool fdEnable;                              /* Enable or disable CANFD */
    CAN_FD_PAYLOAD_SIZE_T fdPayloadSize;    /* Payload size of the mailboxes in bytes */
    CAN_TIME_SEGMENT_T fdBitrate;           /* Bitrate used for CANFD data phase */
    CAN_CLK_SOURCE_T clockSource;           /* CAN clock source */
    uint8_t rxFifoDmaChannel;                   /* DMA channel number used by RxFIFO */
} CAN_USER_CONFIG_T;

/* Pretended Networking ID filter */
typedef struct {
    bool isExtended;    /* Extended frame */
    bool isRemote;      /* Remote frame */
    uint32_t idFilter;  /* ID filter */
} CAN_PN_ID_FILTER_T;

/* Pretended Networking payload filter */
typedef struct {
    uint8_t minPayloadLen;      /* Lower limit of the payload size */
    uint8_t maxPayloadLen;      /* Upper limit of the payload size */
    uint8_t payloadFilter1[8U]; /* The payload to be matched (for MATCH_EXACT),
                                   the lower limit (for MATCH_GEQ and MATCH_RANGE) or
                                   the upper limit (for MATCH_LEQ) */
    uint8_t payloadFilter2[8U]; /* The mask (for MATCH_EXACT) or the upper limit (for MATCH_RANGE) */
} CAN_PN_PAYLOAD_FILTER_T;

/* Pretended Networking configurations */
typedef struct {
    bool wakeupByMatch;                             /* If wakeup event is triggered on match */
    bool wakeupByTimeout;                           /* If wakeup event is triggered on timeout */
    uint16_t matchCount;                            /* Number of matches needed before generating an wakeup event */
    uint16_t matchTimeout;                          /* Timeout value that generates an wakeup event if wakeupByTimeout is true */
    CAN_PN_FILTER_TYPE_T filterType;            /* Filtering type */
    CAN_PN_ID_FILTER_T idFilter1;               /* The first ID filter (match exact / lower limit / upper limit) */
    CAN_PN_ID_FILTER_T idFilter2;               /* The second ID filter (mask / upper limit) */
    CAN_PN_MATCH_SCHEME_T idFilterScheme;       /* ID filtering scheme */
    CAN_PN_MATCH_SCHEME_T payloadFilterScheme;  /* Payload filtering scheme */
    CAN_PN_PAYLOAD_FILTER_T payloadFilter;      /* Configuration of the payload filter */
} CAN_PN_CONFIG_T;

/* CAN driver callback function type */
typedef void (*CAN_CALLBACK_T)(
    uint8_t instance,
    CAN_EVENT_TYPE_T eventType,
    uint32_t mbIndex,
    CAN_STATE_T *driverState);

/* CAN driver error callback function type */
typedef void (*CAN_ERROR_CALLBACK_T)(
    uint8_t instance,
    CAN_EVENT_TYPE_T eventType,
    CAN_STATE_T *driverState);

/**@} end of group CAN_Structures*/

/** @defgroup CAN_Variables Variables
  @{
*/
/**@} end of group CAN_Variables*/

/** @defgroup CAN_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T CAN_Init(
    uint8_t index,
    CAN_STATE_T *states,
    const CAN_USER_CONFIG_T *config);

STATUS_T CAN_DeInit(uint8_t index);

uint32_t CAN_DefaultConfig(CAN_USER_CONFIG_T * config);

void CAN_GetBitrate(uint8_t index, CAN_TIME_SEGMENT_T *bitrate);
void CAN_SetBitrate(uint8_t index, const CAN_TIME_SEGMENT_T *bitrate);
void CAN_GetBitrateFd(uint8_t index, CAN_TIME_SEGMENT_T *bitrate);
void CAN_SetBitrateFd(uint8_t index, const CAN_TIME_SEGMENT_T *bitrate);
void CAN_SetTdcOffset(uint8_t index, bool isEnabled, uint8_t offset);
uint8_t CAN_GetTdcValue(uint8_t index);
bool CAN_GetTdcFail(uint8_t index);
void CAN_ClearTdcFail(uint8_t index);

STATUS_T CAN_SendBlocking(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload,
    uint32_t timeout);

STATUS_T CAN_SendNonBlocking(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload);

STATUS_T CAN_ReceiveBlocking(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf,
    uint32_t timeout);

STATUS_T CAN_ReceiveNonBlocking(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf);

STATUS_T CAN_ReceiveFifoBlocking(
    uint8_t index,
    CAN_MSG_BUF_T *msgBuf,
    uint32_t timeout);

STATUS_T CAN_ReceiveFifoNonBlocking(
    uint8_t index,
    CAN_MSG_BUF_T *msgBuf);

STATUS_T CAN_AbortTransfer(uint8_t index, uint8_t mbIndex);
STATUS_T CAN_GetTransferStatus(uint8_t index, uint8_t mbIndex);
uint32_t CAN_GetErrorStatus(uint8_t index);

STATUS_T CAN_ConfigureRxMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *rxInfo,
    uint32_t canId);

STATUS_T CAN_ConfigureTxMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId);

void CAN_ConfigureRxFifo(
    uint8_t index,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat,
    const CAN_RXFIFO_ID_FILTER_T *idFilterTable);

STATUS_T CAN_ConfigureRemoteResponseMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload);

void CAN_SetRxMaskType(uint8_t index, CAN_RX_MASK_TYPE_T maskType);

STATUS_T CAN_SetRxIndividualMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint8_t mbIndex,
    uint32_t mask);

void CAN_SetRxFifoGlobalMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask);

void CAN_SetRxMbGlobalMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask);

void CAN_SetRxMb14Mask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask);

void CAN_SetRxMb15Mask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask);

void CAN_InstallEventCallback(
    uint8_t index,
    CAN_CALLBACK_T callBack,
    void *callbackParam);

void CAN_InstallErrorCallback(
    uint8_t index,
    CAN_ERROR_CALLBACK_T callBack,
    void *callbackParam);

void CAN_ConfigurePretendedNetworking(
    uint8_t index,
    bool isEnabled,
    const CAN_PN_CONFIG_T *pnConfig);

void CAN_GetWakeupMessage(
    uint8_t index,
    uint8_t wmbIndex,
    CAN_MSG_BUF_T *wmbPtr);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
void CAN_HW_Init(CAN_T *baseAddr);
void CAN_HW_Enable(CAN_T *baseAddr);
void CAN_HW_Disable(CAN_T *baseAddr);
bool CAN_HW_IsEnabled(const CAN_T *baseAddr);
void CAN_HW_SetOperationMode(CAN_T *baseAddr, CAN_OPERATION_MODE_T mode);
void CAN_HW_SetSelfReceptionEnable(CAN_T *baseAddr, bool enable);
uint32_t CAN_HW_GetLastMbNumber(const CAN_T *baseAddr);
STATUS_T CAN_HW_SetLastMbNumber(CAN_T *baseAddr, uint32_t maxMbNumber);

void CAN_HW_GetTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg);
void CAN_HW_SetTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg);

void CAN_HW_SetRxMaskType(CAN_T *baseAddr, CAN_RX_MASK_TYPE_T type);
void CAN_HW_SetRxIndividualMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t mask);
void CAN_HW_SetRxFifoGlobalMask(CAN_T *baseAddr, uint32_t Mask);

bool CAN_HW_GetFreezeStatus(const CAN_T *baseAddr);
void CAN_HW_EnterFreezeMode(CAN_T *baseAddr);
void CAN_HW_ExitFreezeMode(CAN_T *baseAddr);

uint8_t CAN_HW_GetMbInterruptStatus(const CAN_T *baseAddr, uint32_t mbIndex);
uint8_t CAN_HW_GetMbInterruptEnable(const CAN_T *baseAddr, uint32_t mbIndex);
void CAN_HW_SetMbInterruptEnable(CAN_T *baseAddr, uint32_t mbIndex, bool enable);

void CAN_HW_ClearRam(CAN_T *baseAddr);
void CAN_HW_ClearRxFifo(CAN_T *baseAddr);
void CAN_HW_ClearErrorInterruptFlag(CAN_T *baseAddr);
void CAN_HW_ClearBusOffInterruptFlag(CAN_T *baseAddr);
void CAN_HW_SetErrorInterruptEnable(CAN_T *baseAddr, CAN_INTERRUPT_TYPE_T errType, bool enable);

uint8_t CAN_HW_GetMbInterruptFlag(const CAN_T *baseAddr, uint32_t mbIndex);
void CAN_HW_ClearMbInterruptFlag(CAN_T *baseAddr, uint32_t mbIndex);

void CAN_HW_SetRxMbGlobalStdMask(CAN_T *baseAddr, uint32_t stdMask);
void CAN_HW_SetRxMbGlobalExtMask(CAN_T *baseAddr, uint32_t extMask);
void CAN_HW_SetRxIndividualStdMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t stdMask);
void CAN_HW_SetRxIndividualExtMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t extMask);
void CAN_HW_SetRxMb14StdMask(CAN_T *baseAddr, uint32_t stdMask);
void CAN_HW_SetRxMb14ExtMask(CAN_T *baseAddr, uint32_t extMask);
void CAN_HW_SetRxMb15StdMask(CAN_T *baseAddr, uint32_t stdMask);
void CAN_HW_SetRxMb15ExtMask(CAN_T *baseAddr, uint32_t extMask);

void CAN_HW_LockRxMb(CAN_T *baseAddr, uint32_t mbIndex);
void CAN_HW_UnlockRxMb(const CAN_T *baseAddr);

bool CAN_HW_IsMbOutOfRange(const CAN_T *baseAddr, uint32_t mbIndex);

uint32_t CAN_HW_GetMaxMbNum(const CAN_T *baseAddr);

uint32_t CAN_HW_CalculateRxFifoMask(
    CAN_MSG_ID_TYPE_T idType,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T format,
    uint32_t mask);

STATUS_T CAN_HW_SetRxMb(
    CAN_T *baseAddr,
    uint32_t mbIndex,
    const CAN_MB_CODE_STATUS_T *cs,
    uint32_t messageId);

STATUS_T CAN_HW_SetTxMb(
    CAN_T *baseAddr,
    uint32_t mbIndex,
    const CAN_MB_CODE_STATUS_T *cs,
    uint32_t messageId,
    const uint8_t *msgData,
    const bool isRemote);

void CAN_HW_SetRxFifoFilter(
    CAN_T *baseAddr,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat,
    const CAN_RXFIFO_ID_FILTER_T *idFilterTable);

void CAN_HW_AbortTxMb(CAN_T *baseAddr, uint32_t mbIndex);
void CAN_HW_ResetRxMb(CAN_T *baseAddr, uint32_t mbIndex);
volatile uint32_t* CAN_HW_GetMbRegion(CAN_T *baseAddr, uint32_t mbIndex);
void CAN_HW_GetMbValues(CAN_T *baseAddr, uint32_t mbIndex, CAN_MSG_BUF_T *msgBuf);
bool CAN_HW_IsRxFifoEnabled(const CAN_T *baseAddr);
STATUS_T CAN_HW_EnableRxFifo(CAN_T *baseAddr, uint32_t numOfFilters);
CAN_RXFIFO_ID_ELEMENT_FORMAT_T CAN_HW_GetRxFifoIdFormat(const CAN_T *baseAddr);
void CAN_HW_GetRxFifoValues(const CAN_T *baseAddr, CAN_MSG_BUF_T *rxFifo);
uint8_t CAN_HW_GetAffectedRxFifoNumbers(const CAN_T *baseAddr);
void CAN_HW_SetClockSource(CAN_T *baseAddr, CAN_CLK_SOURCE_T clk);
bool CAN_HW_InstanceHasFd(uint8_t instance);
void CAN_HW_GetExtendedTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg);
void CAN_HW_SetExtendedTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg);
void CAN_HW_GetFdTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg);
void CAN_HW_SetFdTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg);
bool CAN_HW_IsFdEnabled(const CAN_T *baseAddr);
void CAN_HW_SetFdEnable(CAN_T *baseAddr, bool enable);
uint8_t CAN_HW_GetFdPayloadSize(const CAN_T *baseAddr);
void CAN_HW_SetFdPayloadSize(CAN_T *baseAddr, CAN_FD_PAYLOAD_SIZE_T fdPayloadSize);
void CAN_HW_SetIsoFdEnable(CAN_T *baseAddr, bool enable);
void CAN_HW_SetTdcOffset(CAN_T *baseAddr, bool enable, uint8_t offset);
void CAN_HW_SetRxFifoDmaEnable(CAN_T *baseAddr, bool enable);
bool CAN_HW_IsPnEnabled(const CAN_T *baseAddr);
void CAN_HW_SetPnEnable(CAN_T *baseAddr, bool isEnabled);
void CAN_HW_ConfigurePn(CAN_T *baseAddr, const CAN_PN_CONFIG_T *pnCfg);
void CAN_HW_SetPnTimeout(CAN_T *baseAddr, uint16_t timeout);
void CAN_HW_SetPnIdFilter1(CAN_T *baseAddr, CAN_PN_ID_FILTER_T idFilter);
void CAN_HW_SetPnIdFilter2(CAN_T *baseAddr, const CAN_PN_CONFIG_T *pnCfg);
void CAN_HW_SetPnIdFilter2Check(CAN_T *baseAddr);
void CAN_HW_SetPnDlcFilter(CAN_T *baseAddr, uint8_t minPayloadLen, uint8_t maxPayloadLen);
void CAN_HW_SetPnPayloadHighFilter1(CAN_T *baseAddr, const uint8_t *payloadFilter);
void CAN_HW_SetPnPayloadLowFilter1(CAN_T *baseAddr, const uint8_t *payloadFilter);
void CAN_HW_SetPnPayloadHighFilter2(CAN_T *baseAddr, const uint8_t *payloadFilter);
void CAN_HW_SetPnPayloadLowFilter2(CAN_T *baseAddr, const uint8_t *payloadFilter);
uint8_t CAN_HW_GetWakeupByTimeoutFlag(const CAN_T *baseAddr);
void CAN_HW_ClearWakeupByTimeoutFlag(CAN_T *baseAddr);
uint8_t CAN_HW_GetWakeupByMatchFlag(const CAN_T *baseAddr);
void CAN_HW_ClearWakeupByMatchFlag(CAN_T *baseAddr);
void CAN_HW_SetPnFilter(
    CAN_T *baseAddr,
    bool wakeupByTimeout,
    bool wakeupByMatch,
    uint16_t numMatch,
    CAN_PN_FILTER_TYPE_T filterType,
    CAN_PN_MATCH_SCHEME_T idScheme,
    CAN_PN_MATCH_SCHEME_T payloadScheme);

/**@} end of group CAN_Functions*/
/**@} end of group CAN_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_CAN_H */
