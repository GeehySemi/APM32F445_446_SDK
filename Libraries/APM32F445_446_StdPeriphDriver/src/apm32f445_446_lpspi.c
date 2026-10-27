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

/* Includes */
#include "apm32f445_446_lpspi.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup LPSPI_Driver LPSPI Driver
  @{
*/

/** @defgroup LPSPI_Macros Macros
  @{
*/
/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define LPSPI_INSTANCE_NUM                     (3u)

#define LPSPI_INSTANCE_VALID(__instance__)    if(__instance__ >= LPSPI_INSTANCE_NUM){while(1);}

/**@} end of group LPSPI_Macros*/

/** @defgroup LPSPI_Variables Variables
  @{
*/
static const uint32_t g_lpspiBaudRatePre[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

/* Base pointers table for SPI instances */
LPSPI_T * g_lpspiBasePointer[LPSPI_INSTANCE_NUM];

/* A Table to hold the LPSPI IRQ enumeration numbers defined in the CMSIS header file */
IRQn_Type g_lpspiIrqIdNumber[LPSPI_INSTANCE_NUM];

/* Pointer to run-time state structure */
LPSPI_STATE_T * g_lpspiStatusPointer[LPSPI_INSTANCE_NUM];

/* Base pointer table of the SPI instance */
LPSPI_T * g_lpspiBasePointer[LPSPI_INSTANCE_NUM] =  { LPSPI0, LPSPI1, LPSPI2 };

/* LPSPI IRQ enumeration number table */
IRQn_Type g_lpspiIrqIdNumber[LPSPI_INSTANCE_NUM] =  { LPSPI0_IRQn, LPSPI1_IRQn, LPSPI2_IRQn };

/* A pointer to a runtime state structure */
LPSPI_STATE_T * g_lpspiStatusPointer[LPSPI_INSTANCE_NUM] = FEATURE_LPSPI_STATUS_NULL;

/**@} end of group LPSPI_Variables*/

/** @defgroup LPSPI_Functions Functions
  @{
*/
/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
void LPSPI_MasterIRQHandler(uint32_t ins);
void LPSPI_SlaveIRQHandler(uint32_t ins);
void LPSPI0_IRQHandler(void);
void LPSPI1_IRQHandler(void);
void LPSPI2_IRQHandler(void);
void LPSPI_IRQHandler(uint32_t ins);

static uint32_t AbsDiff(uint32_t a, uint32_t b);
static void LPSPI_MasterCompleteTransfer(uint32_t ins);
static void LPSPI_MasterCompleteDMATransfer(void* para,
                                            DMA_CHANNEL_STATUS_T status);
static void LPSPI_MasterCompleteRX(void* para,
                                   DMA_CHANNEL_STATUS_T status);
static void LPSPI_SlaveCompleteDMATransfer(void* para,
                                           DMA_CHANNEL_STATUS_T status);
static void LPSPI_FillupTxBuffer(uint32_t ins);
static void LPSPI_ReadRxBuffer(uint32_t ins);
static void LPSPI_DisableErrorInterrupts(uint32_t ins);
static STATUS_T LPSPI_MasterConfigureBus(uint32_t ins,
                                  const LPSPI_MASTER_CFG_T * spiCfg,
                                  uint32_t * calBaudRate);
static STATUS_T LPSPI_MasterAbortTransfer(uint32_t ins);
static STATUS_T LPSPI_MasterStartTransfer(uint32_t ins,
                                   const uint8_t * txDataBuffer,
                                   uint8_t * rxDataBuffer,
                                   uint16_t transByteNumber);
static STATUS_T LPSPI_SlaveAbortTransfer(uint32_t ins);
static uint16_t LPSPI_BitsToBytes(uint16_t numOfBits);
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
/*!
 * @brief   Function LPSPI_IRQHandler passes IRQ control to the master or slave drive
 *
 * @details Before calling the IRQ handler,check their addresses to ensure they are not zero.
 *          if the address of the IRQ handler is zero,it means driver does not exist in the
 *          link(because the IRQ handler is marked weak)
 *
 * @param   ins : Base address of the current SPI
 *
 */
void LPSPI_IRQHandler(uint32_t ins)
{
    if (ins >= LPSPI_INSTANCE_NUM)
    {
        return;
    }

    const LPSPI_T *base = g_lpspiBasePointer[ins];

    if (LPSPI_HW_IsMaster(base))
    {
        LPSPI_MasterIRQHandler(ins);
    }
    else
    {
        LPSPI_SlaveIRQHandler(ins);
    }
}


/*!
 * @brief   Fill TX FIFO with data
 *
 * @details This function populates the TX FIFO with byte/frame based data
 *
 * @param   ins : Base address of the current SPI
 *
 */
void LPSPI_FillupTxBuffer(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    uint32_t txData = 0;
    uint16_t bytes;

    /* Instantiate a local variable of
        type LPSPI_STATE_T and point to the global state. */
    LPSPI_STATE_T * spiDirState = g_lpspiStatusPointer[ins];
    LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];
    uint8_t freeSpace = (uint8_t)(spiDirState->fifoRxTxSize -
                                  (uint8_t)LPSPI_HW_ReadTxCount(lpspiBase));

    /* Fill TX buffer */
    for (; freeSpace != 0; freeSpace--)
    {
        if ((spiDirState->pcsKeeps == true) &&
            (spiDirState->txNumOfBytes == 1U))
        {
            /* Disable continuous PCS */
            LPSPI_HW_ClearContCBit(lpspiBase);
            spiDirState->txNumOfBytes  = 0U;
            break;
        }
        /* Get the number of bytes that can be written to a single 32-bit word */
        if ((spiDirState->perFrameNumOfBytes -
             spiDirState->txFrameCount) > (uint16_t)4U)
        {
            bytes = 4U;
        }
        else
        {
            bytes = (uint16_t)(spiDirState->perFrameNumOfBytes -
                               spiDirState->txFrameCount);
        }

        txData = 0U;

        if (POINTER_IS_NULL(spiDirState->txBuff) == false)
        {
            if(spiDirState->perFrameNumOfBytes == 2U)
            {
                txData = *((const uint16_t *)(spiDirState->txBuff));
                spiDirState->txBuff = sizeof(uint16_t) +
                                        (spiDirState->txBuff);
            }
            else if(spiDirState->perFrameNumOfBytes == 1U)
            {
                txData = *((const uint8_t *)(spiDirState->txBuff));
                spiDirState->txBuff = sizeof(uint8_t) +
                                        (spiDirState->txBuff);
            }
            else
            {
                txData = *((const uint32_t *)(spiDirState->txBuff));
                spiDirState->txBuff = sizeof(uint32_t) +
                                        (spiDirState->txBuff);
            }
            spiDirState->txFrameCount = (uint16_t)((spiDirState->txFrameCount + bytes) %
                                                spiDirState->perFrameNumOfBytes);
        }
        LPSPI_HW_WriteData(lpspiBase, txData);
        /* Update internal variables used in transmission */
        spiDirState->txNumOfBytes = (uint16_t)(spiDirState->txNumOfBytes - bytes);
        /* Verify that all bytes have been sent */
        if (spiDirState->txNumOfBytes == 0U)
        {
            break;
        }
    }
}

/*!
 * @brief   Read all data from RX FIFO
 *
 * @details This function will read all data from RX FIFO and will transfer this information to
 *          RX software buffer.
 *
 * @param   ins : Base address of the current SPI
 *
 */
void LPSPI_ReadRxBuffer(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    uint16_t x;
    uint32_t rxData;
    uint16_t bytes;

    const LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiDirState = g_lpspiStatusPointer[ins];
    uint8_t fillSpace = (uint8_t)LPSPI_HW_ReadRxCount(lpspiBase);

    for (; fillSpace != 0; fillSpace--)
    {
        rxData = LPSPI_HW_ReadData(lpspiBase);
        /* Get the number of bytes which can be read from this 32 bites */
        if ((spiDirState->perFrameNumOfBytes -
             spiDirState->rxFrameCount) > (uint16_t)4U)
        {
            bytes = 4U;
        }
        else
        {
            bytes = (uint16_t)(spiDirState->perFrameNumOfBytes -
                               spiDirState->rxFrameCount);
        }

        /* Generate words to write to buffer */
        for (x = 0; x < bytes; x++)
        {
            *(spiDirState->rxBuff) = (uint8_t)(rxData >> (x * 8U));
            spiDirState->rxBuff++;
        }
        spiDirState->rxFrameCount = (uint16_t)((spiDirState->rxFrameCount + bytes) %
                                            spiDirState->perFrameNumOfBytes);

        /* Update the internal variables used in the transmission */
        spiDirState->rxNumOfBytes = (uint16_t)(spiDirState->rxNumOfBytes - bytes);
        /* Verify if all bytes have been sent */
        if (spiDirState->rxNumOfBytes == 0U)
        {
            break;
        }
    }
}


/*!
 * @brief   Disable error interrupts at the end of the transfer.
 *
 * @details Close interrupt and clear tx/rx error status
 *
 * @param   ins : Base address of the current SPI
 *
 */
void LPSPI_DisableErrorInterrupts(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *base = g_lpspiBasePointer[ins];

    LPSPI_HW_ConfigIntMode(base, LPSPI_RX_ERROR_FLAG, false);
    LPSPI_HW_ConfigIntMode(base, LPSPI_TX_ERROR_FLAG, false);
    LPSPI_HW_ClearStatusFlag(base, LPSPI_RX_ERROR_FLAG);
    LPSPI_HW_ClearStatusFlag(base, LPSPI_TX_ERROR_FLAG);
}


/*!
 * @brief   initializes the user-provided structure
 *
 * @details Initializes the structured interrupt based LPSPI transmission configuration
 *           provided by the user
 *
 * @param   spiCfg : The pointer to LPSPI_MASTER_CFG_T.
 *
 */
void LPSPI_MasterDefaultConfig(LPSPI_MASTER_CFG_T * spiCfg)
{
    spiCfg->txDMAChan        = 0;
    spiCfg->callback         = NULL;
    spiCfg->callbackParam    = NULL;
    spiCfg->baudBitsPer      = 50000U;
    spiCfg->selectPcs        = LPSPI_PER_PCS0;
    spiCfg->selectPcsPolar   = LPSPI_SIGNAL_ACTIVE_LOW;
    spiCfg->pcsKeeps         = false;
    spiCfg->transferType     = LPSPI_TRANSFER_INTERRUPTS;
    spiCfg->rxDMAChan        = 0;
    spiCfg->bitNumber        = 8U;
    spiCfg->lpspiModSrcClk   = 8000000U;
    spiCfg->selectClkPhase   = LPSPI_CLOCK_PHASE_CFG_1ST;
    spiCfg->clkPolar         = LPSPI_CLK_SIGNAL_HIGH;
    spiCfg->sendLsbFirst     = false;
}

/*!
 * @brief   Initializes the LPSPI instance for interrupt driven master mode operation.
 *
 * @details This function uses interrupt driven methods to transmit data,In this function,
 *          the term "spiCfg" is used to indicate the SPI device with which the LPSPI
 *          master is communicating.configures the clock to the LPSPI module,resets the LPSPI
 *          module,configures the IRQ state structure,enables module level interrupts to the
 *          core,and enables the LPSPI module
 *
 * @param   ins          : Base address of the current SPI
 * @param   spiState     : The pointer to LPSPI_STATE_T.
 * @param   spiCfg : The pointer to LPSPI_MASTER_CFG_T.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterInit(
    uint32_t ins,
    LPSPI_STATE_T * spiState,
    const LPSPI_MASTER_CFG_T * spiCfg)
{
    STATUS_T status = STATUS_SUCCESS;

    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];

    /* Save the runtime structure pointer so that
       the IRQ handler can point to the correct state structure */
    g_lpspiStatusPointer[ins] = spiState;
    /* Reset the LPSPI registers */
    LPSPI_HW_Init(lpspiBase);
    /* Set master mode */
    LPSPI_HW_ConfigMasterSlaveMode(lpspiBase,
                                   LPSPI_MASTER_MODE);
    /* Set Pin configuration : SDO=out and SDI=in */
    LPSPI_HW_ConfigPinConfigMode(lpspiBase,
                                 LPSPI_SDI_IN_SDO_OUT,
                                 LPSPI_DATA_OUTPUT_RETAINED,
                                 true);
    /* Calculate the FIFO size for the LPSPI */
    LPSPI_HW_GetFifoSizes(lpspiBase,
                          &(spiState->fifoRxTxSize));

    status = LPSPI_MasterConfigureBus(ins,
                                      spiCfg,
                                      NULL);
    if (status == STATUS_SUCCESS)
    {
        /* When TX is null,the value sent on the bus will be 0 */
        spiState->dummy = 0;
        /* Initialize semaphore */
        status = OSIF_SemCreate(&(spiState->lpspiSem), 0);
        INT_SYS_EnableIRQ(g_lpspiIrqIdNumber[ins]);
        LPSPI_HW_Enable(lpspiBase);
    }
    return status;
}


