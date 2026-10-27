/*!
 * @file        user_einj_config.h
 *
 * @brief       Peripheral configurations
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

#ifndef USER_MPU_CONFIG_H
#define USER_MPU_CONFIG_H

/* Includes */
#include "apm32f445_446_mpu.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup MPU_MemoryProtection
  @{
  */

/** @defgroup MPU_MemoryProtection_Macros Macros
  @{
  */

#define MPU_REGION_NUMBER (5U)

/**@} end of group MPU_MemoryProtection_Macros */
/**@} end of group MPU_MemoryProtection */
/**@} end of group Examples */

extern MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightGrantALL[];
extern MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightDenyALL[];
extern MPU_MASTER_ACCESS_PERMISSION_T g_mpuMasterAccessRightDenyRead[];
extern MPU_USER_CONFIG_T g_mpuUserConfig[];

#endif /* USER_EINJ_CONFIG_H */
