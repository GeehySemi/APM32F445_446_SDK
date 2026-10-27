/*!
 * @file        apm32f445_446_crc.c
 *
 * @brief       This file provides all the CRC firmware functions
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
#include "apm32f445_446_crc.h"


/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/
/** @addtogroup CRC_Driver CRC Driver
  @{
*/
/** @defgroup CRC_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/* Initial checksum */
#define CRC_INITIAL_SEED    (0U)

/**@} end of group CRC_Macros*/

/** @defgroup CRC_Variables Variables
  @{
*/

/*! @brief Table of base addresses for CRC instances. */
CRC_T * const g_crcBaseAddr[] = CRC_BASE_PTRS;

/**@} end of group CRC_Variables*/


/** @defgroup CRC_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

static STATUS_T CRC_Configure(uint32_t ins, const CRC_USER_CFG_T *userCfg);

/*******************************************************************************
 *                          PUBLIC FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize CRC driver
 * @details This function initializes CRC driver based on user configuration
 *          input. The user must make sure that the clock is enabled.
 *
 * @param ins Device ins No
 * @param userCfg Configuration structure of user
 * @retval CRC driver status
 */
STATUS_T CRC_Init(uint32_t ins, const CRC_USER_CFG_T *userCfg)
{
    STATUS_T result = STATUS_SUCCESS;
    CRC_T *baseAddr = g_crcBaseAddr[ins];

    CRC_HW_Init(baseAddr);
    result = CRC_Configure(ins, userCfg);
    return result;
}

/*!
 * @brief Sets the default configuration
 *
 * This function sets the default configuration
 *
 * @param ins The CRC instance number
 * @retval Execution status
 */
STATUS_T CRC_Deinit(uint32_t ins)
{
    CRC_T *baseAddr = g_crcBaseAddr[ins];
    CRC_HW_Init(baseAddr);
    return STATUS_SUCCESS;
}

/*!
 * @brief Returns the current result of the CRC calculation
 *
 * @param ins The CRC instance number
 * @retval Result of CRC calculation
 */
uint32_t CRC_GetCrcResult(uint32_t ins)
{
    const CRC_T *baseAddr = g_crcBaseAddr[ins];

    /* Result of the CRC calculation */
    return CRC_HW_GetCrcResult(baseAddr);
}

/*!
 * @brief Appends a block of bytes to the current CRC calculation
 *
 * @param ins The CRC instance number
 * @param data Data for current CRC calculation
 * @param dataSize Length of data  calculated
 */
void CRC_WriteData(uint32_t ins, const uint32_t *data, uint32_t dataSize)
{
    CRC_T *baseAddr = g_crcBaseAddr[ins];

    /* 32-bit writes until end of data buffer */
    for (uint32_t i = 0U; i < dataSize; i++)
    {
        baseAddr->DATA.reg = data[i];
    }
}

/*!
 * @brief Get default config of the CRC module
 *
 * @param userCfg Pointer to structure of initialization
 * @retval Execution status
 */
