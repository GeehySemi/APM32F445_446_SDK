/*!
 * @file        apm32f445_446_lpuart.h
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

#ifndef APM32F445_446_LPUART_H
#define APM32F445_446_LPUART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "callbacks.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_dma.h"
#include "osif.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPUART_Driver LPUART Driver
  @{
*/

/** @defgroup LPUART_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define LPUART_REG_ID_BAUDRATE  (1U)
#define LPUART_REG_ID_STATUS    (2U)
#define LPUART_REG_ID_CONTROL   (3U)
#define LPUART_REG_ID_DATA      (4U)
#define LPUART_REG_ID_MATCH     (5U)
#define LPUART_REG_ID_MODEM     (6U)
#define LPUART_REG_ID_FIFO      (7U)
#define LPUART_SHIFT            (16U)

/* Number of instances of the LPUART module */
#define LPUART_INSTANCE_CNT     (3U)
#define LPUART_INSTANCE_VALIDITY(__instance__)  if(__instance__ >= LPUART_INSTANCE_CNT){while(1);}

/**@} end of group LPUART_Macros*/

/** @defgroup LPUART_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* LPUART interrupt flags */
typedef enum
{
    LPUART_INT_RX_ACTIVE_EDGE =
        (((uint32_t)LPUART_REG_ID_BAUDRATE << (uint32_t)LPUART_SHIFT) | (uint32_t)14U), /* RX Active Edge */
    LPUART_INT_LIN_BRK_DETECT =
        (((uint32_t)LPUART_REG_ID_BAUDRATE << (uint32_t)LPUART_SHIFT) | (uint32_t)15U), /* LIN break detect */
    LPUART_INT_MATCH_ADDR_TWO =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)14U),  /* Match address two flag */
    LPUART_INT_MATCH_ADDR_ONE =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)15U),  /* Match address one flag */
    LPUART_INT_IDLE_LINE =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)20U),  /* Idle line */
    LPUART_INT_RX_DATA_REG_FULL =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)21U),  /* Receiver data register full */
    LPUART_INT_TX_COMPLETE =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)22U),  /* Transmission complete */
    LPUART_INT_TX_DATA_REG_EMPTY =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)23U),  /* Transmit data register empty */
    LPUART_INT_PARITY_ERR_FLAG =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)24U),  /* Parity error flag */
    LPUART_INT_FRAME_ERR_FLAG =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)25U),  /* Framing error flag */
    LPUART_INT_NOISE_ERR_FLAG =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)26U),  /* Noise error flag */
    LPUART_INT_RX_OVERRUN =
        (((uint32_t)LPUART_REG_ID_CONTROL << (uint32_t)LPUART_SHIFT) | (uint32_t)27U),  /* Receiver overrun */
    LPUART_INT_FIFO_RX_UNDERFLOW =
        (((uint32_t)LPUART_REG_ID_FIFO << (uint32_t)LPUART_SHIFT) | (uint32_t)8U),  /* Receiver FIFO interrupt */
    LPUART_INT_FIFO_TX_OVERFLOW =
        (((uint32_t)LPUART_REG_ID_FIFO << (uint32_t)LPUART_SHIFT) | (uint32_t)9U)   /* Transmitter FIFO interrupt */
} LPUART_INTERRUPT_T;

