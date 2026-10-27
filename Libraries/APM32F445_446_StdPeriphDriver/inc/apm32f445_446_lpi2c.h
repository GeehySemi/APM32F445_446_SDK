/*!
 * @file        apm32f445_446_lpi2c.h
 *
 * @brief       This file contains all the functions prototypes for the LPI2C firmware library.
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

#ifndef APM32F445_446_LPI2C_H
#define APM32F445_446_LPI2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "callbacks.h"
#include "apm32f445_446_dma.h"
#include "apm32f445_446_clock.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPI2C_Driver LPI2C Driver
  @{
*/

/** @defgroup LPI2C_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/**
 * Size of the master command queue. Worst case: 5 commands in High-Speed
 * receive with 10-bit address: START + master code, REP START + addr_1 + tx,
 * addr_2, REP START + addr_1 + rx, receive command
 */
#define LPI2C_MASTER_CMD_QUEUE_SIZE   5U

/**@} end of group LPI2C_Macros*/

/** @defgroup LPI2C_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief LPI2C PIN configuration selection
 */
typedef enum
{
    LPI2C_4PIN_PUSH_PULL_INVERTED    = 0x07U,
    LPI2C_2PIN_PUSH_PULL_SLAVE       = 0x06U,
    LPI2C_2PIN_OUTPUT_ONLY_SLAVE     = 0x05U,
    LPI2C_2PIN_OPEN_DRAIN_SLAVE      = 0x04U,
    LPI2C_4PIN_PUSH_PULL             = 0x03U,
    LPI2C_2PIN_PUSH_PULL             = 0x02U,
    LPI2C_2PIN_OUTPUT_ONLY           = 0x01U,
    LPI2C_2PIN_OPEN_DRAIN            = 0x00U,
} LPI2C_PIN_CFG_T;

/*!
 * @brief Type of LPI2C transfer (based on interrupts or DMA).
 */
typedef enum
{
    LPI2C_USE_INTERRUPTS = 0x01U,
    LPI2C_USE_DMA = 0x00U,
} LPI2C_DATA_TRANSFER_TYPE_T;

/*!
 * @brief I2C operating modes
 */
typedef enum
{
    LPI2C_FAST_MODE          = 0x01U,
    LPI2C_STANDARD_MODE      = 0x00U,
} LPI2C_OPERATION_MODE_T;

/*!
 * @brief LPI2C master commands select
 */
typedef enum
{
    LPI2C_MASTER_CMD_START_NACK_HS   = 0x07U,
    LPI2C_MASTER_CMD_START_HS        = 0x06U,
    LPI2C_MASTER_CMD_START_NACK      = 0x05U,
    LPI2C_MASTER_CMD_START           = 0x04U,
    LPI2C_MASTER_CMD_RECEIVE_DISCARD = 0x03U,
    LPI2C_MASTER_CMD_STOP            = 0x02U,
    LPI2C_MASTER_CMD_RECEIVE         = 0x01U,
    LPI2C_MASTER_CMD_TRANSMIT        = 0x00U,
} LPI2C_MASTER_CMD_T;

/*!
 * @brief Master NACK reaction configuration
 */
typedef enum
{
    LPI2C_MASTER_NACK_IGNORE  = 0x01U,
    LPI2C_MASTER_NACK_RECEIVE = 0x00U,
} LPI2C_MASTER_NACK_CFG_T;

/*!
 * @brief LPI2C master prescaler options
 */
typedef enum
{
    LPI2C_MASTER_PSC_DIV_128  = 0x07U,
    LPI2C_MASTER_PSC_DIV_64   = 0x06U,
    LPI2C_MASTER_PSC_DIV_32   = 0x05U,
    LPI2C_MASTER_PSC_DIV_16   = 0x04U,
    LPI2C_MASTER_PSC_DIV_8    = 0x03U,
    LPI2C_MASTER_PSC_DIV_4    = 0x02U,
    LPI2C_MASTER_PSC_DIV_2    = 0x01U,
    LPI2C_MASTER_PSC_DIV_1    = 0x00U,
} LPI2C_MASTER_PSC_T;

/*!
 * @brief LPI2C master interrupts
 */