/*!
 * @brief   Close the LPSPI instance
 *
 * @details This function resets the LPSPI peripheral,sets its clock,and disables interrupts to
 *          the core,it first checks whether the transmission is in progress,and if so,returns
 *          error statistics information
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterDeInit(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);
    /* Instantiate local variable of type
        LPSPI_STATE_T and point to the global state */
    const LPSPI_STATE_T * spiDirState = g_lpspiStatusPointer[ins];
    LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];

    /* Reset the LPSPI register to the default state,
        inlcuding disabling LPSPI */
    LPSPI_HW_Init(lpspiBase);
    INT_SYS_DisableIRQ(g_lpspiIrqIdNumber[ins]);
    g_lpspiStatusPointer[ins] = NULL;

    return OSIF_SemDestroy(&(spiDirState->lpspiSem));
}


/*!
 * @brief   Configures the LPSPI master mode bus timing delay options
 *
 * @details This function is used to configure the timing delay of host mode to "fine-tune"
 *          certain signal timing and match the timing needs of slower peripherals,
 *          the timing is adjusted according to the cycles of the baud rate clock.
 *          The bus timing delays that can be adjusted are listed below:
 *          SCK to PCS Delay: The SCK at the last edge between adjustable delay options to the PCS
 *                            signal for unasserting
 *          PCS to SCK Delay: Assertion between adjustable delay options PCS signal to first SCK edge
 *          Delay between Transfers: Adjustable delay option between PCS signal deassertion
 *
 * @param   ins                  : Base address of the current SPI.
 * @param   delayBetweenTransfers : Latency between data transfers.
 * @param   delaySCKtoPCS        : Latency from SCK to PCS.
 * @param   delayPCStoSCK        : Latency from PCS to SCK.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterConfigDelay(
    uint32_t ins,
    uint32_t delayBetweenTransfers,
    uint32_t delaySCKtoPCS,
    uint32_t delayPCStoSCK)
{
    /* Instantiate a local variable of type LPSPI_T and point to the global state */
    STATUS_T status = STATUS_SUCCESS;

    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];

    /* Disable module */
    status = LPSPI_HW_Disable(lpspiBase);
    if (status == STATUS_SUCCESS)
    {
        LPSPI_HW_ConfigDelay(lpspiBase,
                             LPSPI_SCK_TO_PCS_DELAY,
                             delaySCKtoPCS);
        LPSPI_HW_ConfigDelay(lpspiBase,
                             LPSPI_PCS_TO_SCK_DELAY,
                             delayPCStoSCK);
        LPSPI_HW_ConfigDelay(lpspiBase,
                             LPSPI_BETWEEN_TRANSFER,
                             delayBetweenTransfers);
        LPSPI_HW_Enable(lpspiBase);
    }
    return status;
}

/*!
 * @brief   When the LPSPI instance is configured for interrupt operation,configure the LPSPI port
 *          physical parameters to access devices on the bus
 *
 * @details In this function, the term "spiCfg" is used to denote the SPI device for which the
 *          LPSPI master is communicating. This is an optional function because the spiCfg parameter
 *          is typically configured in an initialization function or transfer function, where these
 *          different functions call the configuration bus function.
 *          The user can pass a different spiCfg structure to a transfer function that contains
 *          parameters for the SPI bus to allow communication with different SPI devices. However,
 *          users can also choose to call this function directly, especially to get the calculated
 *          baud rate,in which case they can pass NULL in the transfer function for the spiCfg
 *          structure
 *
 * @param   ins          : Base address of the current SPI.
 * @param   spiCfg : The pointer to LPSPI_MASTER_CFG_T.
 * @param   calBaudRate  : Calculate baud rate.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterConfigureBus(uint32_t ins,
                                  const LPSPI_MASTER_CFG_T * spiCfg,
                                  uint32_t * calBaudRate)
{
    /* Instantiate a local variable of type LPSPI_STATE_T and point to the global state */
    LPSPI_T *base = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiDirState = g_lpspiStatusPointer[ins];
    uint32_t lpspiBaudRate;

    /* The Transmit Command Register (TCR) */
    uint32_t tcrPreVal;

    /* Disable LPSPI mode */
    if (LPSPI_HW_Disable(base) == STATUS_SUCCESS)
    {
        /* Check the bit count to ensure it falls within the boundary conditions */
        if ((spiCfg->bitNumber >= 8U) && (spiCfg->bitNumber <= 4096U))
        {
            /* Configure the internal state structure */
            spiDirState->lpspiModSrcClk = spiCfg->lpspiModSrcClk;
            spiDirState->pcsKeeps = spiCfg->pcsKeeps;
            spiDirState->perFrameNumOfBits = spiCfg->bitNumber;
            spiDirState->lsbTrue = spiCfg->sendLsbFirst;
            spiDirState->isBlocking = false;
            /* Save transfer type DMA/Interrupt */
            spiDirState->transferType = spiCfg->transferType;
            /* Update transmission status */
            spiDirState->inTransfer = false;
            /* Calculate the bytes/frame for spiDirState->perFrameNumOfBytes. */
            spiDirState->perFrameNumOfBytes =
                (uint16_t)((spiDirState->perFrameNumOfBits + 7U) / 8U);

            /* Due to DMA limitations, 3 bytes/frame frames
                will be processed internally as 4 bytes/frame*/
            if (spiDirState->perFrameNumOfBytes == 3U)
            {
                spiDirState->perFrameNumOfBytes = 4U;
            }
            /* Due to some limitations, all frames bigger
                than 4 bytes/frame must be composed only from 4 bytes chunks. */
            if (spiDirState->perFrameNumOfBytes > 4U)
            {
                spiDirState->perFrameNumOfBytes =
                    (((spiDirState->perFrameNumOfBytes - 1U) / 4U) + 1U) * 4U;
            }

            /* Store DMA channel number used in transfer */
            spiDirState->txDMAChan = spiCfg->txDMAChan;
            spiDirState->rxDMAChan = spiCfg->rxDMAChan;

            /* Store callback */
            spiDirState->callbackParam = spiCfg->callbackParam;
            spiDirState->callback = spiCfg->callback;

            /* Configure the desired PCS polarity */
            LPSPI_HW_ConfigPcsPolarityMode(base,
                                           spiCfg->selectPcs,
                                           spiCfg->selectPcsPolar);
            /* Set up the baud rate */
            lpspiBaudRate = LPSPI_HW_ConfigBaudRate(base,
                                               spiCfg->baudBitsPer,
                                               spiCfg->lpspiModSrcClk,
                                               &tcrPreVal);
            /* Enable sampling point delay */
            LPSPI_HW_ConfigSamplingPoint(base, true);
            /* Now, re-enable the LPSPI module */
            LPSPI_HW_Enable(base);
            /* If the baud rate return value is "0", it indicates an error */
            if (lpspiBaudRate == (uint32_t)0)
            {
                return STATUS_ERROR;
            }
            /* If the user wants to know the calculated baud rate, it is returned */
            if (POINTER_IS_NULL(calBaudRate) == false)
            {
                *calBaudRate = lpspiBaudRate;
            }
            /* Write TCR for this transmission */
            LPSPI_TX_CMD_CFG_T txCmdCfg =
            {
                .contentCmd = false,
                .contentTransfer = spiCfg->pcsKeeps,
                .byteInter = false,
                .sendLsbFirst = spiCfg->sendLsbFirst,
                .frameNum = spiDirState->perFrameNumOfBits,
                .width = LPSPI_ONE_BIT_TRANSFER,
                .txDataMask = false,
                .rxDataMask = false,
                .selectPcs = spiCfg->selectPcs,
                .preDiv = tcrPreVal,
                .selectClkPhase = spiCfg->selectClkPhase,
                .clkPolar = spiCfg->clkPolar
            };
            LPSPI_HW_ConfigTxCommandReg(base, &txCmdCfg);
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_ERROR;
        }
    }
    else
    {
        return STATUS_ERROR;
    }
}

