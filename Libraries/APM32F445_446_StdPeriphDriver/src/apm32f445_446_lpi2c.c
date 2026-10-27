/*!
 * @file        apm32f445_446_lpi2c.c
 *
 * @brief       This file provides all the LPI2C firmware functions
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
#include "apm32f445_446_lpi2c.h"
#include "osif.h"

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

/* Used for baudrate calculation */
#define SDA_HOLD_TIME_MIN_VALUE     1U
#define BUS_IDLE_MAX_VALUE          0xFFFU
#define SCL_SETUP_TIME_MIN_VALUE    2U
#define SCL_HIGH_MIN_VALUE          1U
#define SCL_HIGH_MAX_VALUE          ((1U << 6) - 1U)
#define SCL_LOW_MIN_VALUE           3U
#define SCL_LOW_MAX_VALUE           SCL_HIGH_MAX_VALUE
#define SCL_TOTAL_MAX_VALUE         (SCL_HIGH_MAX_VALUE + SCL_LOW_MAX_VALUE + 2U)

/* Number of instances of the LPI2C module */
#define LPI2C_INSTANCE_COUNT            (1u)
#define ASSERT_INSTANCE(__instance__)   ({if (__instance__ >= LPI2C_INSTANCE_COUNT) {while(1);}})

/**@} end of group LPI2C_Macros*/

/** @defgroup LPI2C_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief Direction of a LPI2C transfer
 */
typedef enum
{
    LPI2C_REQUEST_RX = 1,
    LPI2C_REQUEST_TX = 0,
} LPI2C_TRANSFER_DIR_T;

/**@} end of group LPI2C_Enumerations*/

/** @defgroup LPI2C_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief DMA internal parameters structure
 */
typedef struct
{
    DMA_TRANSFER_TYPE_T transferType;
    LPI2C_TRANSFER_DIR_T transferDir;
    uint8_t dmaChannel;
    uint32_t regAddrOfData;
    uint32_t transferSize;
    uint8_t *transferBuffer;
} LPI2C_DMA_TRANSFER_PARAMS_T;

/**@} end of group LPI2C_Structures*/

/** @defgroup LPI2C_Variables Variables
  @{
*/

/* Pointer to runtime state structure.*/
static LPI2C_MASTER_STATE_T *g_lpi2cMasterStates[LPI2C_INSTANCE_COUNT] = { NULL };
static LPI2C_SLAVE_STATE_T *g_lpi2cSlaveStates[LPI2C_INSTANCE_COUNT] = { NULL };

/* Table of base addresses for LPI2C instances */
static LPI2C_T *const g_lpi2cBaseAddress[LPI2C_INSTANCE_COUNT] = { LPI2C };

/* Table for lpi2c IRQ numbers */
static const IRQn_Type g_lpi2cMasterIrqId[LPI2C_INSTANCE_COUNT] = { LPI2C_Master_IRQn };
static const IRQn_Type g_lpi2cSlaveIrqId[LPI2C_INSTANCE_COUNT] = { LPI2C_Slave_IRQn };

/* PCM clock sources, for getting the input clock frequency */
static const CLOCK_NAMES_T g_lpi2cClock[LPI2C_INSTANCE_COUNT] = LPI2C_PCM_CLOCKS;

/* LPI2C DMA request sources */
static const uint8_t g_lpi2cDMARequestSources[LPI2C_INSTANCE_COUNT][2] = LPI2C_DMA_REQUEST;

/**@} end of group LPI2C_Variables*/

/** @defgroup LPI2C_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/

static bool LPI2C_MasterCmdQueueIsEmpty(const LPI2C_MASTER_STATE_T *masterState);
static void LPI2C_MasterQueueReset(LPI2C_MASTER_STATE_T *masterState);

static void LPI2C_MasterQueueCmd(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    LPI2C_MASTER_CMD_T cmd,
    uint8_t data);

static void LPI2C_MasterSendQueuedCmd(LPI2C_T *lpi2cBase, LPI2C_MASTER_STATE_T *masterState);

static void LPI2C_MasterSendAddress(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    bool isReceive);

static void LPI2C_MasterQueueData(LPI2C_T *lpi2cBase, LPI2C_MASTER_STATE_T *masterState);

static void LPI2C_MasterEndTransfer(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    bool isSendStop,
    bool isResetFIFO);

static void LPI2C_SlaveEndTransfer( LPI2C_T *lpi2cBase, LPI2C_SLAVE_STATE_T *slaveState);
static void LPI2C_SlaveEndTransferHandler( LPI2C_SLAVE_STATE_T *slaveState, LPI2C_T *lpi2cBase);
static void LPI2C_MasterConfigOperatingMode( uint32_t instance, LPI2C_OPERATION_MODE_T i2cMode);
static void LPI2C_SlaveConfigOperatingMode( uint32_t instance, LPI2C_OPERATION_MODE_T i2cMode);
static void LPI2C_ConfigDmaTransfer( uint32_t instance, const LPI2C_DMA_TRANSFER_PARAMS_T *dmaTransParams);

static void LPI2C_MasterCompleteDmaTransfer(void *parameter, DMA_CHANNEL_STATUS_T status);
static void LPI2C_MasterStartDmaTransfer(uint32_t instance);
static void LPI2C_SlaveStartDmaTransfer(uint32_t instance);
static STATUS_T LPI2C_MasterReInit(uint32_t instance, LPI2C_MASTER_STATE_T *masterState);
static STATUS_T LPI2C_MasterWaitTransferEnd(uint32_t instance, uint32_t timeout);
static STATUS_T LPI2C_SlaveWaitTransferEnd(uint32_t instance, uint32_t timeout);

static void LPI2C_MasterHandleTransmitDataRequest(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState);

static void LPI2C_MasterHandleReceiveDataReadyEvent(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState);

static void LPI2C_SlaveHandleAddressValidEvent(
    uint32_t instance,
    const LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState);

static void LPI2C_SlaveHandleTransmitDataEvent(
    LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState);

static void LPI2C_SlaveHandleReceiveDataEvent(
    const LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState);


/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Config initialize the LPI2C master mode driver
 *
 * @param instance: LPI2C peripheral instance number
 * @param masterUserConfig: Pointer to the LPI2C master user configuration structure. The function
 *                    reads configuration data from this structure and initializes the
 *                    driver accordingly. The application may free this structure after
 *                    the function returns.
 * @param masterState: Pointer to the LPI2C master driver context structure. The driver uses
 *                     this memory area for its internal logic. The application must make no
 *                     assumptions about the content of this structure, and must not free this
 *                     memory until the driver is de-initialized using LPI2C_MasterDeInit().
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterInit(
    uint32_t instance,
    const LPI2C_MASTER_USER_CONFIG_T *masterUserConfig,
    LPI2C_MASTER_STATE_T *masterState)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    STATUS_T result = STATUS_SUCCESS;
    uint32_t inputClk  = 0;
    LPI2C_BAUDRATE_PARAMS_T baudrateParam = {0};

    /* Check the protocol clock frequency */
    result = CLOCK_SYS_ReadFreq(g_lpi2cClock[instance], &inputClk);
    if ((result == STATUS_SUCCESS) && (inputClk > 0U))
    {
        g_lpi2cMasterStates[instance] = masterState;

        /* Initialize master status structure */
        masterState->transferType = masterUserConfig->transferType;
        masterState->isIdle = true;
        masterState->rxSize = 0U;
        masterState->rxBuffer = NULL;
        masterState->txSize = 0U;
        masterState->txBuffer = NULL;
        masterState->dmaChannel = masterUserConfig->dmaChannel;
        masterState->baudrate = masterUserConfig->baudrate;
        masterState->isAddrFor10bit = masterUserConfig->isAddrFor10bit;
        masterState->slaveAddr = masterUserConfig->slaveAddr;
        masterState->callbackParam = masterUserConfig->callbackParam;
        masterState->masterCallback = masterUserConfig->masterCallback;
        masterState->abortedTransfer = false;
        masterState->isBlocking = false;
        masterState->status = STATUS_SUCCESS;

        /* Initialize the semaphore */
        result = OSIF_SemCreate(&(masterState->idleSem), 0);

        LPI2C_MasterQueueReset(masterState);

        /* Enable lpi2c interrupt */
        INT_SYS_EnableIRQ(g_lpi2cMasterIrqId[instance]);

        /* Initialize module */
        LPI2C_HW_Init(lpi2cBase);

        /* Config baudrate */
        baudrateParam.baudrate = masterUserConfig->baudrate;
        LPI2C_MasterSetBaudrate(instance,
                                masterUserConfig->i2cMode,
                                baudrateParam);

        /* Config slave address */
        LPI2C_MasterSetSlaveAddr(instance,
                                 masterUserConfig->slaveAddr,
                                 masterUserConfig->isAddrFor10bit);

        /* Enable LPI2C master */
        LPI2C_HW_ConfigMasterEnable(lpi2cBase, true);
    }

    return result;
}

/*!
 * @brief De-initialize the LPI2C master mode driver
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval Error or success status returned by API
 *
 * @note The driver can't be used again until reinitialized.
 *       The context structure is no longer needed by the driver
 *       and can be freed after calling this function.
 */
STATUS_T LPI2C_MasterDeInit(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    LPI2C_HW_ConfigMasterEnable(lpi2cBase, false);
    INT_SYS_DisableIRQ(g_lpi2cMasterIrqId[instance]);
    g_lpi2cMasterStates[instance] = NULL;
    (void)OSIF_SemDestroy(&(masterState->idleSem));

    return result;
}

/*!
 * @brief Config bus idle timeout for LPI2C
 *
 * @details This function sets time out for bus idle for Master.
 *          If both SCL and SDA are high for longer than Timeout cycles,
 *          then the I2C bus is assumed to be idle and
 *          the master can generate a START condition
 *
 * @param instance: LPI2C peripheral instance number
 * @param timeout: bus idle timeout period in clock cycle.
 *        Zero means no bus idle timeout
 *
 * @retval None
 */
void LPI2C_MasterSetBusIdleTimeout(uint32_t instance, uint16_t timeout)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    uint16_t timeoutValue = 0;

    /* Limit timeout value since BUSIDLE field is 12-bit long */
    timeoutValue = ((timeout > BUS_IDLE_MAX_VALUE) ? BUS_IDLE_MAX_VALUE : timeout);

    LPI2C_HW_ConfigMasterEnable(lpi2cBase, false);
    LPI2C_HW_ConfigMasterBusIdleTimeoutValue(lpi2cBase, timeoutValue);
    LPI2C_HW_ConfigMasterEnable(lpi2cBase, true);
}

/*!
 * @brief Read the currently configured baudrate
 *
 * @param instance: LPI2C peripheral instance number
 * @param baudrateParam: structure that contains the current baudrate in hertz
 *                       and the baudrate in hertz for High-Speed mode (unused
 *                       in other modes, can be NULL)
 *
 * @retval None
 */
void LPI2C_MasterGetBaudrate(uint32_t instance, LPI2C_BAUDRATE_PARAMS_T *baudrateParam)
{
    ASSERT_INSTANCE(instance);

    const LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    uint32_t inputClock = 0U;
    uint32_t sclLow = 0U;
    uint32_t sclHigh = 0U;
    LPI2C_MASTER_PSC_T prescaler = LPI2C_MASTER_PSC_DIV_1;
    STATUS_T result = STATUS_SUCCESS;

    /* Read the protocol clock frequency */
    result = CLOCK_SYS_ReadFreq(g_lpi2cClock[instance], &inputClock);

    if (result == STATUS_SUCCESS)
    {
        /* Ignoring the glitch filter, the baudrate formula is:
           SCL_freq = Input_freq / (2^PRESCALER * (SCL_LOW + SCL_HIGH + 2))
        */
        sclLow = (uint32_t)LPI2C_HW_ReadMasterSCLLowPeriod(lpi2cBase);
        sclHigh = (uint32_t)LPI2C_HW_ReadMasterSCLHighPeriod(lpi2cBase);
        prescaler = LPI2C_HW_ReadMasterPrescaler(lpi2cBase);

        baudrateParam->baudrate = inputClock
            / (((uint32_t)1U << (uint32_t)prescaler) * (sclLow + sclHigh + (uint32_t)2U));

        (void)masterState;
    }
}