typedef enum
{
    LPI2C_MASTER_INT_DATA_MATCH         = 0x4000U,
    LPI2C_MASTER_INT_PIN_LOW_TIMEOUT    = 0x2000U,
    LPI2C_MASTER_INT_FIFO_ERROR         = 0x1000U,
    LPI2C_MASTER_INT_ARBITRATION_LOST   = 0x0800U,
    LPI2C_MASTER_INT_NACK_DETECT        = 0x0400U,
    LPI2C_MASTER_INT_STOP_DETECT        = 0x0200U,
    LPI2C_MASTER_INT_END_PACKET         = 0x0100U,
    LPI2C_MASTER_INT_RX_DATA            = 0x0002U,
    LPI2C_MASTER_INT_TX_DATA            = 0x0001U,
} LPI2C_MASTER_INT_T;

/*!
 * @brief Slave address configuration
 */
typedef enum
{
    LPI2C_SLAVE_ADDR_MATCH_RANGE_10BIT         = 0x07U,  /* From Address match 0 (10-bit) to Address match 1 (10-bit) */
    LPI2C_SLAVE_ADDR_MATCH_RANGE_7BIT          = 0x06U,  /* From Address match 0 (7-bit) to Address match 1 (7-bit) */
    LPI2C_SLAVE_ADDR_MATCH_0_10BIT_OR_1_7BIT   = 0x05U,  /* Address match 0 (10-bit) or Address match 1 (7-bit) */
    LPI2C_SLAVE_ADDR_MATCH_0_7BIT_OR_1_10BIT   = 0x04U,  /* Address match 0 (7-bit) or Address match 1 (10-bit) */
    LPI2C_SLAVE_ADDR_MATCH_0_10BIT_OR_1_10BIT  = 0x03U,  /* Address match 0 (10-bit) or Address match 1 (10-bit) */
    LPI2C_SLAVE_ADDR_MATCH_0_7BIT_OR_1_7BIT    = 0x02U,  /* Address match 0 (7-bit) or Address match 1 (7-bit) */
    LPI2C_SLAVE_ADDR_MATCH_0_10BIT             = 0x01U,
    LPI2C_SLAVE_ADDR_MATCH_0_7BIT              = 0x00U,
} LPI2C_SLAVE_ADDR_CFG_T;

/*!
 * @brief LPI2C slave interrupts
 */
typedef enum
{
    LPI2C_SLAVE_INT_TX_DATA                 = 0x0001U,   /* Transmit Data Interrupt        */
    LPI2C_SLAVE_INT_RX_DATA                 = 0x0002U,   /* Receive Data Interrupt         */
    LPI2C_SLAVE_INT_ADDR_VALID              = 0x0004U,   /* Address Valid Interrupt        */
    LPI2C_SLAVE_INT_TRANSMIT_ACK            = 0x0008U,   /* Transmit ACK Interrupt         */
    LPI2C_SLAVE_INT_REPEATED_START          = 0x0100U,   /* Repeated Start Interrupt       */
    LPI2C_SLAVE_INT_STOP_DETECT             = 0x0200U,   /* STOP Detect Interrupt          */
    LPI2C_SLAVE_INT_BIT_ERROR               = 0x0400U,   /* Bit Error Interrupt            */
    LPI2C_SLAVE_INT_FIFO_ERROR              = 0x0800U,   /* FIFO Error Interrupt           */
    LPI2C_SLAVE_INT_ADDR_MATCH_0            = 0x1000U,   /* Address Match 0 Interrupt      */
    LPI2C_SLAVE_INT_ADDR_MATCH_1            = 0x2000U,   /* Address Match 1 Interrupt      */
    LPI2C_SLAVE_INT_GENERAL_CALL            = 0x4000U,   /* General Call Interrupt         */
    LPI2C_SLAVE_INT_SMBUS_ALERT_RESPONSE    = 0x8000U,   /* SMBus Alert Response Interrupt */
} LPI2C_SLAVE_INT_T;

/*!
 * @brief Slave NACK reaction configuration
 */
typedef enum
{
    LPI2C_SLAVE_NACK_TRANSFER_CONTINUE  = 0x01U,
    LPI2C_SLAVE_NACK_TRANSFER_END       = 0x00U,

} LPI2C_SLAVE_NACK_CFG_T;

/*!
 * @brief Slave ACK transmission options
 */
typedef enum
{
    LPI2C_SLAVE_TRANSMIT_NACK  = 0x01U,
    LPI2C_SLAVE_TRANSMIT_ACK   = 0x00U,
} LPI2C_SLAVE_NACK_TRANSMIT_T;

/**@} end of group LPI2C_Enumerations*/

/** @defgroup LPI2C_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief Baudrate structure
 */
typedef struct
{
    uint32_t baudrate;

} LPI2C_BAUDRATE_PARAMS_T;

