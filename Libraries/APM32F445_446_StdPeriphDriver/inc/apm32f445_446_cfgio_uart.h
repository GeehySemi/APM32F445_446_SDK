/*!
 * @file        apm32f445_446_cfgio_uart.h
 *
 * @brief       Header of apm32f445_446_cfgio_uart.c
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

#ifndef APM32F445_446_CFGIO_UART_H
#define APM32F445_446_CFGIO_UART_H

/* Includes */
#include "device.h"
#include "apm32f445_446_cfgio_common.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_dma.h"
#include "callbacks.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_UART_Driver
  @{
*/

/** @defgroup CFGIO_UART_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief CFGIO_UART driver direction (Rx or Tx)
 */
typedef enum
{
    CFGIO_UART_DIR_RX = 0x00U,    /* Rx UART driver */
    CFGIO_UART_DIR_TX = 0x01U,    /* Tx UART driver */
} CFGIO_UART_DRIVER_DIR_T;

/**@} end of group CFGIO_UART_Enumerations*/

/** @defgroup CFGIO_UART_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief   Configuration for the CFGIO_UART driver
 */
typedef struct
{
    CFGIO_UART_DRIVER_DIR_T direction;    /* Driver direction: Rx or Tx */
    CFGIO_DRIVER_TYPE_T transferType;     /* Data transfer type: Interrupts/DMA/Polling */
    uint32_t baudrate;                      /* Baudrate */
    uint8_t numBits;                        /* Number of bits per word */
    uint8_t cfgioPin;                     /* CFGIO pin to use as Rx or Tx pin */
    uint8_t dmaChannel;                     /* DMA channel number, only used in DMA mode */
    UART_CALLBACK_T callback;               /* User callback function */
    void *callbackParam;                    /* Parameter for the callback function */
} CFGIO_UART_USER_CONFIG_T;

/*!
 * @brief   CFGIO_UART driver internal state
 * @details This structure is used by the CFGIO_UART driver for its internal
 *          state. It must be provided by the application through the
 *          CFGIO_UART_Init() function, and it cannot be freed until the
 *          driver is reset using CFGIO_UART_Deinit(). The application should
 *          make no assumptions about the content of this structure.
 */
typedef struct
{
/*! @cond DRIVER_INTERNAL_USE_ONLY */
    CFGIO_COMMON_STATE_T commonState;     /* CFGIO driver structure */
    CFGIO_UART_DRIVER_DIR_T dir;          /* Driver direction: Rx or Tx */
    CFGIO_DRIVER_TYPE_T transferType;     /* Data transfer type: Interrupts/DMA/Polling */
    uint8_t numBits;                        /* Number of bits per word */
    uint8_t dmaChannel;                     /* DMA channel number */
    uint8_t endBytes;                       /* Number of bytes to flush before ending a transmission */
    uint8_t *rxBuffer;                      /* Receive buffer */
    const uint8_t *txBuffer;                /* Transmit buffer */
    uint32_t bytesLeft;                     /* Number of remaining bytes to be transferred */
    STATUS_T driverStatus;                  /* Current status of the driver */
    bool isIdle;                            /* Idle/busy state of the driver */
    bool isBlocking;                        /* If the current transfer is blocking */
    SEMAPHORE_T sem;                        /* Semaphore used by blocking functions */
    UART_CALLBACK_T callback;               /* User callback function */
    void *callbackParam;                    /* Parameter for the callback function */
/*! @endcond */
} CFGIO_UART_STATE_T;

/**@} end of group CFGIO_UART_Structures*/

/** @defgroup CFGIO_UART_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGIO_UART_Init(
    uint32_t instance,
    const CFGIO_UART_USER_CONFIG_T *configPtr,
    CFGIO_UART_STATE_T *states);

STATUS_T CFGIO_UART_Deinit(CFGIO_UART_STATE_T *states);

void CFGIO_UART_DefaultConfig(CFGIO_UART_USER_CONFIG_T *configPtr);

STATUS_T CFGIO_UART_SetBaudrate(
    CFGIO_UART_STATE_T *states,
    uint32_t baudrate,
    uint8_t numBits);

STATUS_T CFGIO_UART_GetBaudrate(
    CFGIO_UART_STATE_T *states,
    uint32_t *baudrate);

STATUS_T CFGIO_UART_RxDataBlocking(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength,
    uint32_t timeout);

STATUS_T CFGIO_UART_RxDataNonBlocking(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength);

STATUS_T CFGIO_UART_SetRxBuffer(
    CFGIO_UART_STATE_T *states,
    uint8_t *rxBuffer,
    uint32_t rxLength);

STATUS_T CFGIO_UART_TxDataBlocking(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
    uint32_t txLength,
    uint32_t timeout);

STATUS_T CFGIO_UART_TxDataNonBlocking(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
    uint32_t txLength);

STATUS_T CFGIO_UART_SetTxBuffer(
    CFGIO_UART_STATE_T *states,
    const uint8_t *txBuffer,
    uint32_t txLength);

STATUS_T CFGIO_UART_AbortTransfer(CFGIO_UART_STATE_T *states);

STATUS_T CFGIO_UART_GetStatus(
    CFGIO_UART_STATE_T *states,
    uint32_t *bytesLeft);

/**@} end of group CFGIO_UART_Functions*/
/**@} end of group CFGIO_UART_Driver*/
/**@} end of group APM32F445_446x_StdPeriphDriver*/

#ifdef __cplusplus
}
#endif

#endif /* APM32F445_446_CFGIO_UART_H */
