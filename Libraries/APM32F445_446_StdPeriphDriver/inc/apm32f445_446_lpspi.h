/*!
 * @file        apm32f445_446_spi.h
 *
 * @brief       This file provides all the POWER firmware functions
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

#ifndef APM32F445_446_SPI_H
#define APM32F445_446_SPI_H

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

/** @addtogroup LPSPI_Driver LPSPI Driver
  @{
*/

/** @defgroup LPSPI_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                                ENUMS
 ******************************************************************************/
/* LPSPI delay type selection */
typedef enum
{
    LPSPI_BETWEEN_TRANSFER = 0x8U,
    LPSPI_PCS_TO_SCK_DELAY = 0x16U,
    LPSPI_SCK_TO_PCS_DELAY = 0x24U
} LPSPI_DELAY_TYPE_T ;

/* LPSPI transfer width configuration */
typedef enum
{
    LPSPI_FOUR_BIT_TRANSFER = 0x02U,    /* 4-bits shift output on SDO/SDI/PCS[3:2]
                                            and input on SDO/SDI/PCS[3:2] */
    LPSPI_TWO_BIT_TRANSFER = 0x01U,     /* 2-bits shift output on SDO/SDI and input on SDO/SDI */
    LPSPI_ONE_BIT_TRANSFER = 0x00U      /* 1-bit shift at a time, data output on SDO,
                                            data input on SDI*/
} LPSPI_WIDTH_TRANSFER_T;

/* LPSPI data output configuration */
typedef enum
{
    LPSPI_DATA_OUTPUT_TRISTATE = 0x01U,
    LPSPI_DATA_OUTPUT_RETAINED = 0x00U
} LPSPI_DATA_OUTPUT_CFG_T;

/* LPSPI SDO/SDI configure as input/output */
typedef enum
{
    LPSPI_SDI_OUT_SDO_IN = 0x03U,
    LPSPI_SDO_IN_OUT     = 0x02U,
    LPSPI_SDI_IN_OUT     = 0x01U,
    LPSPI_SDI_IN_SDO_OUT = 0x00U
} LPSPI_PIN_CFG_T;

/* Select the LPSPI as the master or slave */
typedef enum
{
    LPSPI_MASTER_MODE = 1U,
    LPSPI_SLAVE_MODE  = 0U
} LPSPI_SELECT_MODE_T;

/* LPSPI status flags */
typedef enum
{
    LPSPI_ALL_STATUS             = 0x00003F00U,
    LPSPI_DATA_MODULE_BUSY       = 24U,
    LPSPI_DATA_MATCH_FLAG        = 13U,
    LPSPI_RX_ERROR_FLAG          = 12U,
    LPSPI_TX_ERROR_FLAG          = 11U,
    LPSPI_TRANSFER_COMPLETE_FLAG = 10U,
    LPSPI_FRAME_COMPLETE_FLAG    = 0x09U,
    LPSPI_WORD_COMPLETE_FLAG     = 0x08U,
    LPSPI_RX_DATA_FLAG           = 0x01U,
    LPSPI_TX_DATA_FLAG           = 0x00U
} LPSPI_STATUS_FLAG_T;

/* Number of LPSPI prescalers */
 typedef enum
{
    LPSPI_DIV_128   = 0x07U,
    LPSPI_DIV_64    = 0x06U,
    LPSPI_DIV_32    = 0x05U,
    LPSPI_DIV_16    = 0x04U,
    LPSPI_DIV_8     = 0x03U,
    LPSPI_DIV_4     = 0x02U,
    LPSPI_DIV_2     = 0x01U,
    LPSPI_DIV_1     = 0x00U
} LPSPI_PRESCALER_T;

 /* Error type of LPSPI */
typedef enum
{
    LPSPI_RX_FAIL = 0x02U,
    LPSPI_TX_FAIL = 0x01U,
    LPSPI_TRANSMISSION_OK = 0x00U
} LPSPI_TRANSFER_STATUS_T;

