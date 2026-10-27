/*!
 * @file        apm32f445_446_dma.h
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

#ifndef APM32F445_446_DMA_H
#define APM32F445_446_DMA_H

#if defined(__cplusplus)
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"
#include "apm_core_cm4.h"
#include "stddef.h"

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

/*!
 * @brief Macro for the memory size needed for the software TCD.
 */
#define STCD_SIZE(number)       (((number) * 32U) - 1U)
#define STCD_ADDR(address)      (((uint32_t)address + 31UL) & ~0x1FUL)

/*!
 * @brief Macro for accessing the least significant bit of the ERR register.
 */
#define DMA_ERR_LSB_MASK        1U

/**@} end of group DMA_Macros*/

/** @defgroup DMA_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

#ifdef FEATURE_DMA_ENGINE_STALL
/*!
 * @brief Specifies the number of cycles the DMA Engine is stalled.
 */
typedef enum
{
    DMA_ENGINE_STALL_8_CYCLES = 3,
    DMA_ENGINE_STALL_4_CYCLES = 2,
    DMA_ENGINE_STALL_0_CYCLES = 0
} DMA_ENGINE_STALL_T;
#endif /* FEATURE_DMA_ENGINE_STALL */

/*!
 * @brief DMA channel interrupts.
 */
typedef enum
{
    DMA_CHANNEL_ERR_INT = 0U,         /* Error interrupt */
    DMA_CHANNEL_HALF_MAJOR_LOOP_INT,  /* Half major loop interrupt */
    DMA_CHANNEL_MAJOR_LOOP_INT        /* Complete major loop interrupt */
} DMA_CHANNEL_INTERRUPT_T;

/*!
 * @brief DMA channel arbitration algorithm used for selection among channels.
 */
typedef enum
{
    DMA_ARBITRATION_ROUND_ROBIN    = 1U,   /* Round-Robin arbitration */
    DMA_ARBITRATION_FIXED_PRIORITY = 0U    /* Fixed Priority */
} DMA_ARBITRATION_ALGORITHM_T;

/*!
 * @brief DMA channel priority setting
 */
typedef enum
{
    DMA_CHANNEL_PRIORITY_0 = 0U,
    DMA_CHANNEL_PRIORITY_1 = 1U,
    DMA_CHANNEL_PRIORITY_2 = 2U,
    DMA_CHANNEL_PRIORITY_3 = 3U,
#ifndef FEATURE_DMA_4_CH_PRIORITIES
    DMA_CHANNEL_PRIORITY_4 = 4U,
    DMA_CHANNEL_PRIORITY_5 = 5U,
    DMA_CHANNEL_PRIORITY_6 = 6U,
    DMA_CHANNEL_PRIORITY_7 = 7U,
#ifndef FEATURE_DMA_8_CH_PRIORITIES
    DMA_CHANNEL_PRIORITY_8 = 8U,
    DMA_CHANNEL_PRIORITY_9 = 9U,
    DMA_CHANNEL_PRIORITY_10 = 10U,
    DMA_CHANNEL_PRIORITY_11 = 11U,
    DMA_CHANNEL_PRIORITY_12 = 12U,
    DMA_CHANNEL_PRIORITY_13 = 13U,
    DMA_CHANNEL_PRIORITY_14 = 14U,
    DMA_CHANNEL_PRIORITY_15 = 15U,
#endif /* FEATURE_DMA_8_CH_PRIORITIES */
#endif /* FEATURE_DMA_4_CH_PRIORITIES */
    DMA_CHANNEL_DEFAULT_PRIORITY = 255U
} DMA_CHANNEL_PRIORITY_T;

/*!
 * @brief DMA modulo configuration
 */
