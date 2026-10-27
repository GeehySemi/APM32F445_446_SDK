/*!
 * @file        apm32f445_446_cfgio_spi.h
 *
 * @brief       This file contains all the functions prototypes for the CFGIO_SPI firmware library.
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

#ifndef APM32F445_446_CFGIO_SPI_H
#define APM32F445_446_CFGIO_SPI_H

/* Includes */
#include "device.h"
#include "apm32f445_446_cfgio_common.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_dma.h"
#include "callbacks.h"
#include "osif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGIO_SPI_Driver CFGIO_SPI Driver
  @{
*/

/** @defgroup CFGIO_SPI_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                                ENUMS
 ******************************************************************************/
/* The transmission sequence of data bits */
typedef enum
{
    CFGIO_SPI_SEND_MSB_FIRST   = 0U,  /* Sending from the most significant bit */
    CFGIO_SPI_SEND_LSB_FIRST   = 1U,  /* Sending from the least significant bit */
} CFGIO_SPI_SEND_BIT_FIRST_T;

/* Size of transferred data (bytes) */
typedef enum
{
    CFGIO_SPI_DATA_SIZE_1BYTE   = 1U,  /* Data size 1-byte */
    CFGIO_SPI_DATA_SIZE_2BYTE   = 2U,  /* Data size 2-bytes */
    CFGIO_SPI_DATA_SIZE_4BYTE   = 4U,  /* Data size 4-bytes */
} CFGIO_SPI_DATA_SIZE_T;

/**@} end of group CFGIO_SPI_Enumerations*/

/** @defgroup CFGIO_SPI_Structures Structures
  @{
*/

/*******************************************************************************
 *                              TYPE DEFINES
 ******************************************************************************/
 /* This structure is used to provide configuration parameters for the CFGIO_SPI master during initialization */
typedef struct
{
    uint8_t txDMAChannel;                       /* Tx DMA channel number */
    uint8_t rxDMAChannel;                       /* Rx DMA channel number */
    void *callbackParam;                        /* Parameter of callback function */
    SPI_CALLBACK_T callback;                    /* User callback,not interrupt callbacks,should take as little time as
                                                   possible to execute.If it is not needed,it is NULL */
    uint8_t ssPin;                              /* CFGIO pin to SS */
    uint8_t sckPin;                             /* CFGIO pin to SCK */
    uint8_t misoPin;                            /* CFGIO pin to MISO */
    uint8_t mosiPin;                            /* CFGIO pin to MOSI */
    uint8_t clockPha;                           /* Clock Phase:0 = sample on leading clock edge;
                                                    1 = sample on trailing clock edge */
    uint8_t clockPol;                           /* Clock Polarity: 0 = high clock; 1 = low clock */
    CFGIO_SPI_DATA_SIZE_T transferSize;       /* Transfer size in bytes: 1/2/4 */
    CFGIO_SPI_SEND_BIT_FIRST_T firstBitOrder; /* first Bit order: LSB/MSB */
    CFGIO_DRIVER_TYPE_T cfgspiDirType;       /* cfgio_SPI Driver type: interrupts/polling/DMA */
    uint32_t baudrate;                          /* Baud rate */
} CFGIO_SPI_MASTER_CFG_T;

 /* This structure is used to provide configuration parameters for the CFGIO_SPI slave during initialization */
typedef struct
{
    uint8_t txDMAChannel;                       /* Tx DMA channel number */
    uint8_t rxDMAChannel;                       /* Rx DMA channel number */
    void *callbackParam;                        /* Parameter of callback function */
    SPI_CALLBACK_T callback;                    /* User callback,not interrupt callbacks,should take as little time as
                                                   possible to execute.If it is not needed,it is NULL */
    uint8_t ssPin;                              /* CFGIO pin to SS */
    uint8_t sckPin;                             /* CFGIO pin to SCK */
    uint8_t misoPin;                            /* CFGIO pin to MISO */
    uint8_t mosiPin;                            /* CFGIO pin to MOSI */
    uint8_t clockPha;                           /* Clock Phase:0 = sample on leading clock edge;
                                                    1 = sample on trailing clock edge */
    uint8_t clockPol;                           /* Clock Polarity: 0 = high clock; 1 = low clock */
    CFGIO_SPI_DATA_SIZE_T transferSize;       /* Transfer size in bytes: 1/2/4 */
    CFGIO_SPI_SEND_BIT_FIRST_T firstBitOrder; /* first Bit order: LSB/MSB */
    CFGIO_DRIVER_TYPE_T cfgspiDirType;       /* cfgio_SPI Driver type: interrupts/polling/DMA */
} CFGIO_SPI_SLAVE_CFG_T;


/* Master internal structure */
typedef struct
{
    CFGIO_COMMON_STATE_T commonCFGIO;       /* Common cfgio drivers structure */
    const uint8_t *txData;                      /* Transmit buffer */
    uint8_t *rxData;                            /* Receive buffer */
    uint32_t txBytesRemaining;                  /* The number of bytes remaining to transfer */
    uint32_t rxBytesRemaining;                  /* The number of bytes remaining to received */
    uint32_t virtualDmaData;                    /* Virtual location for DMA Transmission */
    uint8_t rxDMAChannel;                       /* Rx DMA channel number */
    uint8_t txDMAChannel;                       /* Tx DMA channel number */
    CFGIO_DRIVER_TYPE_T cfgspiDirType;       /* cfgio_SPI Driver type: interrupts/polling/DMA */
    CFGIO_SPI_SEND_BIT_FIRST_T firstBitOrder; /* first Bit order: LSB/MSB */
    CFGIO_SPI_DATA_SIZE_T transferSize;       /* Transfer size in bytes: 1/2/4 */
    STATUS_T status;                            /* Current status of the driver */
    bool driverIdleStatus;                      /* Idle/busy status of the driver */
    SEMAPHORE_T blockSemaphore;                 /* Semaphore used by blocking functions */
    bool blocking;                              /* Specifies whether the current transfer is blocked */
    bool master;                                /* Specifies whether the current ins is initialized as master */
    SPI_CALLBACK_T callback;                    /* User callback function */
    void *callbackParam;                        /* Parameter of callback function */
} CFGIO_SPI_MASTER_STATE_T;

/**@} end of group CFGIO_SPI_Structures*/
/** @defgroup CFGIO_SPI_Typedefs Typedefs
  @{
*/

/* Slave internal structure */
typedef CFGIO_SPI_MASTER_STATE_T CFGIO_SPI_SLAVE_STATE_T;

/**@} end of group CFGIO_SPI_Typedefs*/

/** @defgroup CFGIO_SPI_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T CFGIO_SPI_MasterInit(
    uint32_t ins,
    const CFGIO_SPI_MASTER_CFG_T *configPtr,
    CFGIO_SPI_MASTER_STATE_T *master);
STATUS_T CFGIO_SPI_MasterDeinit(CFGIO_SPI_MASTER_STATE_T *master);
void CFGIO_SPI_MasterDefaultConfig(CFGIO_SPI_MASTER_CFG_T * configPtr);
STATUS_T CFGIO_SPI_MasterReadBaudrate(CFGIO_SPI_MASTER_STATE_T *master, uint32_t *baudrate);
STATUS_T CFGIO_SPI_MasterSetBaudrate(CFGIO_SPI_MASTER_STATE_T *master, uint32_t baudrate);
STATUS_T CFGIO_SPI_MasterTransferNonBlocking(
    CFGIO_SPI_MASTER_STATE_T *master,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize);
STATUS_T CFGIO_SPI_MasterTransferBlocking(
    CFGIO_SPI_MASTER_STATE_T *master,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize,
    uint32_t timeout);
STATUS_T CFGIO_SPI_MasterAbortTransfer(CFGIO_SPI_MASTER_STATE_T *master);
STATUS_T CFGIO_SPI_MasterReadStatus(
    CFGIO_SPI_MASTER_STATE_T *master,
    uint32_t *bytesRemaining);
STATUS_T CFGIO_SPI_SlaveInit(
    uint32_t ins,
    const CFGIO_SPI_SLAVE_CFG_T *configPtr,
    CFGIO_SPI_SLAVE_STATE_T *slave);
STATUS_T CFGIO_SPI_SlaveDeinit(CFGIO_SPI_SLAVE_STATE_T *slave);
void CFGIO_SPI_SlaveDefaultConfig(CFGIO_SPI_SLAVE_CFG_T * configPtr);
STATUS_T CFGIO_SPI_SlaveTransferNonBlocking(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize);
STATUS_T CFGIO_SPI_SlaveTransferBlocking(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    const uint8_t *txData,
    uint8_t *rxData,
    uint32_t dataSize,
    uint32_t timeout);
STATUS_T CFGIO_SPI_SlaveAbortTransfer(CFGIO_SPI_SLAVE_STATE_T *slave);
STATUS_T CFGIO_SPI_SlaveReadStatus(
    CFGIO_SPI_SLAVE_STATE_T *slave,
    uint32_t *bytesRemaining);

/**@} end of group CFGIO_SPI_Functions*/
/**@} end of group CFGIO_SPI_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_CFGIO_SPI_H */
