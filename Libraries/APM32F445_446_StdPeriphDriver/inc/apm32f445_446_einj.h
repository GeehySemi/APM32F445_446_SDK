/*!
 * @file        apm32f445_446_einj.h
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
 *  The program is only for reference,  which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing,  the program is distributed on an "AS IS" BASIS,  WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND,  either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#ifndef APM32F445_446_EINJ_H
#define APM32F445_446_EINJ_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

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
#define EINJ_INSTANCE_COUNT   (1U)
#define EINJ_INSTANCE_VALIDITY(__instance__) if(__instance__ >= EINJ_INSTANCE_COUNT){while(1);}

#define EINJ_CHECKBITMASK_DEFAULT    (0x01U)    /* EINJ check-bit mask */
#define EINJ_DATAMASK_DEFAULT        (0x00U)    /* EINJ data mask */

/**@} end of group EINJ_Macros*/

/** @defgroup EINJ_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief EINJ Channel configuration.
 */
typedef struct
{
    uint8_t channel;      /* EINJ channel number */
    uint8_t checkBitMask; /* Specifies whether the corresponding bit of the check-bit bus
                             from the target RAM should be inverted or remain unmodified */
    uint32_t dataMask;    /* Specifies whether the corresponding bit of the read data bus
                             from the target RAM should be inverted or remain unmodified */
    bool enable;          /* true : EINJ channel enabled
                             false : EINJ channel disabled */
} EINJ_USER_CHANNEL_CONFIG_T;

/**@} end of group EINJ_Structures*/

/** @defgroup EINJ_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void EINJ_Init(uint32_t ins, uint8_t channelCnt, const EINJ_USER_CHANNEL_CONFIG_T *channelCfgArr);
void EINJ_DefaultConfig(uint8_t channel, EINJ_USER_CHANNEL_CONFIG_T *channelCfg);
void EINJ_DeInit(uint32_t ins);

void EINJ_ChannelConfig(uint32_t ins, const EINJ_USER_CHANNEL_CONFIG_T *userChannelCfg);
void EINJ_ReadChannelConfig(uint32_t ins, uint8_t channel, EINJ_USER_CHANNEL_CONFIG_T *channelCfg);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void EINJ_HW_Enable(EINJ_T *const einjBase);
void EINJ_HW_Disable(EINJ_T *const einjBase);

void EINJ_HW_ChannelSetup(EINJ_T *const einjBase);
void EINJ_HW_ChannelCmd(EINJ_T *const einjBase, uint8_t channel, bool enable);
bool EINJ_HW_IsChannelEnabled(const EINJ_T *const einjBase, uint8_t channel);

void EINJ_HW_AssignCheckBit(EINJ_T *const einjBase, uint8_t channel, uint8_t checkBitMask);
uint32_t EINJ_HW_ReadCheckBit(const EINJ_T *const einjBase, uint8_t channel);

void EINJ_HW_AssignData(EINJ_T *const einjBase, uint8_t channel, uint32_t dataMask);
uint32_t EINJ_HW_ReadData(const EINJ_T *const einjBase, uint8_t channel);

/**@} end of group EINJ_Functions*/
/**@} end of group EINJ_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif /* APM32F445_446_EINJ_H */