/*!
 * @brief   Perform interrupt-driven blocking SPI main mode transmission.
 *
 * @details This function sends and receives data on both the SPI bus,
 *          which is naturally a full-duplex bus. The function does not
 *          return until the transfer is complete. This feature allows the user
 *          to selectively pass in an SPI configuration structure that allows the
 *          user to change SPI bus properties while initiating an SPI transfer.
 *          The difference between passing in the SPI configuration structure here
 *          and the configuration bus function is that the configuration bus function
 *          returns the calculated baud rate, while this function does not. The user can
 *          also call the configuration bus function before the transfer, in which case
 *          the user only needs to pass NULL to the device structure parameters of
 *          the transfer function.
 *
 * @param   ins               : Base address of the current SPI.
 * @param   txDataBuffer      : User-sent data cache.
 * @param   rxDataBuffer      : User-received data cache.
 * @param   transByteNumber : Transfer byte count.
 * @param   timeout           : Set timeout.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterTransferBlocking(
    uint32_t ins,
    const uint8_t * txDataBuffer,
    uint8_t * rxDataBuffer,
    uint16_t transByteNumber,
    uint32_t timeout)
{
    /* Instantiate local variable of type LPSPI_STATE_T and point to global state */
    LPSPI_T *lpspiBase = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];
    STATUS_T status = STATUS_SUCCESS;
    STATUS_T osifStatus;
    /* If the transfer count is zero, immediately return */
    if (transByteNumber != (uint16_t)0)
    {
        /* Check if another transfer is in progress */
        if (!LPSPI_HW_GetStatusFlag(lpspiBase,
                                    LPSPI_DATA_MODULE_BUSY))
        {
            /* Dummy waits to ensure that the semaphore is 0 and
                there is no need to check the result */
            OSIF_SemWait(&(spiState->lpspiSem), 0);
            spiState->isBlocking = true;

            status = LPSPI_MasterStartTransfer(ins,
                                               txDataBuffer,
                                               rxDataBuffer,
                                               transByteNumber);
            /* Start the transfer process and, if a status code is returned,
                return it to the user */
            if (status == STATUS_SUCCESS)
            {
                /* wait until the transfer is complete */
                osifStatus = OSIF_SemWait(&(spiState->lpspiSem),
                                          timeout);


                if (osifStatus != STATUS_TIMEOUT)
                {
                    LPSPI_DisableErrorInterrupts(ins);
                    LPSPI_HW_ConfigIntMode(lpspiBase,
                                           LPSPI_TRANSFER_COMPLETE_FLAG,
                                           false);
                    LPSPI_HW_ClearStatusFlag(lpspiBase,
                                             LPSPI_TRANSFER_COMPLETE_FLAG);
                }
                else
                {
                    /* If a timeout occurs, stop the transfer,
                       Set the isBlocking variable to false to
                       avoid false semaphore publishing */
                    spiState->isBlocking = false;
                    /* Complete transfer. */
                    LPSPI_MasterCompleteTransfer(ins);
                    return STATUS_TIMEOUT;
                }
            }
            else
            {
                /* Disable interrupt requests */
                LPSPI_HW_ConfigIntMode(lpspiBase,
                                       LPSPI_RX_DATA_FLAG,
                                       false);
                LPSPI_HW_ConfigIntMode(lpspiBase,
                                       LPSPI_TX_DATA_FLAG,
                                       false);

                LPSPI_DisableErrorInterrupts(ins);
                LPSPI_HW_ConfigIntMode(lpspiBase,
                                       LPSPI_TRANSFER_COMPLETE_FLAG,
                                       false);
                LPSPI_HW_ClearStatusFlag(lpspiBase,
                                         LPSPI_TRANSFER_COMPLETE_FLAG);

                spiState->isBlocking = false;
                return status;
            }
        }
        else
        {
            return  STATUS_BUSY;
        }
    }
    return status;
}


/*!
 * @brief   Perform interrupt-driven non-blocking SPI main mode transfer.
 *
 * @details This function sends and receives data on both the SPI bus,
 *          which is naturally a full-duplex bus. The function returns immediately
 *          after initiating the transfer. The user needs to check if the transfer
 *          is complete using the LPSPI_MasterGetTransferStatus function.
 *          This feature allows the user to selectively pass in an SPI configuration
 *          structure that allows the user to change the SPI bus properties while initiating
 *          an SPI transfer. The difference between passing in the SPI configuration
 *          structure here and the configuration bus function is that the configuration bus
 *          function returns the calculated baud rate, while this function does not.
 *          The user can also call the configuration bus function before the transfer,
 *          in which case the user only needs to pass NULL to the device structure
 *          parameters of the transfer function.
 *
 * @param   ins               : Base address of the current SPI.
 * @param   txDataBuffer      : User-sent data cache.
 * @param   rxDataBuffer      : User-received data cache.
 * @param   transByteNumber : Transfer byte count.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterTransfer(
    uint32_t ins,
    const uint8_t * txDataBuffer,
    uint8_t * rxDataBuffer,
    uint16_t transByteNumber)
{
    STATUS_T status = STATUS_SUCCESS;

    LPSPI_INSTANCE_VALID(ins);

    if (transByteNumber != (uint16_t)0)
    {
        /* Start the transfer process,
            and if it returns a status code, return it to the user */
        status = LPSPI_MasterStartTransfer(ins,
                                           txDataBuffer,
                                           rxDataBuffer,
                                           transByteNumber);
        if (status != STATUS_SUCCESS)
        {
            return status;
        }
    }
    return STATUS_SUCCESS;
}


/*!
 * @brief   returns whether the last interrupt-driven transfer was complete.
 *
 * @details When performing an a-sync (non-blocking) transfer, the user can call
 *          this function to determine the status of the current transfer:
 *          in progress (or busy) or completed (successful).
 *          In addition, if the transmission is still in progress,
 *          the user can get the number of words that should be received.
 *
 * @param   ins             : Base address of the current SPI.
 * @param   remainingBytes   : Remaining bytes.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterGetTransferStatus(
    uint32_t ins,
    uint32_t * remainingBytes)
{
    LPSPI_INSTANCE_VALID(ins);

    /* Instantiate local variable of type LPSPI_STATE_T and point to global state */
    const LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];
    /* Fill in the number of bytes transferred */
    if (POINTER_IS_NULL(remainingBytes) == false)
    {
        *remainingBytes = spiState->rxNumOfBytes;
    }
    if (spiState->lpspiStatus != LPSPI_TRANSMISSION_OK)
    {
        return STATUS_ERROR;
    }
    else
    {
        return (STATUS_T)(spiState->inTransfer ? STATUS_BUSY : STATUS_SUCCESS);
    }
}

/*!
 * @brief   Early termination of interrupt-driven asynchronous transmission
 *
 * @details During a-sync (non blocking) transmission, if the transmission is still in progress,
 *          the use can choose to terminate the transmission early
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterAbortTransfer(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    /* Stop a running transfer */
    LPSPI_MasterCompleteTransfer(ins);
    LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
    /* The second clear command is used to
        avoid situations where a word is still in the shifter */
    LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Select the chip to communicate with.
 *
 * @details The main purpose of this function is to set the PCS and the proper polarity.
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterConfigPcs(
    uint32_t ins,
    LPSPI_PER_PCS_T selectPcs,
    LPSPI_SIGNAL_ACTIVE_T pcsPolar)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    STATUS_T status = STATUS_ERROR;

    if (LPSPI_HW_Disable(baseSpi) == STATUS_SUCCESS)
    {
        status = LPSPI_HW_ConfigPcsPolarityMode(baseSpi,
                                                selectPcs,
                                                pcsPolar);
        if (status == STATUS_SUCCESS)
        {
            LPSPI_HW_Enable(baseSpi);
            LPSPI_HW_ConfigPcs(baseSpi, selectPcs);
        }
        else
        {
            status = STATUS_ERROR;
        }
    }
    return status;
}

