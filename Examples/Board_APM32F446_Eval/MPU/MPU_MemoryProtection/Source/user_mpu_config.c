/*!
 * @file        user_mpu_config.c
 *
 * @brief       Peripheral configurations
 *
 * @version     V1.0.0
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2024 Geehy Semiconductor
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
#include "user_mpu_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup MPU_MemoryProtection
  @{
  */

/** @defgroup MPU_MemoryProtection_Variables Variables
  @{
  */

MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightDenyALL[] =
{
    /* Core */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_USER_NONE,
        .numOfMaster = FEATURE_MPU_MASTER_CORE,
    },
    /* Debugger */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DEBUGGER,
    },
    /* DMA */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DMA,
    },
};

MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightGrantALL[] =
{
    /* Core */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_CORE,
    },
    /* Debugger */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DEBUGGER,
    },
    /* DMA */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DMA,
    },
};

MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightDenyRead[] =
{
    /* Core */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_WX_USER_WX,
        .numOfMaster = FEATURE_MPU_MASTER_CORE
    },
    /* Debugger */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DEBUGGER
    },
    /* DMA */
    {
        .isProcessIdentifier = false,
        .accessPerssion = MPU_SUPV_RWX_USER_RWX,
        .numOfMaster = FEATURE_MPU_MASTER_DMA
    },
};

MPU_USER_CONFIG_T g_mpuUserConfig[MPU_REGION_NUMBER] =
{
    /* Region0(placeholder) */
    {
        .processID = 0x0U,
        .processIdMask = 0x0U,
        .startAddr = 0x0U,
        .endAddr = 0xFFFFFFFFU,
        .masterAccessPerssion = g_mpuMasterAccessRightDenyALL

    },
    /* Region1 */
    {
        .processID = 0x0U,
        .processIdMask = 0x0U,
        .startAddr = 0x0U,
        .endAddr = 0x7FEFFU,
        .masterAccessPerssion = g_mpuMasterAccessRightGrantALL

    },
    /* Region2 */
    {
        .processID = 0x0U,
        .processIdMask = 0x0U,
        .startAddr = 0x7FF00U,
        .endAddr = 0x7FF1FU,
        .masterAccessPerssion = g_mpuMasterAccessRightDenyRead

    },
    /* Region3 */
    {
        .processIdMask = 0x0U,
        .startAddr = 0x7FF00U,
        .endAddr = 0x7FF1FU,
        .processID = 0x0U,
        .masterAccessPerssion = g_mpuMasterAccessRightGrantALL
    },
    /* Region4 */
    {
        .processID = 0x0U,
        .processIdMask = 0x0U,
        .startAddr = 0x7FF20U,
        .endAddr = 0xFFFFFFFFU,
        .masterAccessPerssion = g_mpuMasterAccessRightGrantALL
    }
};

/**@} end of group MPU_MemoryProtection_Variables */
/**@} end of group MPU_MemoryProtection */
/**@} end of group Examples */