typedef enum
{
    DMA_MODULO_2GB    = 31U,
    DMA_MODULO_1GB    = 30U,
    DMA_MODULO_512MB  = 29U,
    DMA_MODULO_256MB  = 28U,
    DMA_MODULO_128MB  = 27U,
    DMA_MODULO_64MB   = 26U,
    DMA_MODULO_32MB   = 25U,
    DMA_MODULO_16MB   = 24U,
    DMA_MODULO_8MB    = 23U,
    DMA_MODULO_4MB    = 22U,
    DMA_MODULO_2MB    = 21U,
    DMA_MODULO_1MB    = 20U,
    DMA_MODULO_512KB  = 19U,
    DMA_MODULO_256KB  = 18U,
    DMA_MODULO_128KB  = 17U,
    DMA_MODULO_64KB   = 16U,
    DMA_MODULO_32KB   = 15U,
    DMA_MODULO_16KB   = 14U,
    DMA_MODULO_8KB    = 13U,
    DMA_MODULO_4KB    = 12U,
    DMA_MODULO_2KB    = 11U,
    DMA_MODULO_1KB    = 10U,
    DMA_MODULO_512B   = 9U,
    DMA_MODULO_256B   = 8U,
    DMA_MODULO_128B   = 7U,
    DMA_MODULO_64B    = 6U,
    DMA_MODULO_32B    = 5U,
    DMA_MODULO_16B    = 4U,
    DMA_MODULO_8B     = 3U,
    DMA_MODULO_4B     = 2U,
    DMA_MODULO_2B     = 1U,
    DMA_MODULO_OFF    = 0U
} DMA_MODULO_T;

/*!
 * @brief DMA transfer configuration
 */
typedef enum
{
#ifdef FEATURE_DMA_TRANSFER_SIZE_64B
    DMA_TRANSFER_SIZE_64B = 0x6U,
#endif /* FEATURE_DMA_TRANSFER_SIZE_64B */

#ifdef FEATURE_DMA_TRANSFER_SIZE_32B
    DMA_TRANSFER_SIZE_32B = 0x5U,
#endif /* FEATURE_DMA_TRANSFER_SIZE_32B */

#ifdef FEATURE_DMA_TRANSFER_SIZE_16B
    DMA_TRANSFER_SIZE_16B = 0x4U,
#endif /* FEATURE_DMA_TRANSFER_SIZE_16B */

#ifdef FEATURE_DMA_TRANSFER_SIZE_8B
    DMA_TRANSFER_SIZE_8B  = 0x3U,
#endif /* FEATURE_DMA_TRANSFER_SIZE_8B */

    DMA_TRANSFER_SIZE_4B  = 0x2U,
    DMA_TRANSFER_SIZE_2B  = 0x1U,
    DMA_TRANSFER_SIZE_1B  = 0x0U
} DMA_TRANSFER_SIZE_T;

/*!
 * @brief Channel status for DMA channel.
 */
typedef enum
{
    DMA_CHANNEL_NORMAL = 0U,        /* DMA channel normal state */
    DMA_CHANNEL_ERROR               /* An error occurred in the DMA channel */
} DMA_CHANNEL_STATUS_T;

/*!
 * @brief A type for the DMA transfer.
 */
typedef enum
{
    DMA_TRANSFER_PERIPH2PERIPH = 3U,        /* Transfer from peripheral to peripheral */
    DMA_TRANSFER_MEM2MEM       = 2U,        /* Transfer from memory to memory */
    DMA_TRANSFER_MEM2PERIPH    = 1U,        /* Transfer from memory to peripheral */
    DMA_TRANSFER_PERIPH2MEM    = 0U         /* Transfer from peripheral to memory */
} DMA_TRANSFER_TYPE_T;

/**@} end of group DMA_Enumerations*/

/** @defgroup DMA_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief The user configuration structure for the DMA driver.
 */
typedef struct
{
    bool haltOnError;                               /* Any error causes the HALT bit to set. Subsequently, all
                                                       service requests are ignored until the HALT bit is cleared. */
    DMA_ARBITRATION_ALGORITHM_T chnArbitration;     /* DMA channel arbitration. */
} DMA_USER_CONFIG_T;

/*!
 * @brief Definition for the DMA channel callback function.
 */
typedef void (*DMA_CALLBACK_T)(void *parameter, DMA_CHANNEL_STATUS_T status);