/*!
 * @brief   Configure non blocking transmission
 *
 * @details The number of transByteNumber must be divided by the number of bytes/frame.
 *          txDataBuffer cannot be NULL, but rxDataBuffer can be NULL.
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_MasterStartTransfer(uint32_t ins,
                                   const uint8_t * txDataBuffer,
                                   uint8_t * rxDataBuffer,
                                   uint16_t transByteNumber)
{
    LPSPI_INSTANCE_VALID(ins);
    DMA_TRANSFER_SIZE_T dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];
    int i = 0;

    /* Make sure we're not busy. */
    for (i = 0; i < 2; i++)
    {
        if (LPSPI_HW_GetStatusFlag(baseSpi, LPSPI_DATA_MODULE_BUSY))
        {
            return STATUS_BUSY;
        }
    }

    /* Verify if the number of bytes is divided by the number of bytes/frame */
    if ((transByteNumber % spiState->perFrameNumOfBytes) == (uint16_t)0)
    {
        /* Clean the RX and TX buffers */
        LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
        LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);

        if(spiState->pcsKeeps == true)
        {
            LPSPI_HW_ConfigContCBit(baseSpi);
        }
        spiState->lpspiStatus = LPSPI_TRANSMISSION_OK;
        /* Clear all interrupts sources */
        LPSPI_HW_ClearStatusFlag(baseSpi, LPSPI_ALL_STATUS);
        /* Enable fault interrupts sources */
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TX_ERROR_FLAG,
                               true);
        if (POINTER_IS_NULL(rxDataBuffer) == false)
        {
            LPSPI_HW_ConfigIntMode(baseSpi,
                                   LPSPI_RX_ERROR_FLAG,
                                   true);
        }

        /* Configure rxNumOfBytes based on transmission type */
        if (POINTER_IS_NULL(rxDataBuffer) != true)
        {
            spiState->rxNumOfBytes = transByteNumber;
            LPSPI_HW_ClearRxmaskBit(baseSpi);
        }
        else
        {
            spiState->rxNumOfBytes = 0;
            LPSPI_HW_ConfigRxmaskBit(baseSpi);
        }

        /* Configure watermarks */
        LPSPI_HW_ConfigTxWatermarks(baseSpi, 2U);
        LPSPI_HW_ConfigRxWatermarks(baseSpi, 0U);

        if (spiState->transferType != LPSPI_TRANSFER_INTERRUPTS)
        {
            /* When LPSPI use DMA frames with 3 bytes size are not accepted. */
            if(spiState->perFrameNumOfBytes == 2U)
            {
                dmaTransferSize = DMA_TRANSFER_SIZE_2B;
            }
            else if(spiState->perFrameNumOfBytes == 1U)
            {
                dmaTransferSize = DMA_TRANSFER_SIZE_1B;
            }
            else
            {
                dmaTransferSize = DMA_TRANSFER_SIZE_4B;
            }

            /* Configure TX DMA channel */
            if (POINTER_IS_NULL(txDataBuffer) != false)
            {
                DMA_ConfigMultiBlockTransfer(
                    spiState->txDMAChan, DMA_TRANSFER_PERIPH2PERIPH,
                    (uint32_t)(&(spiState->dummy)),
                    (uint32_t)(&(baseSpi->TXDATA.reg)),
                    dmaTransferSize,
                    (uint32_t)1U<<(uint8_t)(dmaTransferSize),
                    (uint32_t)transByteNumber/(uint32_t)((uint32_t)1U <<(uint8_t)(dmaTransferSize)),
                    true);
            }
            else
            {
                DMA_ConfigMultiBlockTransfer(
                    spiState->txDMAChan,
                    DMA_TRANSFER_MEM2PERIPH,
                    (uint32_t)txDataBuffer,
                    (uint32_t)(&(baseSpi->TXDATA.reg)),
                    dmaTransferSize,
                    (uint32_t)1U<<(uint8_t)(dmaTransferSize),
                    (uint32_t)transByteNumber/(uint32_t)((uint32_t)1U <<(uint8_t)(dmaTransferSize)),
                    true);
            }
            /* If used in the current transport,configure the RX DMA channel */
            if(POINTER_IS_NULL(rxDataBuffer) == false)
            {
                DMA_ConfigMultiBlockTransfer(
                    spiState->rxDMAChan,
                    DMA_TRANSFER_PERIPH2MEM,
                    (uint32_t)(&(baseSpi->RXDATA.reg)),
                    (uint32_t)rxDataBuffer,
                    dmaTransferSize,
                    (uint32_t)1U<<(uint8_t)(dmaTransferSize),
                    (uint32_t)transByteNumber/(uint32_t)((uint32_t)1U <<(uint8_t)(dmaTransferSize)),
                    true);
                DMA_RegisterCallback(spiState->rxDMAChan,
                                     (LPSPI_MasterCompleteRX),
                                     (void*)(ins));
                /* Start RX channel */
                DMA_StartChannel(spiState->rxDMAChan);
            }

            /* If the RX buffer is empty, the transmission is completed when all bytes are sent */
            DMA_RegisterCallback(spiState->txDMAChan,
                                 (LPSPI_MasterCompleteDMATransfer),
                                 (void*)(ins));

            /* Start TX channel */
            DMA_StartChannel(spiState->txDMAChan);
            /* Update transmission status */
            spiState->inTransfer = true;
            /* Enable LPSPI DMA request */
            if (POINTER_IS_NULL(rxDataBuffer) == false)
            {
                LPSPI_HW_ConfigRxDmaCmd(baseSpi, true);
            }
            LPSPI_HW_ConfigTxDmaCmd(baseSpi, true);
        }
        else
        {

            /* Fill in the other members of the runtime state structure */
            spiState->rxBuff = (uint8_t *)rxDataBuffer;
            spiState->txFrameCount = 0;
            spiState->txBuff = (const uint8_t *)txDataBuffer;
            spiState->txNumOfBytes = transByteNumber;
            spiState->rxFrameCount = 0;

            /*For continuous mode, an additional word must be written to negate PCS */
            if (spiState->pcsKeeps == true)
            {
                spiState->txNumOfBytes++;
            }

            /* Update transfer status */
            spiState->inTransfer = true;
            /* If the RX buffer is not NULL,enable RDF interrupt */
            if (POINTER_IS_NULL(spiState->rxBuff) == false)
            {
                LPSPI_HW_ConfigIntMode(baseSpi,
                                       LPSPI_RX_DATA_FLAG,
                                       true);
            }
            /* Enable the TDF and RDF interrupt */
            LPSPI_HW_ConfigIntMode(baseSpi,
                                   LPSPI_TX_DATA_FLAG,
                                   true);
        }
    }
    else
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief   Complete conversion
 *
 * @details Clean up after the transmission is completed.Enable interrupt,enable LPSPI module
 *
 * @param   ins : Base address of the current SPI.
 *
 */
void LPSPI_MasterCompleteTransfer(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];
    spiState->inTransfer = false;

    if(spiState->transferType != LPSPI_TRANSFER_DMA)
    {
        /* Disable interrupt requests */
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TX_DATA_FLAG,
                               false);
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_RX_DATA_FLAG,
                               false);
    }
    else
    {
        /* Disable LPSPI DMA request */
        LPSPI_HW_ConfigTxDmaCmd(baseSpi, false);
        LPSPI_HW_ConfigRxDmaCmd(baseSpi, false);
    }

    LPSPI_DisableErrorInterrupts(ins);
    LPSPI_HW_ConfigIntMode(baseSpi,
                           LPSPI_TRANSFER_COMPLETE_FLAG,
                           false);
    LPSPI_HW_ClearStatusFlag(baseSpi,
                             LPSPI_TRANSFER_COMPLETE_FLAG);

    if (spiState->isBlocking)
    {
        OSIF_SemPost(&(spiState->lpspiSem));
        spiState->isBlocking = false;
    }

    if (!POINTER_IS_NULL(spiState->callback))
    {
        spiState->callback(spiState,
                           SPI_EVENT_TRANSFER_COMPLETE,
                           spiState->callbackParam);
    }

}

/*!
 * @brief   Complete a DMA transfer
 *
 * @details The main purpose of this function is to create a function that is compatible with
 *          the DMA callback type
 *
 * @param   para   : Base address for current SPI instance.
 * @param   status : DMA channel status.
 *
 */
void LPSPI_MasterCompleteDMATransfer(void* para, DMA_CHANNEL_STATUS_T status)
{

    uint32_t ins = (uint32_t)para;

    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiDirState = g_lpspiStatusPointer[ins];

    if (status != DMA_CHANNEL_ERROR)
    {
        if (spiDirState->pcsKeeps == true)
        {
            LPSPI_HW_ClearContCBit(baseSpi);
        }

        /* Enable transmission completion flag interrupt to capture the end of transmission */
        spiDirState->rxNumOfBytes = 0;
        spiDirState->txNumOfBytes = 0;
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TRANSFER_COMPLETE_FLAG,
                               true);
    }
    else
    {
        LPSPI_MasterAbortTransfer(ins);
        spiDirState->lpspiStatus = LPSPI_TX_FAIL;
    }
}

/*!
 * @brief   Check whether an error is detected on the RX channel
 *
 * @details The main purpose of this function is to check for DMA errors on the RX channel
 *
 * @param   para   : Base address for current SPI instance.
 * @param   status : DMA channel status.
 *
 */
void LPSPI_MasterCompleteRX(void* para, DMA_CHANNEL_STATUS_T status)
{
    uint32_t ins = (uint32_t)para;

    LPSPI_INSTANCE_VALID(ins);

    LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];

    if (status == DMA_CHANNEL_ERROR)
    {
        LPSPI_MasterAbortTransfer(ins);
        spiState->lpspiStatus = LPSPI_TX_FAIL;
    }
}

/*!
 * @brief   LPSPI master mode interrupt handler
 *
 * @details This handler uses a buffer stored in the LPSPI_STATE_T structure to transfer data.
 *
 * @param   ins : Base address of the current SPI.
 *
 */
void LPSPI_MasterIRQHandler(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    /* Instantiate local variable of type LPSPI_STATE_T and point to global state */
    LPSPI_STATE_T * spiState = g_lpspiStatusPointer[ins];
    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];

    /* If an error is detected, the transfer is aborted */
    if (LPSPI_HW_GetStatusFlag(baseSpi,
                               LPSPI_TX_ERROR_FLAG) &&
        (POINTER_IS_NULL(spiState->txBuff) == false))
    {
        LPSPI_MasterAbortTransfer(ins);
        LPSPI_HW_ClearStatusFlag(baseSpi,
                                 LPSPI_TX_ERROR_FLAG);
        spiState->lpspiStatus = LPSPI_TX_FAIL;
        return;
    }
    if (LPSPI_HW_GetStatusFlag(baseSpi,
                               LPSPI_RX_ERROR_FLAG) &&
        (POINTER_IS_NULL(spiState->rxBuff) == false))
    {
        LPSPI_MasterAbortTransfer(ins);

        LPSPI_HW_ClearStatusFlag(baseSpi, LPSPI_RX_ERROR_FLAG);
        spiState->lpspiStatus = LPSPI_RX_FAIL;
        return;
    }

    /* Receive IRQ handler: Check read buffer
        only when there are remaining bytes to read */
    if (LPSPI_HW_GetStatusFlag(baseSpi,LPSPI_RX_DATA_FLAG))
    {
        if (spiState->rxNumOfBytes == (uint16_t)0)
        {
            /* When has a noise, the Rx FIFO count was corrupted the tranfer will be
               abort to avoid the RX check interrupt occurs continuously */
            LPSPI_MasterAbortTransfer(ins);
            spiState->lpspiStatus = LPSPI_RX_FAIL;
        }
        else
        {
            LPSPI_ReadRxBuffer(ins);
        }
    }

    /* Transmit data */
    if (LPSPI_HW_GetStatusFlag(baseSpi,LPSPI_TX_DATA_FLAG) &&
        (spiState->txNumOfBytes != (uint16_t)0))
    {
        LPSPI_FillupTxBuffer(ins);
    }

    if (spiState->txNumOfBytes == (uint16_t)0)
    {
        /* Disable TX flag. The software buffer is empty */
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TRANSFER_COMPLETE_FLAG,
                               true);
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TX_DATA_FLAG,
                               false);

        /* Check that we have completed this transfer */
        if ((spiState->rxNumOfBytes == (uint16_t)0) &&
            LPSPI_HW_GetStatusFlag(baseSpi, LPSPI_TRANSFER_COMPLETE_FLAG))
        {
            LPSPI_MasterCompleteTransfer(ins);
        }
    }
}

/*!
 * @brief   Initializes user provided structured data using
 *          interrupt based LPSPI transmission configuration
 *
 * @param   ins : Base address of the current SPI.
 *
 */
void LPSPI_SlaveDefaultConfig(LPSPI_SLAVE_CFG_T * spiCfg)
{
    spiCfg->transferType     = LPSPI_TRANSFER_INTERRUPTS;
    spiCfg->rxDMAChan        = 0;
    spiCfg->txDMAChan        = 0;
    spiCfg->callback         = NULL;
    spiCfg->callbackParam    = NULL;
    spiCfg->selectPcs        = LPSPI_PER_PCS0;
    spiCfg->selectPcsPolar   = LPSPI_SIGNAL_ACTIVE_LOW;
    spiCfg->bitNumber        = 8U;
    spiCfg->selectClkPhase   = LPSPI_CLOCK_PHASE_CFG_1ST;
    spiCfg->clkPolar         = LPSPI_CLK_SIGNAL_HIGH;
    spiCfg->sendLsbFirst     = false;
}


