/*!
 * @file        apm32f445_446_dma.c
 *
 * @brief       This file provides all the DMA firmware functions
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
#include "apm32f445_446_dma.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup DMA_Driver DMA Driver
  @{
*/

/** @defgroup DMA_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

#define DMA_INSTANCE_VALIDITY(__instance__) if(__instance__ >= DMA_INSTANCE_COUNT){while(1);}

/**@} end of group DMA_Macros*/

/** @defgroup DMA_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/* Array of dmaBase addresses for DMA instances. */
static DMA_T *const g_dmaBasePointer[DMA_INSTANCE_COUNT] = { DMA };

/* Array of dmaBase addresses for DMACHM instances. */
static DMACHM_T *const g_dmaChmBasePointer[DMACHM_INSTANCE_COUNT] = { DMACHM };

/* Array of default DMA channel interrupt handlers. */
static const IRQn_Type g_dmaIrqNum[FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES] =
{
    DMA0_IRQn, DMA1_IRQn, DMA2_IRQn, DMA3_IRQn,
    DMA4_IRQn, DMA5_IRQn, DMA6_IRQn, DMA7_IRQn,
    DMA8_IRQn, DMA9_IRQn, DMA10_IRQn, DMA11_IRQn,
    DMA12_IRQn, DMA13_IRQn, DMA14_IRQn, DMA15_IRQn
};

/* Array of default DMA error interrupt handlers. */
static const IRQn_Type g_dmaErrIrqNum[FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES] = { DMA_Error_IRQn };

/* DMA global structure to maintain DMA state */
static DMA_STATE_T *g_dmaState;

/**@} end of group DMA_Variables*/

/** @defgroup DMA_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/

static void DMA_ClearStructure(uint8_t *structPtr, size_t length);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configures the DMA req for the DMA channel.
 *
 * Configures the periodic trigger capability for the triggered DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 * @param req DMA req source.
 * @param enableTrigger DMA channel periodic trigger.
 *
 * @retval STATUS_SUCCESS or STATUS_UNSUPPORTED.
 */
STATUS_T DMA_ConfigChannelRequestAndTrigger(uint8_t vtChannel, uint8_t req, bool enableTrigger)
{
    uint8_t dmaChmIns = (uint8_t)FEATURE_DMACHM_REQUEST_SRC_TO_INSTANCE(req);
    DMACHM_T *dmaChmBasePointer = g_dmaChmBasePointer[dmaChmIns];

    uint8_t dmaChmChannel = (uint8_t)FEATURE_DMACHM_DMA_CHN_TO_CHN(vtChannel);
    uint8_t dmaChmReq = (uint8_t)FEATURE_DMACHM_REQUEST_SRC_TO_CHN(req);

    /* Config req and trigger */
    DMACHM_HW_ConfigChannelCmd(dmaChmBasePointer, dmaChmChannel, false);
    DMACHM_HW_ConfigChannelSource(dmaChmBasePointer, dmaChmChannel, dmaChmReq);
    DMACHM_HW_ConfigChannelTrigger(dmaChmBasePointer, dmaChmChannel, enableTrigger);
    DMACHM_HW_ConfigChannelCmd(dmaChmBasePointer, dmaChmChannel, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Registers the callback function and the parameter for DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 * @param callback The pointer to the callback function.
 * @param parameter The pointer to the callback function's parameter.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_RegisterCallback(uint8_t vtChannel, DMA_CALLBACK_T callback, void *parameter)
{
    g_dmaState->vtChnState[vtChannel]->parameter = parameter;
    g_dmaState->vtChnState[vtChannel]->callback = callback;

    return STATUS_SUCCESS;
}

/*!
 * @brief Config an DMA channel.
 *
 * @param dmaChannelState Pointer to the DMA channel state structure.
 * @param dmaChannelConfig User configuration structure for DMA channel.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_ConfigChannel(DMA_CHN_STATE_T *dmaChannelStatus, const DMA_CHANNEL_CONFIG_T *dmaChannelCfg)
{
    STATUS_T state = STATUS_SUCCESS;

    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(dmaChannelCfg->vtChannel);
    DMA_T *dmaRegBase = g_dmaBasePointer[dmaIns];

    uint8_t vtChannel = dmaChannelCfg->vtChannel;
    uint8_t dmaChannel = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(dmaChannelCfg->vtChannel);

    /* Reset the channel state structure to default value. */
    DMA_ClearStructure((uint8_t *)dmaChannelStatus, sizeof(DMA_CHN_STATE_T));

    state = DMA_ConfigChannelRequestAndTrigger(dmaChannelCfg->vtChannel,
                                                  (uint8_t)dmaChannelCfg->source,
                                                  dmaChannelCfg->enableTrigger);

    /* Clear the TCD registers for this channel */
    DMA_HW_TCDClearReg(dmaRegBase, dmaChannel);

    if (state == STATUS_SUCCESS)
    {
        g_dmaState->vtChnState[vtChannel] = dmaChannelStatus;
        g_dmaState->vtChnState[vtChannel]->vtChnNum = vtChannel;
        g_dmaState->vtChnState[vtChannel]->status = DMA_CHANNEL_NORMAL;

        /* Enable error interrupt for this channel */
        DMA_HW_ConfigErrorIntCmd(dmaRegBase, dmaChannel, true);

        /* Set the channel priority */
        if ((DMA_HW_ReadChannelArbitrationMode(dmaRegBase) == DMA_ARBITRATION_FIXED_PRIORITY) &&
            (dmaChannelCfg->channelPriority != DMA_CHANNEL_DEFAULT_PRIORITY))
        {
            DMA_HW_ConfigChannelPriority(dmaRegBase, dmaChannel, dmaChannelCfg->channelPriority);
        }
        /* Register the user callback */
        state = DMA_RegisterCallback(dmaChannelCfg->vtChannel,
                                        dmaChannelCfg->callback,
                                        dmaChannelCfg->callbackParam);
    }

    return state;
}

/*!
 * @brief Config the DMA module.
 *
 * @param dmaStatus The pointer to the DMA peripheral driver state structure. The user passes
 *        the memory for this run-time state structure and the DMA peripheral driver populates the
 *        members. This run-time state structure keeps track of the DMA channels status. The memory must
 *        be kept valid before calling the DMA_Reset.
 * @param userConfig User configuration structure for DMA peripheral drivers. The user populates the
 *        members of this structure and passes the pointer of this structure into the function.
 * @param chnStatusArray Array of pointers to run-time state structures for DMA channels;
 *        will populate the state structures inside the DMA driver state structure.
 * @param chnConfigArray Array of pointers to channel initialization structures.
 * @param chnCount The number of DMA channels to be initialized.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_Init(DMA_STATE_T *dmaStatus,
                  const DMA_USER_CONFIG_T *userConfig,
                  DMA_CHN_STATE_T *const chnStatusArray[],
                  const DMA_CHANNEL_CONFIG_T *const chnConfigArray[],
                  uint32_t chnCount)
{
    uint32_t index = 0U;
    DMA_T *dmaRegBase = NULL;
    STATUS_T dmaInitStatus = STATUS_SUCCESS;

    /* Save the runtime state structure for the driver */
    g_dmaState = dmaStatus;

    /* Clear the state structure. */
    DMA_ClearStructure((uint8_t *)g_dmaState, sizeof(DMA_STATE_T));

    /* Init all DMA instances */
    for(index = 0U; index < (uint32_t)DMA_INSTANCE_COUNT; index++)
    {
        dmaRegBase = g_dmaBasePointer[index];

        /* Init DMA module on hardware level. */
        DMA_HW_ClearReg(dmaRegBase);

        /* Set arbitration mode */
        DMA_HW_ConfigChannelArbitrationMode(dmaRegBase, userConfig->chnArbitration);

        /* Enable 'Halt on error' configuration */
        DMA_HW_ConfigHaltOnErrorCmd(dmaRegBase, userConfig->haltOnError);
    }

    /* Clear all DMACHM registers */
    for (index = 0U; index < (uint32_t)DMACHM_INSTANCE_COUNT; index++)
    {
        DMAMUX_HW_ClearReg(g_dmaChmBasePointer[index]);
    }
    /* Register all dma channel interrupt handlers into vector table. */
    for (index = 0U; index < (uint32_t)FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES; index++)
    {
        INT_SYS_EnableIRQ(g_dmaIrqNum[index]);
    }

    /* Enable the error interrupts for DMA module. */
    for (index = 0U; index < (uint32_t)FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES; index++)
    {
        INT_SYS_EnableIRQ(g_dmaErrIrqNum[index]);
    }

    /* Initialize the channels dmaBased on configuration list */
    if ((POINTER_IS_NULL(chnStatusArray) == false) && (POINTER_IS_NULL(chnConfigArray) == false))
    {
        for (index = 0U; index < chnCount; index++)
        {
            dmaInitStatus = DMA_ConfigChannel(chnStatusArray[index], chnConfigArray[index]);
        }
    }

    return dmaInitStatus;
}