/*!
 * @brief Config the baudrate for any subsequent I2C communication
 *
 * @param instance: LPI2C peripheral instance number
 * @param i2cMode: I2C operating mode
 * @param baudrateParam: structure that contains the baudrate in hertz to use by current slave device
 *                       and also the baudrate in hertz for High-Speed mode (unused in other modes)
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterSetBaudrate(
    uint32_t instance,
    const LPI2C_OPERATION_MODE_T i2cMode,
    const LPI2C_BAUDRATE_PARAMS_T baudrateParam)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_MASTER_STATE_T * masterState = g_lpi2cMasterStates[instance];
    uint32_t dataValidDelay = 0U;
    uint32_t prescalerMin = 0U;
    uint32_t prescaler = 0U;
    uint32_t inputClock = 0U;
    uint32_t sclTotal = 0U;
    uint32_t sclHigh = 0U;
    uint32_t sclLow = 0U;
    uint32_t setHold = 0U;
    STATUS_T result = STATUS_BUSY;

    /* Check if driver is idle */
    if (masterState->isIdle)
    {
        /* Read the protocol clock frequency */
        result = CLOCK_SYS_ReadFreq(g_lpi2cClock[instance], &inputClock);

        if ((result == STATUS_SUCCESS) && (inputClock > 0U))
        {
            /* Disable master */
            LPI2C_HW_ConfigMasterEnable(lpi2cBase, false);

            /**
             * Ignore the glitch filter, the baudrate formula is:
             * SCL_Freq = Input_Freq / (2^PRESCALER * (SCL_LOW + SCL_HIGH + 2))
             * Assume SCL_LOW = SCL_HIGH, SETHOLD = SCL_HIGH, DATAVD = SCL_HIGH/2
             */
            if (baudrateParam.baudrate == 0U)
            {
                sclTotal = SCL_TOTAL_MAX_VALUE;
                prescaler = 7U;
            }
            else
            {
                /**
                 * Calculate minimum prescaler for which SCL_LOW and SCL_HIGH
                 * values are in valid range. Always round up.
                 */
                prescalerMin = (  (inputClock - 1U)
                                / ((baudrateParam.baudrate) * SCL_TOTAL_MAX_VALUE))
                             + (uint32_t)1U;

                for (prescaler = 0U; prescaler < 7U; prescaler++)
                {
                    if (((uint32_t)1U << prescaler) >= prescalerMin)
                    {
                        break;
                    }
                }

                /* Calculate SCL_LOW and SCL_HIGH values for this prescaler. Round to nearest integer */
                sclTotal = (inputClock + ((baudrateParam.baudrate << prescaler) >> 1U))
                         / (baudrateParam.baudrate << prescaler);
            }
            sclTotal = (sclTotal > SCL_TOTAL_MAX_VALUE) ? SCL_TOTAL_MAX_VALUE : sclTotal;

            /*
             * If we try to calculate clock high and low values using sclTotal equal with 0
             * (this is the case when the baudrateParam is 0), we will get negative values for
             * them, so we set them to 0 for this case.
             */
            if (sclTotal > 1U)
            {
                sclHigh = (sclTotal - 2U) / 2U;
                sclLow = sclTotal - 2U - sclHigh;
            }
            sclLow = (sclLow < SCL_LOW_MIN_VALUE) ? SCL_LOW_MIN_VALUE : sclLow;
            sclHigh = (sclHigh < SCL_HIGH_MIN_VALUE) ? SCL_HIGH_MIN_VALUE : sclHigh;

            /* Calculate Data Valid Delay and SETHOLD */
            setHold = sclHigh;
            setHold = (setHold < SCL_SETUP_TIME_MIN_VALUE) ? SCL_SETUP_TIME_MIN_VALUE : setHold;

            dataValidDelay = sclHigh >> 1U;
            dataValidDelay = (dataValidDelay < SDA_HOLD_TIME_MIN_VALUE) ? SDA_HOLD_TIME_MIN_VALUE : dataValidDelay;

            /* Apply settings */
            LPI2C_HW_ConfigMasterPrescaler(lpi2cBase, (LPI2C_MASTER_PSC_T)prescaler);
            LPI2C_HW_ConfigMasterSCLLowPeriod(lpi2cBase, (uint8_t)sclLow);
            LPI2C_HW_ConfigMasterSCLHighPeriod(lpi2cBase, (uint8_t)sclHigh);
            LPI2C_HW_ConfigMasterSetupHoldDelay(lpi2cBase, (uint8_t)setHold);
            LPI2C_HW_ConfigMasterSDAHoldTime(lpi2cBase, (uint8_t)dataValidDelay);

            /* Perform other settings related to the operating mode */
            LPI2C_MasterConfigOperatingMode(instance, i2cMode);

            /* Re-enable master */
            LPI2C_HW_ConfigMasterEnable(lpi2cBase, true);
        }
    }
    return result;
}

/*!
 * @brief Config the slave address for any subsequent I2C communication
 *
 * @param instance: LPI2C peripheral instance number
 * @param addr: slave address, 7-bit or 10-bit
 * @param isAddrFor10bit: specifies if provided address is 10-bit
 *
 * @retval None
 */
void LPI2C_MasterSetSlaveAddr(
    uint32_t instance,
    const uint16_t addr,
    const bool isAddrFor10bit)
{
    ASSERT_INSTANCE(instance);

    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    if (false == POINTER_IS_NULL(masterState))
    {
        masterState->isAddrFor10bit = isAddrFor10bit;
        masterState->slaveAddr = addr;
    }
}

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus
 *
 * @param instance: LPI2C peripheral instance number
 * @param txBuffer: pointer to the data to be transferred
 * @param txSize: length in bytes of the data to be transferred
 * @param isSendStop: specifies whether or not to generate stop condition after the transmission
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterSendDataNonBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    bool isSendStop)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    /* Check if driver is idle */
    if (masterState->isIdle)
    {
        /* Copy parameters to driver state structure */
        masterState->isIdle = false;
        masterState->txBuffer = txBuffer;
        masterState->txSize = txSize;
        masterState->isSendStop = isSendStop;
        masterState->status = STATUS_BUSY;

        if (masterState->transferType != LPI2C_USE_DMA)
        {
            /* Reset fifo before start a new transmission */
            LPI2C_HW_ResetMasterTxFIFO(lpi2cBase);
            LPI2C_HW_ResetMasterRxFIFO(lpi2cBase);

            /* Clear all status frag */
            LPI2C_HW_ClearMasterAllEvents(lpi2cBase);

            /* Initiate communication */
            LPI2C_MasterSendAddress(lpi2cBase, masterState, false);

            /* Queue data bytes to fill tx fifo */
            LPI2C_MasterQueueData(lpi2cBase, masterState);

            /* Config tx FIFO watermark */
            LPI2C_HW_ConfigMasterTxFIFOWatermark(lpi2cBase, 0U);

            /* Enable relevant events */
            LPI2C_HW_ConfigMasterInt(lpi2cBase, LPI2C_MASTER_INT_FIFO_ERROR |
                                                LPI2C_MASTER_INT_ARBITRATION_LOST |
                                                LPI2C_MASTER_INT_NACK_DETECT |
                                                LPI2C_MASTER_INT_TX_DATA,
                                        true);
        }
        else
        {
            LPI2C_HW_ConfigMasterInt(lpi2cBase, LPI2C_MASTER_INT_FIFO_ERROR |
                                            LPI2C_MASTER_INT_ARBITRATION_LOST |
                                            LPI2C_MASTER_INT_NACK_DETECT,
                                true);

            LPI2C_MasterStartDmaTransfer(instance);
        }
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief Perform a blocking send transaction on the I2C bus
 *
 * @param instance: LPI2C peripheral instance number
 * @param txBuffer: pointer to the data to be transferred
 * @param txSize: length in bytes of the data to be transferred
 * @param sendStop: specifies whether or not to generate stop condition after the transmission
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterSendDataBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    bool isSendStop,
    uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    /* Check if driver is idle */
    if (masterState->isIdle)
    {
        /* mark transfer as blocking */
        masterState->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(masterState->idleSem), 0);

        (void)LPI2C_MasterSendDataNonBlocking(instance, txBuffer, txSize, isSendStop);

        /* Wait for transfer to end */
        result = LPI2C_MasterWaitTransferEnd(instance, timeout);
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief Abort a non-blocking I2C Master write or read data
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterAbortTransfer(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    if (POINTER_IS_NULL(masterState->rxBuffer) == true)
    {
        /* End transfer: force stop generation, reset FIFOs */
        masterState->status = STATUS_I2C_ABORTED;
        LPI2C_MasterEndTransfer(lpi2cBase, masterState, true, true);
    }
    else
    {
        /* Aborting a reception not supported because hardware will continue the
        current command even if the FIFO is reset and this could last indefinitely */
        result = STATUS_UNSUPPORTED;
    }

    return result;
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus
 *
 * @param instance: LPI2C peripheral instance number
 * @param rxBuffer: pointer to the buffer where to store received data
 * @param rxSize: length in bytes of the data to be transferred
 * @param isSendStop: specifies whether or not to generate stop condition after the reception
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterReceiveDataNonBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    bool isSendStop)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    uint16_t fifoSize = 0U;

    /* Check if driver is idle */
    if (masterState->isIdle)
    {
        /* Copy parameters to driver state structure */
        masterState->isIdle = false;
        masterState->rxSize = rxSize;
        masterState->rxBuffer = rxBuffer;
        masterState->status = STATUS_BUSY;
        masterState->isSendStop = isSendStop;

        if (masterState->transferType != LPI2C_USE_DMA)
        {
            /* Reset fifo before start a new transmission */
            LPI2C_HW_ResetMasterTxFIFO(lpi2cBase);
            LPI2C_HW_ResetMasterRxFIFO(lpi2cBase);

            /* Clear all status frag */
            LPI2C_HW_ClearMasterAllEvents(lpi2cBase);

            /* Initiate communication */
            LPI2C_MasterSendAddress(lpi2cBase, masterState, true);
            /* Queue receive command for rxSize bytes */
            LPI2C_MasterQueueCmd(lpi2cBase, masterState, LPI2C_MASTER_CMD_RECEIVE, (uint8_t)(rxSize - 1U));

            /* Config rx FIFO watermark */
            fifoSize = LPI2C_HW_ReadMasterRxFIFOSize(lpi2cBase);
            if (fifoSize > rxSize)
            {
                fifoSize = (uint8_t)rxSize;
            }
            LPI2C_HW_ConfigMasterRxFIFOWatermark(lpi2cBase, (uint16_t)(fifoSize - 1U));

            /* Enable relevant events */
            if (LPI2C_MasterCmdQueueIsEmpty(masterState))
            {
                LPI2C_HW_ConfigMasterInt(lpi2cBase, LPI2C_MASTER_INT_FIFO_ERROR |
                                                    LPI2C_MASTER_INT_ARBITRATION_LOST |
                                                    LPI2C_MASTER_INT_NACK_DETECT |
                                                    LPI2C_MASTER_INT_RX_DATA,
                                         true);
            }
            else
            {
                /* Enable tx event too if there are commands in the software FIFO */
                LPI2C_HW_ConfigMasterInt(lpi2cBase, LPI2C_MASTER_INT_FIFO_ERROR |
                                                    LPI2C_MASTER_INT_ARBITRATION_LOST |
                                                    LPI2C_MASTER_INT_NACK_DETECT |
                                                    LPI2C_MASTER_INT_TX_DATA |
                                                    LPI2C_MASTER_INT_RX_DATA,
                                         true);
            }
        }
        else
        {
            LPI2C_HW_ConfigMasterInt(lpi2cBase, LPI2C_MASTER_INT_FIFO_ERROR |
                                                LPI2C_MASTER_INT_ARBITRATION_LOST |
                                                LPI2C_MASTER_INT_NACK_DETECT,
                                     true);

            LPI2C_MasterStartDmaTransfer(instance);
        }
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief Perform a blocking receive transaction on the I2C bus
 *
 * @param instance: LPI2C peripheral instance number
 * @param rxBuffer: pointer to the buffer where to store received data
 * @param rxSize: length in bytes of the data to be transferred
 * @param isSendStop: specifies whether or not to generate stop condition after the reception
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterReceiveDataBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    bool isSendStop,
    uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    STATUS_T result = STATUS_SUCCESS;

    /* Check if driver is idle */
    if (masterState->isIdle)
    {
        /* mark transfer as blocking */
        masterState->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(masterState->idleSem), 0);

        result = LPI2C_MasterReceiveDataNonBlocking(instance, rxBuffer, rxSize, isSendStop);

        /* Wait for transfer to end */
        result = LPI2C_MasterWaitTransferEnd(instance, timeout);
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief Read the current status of the I2C master transfer
 *
 * @detail This function can be called during a non-blocking transmission to check the
 * status of the transfer.
 *
 * @param instance: LPI2C peripheral instance number
 * @param bytesRemain: the number of remaining bytes in the active I2C transfer
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemain)
{
    ASSERT_INSTANCE(instance);

    const LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    if (POINTER_IS_NULL(bytesRemain))
    {
        return masterState->status;
    }

    if (LPI2C_USE_INTERRUPTS == masterState->transferType)
    {
        *bytesRemain = 0U;

        if (masterState->rxSize > 0U)
        {
            /* Remaining bytes = free space in buffer - bytes in rx FIFO */
            *bytesRemain = masterState->rxSize
                         - (uint32_t)LPI2C_HW_ReadMasterRxFIFOCount(lpi2cBase);
        }
        else if (masterState->txSize > 0U)
        {
            /* Remaining bytes = bytes in buffer + bytes in tx FIFO */
            *bytesRemain = (uint32_t)LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase)
                         + masterState->txSize;
        }
    }
    return masterState->status;
}