/*!
 * @brief   Initializes SPI slave device.
 *
 * @param   ins            : Base address of the current SPI.
 * @param   spiState       : The pointer to LPSPI_STATE_T.
 * @param   slaveConfigure : The pointer to LPSPI_SLAVE_CFG_T.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveInit(
    uint32_t ins,
    LPSPI_STATE_T * spiState,
    const LPSPI_SLAVE_CFG_T * slaveConfigure)
{
    LPSPI_INSTANCE_VALID(ins);

    STATUS_T status = STATUS_SUCCESS;
    LPSPI_T * baseSpi = g_lpspiBasePointer[ins];

    spiState->lsbTrue = slaveConfigure->sendLsbFirst;
    spiState->transferType = slaveConfigure->transferType;
    spiState->isBlocking = false;
    spiState->perFrameNumOfBits = slaveConfigure->bitNumber;
    spiState->txDMAChan = slaveConfigure->txDMAChan;
    spiState->rxDMAChan = slaveConfigure->rxDMAChan;
    spiState->callbackParam = slaveConfigure->callbackParam;
    spiState->callback = slaveConfigure->callback;
    spiState->perFrameNumOfBytes = LPSPI_BitsToBytes(spiState->perFrameNumOfBits);
    spiState->inTransfer = false;

    status = OSIF_SemCreate(&(spiState->lpspiSem), 0);
    g_lpspiStatusPointer[ins] = spiState;
    LPSPI_HW_Init(baseSpi);

    /* Configure LPSPI to slave mode */
    LPSPI_HW_ConfigMasterSlaveMode(baseSpi, LPSPI_SLAVE_MODE);
    /* Example set the terminal number */
    LPSPI_HW_ConfigPinConfigMode(baseSpi,
                                 LPSPI_SDI_IN_SDO_OUT,
                                 LPSPI_DATA_OUTPUT_RETAINED,
                                 true);
    /* Calculate the FIFO size of LPSPI */
    LPSPI_HW_GetFifoSizes(baseSpi, &(spiState->fifoRxTxSize));

    /* Set polarity */
    LPSPI_HW_ConfigPcsPolarityMode(baseSpi,
                                   slaveConfigure->selectPcs,
                                   slaveConfigure->selectPcsPolar);

     /* Write a TCR for this transfer */
    LPSPI_TX_CMD_CFG_T txCmdCfg = {
        .frameNum = spiState->perFrameNumOfBits,
        .byteInter = false,
        .selectClkPhase = slaveConfigure->selectClkPhase,
        .clkPolar = slaveConfigure->clkPolar,
        .selectPcs = slaveConfigure->selectPcs,
        .width = LPSPI_ONE_BIT_TRANSFER,
        .txDataMask = false,
        .rxDataMask = false,
        .sendLsbFirst = slaveConfigure->sendLsbFirst,
    };

    /* Write TX CMD register */
    LPSPI_HW_ConfigTxCommandReg(baseSpi, &txCmdCfg);
    LPSPI_HW_Enable(baseSpi);
    /* Enable interrupt request */
    INT_SYS_EnableIRQ(g_lpspiIrqIdNumber[ins]);

    return status;
}


/*!
 * @brief   DeInitializes SPI slave device.
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveDeInit(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);
    LPSPI_T *baseSpi = g_lpspiBasePointer[ins];
    STATUS_T status = STATUS_SUCCESS;
    /* Instantiate a local variable of type
        lpspi_master_state_t and point to the global state */
    const LPSPI_STATE_T * spiState =
        (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    /* Destroy semaphore */
    status = OSIF_SemDestroy(&(spiState->lpspiSem));
    /* Reset the LPSPI registers to its default state, including disabling LPSPI */
    LPSPI_HW_Init(baseSpi);

    /* Disable interrupt */
    INT_SYS_DisableIRQ(g_lpspiIrqIdNumber[ins]);

    /* Clear status pointer */
    g_lpspiStatusPointer[ins] = NULL;

    return status;
}

/*!
 * @brief   Execute interrupt driven to blocking SPI master mode transfer.
 *
 * @details The function sends and receives data on the SPI bus at the same time,
 *          and does not return before the transmission is complete. The function
 *          can change the SPI properties through the SPI configuration structure
 *          provided by the user. When the user calls the configure bus function
 *          before transmission, it only needs to pass NULL parameters.
 *
 * @param   ins                 : Base address of the current SPI.
 * @param   txDataBuffer        : User sent data cache.
 * @param   rxDataBuffer        : User received data cache.
 * @param   transByteNumber   : Transfer byte count.
 * @param   timeout             : Set timeout.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveTransferBlocking(
    uint32_t ins,
    const uint8_t *txDataBuffer,
    uint8_t *rxDataBuffer,
    uint16_t transByteNumber,
    uint32_t timeout)
{
    STATUS_T status;
    STATUS_T osifStatus;

    LPSPI_INSTANCE_VALID(ins);

    const LPSPI_T * baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * states = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    /* Check if another transfer is in progress */
    if (!LPSPI_HW_GetStatusFlag(baseSpi,
                                LPSPI_DATA_MODULE_BUSY))
    {
        /* Pseudo-wait to ensure that the semaphore is 0 without checking the result */
        OSIF_SemWait(&(states->lpspiSem), 0);
        states->isBlocking = true;

        status = LPSPI_SlaveTransfer(ins,
                                     txDataBuffer,
                                     rxDataBuffer,
                                     transByteNumber);
        if(status == STATUS_SUCCESS)
        {
            /* As this is a synchronous transmission,
                please wait until the transmission is complete */
            osifStatus = OSIF_SemWait(&(states->lpspiSem), timeout);

            if (osifStatus != STATUS_TIMEOUT)
            {
                LPSPI_DisableErrorInterrupts(ins);
                return STATUS_SUCCESS;
            }
            else
            {
                /* Set isBlocking variable to false */
                states->isBlocking = false;
                /* Complete transfer */
                LPSPI_SlaveAbortTransfer(ins);
                return STATUS_TIMEOUT;
            }
        }
        else
        {
            states->isBlocking = false;
            LPSPI_DisableErrorInterrupts(ins);
            return status;
        }
    }
    else
    {
        return STATUS_BUSY;
    }
}

/*!
 * @brief   Performs interrupt-driven non-blocking SPI master mode transfer.
 *
 * @details This function sends and receives data on the SPI bus at the same time, the function
 *          returns immediately after the initial transmission, the user can call
 *          LPSPI_MasterGetTransferStatus to check whether the transmission is complete,
 *          this function can change the SPI properties through the configuration structure
 *          provided by the user, the difference between this function and the configure bus
 *          function is that configure bus returns the baud rate, while this function
 *          does not return. The user can also call the configure bus function before the
 *          transfer, just by passing NULL to the argument
 *
 * @param   ins                 : Base address of the current SPI.
 * @param   txDataBuffer        : User sent data cache.
 * @param   rxDataBuffer        : User received data cache.
 * @param   transByteNumber   : Transfer byte count.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveTransfer(
    uint32_t ins,
    const uint8_t *txDataBuffer,
    uint8_t *rxDataBuffer,
    uint16_t transByteNumber)
{
    LPSPI_INSTANCE_VALID(ins);

    const uint8_t * buffer;

    LPSPI_T * baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * states = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];
    states->dummy = 0xFFU;
    DMA_TRANSFER_SIZE_T dmaTransferSize = DMA_TRANSFER_SIZE_1B;

    /* The number of bytes transmitted should be divisible by the frame size */
    if ((uint16_t)(transByteNumber % states->perFrameNumOfBytes) == (uint16_t)0U)
    {
        /* Check that the LPSPI module is not busy */
        if (states->inTransfer != true)
        {
            /* Initialize the current transmission status */
            states->lpspiStatus = LPSPI_TRANSMISSION_OK;
            /* Clean RX and TX buffers */
            LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
            /* The second clear command is used to avoid
                situations where a word is still in the shifter */
            LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
            /* Clear all interrupts sources */
            LPSPI_HW_ClearStatusFlag(baseSpi, LPSPI_ALL_STATUS);
            /* Enable fault interrupts sources */
            LPSPI_HW_ConfigIntMode(baseSpi,LPSPI_RX_ERROR_FLAG , true);
            LPSPI_HW_ConfigIntMode(baseSpi,LPSPI_TX_ERROR_FLAG , true);
            /* Fill in the other members of the runtime state structure */
            states->rxBuff = (uint8_t *)rxDataBuffer;
            states->txBuff = (const uint8_t *)txDataBuffer;
            if (states->transferType != LPSPI_TRANSFER_INTERRUPTS)
            {
                /* Configure watermarks */
                LPSPI_HW_ConfigTxWatermarks(baseSpi, 3U);
                LPSPI_HW_ConfigRxWatermarks(baseSpi, 0U);

                /* Not accepted when LPSPI uses DMA frames with a size if 3 bytes */
                if(states->perFrameNumOfBytes == 2U)
                {
                    dmaTransferSize = DMA_TRANSFER_SIZE_2B;
                }
                else if(states->perFrameNumOfBytes == 1U)
                {
                    dmaTransferSize = DMA_TRANSFER_SIZE_1B;
                }
                else
                {
                    dmaTransferSize = DMA_TRANSFER_SIZE_4B;
                }

                if(POINTER_IS_NULL(rxDataBuffer) != false)
                {
                    states->rxNumOfBytes = 0U;
                    /* if there is no data to receive, use the pseudo-data
                        as the destination of the DMA transmission */
                    buffer = (uint8_t *)(&(states->dummy));
                }
                else
                {
                    states->rxNumOfBytes = transByteNumber;
                    buffer = rxDataBuffer;
                }
                DMA_ConfigMultiBlockTransfer(
                    states->rxDMAChan,
                    DMA_TRANSFER_PERIPH2MEM,
                    (uint32_t)(&(baseSpi->RXDATA.reg)),
                    (uint32_t)buffer, dmaTransferSize,
                    (uint32_t)1U<<(uint8_t)(dmaTransferSize),
                    (uint32_t)transByteNumber/(uint32_t)((uint32_t)1U <<(uint8_t)(dmaTransferSize)),
                    true);
                if(POINTER_IS_NULL(rxDataBuffer) == true)
                {
                    /* if there is no data to receive,
                        don't not increase the target offset */
                    DMA_ConfigDestOffset(states->rxDMAChan, 0);
                }

                if(POINTER_IS_NULL(txDataBuffer) != false)
                {
                    states->txNumOfBytes = 0U;
                    /* if there is no data to receive, use the
                        pseudo-data as the destination of the DMA transmission */
                    buffer = (uint8_t *)(&(states->dummy));
                }
                else
                {
                    buffer = txDataBuffer;
                    states->txNumOfBytes = transByteNumber;
                }
                DMA_ConfigMultiBlockTransfer(
                    states->txDMAChan,
                    DMA_TRANSFER_MEM2PERIPH,
                    (uint32_t)buffer,
                    (uint32_t)(&(baseSpi->TXDATA.reg)),
                    dmaTransferSize,
                    (uint32_t)1U<<(uint8_t)(dmaTransferSize),
                    (uint32_t)transByteNumber/(uint32_t)((uint32_t)1U <<(uint8_t)(dmaTransferSize)),
                    true);
                if (POINTER_IS_NULL(txDataBuffer) == true)
                {
                    /* if there is no data to receive,
                        don't not increase the target offset */
                    DMA_ConfigSrcOffset(states->txDMAChan, 0);
                }

                states->inTransfer = true;

                DMA_RegisterCallback(states->rxDMAChan,
                                     (LPSPI_SlaveCompleteDMATransfer),
                                     (void*)(ins));

                DMA_StartChannel(states->txDMAChan);
                DMA_StartChannel(states->rxDMAChan);

                /* Enable LPSPI DMA requests */
                LPSPI_HW_ConfigTxDmaCmd(baseSpi, true);
                LPSPI_HW_ConfigRxDmaCmd(baseSpi, true);
            }
            else
            {
                if(POINTER_IS_NULL(states->txBuff) == true)
                {
                    states->txNumOfBytes = 0;
                    LPSPI_HW_ConfigTxmskBit(baseSpi);
                }
                else
                {
                    states->txNumOfBytes = transByteNumber;
                    LPSPI_HW_ClearTxmaskBit(baseSpi);
                }

                if(POINTER_IS_NULL(states->rxBuff) == true)
                {
                    states->rxNumOfBytes = 0;
                    LPSPI_HW_ConfigRxmaskBit(baseSpi);
                }
                else
                {
                    states->rxNumOfBytes = transByteNumber;
                    LPSPI_HW_ClearRxmaskBit(baseSpi);
                }

                states->rxFrameCount = 0;
                states->txFrameCount = 0;
                states->pcsKeeps = false;
                /* Configure watermarks */
                LPSPI_HW_ConfigTxWatermarks(baseSpi, 2U);
                LPSPI_HW_ConfigRxWatermarks(baseSpi, 0U);

                states->inTransfer = true;
                /* Enable interrupts for RX/TX */
                if(POINTER_IS_NULL(states->txBuff) == false)
                {
                    LPSPI_HW_ConfigIntMode(baseSpi,LPSPI_TX_DATA_FLAG , true);
                }
                if(POINTER_IS_NULL(states->rxBuff) == false)
                {
                    LPSPI_HW_ConfigIntMode(baseSpi, LPSPI_RX_DATA_FLAG, true);
                }
            }
            return STATUS_SUCCESS;
        }
        else
        {
            return STATUS_BUSY;
        }
    }
    else
    {
        return STATUS_ERROR;
    }
}