/*!
 * @brief Releases an DMA channel.
 *
 * This function stops the DMA channel and disables the interrupt of this channel. The channel state
 * structure can be released after this function is called.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_ReleaseChannel(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    DMA_CHN_STATE_T *chnStatus = g_dmaState->vtChnState[vtChannel];

    /* Stop dma channel. */
    DMA_HW_ConfigDmaRequestCmd(dmaRegBasePointer, dmaChannelNum, false);

    /* Reset the channel state structure to default value. */
    DMA_ClearStructure((uint8_t *)chnStatus, sizeof(DMA_CHN_STATE_T));

    g_dmaState->vtChnState[vtChannel] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Reset the DMA module.
 *
 * This function resets the DMA module to reset state and disables the interrupt to the core.
 *
 * @param None.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_Reset(void)
{
    uint32_t index = 0U;
    const DMA_CHN_STATE_T *chnStatus = NULL;

    /* Disable the error interrupts for DMA module. */
    for (index = 0U; index < (uint32_t)FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES; index++)
    {
        INT_SYS_DisableIRQ(g_dmaErrIrqNum[index]);
    }

    if (POINTER_IS_NULL(g_dmaState) == false)
    {
        for (index = 0U; index < (uint32_t)FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES; index++)
        {
            INT_SYS_DisableIRQ(g_dmaIrqNum[index]);
        }

        /* Release all dma channel. */
        for (index = 0U; index < (uint32_t)FEATURE_DMA_VIRTUAL_CHANNELS; index++)
        {
            /* Release all channels. */
            chnStatus = g_dmaState->vtChnState[index];
            if (POINTER_IS_NULL(chnStatus) == false)
            {
                (void) DMA_ReleaseChannel(chnStatus->vtChnNum);
            }
        }
    }
    g_dmaState = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Clear done and interrupt flags.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval None.
 */
void DMA_ClearIntStatus(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBase = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    DMA_HW_ClearIntStatusFlag(dmaRegBase, dmaChannelNum);
    DMA_HW_ClearDoneStatusFlag(dmaRegBase, dmaChannelNum);
}

/*!
 * @brief Clear the software tcd structure.
 *
 * @param stcd DMA_SOFTWARE_TCD_T structure pointer.
 *
 * @retval None.
 */
void DMA_ClearSoftwareTCD(DMA_SOFTWARE_TCD_T *stcd)
{
    DMA_ClearStructure((uint8_t *)stcd, sizeof(DMA_SOFTWARE_TCD_T));
}

/*!
 * @brief DMA IRQ handler.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval None.
 */
void DMA_IRQHandler(uint8_t vtChannel)
{
    const DMA_CHN_STATE_T *chnStatus = g_dmaState->vtChnState[vtChannel];

    DMA_ClearIntStatus(vtChannel);

    if ((POINTER_IS_NULL(chnStatus) == false) && (POINTER_IS_NULL(chnStatus->callback) == false))
    {
        chnStatus->callback(chnStatus->parameter, chnStatus->status);
    }
}

/*!
 * @brief DMA error IRQ handler.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval None.
 */
void DMA_ErrorIRQHandler(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);
    DMA_CHN_STATE_T *chnStatus = g_dmaState->vtChnState[vtChannel];

    /* Disables the DMA req */
    DMA_HW_ConfigDmaRequestCmd(dmaRegBasePointer, dmaChannelNum, false);

    if (POINTER_IS_NULL(chnStatus) == false)
    {
        DMA_ClearIntStatus(vtChannel);
        DMA_HW_ClearErrorIntStatusFlag(dmaRegBasePointer, dmaChannelNum);
        chnStatus->status = DMA_CHANNEL_ERROR;

        if (POINTER_IS_NULL(chnStatus->callback) == false)
        {
            chnStatus->callback(chnStatus->parameter, chnStatus->status);
        }
    }
}

/*!
 * @brief Configures a simple single block data transfer with DMA.
 *
 * @param vtChannel DMA virtual channel number.
 * @param type Transfer type (M->M, P->M, M->P, P->P).
 * @param srcAddr A source register address or a source memory address.
 * @param destAddr A destination register address or a destination memory address.
 * @param transferSize The number of bytes to be transferred on every DMA write/read.
 *        Source/Dest share the same write/read size.
 * @param dataBufferSize The total number of bytes to be transferred.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS
 */
STATUS_T DMA_ConfigSingleBlockTransfer(uint8_t vtChannel,
                                       DMA_TRANSFER_TYPE_T type,
                                       uint32_t srcAddr,
                                       uint32_t destAddr,
                                       DMA_TRANSFER_SIZE_T transferSize,
                                       uint32_t dataBufferSize)
{
    STATUS_T state = STATUS_SUCCESS;

    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Compute the transfer offset, dmaBased on transfer size.
     * The number of bytes transferred in each source read/destination write
     * is obtained with the following formula:
     *    source_read_size = 2 ^ SSIZE
     *    destination_write_size = 2 ^ DSIZE
     */
    uint8_t txOffset = (uint8_t)(1U << ((uint8_t)transferSize));

    /* The number of bytes to be transferred (buffer size) must
     * be a multiple of the source read/destination write size
     */
    if ((dataBufferSize % txOffset) != 0U)
    {
        state = STATUS_ERROR;
    }

    if (state == STATUS_SUCCESS)
    {
        /* Clear transfer control descriptor for the current channel */
        DMA_HW_TCDClearReg(dmaRegBasePointer, dmaChannelNum);

#ifdef FEATURE_DMA_ENGINE_STALL
        /* Configure the DMA Engine to stall for a number of cycles after each R/W */
        DMA_HW_TCDCOnfigEngineStall(dmaRegBase, dmaChannel, DMA_ENGINE_STALL_4_CYCLES);
#endif

        DMA_HW_ConfigMinorLoopMappingCmd(dmaRegBasePointer, true);

        /* Configure source and destination addresses */
        DMA_HW_TCDConfigDestAddr(dmaRegBasePointer, dmaChannelNum, destAddr);
        DMA_HW_TCDConfigSrcAddr(dmaRegBasePointer, dmaChannelNum, srcAddr);

        /* Set transfer size (1B/2B/4B/16B/32B) */
        DMA_HW_TCDConfigAttribute(dmaRegBasePointer,
                                  dmaChannelNum,
                                  DMA_MODULO_OFF,
                                  DMA_MODULO_OFF,
                                  transferSize,
                                  transferSize);

        /* Configure source/destination offset. */
        if (type == DMA_TRANSFER_PERIPH2MEM)
        {
            DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, 0);
            DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, (int8_t)txOffset);
        }
        else if (type == DMA_TRANSFER_MEM2PERIPH)
        {
            DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, (int8_t)txOffset);
            DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, 0);
        }
        else if (type == DMA_TRANSFER_MEM2MEM)
        {
            DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, (int8_t)txOffset);
            DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, (int8_t)txOffset);
        }
        else if (type == DMA_TRANSFER_PERIPH2PERIPH)
        {
            DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, 0);
            DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, 0);
        }

        /* Set the total number of bytes to be transfered */
        DMA_HW_TCDConfigNbytes(dmaRegBasePointer, dmaChannelNum, dataBufferSize);

        /* Set major iteration count to 1 (single block mode) */
        DMA_HW_TCDConfigMajorCount(dmaRegBasePointer, dmaChannelNum, 1U);

        /* Enable interrupt when the transfer completes */
        DMA_HW_TCDConfigMajorCompleteIntCmd(dmaRegBasePointer, dmaChannelNum, true);

        /* Set virtual channel status to normal */
        g_dmaState->vtChnState[vtChannel]->status = DMA_CHANNEL_NORMAL;
    }

    return state;
}

/*!
 * @brief Configures a multiple block data transfer with DMA.
 *
 * @param vtChannel DMA virtual channel number.
 * @param type Transfer type (M->M, P->M, M->P, P->P).
 * @param srcAddr A source register address or a source memory address.
 * @param destAddr A destination register address or a destination memory address.
 * @param transferSize The number of bytes to be transferred on every DMA write/read.
 *        Source/Dest share the same write/read size.
 * @param blockSize The total number of bytes inside a block.
 * @param blockCount The total number of data blocks (one block is transferred upon a DMA req).
 * @param disableReqOnCompletion This parameter specifies whether the DMA channel should
 *        be disabled when the transfer is complete (further reqs will remain untreated).
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS
 */
