/*!
 * @file        apm32f445_446_can.c
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

/* Includes */
#include "apm32f445_446_can.h"


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
#define CAN_ID_EXT_MASK         0x3FFFFu
#define CAN_ID_EXT_SHIFT        0
#define CAN_ID_EXT_WIDTH        18
#define CAN_ID_STD_MASK         0x1FFC0000u
#define CAN_ID_STD_SHIFT        18
#define CAN_ID_STD_WIDTH        11
#define CAN_CS_DLC_MASK         0xF0000u
#define CAN_CS_DLC_SHIFT        16
#define CAN_CS_DLC_WIDTH        4
#define CAN_CS_RTR_MASK         0x100000u
#define CAN_CS_RTR_SHIFT        20
#define CAN_CS_RTR_WIDTH        1
#define CAN_CS_IDE_MASK         0x200000u
#define CAN_CS_IDE_SHIFT        21
#define CAN_CS_IDE_WIDTH        1
#define CAN_CS_SRR_MASK         0x400000u
#define CAN_CS_SRR_SHIFT        22
#define CAN_CS_SRR_WIDTH        1
#define CAN_CS_CODE_MASK        0xF000000u
#define CAN_CS_CODE_SHIFT       24
#define CAN_CS_CODE_WIDTH       4
#define CAN_MB_EDL_MASK         0x80000000u
#define CAN_MB_BRS_MASK         0x40000000u

#define CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_MASK          (0x3FFFFFFFU)
#define CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_SHIFT         (1U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_MASK          (0x3FF80000U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_SHIFT         (19U)

#define CAN_RXFIFO_ID_FILTER_FORMAT_B_RTR_SHIFT         (15U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_IDE_SHIFT         (14U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_MASK          (0x1FFF8000U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_MASK1         (0x3FFFU)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT1        (16U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT2        (0U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_MASK          (0x7FFU)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_SHIFT1        (19U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_SHIFT2        (3U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_CMP_SHIFT     (15U)

#define CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_MASK          (0x1FE00000U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_MASK          (0x7F8U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1            (24U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT2            (16U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT3            (8U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT4            (0U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_CMP_SHIFT     (21U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_CMP_SHIFT     (3U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_C_MASK              (0xFFU)

#define CAN_RXFIFO_ID_FILTER_FORMAT_AB_RTR_SHIFT        (31U)
#define CAN_RXFIFO_ID_FILTER_FORMAT_AB_IDE_SHIFT        (30U)

#define CAN_MB_HANDLE_RXFIFO    0U

/* CAN bit timing values */
#define CAN_NUM_TQ_MIN          8U
#define CAN_NUM_TQ_MAX          26U
#define CAN_PRE_DIV_MAX         256U
#define CAN_PSEG1_MAX           8U
#define CAN_PSEG2_MIN           1U
#define CAN_PSEG2_MAX           8U
#define CAN_PROP_SEG_MAX        8U
#define CAN_TSEG1_MIN           2U
#define CAN_TSEG1_MAX           17U
#define CAN_TSEG2_MIN           2U
#define CAN_TSEG2_MAX           9U
#define CAN_RJW_MAX             3U

#define CAN_ALL_INTERRUPTS      (0x3B0006U)     /* Masks for wakeup, error, bus off */
#define CAN_BUS_OFF_INTERRUPT   (0xB0004U)      /* Masks for bus off, Tx/Rx warning */
#define CAN_ERROR_INTERRUPT     (0x300002U)     /* Masks for error */

/* CANFD extended data length encoding */
#define CAN_DLC_VALUE_12_BYTES  9U
#define CAN_DLC_VALUE_16_BYTES  10U
#define CAN_DLC_VALUE_20_BYTES  11U
#define CAN_DLC_VALUE_24_BYTES  12U
#define CAN_DLC_VALUE_32_BYTES  13U
#define CAN_DLC_VALUE_48_BYTES  14U
#define CAN_DLC_VALUE_64_BYTES  15U

#define CAN_RAMn_COUNT      128U
#define CAN_RXIMR_COUNT     32U

#define CAN_RXFIFO_FILTER_OFFSET    0x18U

#define CAN_RXFIFO_ACCEPT_REMOTE_FRAME      1UL
#define CAN_RXFIFO_ACCEPT_EXTENDED_FRAME    1UL

/* RxFIFO filter element number */
#define CAN_RXFIFO_FILTER_NUM(x)    (((x) + 1U) * 8U)

/* CAN endianness handling */
#ifdef CORE_LITTLE_ENDIAN
    #define CAN_SWAP_BYTES_IN_WORD_INDEX(index)     (((index) & ~3U) + (3U - ((index) & 3U)))
    #define CAN_SWAP_BYTES_IN_WORD(a, b)            REV_BYTES_32(a, b)
#endif

#define CAN_VALIDATE_INSTANCE(instance)     if (instance > CAN_INSTANCE_COUNT) { while(1); }

/**@} end of group CAN_Macros*/

/** @defgroup CAN_Enumerations Enumerations
  @{
*/

/**@} end of group CAN_Enumerations*/


/** @defgroup CAN_Structures Structures
  @{
*/

/**@} end of group CAN_Structures*/

/** @defgroup CAN_Variables Variables
  @{
*/

/* Base addresses for CAN instances */
static CAN_T * const g_canBaseAddress[] = {CAN0, CAN1, CAN2};

static const IRQn_Type g_canErrorIrq[] =  {CAN0_Error_IRQn, CAN1_Error_IRQn, CAN2_Error_IRQn};
static const IRQn_Type g_canBusOffIrq[] = {CAN0_ORed_IRQn, CAN1_ORed_IRQn, CAN2_ORed_IRQn};
static const IRQn_Type g_canOredMbIrq[FEATURE_CAN_MB_IRQS_MAX][CAN_INSTANCE_COUNT] = FEATURE_CAN_MB_IRQS;

/* Pointer to runtime states */
static CAN_STATE_T * g_canState[CAN_INSTANCE_COUNT] = {NULL};

/* Table to store CAN IRQ enum numbers */
static const IRQn_Type g_canWakeupIrq[] = {CAN0_Wake_Up_IRQn, NotAvail_IRQn, NotAvail_IRQn};

/**@} end of group CAN_Variables*/

/** @defgroup CAN_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
static void CAN_EnableIRQs(uint8_t index);
static void CAN_IRQHandler(uint8_t index);
static void CAN_IRQHandlerRxMb(uint8_t index, uint32_t mbIndex);
static void CAN_IRQHandlerRxFifo(uint8_t index, uint32_t mbIndex);
static void CAN_IRQHandlerBusOff(uint8_t index);
static void CAN_IRQHandlerError(uint8_t index);
static void CAN_IRQHandlerWakeup(uint8_t index);
static uint32_t CAN_BitrateToTimeSeg(
    uint32_t bitrate,
    uint32_t clockFreq,
    CAN_TIME_SEGMENT_T *timeSeg);

static void CAN_CalculateTseg(uint32_t * tSeg1Ptr, uint32_t * tSeg2Ptr);
static void CAN_CalculatePseg(uint32_t * tmpPropseg1, uint32_t * tmpPseg1);
static uint32_t CAN_GetSampleValue(uint32_t tmpSample, uint32_t samplePoint);
static uint32_t CAN_GetBitrateValue(uint32_t tmpBitrate, uint32_t bitrate);
static uint32_t CAN_GetJumpWidthValue(uint32_t pseg);

static uint8_t CAN_GetPayloadSize(uint8_t dlc);
static uint8_t CAN_CalculateDlcValue(uint8_t payloadSize);
static uint32_t CAN_LastMbOccupiedByRxFifo(uint32_t val);

static STATUS_T CAN_StartSendMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload,
    bool isBlocking );

static STATUS_T CAN_StartReceiveMb(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf,
    bool isBlocking);

static void CAN_FinishDataTransfer(uint8_t index, uint32_t mbIndex);

static STATUS_T CAN_StartReceiveFromFifo(
    uint8_t index,
    CAN_MSG_BUF_T *msgBuf,
    bool isBlocking);

static void CAN_FinishReceiveFifoData(uint8_t index);
static void CAN_CompleteRxFifoDataDMA(void *param, DMA_CHANNEL_STATUS_T channelStatus);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the CAN driver
 * @details This function will select a source clock, reset CAN controller, set
 *          maximum number of message buffers, initialize all message buffers
 *          as inactive, enable RxFIFO if needed, mask all mask bits, disable
 *          all MB interrupts, enable CAN normal mode, and enable all the error
 *          interrupts if needed.
 *
 * @param   index   CAN controller index
 * @param   states  Pointer to CAN driver state structure
 * @param   config  Configuration for the CAN driver
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_ERROR:   Error occurred
 */
STATUS_T CAN_Init(
    uint8_t index,
    CAN_STATE_T *states,
    const CAN_USER_CONFIG_T *config)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    STATUS_T osResult;
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_TIME_SEGMENT_T bitrate;

    if (CAN_HW_IsEnabled(baseAddr))
    {
        /* Enter disable mode requires freeze mode first */
        CAN_HW_EnterFreezeMode(baseAddr);
        CAN_HW_Disable(baseAddr);
    }

    /* Select source clock for the CAN engine */
    CAN_HW_SetClockSource(baseAddr, config->clockSource);

    /* Enable the CAN clock */
    CAN_HW_Enable(baseAddr);

    CAN_HW_EnterFreezeMode(baseAddr);

    /* Initialize the CAN controller */
    CAN_HW_Init(baseAddr);

    if (CAN_HW_InstanceHasFd(index))
    {
        /**
         * Enable or disable FD and check if FD was set as expected.
         * Enable FD might fail if the current CAN index does not support FD.
         */
        CAN_HW_SetFdEnable(baseAddr, config->fdEnable);
        if (CAN_HW_IsFdEnabled(baseAddr) != config->fdEnable)
        {
            return STATUS_ERROR;
        }

        /* Use CANFD ISO */
        CAN_HW_SetIsoFdEnable(baseAddr, config->fdEnable);
    }
    else if (config->fdEnable)
    {
        /* Return error when index doesn't support FD but enabled by user */
        return STATUS_ERROR;
    }
    else
    {
        /* Nothing to do */
    }

    /* Disable self reception if not in loopback mode */
    if (config->operationMode != CAN_MODE_LOOPBACK)
    {
        CAN_HW_SetSelfReceptionEnable(baseAddr, false);
    }

    /* Enable RxFIFO if requested. This will fail if FD mode is enabled. */
    if (config->rxFifoEnable)
    {
        result = CAN_HW_EnableRxFifo(baseAddr, (uint32_t)config->rxFifoFilterNum);
        if (result != STATUS_SUCCESS)
        {
            return result;
        }
    }

    /* Enable DMA support for RxFIFO */
    if (config->rxFifoTransferType == CAN_RXFIFO_USE_DMA)
    {
        if (CAN_HW_IsRxFifoEnabled(baseAddr))
        {
            CAN_HW_SetRxFifoDmaEnable(baseAddr, true);
        }
        else
        {
            return STATUS_ERROR;
        }
    }
    if (config->rxFifoTransferType == CAN_RXFIFO_USE_INTERRUPT)
    {
        CAN_HW_SetRxFifoDmaEnable(baseAddr, false);
    }

    /* Set payload size */
    CAN_HW_SetFdPayloadSize(baseAddr, config->fdPayloadSize);

    result = CAN_HW_SetLastMbNumber(baseAddr, config->maxMbNumber);
    if (result != STATUS_SUCCESS)
    {
        return result;
    }

    /* Set FD bitrate */
    if (CAN_HW_IsFdEnabled(baseAddr))
    {
        bitrate = config->bitrate;
        CAN_HW_SetExtendedTimeSegments(baseAddr, &bitrate);
        bitrate = config->fdBitrate;
        CAN_HW_SetFdTimeSegments(baseAddr, &bitrate);
    }
    else
    {
        bitrate = config->bitrate;
        CAN_HW_SetTimeSegments(baseAddr, &bitrate);
    }

    /* Set operation mode */
    CAN_HW_SetOperationMode(baseAddr, config->operationMode);
    if (config->operationMode != CAN_MODE_FREEZE)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }

    /* Enable CAN interrupts */
    CAN_EnableIRQs(index);

    for (uint32_t i = 0; i < FEATURE_CAN_MAX_MB_NUM; i++)
    {
        osResult = OSIF_SemCreate(&states->mbHandles[i].sem, 0U);
        if (osResult != STATUS_SUCCESS)
        {
            for (uint32_t j = 0; j < i; j++)
            {
                (void)OSIF_SemDestroy(&states->mbHandles[j].sem);
            }
            return STATUS_ERROR;
        }
        states->mbHandles[i].msgBuf = NULL;
        states->mbHandles[i].isBlocking = false;
        states->mbHandles[i].state = CAN_MB_STATE_IDLE;
    }

    /* Store transfer type and DMA channel number */
    states->rxFifoTransferType = config->rxFifoTransferType;
    states->rxFifoDmaChannel = config->rxFifoDmaChannel;

    /* Clear callbacks */
    states->callback = NULL;
    states->errorCallbackParam = NULL;
    states->callbackParam = NULL;
    states->errorCallback = NULL;

    /* Store runtime state structure */
    g_canState[index] = states;

    return STATUS_SUCCESS;
}

/*!
 * @brief   Deinitialize the CAN driver
 * @details Disable all the CAN interrupts and the CAN controller.
 *
 * @param   index   CAN controller index
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_ERROR:   Error occurred
 */
STATUS_T CAN_DeInit(uint8_t index)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result = STATUS_SUCCESS;
    STATUS_T osResult;
    CAN_T *baseAddr = g_canBaseAddress[index];
    const CAN_STATE_T *states = g_canState[index];

    /* Disable CAN interrupts */
    if (states != NULL)
    {
        if (states->errorCallback != NULL)
        {
            CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_BUS_OFF, false);
            CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_ERROR, false);
            CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_TX_WARNING, false);
            CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_RX_WARNING, false);
        }
    }

    if (g_canWakeupIrq[index] != NotAvail_IRQn)
    {
        INT_SYS_DisableIRQ(g_canWakeupIrq[index]);
    }

    INT_SYS_DisableIRQ(g_canErrorIrq[index]);
    INT_SYS_DisableIRQ(g_canBusOffIrq[index]);

    for (uint32_t i = 0; i < FEATURE_CAN_MB_IRQS_MAX; i++)
    {
        if (g_canOredMbIrq[i][index] != NotAvail_IRQn)
        {
            INT_SYS_DisableIRQ(g_canOredMbIrq[i][index]);
        }
    }

    if (CAN_HW_IsEnabled(g_canBaseAddress[index]))
    {
        CAN_HW_EnterFreezeMode(g_canBaseAddress[index]);
        CAN_HW_Disable(g_canBaseAddress[index]);
    }

    if (states != NULL)
    {
        for (uint32_t i = 0; i < FEATURE_CAN_MAX_MB_NUM; i++)
        {
            osResult = OSIF_SemDestroy(&states->mbHandles[i].sem);
            if (osResult != STATUS_SUCCESS)
            {
                result = STATUS_ERROR;
            }
        }
    }

    if (result == STATUS_SUCCESS)
    {
        g_canState[index] = NULL;
    }
    return result;
}

/*!
 * @brief   Get the default configuration
 * @details This function get the default configuration structure with the
 *          following settings:
 *          - 16 message buffers
 *          - RxFIFO disabled
 *          - CANFD disabled
 *          - Normal operation mode
 *          - 8 bytes payload size
 *          - Clock source: oscillator clock
 *          - Bitrate: 500 Kbit/s (computed for sample point = 87.5)
 *
 * @param   config  The configuration structure
 *
 * @retval  The bitrate for generated configuration structure
 */
uint32_t CAN_DefaultConfig(CAN_USER_CONFIG_T *config)
{
    uint32_t bitrate;
    uint32_t clockFreq;
    CAN_TIME_SEGMENT_T timeSeg;

    /* Maximum number of message buffers */
    config->maxMbNumber = 16;
    /* Normal operation mode */
    config->operationMode = CAN_MODE_NORMAL;
    /* RxFIFO is disabled */
    config->rxFifoEnable = false;
    /* Number of RxFIFO ID filters */
    config->rxFifoFilterNum = CAN_RXFIFO_ID_FILTERS_8;

    /* Get the PE clock frequency */
    (void)CLOCK_SYS_ReadFreq(FEATURE_CAN_OSC_CLK_NAME, &clockFreq);

    /* Time segments calculated for PE bitrate = 500 Kbit/s, sample point = 87.5 */
    bitrate = CAN_BitrateToTimeSeg(500000U, clockFreq, &timeSeg);

    /* Time segments for the arbitration phase */
    config->bitrate = timeSeg;

    /* Configurable data rate is disabled */
    config->fdEnable = false;
    /* Payload size */
    config->fdPayloadSize = CAN_FD_PAYLOAD_SIZE_8;
    /* Time segments for the data phase of FD frames */
    config->fdBitrate = timeSeg;

    /* Clock source is system oscillator */
    config->clockSource = CAN_CLOCK_SOURCE_OSC;

    /* RxFIFO transfer type */
    config->rxFifoTransferType = CAN_RXFIFO_USE_INTERRUPT;

    /* RxFIFO DMA channel */
    config->rxFifoDmaChannel = 0U;

    return bitrate;
}

/*!
 * @brief   Get CAN bitrate
 * @details Return the current bitrate settings for classic frames or the
 *          arbitration phase of FD frames.
 *
 * @param   index   CAN controller index
 * @param   bitrate Pointer to CAN bitrate settings
 *
 * @retval  None
 */
void CAN_GetBitrate(uint8_t index, CAN_TIME_SEGMENT_T *bitrate)
{
    CAN_VALIDATE_INSTANCE(index);

    const CAN_T *baseAddr = g_canBaseAddress[index];

    if (CAN_HW_IsFdEnabled(baseAddr))
    {
        /* Get extended time segments for CANFD*/
        CAN_HW_GetExtendedTimeSegments(baseAddr, bitrate);
    }
    else
    {
        /* Get the time segments */
        CAN_HW_GetTimeSegments(baseAddr, bitrate);
    }
}

