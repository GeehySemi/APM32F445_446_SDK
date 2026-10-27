/*!
 * @file        apm32f445_446_mpu.h
 *
 * @brief       This file contains all the functions prototypes for the MPU firmware library.
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

#ifndef APM32F445_446_MPU_H
#define APM32F445_446_MPU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup MPU_Driver MPU Driver
  @{
*/

/** @defgroup MPU_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief MPU access error
 */
typedef enum
{
    MPU_ERR_TYPE_WRITE = 1U,
    MPU_ERR_TYPE_READ  = 0U
} MPU_ERR_ACCESS_TYPE_T;

/*!
 * @brief MPU access error attributes
 */
typedef enum
{
    MPU_DATA_ACCESS_IN_SUPERVISOR_MODE        = 3U,
    MPU_INSTRUCTION_ACCESS_IN_SUPERVISOR_MODE = 2U,
    MPU_DATA_ACCESS_IN_USER_MODE              = 1U,
    MPU_INSTRUCTION_ACCESS_IN_USER_MODE       = 0U
} MPU_ERR_ATTRIBUTES_T;

/*!
 * @brief MPU access permissions.
 * @details
 * Reference: M_R_W_SM_UM
 * M  : 1 bit  - Specify that access permission is for masters which have separated
 *               privilege rights for user and supervisor mode accesses (e.g. master0~3)
 * R  : 1 bit  - Read access permission
 * W  : 1 bit  - Write access permission
 * SM : 2 bits - Supervisor Mode Access Control
 * UM: 3 bits - User Mode Access Control
 *
 * |      Code               |  Supervisor  |     User    |  Description                                                                       |
 * |-------------------------|--------------|-------------|------------------------------------------------------------------------------------|
 * |  MPU_SUPV_RWX_USER_NONE |    r w x     |    - - -    | Allow Read, write, execute in supervisor mode; no access in user mode              |
 * |  MPU_SUPV_RWX_USER_X    |    r w x     |    - - x    | Allow Read, write, execute in supervisor mode; execute in user mode                |
 * |  MPU_SUPV_RWX_USER_W    |    r w x     |    - w -    | Allow Read, write, execute in supervisor mode; write in user mode                  |
 * |  MPU_SUPV_RWX_USER_WX   |    r w x     |    - w x    | Allow Read, write, execute in supervisor mode; write and execute in user mode      |
 * |  MPU_SUPV_RWX_USER_R    |    r w x     |    r - -    | Allow Read, write, execute in supervisor mode; read in user mode                   |
 * |  MPU_SUPV_RWX_USER_RX   |    r w x     |    r - x    | Allow Read, write, execute in supervisor mode; read and execute in user mode       |
 * |  MPU_SUPV_RWX_USER_RW   |    r w x     |    r w -    | Allow Read, write, execute in supervisor mode; read and write in user mode         |
 * |  MPU_SUPV_RWX_USER_RWX  |    r w x     |    r w x    | Allow Read, write, execute in supervisor mode; read, write and execute in user mode|
 * |  MPU_SUPV_RX_USER_NONE  |    r - x     |    - - -    | Allow Read, execute in supervisor mode; no access in user mode                     |
 * |  MPU_SUPV_RX_USER_X     |    r - x     |    - - x    | Allow Read, execute in supervisor mode; execute in user mode                       |
 * |  MPU_SUPV_RX_USER_W     |    r - x     |    - w -    | Allow Read, execute in supervisor mode; write in user mode                         |
 * |  MPU_SUPV_RX_USER_WX    |    r - x     |    - w x    | Allow Read, execute in supervisor mode; write and execute in user mode             |
 * |  MPU_SUPV_RX_USER_R     |    r - x     |    r - -    | Allow Read, execute in supervisor mode; read in user mode                          |
 * |  MPU_SUPV_RX_USER_RX    |    r - x     |    r - x    | Allow Read, execute in supervisor mode; read and execute in user mode              |
 * |  MPU_SUPV_RX_USER_RW    |    r - x     |    r w -    | Allow Read, execute in supervisor mode; read and write in user mode                |
 * |  MPU_SUPV_RX_USER_RWX   |    r - x     |    r w x    | Allow Read, execute in supervisor mode; read, write and execute in user mode       |
 * |  MPU_SUPV_RW_USER_NONE  |    r w -     |    - - -    | Allow Read, write in supervisor mode; no access in user mode                       |
 * |  MPU_SUPV_RW_USER_X     |    r w -     |    - - x    | Allow Read, write in supervisor mode; execute in user mode                         |
 * |  MPU_SUPV_RW_USER_W     |    r w -     |    - w -    | Allow Read, write in supervisor mode; write in user mode                           |
 * |  MPU_SUPV_RW_USER_WX    |    r w -     |    - w x    | Allow Read, write in supervisor mode; write and execute in user mode               |
 * |  MPU_SUPV_RW_USER_R     |    r w -     |    r - -    | Allow Read, write in supervisor mode; read in user mode                            |
 * |  MPU_SUPV_RW_USER_RX    |    r w -     |    r - x    | Allow Read, write in supervisor mode; read and execute in user mode                |
 * |  MPU_SUPV_RW_USER_RW    |    r w -     |    r w -    | Allow Read, write in supervisor mode; read and write in user mode                  |
 * |  MPU_SUPV_RW_USER_RWX   |    r w -     |    r w x    | Allow Read, write in supervisor mode; read, write and execute in user mode         |
 * |  MPU_SUPV_USER_NONE     |    - - -     |    - - -    | No access allowed in user and supervisor modes                                     |
 * |  MPU_SUPV_X_USER_X      |    - - x     |    - - x    | Execute operation is allowed in user and supervisor modes                          |
 * |  MPU_SUPV_W_USER_W      |    - w -     |    - w -    | Write operation is allowed in user and supervisor modes                            |
 * |  MPU_SUPV_WX_USER_WX    |    - w x     |    - w x    | Write and execute operations are allowed in user and supervisor modes              |
 * |  MPU_SUPV_R_USER_R      |    r - -     |    r - -    | Read operation is allowed in user and supervisor modes                             |
 * |  MPU_SUPV_USER_RX       |    r - x     |    r - x    | Read and execute operations are allowed in user and supervisor modes               |
 * |  MPU_SUPV_USER_RW       |    r w -     |    r w -    | Read and write operations are allowed in user and supervisor modes                 |
 * |  MPU_SUPV_USER_RWX      |    r w x     |    r w x    | Read write and execute operations are allowed in user and supervisor modes         |
 * |  MPU_NONE               |          - -            | No Read/Write access permission   |
 * |  MPU_W                  |          - w            | Write access permission           |
 * |  MPU_R                  |          r -            | Read access permission            |
 * |  MPU_RW                 |          r w            | Read/Write access permission      |
 */