STATUS_T DMA_ConfigMultiBlockTransfer(uint8_t vtChannel,
                                      DMA_TRANSFER_TYPE_T type,
                                      uint32_t srcAddr,
                                      uint32_t destAddr,
                                      DMA_TRANSFER_SIZE_T transferSize,
                                      uint32_t blockSize,
                                      uint32_t blockCount,
                                      bool disableReqOnCompletion)
{
    STATUS_T state = STATUS_SUCCESS;
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Configure the transfer for one data block */
    state = DMA_ConfigSingleBlockTransfer(vtChannel, type, srcAddr, destAddr, transferSize, blockSize);

    if (state == STATUS_SUCCESS)
    {
        DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];

        /* Set the number of data blocks */
        DMA_HW_TCDConfigMajorCount(dmaRegBasePointer, dmaChannelNum, blockCount);

        /* Enable/disable reqs upon completion */
        DMA_HW_TCDConfigDisableDmaRequestAfterTCDDoneCmd(dmaRegBasePointer,
                                                         dmaChannelNum,
                                                         disableReqOnCompletion);
    }

    return state;
}

/*!
 * @brief Copy the channel configuration to the software TCD structure.
 *
 * This function copies the properties from the channel configuration to the software TCD structure;
 * the address of the software TCD can be used to enable scatter/gather operation
 * (pointer to the next TCD).

 * @param cfg Pointer to the channel configuration structure.
 * @param stcdcfg Pointer to the software TCD structure.
 *
 * @retval None.
 */
void DMA_CopyConfigurationToSTCD(const DMA_TRANSFER_CONFIG_T *cfg, DMA_SOFTWARE_TCD_T *stcdcfg)
{
    if ((POINTER_IS_NULL(cfg) == false) && (POINTER_IS_NULL(stcdcfg) == false))
    {
        /* Clear the array of software TCDs passed by the user */
        DMA_ClearSoftwareTCD(stcdcfg);

        /* Set the software TCD fields */
        stcdcfg->ATTR = (uint16_t)(((uint16_t)cfg->srcModulo << 11U) |
                                   ((uint16_t)cfg->srcTransferSize << 8U) |
                                   ((uint16_t)cfg->destModulo << 3U) |
                                   ((uint16_t)cfg->destTransferSize << 0U));

        stcdcfg->SADDR = cfg->srcAddr;
        stcdcfg->SOFF = cfg->srcOffset;
        stcdcfg->DADDR = cfg->destAddr;
        stcdcfg->DOFF = cfg->destOffset;
        stcdcfg->SLAST = cfg->lastSrcAddrAdjust;
        stcdcfg->NBYTES = cfg->minorByteTransferNum;
        stcdcfg->CITER = (uint16_t) cfg->loopTransferCfg->numOfMajorLoopIteration;

        if (cfg->scatterGatherEn == false)
        {
            stcdcfg->DLAST_SGA = cfg->lastDestAddrAdjust;
        }
        else
        {
            stcdcfg->DLAST_SGA = (int32_t) cfg->scatterGatherNextAddrOfDescriptor;
        }

        stcdcfg->CSR = (uint16_t) (((cfg->interruptEn ? 1UL : 0UL) << 1U) |
                                   ((cfg->scatterGatherEn ? 1UL : 0UL) << 4U));

        stcdcfg->BITER = (uint16_t) cfg->loopTransferCfg->numOfMajorLoopIteration;
    }
}

/*!
 * @brief Copy the channel configuration to the TCD registers.
 *
 * @param vtChannel DMA virtual channel number.
 * @param tcdcfg Pointer to the channel configuration structure.
 *
 * @retval None.
 */
void DMA_CopyConfigurationToTCDReg(uint8_t vtChannel, const DMA_TRANSFER_CONFIG_T *tcdcfg)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Clear TCD registers */
    DMA_HW_TCDClearReg(dmaRegBasePointer, dmaChannelNum);

#ifdef FEATURE_DMA_ENGINE_STALL
    /* Configure the DMA Engine to stall for a number of cycles after each R/W */
    DMA_HW_TCDCOnfigEngineStall(dmaRegBasePointer, dmaChannelNum, DMA_ENGINE_STALL_4_CYCLES);
#endif

    /* Config source and destination addresses */
    DMA_HW_TCDConfigDestAddr(dmaRegBasePointer, dmaChannelNum, tcdcfg->destAddr);
    DMA_HW_TCDConfigSrcAddr(dmaRegBasePointer, dmaChannelNum, tcdcfg->srcAddr);

    /* Config source/destination modulo feature and transfer size */
    DMA_HW_TCDConfigAttribute(dmaRegBasePointer,
                              dmaChannelNum,
                              tcdcfg->srcModulo,
                              tcdcfg->destModulo,
                              tcdcfg->srcTransferSize,
                              tcdcfg->destTransferSize);

    /* Set source/destination offset and last adjustment; for scatter/gather operation, destination
     * last adjustment is the address of the next TCD structure to be loaded by the DMA engine */
    DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, tcdcfg->destOffset);
    DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, tcdcfg->srcOffset);
    DMA_HW_TCDConfigSrcLastAdjust(dmaRegBasePointer, dmaChannelNum, tcdcfg->lastSrcAddrAdjust);

    if (tcdcfg->scatterGatherEn == false)
    {
        DMA_HW_TCDConfigScatterGatherCmd(dmaRegBasePointer, dmaChannelNum, false);
        DMA_HW_TCDConfigDestLastAdjust(dmaRegBasePointer, dmaChannelNum, tcdcfg->lastDestAddrAdjust);
    }
    else
    {
        DMA_HW_TCDConfigScatterGatherCmd(dmaRegBasePointer, dmaChannelNum, true);
        DMA_HW_TCDConfigScatterGatherLink(dmaRegBasePointer,
                                          dmaChannelNum,
                                          tcdcfg->scatterGatherNextAddrOfDescriptor);
    }

    /* Configure channel interrupt */
    DMA_HW_TCDConfigMajorCompleteIntCmd(dmaRegBasePointer, dmaChannelNum, tcdcfg->interruptEn);

    /* If loop configuration is available, copy minor/major loop setup to registers */
    if (POINTER_IS_NULL(tcdcfg->loopTransferCfg))
    {
        DMA_HW_TCDConfigNbytes(dmaRegBasePointer, dmaChannelNum, tcdcfg->minorByteTransferNum);
    }
    else
    {
        DMA_HW_TCDConfigSrcMinorLoopOffsetCmd(dmaRegBasePointer,
                                              dmaChannelNum,
                                              tcdcfg->loopTransferCfg->srcOffsetEn);
        DMA_HW_TCDConfigDestMinorLoopOffsetCmd(dmaRegBasePointer,
                                               dmaChannelNum,
                                               tcdcfg->loopTransferCfg->destOffsetEn);
        DMA_HW_TCDConfigMinorLoopOffset(dmaRegBasePointer,
                                        dmaChannelNum,
                                        tcdcfg->loopTransferCfg->minorLoopOffset);
        DMA_HW_TCDConfigNbytes(dmaRegBasePointer, dmaChannelNum, tcdcfg->minorByteTransferNum);

        DMA_HW_TCDConfigChannelMinorLink(dmaRegBasePointer,
                                         dmaChannelNum,
                                         tcdcfg->loopTransferCfg->minorLoopChannelLinkNum,
                                         tcdcfg->loopTransferCfg->minorLoopChannelLinkEn);
        DMA_HW_TCDConfigChannelMajorLink(dmaRegBasePointer,
                                         dmaChannelNum,
                                         tcdcfg->loopTransferCfg->majorLoopChannelLinkNum,
                                         tcdcfg->loopTransferCfg->majorLoopChannelLinkEn);

        DMA_HW_TCDConfigMajorCount(dmaRegBasePointer,
                                   dmaChannelNum,
                                   tcdcfg->loopTransferCfg->numOfMajorLoopIteration);
    }
}

/*!
 * @brief Configures the DMA transfer in loop mode.
 *
 * @param vtChannel DMA virtual channel number.
 * @param transferCfg Pointer to the transfer configuration strucutre; this structure defines fields for setting
 * up the basic transfer and also a pointer to a memory strucure that defines the loop chain properties (minor/major).
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS
 */
STATUS_T DMA_ConfigLoopTransfer(uint8_t vtChannel, const DMA_TRANSFER_CONFIG_T *transferCfg)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];

    DMA_HW_ConfigMinorLoopMappingCmd(dmaRegBasePointer, true);

    /* Write the configuration in the transfer control descriptor registers */
    DMA_CopyConfigurationToTCDReg(vtChannel, transferCfg);

    /* Set virtual channel status to normal */
    g_dmaState->vtChnState[vtChannel]->status = DMA_CHANNEL_NORMAL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Configures the DMA transfer in a scatter-gather mode.
 *
 * @param vtChannel DMA virtual channel number.
 * @param stcd Array of empty software TCD structures. The user must prepare this memory block. We don't need a
 *        software TCD structure for the first descriptor, since the configuration is pushed directly to registers.The "stcd"
 *        buffer must align with 32 bytes; if not, an error occurs in the DMA driver. Thus, the required
 *        memory size for "stcd" is equal to tcdCount * size_of(dma_software_tcd_t) - 1; the driver will take
 *        care of the memory alignment if the provided memory buffer is big enough. For proper allocation of the
 *        "stcd" buffer it is recommended to use STCD_SIZE macro.
 * @param transferSize The number of bytes to be transferred on every DMA write/read.
 * @param bytesOnEachRequest Bytes to be transferred in each DMA req.
 * @param srcList Data structure storing the address, length and type of transfer (M->M, M->P, P->M, P->P) for
 *        the bytes to be transferred for source memory blocks. If the source memory is peripheral, the length
 *        is not used.
 * @param destList Data structure storing the address, length and type of transfer (M->M, M->P, P->M, P->P) for
 *        the bytes to be transferred for destination memory blocks. In the memory-to-memory transfer mode, the
 *        user must ensure that the length of the destination scatter gather list is equal to the source
 *        scatter gather list. If the destination memory is a peripheral register, the length is not used.
 * @param tcdCount The number of TCD memory blocks contained in the scatter gather list.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS
 */