/*!
 * @brief   Set CAN bitrate
 * @details Setup all the time segment values for classic frames or the
 *          extended time segments for the arbitration phase of FD frames.
 *
 * @param   index   CAN controller index
 * @param   bitrate Pointer to CAN bitrate settings
 *
 * @retval  None
 */
void CAN_SetBitrate(uint8_t index, const CAN_TIME_SEGMENT_T *bitrate)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    bool fdEnabled = CAN_HW_IsFdEnabled(baseAddr);
    if (fdEnabled)
    {
        /* Set FD extended time segments */
        CAN_HW_SetExtendedTimeSegments(baseAddr, bitrate);
    }
    else
    {
        /* Set time segments */
        CAN_HW_SetTimeSegments(baseAddr, bitrate);
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Get CANFD bitrate
 * @details Return the current bitrate settings for the data phase of FD frames.
 *
 * @param   index   CAN controller index
 * @param   bitrate Pointer to CAN bitrate settings
 *
 * @retval  None
 */
void CAN_GetBitrateFd(uint8_t index, CAN_TIME_SEGMENT_T *bitrate)
{
    CAN_VALIDATE_INSTANCE(index);

    const CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_HW_GetFdTimeSegments(baseAddr, bitrate);
}

/*!
 * @brief   Set CANFD bitrate
 * @details Set the time segment values for the data phase of FD frames.
 *
 * @param   index   CAN controller index
 * @param   bitrate Pointer to CAN bitrate settings
 *
 * @retval  None
 */
void CAN_SetBitrateFd(uint8_t index, const CAN_TIME_SEGMENT_T *bitrate)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
        CAN_HW_SetFdTimeSegments(baseAddr, bitrate);
        CAN_HW_ExitFreezeMode(baseAddr);
    }
    else
    {
        CAN_HW_SetFdTimeSegments(baseAddr, bitrate);
    }
}

/*!
 * @brief   Set Transceiver Delay Compensation Offset
 * @details Enable or disable the Transceiver Delay Compensation feature and
 *          set the Transceiver Delay Compensation Offset (offset value to be
 *          added to the measured transceiver's loop delay in order to define
 *          the position of the delayed comparison point when bitrate switching
 *          is active).
 *
 * @param   index       CAN controller index
 * @param   isEnabled   Enable or disable Transceiver Delay Compensation
 * @param   offset      Transceiver Delay Compensation Offset
 *
 * @retval  None
 */
void CAN_SetTdcOffset(uint8_t index, bool isEnabled, uint8_t offset)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
        CAN_HW_SetTdcOffset(baseAddr, isEnabled, offset);
        CAN_HW_ExitFreezeMode(baseAddr);
    }
    else
    {
        CAN_HW_SetTdcOffset(baseAddr, isEnabled, offset);
    }
}

/*!
 * @brief   Get the transceiver delay compensation value
 *
 * @param   index   CAN controller index
 *
 * @retval  Transceiver delay compensation value
 */
uint8_t CAN_GetTdcValue(uint8_t index)
{
    const CAN_T *baseAddr = g_canBaseAddress[index];

    return (uint8_t)(baseAddr->FDCTRL.bit.TDCV);
}

/*!
 * @brief   Get the value of the TDC fail flag
 *
 * @param   index   CAN controller index
 *
 * @retval  true:   Transceiver delay compensation was failed
 *          false:  Transceiver delay compensation was not failed
 */
bool CAN_GetTdcFail(uint8_t index)
{
    const CAN_T *baseAddr = g_canBaseAddress[index];
    return (baseAddr->FDCTRL.bit.TDCF != 0U) ? true : false;
}

/*!
 * @brief   Clear the TDC fail flag
 *
 * @param   index   CAN controller index
 *
 * @retval  None
 */
void CAN_ClearTdcFail(uint8_t index)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    baseAddr->FDCTRL.bit.TDCF = CAN_FDCTRL_TDCF_1;
}

/*!
 * @brief   Blocking send CAN frame
 * @details Send a CAN frame using a configured message buffer. The function
 *          blocks until either the frame was sent, or the timeout expired.
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   txInfo  Information of the CAN frame
 * @param   canId   CAN message ID
 * @param   payload Payload of the CAN frame
 * @param   timeout Timeout in milliseconds
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_TIMEOUT: Timeout is reached
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_SendBlocking(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload,
    uint32_t timeout)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    STATUS_T osResult;
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];
    uint32_t mbConfig;

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    result = CAN_StartSendMb(index, mbIndex, txInfo, canId, payload, true);
    if (result == STATUS_SUCCESS)
    {
        /* Enable message buffer interrupt */
        CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, true);

        osResult = OSIF_SemWait(&states->mbHandles[mbIndex].sem, timeout);

        /* Disable message buffer interrupt */
        CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, false);

        if (osResult == STATUS_TIMEOUT)
        {
            if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
            {
                result = STATUS_SUCCESS;
                (void)OSIF_SemWait(&states->mbHandles[mbIndex].sem, 0);
            }
            else
            {
                /* Clear message buffer flag */
                CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
                CAN_HW_AbortTxMb(baseAddr, mbIndex);

                /* Wait to finish abort operation */
                while(CAN_HW_GetMbInterruptFlag(baseAddr, mbIndex) == 0U)
                {
                }

                volatile const uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
                mbConfig = *mbRegion;

                /* Check if the MBs have been safely inactivated */
                if (((mbConfig & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)CAN_CODE_TX_INACTIVE)
                {
                    result = STATUS_SUCCESS;
                }

                if (((mbConfig & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)CAN_CODE_TX_ABORT)
                {
                    /* Transmission have been aborted */
                    result = STATUS_TIMEOUT;
                }

                /* Clear message buffer flag */
                CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
                states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;
            }
        }
    }
    return result;
}

/*!
 * @brief   Non-blocking send CAN frame
 * @details Send a CAN frame using the configured message buffer. The function
 *          returns immediately. If a callback is installed, it will be invoked
 *          after the frame was sent.
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   txInfo  Information of the CAN frame
 * @param   canId   CAN message ID
 * @param   payload Payload of the CAN frame
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_SendNonBlocking(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    CAN_T *baseAddr = g_canBaseAddress[index];

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    result = CAN_StartSendMb(index, mbIndex, txInfo, canId, payload, false);
    if (result == STATUS_SUCCESS)
    {
        /* Enable message buffer interrupt */
        CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, true);
    }
    return result;
}

/*!
 * @brief   Blocking receive CAN frame from message buffer
 * @details Receive CAN frame using the configured message buffer. The function
 *          blocks until either a frame was received, or the timeout expired.
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   msgBuf  Buffer to store the received CAN frame
 * @param   timeout Timeout in milliseconds
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_TIMEOUT: Timeout is reached
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_ReceiveBlocking(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf,
    uint32_t timeout)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    STATUS_T osResult;
    CAN_STATE_T *states = g_canState[index];
    CAN_T *baseAddr = g_canBaseAddress[index];

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    result = CAN_StartReceiveMb(index, mbIndex, msgBuf, true);
    if (result == STATUS_SUCCESS)
    {
        osResult = OSIF_SemWait(&states->mbHandles[mbIndex].sem, timeout);

        /* Disable message buffer interrupt */
        CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, false);

        if (osResult == STATUS_TIMEOUT)
        {
            if (states->mbHandles[mbIndex].state != CAN_MB_STATE_IDLE)
            {
                states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;
                result = STATUS_TIMEOUT;
            }
            else
            {
                (void)OSIF_SemWait(&states->mbHandles[mbIndex].sem, 0);
                result = STATUS_SUCCESS;
            }
        }
    }
    return result;
}

/*!
 * @brief   Non-blocking receive CAN frame from message buffer
 * @details Receive CAN frame using the configured message buffer. The function
 *          returns immediately. If a callback is installed, it will be invoked
 *          after the frame was received and read into the specified buffer.
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   msgBuf  Buffer to store the received CAN frame
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_TIMEOUT: Timeout is reached
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_ReceiveNonBlocking(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf)
{
    CAN_VALIDATE_INSTANCE(index);

    const CAN_T *baseAddr = g_canBaseAddress[index];

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    return CAN_StartReceiveMb(index, mbIndex, msgBuf, false);
}

/*!
 * @brief   Blocking receive CAN frame from RxFIFO
 * @details Receive CAN frame using the RxFIFO. The function blocks until
 *          either a frame was received, or the timeout expired.
 *
 * @param   index   CAN controller index
 * @param   msgBuf  Buffer to store the received CAN frame
 * @param   timeout Timeout in milliseconds
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_TIMEOUT: Timeout is reached
 *          STATUS_ERROR:   Error occurred
 */
STATUS_T CAN_ReceiveFifoBlocking(
    uint8_t index,
    CAN_MSG_BUF_T *msgBuf,
    uint32_t timeout)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    result = CAN_StartReceiveFromFifo(index, msgBuf, true);
    if (result == STATUS_SUCCESS)
    {
        result = OSIF_SemWait(&states->mbHandles[CAN_MB_HANDLE_RXFIFO].sem, timeout);

        if (states->rxFifoTransferType == CAN_RXFIFO_USE_INTERRUPT)
        {
            /* Disable RxFIFO interrupts */
            CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_WARNING, false);
            CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_OVERFLOW, false);
            CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, false);
        }
        else
        {
            (void)DMA_StopChannel(states->rxFifoDmaChannel);
        }

        if (result == STATUS_TIMEOUT)
        {
             /* Reception successful if the status is updated */
            if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_IDLE)
            {
                (void)OSIF_SemWait(&states->mbHandles[CAN_MB_HANDLE_RXFIFO].sem, 0);
                result = STATUS_SUCCESS;
            }
            else
            {
                states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;
            }
        }
    }
    return result;
}

/*!
 * @brief   Non-blocking receive CAN frame from RxFIFO
 * @details Receive CAN frame using the RxFIFO. The function returns
 *          immediately. If a callback is installed, it will be invoked after
 *          the frame was received and read into the specified buffer.
 *
 * @param   index   CAN controller index
 * @param   msgBuf  Buffer to store the received CAN frame
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_ERROR:   Error occurred
 */
STATUS_T CAN_ReceiveFifoNonBlocking(uint8_t index, CAN_MSG_BUF_T *msgBuf)
{
    CAN_VALIDATE_INSTANCE(index);

    return CAN_StartReceiveFromFifo(index, msgBuf, false);
}

/*!
 * @brief   Abort a non-blocking transfer
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_NO_TRANSFER_IN_PROGRESS: No transfer was running
 */
STATUS_T CAN_AbortTransfer(uint8_t index, uint8_t mbIndex)
{
    CAN_VALIDATE_INSTANCE(index);

    uint32_t mbConfig;
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    /* Check if there is a transfer running */
    if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
    {
        return STATUS_CAN_NO_TRANSFER_IN_PROGRESS;
    }

    if (states->mbHandles[mbIndex].state == CAN_MB_STATE_TX_BUSY)
    {
        STATUS_T result = STATUS_SUCCESS;

        /* Stop the running transfer */
        CAN_FinishDataTransfer(index, mbIndex);

        /* Clear message buffer flag */
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
        CAN_HW_AbortTxMb(baseAddr, mbIndex);

        /* Wait to finish abort operation */
        while(CAN_HW_GetMbInterruptFlag(baseAddr, mbIndex) == 0U)
        {
        }

        volatile const uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
        mbConfig = *mbRegion;

        /* Check if the MBs have been safely inactivated */
        if (((mbConfig & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)CAN_CODE_TX_INACTIVE)
        {
            /* Transmission have occurred */
            result = STATUS_CAN_NO_TRANSFER_IN_PROGRESS;
        }

        if (((mbConfig & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)CAN_CODE_TX_ABORT)
        {
            /* Transmission have been aborted */
            result = STATUS_SUCCESS;
        }

        /* Clear message buffer flag */
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

        states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;
        return result;
    }

    if (states->mbHandles[mbIndex].state == CAN_MB_STATE_RX_BUSY)
    {
        /* Stop the running transfer */
        CAN_FinishDataTransfer(index, mbIndex);

        if (!CAN_HW_IsRxFifoEnabled(baseAddr))
        {
            /* This operation is not allowed for MB that are part of RxFIFO */
            CAN_HW_ResetRxMb(baseAddr, mbIndex);
        }
        else
        {
            /* Get the number of RxFIFO filters */
            uint32_t numFilters = (uint32_t)(baseAddr->CTRL2.bit.NUMFIFO);

            /* Get the number of MBs occupied by RxFIFO and ID filter table */
            uint32_t numMbs = CAN_LastMbOccupiedByRxFifo(numFilters);
            if (mbIndex > numMbs)
            {
                /* This operation is not allowed for MB that are part of RxFIFO */
                CAN_HW_ResetRxMb(baseAddr, mbIndex);
            }

            if (mbIndex == CAN_MB_HANDLE_RXFIFO)
            {
                /* Disable RxFIFO interrupts */
                CAN_HW_SetMbInterruptEnable(baseAddr,
                                                      FEATURE_CAN_RXFIFO_FRAME_AVAILABLE,
                                                      false);
                if (states->rxFifoTransferType == CAN_RXFIFO_USE_DMA)
                {
                    (void)DMA_StopChannel(states->rxFifoDmaChannel);
                }
            }
        }
    }

    if (CAN_HW_GetMbInterruptEnable(baseAddr, mbIndex) == 1U)
    {
        CAN_FinishDataTransfer(index, mbIndex);
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief   Return whether the previous receive is completed
 * @details When performing non-blocking receive, the user can call this
 *          function to ascertain the state of the current receive progress:
 *          in progress (busy) or complete (success).
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_ERROR:   DMA error
 */
STATUS_T CAN_GetTransferStatus(uint8_t index, uint8_t mbIndex)
{
    CAN_VALIDATE_INSTANCE(index);

    const CAN_STATE_T *states = g_canState[index];

    if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
    {
        return STATUS_SUCCESS;
    }
    else if (states->mbHandles[mbIndex].state == CAN_MB_STATE_DMA_ERROR)
    {
        return STATUS_ERROR;
    }
    else
    {
        return STATUS_BUSY;
    }
}

/*!
 * @brief   Return reported error conditions
 * @details Report various error conditions detected in the reception and
 *          transmission of a CAN frame and some general status of the device.
 *
 * @param   index   CAN controller index
 *
 * @retval  Value of the Error Flag Register 1
 */
uint32_t CAN_GetErrorStatus(uint8_t index)
{
    CAN_VALIDATE_INSTANCE(index);

    const CAN_T *baseAddr = g_canBaseAddress[index];

    return ((uint32_t)(baseAddr->ERRFLG1.reg));
}

/*!
 * @brief   Configure the Rx message buffer
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   rxInfo  Information of the CAN data
 * @param   canId   CAN message ID
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_ConfigureRxMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *rxInfo,
    uint32_t canId)
{
    CAN_VALIDATE_INSTANCE(index);

    STATUS_T result;
    CAN_MB_CODE_STATUS_T mbcs;
    CAN_T *baseAddr = g_canBaseAddress[index];

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    /* Clear the interrupt flag if previous triggered */
    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

    mbcs.fdEnable = rxInfo->fdEnable;
    mbcs.msgIdType = rxInfo->msgIdType;
    mbcs.dataLen = rxInfo->dataLen;

    mbcs.code = (uint32_t)CAN_CODE_RX_NOT_USED;
    result = CAN_HW_SetRxMb(baseAddr, mbIndex, &mbcs, canId);

    if (result == STATUS_SUCCESS)
    {
        mbcs.code = (uint32_t)CAN_CODE_RX_INACTIVE;
        result = CAN_HW_SetRxMb(baseAddr, mbIndex, &mbcs, canId);
    }
    if (result == STATUS_SUCCESS)
    {
         /* Setup message buffer fields for receiving data */
         mbcs.code = (uint32_t)CAN_CODE_RX_EMPTY;
         result = CAN_HW_SetRxMb(baseAddr, mbIndex, &mbcs, canId);
    }
    if (result == STATUS_SUCCESS)
    {
        /* Clear the interrupt flag if previous triggered */
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
    }
    return result;
}

/*!
 * @brief   Configure the Tx message buffer
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   txInfo  Information of the CAN data
 * @param   canId   CAN message ID
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_ConfigureTxMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_MB_CODE_STATUS_T mbcs;
    CAN_T *baseAddr = g_canBaseAddress[index];

    /* Check if the MB is valid */
    if (CAN_HW_IsMbOutOfRange(baseAddr, mbIndex))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    mbcs.msgIdType = txInfo->msgIdType;
    mbcs.dataLen = txInfo->dataLen;
    mbcs.fdPadding = txInfo->fdPadding;
    mbcs.fdEnable = txInfo->fdEnable;
    mbcs.brsEnable = txInfo->brsEnable;
    mbcs.code = (uint32_t)CAN_CODE_TX_INACTIVE;

    return CAN_HW_SetTxMb(baseAddr, mbIndex, &mbcs, canId, NULL, false);
}

/*!
 * @brief   Configure the RxFIFO
 * @details This function will configure the RxFIFO ID filter table elements,
 *          and enable RxFIFO interrupts.
 *
 * @param   index           CAN controller index
 * @param   idFormat        The format of the RxFIFO ID filter table elements
 * @param   idFilterTable   ID filter table elements
 *
 * @retval  None
 */
void CAN_ConfigureRxFifo(
    uint8_t index,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat,
    const CAN_RXFIFO_ID_FILTER_T *idFilterTable)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
        CAN_HW_SetRxFifoFilter(baseAddr, idFormat, idFilterTable);
        CAN_HW_ExitFreezeMode(baseAddr);
    }
    else
    {
        CAN_HW_SetRxFifoFilter(baseAddr, idFormat, idFilterTable);
    }
}

/*!
 * @brief   Configure the transmit message buffer for remote frame response
 * @details This function will setup the message buffer fields, configure the
 *          message buffer code for Tx buffer as RX_RANSWER and enable the
 *          message buffer interrupt.
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 * @param   txInfo  Information of the CAN data
 * @param   canId   CAN message ID
 * @param   payload Message payload
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_ConfigureRemoteResponseMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_MB_CODE_STATUS_T mbcs;
    CAN_T *baseAddr = g_canBaseAddress[index];

    if (mbIndex <= (uint32_t)(baseAddr->MCFG.bit.LASTMB))
    {
        mbcs.dataLen = txInfo->dataLen;
        mbcs.msgIdType = txInfo->msgIdType;
        mbcs.code = (uint32_t)CAN_CODE_RX_RANSWER;

        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

        return CAN_HW_SetTxMb(baseAddr, mbIndex, &mbcs, canId, payload, txInfo->isRemote);
    }
    else
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }
}

/*!
 * @brief   Set RX masking type to global mask or individual mask
 *
 * @param   index       CAN controller index
 * @param   maskType    Rx masking type
 *
 * @retval  None
 */