/*!
 * @brief initializate the default configuration structure for master
 *
 * @param masterUserConfig: Pointer to configuration structure
 *
 * @retval None
 */
void LPI2C_MasterDefaultConfig(LPI2C_MASTER_USER_CONFIG_T *masterUserConfig)
{
    masterUserConfig->i2cMode        = LPI2C_STANDARD_MODE;
    masterUserConfig->transferType   = LPI2C_USE_INTERRUPTS;
    masterUserConfig->isAddrFor10bit    = false;
    masterUserConfig->slaveAddr      = 32U;
    masterUserConfig->baudrate       = 100000U;
    masterUserConfig->dmaChannel     = 0U;
    masterUserConfig->callbackParam  = NULL;
    masterUserConfig->masterCallback = NULL;
}

/*!
 * @brief Handle master operation when I2C interrupt occurs
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval None
 */
void LPI2C_MasterIRQHandler(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    /* Check which event caused the interrupt */
    if (LPI2C_HW_ReadMasterTransmitDataRequestEvent(lpi2cBase))
    {
        LPI2C_MasterHandleTransmitDataRequest(lpi2cBase, masterState);
    }

    if (LPI2C_HW_ReadMasterReceiveDataReadyEvent(lpi2cBase))
    {
        if (!masterState->abortedTransfer)
        {
            LPI2C_MasterHandleReceiveDataReadyEvent(lpi2cBase, masterState);
        }
        else
        {
            /* Signal transfer end for blocking transfers */
            if (masterState->isBlocking == true)
            {
                (void)OSIF_SemPost(&(masterState->idleSem));
            }

            masterState->status = STATUS_TIMEOUT;

            (void)LPI2C_MasterReInit(instance, masterState);
        }
    }

    if (LPI2C_HW_ReadMasterFIFOErrorEvent(lpi2cBase))
    {
        /* FIFO error */
        LPI2C_HW_ClearMasterFIFOErrorEvent(lpi2cBase);
        masterState->status = STATUS_ERROR;

        /* End transfer: no stop generation (the module will handle that by itself
           if needed), FIFOs will be reset on new frame to avoid stuck STOP bit */
        LPI2C_MasterEndTransfer(lpi2cBase, masterState, false, false);

        /* Signal transfer end for blocking transfers */
        if (masterState->isBlocking == true)
        {
            (void)OSIF_SemPost(&(masterState->idleSem));
        }

        if (POINTER_IS_NULL(masterState->masterCallback) == false)
        {
            masterState->masterCallback(I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                                        masterState->callbackParam);
        }
    }

    if (LPI2C_HW_ReadMasterArbitrationLostEvent(lpi2cBase))
    {
        /* Arbitration lost */
        LPI2C_HW_ClearMasterArbitrationLostEvent(lpi2cBase);

        /* End transfer: no stop generation (the module will handle that by itself
           if needed), FIFOs will be reset on new frame to avoid stuck STOP bit */
        LPI2C_MasterEndTransfer(lpi2cBase, masterState, false, false);

        /* Signal transfer end for blocking transfers */
        if (masterState->isBlocking == true)
        {
            (void)OSIF_SemPost(&(masterState->idleSem));
        }

        masterState->status = STATUS_I2C_ARBITRATION_LOST_ERROR;

        if (POINTER_IS_NULL(masterState->masterCallback) == false)
        {
            masterState->masterCallback(I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                                        masterState->callbackParam);
        }
    }

    if (LPI2C_HW_ReadMasterNACKDetectEvent(lpi2cBase))
    {
        /* Received NACK */
        /* Signal transfer end for blocking transfers */
        if (masterState->isBlocking == true)
        {
            (void)OSIF_SemPost(&(masterState->idleSem));
        }

        masterState->status = STATUS_I2C_NACK_ERROR;

        /* End transfer: no stop generation (the module will handle that by itself
            if needed), FIFOs will be reset on new frame to avoid stuck STOP bit */
        LPI2C_MasterEndTransfer(lpi2cBase, masterState, false, false);

        if (POINTER_IS_NULL(masterState->masterCallback) == false)
        {
            masterState->masterCallback(I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                                        masterState->callbackParam);
        }

        /* Clear NACK flag */
        LPI2C_HW_ClearMasterNACKDetectEvent(lpi2cBase);
    }
}

/*!
 * @brief Initialize the I2C slave mode driver
 *
 * @param instance: LPI2C peripheral instance number.
 * @param slaveUserConfig: Pointer to the LPI2C slave user configuration structure. The function
 *                    reads configuration data from this structure and initializes the
 *                    driver accordingly. The application may free this structure after
 *                    the function returns.
 * @param slaveState: Pointer to the LPI2C slave driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using LPI2C_SlaveDeInit().
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveInit(
    uint32_t instance,
    const LPI2C_SLAVE_USER_CONFIG_T *slaveUserConfig,
    LPI2C_SLAVE_STATE_T *slaveState)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    STATUS_T result = STATUS_SUCCESS;
    uint32_t inputClock = 0;

    /*
     * Check the protocol clock frequency.
     * LPI2C slave remains operational, even when the LPI2C functional
     * clock is disabled, so we don't need to check if inputClock is 0.
     */
    result = CLOCK_SYS_ReadFreq(g_lpi2cClock[instance], &inputClock);

    if (result == STATUS_SUCCESS)
    {
        g_lpi2cSlaveStates[instance] = slaveState;

        /* Initialize driver status structure */
        slaveState->transferType = slaveUserConfig->transferType;
        slaveState->rxSize = 0U;
        slaveState->rxBuffer = NULL;
        slaveState->txSize = 0U;
        slaveState->txBuffer = NULL;
        slaveState->isListening = slaveUserConfig->isListening;
        slaveState->slaveCallback = slaveUserConfig->slaveCallback;
        slaveState->callbackParam = slaveUserConfig->callbackParam;
        /* Store DMA channel number used in transfer */
        slaveState->dmaChannel = slaveUserConfig->dmaChannel;
        slaveState->isBusy = false;
        slaveState->isBlocking = false;
        slaveState->isAddrFor10bit = slaveUserConfig->isAddrFor10bit;
        slaveState->repeatedStarts = 0U;
        slaveState->status = STATUS_SUCCESS;

        /* Initialize the semaphore */
        OSIF_SemCreate(&(slaveState->idleSem), 0);

        /* Enable lpi2c interrupt */
        INT_SYS_EnableIRQ(g_lpi2cSlaveIrqId[instance]);

        /* Initialize module */
        LPI2C_HW_Init(lpi2cBase);

        /* Config slave address */
        LPI2C_HW_ConfigSlaveAddr0(lpi2cBase, slaveUserConfig->slaveAddr);
        if (!slaveUserConfig->isAddrFor10bit)
        {
            LPI2C_HW_ConfigSlaveAddr(lpi2cBase, LPI2C_SLAVE_ADDR_MATCH_0_7BIT);
        }
        else
        {
            LPI2C_HW_ConfigSlaveAddr(lpi2cBase, LPI2C_SLAVE_ADDR_MATCH_0_10BIT);
        }

        /* Config operating mode */
        LPI2C_SlaveConfigOperatingMode(instance, slaveUserConfig->i2cMode);

        if (slaveUserConfig->isListening)
        {
            if (slaveState->transferType == LPI2C_USE_INTERRUPTS)
            {
                /* Activate events */
                LPI2C_HW_ConfigSlaveInt(lpi2cBase, LPI2C_SLAVE_INT_BIT_ERROR |
                                                   LPI2C_SLAVE_INT_FIFO_ERROR |
                                                   LPI2C_SLAVE_INT_STOP_DETECT |
                                                   LPI2C_SLAVE_INT_REPEATED_START |
                                                   LPI2C_SLAVE_INT_ADDR_VALID |
                                                   LPI2C_SLAVE_INT_RX_DATA,
                                                    true);
            }
            if (slaveState->transferType == LPI2C_USE_DMA)
            {
                /* Activate events */
                LPI2C_HW_ConfigSlaveInt(lpi2cBase, LPI2C_SLAVE_INT_BIT_ERROR |
                                                   LPI2C_SLAVE_INT_FIFO_ERROR |
                                                   LPI2C_SLAVE_INT_STOP_DETECT |
                                                   LPI2C_SLAVE_INT_REPEATED_START |
                                                   LPI2C_SLAVE_INT_ADDR_VALID,
                                                    true);
            }

            /* Enable LPI2C slave */
            LPI2C_HW_ConfigSlaveEnable(lpi2cBase, true);
        }
    }
    return result;
}

/*!
 * @brief De-initialize the I2C slave mode driver
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveDeInit(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    /* Destroy the semaphore */
    (void)OSIF_SemDestroy(&(slaveState->idleSem));

    if ((slaveState->transferType == LPI2C_USE_DMA) && (slaveState->isListening))
    {
        /* Disable LPI2C DMA requests */
        (void)LPI2C_HW_ConfigSlaveTxDMA(lpi2cBase, false);
        (void)LPI2C_HW_ConfigSlaveRxDMA(lpi2cBase, false);
    }

    g_lpi2cSlaveStates[instance] = NULL;

    /* Disable LPI2C slave */
    LPI2C_HW_ConfigSlaveEnable(lpi2cBase, false);

    /* Disable i2c interrupt */
    INT_SYS_DisableIRQ(g_lpi2cSlaveIrqId[instance]);

    return STATUS_SUCCESS;
}

/*!
 * @brief This function provides a buffer in which the LPI2C slave-mode
 * driver can store received data. It can be called for example from the user callback
 * provided at initialization time, when the driver reports events RX_REQ or RX_FULL event.
 *
 * @param instance: LPI2C peripheral instance number
 * @param rxBuffer: pointer to the data to be transferred
 * @param rxSize: length in bytes of the data to be transferred
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveSetRxBuffer(uint32_t instance, uint8_t *rxBuffer, uint32_t rxSize)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    if (POINTER_IS_NULL(rxBuffer) == false)
    {
        if (POINTER_IS_NULL(slaveState) == false)
        {
            slaveState->rxSize = rxSize;
            slaveState->rxBuffer = rxBuffer;
        }
        else
        {
            result = STATUS_ERROR;
        }
    }
    else
    {
         result = STATUS_ERROR;
    }

    return result;
}

/*!
 * @brief This function provides a buffer from which the LPI2C slave-mode
 * driver can transmit data. It can be called for example from the user callback provided at
 * initialization time, when the driver reports events TX_REQ or TX_EMPTY event.
 *
 * @param instance: LPI2C peripheral instance number
 * @param txBuffer: pointer to the data to be transferred
 * @param txSize: length in bytes of the data to be transferred
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveSetTxBuffer(uint32_t instance, const uint8_t *txBuffer, uint32_t txSize)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    if (POINTER_IS_NULL(txBuffer) == false)
    {
        if (POINTER_IS_NULL(slaveState) == false)
        {
            slaveState->txSize = txSize;
            slaveState->txBuffer = txBuffer;
        }
        else
        {
            result = STATUS_ERROR;
        }
    }
    else
    {
         result = STATUS_ERROR;
    }

    return result;
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus when the slave
 * is not in listening mode.
 *
 * @param instance: LPI2C peripheral instance number
 * @param rxBuffer: pointer to the buffer where to store received data
 * @param rxSize: length in bytes of the data to be transferred
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveReceiveDataNonBlocking(uint32_t instance, uint8_t *rxBuffer, uint32_t rxSize)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_BUSY;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];
    uint32_t interrupts;

    if (!slaveState->isBusy)
    {
        slaveState->isBusy = true;
        slaveState->status = STATUS_BUSY;
        slaveState->rxBuffer = rxBuffer;
        slaveState->rxSize = rxSize;

        interrupts = LPI2C_SLAVE_INT_BIT_ERROR |
                     LPI2C_SLAVE_INT_FIFO_ERROR |
                     LPI2C_SLAVE_INT_STOP_DETECT |
                     LPI2C_SLAVE_INT_REPEATED_START |
                     LPI2C_SLAVE_INT_ADDR_VALID;

        if (slaveState->transferType != LPI2C_USE_DMA)
        {
            /* Activate interrupts */
            interrupts |= LPI2C_SLAVE_INT_RX_DATA;
            LPI2C_HW_ConfigSlaveInt(lpi2cBase, interrupts, true);

            /* Enable LPI2C slave */
            LPI2C_HW_ConfigSlaveEnable(lpi2cBase, true);
        }
        else
        {
            /* Activate interrupts */
            LPI2C_HW_ConfigSlaveInt(lpi2cBase, interrupts, true);

            /* Enable LPI2C slave */
            LPI2C_HW_ConfigSlaveEnable(lpi2cBase, true);

            /* Start DMA transfer */
            LPI2C_SlaveStartDmaTransfer(instance);
        }
    }
    return result;
}