STATUS_T DMA_ConfigScatterGatherTransfer(uint8_t vtChannel,
                                         DMA_SOFTWARE_TCD_T *stcd,
                                         DMA_TRANSFER_SIZE_T transferSize,
                                         uint32_t bytesOnEachRequest,
                                         const DMA_SCATTER_GATHER_LIST_T *srcList,
                                         const DMA_SCATTER_GATHER_LIST_T *destList,
                                         uint8_t tcdCount)
{
    STATUS_T state = STATUS_SUCCESS;
    DMA_LOOP_TRANSFER_CONFIG_T dmaLoopCfg;
    DMA_TRANSFER_CONFIG_T dmaTransferCfg;
    uint8_t index = 0U;
    uint16_t txOffset = 0U;
    uint32_t stcdAlignedAddr = STCD_ADDR(stcd);
    DMA_SOFTWARE_TCD_T *dmaSwTcdAddr = (DMA_SOFTWARE_TCD_T *)stcdAlignedAddr;

    g_dmaState->vtChnState[vtChannel]->status = DMA_CHANNEL_NORMAL;

    /* Compute the transfer offset, dmaBased on transfer size.
     * The number of bytes transferred in each source read/destination write
     * is obtained with the following formula:
     *    source_read_size = 2 ^ SSIZE
     *    destination_write_size = 2 ^ DSIZE
     */
    txOffset = (uint16_t) (1UL << ((uint16_t)transferSize));

    /* The number of bytes to be transferred on each req must
     * be a multiple of the source read/destination write size
     */
    if ((bytesOnEachRequest % txOffset) != 0U)
    {
        state = STATUS_ERROR;
    }

    /* Clear the configuration structures before initializing them. */
    DMA_ClearStructure((uint8_t *)(&dmaLoopCfg), sizeof(DMA_LOOP_TRANSFER_CONFIG_T));
    DMA_ClearStructure((uint8_t *)(&dmaTransferCfg), sizeof(DMA_TRANSFER_CONFIG_T));

    /* Configure the transfer for scatter/gather mode. */
    dmaTransferCfg.interruptEn = true;
    dmaTransferCfg.scatterGatherEn = true;
    dmaTransferCfg.loopTransferCfg = &dmaLoopCfg;
    dmaTransferCfg.loopTransferCfg->srcOffsetEn = false;
    dmaTransferCfg.loopTransferCfg->destOffsetEn = false;
    dmaTransferCfg.loopTransferCfg->minorLoopChannelLinkEn = false;
    dmaTransferCfg.loopTransferCfg->majorLoopChannelLinkEn = false;
    dmaTransferCfg.lastSrcAddrAdjust = 0;
    dmaTransferCfg.lastDestAddrAdjust = 0;
    dmaTransferCfg.srcModulo = DMA_MODULO_OFF;
    dmaTransferCfg.destModulo = DMA_MODULO_OFF;
    dmaTransferCfg.srcTransferSize = transferSize;
    dmaTransferCfg.destTransferSize = transferSize;
    dmaTransferCfg.minorByteTransferNum = bytesOnEachRequest;

    /* Copy scatter/gather lists to transfer configuration */
    for (index = 0U; (index < tcdCount) && (state == STATUS_SUCCESS); index++)
    {
        dmaTransferCfg.srcAddr = srcList[index].address;
        dmaTransferCfg.destAddr = destList[index].address;
        if ((srcList[index].length != destList[index].length) || (srcList[index].type != destList[index].type))
        {
            state = STATUS_ERROR;
            break;
        }
        dmaTransferCfg.loopTransferCfg->numOfMajorLoopIteration = srcList[index].length/bytesOnEachRequest;

        if (srcList[index].type == DMA_TRANSFER_PERIPH2MEM)
        {
            /* Configure Source Read. */
            dmaTransferCfg.srcOffset = 0;
            /* Configure Dest Write. */
            dmaTransferCfg.destOffset = (int16_t) txOffset;
        }
        else if (srcList[index].type == DMA_TRANSFER_MEM2PERIPH)
        {
            /* Configure Source Read. */
            dmaTransferCfg.srcOffset = (int16_t) txOffset;
            /* Configure Dest Write. */
            dmaTransferCfg.destOffset = 0;
        }
        else if (srcList[index].type == DMA_TRANSFER_MEM2MEM)
        {
            /* Configure Source Read. */
            dmaTransferCfg.srcOffset = (int16_t) txOffset;
            /* Configure Dest Write. */
            dmaTransferCfg.destOffset = (int16_t) txOffset;
        }
        else if (srcList[index].type == DMA_TRANSFER_PERIPH2PERIPH)
        {
            /* Configure Source Read. */
            dmaTransferCfg.srcOffset = 0;
            /* Configure Dest Write. */
            dmaTransferCfg.destOffset = 0;
        }

        /* Configure the pointer to next software TCD structure; for the last one, this address should be 0 */
        if (index != ((uint8_t)(tcdCount - 1U)))
        {
            DMA_SOFTWARE_TCD_T * ptNextAddr = &dmaSwTcdAddr[index];
            dmaTransferCfg.scatterGatherNextAddrOfDescriptor = ((uint32_t) ptNextAddr);
        }
        else
        {
            dmaTransferCfg.scatterGatherNextAddrOfDescriptor = 0U;
        }

        if (index != 0U)
        {
            /* Copy configuration to software TCD structure */
            DMA_CopyConfigurationToSTCD(&dmaTransferCfg, &dmaSwTcdAddr[index - 1U]);
        }
        else
        {
            /* Push the configuration for the first descriptor to registers */
            DMA_CopyConfigurationToTCDReg(vtChannel, &dmaTransferCfg);
        }
    }

    return state;
}

/*!
 * @brief Starts an DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_StartChannel(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Enable the DMA reqs for current channel */
    DMA_HW_ConfigDmaRequestCmd(dmaRegBasePointer, dmaChannelNum, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Stops the DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval STATUS_ERROR or STATUS_SUCCESS.
 */
STATUS_T DMA_StopChannel(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Disable the DMA reqs for current channel */
    DMA_HW_ConfigDmaRequestCmd(dmaRegBasePointer, dmaChannelNum, false);

    return STATUS_SUCCESS;
}

/*!
 * @brief Clears all registers to 0 for the channel's TCD.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval None.
 */
void DMA_ClearTCD(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Clear the TCD memory */
    DMA_HW_TCDClearReg(dmaRegBasePointer, dmaChannelNum);
}

/*!
 * @brief Configures the source address for the DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 * @param address The pointer to the source memory address.
 *
 * @retval None.
 */
void DMA_ConfigSrcAddr(uint8_t vtChannel, uint32_t address)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source address */
    DMA_HW_TCDConfigSrcAddr(dmaRegBasePointer, dmaChannelNum, address);
}

/*!
 * @brief Configures the source address signed offset for the DMA channel.
 *
 * Sign-extended offset applied to the current source address to form the next-state value as each
 * source read is complete.
 *
 * @param vtChannel DMA virtual channel number.
 * @param offset Signed-offset for source address.
 *
 * @retval None.
 */
void DMA_ConfigSrcOffset(uint8_t vtChannel, int16_t offset)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source offset */
    DMA_HW_TCDConfigSrcOffset(dmaRegBasePointer, dmaChannelNum, offset);
}

/*!
 * @brief Configures the source data chunk size (transferred in a read sequence).
 *
 * Source data read transfer size (1/2/4/16/32 bytes).
 *
 * @param vtChannel DMA virtual channel number.
 * @param size Source transfer size.
 *
 * @retval None.
 */
void DMA_ConfigSrcReadChunkSize(uint8_t vtChannel, DMA_TRANSFER_SIZE_T size)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source transfer size */
    DMA_HW_TCDConfigSrcTransferSize(dmaRegBasePointer, dmaChannelNum, size);
}

/*!
 * @brief Configures the source address last adjustment.
 *
 * @param vtChannel DMA virtual channel number.
 * @param adjust Adjustment value.
 *
 * @retval None.
 */