void CAN_SetRxMaskType(uint8_t index, CAN_RX_MASK_TYPE_T maskType)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
        CAN_HW_SetRxMaskType(baseAddr, maskType);
        CAN_HW_ExitFreezeMode(baseAddr);
    }
    else
    {
        CAN_HW_SetRxMaskType(baseAddr, maskType);
    }
}

/*!
 * @brief   Set the Rx individual mask (standard or extended)
 *
 * @param   index   CAN controller index
 * @param   idType  Standard ID or extended ID
 * @param   mbIndex Index of the message buffer
 * @param   mask    Mask value
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_SetRxIndividualMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint8_t mbIndex,
    uint32_t mask)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if ((mbIndex >= CAN_RXIMR_COUNT) || (mbIndex > CAN_HW_GetLastMbNumber(baseAddr)))
    {
        if (!freezeStatus)
        {
            CAN_HW_ExitFreezeMode(baseAddr);
        }
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    if (CAN_HW_IsRxFifoEnabled(baseAddr))
    {
        if (mbIndex > CAN_HW_GetAffectedRxFifoNumbers(baseAddr))
        {
            if (idType == CAN_ID_STANDARD)
            {
                CAN_HW_SetRxIndividualStdMask(baseAddr, mbIndex, mask);
            }
            else if (idType == CAN_ID_EXTENDED)
            {
                CAN_HW_SetRxIndividualExtMask(baseAddr, mbIndex, mask);
            }
            else
            {
            }
        }
        else
        {
            /* Get the ID filter format already configured by CAN_ConfigureRxFifo() */
            CAN_RXFIFO_ID_ELEMENT_FORMAT_T formatType = CAN_HW_GetRxFifoIdFormat(baseAddr);

            uint32_t calcMask = CAN_HW_CalculateRxFifoMask(idType, formatType, mask);
            if (formatType == CAN_RXFIFO_ID_FORMAT_A)
            {
                CAN_HW_SetRxIndividualMask(baseAddr, mbIndex, calcMask);
            }
            else if (formatType == CAN_RXFIFO_ID_FORMAT_B)
            {
                CAN_HW_SetRxIndividualMask(
                    baseAddr,
                    mbIndex,
                    (calcMask | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT1)));
            }
            else if (formatType == CAN_RXFIFO_ID_FORMAT_C)
            {
                CAN_HW_SetRxIndividualMask(
                    baseAddr,
                    mbIndex,
                    (calcMask | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1)
                              | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT2)
                              | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT3)));
            }
            else
            {
                CAN_HW_SetRxIndividualMask(baseAddr, mbIndex, 0xFFFFFFFFU);
            }
        }
    }
    else
    {
        if (idType == CAN_ID_STANDARD)
        {
            CAN_HW_SetRxIndividualStdMask(baseAddr, mbIndex, mask);
        }
        else if (idType == CAN_ID_EXTENDED)
        {
            CAN_HW_SetRxIndividualExtMask(baseAddr, mbIndex, mask);
        }
        else
        {
        }
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
    return STATUS_SUCCESS;
}

/*!
 * @brief   Set the RxFIFO global mask (standard or extended)
 *
 * @param   index   CAN controller index
 * @param   idType  Standard ID or extended ID
 * @param   mask    Mask value
 *
 * @retval  None
 */
void CAN_SetRxFifoGlobalMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_RXFIFO_ID_ELEMENT_FORMAT_T format;
    uint32_t calcMask = 0U;
    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if (CAN_HW_IsRxFifoEnabled(baseAddr))
    {
        format = CAN_HW_GetRxFifoIdFormat(baseAddr);
        calcMask = CAN_HW_CalculateRxFifoMask(idType, format, mask);

        if (format == CAN_RXFIFO_ID_FORMAT_A)
        {
            CAN_HW_SetRxFifoGlobalMask(baseAddr, calcMask);
        }
        else if (format == CAN_RXFIFO_ID_FORMAT_B)
        {
            CAN_HW_SetRxFifoGlobalMask(
                baseAddr,
                (calcMask | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT1)));
        }
        else if (format == CAN_RXFIFO_ID_FORMAT_C)
        {
            CAN_HW_SetRxFifoGlobalMask(
                baseAddr,
                (calcMask | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1)
                          | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT2)
                          | (calcMask >> CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT3)));
        }
        else
        {
            CAN_HW_SetRxFifoGlobalMask(baseAddr, 0xFFFFFFFFU);
        }
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Set the Rx message buffer global mask (standard or extended)
 *
 * @param   index   CAN controller index
 * @param   idType  Standard ID or extended ID
 * @param   mask    Mask value
 *
 * @retval  None
 */
void CAN_SetRxMbGlobalMask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];

    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if (idType == CAN_ID_STANDARD)
    {
        CAN_HW_SetRxMbGlobalStdMask(baseAddr, mask);
    }
    else if (idType == CAN_ID_EXTENDED)
    {
        CAN_HW_SetRxMbGlobalExtMask(baseAddr, mask);
    }
    else
    {
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Set the Rx message buffer 14 mask (standard or extended)
 *
 * @param   index   CAN controller index
 * @param   idType  Standard ID or extended ID
 * @param   mask    Mask value
 *
 * @retval  None
 */
void CAN_SetRxMb14Mask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if (idType == CAN_ID_STANDARD)
    {
        CAN_HW_SetRxMb14StdMask(baseAddr, mask);
    }
    else if (idType == CAN_ID_EXTENDED)
    {
        CAN_HW_SetRxMb14ExtMask(baseAddr, mask);
    }
    else
    {
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Set the Rx message buffer 15 mask (standard or extended)
 *
 * @param   index   CAN controller index
 * @param   idType  Standard ID or extended ID
 * @param   mask    Mask value
 *
 * @retval  None
 */
void CAN_SetRxMb15Mask(
    uint8_t index,
    CAN_MSG_ID_TYPE_T idType,
    uint32_t mask)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if (idType == CAN_ID_STANDARD)
    {
        CAN_HW_SetRxMb15StdMask(baseAddr, mask);
    }
    else if (idType == CAN_ID_EXTENDED)
    {
        CAN_HW_SetRxMb15ExtMask(baseAddr, mask);
    }
    else
    {
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Install a callback function for CAN events
 *
 * @param   index           CAN controller index
 * @param   callBack        The callback function
 * @param   callbackParam   Parameter passed to the callback function
 *
 * @retval  None
 */
void CAN_InstallEventCallback(
    uint8_t index,
    CAN_CALLBACK_T callBack,
    void *callbackParam)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_STATE_T *states = g_canState[index];

    states->callback = callBack;
    states->callbackParam = callbackParam;
}

/*!
 * @brief   Install a callback function for CAN errors
 *
 * @param   index           CAN controller index
 * @param   callBack        The callback function
 * @param   callbackParam   Parameter passed to the callback function
 *
 * @retval  None
 */
void CAN_InstallErrorCallback(
    uint8_t index,
    CAN_ERROR_CALLBACK_T callBack,
    void *callbackParam)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    states->errorCallback = callBack;
    states->errorCallbackParam = callbackParam;

    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

    if (callBack != NULL)
    {
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_RX_WARNING, true);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_TX_WARNING, true);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_ERROR, true);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_BUS_OFF, true);
    }
    else
    {
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_RX_WARNING, false);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_TX_WARNING, false);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_ERROR, false);
        CAN_HW_SetErrorInterruptEnable(baseAddr, CAN_INTERRUPT_BUS_OFF, false);
    }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Configure pretended networking
 *
 * @param   index       CAN controller index
 * @param   isEnabled   Enable or disable pretended networking
 * @param   pnConfig    Pointer to pretended networking configuration
 *
 * @retval  None
 */
void CAN_ConfigurePretendedNetworking(
    uint8_t index,
    bool isEnabled,
    const CAN_PN_CONFIG_T *pnConfig)
{
    CAN_VALIDATE_INSTANCE(index);

    CAN_T *baseAddr = g_canBaseAddress[index];
    bool freezeStatus = CAN_HW_GetFreezeStatus(baseAddr);

    if (!freezeStatus)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }

   if (isEnabled)
   {
       CAN_HW_ConfigurePn(baseAddr, pnConfig);
       CAN_HW_SetPnEnable(baseAddr, true);
   }
   else
   {
       CAN_HW_SetPnEnable(baseAddr, false);
   }

    if (!freezeStatus)
    {
        CAN_HW_ExitFreezeMode(baseAddr);
    }
}

/*!
 * @brief   Get the CAN frame which triggered the wakeup event
 *
 * @param   index       CAN controller index
 * @param   wmbIndex    The index of the message buffer to be extracted
 * @param   wmbPtr      Pointer to the structure where the frame to be stored
 *
 * @retval  None
 */
void CAN_GetWakeupMessage(
    uint8_t index,
    uint8_t wmbIndex,
    CAN_MSG_BUF_T *wmbPtr)
{
    CAN_VALIDATE_INSTANCE(index);

    uint32_t wmbData;
    uint32_t *tmp;
    const CAN_T *baseAddr =  g_canBaseAddress[index];

    wmbData = baseAddr->WMB[wmbIndex].WMBn_LDATA.reg;
    tmp = (uint32_t *)&wmbPtr->data[0];
    CAN_SWAP_BYTES_IN_WORD(wmbData, *tmp);

    wmbData = baseAddr->WMB[wmbIndex].WMBn_HDATA.reg;
    tmp = (uint32_t *)&wmbPtr->data[4];
    CAN_SWAP_BYTES_IN_WORD(wmbData, *tmp);

    wmbPtr->cs = baseAddr->WMB[wmbIndex].WMBn_CS.reg;

    if ((wmbPtr->cs & CAN_CS_IDE_MASK) == 0U)
    {
        wmbPtr->messageId = baseAddr->WMB[wmbIndex].WMBn_ID.reg >> CAN_ID_STD_SHIFT;
    }
    else
    {
        wmbPtr->messageId = baseAddr->WMB[wmbIndex].WMBn_ID.reg;
    }

    wmbPtr->dataLen = (uint8_t)((wmbPtr->cs & CAN_CS_DLC_MASK) >> 16);
}

#if (CAN_INSTANCE_COUNT > 0U)
/*!
 * @brief   CAN0 IRQ handler for OR'ed interrupts (Bus Off, Transmit Warning,
 *          Receive Warning)
 */
void CAN0_ORed_IRQHandler(void)
{
    CAN_IRQHandlerBusOff(0U);
}

/*!
 * @brief   CAN0 IRQ handler for interrupts indicating that errors were
 *          detected on the CAN bus
 */
void CAN0_Error_IRQHandler(void)
{
    CAN_IRQHandlerError(0U);
}

/*!
 * @brief   CAN0 IRQ handler for interrupts indicating a wakeup event
 */
void CAN0_Wake_Up_IRQHandler(void)
{
    CAN_IRQHandlerWakeup(0U);
}

/*!
 * @brief   CAN0 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 0 - 15
 */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    CAN_IRQHandler(0U);
}

/*!
 * @brief   CAN0 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 16 - 31
 */
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    CAN_IRQHandler(0U);
}
#endif /* CAN_INSTANCE_COUNT > 0U */

#if (CAN_INSTANCE_COUNT > 1U)
/*!
 * @brief   CAN1 IRQ handler for OR'ed interrupts (Bus Off, Transmit Warning,
 *          Receive Warning)
 */
void CAN1_ORed_IRQHandler(void)
{
    CAN_IRQHandlerBusOff(1U);
}

/*!
 * @brief   CAN1 IRQ handler for interrupts indicating that errors were
 *          detected on the CAN bus
 */
void CAN1_Error_IRQHandler(void)
{
    CAN_IRQHandlerError(1U);
}

/*!
 * @brief   CAN1 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 0 - 15
 */
void CAN1_ORed_0_15_MB_IRQHandler(void)
{
    CAN_IRQHandler(1U);
}

/*!
 * @brief   CAN1 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 16 - 31
 */
void CAN1_ORed_16_31_MB_IRQHandler(void)
{
    CAN_IRQHandler(1U);
}
#endif /* CAN_INSTANCE_COUNT > 1U */

#if (CAN_INSTANCE_COUNT > 2U)
/*!
 * @brief   CAN2 IRQ handler for OR'ed interrupts (Bus Off, Transmit Warning,
 *          Receive Warning)
 */
void CAN2_ORed_IRQHandler(void)
{
    CAN_IRQHandlerBusOff(2U);
}

/*!
 * @brief   CAN2 IRQ handler for interrupts indicating that errors were
 *          detected on the CAN bus
 */
void CAN2_Error_IRQHandler(void)
{
    CAN_IRQHandlerError(2U);
}

/*!
 * @brief   CAN2 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 0 - 15
 */
void CAN2_ORed_0_15_MB_IRQHandler(void)
{
    CAN_IRQHandler(2U);
}

/*!
 * @brief   CAN2 IRQ handler for interrupts indicating a successful
 *          transmission or reception for Message Buffers 16 - 31
 */
void CAN2_ORed_16_31_MB_IRQHandler(void)
{
    CAN_IRQHandler(2U);
}
#endif /* CAN_INSTANCE_COUNT > 2U */

/*******************************************************************************
                            PRIVATE FUNCTIONS
*******************************************************************************/

/*!
 * @brief   Enable CAN interrupts
 *
 * @param   index   CAN controller index
 */
void CAN_EnableIRQs(uint8_t index)
{
    if (g_canWakeupIrq[index] != NotAvail_IRQn)
    {
        INT_SYS_EnableIRQ(g_canWakeupIrq[index]);
    }

    INT_SYS_EnableIRQ(g_canErrorIrq[index]);
    INT_SYS_EnableIRQ(g_canBusOffIrq[index]);

    for (uint8_t i = 0; i < FEATURE_CAN_MB_IRQS_MAX; i++)
    {
        if (g_canOredMbIrq[i][index] != NotAvail_IRQn)
        {
            INT_SYS_EnableIRQ(g_canOredMbIrq[i][index]);
        }
    }
}

/*!
 * @brief   CAN interrupt handler
 *
 * @param   index   CAN controller index
 */
void CAN_IRQHandler(uint8_t index)
{
    uint32_t regFlag = 0;
    uint32_t mbIndex = 0;
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    /* Get the interrupts that are enabled and ready */
    regFlag = CAN_HW_GetMbInterruptStatus(baseAddr, mbIndex);
    while ((regFlag & 1U) == 0U)
    {
        mbIndex++;
        regFlag = CAN_HW_GetMbInterruptStatus(baseAddr, mbIndex);

        if (mbIndex < FEATURE_CAN_MAX_MB_NUM)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    /* Check Tx/Rx interrupt flag and clear the interrupt */
    if (regFlag != 0U)
    {
        if (CAN_HW_IsRxFifoEnabled(baseAddr) && (mbIndex <= FEATURE_CAN_RXFIFO_OVERFLOW))
        {
            CAN_IRQHandlerRxFifo(index, mbIndex);
        }
        else
        {
            /* Check mailbox completed reception */
            if (states->mbHandles[mbIndex].state == CAN_MB_STATE_RX_BUSY)
            {
                CAN_IRQHandlerRxMb(index, mbIndex);
            }
        }

        /* Check mailbox completed transmission */
        if (states->mbHandles[mbIndex].state == CAN_MB_STATE_TX_BUSY)
        {
            if (!(states->mbHandles[mbIndex].isRemote))
            {
                CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
            }
            else
            {
                /**
                 * If the frame was a remote frame, clear the flag only if the
                 * response was not received yet. If the response was received,
                 * leave the flag set in order to be handled later. */
                CAN_MSG_BUF_T mb;
                CAN_HW_LockRxMb(baseAddr, mbIndex);
                CAN_HW_GetMbValues(baseAddr, mbIndex, &mb);
                CAN_HW_UnlockRxMb(baseAddr);

                if (((mb.cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)CAN_CODE_RX_EMPTY)
                {
                    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
                }
            }

            states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;

            if (states->callback != NULL)
            {
                states->callback(index, CAN_EVENT_TX_COMPLETE, mbIndex, states);
            }

            if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
            {
                CAN_FinishDataTransfer(index, mbIndex);
            }
        }

        if (CAN_HW_GetMbInterruptStatus(baseAddr, mbIndex) != 0U)
        {
            if (CAN_HW_IsRxFifoEnabled(baseAddr) && (mbIndex <= FEATURE_CAN_RXFIFO_OVERFLOW))
            {
                if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_IDLE)
                {
                    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
                }
            }
            else
            {
                if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
                {
                    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);
                }
            }
        }
    }
}