/* Type of LPSPI transfer */
typedef enum
{
    LPSPI_TRANSFER_INTERRUPTS = 0x01U,
    LPSPI_TRANSFER_DMA = 0x00U
} LPSPI_TRANSFER_T;

/* LPSPI Clock Signal (SCK) Polarity configuration */
typedef enum
{
    LPSPI_CLK_SIGNAL_LOW  = 0x01U,
    LPSPI_CLK_SIGNAL_HIGH = 0x00U
} LPSPI_CLK_SIGNAL_T;

/* LPSPI clock phase configuration */
typedef enum
{
    LPSPI_CLOCK_PHASE_CFG_2ND = 0x01U,
    LPSPI_CLOCK_PHASE_CFG_1ST = 0x00U
} LPSPI_CLOCK_PHASE_CFG_T;

/* LPSPI Signal Active High/low  */
typedef enum
{
    LPSPI_SIGNAL_ACTIVE_HIGH = 0x01U,
    LPSPI_SIGNAL_ACTIVE_LOW  = 0x00U
} LPSPI_SIGNAL_ACTIVE_T;

/* LPSPI Peripheral PCS selection */
typedef enum
{
    LPSPI_PER_PCS3 = 0x03U,
    LPSPI_PER_PCS2 = 0x02U,
    LPSPI_PER_PCS1 = 0x01U,
    LPSPI_PER_PCS0 = 0x00U
} LPSPI_PER_PCS_T;



/**@} end of group LPSPI_Enumerations */

/** @defgroup LPSPI_Structures Structures
  @{
*/
/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/
/* Runtime state structure of the LPSPI master driver */
typedef struct
{
    uint32_t dummy;
    void *callbackParam;
    SPI_CALLBACK_T callback;
    LPSPI_TRANSFER_STATUS_T lpspiStatus;
    SEMAPHORE_T lpspiSem;
    LPSPI_TRANSFER_T transferType;
    uint8_t txDMAChan;
    uint8_t rxDMAChan;
    uint8_t fifoRxTxSize;
    volatile bool lsbTrue;               /* True for LSB first, false for MSB first */
    volatile uint16_t rxFrameCount;
    volatile uint16_t txFrameCount;
    volatile uint16_t rxNumOfBytes;
    volatile uint16_t txNumOfBytes;
    uint8_t * rxBuff;
    const uint8_t * txBuff;
    volatile bool inTransfer;
    uint32_t lpspiModSrcClk;
    bool isBlocking;
    bool pcsKeeps;                       /* Choose to keep the chip selection assertion
                                            until the transfer is completed */
    uint16_t perFrameNumOfBytes;
    uint16_t perFrameNumOfBits;
} LPSPI_STATE_T;


/* A Data structure that contains information about the devices on the SPI bus */
typedef struct
{
    void *callbackParam;
    SPI_CALLBACK_T callback;
    uint8_t txDMAChan;
    uint8_t rxDMAChan;
    LPSPI_TRANSFER_T transferType;
    bool sendLsbFirst;
    LPSPI_CLK_SIGNAL_T clkPolar;
    LPSPI_CLOCK_PHASE_CFG_T selectClkPhase;
    uint32_t lpspiModSrcClk;
    uint16_t bitNumber;
    bool pcsKeeps;
    LPSPI_SIGNAL_ACTIVE_T selectPcsPolar;
    LPSPI_PER_PCS_T selectPcs;
    uint32_t baudBitsPer;
} LPSPI_MASTER_CFG_T;

/* A Data structure that contains information about the devices on the SPI bus */
typedef struct
{
    void *callbackParam;
    SPI_CALLBACK_T callback;
    uint8_t txDMAChan;
    uint8_t rxDMAChan;
    LPSPI_TRANSFER_T transferType;
    bool sendLsbFirst;
    LPSPI_CLK_SIGNAL_T clkPolar;
    LPSPI_PER_PCS_T selectPcs;
    LPSPI_CLOCK_PHASE_CFG_T selectClkPhase;
    uint16_t bitNumber;
    LPSPI_SIGNAL_ACTIVE_T selectPcsPolar;
} LPSPI_SLAVE_CFG_T;