void DMA_ConfigSrcLastAddrAdjustment(uint8_t vtChannel, int32_t adjust)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source last adjustment */
    DMA_HW_TCDConfigSrcLastAdjust(dmaRegBasePointer, dmaChannelNum, adjust);
}

/*!
 * @brief Configures the destination address last adjustment.
 *
 * @param vtChannel DMA virtual channel number.
 * @param adjust Adjustment value.
 *
 * @retval None.
 */
void DMA_ConfigDestLastAddrAdjustment(uint8_t vtChannel, int32_t adjust)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source last adjustment */
    DMA_HW_TCDConfigDestLastAdjust(dmaRegBasePointer, dmaChannelNum, adjust);
}

/*!
 * @brief Configures the destination address for the DMA channel.
 *
 * @param vtChannel DMA virtual channel number.
 * @param address The pointer to the destination memory address.
 *
 * @retval None.
 */
void DMA_ConfigDestAddr(uint8_t vtChannel, uint32_t address)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD destination address */
    DMA_HW_TCDConfigDestAddr(dmaRegBasePointer, dmaChannelNum, address);
}

/*!
 * @brief Configures the destination address signed offset for the DMA channel.
 *
 * Sign-extended offset applied to the current destination address form the next-state value as each
 * destination write is complete.
 *
 * @param vtChannel DMA virtual channel number.
 * @param offset signed-offset
 *
 * @retval None.
 */
void DMA_ConfigDestOffset(uint8_t vtChannel, int16_t offset)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD destination offset */
    DMA_HW_TCDConfigDestOffset(dmaRegBasePointer, dmaChannelNum, offset);
}

/*!
 * @brief Configures the destination data chunk size (transferred in a write sequence).
 *
 * Destination data write transfer size (1/2/4/16/32 bytes).
 *
 * @param vtChannel DMA virtual channel number.
 * @param size Destination transfer size.
 *
 * @retval None.
 */
void DMA_ConfigDestWriteChunkSize(uint8_t vtChannel, DMA_TRANSFER_SIZE_T size)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD source transfer size */
    DMA_HW_TCDConfigDestTransferSize(dmaRegBasePointer, dmaChannelNum, size);
}

/*!
 * @brief Configures the number of bytes to be transferred in each service req of the channel.
 *
 * Sets the number of bytes to be transferred each time a req is received (one major loop iteration).
 * This number needs to be a multiple of the source/destination transfer size, as the data block will be
 * transferred within multiple read/write sequences (minor loops).
 *
 * @param vtChannel DMA virtual channel number.
 * @param nbytes Number of bytes to be transferred in each service req of the channel
 *
 * @retval None.
 */
void DMA_ConfigMinorLoopBlockSize(uint8_t vtChannel, uint32_t nbytes)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the channel TCD minor loop block size */
    DMA_HW_TCDConfigNbytes(dmaRegBasePointer, dmaChannelNum, nbytes);
}

/*!
 * @brief Configures the number of major loop iterations.
 *
 * Sets the number of major loop iterations; each major loop iteration will be served upon a req
 * for the current channel, transferring the data block configured for the minor loop (NBYTES).
 *
 * @param vtChannel DMA virtual channel number.
 * @param majorLoopCount Number of major loop iterations.
 *
 * @retval None.
 */
void DMA_ConfigMajorLoopIterationCount(uint8_t vtChannel, uint32_t majorLoopCount)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the major loop iteration count */
    DMA_HW_TCDConfigMajorCount(dmaRegBasePointer, dmaChannelNum, majorLoopCount);
}

/*!
 * @brief Returns the remaining major loop iteration count.
 *
 * Gets the number minor loops yet to be triggered (major loop iterations).
 *
 * @param vtChannel DMA virtual channel number.

 * @retval number of major loop iterations yet to be triggered
 */
uint32_t DMA_ReadRemainingMajorIterationsCount(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    const DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Retrieve the number of minor loops yet to be triggered */
    return DMA_HW_TCDReadCurrentMajorCount(dmaRegBasePointer, dmaChannelNum);
}

/*!
 * @brief Configures the memory address of the next TCD, in scatter/gather mode.
 *
 * @param vtChannel DMA virtual channel number.
 * @param nextTCDAddr The address of the next TCD to be linked to this TCD.
 *
 * @retval None.
 */
void DMA_ConfigScatterGatherLink(uint8_t vtChannel, uint32_t nextTCDAddr)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Config the memory address of the next TCD */
    DMA_HW_TCDConfigScatterGatherLink(dmaRegBasePointer, dmaChannelNum, nextTCDAddr);
}

/*!
 * @brief Disables/Enables the DMA req after the major loop completes for the TCD.
 *
 * If disabled, the DMA hardware automatically clears the corresponding DMA req when the
 * current major iteration count reaches zero.
 *
 * @param vtChannel DMA virtual channel number.
 * @param disable Disable (true)/Enable (false) DMA req after TCD complete.
 *
 * @retval None.
 */
void DMA_DisableRequestsOnTransferComplete(uint8_t vtChannel, bool disable)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Disables/Enables the DMA req upon TCD completion */
    DMA_HW_TCDConfigDisableDmaRequestAfterTCDDoneCmd(dmaRegBasePointer, dmaChannelNum, disable);
}

/*!
 * @brief Config Disables/Enables the DMA channel interrupt reqs.
 *
 * @param vtChannel DMA virtual channel number.
 * @param interrupt Interrupt event (error/half major loop/complete major loop).
 * @param enable Enable (true)/Disable (false) interrupts for the current channel.
 *
 * @retval None.
 */
void DMA_ConfigInterrupt(uint8_t vtChannel, DMA_CHANNEL_INTERRUPT_T intSrc, bool enable)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Disables/Enables the channel interrupt reqs. */
    if(intSrc == DMA_CHANNEL_ERR_INT)
    {
        DMA_HW_ConfigErrorIntCmd(dmaRegBasePointer, dmaChannelNum, enable);
    }
    else if(intSrc == DMA_CHANNEL_HALF_MAJOR_LOOP_INT)
    {
        DMA_HW_TCDConfigMajorHalfCompleteIntCmd(dmaRegBasePointer, dmaChannelNum, enable);
    }
    else if(intSrc == DMA_CHANNEL_MAJOR_LOOP_INT)
    {
        DMA_HW_TCDConfigMajorCompleteIntCmd(dmaRegBasePointer, dmaChannelNum, enable);
    }
}

/*!
 * @brief Cancel the running transfer.
 *
 * This function cancels the current transfer, optionally signalling an error.
 *
 * @param bool error If true, an error will be logged for the current transfer.
 *
 * @retval None.
 */
void DMA_CancelTransfer(bool error)
{
    uint32_t dmaIns = 0U;
    DMA_T *dmaRegBasePointer = NULL;

    for(dmaIns = 0U; dmaIns < (uint32_t)DMA_INSTANCE_COUNT; dmaIns++)
    {
        /* Cancel the running transfer. */
        dmaRegBasePointer = g_dmaBasePointer[dmaIns];
        if (error == false)
        {
            DMA_HW_DoCancelTransfer(dmaRegBasePointer);
        }
        else
        {
            DMA_HW_CancelTransferWithError(dmaRegBasePointer);
        }
    }
}

/*!
 * @brief Triggers a sw req for the current channel.
 *
 * This function starts a transfer using the current channel (sw req).
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval None.
 */
void DMA_TriggerSwRequest(uint8_t vtChannel)
{
    uint8_t dmaIns = (uint8_t)FEATURE_DMA_VCHN_TO_INSTANCE(vtChannel);
    DMA_T *dmaRegBasePointer = g_dmaBasePointer[dmaIns];
    uint8_t dmaChannelNum = (uint8_t)FEATURE_DMA_VCHN_TO_CHN(vtChannel);

    /* Trigger the channel transfer. */
    DMA_HW_TriggerChannelStart(dmaRegBasePointer, dmaChannelNum);
}

/*!
 * @brief Read the DMA channel status.
 *
 * @param vtChannel DMA virtual channel number.
 *
 * @retval Channel status.
 */
DMA_CHANNEL_STATUS_T DMA_ReadChannelStatus(uint8_t vtChannel)
{
    return g_dmaState->vtChnState[vtChannel]->status;
}

/*!
 * @brief Read the address of the selected DMA module.
 *
 * @param instance DMA instance to be returned.
 *
 * @retval DMA register dmaBase address
 */
DMA_T *DMA_ReadDmaRegBaseAddr(uint32_t instance)
{
    DMA_INSTANCE_VALIDITY(instance);

    return g_dmaBasePointer[instance];
}