/*!
 * @brief   Interrupt handler for Rx message buffer
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 */
void CAN_IRQHandlerRxMb(uint8_t index, uint32_t mbIndex)
{
     CAN_T *baseAddr = g_canBaseAddress[index];
     CAN_STATE_T *states = g_canState[index];

     /* Lock Rx message buffer and RxFIFO*/
     CAN_HW_LockRxMb(baseAddr, mbIndex);

     /* Get Rx message buffer values */
     CAN_HW_GetMbValues(baseAddr, mbIndex, states->mbHandles[mbIndex].msgBuf);

    /* Clear the proper interrupt flag */
    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

    /**
     * The CODE field is updated with an incorrect value when MBx is locked by
     * software for more than 20 CAN bit times and FIFO is enabled.
     */
    if (   (CAN_HW_IsRxFifoEnabled(baseAddr))
        && (((states->mbHandles[mbIndex].msgBuf->cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT)
            == (uint32_t)CAN_CODE_RX_INACTIVE))
    {
        /**
         * Update the code for next sequence move in MB.
         * A CPU write into the C/S word also unlocks the MB.
         */
        volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
        *mbRegion &= ~CAN_CS_CODE_MASK;
        *mbRegion |= (((uint32_t)CAN_CODE_RX_EMPTY) << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    }
    else
    {
        /* Unlock Rx message buffer and and RxFIFO */
        CAN_HW_UnlockRxMb(baseAddr);
    }

     states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;

     if (states->callback != NULL)
     {
         states->callback(index, CAN_EVENT_RX_COMPLETE, mbIndex, states);
     }

     if (states->mbHandles[mbIndex].state == CAN_MB_STATE_IDLE)
     {
         CAN_FinishDataTransfer(index, mbIndex);
     }
}

/*!
 * @brief   Interrupt handler for RxFIFO
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 */
void CAN_IRQHandlerRxFifo(uint8_t index, uint32_t mbIndex)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (mbIndex == FEATURE_CAN_RXFIFO_FRAME_AVAILABLE)
    {
        if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_RX_BUSY)
        {
            /* Get RxFIFO field values */
            CAN_HW_GetRxFifoValues(baseAddr, states->mbHandles[CAN_MB_HANDLE_RXFIFO].msgBuf);

            CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

            states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;

            if (states->callback != NULL)
            {
                states->callback(index, CAN_EVENT_RXFIFO_COMPLETE, CAN_MB_HANDLE_RXFIFO, states);
            }

            if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_IDLE)
            {
                CAN_FinishReceiveFifoData(index);
            }
        }
    }
    else if (mbIndex == FEATURE_CAN_RXFIFO_WARNING)
    {
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

        if (states->callback != NULL)
        {
            states->callback(index, CAN_EVENT_RXFIFO_WARNING, CAN_MB_HANDLE_RXFIFO, states);
        }
    }
    else if (mbIndex == FEATURE_CAN_RXFIFO_OVERFLOW)
    {
        CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

        if (states->callback != NULL)
        {
            states->callback(index, CAN_EVENT_RXFIFO_OVERFLOW, CAN_MB_HANDLE_RXFIFO, states);
        }
    }
    else
    {
    }
}

/*!
 * @brief   Bus Off and Tx/Rx Warning interrupt handler
 *
 * @param   index   CAN controller index
 */
void CAN_IRQHandlerBusOff(uint8_t index)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (states->errorCallback != NULL)
    {
        states->errorCallback(index, CAN_EVENT_ERROR, states);
    }

    /* Clear all Bus Off and Tx/Rx Warning interrupt flags */
    CAN_HW_ClearBusOffInterruptFlag(baseAddr);
}

/*!
 * @brief   Error interrupt handler
 *
 * @param   index   CAN controller index
 */
void CAN_IRQHandlerError(uint8_t index)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (states->errorCallback != NULL)
    {
        states->errorCallback(index, CAN_EVENT_ERROR, states);
    }

    /* Clear all other interrupts */
    CAN_HW_ClearErrorInterruptFlag(baseAddr);
}

/*!
 * @brief   Wakeup handler
 *
 * @param   index   CAN controller index
 *
 */
void CAN_IRQHandlerWakeup(uint8_t index)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (CAN_HW_IsPnEnabled(baseAddr))
    {
        if (CAN_HW_GetWakeupByMatchFlag(baseAddr) != 0U)
        {
            CAN_HW_ClearWakeupByMatchFlag(baseAddr);

            if (states->callback != NULL)
            {
                states->callback(index, CAN_EVENT_WAKEUP_MATCH, 0U, states);
            }
        }

        if (CAN_HW_GetWakeupByTimeoutFlag(baseAddr) != 0U)
        {
            CAN_HW_ClearWakeupByTimeoutFlag(baseAddr);

            if (states->callback != NULL)
            {
                states->callback(index, CAN_EVENT_WAKEUP_TIMEOUT, 0U, states);
            }
        }
    }
}

/*!
 * @brief   Convert bitrate (Kbit/s) to time segment values
 *
 * @param   bitrate     CAN bitrate
 * @param   clockFreq   Clock frequency
 * @param   timeSeg     Pointer to the time segment structure
 *
 * @retval  The real bitrate
 */
uint32_t CAN_BitrateToTimeSeg(
    uint32_t bitrate,
    uint32_t clockFreq,
    CAN_TIME_SEGMENT_T *timeSeg)
{
    uint32_t samplePoint = 88U;
    uint32_t dataBitrateMin = 1000000U;
    uint32_t dataSampleMin = 100U;
    uint32_t tempBitrate = 0;
    uint32_t presdiv = 0;
    uint32_t propseg = 0U;
    uint32_t pseg1 = 0U;
    uint32_t pseg2 = 0U;
    uint32_t dataBitrate;
    uint32_t tempPresdiv;
    uint32_t tempSample;
    uint32_t dataSample;
    uint32_t numTq;
    uint32_t tSeg1;
    uint32_t tSeg2;
    uint32_t tempPseg1;
    uint32_t tempPseg2;
    uint32_t tempPropseg;

    for (tempPresdiv = 0U; tempPresdiv < CAN_PRE_DIV_MAX; tempPresdiv++)
    {
        /* Calculate the number of time quanta in 1 bit time */
        numTq = clockFreq / ((tempPresdiv + 1U) * bitrate);

        /* Calculate the real bitrate */
        tempBitrate = clockFreq / ((tempPresdiv + 1U) * numTq);

        /* The number of time quanta in 1 bit time must be lower than the one supported */
        if ((numTq >= CAN_NUM_TQ_MIN) && (numTq < CAN_NUM_TQ_MAX))
        {
            /* Calculate time segments based on the value of the sampling point */
            tSeg1 = (numTq * samplePoint / 100U) - 1U;
            tSeg2 = numTq - 1U - tSeg1;

            /* Adjust time segment 1 and time segment 2 */
            CAN_CalculateTseg(&tSeg1, &tSeg2);

            tempPseg2 = tSeg2 - 1U;

            /* Start from pseg1 = pseg2 and adjust until pseg is valid */
            tempPseg1 = tempPseg2;
            tempPropseg = tSeg1 - tempPseg1 - 2U;

            CAN_CalculatePseg(&tempPropseg, &tempPseg1);

            if (((tSeg1 >= CAN_TSEG1_MAX)
                    || (tSeg2 >= CAN_TSEG2_MAX)
                    || (tSeg2 < CAN_TSEG2_MIN)
                    || (tSeg1 < CAN_TSEG1_MIN))
                || ((tempPropseg >= CAN_PROP_SEG_MAX)
                    || (tempPseg1 >= CAN_PSEG1_MAX)
                    || (tempPseg2 < CAN_PSEG2_MIN)
                    || (tempPseg2 >= CAN_PSEG2_MAX)))
            {
                continue;
            }

            tempSample = ((tSeg1 + 1U) * 100U) / numTq;
            dataSample = CAN_GetSampleValue(tempSample , samplePoint);
            dataBitrate = CAN_GetBitrateValue(tempBitrate , bitrate);

            if ((dataBitrate < dataBitrateMin)
                || ((dataBitrate == dataBitrateMin) && (dataSample < dataSampleMin)))
            {
                dataSampleMin = dataSample;
                dataBitrateMin = dataBitrate;
                pseg1 = tempPseg1;
                pseg2 = tempPseg2;
                presdiv = tempPresdiv;
                propseg = tempPropseg;

                if ((dataBitrate == 0U) && (dataSample <= 1U))
                {
                    break;
                }
            }
        }
    }

    timeSeg->preDiv = presdiv;
    timeSeg->propSeg = propseg;
    timeSeg->phaseSeg1 = pseg1;
    timeSeg->phaseSeg2 = pseg2;
    timeSeg->resyncJumpWidth = CAN_GetJumpWidthValue(pseg1);

    return tempBitrate;
}

/*!
 * @brief   Calculate Tseg value
 */
void CAN_CalculateTseg(uint32_t * tSeg1Ptr, uint32_t * tSeg2Ptr)
{
    /* Adjust time segment 1 and time segment 2 */
    while ((*tSeg1Ptr >= CAN_TSEG1_MAX) || (*tSeg2Ptr < CAN_TSEG2_MIN))
    {
        *tSeg2Ptr = *tSeg2Ptr + 1U;
        *tSeg1Ptr = *tSeg1Ptr - 1U;
    }
}

/*!
 * @brief   Calculate Pseg value
 */
void CAN_CalculatePseg(uint32_t * tmpPropseg1, uint32_t * tmpPseg1)
{
    while (*tmpPropseg1 <= 0U)
    {
        *tmpPropseg1 = *tmpPropseg1 + 1U;
        *tmpPseg1 = *tmpPseg1 - 1U;
    }

    while (*tmpPropseg1 >= CAN_PROP_SEG_MAX)
    {
        *tmpPropseg1 = *tmpPropseg1 - 1U;
        *tmpPseg1 = *tmpPseg1 + 1U;
    }
}

/*!
 * @brief   Get the sample value
 */
uint32_t CAN_GetSampleValue(uint32_t tmpSample, uint32_t samplePoint)
{
    if (tmpSample > samplePoint)
    {
        return (tmpSample - samplePoint);
    }
    return (samplePoint - tmpSample);
}

/*!
 * @brief   Get the bitrate value
 */
uint32_t CAN_GetBitrateValue(uint32_t tmpBitrate, uint32_t bitrate)
{
    if (tmpBitrate > bitrate)
    {
        return (tmpBitrate - bitrate);
    }
    return (bitrate - tmpBitrate);
}

/*!
 * @brief   Get the jump width value
 */
uint32_t CAN_GetJumpWidthValue(uint32_t pseg)
{
    if (pseg < CAN_RJW_MAX)
    {
        return pseg;
    }
    return CAN_RJW_MAX;
}

/*!
 * @brief   Get the payload size from DLC
 *
 * @param   dlc Data length
 *
 * @retval  Payload size
 */
uint8_t CAN_GetPayloadSize(uint8_t dlc)
{
    uint8_t payloadSize = 0U;

    if (dlc <= 8U)
    {
        payloadSize = dlc;
    }
    else
    {
       if (dlc == CAN_DLC_VALUE_12_BYTES)
        {
            payloadSize = 12U;
        }
        else if (dlc == CAN_DLC_VALUE_16_BYTES)
        {
            payloadSize = 16U;
        }
        else if (dlc == CAN_DLC_VALUE_20_BYTES)
        {
            payloadSize = 20U;
        }
        else if (dlc == CAN_DLC_VALUE_24_BYTES)
        {
            payloadSize = 24U;
        }
        else if (dlc == CAN_DLC_VALUE_32_BYTES)
        {
            payloadSize = 32U;
        }
        else if (dlc == CAN_DLC_VALUE_48_BYTES)
        {
            payloadSize = 48U;
        }
        else if (dlc == CAN_DLC_VALUE_64_BYTES)
        {
            payloadSize = 64U;
        }
    }
    return payloadSize;
}

/*!
 * @brief   Calculate DLC field value given a payload size (in bytes)
 *
 * @param   payloadSize The payload size
 *
 * @retval  The DLC value
 */
uint8_t CAN_CalculateDlcValue(uint8_t payloadSize)
{
    uint32_t ret = 0xFFU;

    static const uint8_t payloadCode[65] = {
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
        /* 9 to 12 payload have DLC Code 12 Bytes */
        CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES,
        /* 13 to 16 payload have DLC Code 16 Bytes */
        CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES,
        /* 17 to 20 payload have DLC Code 20 Bytes */
        CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES,
        /* 21 to 24 payload have DLC Code 24 Bytes */
        CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES,
        /* 25 to 32 payload have DLC Code 32 Bytes */
        CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES,
        CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES,
        /* 33 to 48 payload have DLC Code 48 Bytes */
        CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
        CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
        CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
        CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
        /* 49 to 64 payload have DLC Code 64 Bytes */
        CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
        CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
        CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
        CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES
    };

    if (payloadSize <= 64U)
    {
        ret = payloadCode[payloadSize];
    }

    return (uint8_t)ret;
}

/*!
 * @brief   Return the last message buffer occupied by the RxFIFO
 *
 * @param   numFilters  Number of RxFIFO Filters
 *
 * @retval  Number of the last MB occupied by the RxFIFO
 *
 */
uint32_t CAN_LastMbOccupiedByRxFifo(uint32_t numFilters)
{
    return (5U + ((((numFilters) + 1U) * 8U) / 4U));
}

/*!
 * @brief   Start transmit by beginning the process of sending data
 *
 * @param   index       CAN controller index
 * @param   mbIndex     Index of the message buffer
 * @param   txInfo      Information of the CAN frame
 * @param   canId       CAN message ID
 * @param   payload     Payload of the CAN frame
 * @param   isBlocking  Blocking or not
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_StartSendMb(
    uint8_t index,
    uint8_t mbIndex,
    const CAN_DATA_INFO_T *txInfo,
    uint32_t canId,
    const uint8_t *payload,
    bool isBlocking)
{
    STATUS_T result;
    CAN_MB_CODE_STATUS_T mbcs;
    CAN_STATE_T *states = g_canState[index];
    CAN_T *baseAddr = g_canBaseAddress[index];

    if (mbIndex >= CAN_HW_GetMaxMbNum(baseAddr))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    if (states->mbHandles[mbIndex].state != CAN_MB_STATE_IDLE)
    {
        return STATUS_BUSY;
    }

    CAN_HW_ClearMbInterruptFlag(baseAddr, mbIndex);

    states->mbHandles[mbIndex].state = CAN_MB_STATE_TX_BUSY;
    states->mbHandles[mbIndex].isRemote = txInfo->isRemote;
    states->mbHandles[mbIndex].isBlocking = isBlocking;
    mbcs.fdEnable = txInfo->fdEnable;
    mbcs.brsEnable = txInfo->brsEnable;
    mbcs.fdPadding = txInfo->fdPadding;
    mbcs.msgIdType = txInfo->msgIdType;
    mbcs.dataLen = txInfo->dataLen;

    if (txInfo->isRemote)
    {
        mbcs.code = (uint32_t)CAN_CODE_TX_REMOTE;
    }
    else
    {
        mbcs.code = (uint32_t)CAN_CODE_TX_DATA;
    }

    result = CAN_HW_SetTxMb(baseAddr, mbIndex, &mbcs, canId, payload, false);
    if (result != STATUS_SUCCESS)
    {
        states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;
    }
    return result;
}

/*!
 * @brief   Start receive by beginning the process of receiving data and
            enabling the interrupt
 *
 * @param   index       CAN controller index
 * @param   mbIndex     Index of the message buffer
 * @param   msgBuf      Buffer to store the received CAN frame
 * @param   isBlocking  Blocking or not
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_StartReceiveMb(
    uint8_t index,
    uint8_t mbIndex,
    CAN_MSG_BUF_T *msgBuf,
    bool isBlocking)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (mbIndex >= CAN_HW_GetMaxMbNum(baseAddr))
    {
        return STATUS_CAN_MB_OUT_OF_RANGE;
    }

    /* Check if RxFIFO is enabled */
    if (CAN_HW_IsRxFifoEnabled(baseAddr))
    {
        /* Get the number of RxFIFO filters */
        uint32_t numFilters = (uint32_t)(baseAddr->CTRL2.bit.NUMFIFO);

        /* Get the number of MBs occupied by RxFIFO and ID filter table */
        uint32_t numMbs = CAN_LastMbOccupiedByRxFifo(numFilters);

        if (mbIndex <= numMbs)
        {
            return STATUS_CAN_MB_OUT_OF_RANGE;
        }
    }

    /* Start receiving mailbox */
    if (states->mbHandles[mbIndex].state != CAN_MB_STATE_IDLE)
    {
        return STATUS_BUSY;
    }
    states->mbHandles[mbIndex].state = CAN_MB_STATE_RX_BUSY;
    states->mbHandles[mbIndex].msgBuf = msgBuf;
    states->mbHandles[mbIndex].isBlocking = isBlocking;

    CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, true);
    return STATUS_SUCCESS;
}

/*!
 * @brief   Finish data transfer by completing the process of sending data
 *          and disabling the interrupt
 *
 * @param   index   CAN controller index
 * @param   mbIndex Index of the message buffer
 *
 * @retval  None
 */
void CAN_FinishDataTransfer(uint8_t index, uint32_t mbIndex)
{
    CAN_STATE_T *states = g_canState[index];
    CAN_T *baseAddr = g_canBaseAddress[index];

    /* Disable the transmitter data register empty interrupt */
    CAN_HW_SetMbInterruptEnable(baseAddr, mbIndex, false);

    /* Update the information of the driver states */
    if (states->mbHandles[mbIndex].isBlocking)
    {
        (void)OSIF_SemPost(&states->mbHandles[mbIndex].sem);
    }
    states->mbHandles[mbIndex].state = CAN_MB_STATE_IDLE;
}

/*!
 * @brief   Start receive from the RxFIFO
 *
 * @param   index       CAN controller index
 * @param   msgBuf      Buffer to store the received CAN frame
 * @param   isBlocking  Blocking or not
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_BUSY:    Resource is busy
 */
