/*!
 * @file        apm32f445_446_cfgio_i2c.h
 *
 * @brief       Header of CFGIO_I2C driver functions
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

#ifndef APM32F445_446_CFGIO_I2C_H
#define APM32F445_446_CFGIO_I2C_H

#include "device.h"
#include "apm32f445_446_cfgio_common.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_interrupt.h"
#include "apm32f445_446_dma.h"
#include "callbacks.h"
#include "osif.h"

#ifdef __cplusplus
extern "C" {
#endif


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

/**
 * Length of the DMA scatter-gather chain for Tx. Two blocks are needed, but as
 * the fist one is loaded directly in the TCD, only one block needs to be in RAM.
 */
#define CFGIO_I2C_DMA_TX_CHAIN_LENGTH     (1U)

/**
 * Length of the DMA scatter-gather chain for Rx. Six blocks are needed, but as
 * the fist one is loaded directly in the TCD, only five blocks needs to be in RAM.
 */
#define CFGIO_I2C_DMA_RX_CHAIN_LENGTH     (5U)

/**
 * Total size of the DMA scatter-gather buffer. Add 1 because STCD_SIZE already
 * compensates for the first STCD not being loaded in RAM.
 */
#define CFGIO_I2C_DMA_BUF_SIZE   (STCD_SIZE(CFGIO_I2C_DMA_TX_CHAIN_LENGTH + \
                                              CFGIO_I2C_DMA_RX_CHAIN_LENGTH + 1U))

/**@} end of group CFGIO_I2C_Macros*/
/** @defgroup CFGIO_I2C_Enumerations Macros
  @{
*/
/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/
/*!
 * @brief   I2C data transfer direction
 */
typedef enum {
    CFGIO_I2C_TX,
    CFGIO_I2C_RX
} CFGIO_I2C_DIR_T;

/*!
 * @brief   I2C bus lines
 */
typedef enum {
    CFGIO_I2C_SDA_LINE,
    CFGIO_I2C_SCL_LINE
} CFGIO_I2C_BUS_LINE_T;

/**@} end of group CFGIO_I2C_Enumerations*/

/** @defgroup CFGIO_I2C_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief   I2C master user configuration
 */
typedef struct
{
    uint16_t slaveAddr;     /* Slave address 7-bit */
    uint8_t sdaPin;         /* CFGIO pin to use as I2C SDA pin */
    uint8_t sclPin;         /* CFGIO pin to use as I2C SCL pin */
    uint32_t baudrate;      /* Baudrate in Hertz */
    CFGIO_DRIVER_TYPE_T transferType; /* Data transfer type: Interrupts/DMA/Polling */
    uint8_t txDmaChannel;           /* Tx DMA channel number */
    uint8_t rxDmaChannel;           /* Rx DMA channel number */
    I2C_MASTER_CALLBACK_T callback; /* User callback function. Note that this function will be
                                       called from the interrupt service routine, so its
                                       execution time should be as small as possible. It can be
                                       NULL if it is not needed */
    void *callbackParam;            /* Parameter for the callback function */
} CFGIO_I2C_MASTER_USER_CONFIG_T;

/*!
 * @brief   I2C Master internal states
 * @details This structure is used by the driver for its internal logic.
 *          It must be provided by the application through the
 *          CFGIO_I2C_MasterInit() function, then it cannot be freed until
 *          the driver is reset using CFGIO_I2C_MasterDeinit(). The
 *          application should make no assumptions about the content of this
 *          structure.
 */
