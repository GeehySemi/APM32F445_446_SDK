/*!
 * @file        apm32f445_446_csec.h
 *
 * @brief       CSEc driver functions
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

#ifndef APM32F445_446_CSEC_H
#define APM32F445_446_CSEC_H

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"
#include "callbacks.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CSEC_Driver CSEC Driver
  @{
*/

/** @defgroup CSEC_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/*!
 * @brief  Result of the execution of a command
 */
#define CSEC_ERC_NO_ERROR               (0x1U)
#define CSEC_ERC_SEQUENCE_ERROR         (0x2U)
#define CSEC_ERC_KEY_NOT_AVAILABLE      (0x4U)
#define CSEC_ERC_KEY_INVALID            (0x8U)
#define CSEC_ERC_KEY_EMPTY              (0x10U)
#define CSEC_ERC_NO_SECURE_BOOT         (0x20U)
#define CSEC_ERC_KEY_WRITE_PROTECTED    (0x40U)
#define CSEC_ERC_KEY_UPDATE_ERROR       (0x80U)
#define CSEC_ERC_RNG_SEED               (0x100U)
#define CSEC_ERC_NO_DEBUGGING           (0x200U)
#define CSEC_ERC_MEMORY_FAILURE         (0x400U)
#define CSEC_ERC_GENERAL_ERROR          (0x800U)

/*!
 * @brief  CSEc status bits
 */
#define CSEC_STATE_BUSY                 (0x1U)
#define CSEC_STATE_SECURE_BOOT          (0x2U)
#define CSEC_STATE_BOOT_INIT            (0x4U)
#define CSEC_STATE_BOOT_FINISHED        (0x8U)
#define CSEC_STATE_BOOT_OK              (0x10U)
#define CSEC_STATE_RND_INIT             (0x20U)
#define CSEC_STATE_EXTERNAL_DEBUG       (0x40U)
#define CSEC_STATE_INTERNAL_DEBUG       (0x80U)

/**@} end of group CSEC_Macros*/
/** @defgroup CSEC_Typedef Typedefs
  @{
*/

/*!
 * @brief   Status of the CSEc module
 */
typedef uint8_t CSEC_MODULE_STATE_T;

/**@} end of group CSEC_Typedef*/
/** @defgroup CSEC_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief   Key IDs
 */
typedef enum {
    CSEC_SECRET_KEY = 0x0U,
    CSEC_MASTER_ECU,
    CSEC_BOOT_MAC_KEY,
    CSEC_BOOT_MAC,
    CSEC_KEY_1,
    CSEC_KEY_2,
    CSEC_KEY_3,
    CSEC_KEY_4,
    CSEC_KEY_5,
    CSEC_KEY_6,
    CSEC_KEY_7,
    CSEC_KEY_8,
    CSEC_KEY_9,
    CSEC_KEY_10,
    CSEC_RAM_KEY = 0xFU,
    CSEC_KEY_11 = 0x14U,
    CSEC_KEY_12,
    CSEC_KEY_13,
    CSEC_KEY_14,
    CSEC_KEY_15,
    CSEC_KEY_16,
    CSEC_KEY_17
} CSEC_KEY_ID_T;

/*!
 * @brief   CSEc commands
 */
typedef enum {
    CSEC_CMD_ENC_ECB = 0x1U,
    CSEC_CMD_ENC_CBC,
    CSEC_CMD_DEC_ECB,
    CSEC_CMD_DEC_CBC,
    CSEC_CMD_GENERATE_MAC,
    CSEC_CMD_VERIFY_MAC,
    CSEC_CMD_LOAD_KEY,
    CSEC_CMD_LOAD_PLAIN_KEY,
    CSEC_CMD_EXPORT_RAM_KEY,
    CSEC_CMD_INIT_RNG,
    CSEC_CMD_EXTEND_SEED,
    CSEC_CMD_RND,
    CSEC_CMD_RESERVED_1,
    CSEC_CMD_BOOT_FAILURE,
    CSEC_CMD_BOOT_OK,
    CSEC_CMD_GET_ID,
    CSEC_CMD_BOOT_DEFINE,
    CSEC_CMD_DBG_CHAL,
    CSEC_CMD_DBG_AUTH,
    CSEC_CMD_RESERVED_2,
    CSEC_CMD_RESERVED_3,
    CSEC_CMD_MP_COMPRESS
} CSEC_CMD_T;