STATUS_T CAN_StartReceiveFromFifo(
    uint8_t index,
    CAN_MSG_BUF_T *msgBuf,
    bool isBlocking)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (states->rxFifoTransferType == CAN_RXFIFO_USE_DMA)
    {
        if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_DMA_ERROR)
        {
            /**
             * Check if the RxFIFO has pending request that generated error,
             * the RxFIFO need to be empty to activate DMA.
             */
            if (CAN_HW_GetMbInterruptFlag(baseAddr,FEATURE_CAN_RXFIFO_FRAME_AVAILABLE) == (uint8_t)1U)
            {
                /* Enter freeze mode to clear RxFIFO */
                CAN_HW_EnterFreezeMode(baseAddr);

                CAN_HW_ClearRxFifo(baseAddr);
                do
                {   /* Read offset 0x8C to clear DMA pending request */
                    (void)baseAddr->RAMn[3];
                } while (CAN_HW_GetMbInterruptFlag(baseAddr,FEATURE_CAN_RXFIFO_FRAME_AVAILABLE)
                         == (uint8_t)1U);

                /* Return to normal mode */
                CAN_HW_ExitFreezeMode(baseAddr);
            }
            /* Change status of MB to be reconfigured with DMA transfer */
            states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;
        }
    }

    if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state != CAN_MB_STATE_IDLE)
    {
        return STATUS_BUSY;
    }

    /* Check if RxFIFO is enabled */
    if (!CAN_HW_IsRxFifoEnabled(baseAddr))
    {
        return STATUS_ERROR;
    }

    states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_RX_BUSY;
    states->mbHandles[CAN_MB_HANDLE_RXFIFO].isBlocking = isBlocking;

    /* This will get filled by the interrupt handler */
    states->mbHandles[CAN_MB_HANDLE_RXFIFO].msgBuf = msgBuf;

    if (states->rxFifoTransferType == CAN_RXFIFO_USE_INTERRUPT)
    {
        /* Enable RxFIFO interrupts */
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, true);
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_WARNING, true);
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_OVERFLOW, true);
    }

    if (states->rxFifoTransferType == CAN_RXFIFO_USE_DMA)
    {
        STATUS_T dmaStatus = DMA_RegisterCallback(states->rxFifoDmaChannel,
                                                  CAN_CompleteRxFifoDataDMA,
                                                  (void *)((uint32_t)index));
        if (dmaStatus != STATUS_SUCCESS)
        {
            states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;
            return STATUS_ERROR;
        }

        dmaStatus = DMA_ConfigSingleBlockTransfer(
                        states->rxFifoDmaChannel,
                        DMA_TRANSFER_MEM2MEM,
                        (uint32_t)(baseAddr->RAMn),
                        (uint32_t)(states->mbHandles[CAN_MB_HANDLE_RXFIFO].msgBuf),
                        DMA_TRANSFER_SIZE_4B,
                        16U);
        if (dmaStatus != STATUS_SUCCESS)
        {
            states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;
            return STATUS_ERROR;
        }

        DMA_DisableRequestsOnTransferComplete(states->rxFifoDmaChannel, true);

        dmaStatus = DMA_StartChannel(states->rxFifoDmaChannel);
        if (dmaStatus != STATUS_SUCCESS)
        {
            states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief   Finish receive data from the RxFIFO
 *
 * @param   index   CAN controller index
 */
void CAN_FinishReceiveFifoData(uint8_t index)
{
    CAN_T *baseAddr = g_canBaseAddress[index];
    CAN_STATE_T *states = g_canState[index];

    if (states->rxFifoTransferType == CAN_RXFIFO_USE_INTERRUPT)
    {
        /* Disable RxFIFO interrupts */
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, false);
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_OVERFLOW, false);
        CAN_HW_SetMbInterruptEnable(baseAddr, FEATURE_CAN_RXFIFO_WARNING, false);
    }

    else
    {
        if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state != CAN_MB_STATE_DMA_ERROR)
        {
            CAN_MSG_BUF_T *fifoMsg = states->mbHandles[CAN_MB_HANDLE_RXFIFO].msgBuf;
            uint32_t *msgDataPtr = (uint32_t *)fifoMsg->data;

            (void)DMA_StopChannel(states->rxFifoDmaChannel);

            /* Adjust the ID if it is not extended */
            if (((fifoMsg->cs) & CAN_CS_IDE_MASK) == 0U)
            {
                fifoMsg->messageId = fifoMsg->messageId >> CAN_ID_STD_SHIFT;
            }

            /* Extract the data length */
            fifoMsg->dataLen = (uint8_t)((fifoMsg->cs & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

            /* Reverse the endianness */
            CAN_SWAP_BYTES_IN_WORD(msgDataPtr[1], msgDataPtr[1]);
            CAN_SWAP_BYTES_IN_WORD(msgDataPtr[0], msgDataPtr[0]);
        }
    }

    /* Clear the RxFIFO message */
    states->mbHandles[CAN_MB_HANDLE_RXFIFO].msgBuf = NULL;

    if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state != CAN_MB_STATE_DMA_ERROR)
    {
        states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_IDLE;

        if ((states->callback != NULL)
            && (states->rxFifoTransferType == CAN_RXFIFO_USE_DMA))
        {
            states->callback(index, CAN_EVENT_DMA_COMPLETE, CAN_MB_HANDLE_RXFIFO, states);
        }
    }

    /* Update the status */
    if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].isBlocking)
    {
        if (states->mbHandles[CAN_MB_HANDLE_RXFIFO].state == CAN_MB_STATE_IDLE)
        {
            (void)OSIF_SemPost(&states->mbHandles[CAN_MB_HANDLE_RXFIFO].sem);
        }
    }
}

/*!
 * @brief   Finish DMA transfer
 *
 * @param   param           CAN controller index
 * @param   channelStatus   DMA channel status
 */
void CAN_CompleteRxFifoDataDMA(void *param, DMA_CHANNEL_STATUS_T channelStatus)
{
    uint32_t index = (uint32_t)param;

    if (channelStatus == DMA_CHANNEL_ERROR)
    {
        CAN_STATE_T *states = g_canState[index];

        states->mbHandles[CAN_MB_HANDLE_RXFIFO].state = CAN_MB_STATE_DMA_ERROR;

        if (states->callback != NULL)
        {
            states->callback((uint8_t)index, CAN_EVENT_DMA_ERROR, CAN_MB_HANDLE_RXFIFO, states);
        }
    }
    CAN_FinishReceiveFifoData((uint8_t)index);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initializes the CAN controller
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_Init(CAN_T *baseAddr)
{
    /* Reset the CAN controller */
    baseAddr->MCFG.bit.CANRST = CAN_MCFG_CANRST_1;

    /* Wait for reset cycle to complete */
    while (baseAddr->MCFG.bit.CANRST != 0U) {}

    /* Enable abort transmission */
    baseAddr->MCFG.bit.TAEN = CAN_MCFG_TAEN_1;

    /* Clear CAN memory */
    CAN_HW_ClearRam(baseAddr);

    /* Set Rx global mask */
    (baseAddr->RXMASK.reg) = 0xFFFFFFFFU;

    /* Set Rx 14 mask register */
    (baseAddr->RX14MASK.reg) = 0xFFFFFFFFU;

    /* Set Rx 15 mask register */
    (baseAddr->RX15MASK.reg) = 0xFFFFFFFFU;

    /* Disable all MB interrupts */
    (baseAddr->MBiIEN.reg) = 0x0U;

    /* Clear all MB interrupt flags */
    (baseAddr->IFLG.reg) = 0xFFFFFFFFU;

    /* Clear all error interrupt flags */
    (baseAddr->ERRFLG1.reg) = CAN_ALL_INTERRUPTS;
}

/*!
 * @brief   Enable CAN module
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_Enable(CAN_T *baseAddr)
{
    /* Check for low power mode */
    if (baseAddr->MCFG.bit.LPM == 1U)
    {
        baseAddr->MCFG.bit.CMD = CAN_MCFG_CMD_0;
        baseAddr->MCFG.bit.FRZEN = CAN_MCFG_FRZEN_0;
        baseAddr->MCFG.bit.EFRZ = CAN_MCFG_EFRZ_0;

        /* Wait until enabled */
        while (baseAddr->MCFG.bit.NRDY != 0U) {}
    }
}

/*!
 * @brief   Disable CAN module
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_Disable(CAN_T *baseAddr)
{
    if (baseAddr->MCFG.bit.CMD == 0U)
    {
        baseAddr->MCFG.bit.CMD = CAN_MCFG_CMD_1;

        /* Wait until disable mode acknowledged */
        while (baseAddr->MCFG.bit.LPM == 0U) {}
    }
}

/*!
 * @brief   Checks if the CAN is enabled
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  true:   Enabled
 *          false:  Disabled
 *
 */
bool CAN_HW_IsEnabled(const CAN_T *baseAddr)
{
    return ((baseAddr->MCFG.bit.CMD != 0U) ? false : true);
}

 /*!
 * @brief   Enable a CAN operation mode
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mode        Operation mode
 *
 * @retval  None
 */
void CAN_HW_SetOperationMode(CAN_T *baseAddr, CAN_OPERATION_MODE_T mode)
{
    if (mode == CAN_MODE_FREEZE)
    {
        CAN_HW_EnterFreezeMode(baseAddr);
    }
    else if (mode == CAN_MODE_DISABLE)
    {
        baseAddr->MCFG.bit.CMD = CAN_MCFG_CMD_1;
    }
    else if (mode == CAN_MODE_NORMAL)
    {
        baseAddr->MCFG.bit.SVEN = (uint32_t)0U;
        baseAddr->CTRL1.bit.LOEN = CAN_CTRL1_LOEN_0;
        baseAddr->CTRL1.bit.LBEN = CAN_CTRL1_LBEN_0;
    }
    else if (mode == CAN_MODE_LISTEN_ONLY)
    {
        baseAddr->CTRL1.bit.LOEN = CAN_CTRL1_LOEN_1;
    }
    else if (mode == CAN_MODE_LOOPBACK)
    {
        baseAddr->CTRL1.bit.LBEN = CAN_CTRL1_LBEN_1;
        baseAddr->CTRL1.bit.LOEN = CAN_CTRL1_LOEN_0;
        CAN_HW_SetSelfReceptionEnable(baseAddr, true);
    }
    else
    {
    }
}

/*!
 * @brief   Enable or disable the self reception
 * @details CAN is allowed to receive frames transmitted by itself if enabled.
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   enable      Enable or disable
 *
 * @retval  None
 */
void CAN_HW_SetSelfReceptionEnable(CAN_T *baseAddr, bool enable)
{
    baseAddr->MCFG.bit.SRD = (uint32_t)(enable ? CAN_MCFG_SRD_0 : CAN_MCFG_SRD_1);
}

/*!
 * @brief   Get the maximum number of message buffers
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  The configured number of message buffers
 *
 */
uint32_t CAN_HW_GetLastMbNumber(const CAN_T *baseAddr)
{
    return (baseAddr->MCFG.bit.LASTMB);
}

/*!
 * @brief   Set the number of the last message buffers
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   maxMbNumber Maximum MB number
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_HW_SetLastMbNumber(CAN_T *baseAddr, uint32_t maxMbNumber)
{
    STATUS_T result = STATUS_SUCCESS;
    uint8_t mbIndex;
    uint32_t dataByte;

    uint8_t arbitrationFieldSize = 8U;
    uint8_t payloadSize = CAN_HW_GetFdPayloadSize(baseAddr);
    volatile uint32_t * mbRegion = CAN_HW_GetMbRegion(baseAddr, (maxMbNumber - 1U));
    uint32_t endMb = (uint32_t)mbRegion + payloadSize + arbitrationFieldSize;

    /* Check that the number of MBs is supported based on the payload size */
    if (   (endMb > ((uint32_t)&baseAddr->RAMn[CAN_RAMn_COUNT-1].reg)+4)
        || (maxMbNumber > CAN_HW_GetMaxMbNum(baseAddr)))
    {
        result = STATUS_CAN_MB_OUT_OF_RANGE;
    }

    if (result == STATUS_SUCCESS)
    {
        /* Set the maximum number of MBs */
        baseAddr->MCFG.bit.LASTMB = maxMbNumber - 1U;
        if (!CAN_HW_IsRxFifoEnabled(baseAddr))
        {
            /* Initialize all message buffers as inactive */
            for (mbIndex = 0; mbIndex < maxMbNumber; mbIndex++)
            {
                volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
                volatile uint32_t *mbId = &mbRegion[1];
                volatile uint8_t  *mbData = (volatile uint8_t *)(&mbRegion[2]);

                *mbRegion = 0x0;
                *mbId = 0x0;
                for (dataByte = 0; dataByte < payloadSize; dataByte++)
                {
                   mbData[dataByte] = 0x0;
                }
            }
        }
    }
    return result;
}

/*!
 * @brief   Get the CAN time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CAN time segments
 *
 * @retval  None
 */
void CAN_HW_GetTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg)
{
    timeSeg->preDiv = baseAddr->CTRL1.bit.PDF;
    timeSeg->propSeg = baseAddr->CTRL1.bit.PTS;
    timeSeg->phaseSeg1 = baseAddr->CTRL1.bit.PBS1;
    timeSeg->phaseSeg2 = baseAddr->CTRL1.bit.PBS2;
    timeSeg->resyncJumpWidth = baseAddr->CTRL1.bit.SJW;
}

/*!
 * @brief   Set the CAN time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CAN time segments
 *
 * @retval  None
 */
void CAN_HW_SetTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg)
{
    baseAddr->CTRL1.bit.PDF = timeSeg->preDiv;
    baseAddr->CTRL1.bit.PTS = timeSeg->propSeg;
    baseAddr->CTRL1.bit.PBS1 = timeSeg->phaseSeg1;
    baseAddr->CTRL1.bit.PBS2 = timeSeg->phaseSeg2;
    baseAddr->CTRL1.bit.SJW = timeSeg->resyncJumpWidth;
}

/*!
 * @brief   Set Rx masking type (RX global mask or RX individual mask)
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   maskType    CAN Rx mask type
 *
 * @retval  None
 *
 */
void CAN_HW_SetRxMaskType(CAN_T *baseAddr, CAN_RX_MASK_TYPE_T maskType)
{
    if (maskType == CAN_RX_MASK_GLOBAL)
    {
        baseAddr->MCFG.bit.IRXMQEN = CAN_MCFG_IRXMQEN_0;
    }
    else
    {
        baseAddr->MCFG.bit.IRXMQEN = CAN_MCFG_IRXMQEN_1;
    }
}

/*!
 * @brief   Set the Rx individual mask for ID filtering in the Rx Message
 *          Buffers and the RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   mask        Individual mask
 *
 * @retval  None
 */
void CAN_HW_SetRxIndividualMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t mask)
{
    baseAddr->RXIMASK[mbIndex].reg = mask;
}

/*!
 * @brief   Set the RxFIFO global mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mask        Global mask
 *
 * @retval  None
 */
void CAN_HW_SetRxFifoGlobalMask(CAN_T *baseAddr, uint32_t mask)
{
    baseAddr->FIFOGMASK.reg = mask;
}

/*!
 * @brief   Get freeze status of the CAN controller
 *
 * @param   baseAddr     The CAN baseAddr address
 *
 * @retval  true:   Module is in freeze mode
 *          false:  Module is not in freeze mode
 */
bool CAN_HW_GetFreezeStatus(const CAN_T *baseAddr)
{
    return (baseAddr->MCFG.bit.FRZFLG == CAN_MCFG_FRZFLG_1) ? true : false;
}

 /*!
 * @brief   Enter the freeze mode
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_EnterFreezeMode(CAN_T *baseAddr)
{
    bool enabled = false;
    uint32_t aux = 0U;

    baseAddr->MCFG.bit.FRZEN = CAN_MCFG_FRZEN_1;
    baseAddr->MCFG.bit.EFRZ = CAN_MCFG_EFRZ_1;

    if (baseAddr->MCFG.bit.CMD == CAN_MCFG_CMD_0)
    {
        enabled = true;
    }
    else
    {
        baseAddr->MCFG.bit.CMD = CAN_MCFG_CMD_0;
    }
    baseAddr->FRTMR.reg = 0U;

    /* MCFG[CANFDEN] was reset to 0, wait for timeout */
    if (!CAN_HW_IsFdEnabled(baseAddr))
    {
        while (!CAN_HW_GetFreezeStatus(baseAddr) && (aux < 180U))
        {
            /* Wait until finish counting 180 bit times and exit */
            aux = (uint32_t)baseAddr->FRTMR.reg;
        }
    }
    else
    {
        while (!CAN_HW_GetFreezeStatus(baseAddr) && (aux < 730U))
        {
            /* Wait until finish counting 730 bit times and exit */
            aux = (uint32_t)baseAddr->FRTMR.reg;
        }
    }

    if (baseAddr->MCFG.bit.FRZFLG  == CAN_MCFG_FRZFLG_0)
    {
        /* Save registers before soft reset */
        uint32_t tempMBiIEN = baseAddr->MBiIEN.reg;
        uint32_t tempMCFG = baseAddr->MCFG.reg;

        /* Do soft reset */
        baseAddr->MCFG.bit.CANRST = CAN_MCFG_CANRST_1;
        while ((baseAddr->MCFG.bit.CANRST) != CAN_MCFG_CANRST_0) {}

        /* Restore register values */
        baseAddr->MBiIEN.reg = tempMBiIEN;
        baseAddr->MCFG.reg = tempMCFG;
    }

    if (!enabled)
    {
        baseAddr->MCFG.bit.CMD = CAN_MCFG_CMD_1;

        /* Wait until disable mode is acknowledged */
        while (baseAddr->MCFG.bit.LPM == CAN_MCFG_LPM_0) {}
    }
}

 /*!
 * @brief  Exit freeze mode
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ExitFreezeMode(CAN_T *baseAddr)
{
    baseAddr->MCFG.bit.EFRZ = CAN_MCFG_EFRZ_0;
    baseAddr->MCFG.bit.FRZEN = CAN_MCFG_FRZEN_0;

    /* Wait until exit freeze mode */
    while (baseAddr->MCFG.bit.FRZFLG != CAN_MCFG_FRZFLG_0) {}
}

/*!
 * @brief   Get the individual CAN MB interrupt flag
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  The individual MB interrupt flag
 *
 */
uint8_t CAN_HW_GetMbInterruptStatus(const CAN_T *baseAddr, uint32_t mbIndex)
{
    uint8_t flag = 0;
    uint32_t mask;

    if (mbIndex < 32U)
    {
        mask = baseAddr->MBiIEN.reg & 0xFFFFFFFFU;
        flag = (uint8_t)(((baseAddr->IFLG.reg & mask) >> (mbIndex % 32U)) & 1U);
    }

    return flag;
}

/*!
 * @brief   Get the interrupt enabled or disabled of the MB
 *
 * @param   baseAddr    The CAN baseAddr address
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  1:  Interrupt enabled
 *          0:  Interrupt disabled
 *
 */
uint8_t CAN_HW_GetMbInterruptEnable(const CAN_T *baseAddr, uint32_t mbIndex)
{
    uint32_t flag = 0U;

    if (mbIndex < 32U)
    {
        flag = ((baseAddr->MBiIEN.reg & ((uint32_t)1U << (mbIndex % 32U))) >> (mbIndex % 32U));
    }
    return (uint8_t)flag;
}

 /*!
 * @brief   Enable or disable the MB interrupt
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   enable      Enable or disable
 *
 * @retval  None
 */