/*!
 * @brief   Early termination of interrupt-driven asynchronous transmission
 *
 * @details During the a-sync (non-blocking) transmission, if the transmission is
 *          still in progress,the user can choose to terminate the transmission in advance
 *
 * @param   ins : Base address of the current SPI.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveAbortTransfer(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T * baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * states = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    if (states->transferType != LPSPI_TRANSFER_INTERRUPTS)
    {
        /* Disable LPSPI DMA request */
        LPSPI_HW_ConfigTxDmaCmd(baseSpi, false);
        LPSPI_HW_ConfigRxDmaCmd(baseSpi, false);
    }
    else
    {
        /* Disable interrupts */
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_RX_DATA_FLAG,
                               false);
        LPSPI_HW_ConfigIntMode(baseSpi,
                               LPSPI_TX_DATA_FLAG,
                               false);
    }

    LPSPI_DisableErrorInterrupts(ins);

    states->inTransfer = false;
    /* Clean the RX and TX buffers */
    LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
    /* The second clear command is used to
        avoid situations where a word is still in the shifter */
    LPSPI_HW_ConfigFlushFifoCmd(baseSpi, true, true);
    if(states->isBlocking == true)
    {
        OSIF_SemPost(&(states->lpspiSem));
        states->isBlocking = false;
    }
    return STATUS_SUCCESS;
}


/*!
 * @brief   convert bits to bytes
 *
 * @param   numOfBits : number of bits
 *
 * @retval  number of bytes
 *
 */
uint16_t LPSPI_BitsToBytes(uint16_t numOfBits)
{
    uint16_t numOfBytes;

    numOfBytes = (uint16_t)((numOfBits + 7U) / 8U);

    /* Due to DMA limitations, frames of 3 bytes/frame
        will be internally processed into 4 bytes/frame */
    if (numOfBytes == 3U)
    {
        numOfBytes = 4U;
    }

    /* Due to some limitations all frames bigger than 4
        bytes/frame must be composed only from 4 bytes chunks */
    if (numOfBytes > 4U)
    {
        numOfBytes = (((numOfBytes - 1U) / 4U) + 1U) * 4U;
    }
    return numOfBytes;
}


/*!
 * @brief   Returns whether the transmission of the previous interrupt driver has been completed
 *
 * @details When an a-sync transfer is performed, the user can call this function to determine
 *          the status of the current transfer: in progress or complete. In addition, if the
 *          transmission is still in progress, the user can get the number of words that should
 *          be received
 *
 * @param   ins           : Base address of the current SPI.
 * @param   remainingBytes : Remaining bytes.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_SlaveGetTransferStatus(
    uint32_t ins,
    uint32_t * remainingBytes)
{
    LPSPI_INSTANCE_VALID(ins);

    const LPSPI_STATE_T * spiState = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    /* Fill in the number of bytes transferred */
    if (POINTER_IS_NULL(remainingBytes) == false)
    {
        *remainingBytes = spiState->txNumOfBytes;
    }
    if (spiState->lpspiStatus != LPSPI_TRANSMISSION_OK)
    {
        return STATUS_ERROR;
    }
    else
    {
        return (STATUS_T)(spiState->inTransfer ? STATUS_BUSY : STATUS_SUCCESS);
    }
}

/*!
 * @brief   Complete transfer DMA
 *
 * @details The main purpose of this function is to create a
 *          function that is compatible with DMA callback types
 *
 * @param   para   : Base address for current SPI instance.
 * @param   status : DMA channel status.
 *
 */
void LPSPI_SlaveCompleteDMATransfer(void* para, DMA_CHANNEL_STATUS_T status)
{
    LPSPI_INSTANCE_VALID((uint32_t)para);

    uint32_t ins = (uint32_t)para;
    LPSPI_STATE_T * spiState = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    LPSPI_SlaveAbortTransfer(ins);

    /* Check the RX and TX DMA channels */
    if (DMA_ReadChannelStatus(spiState->rxDMAChan) == DMA_CHANNEL_ERROR)
    {
        spiState->lpspiStatus = LPSPI_RX_FAIL;
    }
    if (DMA_ReadChannelStatus(spiState->txDMAChan) == DMA_CHANNEL_ERROR)
    {
        spiState->lpspiStatus = LPSPI_TX_FAIL;
    }

    if (POINTER_IS_NULL(spiState->callback) == false)
    {
        spiState->callback(spiState,
                           SPI_EVENT_TRANSFER_COMPLETE,
                           spiState->callbackParam);
    }
}


/*!
 * @brief   Interrupt handler in SLAVE mode.
 *
 * @details The handler uses a buffers stored in the LPSPI_STATE_T structure to transfer data.
 *
 * @param   ins : Base address of the current SPI.
 *
 */
void LPSPI_SlaveIRQHandler(uint32_t ins)
{
    LPSPI_INSTANCE_VALID(ins);

    LPSPI_T * baseSpi = g_lpspiBasePointer[ins];
    LPSPI_STATE_T * spiState = (LPSPI_STATE_T *)g_lpspiStatusPointer[ins];

    /* If an error is detected, the transmission will be aborted */
    if (LPSPI_HW_GetStatusFlag(baseSpi, LPSPI_TX_ERROR_FLAG) &&
        (POINTER_IS_NULL(spiState->txBuff) == false))
    {
        LPSPI_SlaveAbortTransfer(ins);
        spiState->lpspiStatus = LPSPI_TX_FAIL;
        return;
    }
    if (LPSPI_HW_GetStatusFlag(baseSpi, LPSPI_RX_ERROR_FLAG) &&
        ( POINTER_IS_NULL(spiState->rxBuff) == false))
    {
        LPSPI_SlaveAbortTransfer(ins);
        spiState->lpspiStatus = LPSPI_RX_FAIL;
        return;
    }

    /* tx data */
    if (LPSPI_HW_GetStatusFlag(baseSpi,LPSPI_TX_DATA_FLAG) &&
        (spiState->txNumOfBytes != (uint8_t)0))
    {
        LPSPI_FillupTxBuffer(ins);
    }
    /* rx data */
    if (LPSPI_HW_GetStatusFlag(baseSpi,LPSPI_RX_DATA_FLAG) &&
        (spiState->rxNumOfBytes != (uint8_t)0))
    {
        LPSPI_ReadRxBuffer(ins);
    }

    /* If all bytes are sent, interrupt TDF is disable */
    if (spiState->txNumOfBytes == (uint8_t)0)
    {
        LPSPI_HW_ConfigIntMode(baseSpi, LPSPI_TX_DATA_FLAG, false);
    }
    /* If all bytes are received, interrupt RDF is disable */
    if (spiState->rxNumOfBytes == (uint8_t)0)
    {
        LPSPI_HW_ConfigIntMode(baseSpi, LPSPI_RX_DATA_FLAG, false);
    }

    if ((spiState->rxNumOfBytes == (uint8_t)0) &&
        (spiState->txNumOfBytes == (uint8_t)0))
    {
        /* Turn off the fault interrupt source */
        LPSPI_HW_ConfigIntMode(baseSpi, LPSPI_TX_ERROR_FLAG, false);
        LPSPI_HW_ConfigIntMode(baseSpi, LPSPI_RX_ERROR_FLAG, false);

        /* If a callback has been defined,it is invoked */
        if (!POINTER_IS_NULL(spiState->callback))
        {
            spiState->callback(spiState,
                               SPI_EVENT_TRANSFER_COMPLETE,
                               spiState->callbackParam);
        }

        /* If transmission is blocked,send a semaphore */
        if(spiState->isBlocking)
        {
            (void)OSIF_SemPost(&(spiState->lpspiSem));
            spiState->isBlocking = false;
        }

        spiState->inTransfer = false;
    }
}

/*!
 * @brief   This function is the LPSPI2 interrupt handler
 */
void LPSPI2_IRQHandler(void)
{
    LPSPI_IRQHandler(2U);
}

/*!
 * @brief   This function is the LPSPI1 interrupt handler
 */
void LPSPI1_IRQHandler(void)
{
    LPSPI_IRQHandler(1U);
}

/*!
 * @brief   This function is the LPSPI0 interrupt handler
 */