/*!
 * @brief   Call sequence specifies the information if it is the first or a
 *          following function call
 */
typedef enum {
    CSEC_CALLSEQ_FIRST,
    CSEC_CALLSEQ_FOLLOWING
} CSEC_CALL_SEQUENCE_T;

/*!
 * @brief   Secure boot mode
 */
typedef enum {
    CSEC_BOOT_MODE_STRICT,
    CSEC_BOOT_MODE_SERIAL,
    CSEC_BOOT_MODE_PARALLEL,
    CSEC_BOOT_MODE_NOT_DEFINED
} CSEC_BOOT_MODE_T;

/*!
 * @brief   Specifies how the data is transferred to/from the CSE
 */
typedef enum {
    CSEC_CMD_FORMAT_COPY,
    CSEC_CMD_FORMAT_POINTER
} CSEC_CMD_FORMAT_T;

/**@} end of group CSEC_Enumerations*/

/** @defgroup CSEC_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief   Internal CSEc driver state
 * @details The contents of this structure are internal to the driver and
 *          should not be modified by users.
 */
typedef struct {
    bool isCmdBusy;                 /* If the module is busy processing a command */
    CSEC_CMD_T cmd;                 /* Type of the command in execution */
    STATUS_T cmdStatus;             /* Status of the last executed command */
    CSEC_CALL_SEQUENCE_T callSeq;   /* If it is the first or a subsequent function call */

    CSEC_KEY_ID_T keyId;            /* The key used for the command in execution */
    const uint8_t *iv;              /* IV of the command in execution (only for CBC mode) */

    const uint8_t *inputData;       /* Input data of the command in execution */
    uint32_t totalInputSize;        /* Total size of the input data */
    uint32_t currentInputSize;      /* Size of the chunk currently processed */
    uint32_t inputIndex;            /* Index in the input buffer of the command in execution */
    uint8_t *outputData;            /* Output of the command in execution */

    uint32_t msgSize;               /* MAC message size (in bits) for MAC generate/verify commands */
    const uint8_t *macBuf;          /* MAC to be verified for a MAC verify command */
    uint32_t macLen;                /* MAC size (in bits) to be verified for a MAC verify command */
    bool isMacWritten;              /* If the MAC to be verified was written in CSE_PRAM for MAC verify command */
    bool *macVerifyResult;          /* Result of the last MAC verify command */

    CSEC_CALLBACK_T callback;       /* Callback when an asynchronous command is completed */
    void *callbackParam;            /* User parameter for the callback */
} CSEC_STATE_T;


/**@} end of group CSEC_Structures*/

/** @defgroup CSEC_Variables Variables
  @{
*/

/* Pointer to runtime state structure.*/
extern CSEC_STATE_T *g_csecState;

/**@} end of group CSEC_Variables*/