STATUS_T CRC_DefaultConfig(CRC_USER_CFG_T * const userCfg)
{
    userCfg->seed   = FEATURE_CRC_DEFAULT_SEED;
    userCfg->poly   = FEATURE_CRC_DEFAULT_POLY;
    userCfg->resultComplement   = false;
    userCfg->writeTransposeType = FEATURE_CRC_DEFAULT_WR_TRS;
    userCfg->readTransposeType  = FEATURE_CRC_DEFAULT_RD_TRS;
    /* CRC mode default is 16 bit */
    userCfg->crcWidth   = FEATURE_CRC_DEFAULT_WIDTH;

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the current config of the CRC module
 *
 * @param ins The CRC instance number
 * @param userCfg Pointer to structure of initialization
 * @retval Execution status
 */
STATUS_T CRC_GetConfig(uint32_t ins, CRC_USER_CFG_T * const userCfg)
{
    const CRC_T *const baseAddr = g_crcBaseAddr[ins];

    userCfg->seed   = CRC_HW_GetDataReg(baseAddr);
    userCfg->poly   = CRC_HW_GetPolyReg(baseAddr);
    userCfg->resultComplement   = CRC_HW_GetFXorMode(baseAddr);
    userCfg->writeTransposeType = CRC_HW_GetWriteTranspose(baseAddr);
    userCfg->readTransposeType  = CRC_HW_GetReadTranspose(baseAddr);
    userCfg->crcWidth   = CRC_HW_GetProtocolWidth(baseAddr);

    return STATUS_SUCCESS;
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initializes the CRC module
 *
 * @param ins The CRC ins number
 * @param userCfg Pointer to structure of initialization
 * @retval Execution status
 */
static STATUS_T CRC_Configure(uint32_t ins, const CRC_USER_CFG_T *userCfg)
{
    CRC_T *baseAddr = g_crcBaseAddr[ins];

    /* Write the seed, initial checksum */
    CRC_HW_SetSeedReg(baseAddr, userCfg->seed);

    CRC_HW_SetWriteTranspose(baseAddr, userCfg->writeTransposeType);
    CRC_HW_SetReadTranspose(baseAddr, userCfg->readTransposeType);
    CRC_HW_SetPolyReg(baseAddr, userCfg->poly);

    if (userCfg->resultComplement)
    {
        CRC_HW_EnableFXorMode(baseAddr);
    }
    else
    {
        CRC_HW_DisableFXorMode(baseAddr);
    }

    CRC_HW_SetProtocolWidth(baseAddr, userCfg->crcWidth);

    return STATUS_SUCCESS;
}

/*******************************************************************************
 *                          HARDWARE FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Initialize CRC module
 *
 * @param baseAddr CRC base address
 */
void CRC_HW_Init(CRC_T *const baseAddr)
{
    /* Set seed to zero */
    CRC_HW_EnableSeedOrDataMode(baseAddr);
    CRC_HW_SetDataReg(baseAddr, CRC_INITIAL_SEED);
    CRC_HW_DisableSeedOrDataMode(baseAddr);

    /* Set polynomial to 0x1021U */
    CRC_HW_SetPolyReg(baseAddr, FEATURE_CRC_DEFAULT_POLY);

    /* Set transpose and complement checksum to none */
    CRC_HW_SetWriteTranspose(baseAddr, CRC_TRS_NONE);
    CRC_HW_SetReadTranspose(baseAddr, CRC_TRS_NONE);
    CRC_HW_DisableFXorMode(baseAddr);

    /* Set 32-bit CRC mode */
    CRC_HW_SetProtocolWidth(baseAddr, CRC_32BIT_WIDTH_MODE);
}

/*!
 * @brief Sets the polynomial register
 *
 * @param baseAddr CRC base address
 * @param value Polynomial value
 */
void CRC_HW_SetPolyReg(CRC_T *const baseAddr, uint32_t value)
{
    baseAddr->POLY.reg = value;
}

/*!
 * @brief Reads the polynomial register value
 *
 * @param baseAddr CRC base address
 * @retval Returns the polynomial register value
 */
uint32_t CRC_HW_GetPolyReg(const CRC_T *const baseAddr)
{
    return  (baseAddr->POLY.reg);
}

/*!
 * @brief Sets seed value for CRC computation
 *
 * @param baseAddr CRC base address
 * @param value New seed data for CRC computation
 */
void CRC_HW_SetSeedReg(CRC_T *const baseAddr, uint32_t value)
{
    CRC_HW_EnableSeedOrDataMode(baseAddr);
    CRC_HW_SetDataReg(baseAddr, value);
    CRC_HW_DisableSeedOrDataMode(baseAddr);
}

/*!
 * @brief Enable the CRC_DATA register mode
 *
 * @param baseAddr CRC base address
 */
void CRC_HW_EnableSeedOrDataMode(CRC_T *const baseAddr)
{
    baseAddr->CTRL.bit.WASVAL = 1;
}

/*!
 * @brief Disable the CRC_DATA register mode
 *
 * @param baseAddr CRC base address
 */
void CRC_HW_DisableSeedOrDataMode(CRC_T *const baseAddr)
{
    baseAddr->CTRL.bit.WASVAL = 0;
}

/*!
 * @brief   Reads complement read of CRC data register
 * @details Some CRC protocols require the final checksum to be XORed with
 *          0xFFFFFFFF or 0xFFFF. Complement mode enables "on the fly"
 *          complementing of read data.
 *
 * @param baseAddr CRC base address
 *
 * @retval true:  Invert or complement the read value of the CRC Data register
 *         false: No XOR on reading
 */
bool CRC_HW_GetFXorMode(const CRC_T *const baseAddr)
{
    return (bool)(baseAddr->CTRL.bit.XOR_RDATA);
}

/*!
 * @brief Enables complement read of CRC data register
 *
 * @param baseAddr CRC base address
 */
void CRC_HW_EnableFXorMode(CRC_T *const baseAddr)
{
    baseAddr->CTRL.bit.XOR_RDATA = 1;
}

/*!
 * @brief Disables complement read of CRC data register
 *
 * @param baseAddr CRC base address
 */
void CRC_HW_DisableFXorMode(CRC_T *const baseAddr)
{
    baseAddr->CTRL.bit.XOR_RDATA = 0;
}

/*!
 * @brief Sets the CRC transpose type for writes
 *
 * @param baseAddr CRC base address
 * @param transp The CRC input transpose type
 */
void CRC_HW_SetWriteTranspose(CRC_T *const baseAddr, CRC_TRS_T transp)
{
    baseAddr->CTRL.bit.TTWSEL = transp;
}

/*!
 * @brief Reads the CRC transpose type for writes
 *
 * @param baseAddr CRC base address
 * @retval CRC input transpose type for writes
 */
CRC_TRS_T CRC_HW_GetWriteTranspose(const CRC_T *const baseAddr)
{
    CRC_TRS_T type;
    uint32_t regValue = (uint32_t)(baseAddr->CTRL.bit.TTWSEL);

    if (regValue == 1U)
    {
        type = CRC_TRS_BITS;
    }
    else if (regValue == 2U)
    {
        type = CRC_TRS_BITS_AND_BYTES;
    }
    else if (regValue == 3U)
    {
        type = CRC_TRS_BYTES;
    }
    else
    {
        type = CRC_TRS_NONE;
    }
    return type;
}

/*!
 * @brief Sets the CRC protocol width
 *
 * @param baseAddr CRC base address
 * @param width The CRC protocol width
 */
void CRC_HW_SetProtocolWidth(CRC_T *baseAddr, CRC_BIT_WIDTH_T width)
{
    uint32_t regValue = baseAddr->CTRL.reg;
    regValue &= ~(0x1000000u);
    regValue |= (((uint32_t)(((uint32_t)(width)) << 24u)) & 0x1000000u);

    baseAddr->CTRL.reg = regValue;
}

/*!
 * @brief Reads the CRC protocol width
 *
 * @param baseAddr CRC base address
 *
 * @retval CRC protocol width
 */
CRC_BIT_WIDTH_T CRC_HW_GetProtocolWidth(const CRC_T *const baseAddr)
{
    CRC_BIT_WIDTH_T width = CRC_16BIT_WIDTH_MODE;

    if (baseAddr->CTRL.bit.WIDCRCP)
    {
        width = CRC_32BIT_WIDTH_MODE;
    }
    return width;
}

/*!
 * @brief Sets the CRC transpose type for reads
 *
 * @param baseAddr CRC base address
 * @param transp The CRC output transpose type
 */
void CRC_HW_SetReadTranspose(CRC_T *const baseAddr, CRC_TRS_T transp)
{
    baseAddr->CTRL.bit.TTRSEL = transp;
}

/*!
 * @brief Reads the CRC transpose type for reads
 *
 * @param baseAddr CRC base address
 * @retval CRC output transpose type
 */
CRC_TRS_T CRC_HW_GetReadTranspose(const CRC_T *const baseAddr)
{
    CRC_TRS_T type;
    uint32_t regValue = (uint32_t)(baseAddr->CTRL.bit.TTRSEL);

    if (regValue == 1U)
    {
        type = CRC_TRS_BITS;
    }
    else if (regValue == 2U)
    {
        type = CRC_TRS_BITS_AND_BYTES;
    }
    else if (regValue == 3U)
    {
        type = CRC_TRS_BYTES;
    }
    else
    {
        type = CRC_TRS_NONE;
    }
    return type;
}

/*!
 * @brief Reads the current CRC result
 *
 * @param baseAddr CRC base address
 * @retval Returns the current CRC result
 */
uint32_t CRC_HW_GetDataReg(const CRC_T *const baseAddr)
{
    return (uint32_t)(baseAddr->DATA.reg);
}

/*!
 * @brief Reads the upper 16 bits of the current CRC result
 *
 * @param baseAddr CRC base address
 *
 * @retval Upper 16 bits of the current CRC result
 */
uint16_t CRC_HW_GetDataHReg(const CRC_T *const baseAddr)
{
    return (uint16_t)((baseAddr->DATA.reg) >> 16);
}

/*!
 * @brief Reads the lower 16 bits of the current CRC result
 *
 * @param baseAddr CRC base address
 * @retval Lower 16 bits of the current CRC result
 */
uint16_t CRC_HW_GetDataLReg(const CRC_T *const baseAddr)
{
   return (uint16_t)(baseAddr->DATA.reg);
}

/*!
 * @brief Sets the 32 bits of CRC data register
 *
 * @param baseAddr CRC base address
 * @param value New data for CRC computation
 */
void CRC_HW_SetDataReg(CRC_T *const baseAddr, uint32_t value)
{
    baseAddr->DATA.reg = value;
}

/*!
 * @brief Sets the lower 16 bits of CRC data register
 *
 * @param baseAddr CRC base address
 * @param value New data for CRC computation
 */
void CRC_HW_SetDataLReg(CRC_T *const baseAddr, uint16_t value)
{
    baseAddr->DATA.bit.LBLB = (uint8_t) value;
    baseAddr->DATA.bit.LBHB = (uint8_t) (value>>8);
}

/*!
 * @brief Sets the Low Lower Byte
 *
 * @param baseAddr CRC base address
 * @param value New data for CRC computation
 */
void CRC_HW_SetDataLLReg(CRC_T *const baseAddr, uint8_t value)
{
    baseAddr->DATA.bit.LBLB = value;
}

/*!
 * @brief Returns the current result of the CRC calculation
 *
 * @param baseAddr CRC base address
 * @retval Result of CRC calculation
 */
uint32_t CRC_HW_GetCrcResult(const CRC_T *const baseAddr)
{
    uint32_t crcResult;
    CRC_TRS_T transp;
    CRC_BIT_WIDTH_T width = CRC_HW_GetProtocolWidth(baseAddr);

    if (width != CRC_16BIT_WIDTH_MODE)
    {
        crcResult = CRC_HW_GetDataReg(baseAddr);
    }
    else
    {
        transp = CRC_HW_GetReadTranspose(baseAddr);
        if (   (transp != CRC_TRS_BYTES)
            && (transp != CRC_TRS_BITS_AND_BYTES))
        {
            crcResult = (uint32_t)CRC_HW_GetDataLReg(baseAddr);
        }
        else
        {
            crcResult = (uint32_t)CRC_HW_GetDataHReg(baseAddr);
        }
    }
    return crcResult;
}

/**@} end of group CRC_Functions*/
/**@} end of group CRC Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