typedef struct
{
    CFGIO_COMMON_STATE_T commonState; /* CFGIO driver structure */
    uint16_t slaveAddr;                 /* Slave address */
    uint8_t sdaPin;                     /* CFGIO pin to use as I2C SDA pin */
    uint8_t sclPin;                     /* CFGIO pin to use as I2C SCL pin */
    CFGIO_DRIVER_TYPE_T transferType; /* Data transfer type: Interrupts/DMA/Polling */
    uint8_t txDmaChannel;               /* Tx DMA channel number */
    uint8_t rxDmaChannel;               /* Rx DMA channel number */
    I2C_MASTER_CALLBACK_T callback;     /* User callback function */
    void *callbackParam;                /* Parameter for the callback function */
    const uint8_t *txBuf;               /* Transmit buffer */
    uint8_t *rxBuf;                     /* Receive buffer */
    uint32_t txBytesLeft;               /* Number of remaining bytes to be transmitted */
    uint32_t rxBytesLeft;               /* Number of remaining bytes to be received */
    uint16_t eventCount;                /* Number of timer events, needed for long transfers */
    STATUS_T driverStatus;              /* Current status of the driver */
    CFGIO_I2C_DIR_T transferDir;      /* Data transfer direction */
    bool addrReceived;                  /* Indicate start of receive (after address transmission) */
    bool isIdle;                        /* Idle state of the driver */
    bool isBlocking;                    /* Specifies if the current transfer is blocking */
    bool sendStop;                      /* Specifies if STOP condition shall be generated after current transfer */
    SEMAPHORE_T idleSemaphore;          /* Semaphore used by blocking functions */
    uint8_t dummyDmaReceive;            /* Dummy location for DMA transfers */
    uint8_t dummyDmaIdle;               /* Dummy location for DMA transfers */
    uint8_t dummyDmaStop;               /* Dummy location for DMA transfers */
    uint8_t dmaTxStop0;                 /* Setting for Tx shifter stop bit to 0 */
    uint8_t dmaTxStop1;                 /* Setting for Tx shifter stop bit to 1 */
    uint8_t dmaRxStop1;                 /* Setting for Rx shifter stop bit to 1 */
    uint8_t reloadCounter;              /* Counter value for the last timer reload */
    uint8_t stcdBuf[CFGIO_I2C_DMA_BUF_SIZE];  /* Buffer for DMA scatter-gather operations */
} CFGIO_I2C_MASTER_STATE_T;

/**@} end of group CFGIO_I2C_Structures*/


/** @defgroup CFGIO_I2C_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGIO_I2C_MasterInit(
    uint32_t instance,
    const CFGIO_I2C_MASTER_USER_CONFIG_T *configPtr,
    CFGIO_I2C_MASTER_STATE_T *states);

STATUS_T CFGIO_I2C_MasterDeinit(CFGIO_I2C_MASTER_STATE_T *states);

void CFGIO_I2C_DefaultConfig(CFGIO_I2C_MASTER_USER_CONFIG_T *configPtr);

STATUS_T CFGIO_I2C_MasterGetBaudrate(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t *baudrate);

STATUS_T CFGIO_I2C_MasterSetBaudrate(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t baudrate);

STATUS_T CFGIO_I2C_MasterSetSlaveAddr(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint16_t address);

STATUS_T CFGIO_I2C_MasterWriteNonBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    const uint8_t *txBuf,
    uint32_t txLen,
    bool sendStop);

STATUS_T CFGIO_I2C_MasterWriteBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    const uint8_t *txBuf,
    uint32_t txLen,
    bool sendStop,
    uint32_t timeout);

STATUS_T CFGIO_I2C_MasterReadNonBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint8_t *rxBuf,
    uint32_t rxLen,
    bool sendStop);

STATUS_T CFGIO_I2C_MasterReadBlocking(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint8_t *rxBuf,
    uint32_t rxLen,
    bool sendStop,
    uint32_t timeout);

STATUS_T CFGIO_I2C_MasterAbortTransfer(CFGIO_I2C_MASTER_STATE_T *states);

STATUS_T CFGIO_I2C_MasterGetStatus(
    CFGIO_I2C_MASTER_STATE_T *states,
    uint32_t *bytesLeft);

bool CFGIO_I2C_GetBusStatus(
    const CFGIO_I2C_MASTER_STATE_T *states,
    CFGIO_I2C_BUS_LINE_T line);

STATUS_T CFGIO_I2C_GenerateNineClock(CFGIO_I2C_MASTER_STATE_T *states);
STATUS_T CFGIO_I2C_StatusGenerateNineClock(CFGIO_I2C_MASTER_STATE_T *states);

/**@} end of group CFGIO_I2C_Functions*/
/**@} end of group CFGIO_I2C_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_CFGIO_I2C_H */
