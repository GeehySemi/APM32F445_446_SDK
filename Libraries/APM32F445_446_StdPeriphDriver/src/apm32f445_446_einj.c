/*!
 * @file        apm32f445_446_einj.c
 *
 * @brief       This file provides all the EINJ firmware functions
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
#include "apm32f445_446_einj.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup EINJ_Driver EINJ Driver
  @{
*/

/** @defgroup EINJ_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
/* @brief Table of base addresses for EINJ instances */
#define EINJ_CHANNEL_COUNT          (2U)
/* @brief The position of the most significant bit in Error Injection Channel Enable register */
#define POS_MSB_EINJ_EICHANNEL      (31U)

/**@} end of group EINJ_Macros*/

/** @defgroup EINJ_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

static EINJ_T *const g_einjBasePointer[] = { EINJ };

/**@} end of group EINJ_Variables*/

/** @defgroup EINJ_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initializes the EINJ module.
 *
 * @param ins: EINJ module ins number.
 * @param channelCnt: Number of configured channels
 * @param channelCfgArr: EINJ channel configuration structure array
 *
 * @retval None
 */
void EINJ_Init(uint32_t ins, uint8_t channelCnt, const EINJ_USER_CHANNEL_CONFIG_T *channelCfgArr)
{
    EINJ_INSTANCE_VALIDITY(ins);

    EINJ_T *einjBase = g_einjBasePointer[ins];
    uint8_t idx;

    for (idx = 0U; idx < channelCnt; idx++)
    {
        /* Configures for EINJ module */
        EINJ_ChannelConfig(ins, &channelCfgArr[idx]);
    }

    /* Enables EINJ module */
    EINJ_HW_Enable(einjBase);
}

/*!
 * @brief Gets the EINJ channel configuration default.
 *
 * @param channel: EINJ channel number
 * @param channelCfg: Pointer to EINJ channel configuration structure default
 *
 * @retval None
 */
void EINJ_DefaultConfig(uint8_t channel, EINJ_USER_CHANNEL_CONFIG_T *channelCfg)
{
    /* Read the channel number */
    channelCfg->channel = channel;
    /* Read check-bit mask default of EINJ channel */
    channelCfg->checkBitMask = EINJ_CHECKBITMASK_DEFAULT;
    /* Read data mask default of EINJ channel */
    channelCfg->dataMask = EINJ_DATAMASK_DEFAULT;
    /* Enable EINJ channel */
    channelCfg->enable = true;
}

/*!
 * @brief De-initializes the EINJ module.
 *
 * @param ins: EINJ module ins number
 *
 * @retval None
 */
void EINJ_DeInit(uint32_t ins)
{
    EINJ_INSTANCE_VALIDITY(ins);

    EINJ_T *einjBase = g_einjBasePointer[ins];
    /* Disables EINJ module */
    EINJ_HW_Disable(einjBase);
    /* Resets EINJ descriptors and disables EINJ channels */
    EINJ_HW_ChannelSetup(einjBase);
}

/*!
 * @brief Configures the EINJ channel.
 *
 * @param ins: EINJ module ins number
 * @param userChannelCfg: Pointer to EINJ channel configuration structure
 *
 * @retval None
 */
void EINJ_ChannelConfig(uint32_t ins, const EINJ_USER_CHANNEL_CONFIG_T *userChannelCfg)
{
    EINJ_INSTANCE_VALIDITY(ins);

    EINJ_T *einjBase = g_einjBasePointer[ins];

    /* Disables EINJ channel to configure EINJ channel */
    EINJ_HW_ChannelCmd(einjBase, userChannelCfg->channel, false);
    /* Configures check-bit mask for EINJ channel */
    EINJ_HW_AssignCheckBit(einjBase, userChannelCfg->channel, userChannelCfg->checkBitMask);
    /* Configures data mask for EINJ channel */
    EINJ_HW_AssignData(einjBase, userChannelCfg->channel, userChannelCfg->dataMask);
    /* Enables or disables EINJ channel operation corresponding to channel configuration */
    EINJ_HW_ChannelCmd(einjBase, userChannelCfg->channel, userChannelCfg->enable);
}

/*!
 * @brief Gets the EINJ channel configuration.
 *
 * @param ins: EINJ module ins number
 * @param channel: EINJ channel number
 * @param channelCfg: Pointer to EINJ channel configuration structure
 *
 * @retval None
 */