/*!
 * @brief DMA0 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA0_IRQHandler(void)
{
    DMA_IRQHandler(0U);
}

/*!
 * @brief DMA1 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA1_IRQHandler(void)
{
    DMA_IRQHandler(1U);
}

/*!
 * @brief DMA2 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA2_IRQHandler(void)
{
    DMA_IRQHandler(2U);
}

/*!
 * @brief DMA3 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA3_IRQHandler(void)
{
    DMA_IRQHandler(3U);
}

#if (FEATURE_DMA_VIRTUAL_CHANNELS > 4U)
/*!
 * @brief DMA4 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA4_IRQHandler(void)
{
    DMA_IRQHandler(4U);
}

/*!
 * @brief DMA5 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA5_IRQHandler(void)
{
    DMA_IRQHandler(5U);
}

/*!
 * @brief DMA6 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA6_IRQHandler(void)
{
    DMA_IRQHandler(6U);
}

/*!
 * @brief DMA7 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA7_IRQHandler(void)
{
    DMA_IRQHandler(7U);
}
#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 4U) */

#if (FEATURE_DMA_VIRTUAL_CHANNELS > 8U)
/*!
 * @brief DMA8 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA8_IRQHandler(void)
{
    DMA_IRQHandler(8U);
}

/*!
 * @brief DMA9 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA9_IRQHandler(void)
{
    DMA_IRQHandler(9U);
}

/*!
 * @brief DMA10 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA10_IRQHandler(void)
{
    DMA_IRQHandler(10U);
}

/*!
 * @brief DMA11 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA11_IRQHandler(void)
{
    DMA_IRQHandler(11U);
}

/*!
 * @brief DMA12 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA12_IRQHandler(void)
{
    DMA_IRQHandler(12U);
}

/*!
 * @brief DMA13 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA13_IRQHandler(void)
{
    DMA_IRQHandler(13U);
}

/*!
 * @brief DMA14 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA14_IRQHandler(void)
{
    DMA_IRQHandler(14U);
}

/*!
 * @brief DMA15 IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA15_IRQHandler(void)
{
    DMA_IRQHandler(15U);
}
#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 8U) */

/*!
 * @brief DMA Error IRQ handler.
 *
 * @param None.
 *
 * @retval None.
 *
 * @note The function name with the same name in the startup code.
 */
void DMA_Error_IRQHandler(void)
{
    const DMA_T *dmaRegBasePointer = DMA_ReadDmaRegBaseAddr(0U);
    uint32_t errorStatusFlag = DMA_HW_ReadErrorIntStatusFlag(dmaRegBasePointer);
    uint8_t vtChannel = 0U;

    for (vtChannel = 0U; vtChannel < FEATURE_DMA_VIRTUAL_CHANNELS; vtChannel++)
    {
        if((errorStatusFlag & DMA_ERR_LSB_MASK) != 0UL)
        {
            DMA_ErrorIRQHandler(vtChannel);
        }
        errorStatusFlag = errorStatusFlag >> 1U;
    }
}

/*******************************************************************************
                            PRIVATE FUNCTIONS
*******************************************************************************/

/*!
 * @brief Clears all bytes at the passed structure pointer.
 *
 * Initializes the DMAMUX module to the reset state.
 *
 * @param structPtr structure pointer.
 * @param length structure size.
 *
 * @retval None.
 */
static void DMA_ClearStructure(uint8_t *structPtr, size_t length)
{
    while (length > 0U)
    {
        length--;
        *structPtr = 0;
        structPtr++;
    }
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Halts or does not halt the DMA module when an error occurs.
 *
 * @param dmaBase Register base address for DMA module.
 * @param haltOnError Halts (true) or not halt (false) DMA module when an error occurs.
 *
 * @retval None.
 */
void DMA_HW_ConfigHaltOnErrorCmd(DMA_T *dmaBase, bool haltOnError)
{
    if (haltOnError)
    {
        dmaBase->CTRL.bit.HALTEFLG = DMA_CTRL_HALTEFLG_1;
    }
    else
    {
        dmaBase->CTRL.bit.HALTEFLG = DMA_CTRL_HALTEFLG_0;
    }
}

/*!
 * @brief Config the DMA channel priority.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param priority Priority of the DMA channel.
 * Different channels should have different priority
 * setting inside a group.
 *
 * @retval None.
 */
void DMA_HW_ConfigChannelPriority(DMA_T *dmaBase, uint8_t channel, DMA_CHANNEL_PRIORITY_T priority)
{
    uint8_t index = (uint8_t)FEATURE_DMA_CHN_TO_CHPRI_INDEX(channel);

    dmaBase->CHPRI[index].reg &= ~(uint8_t)(0x0FU << 0U);
    dmaBase->CHPRI[index].reg |= ((uint8_t)priority << 0U);
}

/*!
 * @brief Config the channel arbitration algorithm.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channelArbitration Round-Robin way or fixed priority way.
 *
 * @retval None.
 */
void DMA_HW_ConfigChannelArbitrationMode(DMA_T *dmaBase, DMA_ARBITRATION_ALGORITHM_T channelArbitration)
{
    dmaBase->CTRL.bit.CHASEL = (uint32_t)channelArbitration;
}

/*!
 * @brief Gets the channel arbitration algorithm.
 *
 * @param dmaBase Register base address for DMA module.

 * @retval DMA_ARBITRATION_ALGORITHM_T variable indicating the selected
 * channel arbitration: Round-Robin way or fixed priority way.
 */
DMA_ARBITRATION_ALGORITHM_T DMA_HW_ReadChannelArbitrationMode(const DMA_T *dmaBase)
{
    DMA_ARBITRATION_ALGORITHM_T retVal;

    if (dmaBase->CTRL.bit.CHASEL != 0u)
    {
        retVal = DMA_ARBITRATION_ROUND_ROBIN;
    }
    else
    {
        retVal = DMA_ARBITRATION_FIXED_PRIORITY;
    }
    return retVal;
}

/*!
 * @brief COnfig Enables/Disables the minor loop mapping.
 *
 * @param dmaBase Register base address for DMA module.
 * @param enable Enables (true) or Disable (false) minor loop mapping.
 *
 * @retval None.
 */
void DMA_HW_ConfigMinorLoopMappingCmd(DMA_T *dmaBase, bool enable)
{
    if (enable)
    {
        dmaBase->CTRL.bit.MINLMEN = DMA_CTRL_MINLMEN_1;
    }
    else
    {
        dmaBase->CTRL.bit.MINLMEN = DMA_CTRL_MINLMEN_0;
    }
}

/*!
 * @brief Gets the DMA error interrupt status.
 *
 * @param dmaBase Register base address for DMA module.
 *
 * @retval 32 bit variable indicating error channels.
 * If error happens on DMA channel n,
 * the bit n of this variable is '1'.
 * If not, the bit n of this variable is '0'.
 */
uint32_t DMA_HW_ReadErrorIntStatusFlag(const DMA_T *dmaBase)
{
    return dmaBase->CHESTS.reg;
}

/*!
 * @brief Clears the error interrupt status for the DMA channel or channels.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 *
 * @retval None.
 */
void DMA_HW_ClearErrorIntStatusFlag(DMA_T *dmaBase, uint8_t channel)
{
    dmaBase->ESTSCLR.reg = channel;
}

/*!
 * @brief Clears the done status for a channel or all channels.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 *
 * @retval None.
 */
void DMA_HW_ClearDoneStatusFlag(DMA_T *dmaBase, uint8_t channel)
{
    dmaBase->DNESTSCLR.reg = channel;
}

/*!
 * @brief Triggers the DMA channel.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 *
 * @retval None.
 */
void DMA_HW_TriggerChannelStart(DMA_T *dmaBase, uint8_t channel)
{
    dmaBase->STATSET.reg = channel;
}

/*!
 * @brief Clears the interrupt status for the DMA channel or all channels.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 *
 * @retval None.
 */
void DMA_HW_ClearIntStatusFlag(DMA_T *dmaBase, uint8_t channel)
{
    dmaBase->INTREQCLR.reg = (uint8_t)channel;
}

#ifdef FEATURE_DMA_ENGINE_STALL
/*!
 * @brief Configures DMA engine to stall for a number of cycles after each R/W.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 * @param cycles Number of cycles the DMA engine is stalled after each R/W.
 *
 * @retval None.
 */
void DMA_HW_TCDCOnfigEngineStall(DMA_T *dmaBase, uint8_t channel, DMA_ENGINE_STALL_T cycles)
{
    dmaBase->TCD[channel].CSTS.bit.BWCTRL = DMA_TCD0_CSTS_BWCTRL_0;
    dmaBase->TCD[channel].CSTS.bit.BWCTRL = (uint16_t)cycles;
}
#endif

/*!
 * @brief Configures the source address for the hardware TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param address The pointer to the source memory address.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigSrcAddr(DMA_T *dmaBase, uint8_t channel, uint32_t address)
{
    dmaBase->TCD[channel].SADDR.reg = address;
}

/*!
 * @brief Configures the source address signed offset for the hardware TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param offset signed-offset for source address.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigSrcOffset(DMA_T *dmaBase, uint8_t channel, int16_t offset)
{
    dmaBase->TCD[channel].SADDROF.reg = (uint16_t)offset;
}

/*!
 * @brief Configures the source transfer size.
 *
 * Configures the source data read transfer size (1/2/4/16/32 bytes).
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param size Source transfer size.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigSrcTransferSize(DMA_T *dmaBase, uint8_t channel, DMA_TRANSFER_SIZE_T size)
{
    dmaBase->TCD[channel].TATT.bit.STSIZE = (uint16_t)(size);
}

/*!
 * @brief Configures the destination transfer size.
 *
 * Configures the destination data write transfer size (1/2/4/16/32 bytes).
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param size Destination transfer size.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDestTransferSize(DMA_T *dmaBase, uint8_t channel, DMA_TRANSFER_SIZE_T size)
{
    dmaBase->TCD[channel].TATT.bit.DTSIZE = (uint16_t)(size);
}

/*!
 * @brief Config Enables/disables the source minor loop offset feature for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param enable Enables (true) or disables (false) source minor loop offset.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigSrcMinorLoopOffsetCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    if (dmaBase->CTRL.bit.MINLMEN != 0U)
    {
        dmaBase->TCD[channel].MINBTCNT.SMINLOF_MINLMOFEN.bit.SMINLOFEN = enable ? 1U : 0U;
    }
}

/*!
 * @brief Config Enables/disables the destination minor loop offset feature for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param enable Enables (true) or disables (false) destination minor loop offset.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDestMinorLoopOffsetCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    if (dmaBase->CTRL.bit.MINLMEN != 0U)
    {
        dmaBase->TCD[channel].MINBTCNT.SMINLOF_MINLMOFEN.bit.DMINLOFEN = enable ? 1U : 0U;
    }
}

/*!
 * @brief Configures the last source address adjustment for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param size adjustment value
 *
 * @retval None.
 */
void DMA_HW_TCDConfigSrcLastAdjust(DMA_T *dmaBase, uint8_t channel, int32_t size)
{
    dmaBase->TCD[channel].SADDRAV.reg = (uint32_t)size;
}

/*!
 * @brief Configures the destination address for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param address The pointer to the destination address.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDestAddr(DMA_T *dmaBase, uint8_t channel, uint32_t address)
{
    dmaBase->TCD[channel].DADDR.reg = address;
}

/*!
 * @brief Configures the destination address signed offset for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param offset signed-offset
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDestOffset(DMA_T *dmaBase, uint8_t channel, int16_t offset)
{
    dmaBase->TCD[channel].DADDRSOF.reg = (uint16_t)offset;
}

/*!
 * @brief Configures the last source address adjustment.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param adjust adjustment value
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDestLastAdjust(DMA_T *dmaBase, uint8_t channel, int32_t adjust)
{
    dmaBase->TCD[channel].DADDRAVSGA.reg = (uint32_t)adjust;
}

/*!
 * @brief Config Enables/Disables the scatter/gather feature for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param enable Enables (true) /Disables (false) scatter/gather feature.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigScatterGatherCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    dmaBase->TCD[channel].CSTS.bit.SGFEN = enable ? 1U : 0U;
}

/*!
 * @brief Configures the major channel link the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param majorLinkChannel channel number for major link
 * @param enable Enables (true) or Disables (false) channel major link.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigChannelMajorLink(DMA_T *dmaBase, uint8_t channel, uint32_t majorLinkChannel, bool enable)
{
    /* Set Major Loop Link Channel Number */
    dmaBase->TCD[channel].CSTS.bit.MAJLINKCH = (uint16_t)majorLinkChannel;

    /* Enable/Disable channel major link */
    dmaBase->TCD[channel].CSTS.bit.MAJILINKEN = enable ? 1U : 0U;
}