/* LPUART status flags */
typedef enum
{
    /* PARITYE bit, sets if noise detected in current data word */
    LPUART_PARITY_ERR_IN_CURRENT_WORD =
        (((uint32_t)LPUART_REG_ID_DATA << (uint32_t)LPUART_SHIFT) | (uint32_t)14U),
    /* NOISY bit, sets if noise detected in current data word */
    LPUART_NOISE_IN_CURRENT_WORD =
        (((uint32_t)LPUART_REG_ID_DATA << (uint32_t)LPUART_SHIFT) | (uint32_t)15U),
    /* Address two match flag */
    LPUART_MATCH_ADDR_TWO =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)14U),
    /* Address one match flag */
    LPUART_MATCH_ADDR_ONE =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)15U),
    /* If parity enabled, sets upon parity error detection */
    LPUART_PARITY_ERR =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)16U),
    /* Frame error flag, sets if logic 0 was detected where stop bit expected */
    LPUART_FRAME_ERR =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)17U),
    /* Rx takes 3 samples of each received bit. If these differ, the flag sets */
    LPUART_NOISE_DETECT =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)18U),
    /* Rx Overrun sets if new data is received before data is read */
    LPUART_RX_OVERRUN =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)19U),
    /* Idle line detect flag, sets when idle line detected */
    LPUART_IDLE_LINE_DETECT =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)20U),
    /* Rx data register full flag, sets when the receive data buffer is full */
    LPUART_RX_DATA_REG_FULL =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)21U),
    /* Transmission complete flag, sets when transmission activity complete */
    LPUART_TX_COMPLETE =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)22U),
    /* Tx data register empty flag, sets when Tx buffer is empty */
    LPUART_TX_DATA_REG_EMPTY =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)23U),
    /* Receiver Active Flag (RAF), sets at beginning of valid start bit */
    LPUART_RX_ACTIVE =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)24U),
    /* Rx pin active edge interrupt flag, sets when active edge detected */
    LPUART_RX_ACTIVE_EDGE_DETECT =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)30U),
    /* LIN break detect interrupt flag, sets when LIN break char detected */
    LPUART_LIN_BREAK_DETECT =
        (((uint32_t)LPUART_REG_ID_STATUS << (uint32_t)LPUART_SHIFT) | (uint32_t)31U),
    /* Receiver FIFO buffer underflow */
    LPUART_FIFO_RX_UNDERFLOW =
        (((uint32_t)LPUART_REG_ID_FIFO << (uint32_t)LPUART_SHIFT) | (uint32_t)16U),
    /* Transmitter FIFO buffer overflow */
    LPUART_FIFO_TX_OVERFLOW =
        (((uint32_t)LPUART_REG_ID_FIFO << (uint32_t)LPUART_SHIFT) | (uint32_t)17U)
} LPUART_STATUS_FLG_T;

/* LPUART number of bits in a character */
typedef enum
{
    LPUART_8_BITS_DATA  = 0x0U, /* 8-bit data characters */
    LPUART_9_BITS_DATA  = 0x1U, /* 9-bit data characters */
    LPUART_10_BITS_DATA = 0x2U  /* 10-bit data characters */
} LPUART_BIT_COUNT_PER_CHAR_T;

/* LPUART parity mode */
typedef enum
{
    LPUART_PARITY_DISABLED = 0x0U, /* Parity disabled */
    LPUART_PARITY_ENABLED  = 0x1U, /* Parity enabled */
    LPUART_PARITY_EVEN     = 0x2U, /* Parity enabled, type even, bit setting: PE|PT = 10 */
    LPUART_PARITY_ODD      = 0x3U  /* Parity enabled, type odd,  bit setting: PE|PT = 11 */
} LPUART_PARITY_MODE_T;

/* LPUART number of stop bits */
typedef enum
{
    LPUART_ONE_STOP_BIT = 0x0U, /* One stop bit */
    LPUART_TWO_STOP_BIT = 0x1U  /* Two stop bits */
} LPUART_STOP_BIT_COUNT_T;

/* LPUART transfer type */
typedef enum
{
    LPUART_USE_DMA = 0U,     /* The driver will use DMA to perform UART transfer */
    LPUART_USE_INTERRUPTS   /* The driver will use interrupts to perform UART transfer */
} LPUART_TRANSFER_TYPE_T;

/**
 * LPUART break character length settings for transmit or detect.
 * The maximum bit times may vary depending on the LPUART instance.
 */
typedef enum
{
    LPUART_BRK_CHAR_10_BIT_MIN = 0x0U, /* LPUART break char length 10 bit times (if M = 0, SBNS = 0)
                                          or 11 (if M = 1, SBNS = 0 or M = 0, SBNS = 1) or 12 (if M = 1,
                                          SBNS = 1 or M10 = 1, SNBS = 0) or 13 (if M10 = 1, SNBS = 1) */
    LPUART_BRK_CHAR_13_BIT_MIN = 0x1U  /* LPUART break char length 13 bit times (if M = 0, SBNS = 0
                                          or M10 = 0, SBNS = 1) or 14 (if M = 1, SBNS = 0 or M = 1,
                                          SBNS = 1) or 15 (if M10 = 1, SBNS = 1 or M10 = 1, SNBS = 0) */
} LPUART_BRK_CHAR_LEN_T;