void LPSPI0_IRQHandler(void)
{
    LPSPI_IRQHandler(0U);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
/*!
 * @brief   Resets LPSPI internal logic and registers to default settings.
 *
 * @details This function performs a software reset of the LPSPI,resets most registers,and
 *          then proceeds to manually reset all of the LPSPI registers
 *
 * @param   base : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_Init(LPSPI_T * base)
{
    /* Software reset internal logic */
    base->CTRL.bit.SWRST = (uint32_t)LPSPI_CTRL_SWRST_1;
    /* Now cause the LPSPI module to exit reset and clear CR register */
    base->CTRL.reg = (uint32_t)0x0;
}

/*!
 * @brief   Enables LPSPI module.
 *
 * @param   base : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_Enable(LPSPI_T * base)
{
    base->CTRL.bit.MEN = (uint32_t)LPSPI_CTRL_MEN_1;
}

/*!
 * @brief   Gets the FIFO sizes of the LPSPI module.
 *
 * @param   base     : The pointer to LPSPI_T
 * @param   fifoRxTxSize : FIFO size passed to user
 *
 */
void LPSPI_HW_GetFifoSizes(
    const LPSPI_T * base,
    uint8_t * fifoRxTxSize)
{
    if (fifoRxTxSize != NULL)
    {
        *fifoRxTxSize = (uint8_t)(1U << ((base->PARA.reg & 0xFFu)));
    }
}

/*!
 * @brief   Gets the RX FIFO watermark values.
 *
 * @details This function allows the user to set the RX FIFO watermarks.
 *
 * @param   base     : The pointer to LPSPI_T
 * @param   rxWater  : The RX FIFO watermark value
 *
 */
void LPSPI_HW_ConfigRxWatermarks(
    LPSPI_T * base,
    uint32_t rxWater)
{
    base->FCTRL.bit.RXFW = (uint32_t)rxWater;
}

/*!
 * @brief   Sets the TX FIFO watermark values.
 *
 * @details This function allows the user to set the TX FIFO watermarks.
 *
 * @param   base     : The pointer to LPSPI_T
 * @param   txWater  : The TX FIFO watermark value
 *
 */
void LPSPI_HW_ConfigTxWatermarks(
    LPSPI_T * base,
    uint32_t txWater)
{
    base->FCTRL.bit.TXFW = (uint32_t)txWater;
}

/*!
 * @brief   Gets the LPSPI status flag state.
 *
 * @details This function returns the status of one of the LPSPI status flags based on the
 *          user's request.
 *
 * @param   base        : The pointer to LPSPI_T
 * @param   flagState  : The status flag
 *
 * @retval  State of the status flag
 */
bool LPSPI_HW_GetStatusFlag(
    const LPSPI_T * base,
    LPSPI_STATUS_FLAG_T flagState)
{
    return (bool)(((base->STS.reg) >> (uint8_t)flagState) & 1U) ? true : false;
}


/*!
 * @brief   Configures the LPSPI interrupts.
 *
 * @param   base          : The pointer to LPSPI_T
 * @param   interruptSrc  : The interrupt source
 * @param   enable        : enable or disable the interrupt source
 *
 */
void LPSPI_HW_ConfigIntMode(
    LPSPI_T * base,
    LPSPI_STATUS_FLAG_T interruptSrc,
    bool enable)
{
    if (enable != true)
    {
        base->IEN.reg &= ~((uint32_t)1U << (uint8_t)interruptSrc);
    }
    else
    {
        base->IEN.reg |= (uint32_t)1U << (uint8_t)interruptSrc;
    }
}


/*!
 * @brief   Sets the LPSPI Transmit Data DMA configuration.
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   enable  : enable or disable the TX DMA request
 *
 */
void LPSPI_HW_ConfigTxDmaCmd(LPSPI_T * base, bool enable)
{
    base->DMAEN.bit.TXDMAEN = (uint32_t)enable;
}


/*!
 * @brief   Sets the LPSPI receive Data DMA configuration.
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   enable  : enable or disable the RX DMA request
 *
 */
void LPSPI_HW_ConfigRxDmaCmd(LPSPI_T * base, bool enable)
{
    base->DMAEN.bit.RXDMAEN = (uint32_t)enable;
}


/*!
 * @brief   Returns if the LPSPI interrupt request is enabled or disabled.
 *
 * @param   base          : The pointer to LPSPI_T
 * @param   interruptSrc  : The interrupt source
 *
 * @retval  Returns if the interrupt source
 */
bool LPSPI_HW_GetIntMode(
    const LPSPI_T * base,
    LPSPI_STATUS_FLAG_T interruptSrc)
{
    return (bool)(((base->IEN.reg) >> (uint8_t)interruptSrc) & 1U) ? true : false;
}


/*!
 * @brief   Manually configures specific LPSPI delay parameters
 *
 * @param   base   : The pointer to LPSPI_T
 * @param   delay  : The 8-bit delay value 0x00 to 0xFF (255)
 *
 * @retval  Either STATUS_SUCCESS, LPSPI_STATUS_OUT_OF_RANGE, or STATUS_ERROR if
 *         LPSPI is not disabled or if is not set for master mode.
 */
STATUS_T LPSPI_HW_ConfigDelay(
    LPSPI_T * base,
    LPSPI_DELAY_TYPE_T whichDelay,
    uint32_t delay)
{
    uint32_t ccrVal = 0;

    ccrVal = base->CLKCFG.reg & ~(0xFFUL << (uint32_t)whichDelay);
    ccrVal |= delay << (uint32_t)whichDelay;
    base->CLKCFG.reg = ccrVal;
    return STATUS_SUCCESS;
}


/*!
 * @brief   Writes data into the TX data buffer.
 *
 * @details This function writes the user's incoming data to the transfer data register
 *          users can pass 32-bit data into TDR,Any write to the TDR will result in an
 *          immediate push to the TX FIFO
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   data    : The data word to be sent
 *
 */
void LPSPI_HW_WriteData(LPSPI_T * base, uint32_t data)
{
    base->TXDATA.reg = data;
}


/*!
 * @brief   Reads data from the data buffer.
 *
 * @details This function reads the data from the Receive Data Register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  The data read from the data buffer
 */
uint32_t LPSPI_HW_ReadData(const LPSPI_T * base)
{
    return (uint32_t)base->RXDATA.reg;
}


/*!
 * @brief   Reads TX COUNT form FIFO Status Register.
 *
 * @details This function reads the TX COUNT field from the FIFO Status Register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  The data read from the FIFO Status Register
 */
uint32_t LPSPI_HW_ReadTxCount(const LPSPI_T * base)
{
    return (uint32_t)base->FSTS.bit.TXFCNT;
}

/*!
 * @brief   Reads RX COUNT form FIFO Status Register.
 *
 * @details This function reads the RX COUNT field from the FIFO Status Register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  The data read from the FIFO Status Register
 */
uint32_t LPSPI_HW_ReadRxCount(const LPSPI_T * base)
{
    return (uint32_t)base->FSTS.bit.RXFCNT;
}


/*!
 * @brief   Clear RXDATAMASK bit form TCR Register.
 *
 * @details This function clears the RXDATAMASK bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ClearRxmaskBit(LPSPI_T * base)
{
    base->TXCMD.bit.RXDATAMASK = LPSPI_TXCMD_RXDATAMASK_0;
}


/*!
 * @brief   Set RXDATAMASK bit form TCR Register.
 *
 * @details This function set the RXDATAMASK bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ConfigRxmaskBit(LPSPI_T * base)
{
    base->TXCMD.bit.RXDATAMASK = LPSPI_TXCMD_RXDATAMASK_1;
}


/*!
 * @brief   Clear TXDATAMASK bit form TCR Register.
 *
 * @details This function clears the TXDATAMASK bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ClearTxmaskBit(LPSPI_T * base)
{
    base->TXCMD.bit.TXDATAMASK = LPSPI_TXCMD_TXDATAMASK_0;
}


/*!
 * @brief   Set TXDATAMASK bit form TCR Register.
 *
 * @details This function set the TXDATAMASK bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ConfigTxmskBit(LPSPI_T * base)
{
    base->TXCMD.bit.TXDATAMASK = LPSPI_TXCMD_TXDATAMASK_1;
}


/*!
 * @brief   Clear CTXCMDW bit form TCR Register.
 *
 * @details This function clears the CTXCMDW bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ClearContCBit(LPSPI_T * base)
{
    base->TXCMD.bit.CTXCMDW = LPSPI_TXCMD_CTXCMDW_0;
}


/*!
 * @brief   Set CTXCMDW bit form TCR Register.
 *
 * @details This function set the CTXCMDW bit in the transfer command register.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 */
void LPSPI_HW_ConfigContCBit(LPSPI_T * base)
{
    base->TXCMD.bit.CTXCMDW = LPSPI_TXCMD_CTXCMDW_1;
}



/*!
 * @brief   Configures clock prescalers for all LPSPI master logic.
 *
 * @param   base         : The pointer to LPSPI_T
 * @param   prescaler    : Prescaler value for master logic
 *
 */
void LPSPI_HW_ConfigClockPrescaler (
    LPSPI_T * base,
    LPSPI_PRESCALER_T prescaler)
{
    base->TXCMD.bit.PVALCFG = (uint32_t)prescaler;
}


/*!
 * @brief   Get the clock prescalers for all LPSPI master logic.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  Prescaler value for master logic
 *
 */
LPSPI_PRESCALER_T LPSPI_HW_ReadClockPrescaler (const LPSPI_T * base)
{
    return (LPSPI_PRESCALER_T)base->TXCMD.bit.PVALCFG;
}


/*!
 * @brief   Set whether the sampling point of the master device is delayed.
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   isDelay : Configure if the sampling point is delayed for master devices
 *
 */
void LPSPI_HW_ConfigSamplingPoint (
    LPSPI_T * base,
    bool isDelay)
{
    base->CFG1.bit.IDSPCFG = (uint32_t)isDelay;
}


/*!
 * @brief   Returns whether the LPSPI module is in master mode.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  Returns true if LPSPI in master mode or false if in slave mode.
 *
 */
bool LPSPI_HW_IsMaster(const LPSPI_T * base)
{
    return (bool)(base->CFG1.bit.MSCFG & 1U) ? true : false;
}


