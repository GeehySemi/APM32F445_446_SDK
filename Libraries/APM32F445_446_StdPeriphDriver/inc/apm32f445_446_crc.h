/*!
 * @file        apm32f445_446_crc.h
 *
 * @brief       This file contains all the functions prototypes for the CRC firmware library.
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

#ifndef APM32F445_446_CRC_H
#define APM32F445_446_CRC_H

/* Includes */
#include "device.h"

#if defined (__cplusplus)
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CRC_Driver CRC Driver
  @{
*/

/** @defgroup CRC_Enumerations Enumerations
  @{
*/
/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/
 /* CRC bit width */
typedef enum
{
    CRC_16BIT_WIDTH_MODE = 0U,  /* 16-bit CRC code */
    CRC_32BIT_WIDTH_MODE = 1U /* 32-bit CRC code */
} CRC_BIT_WIDTH_T;

/* CRC type of transpose of read write data */
typedef enum
{
    CRC_TRS_NONE            = 0x00U,    /* No transpose */
    CRC_TRS_BITS            = 0x01U,    /* Transpose bits in bytes */
    CRC_TRS_BITS_AND_BYTES  = 0x02U,    /* Transpose bytes and bits in bytes */
    CRC_TRS_BYTES           = 0x03U     /* Transpose bytes */
} CRC_TRS_T;

/**@} end of group CRC_Enumerations*/

/** @defgroup CRC_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/
 /* CRC user configuration structure */
typedef struct
{
    uint32_t poly;                  /* CRC Polynomial, MSB first */
    CRC_TRS_T readTransposeType;    /* Type of transpose while reading CRC result */
    CRC_BIT_WIDTH_T crcWidth;       /* Use 16-bit or 32-bit CRC protocol */
    bool resultComplement;          /* If the result need to be complement of the actual checksum */
    uint32_t seed;                  /* Initial checksum value */
    CRC_TRS_T writeTransposeType;   /* Type of transpose while writing CRC input data */

} CRC_USER_CFG_T;

/**@} end of group CRC_Structures*/

/** @defgroup CRC_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC FUNCTIONS
 ******************************************************************************/

STATUS_T CRC_Init(uint32_t ins, const CRC_USER_CFG_T *userCfg);
STATUS_T CRC_Deinit(uint32_t ins);

STATUS_T CRC_DefaultConfig(CRC_USER_CFG_T * const userCfg);
STATUS_T CRC_GetConfig(uint32_t ins, CRC_USER_CFG_T * const userCfg);

void CRC_WriteData(uint32_t ins, const uint32_t *data, uint32_t dataSize);
uint32_t CRC_GetCrcResult(uint32_t ins);

/*******************************************************************************
 *                          HARDWARE FUNCTIONS
 ******************************************************************************/

void CRC_HW_Init(CRC_T *const baseAddr);

void CRC_HW_SetPolyReg(CRC_T *const baseAddr, uint32_t value);
uint32_t CRC_HW_GetPolyReg(const CRC_T *const baseAddr);

void CRC_HW_SetSeedReg(CRC_T *const baseAddr, uint32_t value);
void CRC_HW_EnableSeedOrDataMode(CRC_T *const baseAddr);
void CRC_HW_DisableSeedOrDataMode(CRC_T *const baseAddr);

bool CRC_HW_GetFXorMode(const CRC_T *const baseAddr);
void CRC_HW_EnableFXorMode(CRC_T *const baseAddr);
void CRC_HW_DisableFXorMode(CRC_T *const baseAddr);

void CRC_HW_SetReadTranspose(CRC_T *const baseAddr, CRC_TRS_T transp);
CRC_TRS_T CRC_HW_GetReadTranspose(const CRC_T *const baseAddr);
void CRC_HW_SetWriteTranspose(CRC_T *const baseAddr, CRC_TRS_T transp);
CRC_TRS_T CRC_HW_GetWriteTranspose(const CRC_T *const baseAddr);

void CRC_HW_SetProtocolWidth(CRC_T *  baseAddress, CRC_BIT_WIDTH_T width);
CRC_BIT_WIDTH_T CRC_HW_GetProtocolWidth(const CRC_T *const baseAddr);

uint32_t CRC_HW_GetDataReg(const CRC_T *const baseAddr);
uint16_t CRC_HW_GetDataHReg(const CRC_T *const baseAddr);
uint16_t CRC_HW_GetDataLReg(const CRC_T *const baseAddr);

void CRC_HW_SetDataReg(CRC_T *const baseAddr, uint32_t value);
void CRC_HW_SetDataLReg(CRC_T *const baseAddr, uint16_t value);
void CRC_HW_SetDataLLReg(CRC_T *const baseAddr, uint8_t value);

uint32_t CRC_HW_GetCrcResult(const CRC_T *const baseAddr);

/**@} end of group CRC_Functions*/
/**@} end of group CRC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif

#endif /* APM32F445_446_CRC_H */
