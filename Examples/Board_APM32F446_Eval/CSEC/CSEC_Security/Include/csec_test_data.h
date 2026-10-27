/*!
 * @file        csec_test_data.h
 *
 * @brief       The data used for testing CSEC functions
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

#ifndef CSEC_TEST_DATA_H
#define CSEC_TEST_DATA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CSEC_Security
  @{
*/

/** @defgroup CSEC_Security_Macros Macros
  @{
*/

#define KEY_SIZE        16
#define ECB_TEXT_SIZE   160
#define CBC_TEXT_SIZE   48
#define CBC_IV_SIZE     16
#define MAC_MSG_SIZE    96
#define MAC_SIZE        16

/**@} end of group CSEC_Security_Macros*/
/**@} end of group CSEC_Security*/
/**@} end of group Examples*/

/* The empty used when the MASTER_ECU_KEY has not been loaded */
extern uint8_t g_emptyKey[KEY_SIZE];

/* The MASTER_ECU_KEY */
extern uint8_t g_masterEcuKey[KEY_SIZE];

/* The user key used for encryption/decryption and CMAC generation */
extern uint8_t g_userKey[KEY_SIZE];

/* Plain text for AES-128 ECB encryption/decryption */
extern uint8_t g_ecbPlainText[ECB_TEXT_SIZE];

/* Cipher text for AES-128 ECB encryption/decryption */
extern uint8_t g_ecbCipherText[ECB_TEXT_SIZE];

/* IV used for AES-128 CBC encryption/decryption */
extern uint8_t g_cbcIv[CBC_IV_SIZE];

/* Plain text for AES-128 CBC encryption/decryption */
extern uint8_t g_cbcPlainText[CBC_TEXT_SIZE];

/* Cipher text for AES-128 CBC encryption/decryption */
extern uint8_t g_cbcCipherText[CBC_TEXT_SIZE];

/* The input message used to generate CMAC */
extern uint8_t g_macMessage[MAC_MSG_SIZE];

/* The expected CMAC values for the input message */
extern uint8_t g_macValues[16];

#ifdef __cplusplus
}
#endif

#endif /* CSEC_TEST_DATA_H */
