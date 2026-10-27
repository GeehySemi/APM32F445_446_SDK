/*!
 * @file        csec_utils.h
 *
 * @brief       Utils for CSEC functions
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

#ifndef CSEC_UTILS_H
#define CSEC_UTILS_H

/* Includes */
#include "apm32f445_446_csec.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CSEC_Security
  @{
*/

/** @defgroup CSEC_Security_Functions Functions
  @{
*/
/**
 * This function sets the MASTER_ECU key with a key (g_masterEcuKey) defined in
 * the csec_utils.c file. This key will be used as an authorization secret for
 * updating user keys. Setting the MASTER_ECU_KEY will work only for the first
 * time, in order to use another value for the key, there are two options:
 * - Erase all the keys and then load MASTER_ECU_KEY again
 * - Use LoadKey with counter > 1 and then update g_masterEcuKey
 */
bool LoadMasterEcuKey(void);

/**
 * Load or update a non-volatile key. When updating the key, the counter must
 * be greater than the previous one.
 */
bool LoadKey(CSEC_KEY_ID_T keyId, uint8_t *newKey, uint8_t counter, bool bootProtection);

/* Get the UID */
STATUS_T GetUID(uint8_t *uid);

/**
 * This function will reset the flash to factory state, this will erase all
 * the keys.
 */
bool FlashFactoryReset(void);

/**@} end of group CSEC_Security_Functions*/
/**@} end of group CSEC_Security*/
/**@} end of group Examples*/

#endif /* CSEC_UTILS_H */