void CAN_HW_SetMbInterruptEnable(CAN_T *baseAddr, uint32_t mbIndex, bool enable)
{
    uint32_t temp = 1UL << (mbIndex % 32U);

    /* Enable the corresponding MB interrupt */
    if (mbIndex  < 32U)
    {
        if (enable)
        {
            (baseAddr->MBiIEN.reg) = ((baseAddr->MBiIEN.reg) | (temp));
        }
        else
        {
            (baseAddr->MBiIEN.reg) = ((baseAddr->MBiIEN.reg) & ~(temp));
        }
    }
}

/*!
 * @brief   Clear CAN memory positions that require initialization
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ClearRam(CAN_T *baseAddr)
{
    uint32_t dataByte;
    uint32_t ramSize = CAN_HW_GetMaxMbNum(baseAddr) * 4U;
    uint32_t imaskSize = CAN_HW_GetMaxMbNum(baseAddr);

    for (dataByte = 0; dataByte < ramSize; dataByte++)
    {
        baseAddr->RAMn[dataByte].reg = 0x0;
    }

    for (dataByte = 0; dataByte < imaskSize; dataByte++)
    {
        baseAddr->RXIMASK[dataByte].reg = 0x0;
    }
}

/*!
 * @brief   Clear the RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 *
 */
void CAN_HW_ClearRxFifo(CAN_T *baseAddr)
{
    baseAddr->IFLG.reg = 0x1U;
}

 /*!
 * @brief   Clear all error interrupt status
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ClearErrorInterruptFlag(CAN_T *baseAddr)
{
    if ((baseAddr->ERRFLG1.reg & CAN_ALL_INTERRUPTS) != 0U)
    {
        (baseAddr->ERRFLG1.reg) = CAN_ERROR_INTERRUPT;

        /* Dummy read as a workaround to ensure the flags are cleared before continue */
        (void)(baseAddr->ERRFLG1.reg);
    }
}

 /*!
 * @brief   Clear all bus off and Tx/Rx warning interrupt status
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ClearBusOffInterruptFlag(CAN_T *baseAddr)
{
    if ((baseAddr->ERRFLG1.reg & CAN_BUS_OFF_INTERRUPT) != 0U)
    {
        baseAddr->ERRFLG1.reg = CAN_BUS_OFF_INTERRUPT;

        (void)(baseAddr->ERRFLG1.reg);
    }
}

 /*!
 * @brief  Enable or disable the error interrupts
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   errorIntType    Error interrupt type
 * @param   enable          Enable or disable
 *
 * @retval  None
 *
 */
void CAN_HW_SetErrorInterruptEnable(
    CAN_T *baseAddr,
    CAN_INTERRUPT_TYPE_T errorIntType,
    bool enable)
{
    uint32_t temp = (uint32_t)errorIntType;

    if (enable)
    {
        if (   (errorIntType == CAN_INTERRUPT_RX_WARNING)
            || (errorIntType == CAN_INTERRUPT_TX_WARNING))
        {
           baseAddr->MCFG.bit.WIEN = CAN_MCFG_WIEN_1;
        }

        if (errorIntType == CAN_INTERRUPT_ERROR)
        {
           baseAddr->CTRL2.bit.FDERRIEN = CAN_CTRL2_FDERRIEN_1;
        }

        (baseAddr->CTRL1.reg) = ((baseAddr->CTRL1.reg) | (temp));
    }
    else
    {
        (baseAddr->CTRL1.reg) = ((baseAddr->CTRL1.reg) & ~(temp));

        if (errorIntType == CAN_INTERRUPT_ERROR)
        {
            baseAddr->CTRL2.bit.FDERRIEN = CAN_CTRL2_FDERRIEN_0;
        }

        temp = baseAddr->CTRL1.reg;
        if (   ((temp & (uint32_t)CAN_INTERRUPT_RX_WARNING) == 0U)
            && ((temp & (uint32_t)CAN_INTERRUPT_TX_WARNING) == 0U))
        {
           baseAddr->MCFG.bit.WIEN = CAN_MCFG_WIEN_0;
        }
    }
}

/*!
 * @brief   Get the interrupt flag of the message buffers
 *
 * @param   baseAddr    The CAN baseAddr address
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  The value of interrupt flag of the message buffer
 *
 */
uint8_t CAN_HW_GetMbInterruptFlag(const CAN_T *baseAddr, uint32_t mbIndex)
{
    uint32_t flag = 0U;

    if (mbIndex < 32U)
    {
        flag = ((baseAddr->IFLG.reg & ((uint32_t)1U << (mbIndex % 32U))) >> (mbIndex % 32U));
    }
    return (uint8_t)flag;
}

/*!
 * @brief   Clear the interrupt flag of the message buffers
 *
 * @param   baseAddr    The CAN baseAddr address
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  None
 *
 */
void CAN_HW_ClearMbInterruptFlag(CAN_T *baseAddr, uint32_t mbIndex)
{
    uint32_t flag = ((uint32_t)1U << (mbIndex % 32U));

    if (mbIndex < 32U)
    {
        (baseAddr->IFLG.reg) = (flag);
    }
}

/*!
 * @brief   Set the CAN Rx message buffer global standard mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   stdMask     Standard mask
 *
 * @retval  None
 *
 */
void CAN_HW_SetRxMbGlobalStdMask(CAN_T *baseAddr, uint32_t stdMask)
{
    (baseAddr->RXMASK.reg) = (((uint32_t)(((uint32_t)(stdMask)) << CAN_ID_STD_SHIFT))
                             & CAN_ID_STD_MASK);
}

/*!
 * @brief   Set the CAN Rx message bufferglobal extended mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   extMask     Extended mask
 *
 * @retval  None
 *
 */
void CAN_HW_SetRxMbGlobalExtMask(CAN_T *baseAddr, uint32_t extMask)
{
    (baseAddr->RXMASK.reg) = (((uint32_t)(((uint32_t)(extMask)) << CAN_ID_EXT_SHIFT))
                             & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
}

/*!
 * @brief   Set the CAN Rx individual standard mask for ID filtering in the
 *          Rx MBs and the RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   stdMask     Individual standard mask
 *
 * @retval  None
 *
 */
void CAN_HW_SetRxIndividualStdMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t stdMask)
{
    (baseAddr->RXIMASK[mbIndex].reg) = (stdMask << CAN_ID_STD_SHIFT)
                                       & CAN_ID_STD_MASK;
}

/*!
 * @brief   Set the CAN Rx individual extended mask for ID filtering in the
 *          Rx message buffers and the RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   extMask     Individual extended mask
 *
 * @retval  None
 *
 */
void CAN_HW_SetRxIndividualExtMask(CAN_T *baseAddr, uint32_t mbIndex, uint32_t extMask)
{
    baseAddr->RXIMASK[mbIndex].reg = (extMask << CAN_ID_EXT_SHIFT)
                                     & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
}

/*!
 * @brief   Set the CAN Rx message buffer 14 standard mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   stdMask     Standard mask
 *
 * @retval  None
 */
void CAN_HW_SetRxMb14StdMask(CAN_T *baseAddr, uint32_t stdMask)
{
    (baseAddr->RX14MASK.reg) = (((uint32_t)(((uint32_t)(stdMask)) << CAN_ID_STD_SHIFT))
                               & CAN_ID_STD_MASK);
}

/*!
 * @brief   Set the CAN Rx message buffer 14 extended mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   extMask     Extended mask
 *
 * @retval  None
 */