/*!
 * @brief Master configuration structure
 */
typedef struct
{
    LPI2C_OPERATION_MODE_T i2cMode;             /* I2C Operating mode */
    uint32_t baudrate;                          /* Baudrate */
    LPI2C_DATA_TRANSFER_TYPE_T transferType;    /* Data transfer type: DMA/Interrupt */
    uint8_t dmaChannel;                         /* DMA transfer channel number */
    uint16_t slaveAddr;                         /* Slave address */
    bool isAddrFor10bit;                           /* Selects 7-bit or 10-bit slave address */

    I2C_MASTER_CALLBACK_T masterCallback;       /* Master callback function. Note that this function will be
                                                   called from the interrupt service routine at the end of a transfer,
                                                   so its execution time should be as small as possible. It can be
                                                   NULL if you want to check manually the status of the transfer. */
    void *callbackParam;
} LPI2C_MASTER_USER_CONFIG_T;

/*!
 * @brief Master software command queue
 */
typedef struct
{
    uint8_t readIndex;
    uint8_t writeIndex;
    uint8_t data[LPI2C_MASTER_CMD_QUEUE_SIZE];
    LPI2C_MASTER_CMD_T cmd[LPI2C_MASTER_CMD_QUEUE_SIZE];
} LPI2C_MASTER_CMD_QUEUE_T;

/*!
 * @brief LIN master internal context structure
 */
typedef struct
{
    LPI2C_OPERATION_MODE_T i2cMode;         /* I2C Operating mode */
    LPI2C_DATA_TRANSFER_TYPE_T transferType;/* Type of LPI2C transfer */
    LPI2C_MASTER_CMD_QUEUE_T cmdQueue;      /* Software queue for commands, when LPI2C FIFO is not big enough */
    volatile bool isIdle;                   /* I2C is Idle/busy state */
    uint32_t baudrate;                      /* Baudrate */
    uint16_t slaveAddr;                     /* Slave address */
    uint8_t dmaChannel;                     /* DMA rx channel number */
    volatile uint8_t idleSem;               /* Semaphore used by blocking functions */
    volatile STATUS_T status;               /* Status of last driver operation */
    bool isAddrFor10bit;                    /* Selects 7-bit or 10-bit slave address */
    uint8_t *rxBuffer;                      /* Pointer to Rx data buffer */
    uint32_t rxSize;                        /* Size of Rx data buffer */
    const uint8_t *txBuffer;                /* Pointer to Tx data buffer */
    uint32_t txSize;                        /* Size of Tx data buffer */
    bool isSendStop;                        /* Specifies if STOP condition must be generated after current transfer */
    bool abortedTransfer;                   /* Specifies if master has aborted transfer */
    bool isBlocking;                        /* Specifies if the current transfer is blocking */
    I2C_MASTER_CALLBACK_T masterCallback;   /* Master callback function */
    void *callbackParam;
} LPI2C_MASTER_STATE_T;

/*!
 * @brief Slave configuration structure
 */
typedef struct
{
    LPI2C_OPERATION_MODE_T i2cMode;             /* I2C Operating mode */
    LPI2C_DATA_TRANSFER_TYPE_T transferType;    /* Type of LPI2C transfer */
    uint8_t dmaChannel;                         /* Channel number for DMA rx channel */
    uint16_t slaveAddr;                         /* Slave address */
    bool isAddrFor10bit;                        /* Selects 7-bit or 10-bit slave address */
    bool isListening;                           /* Slave mode (always listening or on demand only) */
    I2C_SLAVE_CALLBACK_T slaveCallback;         /* Slave callback function. Note that this function will be
                                                   called from the interrupt service routine, so its
                                                   execution time should be as small as possible. It can be
                                                   NULL if the slave is not in listening mode (isListening = false) */
    void *callbackParam;
} LPI2C_SLAVE_USER_CONFIG_T;

/*!
 * @brief LIN slave internal context structure
 */