/** @defgroup CSEC_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

void CSEC_Init(CSEC_STATE_T *states);
void CSEC_DeInit(void);

void CSEC_InstallCallback(
    CSEC_CALLBACK_T callback,
    void *parameter);

STATUS_T CSEC_InitRng(void);
STATUS_T CSEC_ExtendSeed(const uint8_t *entropy);
STATUS_T CSEC_GenerateRandomNumber(uint8_t *randomNumber);

STATUS_T CSEC_EncryptCbcSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    const uint8_t *iv,
    uint8_t *cipherTextBuf,
    uint32_t timeout);

STATUS_T CSEC_EncryptCbcAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    const uint8_t *iv,
    uint8_t *cipherTextBuf);

STATUS_T CSEC_DecryptCbcSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    const uint8_t *iv,
    uint8_t *plainTextBuf,
    uint32_t timeout);

STATUS_T CSEC_DecryptCbcAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    const uint8_t *iv,
    uint8_t *plainTextBuf);

STATUS_T CSEC_EncryptEcbSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    uint8_t *cipherTextBuf,
    uint32_t timeout);

STATUS_T CSEC_EncryptEcbAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    uint8_t *cipherTextBuf);

STATUS_T CSEC_DecryptEcbSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    uint8_t *plainTextBuf,
    uint32_t timeout);

STATUS_T CSEC_DecryptEcbAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    uint8_t *plainTextBuf);

STATUS_T CSEC_GenerateMacSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf,
    uint32_t timeout);

STATUS_T CSEC_GenerateMacAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf);

STATUS_T CSEC_GenerateMacNoCopy(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf);

STATUS_T CSEC_VerifyMacSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult,
    uint32_t timeout);

STATUS_T CSEC_VerifyMacAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult);

STATUS_T CSEC_VerifyMacNoCopy(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult);

STATUS_T CSEC_LoadKey(
    CSEC_KEY_ID_T keyId,
    const uint8_t *m1,
    const uint8_t *m2,
    const uint8_t *m3,
    uint8_t *m4,
    uint8_t *m5);

STATUS_T CSEC_LoadPlainKey(const uint8_t *plainKey);

STATUS_T CSEC_ExportRAMKey(
    uint8_t *m1,
    uint8_t *m2,
    uint8_t *m3,
    uint8_t *m4,
    uint8_t *m5);

STATUS_T CSEC_BootDefine(uint32_t bootSize, CSEC_BOOT_MODE_T bootMode);
STATUS_T CSEC_BootOK(void);
STATUS_T CSEC_BootFailure(void);

CSEC_MODULE_STATE_T CSEC_GetModuleState(void);

STATUS_T CSEC_GetUid(
    const uint8_t *challenge,
    uint8_t *uid,
    uint8_t *statusReg,
    uint8_t *macBuf);

STATUS_T CSEC_DebugChallenge(uint8_t *challenge);
STATUS_T CSEC_DebugAuthorization(const uint8_t *authorization);

STATUS_T CSEC_MpCompress(
    const uint8_t *msgBuf,
    uint16_t msgPages,
    uint8_t *compressBuf,
    uint32_t timeout);

STATUS_T CSEC_GetAsyncCommandStatus(void);
void CSEC_CancelCommand(void);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void CSEC_HW_SetInterruptEnable(bool enable);

void CSEC_HW_ReadCmdBytes(uint8_t offset, uint8_t *bytesBuf, uint8_t bytesCount);
uint8_t CSEC_HW_ReadCmdByte(uint8_t offset);
uint32_t CSEC_HW_ReadCmdWord(uint8_t offset);
STATUS_T CSEC_HW_GetCmdResult(void);
void CSEC_HW_WaitCmdComplete(void);

void CSEC_HW_WriteCmdHeader(
    CSEC_CMD_T cmd,
    CSEC_CMD_FORMAT_T cmdFormat,
    CSEC_CALL_SEQUENCE_T callSeq,
    CSEC_KEY_ID_T keyId);

void CSEC_HW_WriteCmdBytes(uint8_t offset, const uint8_t *bytesBuf, uint8_t bytesCount);
void CSEC_HW_WriteCmdByte(uint8_t offset, uint8_t byte);
void CSEC_HW_WriteCmdWords(uint8_t offset, const uint32_t *wordsBuf, uint8_t wordsCount);
void CSEC_HW_WriteCmdHalfWord(uint8_t offset, uint16_t halfWord);

FUNCTION_DECLARATION_AT_RAMSECTION_START
void CSEC_HW_WriteCmdAndWait(
    CSEC_CMD_T cmd,
    CSEC_CMD_FORMAT_T cmdFormat,
    CSEC_CALL_SEQUENCE_T callSeq,
    CSEC_KEY_ID_T keyId)
FUNCTION_DECLARATION_AT_RAMSECTION_END

/**@} end of group CSEC_Functions*/
/**@} end of group CSEC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif

#endif /* APM32F445_446_CSEC_H */