/*!
 * @brief   Disables the LPSPI module.
 *
 * @param   base    : The pointer to LPSPI_T
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_Disable(LPSPI_T * base)
{
    uint32_t tempLpspi = base->STS.bit.BSYFLG;

    if (tempLpspi != (uint32_t)1U)
    {
        base->CTRL.bit.MEN = (uint32_t)LPSPI_CTRL_MEN_0;
        return STATUS_SUCCESS;
    }
    else
    {
        return STATUS_BUSY;
    }
}


/*!
 * @brief   Configures the LPSPI for master or slave.
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   mode    : Select the master/slave mode
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_ConfigMasterSlaveMode(
    LPSPI_T * base,
    LPSPI_SELECT_MODE_T mode)
{
    base->CFG1.bit.MSCFG = mode;
    return STATUS_SUCCESS;
}

/*!
 * @brief   Flushes the LPSPI FIFOs.
 *
 * @param   base        : The pointer to LPSPI_T
 * @param   flushTxFifo : The parameter is used to flush Tx fifo.
 * @param   flushRxFifo : The parameter is used to flush RX fifo.
 *
 */
void LPSPI_HW_ConfigFlushFifoCmd(
    LPSPI_T * base,
    bool flushTxFifo,
    bool flushRxFifo)
{
    base->STS.bit.RXWCFLG = (uint32_t)flushRxFifo;
    base->STS.bit.FTXCFLG = (uint32_t)flushTxFifo;
}

/*!
 * @brief   Clears the LPSPI status flag.
 *
 * @param   base          : The pointer to LPSPI_T
 * @param   flagState     : The pointer to LPSPI_STATUS_FLAG_T.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_ClearStatusFlag(
    LPSPI_T * base,
    LPSPI_STATUS_FLAG_T flagState)
{
    if (flagState != LPSPI_ALL_STATUS)
    {
        base->STS.reg |= ((uint32_t)1U << (uint32_t)flagState);
    }
    else
    {
        base->STS.reg |= (uint32_t)LPSPI_ALL_STATUS;
    }
    return STATUS_SUCCESS;
}


/*!
 * @brief   Configures the required LPSPI PCS polarity.
 *
 * @details This feature allows the users to configure the polarity of a specific PCS signal.
 *
 * @param   base           : The pointer to LPSPI_T
 * @param   selectPcs       : LPSPI peripheral chip select.
 * @param   selectPcsPolar    : LPSPI signal (PCS and Host Request) polarity.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_ConfigPcsPolarityMode(
    LPSPI_T * base,
    LPSPI_PER_PCS_T selectPcs,
    LPSPI_SIGNAL_ACTIVE_T selectPcsPolar)
{
    uint32_t value = 0;

    /* Clear the PCS polarity bit */
    value = (base->CFG1.reg) & (~((uint32_t)1U << (0x8u + (uint32_t)selectPcs)));

    /* Configure the PCS polarity bit according to the parameter setting */
    value |= (uint32_t)selectPcsPolar << (0x8u + (uint32_t)selectPcs);

    base->CFG1.reg = value;

    return STATUS_SUCCESS;

}

/*!
 * @brief   Configures the LPSPI SDO/SDI pin configuration mode.
 *
 * @details For SDO/SDI pins,the user can configure them as input/output at will
 *          SDI  |  input  SDO  | output
 *          SDI  |  input  SDI  | output
 *          SDO  |  input  SDI  | output
 *          SDO  |  input  SDO  | output
 *          when the chip selection is released,the user choose to configure the
 *          output data to retain the final value or to tristate the data.
 *          Finally,users can choose to configure PCS[3:2] pins to enable PCS,or
            to use these pins as I/O data pins when users want 4-bit transfers
 *
 * @param   base            : The pointer to LPSPI_T
 * @param   pinCfg          : Set IOPCFG.
 * @param   dataOutConfig   : Set ODCFG.
 * @param   pcs3and2Enable  : Set PCSELCFG.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_ConfigPinConfigMode(
    LPSPI_T * base,
    LPSPI_PIN_CFG_T pinCfg,
    LPSPI_DATA_OUTPUT_CFG_T dataOutConfig,
    bool pcs3and2Enable)
{
    base->CFG1.bit.IOPCFG = (uint32_t)pinCfg;
    base->CFG1.bit.ODCFG = (uint32_t)dataOutConfig;
    base->CFG1.bit.PCSELCFG = (uint32_t)!pcs3and2Enable;

    return STATUS_SUCCESS;
}


/*!
 * @brief   Absolute difference between two numbers.
 *
 * @param   a   : number1
 * @param   b   : number2
 *
 * @retval  difference value
 *
 */
uint32_t AbsDiff(uint32_t a, uint32_t b)
{
    return ((a > b) ? (a - b) : (b - a));
}


/*!
 * @brief   Sets the LPSPI baud rate in bits per second.
 *
 * @details This function receives data from the parameters,calculates the closest possible
 *          baud rate,and returns the calculated baud rate in units of (bit/s).the user to provide
 *          the clock frequency of the module
 *
 * @param   base            : The pointer to LPSPI_T
 * @param   baudBitsPer     : Bits per seconds
 * @param   sourceClkInHz   : Input clock source.
 * @param   tcrPreVal       : TCR prescale value.
 *
 * @retval  optimalFreq
 *
 */
uint32_t LPSPI_HW_ConfigBaudRate(
    LPSPI_T * base,
    uint32_t baudBitsPer,
    uint32_t sourceClkInHz,
    uint32_t * tcrPreVal)
{
    uint32_t tempOptimalScaler = 0U;
    uint32_t tempOptimalFreq = 0U;
    uint32_t low, high;
    uint8_t  prescaler = 0U;
    uint8_t  scaler = 0U;
    uint32_t freq2 = 0U;
    uint32_t freq1 = 0U;
    uint32_t optimaPre = 0U;
    uint32_t optimalScaler = 0U;
    uint32_t optimalFreq = 0xFFFFFFFFU;

    for (prescaler = 0; prescaler < 8U; prescaler++)
    {
        low = 0U;
        high = 256U;

        /* Implement golden section search algorithm */

        for(;;)
        {
            scaler = (uint8_t)((low + high) / 2U);
            freq1 = sourceClkInHz / (g_lpspiBaudRatePre[prescaler] * (scaler + (uint32_t)2U));

            if (freq1 >= baudBitsPer)
            {
                low = scaler;
            }
            else
            {
                high = scaler;
            }

            if (AbsDiff(baudBitsPer, optimalFreq) > AbsDiff(baudBitsPer, freq1))
            {
                optimalFreq = freq1;
            }

            if ((high - low) <= 1U)
            {
                break;
            }
        }

        /* Evaluate last 2 scaler values */
        freq1 = sourceClkInHz / (g_lpspiBaudRatePre[prescaler] * (low + (uint32_t)2U));
        freq2 = sourceClkInHz / (g_lpspiBaudRatePre[prescaler] * (high + (uint32_t)2U));

        if (AbsDiff(baudBitsPer, freq1) <= AbsDiff(baudBitsPer, freq2))
        {
            tempOptimalFreq = freq1;
            tempOptimalScaler = low;
        }
        else
        {
            tempOptimalFreq = freq2;
            tempOptimalScaler = high;
        }

        if (AbsDiff(baudBitsPer, optimalFreq) >=
            AbsDiff(baudBitsPer, tempOptimalFreq))
        {
            optimalFreq = tempOptimalFreq;
            optimalScaler = tempOptimalScaler;
            optimaPre = prescaler;
        }

        /* If current frequency is equal to target frequency, the search stops */
        if(optimalFreq == baudBitsPer)
        {
            break;
        }
    }

    /* Add default values for delay between transfers. */
    LPSPI_HW_ConfigDelay(base,
                         LPSPI_SCK_TO_PCS_DELAY,
                         optimalScaler >> 2U);
    LPSPI_HW_ConfigDelay(base,
                         LPSPI_PCS_TO_SCK_DELAY,
                         optimalScaler >> 2U);
    LPSPI_HW_ConfigDelay(base,
                         LPSPI_BETWEEN_TRANSFER,
                         optimalScaler >> 2U);

    /* Write the best baud rate scalar to the CCR */
    LPSPI_HW_ConfigBaudRateDivisor(base, optimalScaler);

    /* return the best prescaler value */
    *tcrPreVal = optimaPre;

    /* Returns the final baud rate */
    return optimalFreq;
}


/*!
 * @brief   Manually configuring baud rate divisor
 *
 * @param   base    : The pointer to LPSPI_T
 * @param   divisor : Divisor factor.
 *
 * @retval  Status return codes.
 *
 */
STATUS_T LPSPI_HW_ConfigBaudRateDivisor(LPSPI_T * base, uint32_t divisor)
{
    base->CLKCFG.bit.SCKDRCFG = (uint32_t)divisor;
    return STATUS_SUCCESS;
}


/*!
 * @brief   Sets Transfer Command Register (TCR) parameters.
 *
 * @details The send command register contains multiple parameters
            that affect data transmission, such as clock phase and polarity,
            which PCS to use, whether PCS maintains assertion at frame completion,
            and so on.Any write operation to this register will cause the entire register
            and its contents to be immediately pushed to TX FIFO
 *
 * @param   base        : The pointer to LPSPI_T
 * @param   txCmdCfgSet : Divisor The pointer to LPSPI_TX_CMD_CFG_T.
 *
 */
void LPSPI_HW_ConfigTxCommandReg(
    LPSPI_T * base,
    const LPSPI_TX_CMD_CFG_T * txCmdCfgSet)
{
    base->TXCMD.reg = (((uint32_t)txCmdCfgSet->clkPolar << 31u)  |  \
                       ((uint32_t)txCmdCfgSet->selectClkPhase << 30u)     |  \
                       ((uint32_t)txCmdCfgSet->preDiv << 27u)       |  \
                       ((uint32_t)txCmdCfgSet->selectPcs << 24u)     |  \
                       ((uint32_t)txCmdCfgSet->sendLsbFirst << 23u)     |  \
                       ((uint32_t)txCmdCfgSet->byteInter<< 22u)      |  \
                       ((uint32_t)txCmdCfgSet->contentTransfer << 21u) |  \
                       ((uint32_t)txCmdCfgSet->contentCmd << 20u)      |  \
                       ((uint32_t)txCmdCfgSet->rxDataMask << 19u)       |  \
                       ((uint32_t)txCmdCfgSet->txDataMask << 18u)       |  \
                       ((uint32_t)txCmdCfgSet->width << 16u)        |  \
                       ((uint32_t)(txCmdCfgSet->frameNum - 1UL) << 0u));
}


/*!
 * @brief   Sets the PCS flag to a value between 0 and 3.
 *
 * @details This function modifies the TCR register and sets the value of the PCS flag
 *          to the value of the selectPcs parameter.
 *
 * @param   base     : The pointer to LPSPI_T
 * @param   selectPcs : Selects which PCS to use
 *
 */
void LPSPI_HW_ConfigPcs(LPSPI_T * base, LPSPI_PER_PCS_T selectPcs)
{
    base->TXCMD.bit.PCSEL = selectPcs;
}


/**@} end of group LPSPI_Functions*/
/**@} end of group LPSPI_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