/*!
 * @brief Perform a blocking receive transaction on the I2C bus when the slave
 * is not in listening mode.
 *
 * @param instance: LPI2C peripheral instance number
 * @param rxBuffer: pointer to the buffer where to store received data
 * @param rxSize: length in bytes of the data to be transferred
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveReceiveDataBlocking(
    uint32_t instance,
    uint8_t *rxBuffer,
    uint32_t rxSize,
    uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    /* Check if slave is idle */
    if (!slaveState->isBusy)
    {
        /* mark transfer as blocking */
        slaveState->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(slaveState->idleSem), 0);

        (void)LPI2C_SlaveReceiveDataNonBlocking(instance, rxBuffer, rxSize);

        /* Wait for transfer to end */
        result = LPI2C_SlaveWaitTransferEnd(instance, timeout);
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus when the slave
 * is not in listening mode.
 *
 * @param instance: LPI2C peripheral instance number
 * @param txBuffer: pointer to the data to be transferred
 * @param txSize: length in bytes of the data to be transferred
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveSendDataNonBlocking(uint32_t instance, const uint8_t *txBuffer, uint32_t txSize)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    /* Check if slave is idle */
    if (!slaveState->isBusy)
    {

        slaveState->txBuffer = txBuffer;
        slaveState->txSize = txSize;
        slaveState->status = STATUS_BUSY;

        if (slaveState->transferType != LPI2C_USE_DMA)
        {
            /* Activate events */
            LPI2C_HW_ConfigSlaveInt(lpi2cBase, LPI2C_SLAVE_INT_BIT_ERROR |
                                               LPI2C_SLAVE_INT_FIFO_ERROR |
                                               LPI2C_SLAVE_INT_STOP_DETECT |
                                               LPI2C_SLAVE_INT_REPEATED_START |
                                               LPI2C_SLAVE_INT_ADDR_VALID,
                                                true);

            /* Enable LPI2C slave */
            LPI2C_HW_ConfigSlaveEnable(lpi2cBase, true);

            slaveState->isBusy = true;
        }
        else
        {
            /* Activate events */
            LPI2C_HW_ConfigSlaveInt(lpi2cBase, LPI2C_SLAVE_INT_BIT_ERROR |
                                               LPI2C_SLAVE_INT_FIFO_ERROR |
                                               LPI2C_SLAVE_INT_STOP_DETECT |
                                               LPI2C_SLAVE_INT_REPEATED_START |
                                               LPI2C_SLAVE_INT_ADDR_VALID,
                                                true);

            /* Enable LPI2C slave */
            LPI2C_HW_ConfigSlaveEnable(lpi2cBase, true);

            slaveState->isBusy = true;

            LPI2C_SlaveStartDmaTransfer(instance);
        }
    }
    else
    {
        result = STATUS_BUSY;
    }
    return result;
}

/*!
 * @brief Perform a blocking send transaction on the I2C bus when the slave
 * is not in listening mode.
 *
 * @param instance: LPI2C peripheral instance number
 * @param txBuffer: pointer to the data to be transferred
 * @param txSize: length in bytes of the data to be transferred
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveSendDataBlocking(
    uint32_t instance,
    const uint8_t *txBuffer,
    uint32_t txSize,
    uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    STATUS_T result = STATUS_SUCCESS;
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    /* Check if slave is idle */
    if (!slaveState->isBusy)
    {
        /* mark transfer as blocking */
        slaveState->isBlocking = true;

        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemWait(&(slaveState->idleSem), 0);

        (void)LPI2C_SlaveSendDataNonBlocking(instance, txBuffer, txSize);

        /* Wait for transfer to end */
        result = LPI2C_SlaveWaitTransferEnd(instance, timeout);
    }
    else
    {
        result = STATUS_BUSY;
    }

    return result;
}

/*!
 * @brief   Read the current status of the I2C slave transfer
 * @details This function can be called during a non-blocking transmission to
 *          check the status of the transfer.
 *
 * @param instance: LPI2C peripheral instance number
 * @param bytesRemain: the number of remaining bytes in the active I2C transfer
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveGetTransferStatus(uint32_t instance, uint32_t *bytesRemain)
{
    ASSERT_INSTANCE(instance);

    const LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    if (  (POINTER_IS_NULL(bytesRemain) == false)
       && (slaveState->transferType == LPI2C_USE_INTERRUPTS))
    {
        if (slaveState->txSize > 0U)
        {
            /* Send data */
            *bytesRemain = slaveState->txSize;
        }
        else if (slaveState->rxSize > 0U)
        {
            /* Receive data */
            *bytesRemain = slaveState->rxSize;
        }
        else
        {
            *bytesRemain = 0U;
        }
    }

    return slaveState->status;
}

/*!
 * @brief Abort a non-blocking I2C Master transmission or reception
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval Error or success status returned by API
 */
STATUS_T LPI2C_SlaveAbortTransfer(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];

    if (slaveState->isListening == false)
    {
        slaveState->status = STATUS_I2C_ABORTED;
        LPI2C_SlaveEndTransfer(lpi2cBase, slaveState);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Initializate the default configuration structure for slave
 *
 * @param slaveUserConfig: Pointer to configuration structure
 *
 * @retval None
 */
void LPI2C_SlaveDefaultConfig(LPI2C_SLAVE_USER_CONFIG_T * slaveUserConfig)
{
    slaveUserConfig->i2cMode = LPI2C_STANDARD_MODE;
    slaveUserConfig->transferType  = LPI2C_USE_INTERRUPTS;
    slaveUserConfig->isAddrFor10bit   = false;
    slaveUserConfig->slaveAddr     = 32U;
    slaveUserConfig->dmaChannel    = 0U;
    slaveUserConfig->isListening   = true;
    slaveUserConfig->callbackParam = NULL;
    slaveUserConfig->slaveCallback = NULL;
}

/*!
 * @brief Handle slave operation when I2C interrupt occurs
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval None
 */
void LPI2C_SlaveIRQHandler(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];
    bool repeatStartDetect = false;
    bool stopDetect = false;

    /* Check for error event */
    if (LPI2C_HW_ReadSlaveBitErrorEvent(lpi2cBase))
    {
        slaveState->status = STATUS_ERROR;
        LPI2C_HW_ClearSlaveBitErrorEvent(lpi2cBase);

        /* Deactivate interrupts for transmitting data */
        LPI2C_HW_ConfigSlaveInt(lpi2cBase, (uint32_t)LPI2C_SLAVE_INT_TX_DATA, false);

        LPI2C_SlaveEndTransferHandler(slaveState, lpi2cBase);
    }
    else
    {
        /* Check for repeated start or stop condition */
        stopDetect = LPI2C_HW_ReadSlaveSTOPDetectEvent(lpi2cBase);
        repeatStartDetect = LPI2C_HW_ReadSlaveRepeatedStartEvent(lpi2cBase);

        if (repeatStartDetect)
        {
            slaveState->repeatedStarts++;

            if ((slaveState->repeatedStarts == 1U) && (slaveState->isAddrFor10bit))
            {
                repeatStartDetect = false;
                LPI2C_HW_ClearSlaveRepeatedStartEvent(lpi2cBase);
            }
        }

        if ((stopDetect != true) && (repeatStartDetect != true))
        {
            /* Check which event caused the interrupt */
            if (LPI2C_HW_ReadSlaveAddressValidEvent(lpi2cBase))
            {
                LPI2C_SlaveHandleAddressValidEvent(instance, lpi2cBase, slaveState);
            }

            if (LPI2C_HW_ReadSlaveTransmitDataEvent(lpi2cBase))
            {
                if (LPI2C_HW_ReadSlaveInt(lpi2cBase, (uint32_t)LPI2C_SLAVE_INT_TX_DATA))
                {
                    LPI2C_SlaveHandleTransmitDataEvent(lpi2cBase, slaveState);
                }
            }

            if (LPI2C_HW_ReadSlaveReceiveDataEvent(lpi2cBase))
            {
                if (LPI2C_HW_ReadSlaveInt(lpi2cBase, (uint32_t)LPI2C_SLAVE_INT_RX_DATA))
                {
                    LPI2C_SlaveHandleReceiveDataEvent(lpi2cBase, slaveState);
                }
            }
        }
        else
        {
            /* Either STOP or repeated START have the same meaning here: the current transfer is over */
            LPI2C_HW_ClearSlaveSTOPDetectEvent(lpi2cBase);
            LPI2C_HW_ClearSlaveRepeatedStartEvent(lpi2cBase);

            /* Deactivate interrupts for transmitting data */
            LPI2C_HW_ConfigSlaveInt(lpi2cBase, (uint32_t)LPI2C_SLAVE_INT_TX_DATA, false);

            if (slaveState->status == STATUS_BUSY)
            {
                /* Report success if no error was recorded */
                slaveState->status = STATUS_SUCCESS;
            }

            LPI2C_SlaveEndTransferHandler(slaveState, lpi2cBase);
        }
    }
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Read the size of the Master Receive FIFO
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval Master Receive FIFO Size
 */
uint16_t LPI2C_HW_ReadMasterRxFIFOSize(const LPI2C_T *lpi2cBase)
{
    uint32_t fifoSize = lpi2cBase->PARA.bit.MRXFS;

    /* RX FIFO size = 2 * (Master Receive FIFO Size) */
    fifoSize = 1U << fifoSize;
    return (uint16_t)fifoSize;
}

/*!
 * @brief Read the size of the Master Transmit FIFO
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval Master Transmit FIFO Size
 */
uint16_t LPI2C_HW_ReadMasterTxFIFOSize(const LPI2C_T *lpi2cBase)
{
    uint32_t fifoSize = lpi2cBase->PARA.bit.MTXFS;

    /* TX FIFO size = 2 * (Master Transmit FIFO Size) */
    fifoSize = 1U << fifoSize;
    return (uint16_t)fifoSize;
}

/*!
 * @brief Reset the master receive FIFO.
 *
 * @param lpi2cBase: base address of the LPI2C module.
 *
 * @retval None
 */
void LPI2C_HW_ResetMasterRxFIFO(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MCTRL.bit.RXFRST = LPI2C_MCTRL_RXFRST_1;
}

/*!
 * @brief Reset the master transmit FIFO.
 *
 * @param lpi2cBase: base address of the LPI2C module.
 *
 * @retval None
 */
void LPI2C_HW_ResetMasterTxFIFO(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MCTRL.bit.TXFRST = LPI2C_MCTRL_TXFRST_1;
}

/*!
 * @brief Set/clear the master reset command.
 *
 * @param lpi2cBase: base address of the LPI2C module.
 * @param enable: specifies the reset state of the LPI2C master logic.
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterSoftwareReset(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->MCTRL.bit.MRST = enable ? LPI2C_MCTRL_MRST_1 : LPI2C_MCTRL_MRST_0;
}

/*!
 * @brief Enable/disable the LPI2C master
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable the LPI2C master
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterEnable(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->MCTRL.bit.MMEN = enable ? LPI2C_MCTRL_MMEN_1 : LPI2C_MCTRL_MMEN_0;
}

/*!
 * @brief Indicate the availability of receive data
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval receive data ready/not ready
 */
bool LPI2C_HW_ReadMasterReceiveDataReadyEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->MSTS.bit.RXFLG;
}

/*!
 * @brief Indicate if the LPI2C master requests more data
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval  transmit data requested/not requested
 */
bool LPI2C_HW_ReadMasterTransmitDataRequestEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->MSTS.bit.TXFLG;
}

/*!
 * @brief Check the occurrence of a FIFO error event
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of a FIFO error event
 */
bool LPI2C_HW_ReadMasterFIFOErrorEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->MSTS.bit.FEFLG;
}

/*!
 * @brief Check the occurrence of an arbitration lost event
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of an arbitration lost event
 */
bool LPI2C_HW_ReadMasterArbitrationLostEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->MSTS.bit.MALFLG;
}

/*!
 * @brief Check the occurrence of an unexpected NACK event
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of an unexpected NACK event
 */
bool LPI2C_HW_ReadMasterNACKDetectEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->MSTS.bit.NACKFLG;
}