typedef struct
{
    LPI2C_OPERATION_MODE_T i2cMode;             /* I2C Operating mode */
    LPI2C_DATA_TRANSFER_TYPE_T transferType;    /* Type of LPI2C transfer */
    bool isAddrFor10bit;                           /* Specifies if 10-bit or 7-bit address */
    bool isListening;                           /* Slave mode (always listening or on demand only) */
    bool isBlocking;                            /* Specifies if the current transfer is blocking */
    bool isTxUnderrun;                          /* Possible slave tx underrun */
    volatile bool isBusy;                       /* Slave is busy because of an ongoing transfer */
    volatile uint8_t idleSem;                   /* Semaphore used by blocking functions */
    uint8_t dmaChannel;                         /* DMA channel number */
    uint8_t repeatedStarts;                     /* Specifies the number of repeated starts */
    uint32_t txSize;                            /* Size of the Tx buffer */
    uint32_t rxSize;                            /* Size of the Rx buffer */
    const uint8_t *txBuffer;                    /* Pointer to Tx Buffer */
    uint8_t *rxBuffer;                          /* Pointer to Rx Buffer */
    I2C_SLAVE_CALLBACK_T slaveCallback;
    void *callbackParam;
    STATUS_T status;                            /* The I2C slave status */
} LPI2C_SLAVE_STATE_T;

/**@} end of group LPI2C_Structures*/

