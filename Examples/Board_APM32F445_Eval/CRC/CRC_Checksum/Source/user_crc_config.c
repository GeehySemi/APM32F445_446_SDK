/*!
 * @file        user_crc_config.c
 *
 * @brief       crc configurations
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
#include "user_crc_config.h"
#include "apm32f445_446_crc.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CRC_Checksum
  @{
  */

/** @defgroup CRC_Checksum_Variables Variables
  @{
  */

/*! @brief Configuration structure g_crcConfig0 */
/* CRC-16-CCITT-FALSE  */
const CRC_USER_CFG_T g_crcConfig0 = {
    .crcWidth = CRC_16BIT_WIDTH_MODE,
    .seed = 0xFFFFU,
    .poly = 0x1021U,
    .writeTransposeType = CRC_TRS_NONE,
    .readTransposeType = CRC_TRS_NONE,
    .resultComplement = false
};

/*! @brief Configuration structure g_crcConfig1 */
/* CRC-32-CCITT-FALSE */
const CRC_USER_CFG_T g_crcConfig1 = {
    .crcWidth = CRC_32BIT_WIDTH_MODE,
    .seed = 0xFFFFFFFFU,
    .poly = 0x04C11DB7U,
    .writeTransposeType = CRC_TRS_NONE,
    .readTransposeType = CRC_TRS_NONE,
    .resultComplement = false
};

/**@} end of group CRC_Checksum_Variables */
/**@} end of group CRC_Checksum */
/**@} end of group Examples */