/* Configuration structure of LPSPI sending command registers */
typedef struct
{
    LPSPI_CLK_SIGNAL_T clkPolar;
    LPSPI_CLOCK_PHASE_CFG_T selectClkPhase;
    uint32_t preDiv;
    LPSPI_PER_PCS_T selectPcs;
    bool sendLsbFirst;
    bool byteInter;
    bool contentTransfer;
    bool contentCmd;
    bool rxDataMask;
    bool txDataMask;
    LPSPI_WIDTH_TRANSFER_T width;
    uint32_t frameNum;
} LPSPI_TX_CMD_CFG_T;

/* LPSPI initialization the configuration structure */
typedef struct
{
    LPSPI_SIGNAL_ACTIVE_T lpspiPcsPol;
    LPSPI_SELECT_MODE_T lpspiMode;
    uint32_t lpspiBaudRate;
    uint32_t lpspiModSrcClk;
} LPSPI_INIT_CFG_T;

/**@} end of group LPSPI_Structures*/

/** @defgroup LPSPI_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T LPSPI_MasterGetTransferStatus(
    uint32_t ins,
    uint32_t * remainingBytes);
STATUS_T LPSPI_SlaveGetTransferStatus(
    uint32_t ins,
    uint32_t * remainingBytes);
STATUS_T LPSPI_MasterTransfer(
    uint32_t ins,
    const uint8_t * txDataBuffer,
    uint8_t * rxDataBuffer,
    uint16_t transByteNumber);
STATUS_T LPSPI_SlaveTransfer(
    uint32_t ins,
    const uint8_t *txDataBuffer,
    uint8_t *rxDataBuffer,
    uint16_t transByteNumber);
STATUS_T LPSPI_MasterTransferBlocking(
    uint32_t ins,
    const uint8_t * txDataBuffer,
    uint8_t * rxDataBuffer,
    uint16_t transByteNumber,
    uint32_t timeout);
STATUS_T LPSPI_SlaveTransferBlocking(
    uint32_t ins,
    const uint8_t *txDataBuffer,
    uint8_t *rxDataBuffer,
    uint16_t transByteNumber,
    uint32_t timeout);
STATUS_T LPSPI_MasterConfigPcs(
    uint32_t ins,
    LPSPI_PER_PCS_T selectPcs,
    LPSPI_SIGNAL_ACTIVE_T pcsPolar);
STATUS_T LPSPI_MasterConfigDelay(
    uint32_t ins,
    uint32_t delayBetweenTransfers,
    uint32_t delaySCKtoPCS,
    uint32_t delayPCStoSCK);
STATUS_T LPSPI_MasterDeInit(uint32_t ins);
STATUS_T LPSPI_SlaveDeInit(uint32_t ins);
STATUS_T LPSPI_MasterInit(
    uint32_t ins,
    LPSPI_STATE_T * spiState,
    const LPSPI_MASTER_CFG_T * spiCfg);
STATUS_T LPSPI_SlaveInit(
    uint32_t ins,
    LPSPI_STATE_T * spiState,
    const LPSPI_SLAVE_CFG_T * slaveConfigure);
void LPSPI_MasterDefaultConfig(LPSPI_MASTER_CFG_T * spiCfg);
void LPSPI_SlaveDefaultConfig(LPSPI_SLAVE_CFG_T * spiCfg);
/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
void LPSPI_HW_Init(LPSPI_T * base);
void LPSPI_HW_Enable(LPSPI_T * base);
bool LPSPI_HW_IsMaster(const LPSPI_T * base);
void LPSPI_HW_GetFifoSizes(
    const LPSPI_T * base,
    uint8_t * fifoRxTxSize);
void LPSPI_HW_ConfigRxWatermarks(
    LPSPI_T * base,
    uint32_t rxWater);
void LPSPI_HW_ConfigTxWatermarks(
    LPSPI_T * base,
    uint32_t txWater);
bool LPSPI_HW_GetStatusFlag(
    const LPSPI_T * base,
    LPSPI_STATUS_FLAG_T flagState);
void LPSPI_HW_ConfigIntMode(
    LPSPI_T * base,
    LPSPI_STATUS_FLAG_T interruptSrc,
    bool enable);
bool LPSPI_HW_GetIntMode(
    const LPSPI_T * base,
    LPSPI_STATUS_FLAG_T interruptSrc);
void LPSPI_HW_ConfigTxDmaCmd(LPSPI_T * base, bool enable);
void LPSPI_HW_ConfigRxDmaCmd(LPSPI_T * base, bool enable);
STATUS_T LPSPI_HW_ConfigDelay(
    LPSPI_T * base,
    LPSPI_DELAY_TYPE_T whichDelay,
    uint32_t delay);
STATUS_T LPSPI_HW_Disable(LPSPI_T * base);
STATUS_T LPSPI_HW_ConfigMasterSlaveMode(
    LPSPI_T * base,
    LPSPI_SELECT_MODE_T mode);
void LPSPI_HW_ConfigFlushFifoCmd(
    LPSPI_T * base,
    bool flushTxFifo,
    bool flushRxFifo);
STATUS_T LPSPI_HW_ClearStatusFlag(
    LPSPI_T * base,
    LPSPI_STATUS_FLAG_T flagState);
STATUS_T LPSPI_HW_ConfigPcsPolarityMode(
    LPSPI_T * base,
    LPSPI_PER_PCS_T selectPcs,
    LPSPI_SIGNAL_ACTIVE_T selectPcsPolar);
STATUS_T LPSPI_HW_ConfigPinConfigMode(
    LPSPI_T * base,
    LPSPI_PIN_CFG_T pinCfg,
    LPSPI_DATA_OUTPUT_CFG_T dataOutConfig,
    bool pcs3and2Enable);
uint32_t LPSPI_HW_ConfigBaudRate(
    LPSPI_T * base,
    uint32_t baudBitsPer,
    uint32_t sourceClkInHz,
    uint32_t * tcrPreVal);
STATUS_T LPSPI_HW_ConfigBaudRateDivisor(LPSPI_T * base, uint32_t divisor);
void LPSPI_HW_ConfigPcs(LPSPI_T * base, LPSPI_PER_PCS_T selectPcs);
void LPSPI_HW_WriteData(LPSPI_T * base, uint32_t data);
uint32_t LPSPI_HW_ReadData(const LPSPI_T * base);
uint32_t LPSPI_HW_ReadTxCount(const LPSPI_T * base);
uint32_t LPSPI_HW_ReadRxCount(const LPSPI_T * base);
void LPSPI_HW_ClearRxmaskBit(LPSPI_T * base);
void LPSPI_HW_ConfigRxmaskBit(LPSPI_T * base);
void LPSPI_HW_ClearTxmaskBit(LPSPI_T * base);
void LPSPI_HW_ConfigTxmskBit(LPSPI_T * base);
void LPSPI_HW_ClearContCBit(LPSPI_T * base);
void LPSPI_HW_ConfigContCBit(LPSPI_T * base);
void LPSPI_HW_ConfigClockPrescaler(
    LPSPI_T * base,
    LPSPI_PRESCALER_T prescaler);
LPSPI_PRESCALER_T LPSPI_HW_ReadClockPrescaler (const LPSPI_T * base);
void LPSPI_HW_ConfigSamplingPoint(
    LPSPI_T * base,
    bool isSamplingPointDelayed);
void LPSPI_HW_ConfigTxCommandReg(
    LPSPI_T * base,
    const LPSPI_TX_CMD_CFG_T * txCmdCfgSet);

/**@} end of group LPSPI_Functions*/
/**@} end of group LPSPI_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_SPI_H */