/** @defgroup LPI2C_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T LPI2C_MasterInit(
    uint32_t instance,
    const LPI2C_MASTER_USER_CONFIG_T *userConfig,
    LPI2C_MASTER_STATE_T *masterState);
STATUS_T LPI2C_MasterDeInit(uint32_t instance);

STATUS_T LPI2C_MasterSetBaudrate(
    uint32_t instance,
    const LPI2C_OPERATION_MODE_T i2cMode,
    const LPI2C_BAUDRATE_PARAMS_T baudrate);
void LPI2C_MasterGetBaudrate(
    uint32_t instance,
    LPI2C_BAUDRATE_PARAMS_T *baudrate);

void LPI2C_MasterSetBusIdleTimeout(uint32_t instance, uint16_t timeout);

void LPI2C_MasterSetSlaveAddr(
    uint32_t instance,
    const uint16_t address,
    const bool isAddrFor10bit);

STATUS_T LPI2C_MasterSendDataNonBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    bool isSendStop);

STATUS_T LPI2C_MasterSendDataBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    bool isSendStop,
    uint32_t timeout);

STATUS_T LPI2C_MasterReceiveDataNonBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    bool isSendStop);

STATUS_T LPI2C_MasterReceiveDataBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    bool isSendStop,
    uint32_t timeout);

STATUS_T LPI2C_MasterAbortTransfer(uint32_t instance);
STATUS_T LPI2C_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemain);
void LPI2C_MasterDefaultConfig(LPI2C_MASTER_USER_CONFIG_T * userConfig);
void LPI2C_MasterIRQHandler(uint32_t instance);

STATUS_T LPI2C_SlaveInit(
    uint32_t instance,
    const LPI2C_SLAVE_USER_CONFIG_T *userConfig,
    LPI2C_SLAVE_STATE_T *slaveState);
STATUS_T LPI2C_SlaveDeInit(uint32_t instance);

STATUS_T LPI2C_SlaveSetRxBuffer(uint32_t instance, uint8_t *rxBuffer, uint32_t rxSize);
STATUS_T LPI2C_SlaveSetTxBuffer(uint32_t instance, const uint8_t *txBuffer, uint32_t txSize);

STATUS_T LPI2C_SlaveReceiveDataNonBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize);

STATUS_T LPI2C_SlaveReceiveDataBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    uint32_t timeout);

STATUS_T LPI2C_SlaveSendDataNonBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize);

STATUS_T LPI2C_SlaveSendDataBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    uint32_t timeout);

STATUS_T LPI2C_SlaveAbortTransfer(uint32_t instance);
STATUS_T LPI2C_SlaveGetTransferStatus(uint32_t instance, uint32_t *bytesRemain);
void LPI2C_SlaveDefaultConfig(LPI2C_SLAVE_USER_CONFIG_T *userConfig);
void LPI2C_SlaveIRQHandler(uint32_t instance);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

uint16_t LPI2C_HW_ReadMasterRxFIFOSize(const LPI2C_T *lpi2cBase);
uint16_t LPI2C_HW_ReadMasterTxFIFOSize(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ResetMasterRxFIFO(LPI2C_T *lpi2cBase);
void LPI2C_HW_ResetMasterTxFIFO(LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterSoftwareReset(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigMasterEnable(LPI2C_T *lpi2cBase, bool enable);
bool LPI2C_HW_ReadMasterReceiveDataReadyEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadMasterTransmitDataRequestEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadMasterFIFOErrorEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadMasterArbitrationLostEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadMasterNACKDetectEvent(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearMasterFIFOErrorEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearMasterArbitrationLostEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearMasterNACKDetectEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearMasterAllEvents(LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterRxDMA(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigMasterTxDMA(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigMasterInt(LPI2C_T *lpi2cBase, uint32_t interrupts, bool enable);
void LPI2C_HW_ConfigMasterPinMode(LPI2C_T *lpi2cBase, LPI2C_PIN_CFG_T pinConfig);
void LPI2C_HW_ConfigMasterNACKMode(LPI2C_T *lpi2cBase, LPI2C_MASTER_NACK_CFG_T nackConfig);
void LPI2C_HW_ConfigMasterPrescaler(LPI2C_T *lpi2cBase, LPI2C_MASTER_PSC_T prescaler);
LPI2C_MASTER_PSC_T LPI2C_HW_ReadMasterPrescaler(const LPI2C_T *lpi2cBase);
uint8_t LPI2C_HW_ReadMasterSCLHighPeriod(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterSCLHighPeriod(LPI2C_T *lpi2cBase, uint8_t value);
void LPI2C_HW_ConfigMasterSDAHoldTime(LPI2C_T *lpi2cBase, uint8_t value);
void LPI2C_HW_ConfigMasterSetupHoldDelay(LPI2C_T *lpi2cBase, uint8_t value);
void LPI2C_HW_ConfigMasterSCLLowPeriod(LPI2C_T *lpi2cBase, uint8_t value);
uint8_t LPI2C_HW_ReadMasterSCLLowPeriod(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterRxFIFOWatermark(LPI2C_T *lpi2cBase, uint16_t value);
uint16_t LPI2C_HW_ReadMasterRxFIFOWatermark(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterTxFIFOWatermark(LPI2C_T *lpi2cBase, uint16_t value);
uint16_t LPI2C_HW_ReadMasterRxFIFOCount(const LPI2C_T *lpi2cBase);
uint16_t LPI2C_HW_ReadMasterTxFIFOCount(const LPI2C_T *lpi2cBase);
void LPI2C_HW_MasterTransmitCmdData(LPI2C_T *lpi2cBase, LPI2C_MASTER_CMD_T cmd, uint8_t data);
uint8_t LPI2C_HW_ReadMasterRxData(const LPI2C_T *lpi2cBase);

void LPI2C_HW_ConfigSlaveEnable(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveSoftwareReset(LPI2C_T *lpi2cBase, bool enable);
bool LPI2C_HW_ReadSlaveBitErrorEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadSlaveSTOPDetectEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadSlaveRepeatedStartEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadSlaveAddressValidEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadSlaveReceiveDataEvent(const LPI2C_T *lpi2cBase);
bool LPI2C_HW_ReadSlaveTransmitDataEvent(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearSlaveBitErrorEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearSlaveSTOPDetectEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ClearSlaveRepeatedStartEvent(LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigSlaveInt(LPI2C_T *lpi2cBase, uint32_t interrupts, bool enable);
bool LPI2C_HW_ReadSlaveInt(const LPI2C_T *lpi2cBase, uint32_t interrupts);
void LPI2C_HW_ConfigSlaveRxDMA(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveTxDMA(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveAddr(LPI2C_T *lpi2cBase, LPI2C_SLAVE_ADDR_CFG_T addrConfig);
void LPI2C_HW_ConfigSlaveHighSpeedModeDetect(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveIgnoreNACK(LPI2C_T *lpi2cBase, LPI2C_SLAVE_NACK_CFG_T nackConfig);
void LPI2C_HW_ConfigSlaveACKStall(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveTXDStall(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveRXStall(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveAddrStall(LPI2C_T *lpi2cBase, bool enable);
void LPI2C_HW_ConfigSlaveAddr0(LPI2C_T *lpi2cBase, uint16_t address0);
uint16_t LPI2C_HW_ReadSlaveReceivedAddr(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigSlaveTransmitNACK(LPI2C_T *lpi2cBase, LPI2C_SLAVE_NACK_TRANSMIT_T ackTransmit);
void LPI2C_HW_TxSlaveData(LPI2C_T *lpi2cBase, uint8_t data);
uint8_t LPI2C_HW_RxSlaveData(const LPI2C_T *lpi2cBase);
void LPI2C_HW_ConfigMasterBusIdleTimeoutValue(LPI2C_T *lpi2cBase, uint16_t timeout);
void LPI2C_HW_Init(LPI2C_T *lpi2cBase);

/**@} end of group LPI2C_Functions*/
/**@} end of group LPI2C_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined(__cplusplus)
}
#endif

#endif /* APM32F445_446_LPI2C_H */