/**@} end of group LPUART_Enumerations*/

/** @defgroup LPUART_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/* LPUART user configurations */
typedef struct
{
    LPUART_TRANSFER_TYPE_T transferType;    /* Type of LPUART transfer (interrupt/dma based) */
    uint32_t baudrate;                      /* LPUART baudrate */
    LPUART_PARITY_MODE_T parityMode;        /* Parity mode */
    LPUART_STOP_BIT_COUNT_T stopBits;       /* Number of stop bits */
    LPUART_BIT_COUNT_PER_CHAR_T dataBits;   /* Number of bits in a character (8-default, 9 or 10).
                                               For DMA transmission only 8-bit char is supported */
    uint8_t rxDmaChannel;                   /* Channel number for DMA rx channel */
    uint8_t txDmaChannel;                   /* Channel number for DMA tx channel */
} LPUART_CFG_T;

/* LPUART internal state */
typedef struct
{
    uint8_t rxDmaChannel;           /* DMA channel number for DMA-based rx */
    uint8_t txDmaChannel;           /* DMA channel number for DMA-based tx */
    volatile STATUS_T rxStatus;     /* Status of last driver receive operation */
    volatile uint8_t rxComplete;    /* Synchronization object for blocking Rx timeout condition */
    volatile STATUS_T txStatus;     /* Status of last driver transmit operation */
    volatile uint8_t txComplete;    /* Synchronization object for blocking Tx timeout condition */
    volatile bool rxBusy;           /* True if there is an active receive */
    volatile bool txBusy;           /* True if there is an active transmit */
    volatile bool rxBlocking;       /* True if receive is blocking transaction */
    volatile bool txBlocking;       /* True if transmit is blocking transaction */
    volatile uint32_t rxLen;        /* The remaining number of bytes to be received */
    volatile uint32_t txLen;        /* The remaining number of bytes to be transmitted */
    uint8_t *rxBuffer;              /* The buffer of received data */
    const uint8_t *txBuffer;        /* The buffer of data being sent */
    UART_CALLBACK_T rxCallback;     /* Callback to invoke for data receive */
    void * rxCallbackParam;         /* Receive callback parameter pointer */
    UART_CALLBACK_T txCallback;     /* Callback to invoke for data send */
    void * txCallbackParam;         /* Transmit callback parameter pointer */
    LPUART_TRANSFER_TYPE_T transferType;    /* Type of LPUART transfer (interrupt/dma based) */
    LPUART_BIT_COUNT_PER_CHAR_T dataBits;   /* Number of bits in a char (8/9/10) */
} LPUART_STATE_T;

/**@} end of group LPUART_Structures*/