/*!
 * @brief Clear the FIFO error event flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearMasterFIFOErrorEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MSTS.bit.FEFLG = LPI2C_MSTS_FEFLG_0;
}

/*!
 * @brief Clear the arbitration lost event flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearMasterArbitrationLostEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MSTS.bit.MALFLG = LPI2C_MSTS_MALFLG_0;
}

/*!
 * @brief Clear the unexpected NACK event flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearMasterNACKDetectEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MSTS.bit.NACKFLG = LPI2C_MSTS_NACKFLG_0;
}

/*!
 * @brief Clear all status flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearMasterAllEvents(LPI2C_T *lpi2cBase)
{
    lpi2cBase->MSTS.bit.ENDFLG  = LPI2C_MSTS_ENDFLG_0;
    lpi2cBase->MSTS.bit.STOPFLG = LPI2C_MSTS_STOPFLG_0;
    lpi2cBase->MSTS.bit.NACKFLG = LPI2C_MSTS_NACKFLG_0;
    lpi2cBase->MSTS.bit.MALFLG  = LPI2C_MSTS_MALFLG_0;
    lpi2cBase->MSTS.bit.FEFLG   = LPI2C_MSTS_FEFLG_0;
    lpi2cBase->MSTS.bit.PLTFLG  = LPI2C_MSTS_PLTFLG_0;
    lpi2cBase->MSTS.bit.RXMFLG  = LPI2C_MSTS_RXMFLG_0;
}

/*!
 * @brief Enable/disable receive data DMA requests
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable DMA requests
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterRxDMA(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->MDMAEN.bit.RXDMAEN = enable ? LPI2C_MDMAEN_RXDMAEN_1 : LPI2C_MDMAEN_RXDMAEN_0;
}

/*!
 * @brief Enable/disable transmit data DMA requests
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable DMA requests
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterTxDMA(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->MDMAEN.bit.TXDMAEN = enable ? LPI2C_MDMAEN_TXDMAEN_1 : LPI2C_MDMAEN_TXDMAEN_0;
}

/*!
 * @brief Enable/disable specified LPI2C master interrupts
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param interrupts: interrupts to be enabled or disabled;
 *  must be a bitwise or between one or more of the following constants:
 *    - LPI2C_MASTER_INT_TX_DATA       - Transmit Data Interrupt
 *    - LPI2C_MASTER_INT_RX_DATA        - Receive Data Interrupt
 *    - LPI2C_MASTER_INT_END_PACKET          - End Packet Interrupt
 *    - LPI2C_MASTER_INT_STOP_DETECT         - STOP Detect Interrupt
 *    - LPI2C_MASTER_INT_NACK_DETECT         - NACK Detect Interrupt
 *    - LPI2C_MASTER_INT_ARBITRATION_LOST    - Arbitration Lost Interrupt
 *    - LPI2C_MASTER_INT_FIFO_ERROR          - FIFO Error Interrupt
 *    - LPI2C_MASTER_INT_PIN_LOW_TIMEOUT     - Pin Low Timeout Interrupt
 *    - LPI2C_MASTER_INT_DATA_MATCH          - Data Match Interrupt
 *
 * @param enable: specifies whether to enable or disable specified interrupts
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterInt(LPI2C_T *lpi2cBase, uint32_t interrupts, bool enable)
{
    uint32_t temp = lpi2cBase->MINTEN.reg;

    if (enable)
    {
        temp |= (uint32_t)interrupts;
    }
    else
    {
        temp &= ~(uint32_t)interrupts;
    }
    lpi2cBase->MINTEN.reg = temp;
}

/*!
 * @brief Config the master pin mode of the module
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param pinConfig: pin mode of the module
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterPinMode(LPI2C_T *lpi2cBase, LPI2C_PIN_CFG_T pinConfig)
{
    lpi2cBase->MCFG1.bit.PINMCFG = (uint32_t)pinConfig;
}

/*!
 * @brief Config the reaction of the module on NACK reception
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param nackConfig: set reaction of the module on NACK reception
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterNACKMode(LPI2C_T *lpi2cBase, LPI2C_MASTER_NACK_CFG_T nackConfig)
{
    lpi2cBase->MCFG1.bit.INACK = (uint32_t)nackConfig;
}

/*!
 * @brief Config the LPI2C master prescaler
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param prescaler: LPI2C master prescaler
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterPrescaler(LPI2C_T *lpi2cBase, LPI2C_MASTER_PSC_T prescaler)
{
    lpi2cBase->MCFG1.bit.CLKPS = (uint32_t)prescaler;
}

/*!
 * @brief Read the LPI2C master prescaler
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval LPI2C master prescaler
 */
LPI2C_MASTER_PSC_T LPI2C_HW_ReadMasterPrescaler(const LPI2C_T *lpi2cBase)
{
    return (LPI2C_MASTER_PSC_T)lpi2cBase->MCFG1.bit.CLKPS;
}

/*!
 * @brief Read the configured minimum SCL high period
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval minimum SCL high period
 */
uint8_t LPI2C_HW_ReadMasterSCLHighPeriod(const LPI2C_T *lpi2cBase)
{
    return (uint8_t)lpi2cBase->MCLKCFG0.bit.SCLHC;
}

/*!
 * @brief Config the minimum SCL high period
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: minimum SCL high period
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterSCLHighPeriod(LPI2C_T *lpi2cBase, uint8_t value)
{
    lpi2cBase->MCLKCFG0.bit.SCLHC = (uint32_t)value;
}

/*!
 * @brief Config the data hold time for SDA
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: value of the data hold time for SDA
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterSDAHoldTime(LPI2C_T *lpi2cBase, uint8_t value)
{
    lpi2cBase->MCLKCFG0.bit.SDAHT = (uint32_t)value;
}

/*!
 * @brief Config the setup and hold delay for a START/STOP condition
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: setup and hold time for a START/STOP condition
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterSetupHoldDelay(LPI2C_T *lpi2cBase, uint8_t value)
{
    lpi2cBase->MCLKCFG0.bit.SETUPT = (uint32_t)value;
}

/*!
 * @brief Config the minimum SCL low period
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: minimum SCL low period
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterSCLLowPeriod(LPI2C_T *lpi2cBase, uint8_t value)
{
    lpi2cBase->MCLKCFG0.bit.SCLLC = (uint32_t)value;
}

/*!
 * @brief Read the configured minimum SCL low period
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval minimum SCL low period
 */
uint8_t LPI2C_HW_ReadMasterSCLLowPeriod(const LPI2C_T *lpi2cBase)
{
    return (uint8_t)lpi2cBase->MCLKCFG0.bit.SCLLC;
}

/*!
 * @brief Config the receive FIFO watermark
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: number of words in the receive FIFO that
 *               will cause the receive data flag to be set
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterRxFIFOWatermark(LPI2C_T *lpi2cBase, uint16_t value)
{
    lpi2cBase->MFCTRL.bit.RXFW = (uint32_t)value;
}

/*!
 * @brief Read the configured receive FIFO watermark
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval number of words in the receive FIFO that
 *         will cause the receive data flag to be set
 */
uint16_t LPI2C_HW_ReadMasterRxFIFOWatermark(const LPI2C_T *lpi2cBase)
{
    return (uint16_t)lpi2cBase->MFCTRL.bit.RXFW;
}

/*!
 * @brief Config the transmit FIFO watermark
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param value: number of words in the transmit FIFO that
 *               will cause the transmit data flag to be set
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterTxFIFOWatermark(LPI2C_T *lpi2cBase, uint16_t value)
{
    lpi2cBase->MFCTRL.bit.TXFW = (uint32_t)value;
}

/*!
 * @brief Read the number of words in the receive FIFO
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval the number of words in the receive FIFO
 */
uint16_t LPI2C_HW_ReadMasterRxFIFOCount(const LPI2C_T *lpi2cBase)
{
    return (uint16_t)lpi2cBase->MFSTS.bit.RXCNT;
}

/*!
 * @brief Read the number of words in the transmit FIFO
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval the number of words in the transmit FIFO
 */
uint16_t LPI2C_HW_ReadMasterTxFIFOCount(const LPI2C_T *lpi2cBase)
{
    return (uint16_t)lpi2cBase->MFSTS.bit.TXCNT;
}

/*!
 * @brief Provide commands and data for the LPI2C master
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param cmd: command for the LPI2C master
 * @param data: data for the LPI2C master
 *
 * @retval None
 */
void LPI2C_HW_MasterTransmitCmdData(LPI2C_T *lpi2cBase, LPI2C_MASTER_CMD_T cmd, uint8_t data)
{
    lpi2cBase->MTXDATA.reg = ((uint32_t)cmd << 8U) + (uint32_t)data;
}

/*!
 * @brief Read the received data
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval data received by the LPI2C master
 */
uint8_t LPI2C_HW_ReadMasterRxData(const LPI2C_T *lpi2cBase)
{
    return (uint8_t)lpi2cBase->MRXDATA.bit.RXDATA;
}

/*!
 * @brief Enable/disable the LPI2C slave
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable the LPI2C slave
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveEnable(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCTRL.bit.SMEN = enable ? LPI2C_SCTRL_SMEN_1 : LPI2C_SCTRL_SMEN_0;
}

/*!
 * @brief Set/clear the slave reset command
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies the reset state of the LPI2C slave logic
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveSoftwareReset(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCTRL.bit.SRST = enable ? LPI2C_SCTRL_SRST_1 : LPI2C_SCTRL_SRST_0;
}

/*!
 * @brief Check the detection of a bit error
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of a bit error
 */
bool LPI2C_HW_ReadSlaveBitErrorEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.BEFLG;
}

/*!
 * @brief Check the detection of a STOP condition
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of a STOP condition
 */
bool LPI2C_HW_ReadSlaveSTOPDetectEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.STOPFLG;
}

/*!
 * @brief Check the detection of a repeated START condition
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of a repeated START condition
 */
bool LPI2C_HW_ReadSlaveRepeatedStartEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.RSFLG;
}

/*!
 * @brief Check the validity of the Address Status Register
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of the validity of the Address Status Register
 */
bool LPI2C_HW_ReadSlaveAddressValidEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.AVFLG;
}

/*!
 * @brief Check the availability of receive data
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of receive data availability
 */
bool LPI2C_HW_ReadSlaveReceiveDataEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.RXFLG;
}

/*!
 * @brief Check if transmit data is requested
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval indication of a transmit data request
 */
bool LPI2C_HW_ReadSlaveTransmitDataEvent(const LPI2C_T *lpi2cBase)
{
    return (bool)lpi2cBase->SSTS.bit.TXFLG;
}

/*!
 * @brief Clear bit error flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearSlaveBitErrorEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->SSTS.bit.BEFLG = LPI2C_SSTS_BEFLG_1;
}

/*!
 * @brief Clear the STOP detect flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearSlaveSTOPDetectEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->SSTS.bit.STOPFLG = LPI2C_SSTS_STOPFLG_1;
}

/*!
 * @brief Clear the repeated START detect flag
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_ClearSlaveRepeatedStartEvent(LPI2C_T *lpi2cBase)
{
    lpi2cBase->SSTS.bit.RSFLG = LPI2C_SSTS_RSFLG_1;
}

/*!
 * @brief Enable/disable specified LPI2C slave interrupts
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param interrupts: interrupts to be enabled or disabled;
 *  must be a bitwise or between one or more of the following constants:
 *    - LPI2C_SLAVE_TRANSMIT_DATA         - Transmit Data Interrupt
 *    - LPI2C_SLAVE_RECEIVE_DATA          - Receive Data Interrupt
 *    - LPI2C_SLAVE_ADDRESS_VALID         - Address Valid Interrupt
 *    - LPI2C_SLAVE_TRANSMIT_ACK          - Transmit ACK Interrupt
 *    - LPI2C_SLAVE_REPEATED_START        - Repeated Start Interrupt
 *    - LPI2C_SLAVE_STOP_DETECT           - STOP Detect Interrupt
 *    - LPI2C_SLAVE_BIT_ERROR             - Bit Error Interrupt
 *    - LPI2C_SLAVE_FIFO_ERROR            - FIFO Error Interrupt
 *    - LPI2C_SLAVE_ADDRESS_MATCH_0       - Address Match 0 Interrupt
 *    - LPI2C_SLAVE_ADDRESS_MATCH_1       - Address Match 1 Interrupt
 *    - LPI2C_SLAVE_GENERAL_CALL          - General Call Interrupt
 *    - LPI2C_SLAVE_SMBUS_ALERT_RESPONSE  - SMBus Alert Response Interrupt
 *
 * @param enable: specifies whether to enable or disable specified interrupts
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveInt(LPI2C_T *lpi2cBase, uint32_t interrupts, bool enable)
{
    uint32_t intEnable = lpi2cBase->SINTEN.reg;

    if (enable)
    {
        intEnable |= (uint32_t)interrupts;
    }
    else
    {
        intEnable &= ~(uint32_t)interrupts;
    }
    lpi2cBase->SINTEN.reg = intEnable;
}

/*!
 * @brief Read the state of the specified LPI2C slave interrupt
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param interrupts: interrupt for which the check is made;
 *  must be one of the following constants:
 *    - LPI2C_SLAVE_TRANSMIT_DATA         - Transmit Data Interrupt
 *    - LPI2C_SLAVE_RECEIVE_DATA          - Receive Data Interrupt
 *    - LPI2C_SLAVE_ADDRESS_VALID         - Address Valid Interrupt
 *    - LPI2C_SLAVE_TRANSMIT_ACK          - Transmit ACK Interrupt
 *    - LPI2C_SLAVE_REPEATED_START        - Repeated Start Interrupt
 *    - LPI2C_SLAVE_STOP_DETECT           - STOP Detect Interrupt
 *    - LPI2C_SLAVE_BIT_ERROR             - Bit Error Interrupt
 *    - LPI2C_SLAVE_FIFO_ERROR            - FIFO Error Interrupt
 *    - LPI2C_SLAVE_ADDRESS_MATCH_0       - Address Match 0 Interrupt
 *    - LPI2C_SLAVE_ADDRESS_MATCH_1       - Address Match 1 Interrupt
 *    - LPI2C_SLAVE_GENERAL_CALL          - General Call Interrupt
 *    - LPI2C_SLAVE_SMBUS_ALERT_RESPONSE  - SMBus Alert Response Interrupt
 *
 * @retval enable/disable state of specified interrupt
 */