/*!
 * @brief Data structure for the DMA channel state.
 */
typedef struct
{
    uint8_t vtChnNum;                    /* Virtual channel number. */
    volatile DMA_CHANNEL_STATUS_T status;/* DMA channel status. */
    void *parameter;                     /* Parameter for the callback function pointer. */
    DMA_CALLBACK_T callback;             /* Callback function pointer for the DMA channel.
                                        It will be called at the DMA channel complete
                                        and DMA channel error. */
} DMA_CHN_STATE_T;

/*!
 * @brief The user configuration structure for the an DMA driver channel.
 */
typedef struct
{
    uint8_t vtChannel;                       /* DMA virtual channel number */
    DMA_REQUEST_SOURCE_T source;             /* Selects the source of the DMA request for this channel */

    DMA_CHANNEL_PRIORITY_T channelPriority;  /* DMA channel priority - only used when channel
                                                arbitration mode is 'Fixed priority' */
    bool enableTrigger;                      /* Enables the periodic trigger capability for the DMA channel */
    DMA_CALLBACK_T callback;                 /* Callback that will be registered for this channel */
    void *callbackParam;                     /* Parameter passed to the channel callback */
} DMA_CHANNEL_CONFIG_T;

/*!
 * @brief Data structure for configuring a discrete memory transfer.
 */
typedef struct
{
    uint32_t length;            /* Length of buffer */
    uint32_t address;           /* Address of buffer */
    DMA_TRANSFER_TYPE_T type;   /* Type of the DMA transfer */
} DMA_SCATTER_GATHER_LIST_T;

/*!
 * @brief Runtime state structure for the DMA driver.
 */
typedef struct
{
    DMA_CHN_STATE_T *volatile vtChnState[(uint32_t)FEATURE_DMA_VIRTUAL_CHANNELS];   /* Pointer array storing channel state. */
} DMA_STATE_T;

/*!
 * @brief DMA loop transfer configuration.
 */
typedef struct
{
    bool srcOffsetEn;                       /* Selects whether the minor loop offset is applied to the
                                               source address upon minor loop completion. */
    bool destOffsetEn;                      /* Selects whether the minor loop offset is applied to the
                                               destination address upon minor loop completion. */
    uint32_t numOfMajorLoopIteration;       /* Number of major loop iterations. */
    bool majorLoopChannelLinkEn;            /* Enables channel-to-channel linking on major loop complete. */
    uint8_t majorLoopChannelLinkNum;        /* The number of the next channel to be started by DMA
                                               engine when major loop completes. */
    int32_t minorLoopOffset;                /* Sign-extended offset applied to the source or destination address
                                               to form the next-state value after the minor loop completes. */
    bool minorLoopChannelLinkEn;            /* Enables channel-to-channel linking on minor loop complete. */
    uint8_t minorLoopChannelLinkNum;        /* The number of the next channel to be started by DMA
                                               engine when minor loop completes. */

} DMA_LOOP_TRANSFER_CONFIG_T;

/*!
 * @brief DMA transfer size configuration.
 */
typedef struct
{
    uint32_t srcAddr;                                 /* Memory address pointing to the source data. */
    uint32_t destAddr;                                /* Memory address pointing to the destination data. */
    int32_t lastSrcAddrAdjust;                        /* Last source address adjustment. */
    int32_t lastDestAddrAdjust;                       /* Last destination address adjustment. Note here it is only
                                                         valid when scatter/gather feature is not enabled. */
    int16_t srcOffset;                                /* Sign-extended offset applied to the current source address
                                                         to form the next-state value as each source read/write
                                                         is completed. */
    int16_t destOffset;                               /* Sign-extended offset applied to the current destination
                                                         address to form the next-state value as each source
                                                         read/write is completed. */
    DMA_TRANSFER_SIZE_T srcTransferSize;              /* Source data transfer size. */
    DMA_TRANSFER_SIZE_T destTransferSize;             /* Destination data transfer size. */
    DMA_MODULO_T srcModulo;                           /* Source address modulo. */
    DMA_MODULO_T destModulo;                          /* Destination address modulo. */
    uint32_t minorByteTransferNum;                    /* Number of bytes to be transferred in each service request
                                                         of the channel. */
    bool scatterGatherEn;                             /* Enable scatter gather feature. */
    bool interruptEn;                                 /* Enable the interrupt request when the major loop
                                                         count completes */
    uint32_t scatterGatherNextAddrOfDescriptor;       /* The address of the next descriptor to be used, when
                                                         scatter/gather feature is enabled.
                                                         Note: this value is not used when scatter/gather
                                                               feature is disabled. */
    DMA_LOOP_TRANSFER_CONFIG_T *loopTransferCfg;      /* Pointer to loop transfer configuration structure
                                                         (defines minor/major loop attributes)
                                                         Note: this field is only used when minor loop mapping is
                                                               enabled from DMA configuration. */
} DMA_TRANSFER_CONFIG_T;