void CAN_HW_SetRxMb14ExtMask(CAN_T *baseAddr, uint32_t extMask)
{
    (baseAddr->RX14MASK.reg) = (((uint32_t)(((uint32_t)(extMask)) << CAN_ID_EXT_SHIFT))
                               & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
}

/*!
 * @brief   Set the CAN Rx message buffer 15 standard mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   stdMask     Standard mask
 *
 * @retval  None
 */
void CAN_HW_SetRxMb15StdMask(CAN_T *baseAddr, uint32_t stdMask)
{
    (baseAddr->RX15MASK.reg) = (((uint32_t)(((uint32_t)(stdMask)) << CAN_ID_STD_SHIFT))
                               & CAN_ID_STD_MASK);
}

/*!
 * @brief Set the CAN Rx message buffer15 extended mask
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   extMask     Extended mask
 *
 * @retval  None
 */
void CAN_HW_SetRxMb15ExtMask(CAN_T *baseAddr, uint32_t extMask)
{
    (baseAddr->RX15MASK.reg) = (((uint32_t)(((uint32_t)(extMask)) << CAN_ID_EXT_SHIFT))
                               & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
}

/*!
 * @brief   Lock the RX message buffer
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  None
 */
void CAN_HW_LockRxMb(CAN_T *baseAddr, uint32_t mbIndex)
{
    volatile const uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);

    /* Lock the MB by reading it */
    (void)*mbRegion;
}

/*!
 * @brief   Unlocks the Rx message buffer
 *
 * @param   baseAddr    The CAN baseAddr address
 *
 * @retval  None
 */
void CAN_HW_UnlockRxMb(const CAN_T *baseAddr)
{
    /* Unlock the mailbox by reading the free running timer */
    (void)baseAddr->FRTMR.reg;
}

/*!
* @brief   Check if message buffer index is out of range
*
* @param   baseAddr Base address of the CAN controller
* @param   mbIndex  Index of the message buffer
*
* @retval  true:    MB index is out of range
*          false:   MB index is not out of range
*/
bool CAN_HW_IsMbOutOfRange(const CAN_T *baseAddr, uint32_t mbIndex)
{
     /* Check if the MB is valid */
    if (mbIndex > (uint32_t)baseAddr->MCFG.bit.LASTMB)
    {
        return true;
    }

    /* Check if RxFIFO is enabled*/
    if (CAN_HW_IsRxFifoEnabled(baseAddr) == true)
    {
        /* Get the number of RxFIFO filters */
        uint32_t numFilters = (uint32_t)(baseAddr->CTRL2.bit.NUMFIFO);

        /* Get the number of MBs occupied by RxFIFO and ID filter table */
        uint32_t numMbs = CAN_LastMbOccupiedByRxFifo(numFilters);

        if (mbIndex <= numMbs)
        {
            return true;
        }
    }
    return false;
}

/*!
 * @brief   Get the maximum MB number
 *
 * @param   baseAddr Base address of the CAN controller
 *
 * @retval  Maximum MB number
 */
uint32_t CAN_HW_GetMaxMbNum(const CAN_T *baseAddr)
{
    uint32_t ret = 0;
    static const uint32_t maxMbNum[] = FEATURE_CAN_MAX_MB_NUM_ARRAY;

    for (uint32_t i = 0; i < CAN_INSTANCE_COUNT; i++)
    {
        if (baseAddr == g_canBaseAddress[i])
        {
            ret = maxMbNum[i];
        }
    }
    return ret;
}

/*!
 * @brief   Calculate the mask for RxFIFO
 *
 * @param   index       CAN controller index
 * @param   idFormat    ID format
 * @param   mask        Mask value
 *
 * @retval  RxFIFO
 */
uint32_t CAN_HW_CalculateRxFifoMask(
    CAN_MSG_ID_TYPE_T idType,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat,
    uint32_t mask)
{
    uint32_t ret = 0;

    if (idFormat == CAN_RXFIFO_ID_FORMAT_A)
    {
        /* Set RTR bit encoded as bit 31 and IDE bit encoded as bit 30 in mask */
        ret = mask & (  (1UL << CAN_RXFIFO_ID_FILTER_FORMAT_AB_RTR_SHIFT)
                      | (1UL << CAN_RXFIFO_ID_FILTER_FORMAT_AB_IDE_SHIFT));
        if (idType == CAN_ID_STANDARD)
        {
            /* Set standard global mask for RxFIFO and IDE will be 1 and check the FIFO filter IDE */
            ret |= ((mask << CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_SHIFT)
                    & CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_MASK);

        }
        else if (idType == CAN_ID_EXTENDED)
        {
            /* Set extended global mask for RxFIFO and IDE will be 0 and don't check the FIFO filter IDE */
            ret |= ((mask << CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_SHIFT)
                    & CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_MASK);
        }
        else
        {
        }
    }
    else if (idFormat == CAN_RXFIFO_ID_FORMAT_B)
    {
        /* Set RTR bit encoded as bit 31 and IDE bit encoded as bit 30 in mask */
        ret = mask & (  (1UL << CAN_RXFIFO_ID_FILTER_FORMAT_AB_RTR_SHIFT)
                      | (1UL << CAN_RXFIFO_ID_FILTER_FORMAT_AB_IDE_SHIFT));
        if (idType == CAN_ID_STANDARD)
        {
            /* Set standard global mask for RxFIFO  */
            ret |= ((mask & CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_MASK)
                    << CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_SHIFT1);

        }
        else if (idType == CAN_ID_EXTENDED)
        {
            /* Set extended global mask for RxFIFO  */
            ret |= ((mask & CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_MASK1)
                    << CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT1);
        }
        else
        {
        }
    }
    else if (idFormat == CAN_RXFIFO_ID_FORMAT_C)
    {
        if ((idType == CAN_ID_EXTENDED) || (idType == CAN_ID_STANDARD))
        {
            ret |= ((mask & CAN_RXFIFO_ID_FILTER_FORMAT_C_MASK)
                    << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1);
        }
        else
        {
        }
    }
    else
    {
    }
    return ret;
}

/*!
 * @brief   Configure a message buffer for receiving
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   mbStatus    Pointer to MB status structure
 * @param   messageId   CAN message ID
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_HW_SetRxMb(
    CAN_T *baseAddr,
    uint32_t mbIndex,
    const CAN_MB_CODE_STATUS_T *mbStatus,
    uint32_t messageId)
{
    STATUS_T result = STATUS_SUCCESS;
    volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
    volatile uint32_t *mbId = &mbRegion[1];

    if (mbIndex > baseAddr->MCFG.bit.LASTMB)
    {
        result = STATUS_CAN_MB_OUT_OF_RANGE;
    }

    /* Check if RxFIFO is enabled */
    if (baseAddr->MCFG.bit.FIFOEN != 0U)
    {
        /* Get the number of RxFIFO filters */
        uint32_t numFilters = (uint32_t)(baseAddr->CTRL2.bit.NUMFIFO);

        /* Get the number of MBs occupied by RxFIFO and ID filter table */
        uint32_t numMbs = CAN_LastMbOccupiedByRxFifo(numFilters);

        if (mbIndex <= numMbs)
        {
            result =  STATUS_CAN_MB_OUT_OF_RANGE;
        }
    }

    if (result == STATUS_SUCCESS)
    {
        /* Clear the arbitration field area */
        *mbRegion = 0;
        *mbId = 0;

        /* Set the ID according the format structure */
        if (mbStatus->msgIdType == CAN_ID_EXTENDED)
        {
            /* Set IDE */
            *mbRegion |= CAN_CS_IDE_MASK;

            /* Set SRR bit */
            *mbRegion |= CAN_CS_SRR_MASK;

            /* ID[28-0] */
            *mbId &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
            *mbId |= (messageId & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
        }
        if (mbStatus->msgIdType == CAN_ID_STANDARD)
        {
            /* Make sure IDE and SRR are not set */
            *mbRegion &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);

            /* ID[28-18] */
            *mbId &= ~CAN_ID_STD_MASK;
            *mbId |= (messageId << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;
        }

        /* Set MB CODE */
        if (mbStatus->code != (uint32_t)CAN_CODE_RX_NOT_USED)
        {
             *mbRegion &= ~CAN_CS_CODE_MASK;
             *mbRegion |= (mbStatus->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
        }
    }
    return result;
}

/*!
 * @brief   Configure a message buffer for transmission
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 * @param   mbStatus    Pointer to MB status structure
 * @param   messageId   CAN message ID
 * @param   msgData     CAN message data
 * @param   isRemote    If this is a remote frame
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_CAN_MB_OUT_OF_RANGE: Message buffer index out of range
 */
STATUS_T CAN_HW_SetTxMb(
    CAN_T *baseAddr,
    uint32_t mbIndex,
    const CAN_MB_CODE_STATUS_T *mbStatus,
    uint32_t messageId,
    const uint8_t *msgData,
    const bool isRemote)
{
    STATUS_T result = STATUS_SUCCESS;
    uint32_t mbConfig = 0;
    uint32_t dataByte;
    uint8_t dlcValue;

    volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
    volatile uint32_t *mbId   = &mbRegion[1];
    volatile uint8_t  *mbData = (volatile uint8_t *)(&mbRegion[2]);
    volatile uint32_t *mbData32 = &mbRegion[2];
    const uint32_t *msgDataPtr = (const uint32_t *)msgData;

    if (mbIndex > baseAddr->MCFG.bit.LASTMB)
    {
        result = STATUS_CAN_MB_OUT_OF_RANGE;
    }

    /* Check if RxFIFO is enabled */
    if ((baseAddr->MCFG.bit.FIFOEN) != 0U)
    {
        /* Get the number of RxFIFO filters */
        uint32_t numFilters = (uint32_t)(baseAddr->CTRL2.bit.NUMFIFO);

        /* Get the number of MBs occupied by RxFIFO and ID filter table */
        uint32_t numMbs = CAN_LastMbOccupiedByRxFifo(numFilters);

        if (mbIndex <= numMbs)
        {
            result =  STATUS_CAN_MB_OUT_OF_RANGE;
        }
    }

    if (result == STATUS_SUCCESS)
    {
        if (CAN_HW_IsFdEnabled(baseAddr) && mbStatus->brsEnable)
        {
            baseAddr->FDCTRL.bit.BRSEN = CAN_FDCTRL_BRSEN_1;
        }

        /* Calculate the value of the DLC field */
        dlcValue = CAN_CalculateDlcValue((uint8_t)mbStatus->dataLen);

        /* Copy user's buffer into the message buffer data area */
        if (msgData != NULL)
        {
            uint8_t payloadSize = CAN_GetPayloadSize(dlcValue);

            for (dataByte = 0; dataByte < (mbStatus->dataLen & ~3U); dataByte += 4U)
            {
                CAN_SWAP_BYTES_IN_WORD(msgDataPtr[dataByte >> 2U], mbData32[dataByte >> 2U]);
            }

            for (; dataByte < mbStatus->dataLen; dataByte++)
            {
                mbData[CAN_SWAP_BYTES_IN_WORD_INDEX(dataByte)] = msgData[dataByte];
            }

            /* Add padding values if needed */
            for (dataByte = mbStatus->dataLen; dataByte < payloadSize; dataByte++)
            {
                mbData[CAN_SWAP_BYTES_IN_WORD_INDEX(dataByte)] = mbStatus->fdPadding;
            }
        }

        /* Clear the arbitration field area */
        *mbRegion = 0;
        *mbId = 0;

        /* Set the ID according the format structure */
        if (mbStatus->msgIdType == CAN_ID_EXTENDED)
        {
            /* ID[28-0] */
            *mbId &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
            *mbId |= (messageId & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));

            /* Set IDE and SRR bit */
            mbConfig |= (CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        }
        if (mbStatus->msgIdType == CAN_ID_STANDARD)
        {
            /* ID[28-18] */
            *mbId &= ~CAN_ID_STD_MASK;
            *mbId |= (messageId << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;

            /* Make sure IDE and SRR are not set */
            mbConfig &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        }

        /* Set the length of data in bytes */
        mbConfig &= ~CAN_CS_DLC_MASK;
        mbConfig |= ((uint32_t)dlcValue << CAN_CS_DLC_SHIFT) & CAN_CS_DLC_MASK;

        /* Set MB CODE */
        if (mbStatus->code != (uint32_t)CAN_CODE_TX_NOT_USED)
        {
            if (mbStatus->code == (uint32_t)CAN_CODE_TX_REMOTE)
            {
                /* Set RTR bit */
                mbConfig |= CAN_CS_RTR_MASK;
            }
            else
            {
                if (isRemote == true)
                {
                    /* Set RTR bit */
                    mbConfig |= CAN_CS_RTR_MASK;
                }
            }

            /* Reset the code */
            mbConfig &= ~CAN_CS_CODE_MASK;

            /* Set the code */
            if (mbStatus->fdEnable)
            {
                mbConfig |= ((mbStatus->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK) | CAN_MB_EDL_MASK;
            }
            else
            {
                mbConfig |= (mbStatus->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
            }

            if (mbStatus->brsEnable)
            {
                mbConfig |= CAN_MB_BRS_MASK;
            }

            *mbRegion |= mbConfig;
        }
    }
    return result;
}

 /*!
 * @brief   Configure RxFIFO ID filter table elements
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   idFormat        ID format
 * @param   idFilterTable   ID filter table
 *
 * @retval  None
 */
void CAN_HW_SetRxFifoFilter(
    CAN_T *baseAddr,
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat,
    const CAN_RXFIFO_ID_FILTER_T *idFilterTable)
{
    uint32_t filterCount;
    uint32_t val = 0;
    uint32_t val1 = 0;
    uint32_t val2 = 0;

    volatile uint32_t *filterTable = &baseAddr->RAMn[CAN_RXFIFO_FILTER_OFFSET].reg;

    filterCount = (uint32_t)baseAddr->CTRL2.bit.NUMFIFO;

    if (idFormat == CAN_RXFIFO_ID_FORMAT_A)
    {
        baseAddr->MCFG.bit.IDFFMT = (uint32_t)CAN_RXFIFO_ID_FORMAT_A;
        for (uint32_t i = 0; i < CAN_RXFIFO_FILTER_NUM(filterCount); i++)
        {
            val = 0;
            if (idFilterTable[i].isRemote)
            {
                val = CAN_RXFIFO_ACCEPT_REMOTE_FRAME
                    << CAN_RXFIFO_ID_FILTER_FORMAT_AB_RTR_SHIFT;
            }
            if (idFilterTable[i].isExtended)
            {
                val |= CAN_RXFIFO_ACCEPT_EXTENDED_FRAME
                    << CAN_RXFIFO_ID_FILTER_FORMAT_AB_IDE_SHIFT;

                filterTable[i] = val + ((idFilterTable[i].idFilter
                                        << CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_SHIFT)
                                        & CAN_RXFIFO_ID_FILTER_FORMAT_A_EXT_MASK);
            }
            else
            {
                filterTable[i] = val + ((idFilterTable[i].idFilter
                                        << CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_SHIFT)
                                        & CAN_RXFIFO_ID_FILTER_FORMAT_A_STD_MASK);
            }
        }
    }
    else if (idFormat == CAN_RXFIFO_ID_FORMAT_B)
    {
        baseAddr->MCFG.bit.IDFFMT = (uint32_t)CAN_RXFIFO_ID_FORMAT_B;

        uint32_t j = 0;
        for (uint32_t i = 0; i < CAN_RXFIFO_FILTER_NUM(filterCount); i++)
        {
            val1 = 0;
            val2 = 0;
            if (idFilterTable[j].isRemote)
            {
                val1 = CAN_RXFIFO_ACCEPT_REMOTE_FRAME
                        << CAN_RXFIFO_ID_FILTER_FORMAT_AB_RTR_SHIFT;
            }
            if (idFilterTable[j + 1U].isRemote)
            {
                val2 = CAN_RXFIFO_ACCEPT_REMOTE_FRAME
                        << CAN_RXFIFO_ID_FILTER_FORMAT_B_RTR_SHIFT;
            }
            if (idFilterTable[j].isExtended)
            {
                val1 |= CAN_RXFIFO_ACCEPT_EXTENDED_FRAME
                        << CAN_RXFIFO_ID_FILTER_FORMAT_AB_IDE_SHIFT;

                filterTable[i] = val1 + (((idFilterTable[j].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_CMP_SHIFT)
                                        << CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT1);
            }
            else
            {
                filterTable[i] = val1 + ((idFilterTable[j].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_MASK)
                                        << CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_SHIFT1);
            }
            if (idFilterTable[j + 1U].isExtended)
            {
                val2 |= CAN_RXFIFO_ACCEPT_EXTENDED_FRAME
                        << CAN_RXFIFO_ID_FILTER_FORMAT_B_IDE_SHIFT;

                filterTable[i] |= val2 + (((idFilterTable[j + 1U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_B_EXT_SHIFT2);
            }
            else
            {
                filterTable[i] |= val2 + ((idFilterTable[j + 1U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_MASK)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_B_STD_SHIFT2);
            }
            j = j + 2U;
        }
    }
    else if (idFormat == CAN_RXFIFO_ID_FORMAT_C)
    {
        baseAddr->MCFG.bit.IDFFMT = (uint32_t)CAN_RXFIFO_ID_FORMAT_C;

        uint32_t j = 0;
        for (uint32_t i = 0; i < CAN_RXFIFO_FILTER_NUM(filterCount); i++)
        {
            if (idFilterTable[j].isExtended)
            {
                filterTable[i] |= val1 + (((idFilterTable[j].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1);
            }
            else
            {
                filterTable[i] |= val1 + (((idFilterTable[j].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT1);
            }
            if (idFilterTable[j + 1U].isExtended)
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 1U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT2);
            }
            else
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 1U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT2);
            }
            if (idFilterTable[j + 2U].isExtended)
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 2U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT3);
            }
            else
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 2U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT3);
            }
            if (idFilterTable[j + 3U].isExtended)
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 3U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_EXT_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT4);
            }
            else
            {
                filterTable[i] |= val1 + (((idFilterTable[j + 3U].idFilter
                                            & CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_MASK)
                                            >> CAN_RXFIFO_ID_FILTER_FORMAT_C_STD_CMP_SHIFT)
                                         << CAN_RXFIFO_ID_FILTER_FORMAT_C_SHIFT4);
            }
            j = j + 4U;
        }
    }
    else if (idFormat == CAN_RXFIFO_ID_FORMAT_D)
    {
        /* All frames rejected */
        baseAddr->MCFG.bit.IDFFMT = (uint32_t)CAN_RXFIFO_ID_FORMAT_D;
    }
    else
    {
    }
}

/*!
 * @brief   Write the abort code into the CODE field of the Tx message buffer
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  None
 */
void CAN_HW_AbortTxMb(CAN_T *baseAddr, uint32_t mbIndex)
{
    uint32_t mbConfig = 0;
    uint32_t code = CAN_CODE_TX_ABORT;

    volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
    mbConfig = *mbRegion;

    /* Reset the code */
    mbConfig &= (~CAN_CS_CODE_MASK);

    /* Write the abort code */
    mbConfig |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    *mbRegion = mbConfig;
}

/*!
 * @brief   Reset Rx message buffer
 * @details Write the inactive code into the CODE field of the requested Rx
 *          message buffer and restore the MB to active Rx. This will force
 *          even the unlock of the Rx MB.
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  None
 */
void CAN_HW_ResetRxMb(CAN_T *baseAddr, uint32_t mbIndex)
{
    uint32_t mbConfig = 0;
    uint32_t code = CAN_CODE_RX_INACTIVE;

    volatile uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
    mbConfig = *mbRegion;

    /* Reset the code and unlock the MB */
    mbConfig &= (~CAN_CS_CODE_MASK);
    mbConfig |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    *mbRegion = mbConfig;

    /* Reconfigure The MB */
    code = CAN_CODE_RX_EMPTY;
    mbConfig &= (~CAN_CS_CODE_MASK);
    mbConfig |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    *mbRegion = mbConfig;
}

/*!
 * @brief   Get the start address of the MB
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  Start address of the MB
 */
volatile uint32_t* CAN_HW_GetMbRegion(CAN_T *baseAddr, uint32_t mbIndex)
{
    uint8_t payloadSize = CAN_HW_GetFdPayloadSize(baseAddr);
    uint8_t arbitrationFieldSize = 8U;
    uint32_t ramBlockSize = 512U;
    uint32_t ramBlockOffset;
    uint8_t mbSize = (uint8_t)(payloadSize + arbitrationFieldSize);
    uint8_t maxMbNum = (uint8_t)(ramBlockSize / mbSize);

    ramBlockOffset = 128U * (mbIndex / (uint32_t)maxMbNum);

    /* Multiply the MB index by the MB size (in words) */
    uint32_t mbOffset = ramBlockOffset
                      + ((mbIndex % (uint32_t)maxMbNum) * ((uint32_t)mbSize >> 2U));

    return &(baseAddr->RAMn[mbOffset].reg);
}

/*!
 * @brief   Get message buffer field values
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   mbIndex     Index of the message buffer
 *
 * @retval  None
 */
void CAN_HW_GetMbValues(CAN_T *baseAddr, uint32_t mbIndex, CAN_MSG_BUF_T *msgBuf)
{
    volatile const uint32_t *mbRegion = CAN_HW_GetMbRegion(baseAddr, mbIndex);
    volatile const uint32_t *mbId   = &mbRegion[1];
    volatile const uint8_t  *mbData = (volatile const uint8_t *)(&mbRegion[2]);
    volatile const uint32_t *mbData32 = &mbRegion[2];
    uint32_t *msgBufData32 = (uint32_t *)(msgBuf->data);
    uint32_t mbWord;
    uint8_t i = 0;

    uint8_t dlcValue = (uint8_t)(((*mbRegion) & CAN_CS_DLC_MASK) >> 16);
    uint8_t payloadSize = CAN_GetPayloadSize(dlcValue);

    if (payloadSize > CAN_HW_GetFdPayloadSize(baseAddr))
    {
        payloadSize = CAN_HW_GetFdPayloadSize(baseAddr);
    }

    msgBuf->dataLen = payloadSize;

    /* Get a MB field values */
    msgBuf->cs = *mbRegion;

    if ((msgBuf->cs & CAN_CS_IDE_MASK) != 0U)
    {
        msgBuf->messageId = (*mbId);
    }
    else
    {
        msgBuf->messageId = (*mbId) >> CAN_ID_STD_SHIFT;
    }

    for (i = 0U ; i < (payloadSize & ~3U); i += 4U)
    {
        mbWord = mbData32[i >> 2U];
        CAN_SWAP_BYTES_IN_WORD(mbWord, msgBufData32[i >> 2U]);
    }

    for ( ; i < payloadSize ; i++)
    {   /* Max allowed value for index is 63 */
        msgBuf->data[i] = mbData[CAN_SWAP_BYTES_IN_WORD_INDEX(i)];
    }
}

/*!
 * @brief   Checks if RxFIFO is enabled
 *
 * @param   baseAddr    The CAN baseAddr address
 *
 * @retval  true:   enabled
 *          false:  disabled
 *
 */
bool CAN_HW_IsRxFifoEnabled(const CAN_T *baseAddr)
{
    return ((baseAddr->MCFG.bit.FIFOEN) != 0U) ? true : false;
}

/*!
 * @brief   Enable RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   filterCount Number of filters
 *
 * @retval  STATUS_SUCCESS: Successful
 *          STATUS_ERROR:   RxFIFO cannot be enabled because FD is enabled
 *
 */
STATUS_T CAN_HW_EnableRxFifo(CAN_T *baseAddr, uint32_t filterCount)
{
    uint16_t maxMbNumber = (uint16_t)CAN_HW_GetMaxMbNum(baseAddr);
    STATUS_T result = STATUS_SUCCESS;

    /* RxFIFO cannot be enabled if FD is enabled */
    if (CAN_HW_IsFdEnabled(baseAddr))
    {
        result = STATUS_ERROR;
    }

    if (result == STATUS_SUCCESS)
    {
        /* Enable RxFIFO */
        baseAddr->MCFG.bit.FIFOEN = CAN_MCFG_FIFOEN_1;

        /* Set the number of the RxFIFO filters needed */
        baseAddr->CTRL2.bit.NUMFIFO = filterCount;

        /* Set RxFIFO global mask, take in consideration all filter fields */
        (baseAddr->FIFOGMASK.bit.FIFOGMASK) = 0xFFFFFFFFU;

        for (uint32_t i = 0; i < maxMbNumber; i++)
        {
            /* Set RX individual mask */
            baseAddr->RXIMASK[i].reg = (CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
        }
    }
    return result;
}

/*!
 * @brief   Get RxFIFO ID format
 *
 * @param   baseAddr    The CAN baseAddr address
 *
 * @retval  RxFifo ID format
 *
 */
CAN_RXFIFO_ID_ELEMENT_FORMAT_T CAN_HW_GetRxFifoIdFormat(const CAN_T *baseAddr)
{
    CAN_RXFIFO_ID_ELEMENT_FORMAT_T idFormat = CAN_RXFIFO_ID_FORMAT_A;

    if (baseAddr->MCFG.bit.IDFFMT == 0U)
    {
        idFormat = CAN_RXFIFO_ID_FORMAT_A;
    }
    else if (baseAddr->MCFG.bit.IDFFMT == 1U)
    {
        idFormat = CAN_RXFIFO_ID_FORMAT_B;
    }
    else if (baseAddr->MCFG.bit.IDFFMT == 2U)
    {
        idFormat = CAN_RXFIFO_ID_FORMAT_C;
    }
    else if (baseAddr->MCFG.bit.IDFFMT == 3U)
    {
        idFormat=CAN_RXFIFO_ID_FORMAT_D;
    }
    else
    {
    }
    return idFormat;
}

 /*!
 * @brief   Get RxFIFO values
 * @details This function will copy MB[0] data field into user's buffer
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   msgBuf      Buffer to store the received CAN frame
 *
 * @retval  None
 */
void CAN_HW_GetRxFifoValues(const CAN_T *baseAddr, CAN_MSG_BUF_T *msgBuf)
{
    volatile const uint32_t *mbRegion = &baseAddr->RAMn[0].reg;
    volatile const uint32_t *mbId = &baseAddr->RAMn[1].reg;
    volatile const uint32_t *mbData32 = &mbRegion[2];
    uint32_t *msgDataPtr = (uint32_t *)(msgBuf->data);
    uint32_t dataByte = 0;
    uint32_t mbWord;

    uint8_t dlcValue = (uint8_t)(((*mbRegion) & CAN_CS_DLC_MASK) >> 16);
    uint8_t payloadSize = CAN_GetPayloadSize(dlcValue);

    msgBuf->dataLen = payloadSize;
    msgBuf->cs = *mbRegion;

    if ((msgBuf->cs & CAN_CS_IDE_MASK) != 0U)
    {
        msgBuf->messageId = *mbId;
    }
    else
    {
        msgBuf->messageId = (*mbId) >> CAN_ID_STD_SHIFT;
    }

    /* Copy MB[0] data field into user's buffer */
    for (dataByte = 0U; dataByte < payloadSize; dataByte += 4U)
    {
        mbWord = mbData32[dataByte >> 2U];
        CAN_SWAP_BYTES_IN_WORD(mbWord, msgDataPtr[dataByte >> 2U]);
    }
}

/*!
 * @brief   Get the number of RxFIFO ID filter table elements affected by
 *          Rx individual masks
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  The number of RxFIFO ID filter table elements affected
 *
 */
uint8_t CAN_HW_GetAffectedRxFifoNumbers(const CAN_T *baseAddr)
{
    /* Get the number of RxFIFO filters */
    uint8_t ret = (uint8_t)(baseAddr->CTRL2.bit.NUMFIFO);

    /**
     * Max filters configured by individual mask are (7 + RFFN * 2) depends
     * on the FIFO size, max allowed value is 31 RXIMR.
     */
    ret = (uint8_t)(7u + ((uint32_t)ret << 1u));
    return ret;
}

/*!
 * @brief   Set the CAN clock source
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   clockSource CAN clock source
 *
 * @retval  None
 */
void CAN_HW_SetClockSource(CAN_T *baseAddr, CAN_CLK_SOURCE_T clockSource)
{
    baseAddr->CTRL1.bit.CLKSEL = clockSource;
}

/*!
 * @brief   Check if selected instance support FD or not

 * @param   index   CAN controller index
 *
 * @retval  true:   CANFD is supported
 *          false:  CANFD is not supported
 */
bool CAN_HW_InstanceHasFd(uint8_t index)
{
    static const bool fdInstances[] = FEATURE_CAN_INSTANCES_WITH_FD;
    return fdInstances[index];
}

/*!
 * @brief   Get the CAN extended time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CAN time segments
 *
 * @retval  None
 */
void CAN_HW_GetExtendedTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg)
{
    timeSeg->preDiv = baseAddr->BTIM.bit.EPDF;
    timeSeg->propSeg = baseAddr->BTIM.bit.EPTS;
    timeSeg->phaseSeg1 = baseAddr->BTIM.bit.EPBS1;
    timeSeg->phaseSeg2 = baseAddr->BTIM.bit.EPBS2;
    timeSeg->resyncJumpWidth = baseAddr->BTIM.bit.ESJW;
}

/*!
 * @brief Set the CAN extended time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CAN time segments
 *
 * @retval  None
 */
void CAN_HW_SetExtendedTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg)
{
    baseAddr->BTIM.bit.EPDF = timeSeg->preDiv;
    baseAddr->BTIM.bit.EPTS = timeSeg->propSeg;
    baseAddr->BTIM.bit.EPBS1 = timeSeg->phaseSeg1;
    baseAddr->BTIM.bit.EPBS2 = timeSeg->phaseSeg2;
    baseAddr->BTIM.bit.ESJW = timeSeg->resyncJumpWidth;
}

/*!
 * @brief   Get the CANFD time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CANFD time segments
 *
 * @retval  None
 */
void CAN_HW_GetFdTimeSegments(const CAN_T *baseAddr, CAN_TIME_SEGMENT_T *timeSeg)
{
    timeSeg->preDiv = baseAddr->FDBTIM.bit.FPDF;
    timeSeg->propSeg = baseAddr->FDBTIM.bit.FPTS;
    timeSeg->phaseSeg1 = baseAddr->FDBTIM.bit.FPBS1;
    timeSeg->phaseSeg2 = baseAddr->FDBTIM.bit.FPBS2;
    timeSeg->resyncJumpWidth = baseAddr->FDBTIM.bit.FSJW;
}

/*!
 * @brief Set the CANFD time segments
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeSeg     CANFD time segments
 *
 * @retval  None
 */
void CAN_HW_SetFdTimeSegments(CAN_T *baseAddr, const CAN_TIME_SEGMENT_T *timeSeg)
{
    /* Set CAN time segments*/
    baseAddr->FDBTIM.bit.FPTS = (uint32_t)timeSeg->propSeg;
    baseAddr->FDBTIM.bit.FPBS2 = (uint32_t)timeSeg->phaseSeg2;
    baseAddr->FDBTIM.bit.FPBS1 = (uint32_t)timeSeg->phaseSeg1;
    baseAddr->FDBTIM.bit.FPDF = (uint32_t)timeSeg->preDiv;
    baseAddr->FDBTIM.bit.FSJW = (uint32_t)timeSeg->resyncJumpWidth;
}

/*!
 * @brief   Checks if CANFD is enabled
 *
 * @param   baseAddr    The CAN baseAddr address
 *
 * @retval  true:   CANFD is enabled
 *          false:  CANFD is disabled
 */
bool CAN_HW_IsFdEnabled(const CAN_T *baseAddr)
{
    return (baseAddr->MCFG.bit.CANFDEN != 0U) ? true : false;
}

/*!
 * @brief Enable or disable CANFD (if supported)
 *
 * @param   baseAddr    The CAN baseAddr address
 * @param   enable      Enable or disable
 *
 * @retval  None
 */
void CAN_HW_SetFdEnable(CAN_T *baseAddr, bool enable)
{
    if (enable)
    {
        baseAddr->MCFG.bit.CANFDEN = CAN_MCFG_CANFDEN_1;
        baseAddr->BTIM.bit.EBTEN = CAN_BTIM_EBTEN_1;
    }
    else
    {
        baseAddr->MCFG.bit.CANFDEN = CAN_MCFG_CANFDEN_0;
        baseAddr->BTIM.bit.EBTEN = CAN_BTIM_EBTEN_0;
    }

    /* Disable transmission delay compensation by default */
    baseAddr->FDCTRL.bit.TDCEN = CAN_FDCTRL_TDCEN_0;
    baseAddr->FDCTRL.bit.TDCOF = (uint32_t)0U;
}

/*!
 * @brief   Get CANFD payload size of the MBs in bytes
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  Payload size
 *
 */
uint8_t CAN_HW_GetFdPayloadSize(const CAN_T *baseAddr)
{
    uint32_t payloadSize;

    if (!CAN_HW_IsFdEnabled(baseAddr))
    {
        /* The standard payload size is 8 bytes */
        payloadSize = 8U;
    }
    else
    {
        payloadSize = 1UL << ((uint32_t)(baseAddr->FDCTRL.bit.MBSIZE) + 3U);
    }

    return (uint8_t)payloadSize;
}

/*!
 * @brief   Set the CANFD payload size of the MBs
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   payloadSize Payload size in bytes
 *
 * @retval  None
 *
 */
void CAN_HW_SetFdPayloadSize(CAN_T *baseAddr, CAN_FD_PAYLOAD_SIZE_T payloadSize)
{
    /* If FD is disabled, only 8 bytes payload is supported */
    if (CAN_HW_IsFdEnabled(baseAddr))
    {
        baseAddr->FDCTRL.bit.MBSIZE = ((uint32_t)payloadSize);
    }
}

/*!
 * @brief   Enable or disable CANFD ISO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   enable      Enable or disable
 *
 * @retval  None
 */
void CAN_HW_SetIsoFdEnable(CAN_T *baseAddr, bool enable)
{
    baseAddr->CTRL2.bit.ISOFDEN = (uint32_t)(enable ? CAN_CTRL2_ISOFDEN_1 : CAN_CTRL2_ISOFDEN_0);
}

/*!
 * @brief   Set the transceiver delay compensation offset
 * @details Enable or disable the transceiver delay compensation feature and
 *          set the transceiver delay compensation offset (offset value to be
 *          added to the measured transceiver's loop delay in order to define
 *          the position of the delayed comparison point when bitrate switching
 *          is active).
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   enable      Enable or disable
 * @param   offset      Transceiver delay compensation offset
 *
 * @retval  None
 */
void CAN_HW_SetTdcOffset(CAN_T *baseAddr, bool enable, uint8_t offset)
{
    if (enable)
    {
        baseAddr->FDCTRL.bit.TDCEN = CAN_FDCTRL_TDCEN_1;
        baseAddr->FDCTRL.bit.TDCOF = (uint32_t)offset;
    }
    else
    {
        baseAddr->FDCTRL.bit.TDCEN = CAN_FDCTRL_TDCEN_0;
        baseAddr->FDCTRL.bit.TDCOF = 0;
    }
}

/*!
 * @brief   Enable or disable DMA support for RxFIFO
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   enable      Enable or disable DMA support
 *
 * @retval  None
 */
void CAN_HW_SetRxFifoDmaEnable(CAN_T *baseAddr, bool enable)
{
    baseAddr->MCFG.bit.DMAEN = (uint32_t)(enable ? CAN_MCFG_DMAEN_1 : CAN_MCFG_DMAEN_0);
}

/*!
 * @brief   Checks if the Pretended Networking is enabled
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  true:   Pretended Networking mode is enabled
 *          false:  Pretended Networking mode is disabled
 */
bool CAN_HW_IsPnEnabled(const CAN_T *baseAddr)
{
    return (baseAddr->MCFG.bit.PNEN != CAN_MCFG_PNEN_0) ? true : false;
}

/*!
 * @brief   Enable or disable the Pretended Networking
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   enable      Enable or disable Pretending Networking
 *
 * @retval  None
 */
void CAN_HW_SetPnEnable(CAN_T *baseAddr, bool enable)
{
    baseAddr->MCFG.bit.PNEN = (enable ? CAN_MCFG_PNEN_1 : CAN_MCFG_PNEN_0);
}

/*!
 * @brief   Configure the Pretended Networking
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   pnCfg       Pointer to Pretended Networking configuration
 *
 * @retval  None
 */
void CAN_HW_ConfigurePn(CAN_T *baseAddr, const CAN_PN_CONFIG_T *pnCfg)
{
    CAN_HW_SetPnFilter(baseAddr,
                           pnCfg->wakeupByTimeout,
                           pnCfg->wakeupByMatch,
                           pnCfg->matchCount,
                           pnCfg->filterType,
                           pnCfg->idFilterScheme,
                           pnCfg->payloadFilterScheme);

    CAN_HW_SetPnTimeout(baseAddr, pnCfg->matchTimeout);

    /* Configure ID filtering */
    CAN_HW_SetPnIdFilter1(baseAddr, pnCfg->idFilter1);

    /* Configure the second ID if needed */
    if (   (pnCfg->idFilterScheme == CAN_PN_MATCH_EXACT)
        || (pnCfg->idFilterScheme == CAN_PN_MATCH_RANGE))
    {
        CAN_HW_SetPnIdFilter2(baseAddr, pnCfg);
    }
    else
    {
        /* In other case, check the IDE and RTR match the ID_MASK is not considered */
        CAN_HW_SetPnIdFilter2Check(baseAddr);
    }

    /* Configure payload filtering */
    if (   (pnCfg->filterType == CAN_PN_FILTER_ID_PAYLOAD)
        || (pnCfg->filterType == CAN_PN_FILTER_ID_PAYLOAD_NTIMES))
    {
        CAN_HW_SetPnDlcFilter(baseAddr,
                                  pnCfg->payloadFilter.minPayloadLen,
                                  pnCfg->payloadFilter.maxPayloadLen);

        CAN_HW_SetPnPayloadHighFilter1(baseAddr, pnCfg->payloadFilter.payloadFilter1);
        CAN_HW_SetPnPayloadLowFilter1(baseAddr, pnCfg->payloadFilter.payloadFilter1);

        /* Configure the second payload if needed */
        if (   (pnCfg->payloadFilterScheme == CAN_PN_MATCH_EXACT)
            || (pnCfg->payloadFilterScheme == CAN_PN_MATCH_RANGE))
        {
            CAN_HW_SetPnPayloadHighFilter2(baseAddr, pnCfg->payloadFilter.payloadFilter2);
            CAN_HW_SetPnPayloadLowFilter2(baseAddr, pnCfg->payloadFilter.payloadFilter2);
        }
    }
}

/*!
 * @brief   Set the Pretended Networking timeout value
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   timeout     Timeout for no message matching
 *
 * @retval  None
 */
void CAN_HW_SetPnTimeout(CAN_T *baseAddr, uint16_t timeout)
{
    baseAddr->PNCTRL2.reg = (uint32_t)timeout;
}

/*!
 * @brief   Set the Pretended Networking ID Filter 1
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   idFilter    ID filter
 *
 * @retval  None
 */
void CAN_HW_SetPnIdFilter1(CAN_T *baseAddr, CAN_PN_ID_FILTER_T idFilter)
{
    baseAddr->PNIDF1.bit.EXFF = (uint32_t)(idFilter.isExtended ? CAN_PNIDF1_EXFF_1 : CAN_PNIDF1_EXFF_0);
    baseAddr->PNIDF1.bit.RFF = (uint32_t)(idFilter.isRemote ? CAN_PNIDF1_RFF_1 : CAN_PNIDF1_RFF_0);

    if (idFilter.isExtended)
    {
        baseAddr->PNIDF1.bit.IDF = idFilter.idFilter;
    }
    else
    {
        baseAddr->PNIDF1.bit.IDF = (uint32_t)(idFilter.idFilter << CAN_ID_STD_SHIFT);
    }
}

/*!
 * @brief   Set the Pretended Networking ID Filter 2
 *
 * @param   baseAddr    Base address of the CAN controller
 * @param   pnCfg       Pointer to Pretended Networking configuration
 *
 * @retval  None
 */
void CAN_HW_SetPnIdFilter2(CAN_T *baseAddr, const CAN_PN_CONFIG_T *pnCfg)
{
    baseAddr->PNIDF2.bit.RFMASK = (uint32_t)(pnCfg->idFilter2.isRemote ? CAN_PNIDF2_RFMASK_1 : CAN_PNIDF2_RFMASK_0);
    baseAddr->PNIDF2.bit.EXFMASK = (uint32_t)(pnCfg->idFilter2.isExtended ? CAN_PNIDF2_EXFMASK_1 : CAN_PNIDF2_EXFMASK_0);

    /* Check if ID Filter 1 is extended and apply accordingly mask */
    if (pnCfg->idFilter1.isExtended)
    {
        baseAddr->PNIDF2.bit.IDMASK = pnCfg->idFilter2.idFilter;
    }
    else
    {
        baseAddr->PNIDF2.bit.IDMASK = (uint32_t)(pnCfg->idFilter2.idFilter << CAN_ID_STD_SHIFT);
    }
}

/*!
 * @brief   Set the Pretended Networking ID Filter 2 Check
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_SetPnIdFilter2Check(CAN_T *baseAddr)
{
    baseAddr->PNIDF2.bit.RFMASK = CAN_PNIDF2_RFMASK_1;
    baseAddr->PNIDF2.bit.EXFMASK = CAN_PNIDF2_EXFMASK_1;
}

/*!
 * @brief   Set Pretended Networking DLC Filter
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   minPayloadLen   Minimum payload length
 * @param   maxPayloadLen   Maximum payload length
 *
 * @retval  None
 */
void CAN_HW_SetPnDlcFilter(
    CAN_T *baseAddr,
    uint8_t minPayloadLen,
    uint8_t maxPayloadLen)
{
    baseAddr->PNDLCF.bit.DLCHI = (uint32_t)maxPayloadLen;
    baseAddr->PNDLCF.bit.DLCLO = (uint32_t)minPayloadLen;
}

/*!
 * @brief   Set PN Payload High Filter 1
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   payloadFilter   Payload filter
 *
 * @retval  None
 */
void CAN_HW_SetPnPayloadHighFilter1(CAN_T *baseAddr, const uint8_t *payloadFilter)
{
    baseAddr->PNPHF1.bit.BYTE4 = (uint32_t)payloadFilter[4];
    baseAddr->PNPHF1.bit.BYTE5 = (uint32_t)payloadFilter[5];
    baseAddr->PNPHF1.bit.BYTE6 = (uint32_t)payloadFilter[6];
    baseAddr->PNPHF1.bit.BYTE7 = (uint32_t)payloadFilter[7];
}

/*!
 * @brief   Set PN Payload Low Filter 1
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   payloadFilter   Payload filter
 *
 * @retval  None
 */
void CAN_HW_SetPnPayloadLowFilter1(CAN_T *baseAddr, const uint8_t *payloadFilter)
{
    baseAddr->PNPLF1.bit.BYTE0 = (uint32_t)payloadFilter[0];
    baseAddr->PNPLF1.bit.BYTE1 = (uint32_t)payloadFilter[1];
    baseAddr->PNPLF1.bit.BYTE2 = (uint32_t)payloadFilter[2];
    baseAddr->PNPLF1.bit.BYTE3 = (uint32_t)payloadFilter[3];
}

/*!
 * @brief   Set PN Payload High Filter 2
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   payloadFilter   Payload filter
 *
 * @retval  None
 */
void CAN_HW_SetPnPayloadHighFilter2(CAN_T *baseAddr, const uint8_t *payloadFilter)
{
    baseAddr->PNPHF2.bit.BYTE4 = (uint32_t)payloadFilter[4];
    baseAddr->PNPHF2.bit.BYTE5 = (uint32_t)payloadFilter[5];
    baseAddr->PNPHF2.bit.BYTE6 = (uint32_t)payloadFilter[6];
    baseAddr->PNPHF2.bit.BYTE7 = (uint32_t)payloadFilter[7];
}

/*!
 * @brief   Set PN Payload Low Filter 2
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   payloadFilter   Payload filter
 *
 * @retval  None
 */
void CAN_HW_SetPnPayloadLowFilter2(CAN_T *baseAddr, const uint8_t *payloadFilter)
{
    baseAddr->PNPLF2.bit.BYTE0 = (uint32_t)payloadFilter[0];
    baseAddr->PNPLF2.bit.BYTE1 = (uint32_t)payloadFilter[1];
    baseAddr->PNPLF2.bit.BYTE2 = (uint32_t)payloadFilter[2];
    baseAddr->PNPLF2.bit.BYTE3 = (uint32_t)payloadFilter[3];
}

/*!
 * @brief   Get the wakeup by timeout flag
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  Wakeup by timeout flag
 */
uint8_t CAN_HW_GetWakeupByTimeoutFlag(const CAN_T *baseAddr)
{
    return (uint8_t)baseAddr->PNWM.bit.WUTFLG;
}

/*!
 * @brief   Clear the wakeup by timeout flag
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ClearWakeupByTimeoutFlag(CAN_T *baseAddr)
{
    baseAddr->PNWM.bit.WUTFLG = CAN_PNWM_WUTFLG_1;
}

/*!
 * @brief   Get the wakeup by match flag
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  Wakeup by match flag
 */
uint8_t CAN_HW_GetWakeupByMatchFlag(const CAN_T *baseAddr)
{
    return (uint8_t)baseAddr->PNWM.bit.WUMFLG;
}

/*!
 * @brief   Clear the wakeup by match flag
 *
 * @param   baseAddr    Base address of the CAN controller
 *
 * @retval  None
 */
void CAN_HW_ClearWakeupByMatchFlag(CAN_T *baseAddr)
{
    baseAddr->PNWM.bit.WUMFLG = CAN_PNWM_WUMFLG_1;
}

/*!
 * @brief   Set Pretended Networking mode filtering type
 *
 * @param   baseAddr        Base address of the CAN controller
 * @param   wakeupByTimeout Enable or disable wakeup by timeout
 * @param   wakeupByMatch   Enable or disable wakeup by match
 * @param   numMatch        Number of messages matching the filtering criteria
 * @param   filterType      Filtering type selection
 * @param   idScheme        ID filtering scheme
 * @param   payloadScheme   Payload filtering scheme
 *
 * @retval  None
 */
void CAN_HW_SetPnFilter(
    CAN_T *baseAddr,
    bool wakeupByTimeout,
    bool wakeupByMatch,
    uint16_t numMatch,
    CAN_PN_FILTER_TYPE_T filterType,
    CAN_PN_MATCH_SCHEME_T idScheme,
    CAN_PN_MATCH_SCHEME_T payloadScheme)
{
    baseAddr->PNCTRL1.bit.WUTEN = (uint32_t)(wakeupByTimeout ? 1UL : 0UL);
    baseAddr->PNCTRL1.bit.WUMEN = (uint32_t)(wakeupByMatch ? 1UL : 0UL);
    baseAddr->PNCTRL1.bit.NUMMSG = (uint32_t)numMatch;
    baseAddr->PNCTRL1.bit.FCSEL = (uint32_t)filterType;
    baseAddr->PNCTRL1.bit.IDFLSEL = (uint32_t)idScheme;
    baseAddr->PNCTRL1.bit.PFLSEL = (uint32_t)payloadScheme;
}

/**@} end of group CAN_Functions*/
/**@} end of group CAN_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