typedef enum
{
    MPU_SUPV_RWX_USER_NONE    = 0x00U,
    MPU_SUPV_RWX_USER_X       = 0x01U,
    MPU_SUPV_RWX_USER_W       = 0x02U,
    MPU_SUPV_RWX_USER_WX      = 0x03U,
    MPU_SUPV_RWX_USER_R       = 0x04U,
    MPU_SUPV_RWX_USER_RX      = 0x05U,
    MPU_SUPV_RWX_USER_RW      = 0x06U,
    MPU_SUPV_RWX_USER_RWX     = 0x07U,
    MPU_SUPV_RX_USER_NONE     = 0x08U,
    MPU_SUPV_RX_USER_X        = 0x09U,
    MPU_SUPV_RX_USER_W        = 0x0AU,
    MPU_SUPV_RX_USER_WX       = 0x0BU,
    MPU_SUPV_RX_USER_R        = 0x0CU,
    MPU_SUPV_RX_USER_RX       = 0x0DU,
    MPU_SUPV_RX_USER_RW       = 0x0EU,
    MPU_SUPV_RX_USER_RWX      = 0x0FU,
    MPU_SUPV_RW_USER_NONE     = 0x10U,
    MPU_SUPV_RW_USER_X        = 0x11U,
    MPU_SUPV_RW_USER_W        = 0x12U,
    MPU_SUPV_RW_USER_WX       = 0x13U,
    MPU_SUPV_RW_USER_R        = 0x14U,
    MPU_SUPV_RW_USER_RX       = 0x15U,
    MPU_SUPV_RW_USER_RW       = 0x16U,
    MPU_SUPV_RW_USER_RWX      = 0x17U,
    MPU_SUPV_USER_NONE        = 0x18U,
    MPU_SUPV_X_USER_X         = 0x19U,
    MPU_SUPV_W_USER_W         = 0x1AU,
    MPU_SUPV_WX_USER_WX       = 0x1BU,
    MPU_SUPV_R_USER_R         = 0x1CU,
    MPU_SUPV_USER_RX          = 0x1DU,
    MPU_SUPV_USER_RW          = 0x1EU,
    MPU_SUPV_USER_RWX         = 0x1FU,
    MPU_NONE                  = 0x80U,
    MPU_W                     = 0xA0U,
    MPU_R                     = 0xC0U,
    MPU_RW                    = 0xE0U
} MPU_ACCESS_PERMISSIONS_T;