/*!
 * @brief DMA TCD
 */
typedef struct
{
    uint32_t SADDR;
    int16_t SOFF;
    uint16_t ATTR;
    uint32_t NBYTES;
    int32_t SLAST;
    uint32_t DADDR;
    int16_t DOFF;
    uint16_t CITER;
    int32_t DLAST_SGA;
    uint16_t CSR;
    uint16_t BITER;
} DMA_SOFTWARE_TCD_T;

/**@} end of group DMA_Structures*/

/** @defgroup DMA_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T DMA_ConfigChannelRequestAndTrigger(uint8_t vtChannel, uint8_t request, bool enableTrigger);
STATUS_T DMA_RegisterCallback(uint8_t vtChannel, DMA_CALLBACK_T callback, void *parameter);
STATUS_T DMA_ConfigChannel(DMA_CHN_STATE_T *dmaChannelStatus, const DMA_CHANNEL_CONFIG_T *dmaChannelCfg);

STATUS_T DMA_Init(DMA_STATE_T *dmaState,
                  const DMA_USER_CONFIG_T *userConfig,
                  DMA_CHN_STATE_T * const chnStateArray[],
                  const DMA_CHANNEL_CONFIG_T * const chnConfigArray[],
                  uint32_t chnCount);

STATUS_T DMA_ReleaseChannel(uint8_t vtChannel);
STATUS_T DMA_Reset(void);
void DMA_ClearIntStatus(uint8_t vtChannel);
void DMA_ClearSoftwareTCD(DMA_SOFTWARE_TCD_T *stcd);
void DMA_IRQHandler(uint8_t vtChannel);
void DMA_ErrorIRQHandler(uint8_t vtChannel);

STATUS_T DMA_ConfigSingleBlockTransfer(uint8_t vtChannel,
                                       DMA_TRANSFER_TYPE_T type,
                                       uint32_t srcAddr,
                                       uint32_t destAddr,
                                       DMA_TRANSFER_SIZE_T transferSize,
                                       uint32_t dataBufferSize);

STATUS_T DMA_ConfigMultiBlockTransfer(uint8_t vtChannel,
                                      DMA_TRANSFER_TYPE_T type,
                                      uint32_t srcAddr,
                                      uint32_t destAddr,
                                      DMA_TRANSFER_SIZE_T transferSize,
                                      uint32_t blockSize,
                                      uint32_t blockCount,
                                      bool disableReqOnCompletion);

void DMA_CopyConfigurationToSTCD(const DMA_TRANSFER_CONFIG_T *cfg, DMA_SOFTWARE_TCD_T *stcdcfg);
void DMA_CopyConfigurationToTCDReg(uint8_t vtChannel, const DMA_TRANSFER_CONFIG_T *tcdcfg);
STATUS_T DMA_ConfigLoopTransfer(uint8_t vtChannel, const DMA_TRANSFER_CONFIG_T *transferCfg);

STATUS_T DMA_ConfigScatterGatherTransfer(uint8_t vtChannel,
                                         DMA_SOFTWARE_TCD_T *stcd,
                                         DMA_TRANSFER_SIZE_T transferSize,
                                         uint32_t bytesOnEachRequest,
                                         const DMA_SCATTER_GATHER_LIST_T *srcList,
                                         const DMA_SCATTER_GATHER_LIST_T *destList,
                                         uint8_t tcdCount);

STATUS_T DMA_StartChannel(uint8_t vtChannel);
STATUS_T DMA_StopChannel(uint8_t vtChannel);
void DMA_ClearTCD(uint8_t vtChannel);
void DMA_ConfigSrcAddr(uint8_t vtChannel, uint32_t address);
void DMA_ConfigSrcOffset(uint8_t vtChannel, int16_t offset);
void DMA_ConfigSrcReadChunkSize(uint8_t vtChannel, DMA_TRANSFER_SIZE_T size);
void DMA_ConfigSrcLastAddrAdjustment(uint8_t vtChannel, int32_t adjust);
void DMA_ConfigDestLastAddrAdjustment(uint8_t vtChannel, int32_t adjust);
void DMA_ConfigDestAddr(uint8_t vtChannel, uint32_t address);
void DMA_ConfigDestOffset(uint8_t vtChannel, int16_t offset);
void DMA_ConfigDestWriteChunkSize(uint8_t vtChannel, DMA_TRANSFER_SIZE_T size);
void DMA_ConfigMinorLoopBlockSize(uint8_t vtChannel, uint32_t nbytes);
void DMA_ConfigMajorLoopIterationCount(uint8_t vtChannel, uint32_t majorLoopCount);
uint32_t DMA_ReadRemainingMajorIterationsCount(uint8_t vtChannel);
void DMA_ConfigScatterGatherLink(uint8_t vtChannel, uint32_t nextTCDAddr);
void DMA_DisableRequestsOnTransferComplete(uint8_t vtChannel, bool disable);
void DMA_ConfigInterrupt(uint8_t vtChannel, DMA_CHANNEL_INTERRUPT_T intSrc, bool enable);
void DMA_CancelTransfer(bool error);
void DMA_TriggerSwRequest(uint8_t vtChannel);
DMA_CHANNEL_STATUS_T DMA_ReadChannelStatus(uint8_t vtChannel);
DMA_T *DMA_ReadDmaRegBaseAddr(uint32_t instance);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void DMA_HW_ConfigHaltOnErrorCmd(DMA_T *dmaBase, bool haltOnError);
void DMA_HW_ConfigChannelPriority(DMA_T *dmaBase, uint8_t channel, DMA_CHANNEL_PRIORITY_T priority);
void DMA_HW_ConfigChannelArbitrationMode(DMA_T *dmaBase, DMA_ARBITRATION_ALGORITHM_T channelArbitration);
void DMA_HW_ConfigMinorLoopMappingCmd(DMA_T *dmaBase, bool enable);

DMA_ARBITRATION_ALGORITHM_T DMA_HW_ReadChannelArbitrationMode(const DMA_T *dmaBase);
uint32_t DMA_HW_ReadErrorIntStatusFlag(const DMA_T *dmaBase);

void DMA_HW_ClearErrorIntStatusFlag(DMA_T *dmaBase, uint8_t channel);
void DMA_HW_ClearDoneStatusFlag(DMA_T *dmaBase, uint8_t channel);
void DMA_HW_TriggerChannelStart(DMA_T *dmaBase, uint8_t channel);
void DMA_HW_ClearIntStatusFlag(DMA_T *dmaBase, uint8_t channel);

#ifdef FEATURE_DMA_ENGINE_STALL
void DMA_HW_TCDCOnfigEngineStall(DMA_T *dmaBase, uint8_t channel, DMA_ENGINE_STALL_T cycles);
#endif /* FEATURE_DMA_ENGINE_STALL */