bool LPI2C_HW_ReadSlaveInt(const LPI2C_T *lpi2cBase, uint32_t interrupts)
{
    uint32_t intEnable = lpi2cBase->SINTEN.reg;
    bool hasInterrupts = false;

    if ((intEnable & interrupts) != (uint32_t)0U)
    {
        hasInterrupts = true;
    }

    return hasInterrupts;
}

/*!
 * @brief Enable/disable slave receive data DMA requests
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable receive data DMA requests
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveRxDMA(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SDMAEN.bit.RXDMAEN = enable ? LPI2C_SDMAEN_RXDMAEN_1 : LPI2C_SDMAEN_RXDMAEN_0;
}

/*!
 * @brief Enable/disable slave transmit data DMA requests
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: specifies whether to enable or disable transmit data DMA requests
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveTxDMA(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SDMAEN.bit.TXDMAEN = enable ? LPI2C_SDMAEN_TXDMAEN_1 : LPI2C_SDMAEN_TXDMAEN_0;
}

/*!
 * @brief Config slave control address match configuration
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param addrConfig: configures the condition that will cause an address to match
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveAddr(LPI2C_T *lpi2cBase, LPI2C_SLAVE_ADDR_CFG_T addrConfig)
{
    lpi2cBase->SCFG1.bit.ADDRMCFG = (uint32_t)addrConfig;
}

/*!
 * @brief Control detection of the High-Speed Mode master code
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: enable/disable the detection of the High-Speed Mode master code
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveHighSpeedModeDetect(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCFG1.bit.HSMDEN = enable ? LPI2C_SCFG1_HSMDEN_1 : LPI2C_SCFG1_HSMDEN_0;
}

/*!
 * @brief Control slave behaviour when NACK is detected
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param nackConfig: slave behaviour when NACK is detected
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveIgnoreNACK(LPI2C_T *lpi2cBase, LPI2C_SLAVE_NACK_CFG_T nackConfig)
{
    lpi2cBase->SCFG1.bit.INACK = (uint32_t)nackConfig;
}

/*!
 * @brief Enable/disable clock stretching for the sending of the ACK bit
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: enable or disable clock stretching
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveACKStall(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCFG1.bit.ACKSEN = enable ? LPI2C_SCFG1_ACKSEN_1 : LPI2C_SCFG1_ACKSEN_0;
}

/*!
 * @brief Enable/disable clock stretching for data transmission
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: enable or disable clock stretching
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveTXDStall(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCFG1.bit.TXDSEN = enable ? LPI2C_SCFG1_TXDSEN_1 : LPI2C_SCFG1_TXDSEN_0;
}

/*!
 * @brief Enable/disable clock stretching for data reception
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: enable or disable clock stretching
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveRXStall(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCFG1.bit.RXDSEN = enable ? LPI2C_SCFG1_RXDSEN_1 : LPI2C_SCFG1_RXDSEN_0;
}

/*!
 * @brief Enable/disable clock stretching for valid address reception
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param enable: enable or disable clock stretching
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveAddrStall(LPI2C_T *lpi2cBase, bool enable)
{
    lpi2cBase->SCFG1.bit.ADDRSEN = enable ? LPI2C_SCFG1_ADDRSEN_1 : LPI2C_SCFG1_ADDRSEN_0;
}

/*!
 * @brief Config the ADDR0 address for slave address match
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param address0: ADDR0 address for slave address match
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveAddr0(LPI2C_T *lpi2cBase, uint16_t address0)
{
    lpi2cBase->SADDRM.bit.A0CV = (uint32_t)address0;
}

/*!
 * @brief Read the received slave address
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval received address
 */
uint16_t LPI2C_HW_ReadSlaveReceivedAddr(const LPI2C_T *lpi2cBase)
{
    return (uint16_t)lpi2cBase->SADDRSTS.bit.RXADDR;
}

/*!
 * @brief Config the ACK/NACK transmission after a received byte
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param ackTransmit: specifies whether to transmit ACK or NACK
 *
 * @retval None
 */
void LPI2C_HW_ConfigSlaveTransmitNACK(LPI2C_T *lpi2cBase, LPI2C_SLAVE_NACK_TRANSMIT_T ackTransmit)
{
    lpi2cBase->STXACK.bit.TXNACK = (uint32_t)ackTransmit;
}

/*!
 * @brief Provide data for the LPI2C slave transmitter
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param data: data for the LPI2C slave transmitter
 *
 * @retval None
 */
void LPI2C_HW_TxSlaveData(LPI2C_T *lpi2cBase, uint8_t data)
{
    lpi2cBase->STXDATA.reg = (uint32_t)data;
}

/*!
 * @brief Read the data received by the LPI2C slave receiver
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval data received by the LPI2C slave receiver
 */
uint8_t LPI2C_HW_RxSlaveData(const LPI2C_T *lpi2cBase)
{
    return (uint8_t)lpi2cBase->SRXDATA.reg;
}

/*!
 * @brief Config the timeout for bus idle for Master
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param timeout: bus idle timeout period. Zero means no bus idle timeout
 *
 * @retval None
 */
void LPI2C_HW_ConfigMasterBusIdleTimeoutValue(LPI2C_T *lpi2cBase, uint16_t timeout)
{
    lpi2cBase->MCFG2.bit.BITO = (uint32_t)timeout;
}

/*!
 * @brief Initializes all the registers of the LPI2C module to their reset value.
 *
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
void LPI2C_HW_Init(LPI2C_T *lpi2cBase)
{
    LPI2C_HW_ConfigMasterSoftwareReset(lpi2cBase, true);
    LPI2C_HW_ConfigSlaveSoftwareReset(lpi2cBase, true);

    lpi2cBase->MCTRL.reg = 0x0U;
    lpi2cBase->SCTRL.reg = 0x0U;
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Checks if there are any commands in the master software queue
 *
 * @param master: Pointer to the LPI2C master driver context structure.
 *
 * @retval the master software queue is empty/not empty
 */
static bool LPI2C_MasterCmdQueueIsEmpty(const LPI2C_MASTER_STATE_T *masterState)
{
    return (masterState->cmdQueue.writeIndex == masterState->cmdQueue.readIndex);
}

/*!
 * @brief resets the master software queue
 *
 * @param masterState: Pointer to the LPI2C master driver context structure.
 *
 * @retval None
 */
static void LPI2C_MasterQueueReset(LPI2C_MASTER_STATE_T *masterState)
{
    masterState->cmdQueue.readIndex = 0U;
    masterState->cmdQueue.writeIndex = 0U;
}

/*!
 * @brief queues a command in the hardware FIFO or in the master software queue
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure.
 * @param cmd: command for the LPI2C master
 * @param data: data for the LPI2C master
 *
 * @retval None
 */
static void LPI2C_MasterQueueCmd(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    LPI2C_MASTER_CMD_T cmd,
    uint8_t data)
{
    uint16_t fifoCount = LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase);
    uint16_t fifoSize = LPI2C_HW_ReadMasterTxFIFOSize(lpi2cBase);

    /* Check if there is room in the hardware FIFO */
    if (fifoCount >= fifoSize)
    {
        masterState->cmdQueue.cmd[masterState->cmdQueue.writeIndex] = cmd;
        masterState->cmdQueue.data[masterState->cmdQueue.writeIndex] = data;
        masterState->cmdQueue.writeIndex++;
    }
    else
    {
        LPI2C_HW_MasterTransmitCmdData(lpi2cBase, cmd, data);
    }
}

/*!
 * @brief transfers commands from the master software queue to the hardware FIFO
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure.
 *
 * @retval None
 */
static void LPI2C_MasterSendQueuedCmd(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState)
{
    uint16_t fifoCount = LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase);
    uint16_t fifoSize = LPI2C_HW_ReadMasterTxFIFOSize(lpi2cBase);

    while (  (!LPI2C_MasterCmdQueueIsEmpty(masterState))
          && (fifoCount < fifoSize))
    {
        LPI2C_HW_MasterTransmitCmdData(
            lpi2cBase,
            masterState->cmdQueue.cmd[masterState->cmdQueue.readIndex],
            masterState->cmdQueue.data[masterState->cmdQueue.readIndex]);

        fifoCount = LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase);
        masterState->cmdQueue.readIndex++;
    }

    if (LPI2C_MasterCmdQueueIsEmpty(masterState))
    {
        /* Reset queue */
        LPI2C_MasterQueueReset(masterState);
    }
}

/*!
 * @brief send start event and slave address
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure.
 * @param isReceive: specifies the direction of the transfer
 *
 * @retval None
 */
static void LPI2C_MasterSendAddress(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    bool isReceive)
{
    uint8_t address;
    LPI2C_MASTER_CMD_T masterCmd;
    /* Normal START command */
    masterCmd = LPI2C_MASTER_CMD_START;

    if (!masterState->isAddrFor10bit)
    {
        /* 7-bit addressing */
        /* Address byte: slave 7-bit address + D = 0(transmit) or 1 (receive) */
        address = (uint8_t)((masterState->slaveAddr << 1U) + (uint8_t)isReceive);
        LPI2C_MasterQueueCmd(lpi2cBase, masterState, masterCmd, address);
    }
    else
    {
        /* 10-bit addressing */
        /* First address byte: 1111 0XXD, where XX are bits 10 and 9 of address, and D = 0(transmit) */
        address = (uint8_t)(0xF0U + ((masterState->slaveAddr >> 7U) & 0x6U) + (uint8_t)0U);
        LPI2C_MasterQueueCmd(lpi2cBase, masterState, masterCmd, address);

        /* Second address byte: Remaining 8 bits of address */
        address = (uint8_t)(masterState->slaveAddr & 0xFFU);
        LPI2C_MasterQueueCmd(lpi2cBase, masterState, LPI2C_MASTER_CMD_TRANSMIT, address);

        if (isReceive)
        {
            /* Receiving from 10-bit slave - must send repeated start and resend first address byte */
            /* First address byte: 1111 0XXD, where XX are bits 10 and 9 of address, and D = 1 (receive) */
            address = (uint8_t)(0xF0U + ((masterState->slaveAddr >> 7U) & 0x6U) + (uint8_t)1U);
            LPI2C_MasterQueueCmd(lpi2cBase, masterState, masterCmd, address);
        }
    }
}

/*!
 * @brief queues transmit data in the LPI2C tx fifo until it is full
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure.
 *
 * @retval None
 */
static void LPI2C_MasterQueueData(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState)
{
    uint16_t fifoCount = LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase);
    uint16_t fifoSize = LPI2C_HW_ReadMasterTxFIFOSize(lpi2cBase);

    /* Don't queue any data if there are commands in the software queue */
    if (LPI2C_MasterCmdQueueIsEmpty(masterState))
    {
        while ((masterState->txSize > 0U) && (fifoCount < fifoSize))
        {
            LPI2C_HW_MasterTransmitCmdData(
                lpi2cBase,
                LPI2C_MASTER_CMD_TRANSMIT,
                *masterState->txBuffer);

            fifoCount = LPI2C_HW_ReadMasterTxFIFOCount(lpi2cBase);

            masterState->txBuffer++;
            masterState->txSize--;
        }
    }
}

/*!
 * @brief ends current transmission or reception
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure
 * @param isSendStop: send stop
 * @param isResetFIFO: reset FIFO
 *
 * @retval None
 */
static void LPI2C_MasterEndTransfer(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState,
    bool isSendStop,
    bool isResetFIFO)
{
    /* Disable all interrupts */
    LPI2C_HW_ConfigMasterInt(
        lpi2cBase,
        LPI2C_MASTER_INT_FIFO_ERROR |
        LPI2C_MASTER_INT_ARBITRATION_LOST |
        LPI2C_MASTER_INT_NACK_DETECT |
        LPI2C_MASTER_INT_TX_DATA |
        LPI2C_MASTER_INT_RX_DATA,
        false);

    /* Stop DMA channel and DMA request */
    if (masterState->transferType == LPI2C_USE_DMA)
    {
        (void)DMA_StopChannel(masterState->dmaChannel);
        if (masterState->rxSize == 0U)
        {
            LPI2C_HW_ConfigMasterTxDMA(lpi2cBase, false);
        }
        else
        {
            LPI2C_HW_ConfigMasterRxDMA(lpi2cBase, false);
        }
    }

    if (isResetFIFO)
    {
        LPI2C_HW_ResetMasterTxFIFO(lpi2cBase);
        LPI2C_HW_ResetMasterRxFIFO(lpi2cBase);
    }

    if (isSendStop)
    {
        /* Queue STOP command if requested */
        LPI2C_HW_MasterTransmitCmdData(lpi2cBase, LPI2C_MASTER_CMD_STOP, 0U);
    }

    masterState->rxSize = 0;
    masterState->txSize = 0;
    masterState->rxBuffer = NULL;
    masterState->txBuffer = NULL;
    masterState->isIdle = true;
}