/*!
 * @brief Config disables the DMA req after the major loop completes for the TCD.
 *
 * If disabled, the DMA hardware automatically clears the corresponding DMA req when the
 * current major iteration count reaches zero.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param disable Disable (true)/Enable (false) DMA req after TCD complete.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigDisableDmaRequestAfterTCDDoneCmd(DMA_T *dmaBase, uint8_t channel, bool disable)
{
    dmaBase->TCD[channel].CSTS.bit.DMAREQD = disable ? 1U : 0U;
}

/*!
 * @brief Enables/Disables the half complete interrupt for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param enable Enable (true) /Disable (false) half complete interrupt.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigMajorHalfCompleteIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    dmaBase->TCD[channel].CSTS.bit.MAJIHCMPIEN = enable ? 1U : 0U;
}

/*!
 * @brief Enables/Disables the interrupt after the major loop completes for the TCD.
 *
 * If enabled, the channel generates an interrupt req by setting the appropriate bit in the
 * interrupt register when the current major iteration count reaches zero.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param enable Enable (true) /Disable (false) interrupt after TCD done.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigMajorCompleteIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    dmaBase->TCD[channel].CSTS.bit.MAJICMPIEN = enable ? 1U : 0U;
}

/*!
 * @brief Enables/Disables the DMACHM channel.
 *
 * @param dmaBase Register base address for DMACHM module.
 * @param channel DMACHM channel number.
 * @param enable Enables (true) or Disables (false) DMACHM channel.
 *
 * @retval None.
 */
void DMACHM_HW_ConfigChannelCmd(DMACHM_T *dmaBase, uint8_t channel, bool enable)
{
    uint32_t regIdx = FEATURE_DMACHM_CHN_TO_REG_INDEX(channel);
    dmaBase->CFG[regIdx].bit.CHEN = enable ? 1U : 0U;
}

/*!
 * @brief Configure DMA Channel Trigger bit in DMACHM.
 *
 * Enables/Disables DMA Channel Trigger bit in DMACHM.
 *
 * @param dmaBase Register base address for DMACHM module.
 * @param channel DMACHM channel number.
 * @param enable/disable command.
 *
 * @retval None.
 */
void DMACHM_HW_ConfigChannelTrigger(DMACHM_T *dmaBase, uint8_t channel, bool enable)
{
    uint32_t regIdx = FEATURE_DMACHM_CHN_TO_REG_INDEX(channel);
    dmaBase->CFG[regIdx].bit.PRDTREN = enable ? 1U : 0U;
}

/*!
 * @brief Configures the DMA req for the DMACHM channel.
 *
 * @param dmaBase Register base address for DMACHM module.
 * @param channel DMACHM channel number.
 * @param source DMA req source.
 *
 * @retval None.
 */
void DMACHM_HW_ConfigChannelSource(DMACHM_T *dmaBase, uint8_t channel, uint8_t source)
{
    uint32_t regIdx = FEATURE_DMACHM_CHN_TO_REG_INDEX(channel);
    uint8_t regTemporValue;

    regTemporValue = dmaBase->CFG[regIdx].reg;
    regTemporValue &= (uint8_t)~(0x3FU << 0U);
    regTemporValue |= (uint8_t)((source & 0x3FU) << 0U);
    dmaBase->CFG[regIdx].reg = regTemporValue;
}

/*!
 * @brief Cancels the remaining data transfer.
 *
 * @param dmaBase Register base address for DMA module.
 *
 * @retval None.
 */
void DMA_HW_DoCancelTransfer(DMA_T *dmaBase)
{
    dmaBase->CTRL.bit.TXCNL = 1U;
    while (dmaBase->CTRL.bit.TXCNL != 0U)
    {}
}

/*!
 * @brief Cancels the remaining data transfer and treats it as an error condition.
 *
 * @param dmaBase Register base address for DMA module.
 *
 * @retval None.
 */
void DMA_HW_CancelTransferWithError(DMA_T *dmaBase)
{
    dmaBase->CTRL.bit.TXCNLERR = 1U;
    while (dmaBase->CTRL.bit.TXCNLERR != 0U)
    {}
}

/*!
 * @brief Enables/Disables the error interrupt for channels.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel Channel indicator.
 * @param enable Enable(true) or Disable (false) error interrupt.
 *
 * @retval None.
 */
void DMA_HW_ConfigErrorIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable)
{
    if (enable)
    {
        dmaBase->EIENSET.reg = channel;
    }
    else
    {
        dmaBase->EIENCLR.reg = channel;
    }
}

/*!
 * @brief Enables/Disables the DMA req for the channel or all channels.
 *
 * @param dmaBase Register base address for DMA module.
 * @param enable Enable(true) or Disable (false) DMA req.
 * @param channel Channel indicator.
 *
 * @retval None.
 */
void DMA_HW_ConfigDmaRequestCmd(DMA_T *dmaBase, uint8_t channel,bool enable)
{
    if (enable)
    {
        dmaBase->REQENSET.reg = channel;
    }
    else
    {
        dmaBase->REQENCLR.reg = channel;
    }
}

/*!
 * @brief Clears all registers to 0 for the hardware TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 *
 * @retval None.
 */