void DMA_HW_TCDConfigSrcAddr(DMA_T *dmaBase, uint8_t channel, uint32_t address);
void DMA_HW_TCDConfigSrcOffset(DMA_T *dmaBase, uint8_t channel, int16_t offset);
void DMA_HW_TCDConfigSrcTransferSize(DMA_T *dmaBase, uint8_t channel, DMA_TRANSFER_SIZE_T size);
void DMA_HW_TCDConfigDestTransferSize(DMA_T *dmaBase, uint8_t channel, DMA_TRANSFER_SIZE_T size);
void DMA_HW_TCDConfigSrcMinorLoopOffsetCmd(DMA_T *dmaBase, uint8_t channel, bool enable);
void DMA_HW_TCDConfigDestMinorLoopOffsetCmd(DMA_T *dmaBase, uint8_t channel, bool enable);
void DMA_HW_TCDConfigSrcLastAdjust(DMA_T *dmaBase, uint8_t channel, int32_t size);
void DMA_HW_TCDConfigDestAddr(DMA_T *dmaBase, uint8_t channel, uint32_t address);
void DMA_HW_TCDConfigDestOffset(DMA_T *dmaBase, uint8_t channel, int16_t offset);
void DMA_HW_TCDConfigDestLastAdjust(DMA_T *dmaBase, uint8_t channel, int32_t adjust);
void DMA_HW_TCDConfigScatterGatherCmd(DMA_T *dmaBase, uint8_t channel, bool enable);
void DMA_HW_TCDConfigChannelMajorLink(DMA_T *dmaBase, uint8_t channel, uint32_t majorLinkChannel, bool enable);
void DMA_HW_TCDConfigDisableDmaRequestAfterTCDDoneCmd(DMA_T *dmaBase, uint8_t channel, bool disable);
void DMA_HW_TCDConfigMajorHalfCompleteIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable);
void DMA_HW_TCDConfigMajorCompleteIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable);