/*!
 * @brief ends current transmission or reception
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param slaveState: Pointer to the LPI2C slave driver context structure
 *
 * @retval None
 */
static void LPI2C_SlaveEndTransfer(
    LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState)
{
    /* Deactivate interrupts */
    LPI2C_HW_ConfigSlaveInt(
        lpi2cBase,
        LPI2C_SLAVE_INT_BIT_ERROR |
        LPI2C_SLAVE_INT_FIFO_ERROR |
        LPI2C_SLAVE_INT_STOP_DETECT |
        LPI2C_SLAVE_INT_REPEATED_START |
        LPI2C_SLAVE_INT_ADDR_VALID |
        LPI2C_SLAVE_INT_RX_DATA |
        LPI2C_SLAVE_INT_TX_DATA,
        false);

    /* Disable the DMA request */
    if (slaveState->transferType == LPI2C_USE_DMA)
    {
        if (slaveState->rxSize == 0U)
        {
            LPI2C_HW_ConfigSlaveTxDMA(lpi2cBase, false);
        }
        else
        {
            LPI2C_HW_ConfigSlaveRxDMA(lpi2cBase, false);
        }
    }

    LPI2C_HW_ConfigSlaveEnable(lpi2cBase, false);
    slaveState->repeatedStarts = 0U;
    slaveState->rxSize = 0U;
    slaveState->txSize = 0U;
    slaveState->rxBuffer = NULL;
    slaveState->txBuffer = NULL;
    slaveState->isBusy = false;
}

/*!
 * @brief handle slave end transfer operations
 *
 * @param slaveState: Pointer to the LPI2C slave driver context structure
 * @param lpi2cBase: base address of the LPI2C module
 *
 * @retval None
 */
static void LPI2C_SlaveEndTransferHandler(
    LPI2C_SLAVE_STATE_T *slaveState,
    LPI2C_T *lpi2cBase)
{
    /* Stop DMA channel if slave is transferring data in DMA mode */
    if (slaveState->transferType == LPI2C_USE_DMA)
    {
        (void)DMA_StopChannel(slaveState->dmaChannel);
    }

    if (!slaveState->isListening)
    {
        LPI2C_SlaveEndTransfer(lpi2cBase, slaveState);

        /* Signal transfer end for blocking transfers */
        if (slaveState->isBlocking == true)
        {
            (void)OSIF_SemPost(&(slaveState->idleSem));
        }
    }

    if (slaveState->slaveCallback != NULL)
    {
        slaveState->slaveCallback(I2C_SLAVE_EVENT_STOP,
                                  slaveState->callbackParam);
    }
}

/*!
 * @brief sets the operating mode of the I2C master
 *
 * @param instance: LPI2C peripheral instance number
 * @param i2cMode: I2C operating mode
 *
 * @retval None
 */
static void LPI2C_MasterConfigOperatingMode(
    uint32_t instance,
    LPI2C_OPERATION_MODE_T i2cMode)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    masterState->i2cMode = i2cMode;

    LPI2C_HW_ConfigMasterPinMode(lpi2cBase, LPI2C_2PIN_OPEN_DRAIN);
    LPI2C_HW_ConfigMasterNACKMode(lpi2cBase, LPI2C_MASTER_NACK_RECEIVE);
}

/*!
 * @brief sets the operating mode of the I2C slave
 *
 * @param instance: LPI2C peripheral instance number
 * @param i2cMode: I2C operating mode
 *
 * @retval None
 */
static void LPI2C_SlaveConfigOperatingMode(
    uint32_t instance,
    LPI2C_OPERATION_MODE_T i2cMode)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    slaveState->i2cMode = i2cMode;

    /**
     * Enable clock stretching except ACKSTALL (we don't need to send
     * ACK/NACK manually).
     */
    LPI2C_HW_ConfigSlaveRXStall(lpi2cBase, true);
    LPI2C_HW_ConfigSlaveTXDStall(lpi2cBase, true);
    LPI2C_HW_ConfigSlaveACKStall(lpi2cBase, false);
    LPI2C_HW_ConfigSlaveAddrStall(lpi2cBase, true);
    LPI2C_HW_ConfigSlaveTransmitNACK(lpi2cBase, LPI2C_SLAVE_TRANSMIT_ACK);
    LPI2C_HW_ConfigSlaveIgnoreNACK(lpi2cBase, LPI2C_SLAVE_NACK_TRANSFER_END);

    /* Disable detection of the High-Speed Mode master */
    LPI2C_HW_ConfigSlaveHighSpeedModeDetect(lpi2cBase, false);

}

/*!
 * @brief config the DMA transfer
 *
 * @param instance: LPI2C peripheral instance number
 * @param dmaTransParams: DMA transfer parameters
 *
 * @retval None
 */
static void LPI2C_ConfigDmaTransfer(
    uint32_t instance,
    const LPI2C_DMA_TRANSFER_PARAMS_T *dmaTransParams)
{
    ASSERT_INSTANCE(instance);

    /* Config DMA channel */
    if (dmaTransParams->transferDir == LPI2C_REQUEST_RX)
    {
        (void)DMA_ConfigChannelRequestAndTrigger(
            dmaTransParams->dmaChannel,
            g_lpi2cDMARequestSources[instance][LPI2C_REQUEST_RX],
            false);

        (void)DMA_ConfigMultiBlockTransfer(
            dmaTransParams->dmaChannel,
            dmaTransParams->transferType,
            (uint32_t)dmaTransParams->regAddrOfData,
            (uint32_t)dmaTransParams->transferBuffer,
            DMA_TRANSFER_SIZE_1B, (uint32_t)1U,
            (uint32_t)dmaTransParams->transferSize,
            false);
    }
    else
    {

        (void)DMA_ConfigChannelRequestAndTrigger(
            dmaTransParams->dmaChannel,
            g_lpi2cDMARequestSources[instance][LPI2C_REQUEST_TX],
            false);

        (void)DMA_ConfigMultiBlockTransfer(
            dmaTransParams->dmaChannel,
            dmaTransParams->transferType,
            (uint32_t)dmaTransParams->transferBuffer,
            (uint32_t)dmaTransParams->regAddrOfData,
            DMA_TRANSFER_SIZE_1B, (uint32_t)1U,
            (uint32_t)dmaTransParams->transferSize,
            false);
    }
}

/*!
 * @brief Finish up a transfer DMA for master.
 *
 * @param parameter: parameter pointer.
 * @param status: DMA channel status
 *
 * @retval None
 */
static void LPI2C_MasterCompleteDmaTransfer(void *parameter, DMA_CHANNEL_STATUS_T status)
{
    uint32_t instance = (uint32_t)parameter;
    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];

    if ((masterState->txSize > 0U) && (status != DMA_CHANNEL_ERROR))
    {
        masterState->txSize = 0U;

        LPI2C_HW_ConfigMasterTxFIFOWatermark(lpi2cBase, 0U);

        /* Disable transmit data DMA requests */
        (void)LPI2C_HW_ConfigMasterTxDMA(lpi2cBase, false);

        /* Activate transmit data events */
        LPI2C_HW_ConfigMasterInt(lpi2cBase, (uint32_t)LPI2C_MASTER_INT_TX_DATA, true);
    }
    else
    {
        /* Signal transfer end for blocking transfers */
        LPI2C_MasterEndTransfer(lpi2cBase, masterState, masterState->isSendStop, false);

        if (masterState->isBlocking == true)
        {
            (void)OSIF_SemPost(&(masterState->idleSem));
        }

        /* Report status error if an error occurred in DMA channel */
        masterState->status = (status != DMA_CHANNEL_ERROR) ? STATUS_SUCCESS : STATUS_ERROR;

        if (masterState->masterCallback != NULL)
        {
            masterState->masterCallback(I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                                        masterState->callbackParam);
        }
    }
}

/*!
 * @brief starts the DMA transfer for master
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval None
 */
static void LPI2C_MasterStartDmaTransfer(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    LPI2C_DMA_TRANSFER_PARAMS_T dmaTransParams;
    bool receive = false;

    dmaTransParams.dmaChannel = masterState->dmaChannel;
    if (masterState->txSize == 0U)
    {
        /* Config watermarks for receive DMA for master */
        LPI2C_HW_ConfigMasterRxFIFOWatermark(lpi2cBase, 0U);

        receive = true;
        dmaTransParams.transferDir = LPI2C_REQUEST_RX;
        dmaTransParams.transferBuffer = masterState->rxBuffer;
        dmaTransParams.transferSize = masterState->rxSize;
        dmaTransParams.transferType = DMA_TRANSFER_PERIPH2MEM;
        dmaTransParams.regAddrOfData = (uint32_t)(&(lpi2cBase->MRXDATA.reg));
    }
    else
    {
        /* Config watermarks for transmit DMA for master */
        uint32_t txBytes = LPI2C_HW_ReadMasterTxFIFOSize(lpi2cBase);
        txBytes = (txBytes > masterState->txSize) ? masterState->txSize : txBytes;
        LPI2C_HW_ConfigMasterTxFIFOWatermark(lpi2cBase, (uint16_t)(txBytes - 1U));

        dmaTransParams.transferDir = LPI2C_REQUEST_TX;
        dmaTransParams.transferBuffer = (uint8_t *)masterState->txBuffer;
        dmaTransParams.transferSize = masterState->txSize;
        dmaTransParams.transferType = DMA_TRANSFER_MEM2PERIPH;
        dmaTransParams.regAddrOfData = (uint32_t)(&(lpi2cBase->MTXDATA.reg));
    }

    (void)LPI2C_ConfigDmaTransfer(instance, &dmaTransParams);

    /* Disable DMA requests for channel when transfer is done */
    DMA_DisableRequestsOnTransferComplete(dmaTransParams.dmaChannel, true);

    /* Call callback function when all the bytes were transfered */
    (void)DMA_RegisterCallback(dmaTransParams.dmaChannel,
                               (LPI2C_MasterCompleteDmaTransfer),
                               (void*)(instance));

    /* Start channel */
    (void)DMA_StartChannel(dmaTransParams.dmaChannel);

    LPI2C_MasterSendAddress(lpi2cBase, masterState, receive);

    /* Enable transmit/receive DMA requests */
    if (masterState->txSize == (uint32_t)0U)
    {
        LPI2C_MasterQueueCmd(lpi2cBase,
                             masterState,
                             LPI2C_MASTER_CMD_RECEIVE,
                             (uint8_t)(masterState->rxSize - 1U));
        (void)LPI2C_HW_ConfigMasterRxDMA(lpi2cBase, true);
    }
    else
    {
        (void)LPI2C_HW_ConfigMasterTxDMA(lpi2cBase, true);
    }
}

/*!
 * @brief starts the DMA transfer for slave
 *
 * @param instance: LPI2C peripheral instance number
 *
 * @retval None
 */
static void LPI2C_SlaveStartDmaTransfer(uint32_t instance)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    const LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];
    LPI2C_DMA_TRANSFER_PARAMS_T dmaTransParams;

    if (slaveState->txSize == 0U)
    {
        dmaTransParams.transferDir = LPI2C_REQUEST_RX;
        dmaTransParams.transferBuffer = slaveState->rxBuffer;
        dmaTransParams.transferSize = slaveState->rxSize;
        dmaTransParams.regAddrOfData = (uint32_t)(&(lpi2cBase->SRXDATA.reg));
        dmaTransParams.transferType = DMA_TRANSFER_PERIPH2MEM;
        dmaTransParams.dmaChannel = slaveState->dmaChannel;
    }
    else
    {
        dmaTransParams.transferDir = LPI2C_REQUEST_TX;
        dmaTransParams.transferBuffer = (uint8_t*)slaveState->txBuffer;
        dmaTransParams.transferSize = slaveState->txSize;
        dmaTransParams.regAddrOfData = (uint32_t)(&(lpi2cBase->SRXDATA.reg));
        dmaTransParams.transferType = DMA_TRANSFER_MEM2PERIPH;
        dmaTransParams.dmaChannel = slaveState->dmaChannel;
    }

    LPI2C_ConfigDmaTransfer(instance, &dmaTransParams);
    /* Adjustment added to source address at the beginning of TX buffer */
    DMA_ConfigSrcLastAddrAdjustment(dmaTransParams.dmaChannel,
                                    -(int32_t)(slaveState->txSize));

    /* Start channel */
    (void)DMA_StartChannel(dmaTransParams.dmaChannel);

    /* Enable transmit/receive DMA requests */
    if (slaveState->txSize == 0U)
    {
        LPI2C_HW_ConfigSlaveRxDMA(lpi2cBase, true);
    }
    else
    {
        LPI2C_HW_ConfigSlaveTxDMA(lpi2cBase, true);
    }
}