void DMA_HW_TCDClearReg(DMA_T *dmaBase, uint8_t channel)
{
    dmaBase->TCD[channel].MINBTCNT.MINBCNT_MINLMD.reg = 0U;
    dmaBase->TCD[channel].SADDR.reg = 0U;
    dmaBase->TCD[channel].SADDROF.reg = (uint16_t)0U;
    dmaBase->TCD[channel].TATT.reg = (uint16_t)0U;
    dmaBase->TCD[channel].SADDRAV.reg = 0U;
    dmaBase->TCD[channel].DADDR.reg = 0U;
    dmaBase->TCD[channel].DADDRSOF.reg = (uint16_t)0U;
    dmaBase->TCD[channel].CMAJICNT.LINKD.reg = (uint16_t)0U;
    dmaBase->TCD[channel].DADDRAVSGA.reg = 0U;
    dmaBase->TCD[channel].CSTS.reg = (uint16_t)0U;
    dmaBase->TCD[channel].MAJICNTSTAT.LINKD.reg = (uint16_t)0U;
}

/*!
 * @brief Clears DMA module registers to known state.
 *
 * @param dmaBase Register base address for DMA module.
 *
 * @retval None.
 */
void DMA_HW_ClearReg(DMA_T *dmaBase)
{
    uint8_t index;
    /* Clear the bit of CTRL register */
    dmaBase->CTRL.bit.DBGEN    = 0U;
    dmaBase->CTRL.bit.CHASEL   = 0U;
    dmaBase->CTRL.bit.CLMEN    = 0U;
    dmaBase->CTRL.bit.MINLMEN  = 0U;
    dmaBase->CTRL.bit.TXCNLERR = 0U;
    dmaBase->CTRL.bit.TXCNL    = 0U;
    for (index = 0; index < FEATURE_DMA_CHNS; index++)
    {
        DMA_HW_TCDClearReg(dmaBase, index);
    }
}

/*!
 * @brief Configures the transfer attribute for the DMA channel.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param srcModulo enumeration type for an allowed source modulo.
 * @param destModulo Enum type for an allowed destination modulo.
 * @param srcTransferSize Enum type for source transfer size.
 * @param destTransferSize Enum type for destination transfer size.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigAttribute(DMA_T *dmaBase,
                               uint8_t channel,
                               DMA_MODULO_T srcModulo,
                               DMA_MODULO_T destModulo,
                               DMA_TRANSFER_SIZE_T srcTransferSize,
                               DMA_TRANSFER_SIZE_T destTransferSize)
{
    dmaBase->TCD[channel].TATT.bit.SADDRMV = (uint16_t)srcModulo;
    dmaBase->TCD[channel].TATT.bit.STSIZE = (uint16_t)srcTransferSize;
    dmaBase->TCD[channel].TATT.bit.DADDRMV = (uint16_t)destModulo;
    dmaBase->TCD[channel].TATT.bit.DTSIZE = (uint16_t)destTransferSize;
}

/*!
 * @brief Configures the nbytes for the DMA channel.
 *
 * Note here that user need firstly configure the minor loop mapping feature and then call this
 * function.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param nbytes Number of bytes to be transferred in each service req of the channel.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigNbytes(DMA_T *dmaBase, uint8_t channel, uint32_t nbytes)
{
    if (dmaBase->CTRL.bit.MINLMEN != 0U)
    {
        bool mlOffNo = false;
        if (dmaBase->TCD[channel].MINBTCNT.SMLOF_MINLMENOFD.bit.SMINLOFEN == 0U)
        {
            if (dmaBase->TCD[channel].MINBTCNT.SMLOF_MINLMENOFD.bit.DMINLOFEN == 0U)
            {
                dmaBase->TCD[channel].MINBTCNT.SMLOF_MINLMENOFD.bit.MINBTCNT = nbytes;
                mlOffNo = true;
            }
        }
        if (!mlOffNo)
        {
            dmaBase->TCD[channel].MINBTCNT.SMINLOF_MINLMOFEN.bit.MINBTCNT = nbytes;
        }
    }
    else
    {
        dmaBase->TCD[channel].MINBTCNT.MINBCNT_MINLMD.reg = nbytes;
    }
}

/*!
 * @brief Configures the minor loop offset for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param offset Minor loop offset.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigMinorLoopOffset(DMA_T *dmaBase, uint8_t channel, int32_t offset)
{
    if (dmaBase->CTRL.bit.MINLMEN != 0U)
    {
        bool mlIsOffNo = false;
        if (dmaBase->TCD[channel].MINBTCNT.SMLOF_MINLMENOFD.bit.SMINLOFEN != 0U)
        {
            mlIsOffNo = true;
        }
        if (dmaBase->TCD[channel].MINBTCNT.SMLOF_MINLMENOFD.bit.DMINLOFEN != 0U)
        {
            mlIsOffNo = true;
        }
        if (mlIsOffNo)
        {
            dmaBase->TCD[channel].MINBTCNT.SMINLOF_MINLMOFEN.bit.MINSELOF = offset;
        }
    }
}

/*!
 * @brief Configures the memory address for the next transfer TCD for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param nextTCDAddr The address of the next TCD to be linked to this TCD.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigScatterGatherLink(DMA_T *dmaBase, uint8_t channel, uint32_t nextTCDAddr)
{
    dmaBase->TCD[channel].DADDRAVSGA.reg = nextTCDAddr;
}

/*!
 * @brief Sets the channel minor link for the TCD.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param linkChannel Channel to be linked on minor loop complete.
 * @param enable Enable (true)/Disable (false) channel minor link.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigChannelMinorLink(DMA_T *dmaBase, uint8_t channel, uint32_t linkChannel, bool enable)
{
    dmaBase->TCD[channel].MAJICNTSTAT.LINKEN.bit.LINKEN = (uint16_t)(enable ? \
        DMA_TCD_MAJICNTSTAT_LINKEN_DMA_LINKEN_1 : DMA_TCD_MAJICNTSTAT_LINKEN_DMA_LINKEN_0);
    dmaBase->TCD[channel].CMAJICNT.LINKEN.bit.LINKEN = (uint16_t)(enable ? \
        DMA_TCD_CMAJICNT_LINKEN_DMA_LINKEN_1 : DMA_TCD_CMAJICNT_LINKEN_DMA_LINKEN_0);

    if (enable)
    {
        dmaBase->TCD[channel].MAJICNTSTAT.LINKEN.bit.MINLINKCH = (uint16_t)linkChannel;
        dmaBase->TCD[channel].CMAJICNT.LINKEN.bit.MINLINKCH = (uint16_t)linkChannel;
    }
}

/*!
 * @brief Sets the major iteration count according to minor loop channel link setting.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 * @param count major loop count.
 *
 * @retval None.
 */
void DMA_HW_TCDConfigMajorCount(DMA_T *dmaBase, uint8_t channel, uint32_t count)
{
    if (dmaBase->TCD[channel].MAJICNTSTAT.LINKD.bit.LINKEN == DMA_TCD_MAJICNTSTAT_LINKD_DMA_LINKEN_1)
    {
        dmaBase->TCD[channel].MAJICNTSTAT.LINKEN.bit.MAJICSTAT = (uint16_t)count;
        dmaBase->TCD[channel].CMAJICNT.LINKEN.bit.CMAJICNT = (uint16_t)count;
    }
    else
    {
        dmaBase->TCD[channel].MAJICNTSTAT.LINKD.bit.MAJICSTAT = (uint16_t)count;
        dmaBase->TCD[channel].CMAJICNT.LINKD.bit.CMAJICNT = (uint16_t)count;
    }
}

/*!
 * @brief Returns the current major iteration count.
 *
 * @param dmaBase Register base address for DMA module.
 * @param channel DMA channel number.
 *
 * @retval current iteration count.
 */
uint32_t DMA_HW_TCDReadCurrentMajorCount(const DMA_T *dmaBase, uint8_t channel)
{
    uint16_t res = 0U;
    if (dmaBase->TCD[channel].MAJICNTSTAT.LINKD.bit.LINKEN == DMA_TCD_MAJICNTSTAT_LINKD_DMA_LINKEN_1)
    {
        res = dmaBase->TCD[channel].CMAJICNT.LINKEN.bit.CMAJICNT;
    }
    else
    {
        res = dmaBase->TCD[channel].CMAJICNT.LINKD.bit.CMAJICNT;
    }
    return (uint32_t) res;
}

/*!
 * @brief Initializes the DMAMUX module to the reset state.
 *
 * @param dmaBase Register base address for DMAMUX module.
 *
 * @retval None.
 */
void DMAMUX_HW_ClearReg(DMACHM_T *dmaBase)
{
    uint8_t index;

    for (index = 0; index < FEATURE_DMACHM_CHANNELS; index++)
    {
        dmaBase->CFG[index].reg = (uint8_t)0U;
    }
}

/**@} end of group DMA_Functions*/
/**@} end of group DMA_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