/** @defgroup LPUART_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T LPUART_Init(uint32_t ins,
                     LPUART_STATE_T *lpuartState,
                     const LPUART_CFG_T *lpuartCfg);
STATUS_T LPUART_DeInit(uint32_t ins);

void LPUART_DefaultConfig(LPUART_CFG_T *lpuartCfg);

STATUS_T LPUART_SetBaudrate(uint32_t ins, uint32_t expectedBaudrate);
void LPUART_GetBaudrate(uint32_t ins, uint32_t *baudrate);

STATUS_T LPUART_SetTxBuffer(uint32_t ins, const uint8_t *txBuffer, uint32_t txLen);
STATUS_T LPUART_SetRxBuffer(uint32_t ins, uint8_t *rxBuffer, uint32_t rxLen);

STATUS_T LPUART_RxData(uint32_t ins, uint8_t *rxBuffer, uint32_t rxLen);
STATUS_T LPUART_RxDataPolling(uint32_t ins, uint8_t *rxBuffer, uint32_t rxLen);
STATUS_T LPUART_RxDataBlocking(uint32_t ins, uint8_t *rxBuffer, uint32_t rxLen, uint32_t timeout);

STATUS_T LPUART_TxData(uint32_t ins, const uint8_t *txBuffer, uint32_t txLen);
STATUS_T LPUART_TxDataPolling(uint32_t ins,const uint8_t *txBuffer, uint32_t txLen);
STATUS_T LPUART_TxDataBlocking(uint32_t ins, const uint8_t *txBuffer, uint32_t txLen, uint32_t timeout);

STATUS_T LPUART_GetRxStatus(uint32_t ins, uint32_t *bytesRemain);
STATUS_T LPUART_GetTxStatus(uint32_t ins, uint32_t *bytesRemain);

STATUS_T LPUART_CancelRx(uint32_t ins);
STATUS_T LPUART_CancelTx(uint32_t ins);

UART_CALLBACK_T LPUART_InstallRxCallback(uint32_t ins,
                                         UART_CALLBACK_T function,
                                         void *callbackParam);
UART_CALLBACK_T LPUART_InstallTxCallback(uint32_t ins,
                                         UART_CALLBACK_T function,
                                         void *callbackParam);



/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void LPUART_HW_Init(LPUART_T *base);
void LPUART_HW_EnableTx(LPUART_T *base);
void LPUART_HW_DisableTx(LPUART_T *base);
void LPUART_HW_EnableRx(LPUART_T *base);
void LPUART_HW_DisableRx(LPUART_T *base);
void LPUART_HW_SetBaudrateDivisor(LPUART_T *base, uint32_t divisor);
uint16_t LPUART_HW_GetBaudrateDivisor(const LPUART_T *base);
void LPUART_HW_SetOversamplingRatio(LPUART_T *base, uint32_t ratio);
uint8_t LPUART_HW_GetOversamplingRatio(const LPUART_T *base);
void LPUART_HW_EnableBothEdgeSamplingCmd(LPUART_T *base);
void LPUART_HW_SetDataBits(LPUART_T *base, LPUART_BIT_COUNT_PER_CHAR_T dataBits, bool parity);
void LPUART_HW_SetParityMode(LPUART_T *base, LPUART_PARITY_MODE_T parity);
void LPUART_HW_SetStopBitCnt(LPUART_T *base, LPUART_STOP_BIT_COUNT_T stopBits);
void LPUART_HW_EnableIntMode(LPUART_T *base, LPUART_INTERRUPT_T intSrc);
void LPUART_HW_DisableIntMode(LPUART_T *base, LPUART_INTERRUPT_T intSrc);
bool LPUART_HW_GetIntMode(const LPUART_T *base, LPUART_INTERRUPT_T intSrc);
void LPUART_HW_EnableTxDma(LPUART_T *base);
void LPUART_HW_DisableTxDma(LPUART_T *base);
void LPUART_HW_EnableRxDma(LPUART_T *base);
void LPUART_HW_DisableRxDma(LPUART_T *base);
void LPUART_HW_TxChar(LPUART_T *base, uint8_t data);
void LPUART_HW_TxChar9(LPUART_T *base, uint16_t data);
void LPUART_HW_TxChar10(LPUART_T *base, uint16_t data);
void LPUART_HW_RxChar(const LPUART_T *base, uint8_t *readData);
void LPUART_HW_RxChar9(const LPUART_T *base, uint16_t *readData);
void LPUART_HW_RxChar10(const LPUART_T *base, uint16_t *readData);
bool LPUART_HW_GetStatusFlag(const LPUART_T *base, LPUART_STATUS_FLG_T status);
STATUS_T LPUART_HW_ClearStatusFlag(LPUART_T *base,LPUART_STATUS_FLG_T status);
void LPUART_HW_EnableErrInterrupts(LPUART_T *base);
void LPUART_HW_DisableErrInterrupts(LPUART_T *base);
void LPUART_HW_ClearErrFlags(LPUART_T *base);
bool LPUART_HW_GetRxDataPolarity(const LPUART_T *base);
void LPUART_HW_EnableRxDataPolarity(LPUART_T *base);
void LPUART_HW_DisableRxDataPolarity(LPUART_T *base);
void LPUART_HW_SetBreakCharTxLen(LPUART_T *base, LPUART_BRK_CHAR_LEN_T length);
void LPUART_HW_SetBreakCharDetectLen(LPUART_T *base, LPUART_BRK_CHAR_LEN_T length);
void LPUART_HW_QueueBreakField(LPUART_T *base);

/**@} end of group LPUART_Functions*/
/**@} end of group LPUART_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_LPUART_H */