/*!
 * @brief re-initialize the I2C master
 *
 * @param instance: LPI2C peripheral instance number
 * @param masterState: Pointer to the LPI2C master driver context structure.
 *
 * @retval Error or success status
 */
static STATUS_T LPI2C_MasterReInit(
    uint32_t instance,
    LPI2C_MASTER_STATE_T *masterState)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];

    LPI2C_BAUDRATE_PARAMS_T baudrateParam;
    baudrateParam.baudrate = masterState->baudrate;

    /* Re-initialize driver status structure */
    masterState->rxSize = 0;
    masterState->txSize = 0;
    masterState->rxBuffer = NULL;
    masterState->txBuffer = NULL;
    masterState->isIdle = true;
    LPI2C_MasterQueueReset(masterState);

    LPI2C_HW_Init(lpi2cBase);
    LPI2C_MasterSetBaudrate(instance, masterState->i2cMode, baudrateParam);
    LPI2C_MasterSetSlaveAddr(instance,
                             masterState->slaveAddr,
                             masterState->isAddrFor10bit);
     LPI2C_HW_ConfigMasterEnable(lpi2cBase, true);

     g_lpi2cMasterStates[instance] = masterState;
     return STATUS_SUCCESS;
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param instance: LPI2C peripheral instance number
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status
 */
static STATUS_T LPI2C_MasterWaitTransferEnd(uint32_t instance, uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_MASTER_STATE_T *masterState = g_lpi2cMasterStates[instance];
    uint16_t rxFifoFill = 0;
    STATUS_T osifError = STATUS_SUCCESS;

    /* Wait for transfer to be completed by the IRQ */
    osifError = OSIF_SemWait(&(masterState->idleSem), timeout);

    if (osifError == STATUS_TIMEOUT)
    {
        /* If master is sending data transfer is aborted now in case timeout occurred */
        if (masterState->txSize == 0U)
        {
            if (masterState->transferType == LPI2C_USE_DMA)
            {
                /* Stop DMA channel and activate interrupts */
                (void)DMA_StopChannel(masterState->dmaChannel);
            }

            /* Disable interrupts to check number of bytes in rx fifo */
            LPI2C_HW_ConfigMasterInt(lpi2cBase, (uint32_t)LPI2C_MASTER_INT_RX_DATA, false);

            /* Check number of bytes in rx fifo */
            rxFifoFill = LPI2C_HW_ReadMasterRxFIFOCount(lpi2cBase);

            /**
             * In case both rx size and number of bytes in rx fifo is 0, then
             * the transfer ended successfully.
             */
            if (  (rxFifoFill == masterState->rxSize)
               && (masterState->rxSize ==  0U))
            {
                /* Blocking transfer is over */
                masterState->isBlocking = false;

                masterState->status = STATUS_SUCCESS;

                return masterState->status;
            }

            /**
             * Config watermark to rxFifoFill in case the rx size is grater
             * than the number of bytes in the rx fifo.
             */
            if (rxFifoFill < masterState->rxSize)
            {
                masterState->abortedTransfer = true;
                LPI2C_HW_ConfigMasterRxFIFOWatermark(lpi2cBase, rxFifoFill);

                masterState->status = STATUS_TIMEOUT;
            }

            LPI2C_HW_ConfigMasterInt(lpi2cBase, (uint32_t)LPI2C_MASTER_INT_RX_DATA, true);

            osifError = OSIF_SemWait(&(masterState->idleSem), timeout);
            if (osifError == STATUS_TIMEOUT)
            {
                (void)LPI2C_MasterReInit(instance, masterState);
                masterState->status = STATUS_TIMEOUT;
            }

            masterState->abortedTransfer = false;
        }
        else
        {
            LPI2C_MasterEndTransfer(lpi2cBase, masterState, false, true);

            masterState->status = STATUS_TIMEOUT;
        }
    }

    /* Blocking transfer is over */
    masterState->isBlocking = false;
    return masterState->status;
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param instance: LPI2C peripheral instance number
 * @param timeout: timeout for the transfer in milliseconds
 *
 * @retval Error or success status
 */
static STATUS_T LPI2C_SlaveWaitTransferEnd(uint32_t instance, uint32_t timeout)
{
    ASSERT_INSTANCE(instance);

    LPI2C_T *lpi2cBase = g_lpi2cBaseAddress[instance];
    LPI2C_SLAVE_STATE_T *slaveState = g_lpi2cSlaveStates[instance];

    /* Wait for transfer to be completed by the IRQ */
    if (OSIF_SemWait(&(slaveState->idleSem), timeout) == STATUS_TIMEOUT)
    {
        LPI2C_SlaveEndTransfer(lpi2cBase, slaveState);
        slaveState->status = STATUS_TIMEOUT;
    }

    /* Blocking transfer is over */
    slaveState->isBlocking = false;
    return slaveState->status;
}

/*!
 * @brief handle a transmit request for master
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure
 *
 * @retval None
 */
static void LPI2C_MasterHandleTransmitDataRequest(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState)
{
    /* More data needed for transmission */
    if (!LPI2C_MasterCmdQueueIsEmpty(masterState))
    {
        /* If there are queued commands, send them */
        LPI2C_MasterSendQueuedCmd(lpi2cBase, masterState);
    }
    else if (masterState->txBuffer != NULL)
    {
        /* A transmission is in progress */
        if (masterState->txSize != 0U)
        {
            /* Queue data bytes to fill tx fifo */
            LPI2C_MasterQueueData(lpi2cBase, masterState);
        }
        else
        {
            /* There is no more data in buffer, the transmission is over */
            LPI2C_MasterEndTransfer(lpi2cBase,
                                    masterState,
                                    masterState->isSendStop,
                                    false);

            /* Signal transfer end for blocking transfers */
            if (masterState->isBlocking == true)
            {
                (void)OSIF_SemPost(&(masterState->idleSem));
            }

            masterState->status = STATUS_SUCCESS;

            if (masterState->masterCallback != NULL)
            {
                masterState->masterCallback(I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                                            masterState->callbackParam);
            }
        }
    }
    else
    {
        /* No more commands and no transmission in progress - disable tx event */
        LPI2C_HW_ConfigMasterInt(lpi2cBase,
                                 (uint32_t)LPI2C_MASTER_INT_TX_DATA,
                                 false);
    }
}

/*!
 * @brief handle a receive request for master
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param masterState: Pointer to the LPI2C master driver context structure
 *
 * @retval None
 */
static void LPI2C_MasterHandleReceiveDataReadyEvent(
    LPI2C_T *lpi2cBase,
    LPI2C_MASTER_STATE_T *masterState)
{
    /* Copy received data to user buffer */
    while (LPI2C_HW_ReadMasterRxFIFOCount(lpi2cBase) > 0U)
    {
        if (masterState->rxSize <= 0U)
        {
            break;
        }
        masterState->rxBuffer[0U] = LPI2C_HW_ReadMasterRxData(lpi2cBase);
        masterState->rxBuffer++;
        masterState->rxSize--;
    }

    if (masterState->rxSize == 0U)
    {
        /* Receive completed, stop transfer */
        LPI2C_MasterEndTransfer(
            lpi2cBase,
            masterState,
            masterState->isSendStop,
            false);

        if (masterState->isBlocking)
        {
            /* Signal transfer end */
            (void)OSIF_SemPost(&(masterState->idleSem));
        }
        masterState->status = STATUS_SUCCESS;

        if (masterState->masterCallback != NULL)
        {
            masterState->masterCallback(
                I2C_MASTER_EVENT_TRANSFER_COMPLETE,
                masterState->callbackParam);
        }
    }
    else
    {
        if (masterState->rxSize <= LPI2C_HW_ReadMasterRxFIFOWatermark(lpi2cBase))
        {
            /* Reduce the rx watermark to receive the last few bytes */
            LPI2C_HW_ConfigMasterRxFIFOWatermark(
                lpi2cBase,
                (uint16_t)(masterState->rxSize - 1U));
        }
    }
}

/*!
 * @brief handle an address valid event for slave
 *
 * @param instance: LPI2C peripheral instance number
 * @param lpi2cBase: base address of the LPI2C module
 * @param slaveState: Pointer to the LPI2C slave driver context structure
 *
 * @retval None
 */
static void LPI2C_SlaveHandleAddressValidEvent(
    uint32_t instance,
    const LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState)
{
    ASSERT_INSTANCE(instance);

    uint16_t receivedAddr = LPI2C_HW_ReadSlaveReceivedAddr(lpi2cBase);

    if ((receivedAddr & 1U) == (uint16_t)0U)
    {
        /* Request from master to receive data */
        if ((slaveState->slaveCallback != NULL) && slaveState->isListening)
        {
            slaveState->slaveCallback(I2C_SLAVE_EVENT_RX_REQUEST,
                                      slaveState->callbackParam);
        }

        if ( (slaveState->transferType == LPI2C_USE_DMA)
           && slaveState->isListening)
        {
            (void)LPI2C_SlaveStartDmaTransfer(instance);
        }
    }
    else
    {
        /* Request from master to transmit data */
        if ( (slaveState->slaveCallback != NULL)
           && slaveState->isListening)
        {
            slaveState->slaveCallback(I2C_SLAVE_EVENT_TX_REQUEST,
                                      slaveState->callbackParam);
        }

        if (slaveState->transferType == LPI2C_USE_INTERRUPTS)
        {
            /* Enable interrupt for transmitting data */
            LPI2C_HW_ConfigSlaveInt(g_lpi2cBaseAddress[instance],
                                    (uint32_t)LPI2C_SLAVE_INT_TX_DATA, true);
        }

        slaveState->isTxUnderrun = false;

        if ( (slaveState->transferType == LPI2C_USE_DMA)
           && slaveState->isListening)
        {
            (void)LPI2C_SlaveStartDmaTransfer(instance);
        }
    }
    slaveState->status = STATUS_BUSY;
}

/*!
 * @brief handle an address valid event for slave
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param slaveState: Pointer to the LPI2C slave driver context structure
 *
 * @retval None
 */
static void LPI2C_SlaveHandleTransmitDataEvent(
    LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState)
{
    if (slaveState->isTxUnderrun == true)
    {
        /* Another Tx event after underflow warning means the dummy char was sent */
        slaveState->status = STATUS_I2C_TX_UNDERRUN_ERROR;
    }

    if (slaveState->txSize == 0U)
    {
        /* Out of data, call callback to allow user to provide a new buffer */
        if (slaveState->slaveCallback != NULL)
        {
            slaveState->slaveCallback(I2C_SLAVE_EVENT_TX_BUFFER_EMPTY,
                                      slaveState->callbackParam);
        }
    }

    if (slaveState->txSize != 0U)
    {
        LPI2C_HW_TxSlaveData(lpi2cBase, slaveState->txBuffer[0U]);
        slaveState->txBuffer++;
        slaveState->txSize--;
    }
    else
    {
        /*
         * Still no data, record tx underflow event and send dummy char.
         * Special case after the last tx byte: the device will ask for more data
         * but the dummy char will not be sent if NACK and then STOP condition are
         * received from master. So only record a "warning" for now.
         */
        slaveState->isTxUnderrun = true;
        LPI2C_HW_TxSlaveData(lpi2cBase, (uint8_t)0xFFU);
    }
}

/*!
 * @brief handle an address valid event for slave
 *
 * @param lpi2cBase: base address of the LPI2C module
 * @param slaveState: Pointer to the LPI2C slave driver context structure
 *
 * @retval None
 */
static void LPI2C_SlaveHandleReceiveDataEvent(
    const LPI2C_T *lpi2cBase,
    LPI2C_SLAVE_STATE_T *slaveState)
{
    if ((slaveState->rxSize == 0U) && (slaveState->slaveCallback != NULL))
    {
        /* No more room for data, call callback to allow user to provide a new buffer */
        slaveState->slaveCallback(I2C_SLAVE_EVENT_RX_BUFFER_FULL,
                                  slaveState->callbackParam);
    }

    if (slaveState->rxSize != 0U)
    {
        *(slaveState->rxBuffer) = LPI2C_HW_RxSlaveData(lpi2cBase);
        slaveState->rxBuffer++;
        slaveState->rxSize--;
    }
    else
    {
        /* Still no room for data, record rx overrun event and dummy read data */
        slaveState->status = STATUS_I2C_RX_OVERRUN_ERROR;
        (void)LPI2C_HW_RxSlaveData(lpi2cBase);
    }
}

/*!
 * @brief LPI2C master IRQ handler.
 *
 * @param None
 *
 * @retval Non
 *
 * @note The function name with the same name in the startup code.
 */
void LPI2C_Master_IRQHandler(void)
{
    LPI2C_MasterIRQHandler(0);
}

/*!
 * @brief LPI2C master IRQ handler.
 *
 * @param None
 *
 * @retval None
 *
 * @note The function name with the same name in the startup code.
 */
void LPI2C_Slave_IRQHandler(void)
{
    LPI2C_SlaveIRQHandler(0);
}

/**@} end of group LPI2C_Functions*/
/**@} end of group LPI2C_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