/**@} end of group MPU_Enumerations*/

/** @defgroup MPU_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief MPU detail error access info
 */
typedef struct
{
    uint8_t                master;
    MPU_ERR_ATTRIBUTES_T   attributes;
    MPU_ERR_ACCESS_TYPE_T  accessType;
    uint16_t               accessCtr;
    uint32_t               addr;
    uint8_t                processorIdentification;
} MPU_ACCESS_ERROR_INFO_T;

/*!
 * @brief MPU master access rights.
 */
typedef struct
{
    uint8_t                   numOfMaster;
    MPU_ACCESS_PERMISSIONS_T  accessPerssion;
    bool                      isProcessIdentifier;
} MPU_MASTER_ACCESS_PERMISSION_T;

/*!
 * @brief MPU user region configuration structure.
 */
typedef struct
{
    uint32_t                             endAddr;
    uint32_t                             startAddr;
    uint8_t                              processIdMask;
    uint8_t                              processID;
    const MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion;
} MPU_USER_CONFIG_T;

/**@} end of group MPU_Structures*/

/** @defgroup MPU_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T MPU_Init(uint32_t ins,
                  uint8_t regionCount,
                  const MPU_USER_CONFIG_T *userConfigArray);
void MPU_DeInit(uint32_t ins);

MPU_USER_CONFIG_T MPU_RegionDefaultConfig(MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion);
void MPU_EnableRegion(uint32_t ins, uint8_t regionNum, bool enable);
STATUS_T MPU_UpdateRegionCfg(uint32_t ins,
                             uint8_t regionNum,
                             const MPU_USER_CONFIG_T *userConfigPtr);
void MPU_ConfigRegionAddr(uint32_t ins,
                          uint8_t regionNum,
                          uint32_t startAddr,
                          uint32_t endAddr);
bool MPU_ReadDetailErrorAccessInfo(uint32_t ins,
                                   uint8_t slavePortNum,
                                   MPU_ACCESS_ERROR_INFO_T *errInfoPtr);
STATUS_T MPU_ConfigMasterAccessPerssion(uint32_t ins,
                                      uint8_t regionNum,
                                      const MPU_MASTER_ACCESS_PERMISSION_T *accessRightsPtr);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void MPU_HW_Enable(MPU_T *const mpuBase);
void MPU_HW_Disable(MPU_T *const mpuBase);

void MPU_HW_EnableRegionValid(MPU_T *const mpuBase, uint8_t regionNum);
void MPU_HW_DisableRegionValid(MPU_T *const mpuBase, uint8_t regionNum);

void MPU_HW_ResetRegion(MPU_T *const mpuBase, uint8_t regionNum);

void MPU_HW_MasterAccessRightDefaultConfig(uint8_t numOfMaster,
                                           MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion);
void MPU_HW_ConfigMasterAccessRight(MPU_T *const mpuBase,
                                    uint8_t regionNum,
                                    const MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion);

void MPU_HW_ConfigProcessIdentifier(MPU_T *const mpuBase, uint8_t regionNum, uint8_t pid);
void MPU_HW_ConfigProcessIdentifierMask(MPU_T *const mpuBase,
                                        uint8_t regionNum,
                                        uint8_t pidMask);

bool MPU_HW_ReadErrorStatus(const MPU_T *const mpuBase, uint8_t slaveNum);
void MPU_HW_ClearErrorFlag(MPU_T *const mpuBase, uint8_t slaveNum);
void MPU_HW_ReadErrorInfo(const MPU_T *const mpuBase,
                          uint8_t slaveNum,
                          MPU_ACCESS_ERROR_INFO_T *errInfoPtr);

/**@} end of group MPU_Functions*/
/**@} end of group MPU_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#ifdef __cplusplus
}
#endif

#endif /* APM32F445_446_MPU_H */