void EINJ_ReadChannelConfig(uint32_t ins, uint8_t channel, EINJ_USER_CHANNEL_CONFIG_T *channelCfg)
{
    EINJ_INSTANCE_VALIDITY(ins);

    const EINJ_T *einjBase = g_einjBasePointer[ins];

    /* Read the channel number */
    channelCfg->channel = channel;
    /* Read check-bit mask of EINJ channel */
    channelCfg->checkBitMask = EINJ_HW_ReadCheckBit(einjBase, channel);
    /* Read data mask of EINJ channel */
    channelCfg->dataMask = EINJ_HW_ReadData(einjBase, channel);
    /* Read operation status */
    channelCfg->enable = EINJ_HW_IsChannelEnabled(einjBase, channel);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enables EINJ module.
 *
 * @param einjBase: EINJ peripheral base address.
 *
 * @retval None
 */
void EINJ_HW_Enable(EINJ_T *const einjBase)
{
    einjBase->CFG.reg |= 0x1U;
}

/*!
 * @brief Disables the EINJ module.
 *
 * @param einjBase: EINJ peripheral base address
 *
 * @retval None
 */
void EINJ_HW_Disable(EINJ_T *const einjBase)
{
    einjBase->CFG.reg &= ~0x1U;
}

/*!
 * @brief Resets for the registers of EINJ descriptor.
 *
 * @param einjBase: EINJ peripheral base address
 *
 * @retval None
 */
void EINJ_HW_ChannelSetup(EINJ_T *const einjBase)
{
    uint8_t channelnum;

    for(channelnum = 0; channelnum < EINJ_CHANNEL_COUNT; channelnum++)
    {
        EINJ_HW_ChannelCmd(einjBase, channelnum, false);
        EINJ_HW_AssignCheckBit(einjBase, channelnum, 0U);
        EINJ_HW_AssignData(einjBase, channelnum, 0U);
    }
}

/*!
 * @brief Enables or disable EINJ channel operation.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 *
 * @retval None
 */
void EINJ_HW_ChannelCmd(EINJ_T *const einjBase, uint8_t channel, bool enable)
{
    uint32_t tempor = einjBase->CHEN.reg;
    tempor &= ~(1UL << (POS_MSB_EINJ_EICHANNEL - channel));
    tempor |= (enable ? 1UL : 0UL) << (POS_MSB_EINJ_EICHANNEL - channel);
    einjBase->CHEN.reg = tempor;
}

/*!
 * @brief Checks whether EINJ channel is enabled or disabled.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 *
 * @retval EINJ channel operation status:
 *         - true: EINJ channel is enabled
 *         - false: EINJ channel is disabled
 */
bool EINJ_HW_IsChannelEnabled(const EINJ_T *const einjBase, uint8_t channel)
{
    return ((einjBase->CHEN.reg & (1UL << (channel ? 30U : 31U))) != 0UL);
}

/*!
 * @brief Assign check bit mask for EINJ channel.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 * @param checkBitMask: Check-bit mask
 *
 * @retval None
 */
void EINJ_HW_AssignCheckBit(EINJ_T *const einjBase, uint8_t channel, uint8_t checkBitMask)
{
    if(channel == 0)
    {
        einjBase->CHD0_WORD0.reg = (uint32_t)(((uint32_t)(checkBitMask) << 25U) & 0xFE000000U);
    }
    else if(channel == 1)
    {
        einjBase->CHD1_WORD0.reg = (uint32_t)(((uint32_t)(checkBitMask) << 25U) & 0xFE000000U);
    }
}

/*!
 * @brief Read check bit mask of EINJ channel.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 *
 * @retval Check-bit mask
 */
uint32_t EINJ_HW_ReadCheckBit(const EINJ_T *const einjBase, uint8_t channel)
{
    return (channel ? (einjBase->CHD1_WORD0.reg >> 25U) : (einjBase->CHD0_WORD0.reg >> 25U));
}

/*!
 * @brief Assign data mask for EINJ channel.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 * @param dataMask: Data mask
 *
 * @retval None
 */
void EINJ_HW_AssignData(EINJ_T *const einjBase, uint8_t channel, uint32_t dataMask)
{
    if(channel == 0)
    {
        einjBase->CHD0_WORD1.reg = dataMask;
    }
    else if(channel == 1)
    {
        einjBase->CHD1_WORD1.reg = dataMask;
    }
}

/*!
 * @brief Read data mask of EINJ channel.
 *
 * @param einjBase: EINJ peripheral base address
 * @param channel: EINJ channel number
 *
 * @retval Data mask
 */
uint32_t EINJ_HW_ReadData(const EINJ_T *const einjBase, uint8_t channel)
{
    return (channel ? (einjBase->CHD1_WORD1.reg) : (einjBase->CHD0_WORD1.reg));
}

/**@} end of group EINJ_Functions*/
/**@} end of group EINJ_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