void DMACHM_HW_ConfigChannelCmd(DMACHM_T *dmaBase, uint8_t channel, bool enable);
void DMACHM_HW_ConfigChannelTrigger(DMACHM_T *dmaBase, uint8_t channel, bool enable);
void DMACHM_HW_ConfigChannelSource(DMACHM_T *dmaBase, uint8_t channel, uint8_t source);

void DMA_HW_DoCancelTransfer(DMA_T *dmaBase);
void DMA_HW_CancelTransferWithError(DMA_T *dmaBase);
void DMA_HW_ConfigErrorIntCmd(DMA_T *dmaBase, uint8_t channel, bool enable);
void DMA_HW_ConfigDmaRequestCmd(DMA_T *dmaBase, uint8_t channel,bool enable);
void DMA_HW_TCDClearReg(DMA_T *dmaBase, uint8_t channel);
void DMA_HW_ClearReg(DMA_T *dmaBase);
void DMA_HW_TCDConfigAttribute(DMA_T *dmaBase,
                               uint8_t channel,
                               DMA_MODULO_T srcModulo,
                               DMA_MODULO_T destModulo,
                               DMA_TRANSFER_SIZE_T srcTransferSize,
                               DMA_TRANSFER_SIZE_T destTransferSize);
void DMA_HW_TCDConfigNbytes(DMA_T *dmaBase, uint8_t channel, uint32_t nbytes);
void DMA_HW_TCDConfigMinorLoopOffset(DMA_T *dmaBase, uint8_t channel, int32_t offset);
void DMA_HW_TCDConfigScatterGatherLink(DMA_T *dmaBase, uint8_t channel, uint32_t nextTCDAddr);
void DMA_HW_TCDConfigChannelMinorLink(DMA_T *dmaBase, uint8_t channel, uint32_t linkChannel, bool enable);
void DMA_HW_TCDConfigMajorCount(DMA_T *dmaBase, uint8_t channel, uint32_t count);
uint32_t DMA_HW_TCDReadCurrentMajorCount(const DMA_T *dmaBase, uint8_t channel);
void DMAMUX_HW_ClearReg(DMACHM_T *dmaBase);

/**@} end of group DMA_Functions*/
/**@} end of group DMA_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#ifdef __cplusplus
}
#endif

#endif /* APM32F445_446_POWER_H */
