/*!
 * @file        apm32f445_446_csec.c
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


/* Includes */
#include "apm32f445_446_csec.h"
#include "osif.h"

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
/* Size of a CSE_PRAM page in bytes */
#define CSEC_PAGE_SIZE_IN_BYTES         (16U)

/**
 * Number of CSE_PRAM pages available for data (excluding the first page, used
 * for command header).
 */
#define CSEC_DATA_PAGES_AVAILABLE       (7U)

/**
 * Number of bytes in CSE_PRAM available for data (excluding the first page,
 * used for command header).
 */
#define CSEC_DATA_BYTES_AVAILABLE       (112U)

/* Shift used to convert CSE_PRAM pages number to/from bytes number */
#define CSEC_BYTES_TO_PAGES_SHIFT       (4U)

/* Shift used to convert CSE_PRAM pages number to/from bits number */
#define CSEC_BYTES_TO_BITS_SHIFT        (3U)

#define CSEC_M1_SIZE_IN_BYTES           (16U)   /* M1 size in bytes */
#define CSEC_M2_SIZE_IN_BYTES           (32U)   /* M2 size in bytes */
#define CSEC_M3_SIZE_IN_BYTES           (16U)   /* M3 size in bytes */
#define CSEC_M4_SIZE_IN_BYTES           (32U)   /* M4 size in bytes */
#define CSEC_M5_SIZE_IN_BYTES           (16U)   /* M5 size in bytes */

#define CSEC_UPPER_HALF_MASK            (0xFFFF0000U)
#define CSEC_UPPER_HALF_SHIFT           (0x10U)
#define CSEC_LOWER_HALF_MASK            (0xFFFFU)
#define CSEC_LOWER_HALF_SHIFT           (0U)

/**@} end of group CSEC_Macros*/

/** @defgroup CSEC_Variables Variables
  @{
*/

/* Pointer to runtime state structure.*/
CSEC_STATE_T *g_csecState = NULL;

/**@} end of group CSEC_Variables*/

/** @defgroup CSEC_Functions Functions
  @{
*/

/*******************************************************************************
 *                      PRIVATE FUNCTION DECLARATIONS
 ******************************************************************************/

static void CSEC_InitState(
    CSEC_KEY_ID_T keyId,
    CSEC_CMD_T cmd,
    const uint8_t *inputData,
    uint8_t *outputData,
    uint32_t totalInputSize);

static void CSEC_StartGenerateMac(void);
static void CSEC_ContinueGenerateMac(void);
static void CSEC_StartVerifyMac(void);
static void CSEC_ContinueVerifyMac(void);
static void CSEC_StartEncryptDecryptCbc(void);
static void CSEC_ContinueEncryptDecryptCbc(void);
static void CSEC_StartEncryptDecryptEcb(void);
static void CSEC_ContinueEncryptDecryptEcb(void);

static uint32_t CSEC_RoundTo(uint32_t value, uint32_t roundTo);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the CESc module
 *
 * @param   states  Pointer to the structure which will be used for holding
 *                  the internal state of the driver
 *
 * @retval  None
 */
void CSEC_Init(CSEC_STATE_T *states)
{
    g_csecState = states;
    g_csecState->isCmdBusy = false;

    INT_SYS_EnableIRQ(FLASH_IRQn);
    OSIF_TimeDelay(0U);
}

/*!
 * @brief   De-initialize the CSEc module
 * @details This function will clear the internal state of the driver and
 *          disable the FLASH interrupt.
 *
 * @retval  None
 */
void CSEC_DeInit(void)
{
    g_csecState = NULL;
    INT_SYS_DisableIRQ(FLASH_IRQn);
}

/*!
 * @brief   Installs a callback function
 * @details The callback function will be called when an asynchronous command
 *          is completed.
 *
 * @param   callback    The function to be invoked.
 * @param   parameter   The parameter to be passed to the callback function
 *
 * @retval  None
 */
void CSEC_InstallCallback(
    CSEC_CALLBACK_T callback,
    void *parameter)
{
    g_csecState->callback = callback;
    g_csecState->callbackParam = parameter;
}

/*!
 * @brief   Initialize the PRNG
 * @details This function initialize and derive a key for the PRNG. It must be
 *          called before the CMD_RND command and after every power cycle or reset.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_InitRng(void)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_INIT_RNG,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Extends the seed of the PRNG
 * @details This function extend the seed of the PRNG by compressing the
 *          former seed value and the supplied entropy into a new seed.
 *          This new seed is then to be used to generate a random number by
 *          invoking the CMD_RND command. The random number generator must be
 *          initialized by CMD_INIT_RNG before the seed may be extended.
 *
 * @param   entropy Pointer to a 128-bit entropy buffer
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_ExtendSeed(const uint8_t *entropy)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the entropy parameter */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          entropy,
                          CSEC_PAGE_SIZE_IN_BYTES);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_EXTEND_SEED,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Generate a vector of 128 random bits
 * @details Return a vector of 128 random bits. The random number generator
 *          must be initialized by calling CSEC_InitRng() before calling this
 *          function.
 *
 * @param   randomNumber    Pointer to a 128-bit buffer where the generated
 *                          random number to be stored
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_GenerateRandomNumber(uint8_t *randomNumber)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_RND,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    /* Read the generated random number */
    if (result == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                             randomNumber,
                             CSEC_PAGE_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Perform AES-128 encryption in CBC mode synchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   plainTextBuf    Pointer to the plain text buffer
 * @param   plainTextSize   Number of bytes of plain text to be encrypted.
 *                          It should be multiple of 16 bytes.
 * @param   iv              Pointer to the initialization vector buffer
 * @param   cipherTextBuf   Pointer to the cipher text buffer. The buffer must
 *                          have the same size as the plain text buffer.
 * @param   timeout         Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_EncryptCbcSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    const uint8_t *iv,
    uint8_t *cipherTextBuf,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId, CSEC_CMD_ENC_CBC, plainTextBuf, cipherTextBuf, plainTextSize);
    g_csecState->iv = iv;

    startTime = OSIF_GetMilliseconds();
    CSEC_StartEncryptDecryptCbc();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueEncryptDecryptCbc();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }
    return g_csecState->cmdStatus;
}

/*!
 * @brief   Perform AES-128 encryption in CBC mode asynchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   plainTextBuf    Pointer to the plain text buffer
 * @param   plainTextSize   Number of bytes of plain text to be encrypted.
 *                          It should be multiple of 16 bytes.
 * @param   iv              Pointer to the initialization vector buffer
 * @param   cipherTextBuf   Pointer to the cipher text buffer. The buffer must
 *                          have the same size as the plain text buffer.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_EncryptCbcAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    const uint8_t *iv,
    uint8_t *cipherTextBuf)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId, CSEC_CMD_ENC_CBC, plainTextBuf, cipherTextBuf, plainTextSize);
    g_csecState->iv = iv;

    CSEC_StartEncryptDecryptCbc();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Perform AES-128 decryption in CBC mode synchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   cipherTextBuf   Pointer to the cipher text buffer
 * @param   cipherTextSize  Number of bytes of cipher text to be decrypted.
 *                          It should be multiple of 16 bytes.
 * @param   iv              Pointer to the initialization vector buffer
 * @param   plainTextBuf    Pointer to the plain text buffer. The buffer must
 *                          have the same size as the cipher text buffer.
 * @param   timeout         Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_DecryptCbcSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    const uint8_t *iv,
    uint8_t *plainTextBuf,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId, CSEC_CMD_DEC_CBC, cipherTextBuf, plainTextBuf, cipherTextSize);
    g_csecState->iv = iv;

    startTime = OSIF_GetMilliseconds();
    CSEC_StartEncryptDecryptCbc();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueEncryptDecryptCbc();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }
    return g_csecState->cmdStatus;
}

/*!
 * @brief   Perform AES-128 decryption in CBC mode asynchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   cipherTextBuf   Pointer to the cipher text buffer
 * @param   cipherTextSize  Number of bytes of cipher text to be decrypted.
 *                          It should be multiple of 16 bytes.
 * @param   iv              Pointer to the initialization vector buffer
 * @param   plainTextBuf    Pointer to the plain text buffer. The buffer must
 *                          have the same size as the cipher text buffer.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_DecryptCbcAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    const uint8_t *iv,
    uint8_t *plainTextBuf)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId, CSEC_CMD_DEC_CBC, cipherTextBuf, plainTextBuf, cipherTextSize);
    g_csecState->iv = iv;

    CSEC_StartEncryptDecryptCbc();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Perform AES-128 encryption in ECB mode synchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   plainTextBuf    Pointer to the plain text buffer
 * @param   plainTextSize   Number of bytes of plain text to be encrypted.
 *                          It should be multiple of 16 bytes.
 * @param   cipherTextBuf   Pointer to the cipher text buffer. The buffer must
 *                          have the same size as the plain text buffer.
 * @param   timeout         Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_EncryptEcbSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    uint8_t *cipherTextBuf,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId, CSEC_CMD_ENC_ECB, plainTextBuf, cipherTextBuf, plainTextSize);

    startTime = OSIF_GetMilliseconds();
    CSEC_StartEncryptDecryptEcb();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueEncryptDecryptEcb();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }
    return g_csecState->cmdStatus;
}

/*!
 * @brief   Perform AES-128 encryption in ECB mode asynchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   plainTextBuf    Pointer to the plain text buffer
 * @param   plainTextSize   Number of bytes of plain text to be encrypted.
 *                          It should be multiple of 16 bytes.
 * @param   iv              Pointer to the initialization vector buffer
 * @param   cipherTextBuf   Pointer to the cipher text buffer. The buffer must
 *                          have the same size as the plain text buffer.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_EncryptEcbAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *plainTextBuf,
    uint32_t plainTextSize,
    uint8_t *cipherTextBuf)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId, CSEC_CMD_ENC_ECB, plainTextBuf, cipherTextBuf, plainTextSize);

    CSEC_StartEncryptDecryptEcb();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Perform AES-128 decryption in ECB mode synchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   cipherTextBuf   Pointer to the cipher text buffer
 * @param   cipherTextSize  Number of bytes of cipher text to be decrypted.
 *                          It should be multiple of 16 bytes.
 * @param   plainTextBuf    Pointer to the plain text buffer. The buffer must
 *                          have the same size as the cipher text buffer.
 * @param   timeout         Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_DecryptEcbSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    uint8_t *plainTextBuf,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId, CSEC_CMD_DEC_ECB, cipherTextBuf, plainTextBuf, cipherTextSize);

    startTime = OSIF_GetMilliseconds();
    CSEC_StartEncryptDecryptEcb();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueEncryptDecryptEcb();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }
    return g_csecState->cmdStatus;
}

/*!
 * @brief   Perform AES-128 decryption in ECB mode asynchronously
 *
 * @param   keyId           The key used to perform the operation
 * @param   cipherTextBuf   Pointer to the cipher text buffer
 * @param   cipherTextSize  Number of bytes of cipher text to be decrypted.
 *                          It should be multiple of 16 bytes.
 * @param   plainTextBuf    Pointer to the plain text buffer. The buffer must
 *                          have the same size as the cipher text buffer.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_DecryptEcbAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *cipherTextBuf,
    uint32_t cipherTextSize,
    uint8_t *plainTextBuf)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId, CSEC_CMD_DEC_ECB, cipherTextBuf, plainTextBuf, cipherTextSize);

    CSEC_StartEncryptDecryptEcb();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Calculate the CMAC with AES-128 of a given message synchronously
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the generated CMAC
 * @param   timeout     Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_GenerateMacSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId,
                   CSEC_CMD_GENERATE_MAC,
                   msgBuf,
                   cmacBuf,
                   CSEC_RoundTo(msgSize, 0x8) >> CSEC_BYTES_TO_BITS_SHIFT);
    g_csecState->msgSize = msgSize;

    startTime = OSIF_GetMilliseconds();
    CSEC_StartGenerateMac();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueGenerateMac();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }
    return g_csecState->cmdStatus;
}

/*!
 * @brief   Calculate the CMAC with AES-128 of a given message asynchronously
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the generated CMAC
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_GenerateMacAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId,
                   CSEC_CMD_GENERATE_MAC,
                   msgBuf,
                   cmacBuf,
                   CSEC_RoundTo(msgSize, 0x8) >> CSEC_BYTES_TO_BITS_SHIFT);
    g_csecState->msgSize = msgSize;

    CSEC_StartGenerateMac();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Calculate the CMAC with AES-128 of a given message
 * @details This function does not involve an extra copy of the data on which
 *          the CMAC is calculated, the message pointer should be a pointer
 *          to the Flash memory.
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer (located in Flash)
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the generated CMAC
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_GenerateMacNoCopy(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    uint8_t *cmacBuf)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the address of the message */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_FLASH_START_ADDRESS_OFFSET,
                          (uint32_t *)&msgBuf,
                          1U);

    /* Write the message size (in bits) */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_MESSAGE_LENGTH_OFFSET, &msgSize, 1U);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdAndWait(CSEC_CMD_GENERATE_MAC,
                            CSEC_CMD_FORMAT_POINTER,
                            CSEC_CALLSEQ_FIRST,
                            keyId);

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    /* Read the generated MAC */
    if (result == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2,
                             cmacBuf,
                             CSEC_PAGE_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Verify the CMAC with AES-128 of a given message synchronously
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the CMAC to be verified
 * @param   cmacSize    Number of bits of the CMAC to be compared. A value of zero
 *                      indicates that all 128-bits are compared.
 * @param   verifyResult    Verification result
 *                          true:   verification passed
 *                          false:  verification failed
 * @param   timeout     Timeout in milliseconds
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_VerifyMacSync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult,
    uint32_t timeout)
{
    uint32_t currentTime = 0;
    uint32_t startTime = 0;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(keyId,
                   CSEC_CMD_VERIFY_MAC,
                   msgBuf,
                   NULL,
                   CSEC_RoundTo(msgSize, 0x8) >> CSEC_BYTES_TO_BITS_SHIFT);
    g_csecState->macBuf = cmacBuf;
    g_csecState->macLen = cmacSize;
    g_csecState->macVerifyResult = verifyResult;
    g_csecState->isMacWritten = false;
    g_csecState->msgSize = msgSize;

    startTime = OSIF_GetMilliseconds();
    CSEC_StartVerifyMac();

    while (g_csecState->isCmdBusy)
    {
        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime <= (startTime + timeout))
        {
            CSEC_ContinueVerifyMac();
        }
        else
        {
            CSEC_CancelCommand();
            g_csecState->cmdStatus = STATUS_TIMEOUT;
            break;
        }
    }

    return g_csecState->cmdStatus;
}

/*!
 * @brief   Verify the CMAC with AES-128 of a given message asynchronously
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the CMAC to be verified
 * @param   cmacSize    Number of bits of the CMAC to be compared. A value of zero
 *                      indicates that all 128-bits are compared.
 * @param   verifyResult    Verification result
 *                          true:   verification passed
 *                          false:  verification failed
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_VerifyMacAsync(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult)
{
    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    CSEC_InitState(keyId,
                   CSEC_CMD_VERIFY_MAC,
                   msgBuf,
                   NULL,
                   CSEC_RoundTo(msgSize, 0x8) >> CSEC_BYTES_TO_BITS_SHIFT);
    g_csecState->msgSize = msgSize;
    g_csecState->macVerifyResult = verifyResult;
    g_csecState->isMacWritten = false;
    g_csecState->macBuf = cmacBuf;
    g_csecState->macLen = cmacSize;

    CSEC_StartVerifyMac();
    CSEC_HW_SetInterruptEnable(true);

    return STATUS_SUCCESS;
}

/*!
 * @brief   Verify the CMAC with AES-128 of a given message
 * @details This function does not involve an extra copy of the data on which
 *          the CMAC is calculated, the message pointer should be a pointer
 *          to the Flash memory.
 *
 * @param   keyId       The key used to perform the operation
 * @param   msgBuf      Pointer to the message buffer (located in Flash)
 * @param   msgSize     Message size in bits
 * @param   cmacBuf     Pointer to the buffer containing the CMAC to be verified
 * @param   cmacSize    Number of bits of the CMAC to be compared. A value of zero
 *                      indicates that all 128-bits are compared.
 * @param   verifyResult    Verification result
 *                          true:   verification passed
 *                          false:  verification failed
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_VerifyMacNoCopy(
    CSEC_KEY_ID_T keyId,
    const uint8_t *msgBuf,
    uint32_t msgSize,
    const uint8_t *cmacBuf,
    uint16_t cmacSize,
    bool *verifyResult)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the address of the message */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_FLASH_START_ADDRESS_OFFSET,
                          (uint32_t *)&msgBuf,
                          1U);

    /* Write the MAC to be verified */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2, cmacBuf, CSEC_PAGE_SIZE_IN_BYTES);

    /* Write the message size (in bits) */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_MESSAGE_LENGTH_OFFSET, &msgSize, 1U);

    /* Write the number of bits of the MAC to be compared */
    CSEC_HW_WriteCmdHalfWord(FEATURE_CSEC_MAC_LENGTH_OFFSET, cmacSize);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdAndWait(CSEC_CMD_VERIFY_MAC,
                            CSEC_CMD_FORMAT_POINTER,
                            CSEC_CALLSEQ_FIRST,
                            keyId);

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    /* Read the MAC verification result */
    if (result == STATUS_SUCCESS)
    {
        uint32_t verifWord = CSEC_HW_ReadCmdWord(FEATURE_CSEC_VERIFICATION_STATUS_OFFSET);
        *verifyResult = ((verifWord & CSEC_UPPER_HALF_MASK) == 0UL);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Updates an internal key
 *
 * @param   keyId   The key to be updated
 * @param   m1      Pointer to the 128-bit M1 message containing the UID,
 *                  Key ID and Authentication Key ID.
 * @param   m2      Pointer to the 256-bit M2 message containing the new
 *                  security flags, counter and the key value all encrypted
 *                  using a derived key generated from the Authentication Key.
 * @param   m3      Pointer to the 128-bit M3 message containing a MAC generated
 *                  over messages M1 and M2.
 * @param   m4      Pointer to a 256 bits buffer where the computed M4 parameter
 *                  is stored.
 * @param   m5      Pointer to a 128 bits buffer where the computed M5 parameter
 *                  is stored.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_LoadKey(
    CSEC_KEY_ID_T keyId,
    const uint8_t *m1,
    const uint8_t *m2,
    const uint8_t *m3,
    uint8_t *m4,
    uint8_t *m5)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the values of M1, M2, M3 */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE4, m3, CSEC_M3_SIZE_IN_BYTES);
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2, m2, CSEC_M2_SIZE_IN_BYTES);
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1, m1, CSEC_M1_SIZE_IN_BYTES);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_LOAD_KEY,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           keyId);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    /* Read the M4 and M5 */
    if (result == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE7, m5, CSEC_M5_SIZE_IN_BYTES);
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE5, m4, CSEC_M4_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Update the RAM key slot with 128-bit plain text
 * @details A plain key can only be loaded into the RAM_KEY slot.
 *
 * @param   plainKey    Pointer to 128-bit buffer containing the plain key
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_LoadPlainKey(const uint8_t *plainKey)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the key */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          plainKey,
                          CSEC_PAGE_SIZE_IN_BYTES);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_LOAD_PLAIN_KEY,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_RAM_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Export the RAM_KEY into a format protected by SECRET_KEY
 *
 * @param   m1  Buffer where the M1 will be exported
 * @param   m2  Buffer where the M2 will be exported
 * @param   m3  Buffer where the M3 will be exported
 * @param   m4  Buffer where the M4 will be exported
 * @param   m5  Buffer where the M5 will be exported
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_ExportRAMKey(
    uint8_t *m1,
    uint8_t *m2,
    uint8_t *m3,
    uint8_t *m4,
    uint8_t *m5)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_EXPORT_RAM_KEY,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_RAM_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();
    if (result == STATUS_SUCCESS)
    {
        /* Read the M1-M5 values associated with the key */
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1, m1, CSEC_M1_SIZE_IN_BYTES);
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2, m2, CSEC_M2_SIZE_IN_BYTES);
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE4, m3, CSEC_M3_SIZE_IN_BYTES);
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE5, m4, CSEC_M4_SIZE_IN_BYTES);
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE7, m5, CSEC_M5_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Define the boot size and boot mode of Secure Boot
 *
 * @param   bootSize    Number of bits to check on boot. Maximum size is 512K Bytes.
 * @param   bootMode    Boot mode
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_BootDefine(uint32_t bootSize, CSEC_BOOT_MODE_T bootMode)
{
    STATUS_T result;
    uint8_t mode = (uint8_t)bootMode;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the boot size and boot mode  */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_BOOT_SIZE_OFFSET, &bootSize, 1U);
    CSEC_HW_WriteCmdByte(FEATURE_CSEC_BOOT_FLAVOR_OFFSET, mode);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_BOOT_DEFINE,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Mark a successful boot verification during later stages of the boot process
 * @details This function is used to mark successful boot verification for later
 *          stages than the autonomous Secure Boot. It may only be called once
 *          after every power cycle/reset and may only be called if Secure Boot
 *          did not detect any errors before and if CSEC_BootFailure() was not
 *          called.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_BootOK(void)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_BOOT_OK,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Mark a failure during later stages of the boot process
 * @details This function will impose the same sanctions as if Secure Boot would
 *          detect a failure, but can be used in later stages of the boot process.
 *          It may only be called once after every power cycle/reset and may
 *          only be called if Secure Boot did not detect any errors before and if
 *          CSEC_BootOK() was not called.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_BootFailure(void)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_BOOT_FAILURE,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Get the state of CSEc module
 *
 * @retval  State of CSEc module
 */
CSEC_MODULE_STATE_T CSEC_GetModuleState(void)
{
    return (FLASH->CSECSTS.reg);
}

/*!
 * @brief   Get the UID
 * @details Get the UID and the value of the status register (protected by a
 *          MAC over a challenge and the data). If the MASTER_ECU_KEY is empty,
 *          the MAC returned value is set to 0.
 *
 * @param   challenge   128-bit buffer containing challenge data
 * @param   uid         120-bit buffer where the UID will be stored
 * @param   statusReg   Value of the status register
 * @param   macBuf      128-bit buffer where the MAC generated over challenge
 *                      and UID and status  will be stored
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_GetUid(
    const uint8_t *challenge,
    uint8_t *uid,
    uint8_t *statusReg,
    uint8_t *macBuf)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the challenge */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1, challenge, CSEC_PAGE_SIZE_IN_BYTES);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_GET_ID,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();
    if (result == STATUS_SUCCESS)
    {
        /* Read the UID */
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2,
                             uid,
                             (uint8_t)(CSEC_PAGE_SIZE_IN_BYTES - 1U));

        /* Read the value of the register */
        *statusReg = CSEC_HW_ReadCmdByte(FEATURE_CSEC_SREG_OFFSET);

        /* Read the MAC over the UID and the SREG */
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE3,
                             macBuf,
                             CSEC_PAGE_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Get a random number which the user shall use along with the
 *          MASTER_ECU_KEY and UID to return an authorization request.
 *
 * @param   challenge 128-bit buffer where the challenge data will be stored
 *
 * @retval  Command execution result
 */

STATUS_T CSEC_DebugChallenge(uint8_t * challenge)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_DBG_CHAL,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    /* Read the generated challenge */
    if (result == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                             challenge,
                             CSEC_PAGE_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Erase all keys if the authorization is confirmed by CSEc
 * @details After CSEc confirms the authentication, the Flash Keys are erased.
 *          If any key has the WRITE_PROTECTION flag set, the command will stop
 *          and not offer any failure analysis mode.
 *
 * @param   authorization   128-bit buffer containing the authorization value
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_DebugAuthorization(const uint8_t *authorization)
{
    STATUS_T result;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    g_csecState->isCmdBusy = true;

    /* Write the authorization computed from the challenge */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          authorization,
                          CSEC_PAGE_SIZE_IN_BYTES);

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(CSEC_CMD_DBG_AUTH,
                           CSEC_CMD_FORMAT_COPY,
                           CSEC_CALLSEQ_FIRST,
                           CSEC_SECRET_KEY);

    /* Wait until the command is completed */
    CSEC_HW_WaitCmdComplete();

    /* Get the result of the command */
    result = CSEC_HW_GetCmdResult();

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Compress the given message by accessing the Miyaguchi-Prenell
 *          compression feature
 *
 * @param   msgBuf      Pointer to the message to be compressed. Messages must
 *                      be pre-processed per SHE specification if they do not
 *                      already meet the full 128-bit block size requirement.
 * @param   msgPages    The number of 128-bit message pages
 * @param   compressBuf 128-bit buffer storing the compressed data
 * @param   timeout     Timeout in milliseconds.
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_MpCompress(
    const uint8_t *msgBuf,
    uint16_t msgPages,
    uint8_t *compressBuf,
    uint32_t timeout)
{
    STATUS_T result = STATUS_SUCCESS;
    uint32_t currentTime = 0;
    uint32_t startTime = 0;
    uint16_t pagesLeft = msgPages;

    if (g_csecState->isCmdBusy)
    {
        return STATUS_BUSY;
    }

    /* Initialize driver internal state */
    CSEC_InitState(CSEC_SECRET_KEY,
                   CSEC_CMD_MP_COMPRESS,
                   msgBuf,
                   compressBuf,
                   (uint32_t)msgPages << CSEC_BYTES_TO_PAGES_SHIFT);

    startTime = OSIF_GetMilliseconds();

    /* Loop and launch commands until the end of the message */
    while (pagesLeft > 0U)
    {
        uint8_t numPages = (uint8_t)((pagesLeft > CSEC_DATA_PAGES_AVAILABLE) ?
                                      CSEC_DATA_PAGES_AVAILABLE : pagesLeft);
        uint8_t numBytes = (uint8_t)(numPages << CSEC_BYTES_TO_PAGES_SHIFT);

        /* Write the message */
        CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                              &msgBuf[g_csecState->inputIndex],
                              numBytes);

        /* Write the size of the message */
        CSEC_HW_WriteCmdHalfWord(FEATURE_CSEC_PAGE_LENGTH_OFFSET, msgPages);

        /* Write command header, which will trigger the command execution. */
        CSEC_HW_WriteCmdHeader(CSEC_CMD_MP_COMPRESS,
                               CSEC_CMD_FORMAT_COPY,
                               g_csecState->callSeq,
                               CSEC_SECRET_KEY);

        /* Wait until the command is completed */
        CSEC_HW_WaitCmdComplete();

        currentTime = OSIF_GetMilliseconds();
        if (currentTime > (startTime + timeout))
        {
            CSEC_CancelCommand();
            result = STATUS_TIMEOUT;
        }

        if (result == STATUS_SUCCESS)
        {
            /* Get the result of the command */
            result = CSEC_HW_GetCmdResult();
        }
        else
        {
            break;
        }

        /* Update the buffer index */
        pagesLeft = (uint16_t)(pagesLeft - numPages);
        g_csecState->inputIndex = (uint32_t)(g_csecState->inputIndex + numBytes);

        if (g_csecState->callSeq == CSEC_CALLSEQ_FIRST)
        {
            g_csecState->callSeq = CSEC_CALLSEQ_FOLLOWING;
        }
    }

    /* Read the result of the compression */
    if (result == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                             compressBuf,
                             CSEC_PAGE_SIZE_IN_BYTES);
    }

    g_csecState->isCmdBusy = false;
    return result;
}

/*!
 * @brief   Get the status of the last asynchronous command
 * @details Return STATUS_BUSY if the command is still in progress.
 *
 * @retval  Status of the last asynchronous command
 */
STATUS_T CSEC_GetAsyncCommandStatus(void)
{
    if (!g_csecState->isCmdBusy)
    {
        return g_csecState->cmdStatus;
    }
    return STATUS_BUSY;
}

/*!
 * @brief   Cancel the last asynchronous command
 *
 * @retval  None
 */
void CSEC_CancelCommand(void)
{
    if (g_csecState->isCmdBusy)
    {
        CSEC_HW_SetInterruptEnable(false);

        /* Wait until the execution of the previous command is complete */
        CSEC_HW_WaitCmdComplete();

        if (   (g_csecState->cmd != CSEC_CMD_ENC_ECB)
            && (g_csecState->cmd != CSEC_CMD_DEC_ECB))
        {
            /* If any command already launched. If so, break the sequence. */
            if (g_csecState->totalInputSize != g_csecState->inputIndex)
            {
                /**
                 * Write the command header. CallSeq is set to 0 in order to
                 * trigger a command that will generate a sequence error,
                 * breaking the chain of calls.
                 */
                CSEC_HW_WriteCmdHeader(g_csecState->cmd,
                                       CSEC_CMD_FORMAT_COPY,
                                       CSEC_CALLSEQ_FIRST,
                                       g_csecState->keyId);

                /* Wait until the command is completed */
                CSEC_HW_WaitCmdComplete();
            }
        }
        g_csecState->isCmdBusy = false;
    }
}

/*!
 * @brief   Implementation of the interrupt handler
 */
void FLASH_IRQHandler(void)
{
    uint8_t sts = (uint8_t)(FLASH->STS.bit.OCIFLG);

    /* Previous command execution ended, continue execution */
    if ((sts != 0U) && g_csecState->isCmdBusy)
    {
        if (g_csecState->cmd == CSEC_CMD_ENC_ECB)
        {
            CSEC_ContinueEncryptDecryptEcb();
        }
        else if (g_csecState->cmd == CSEC_CMD_DEC_ECB)
        {
            CSEC_ContinueEncryptDecryptEcb();
        }
        else if (g_csecState->cmd == CSEC_CMD_ENC_CBC)
        {
            CSEC_ContinueEncryptDecryptCbc();
        }
        else if (g_csecState->cmd == CSEC_CMD_DEC_CBC)
        {
            CSEC_ContinueEncryptDecryptCbc();
        }
        else if (g_csecState->cmd == CSEC_CMD_GENERATE_MAC)
        {
            CSEC_ContinueGenerateMac();
        }
        else if (g_csecState->cmd == CSEC_CMD_VERIFY_MAC)
        {
            CSEC_ContinueVerifyMac();
        }
        else
        {
        }

        /* Disable interrupt if command has finished processing */
        if (!g_csecState->isCmdBusy)
        {
            CSEC_HW_SetInterruptEnable(false);
            if (g_csecState->callback != NULL)
            {
                g_csecState->callback((uint32_t)g_csecState->cmd,
                                      g_csecState->callbackParam);
            }
        }
    }
}

/*******************************************************************************
 *                          PRIVATE FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Initialize the internal state of the driver
 *
 * @param   keyId           KeyID used to perform the cryptographic operation.
 * @param   cmd             The command to be executed
 * @param   inputData       Input data buffer
 * @param   outputData      Output data buffer
 * @param   totalInputSize  Total size of the input data
 */
 void CSEC_InitState(
    CSEC_KEY_ID_T keyId,
    CSEC_CMD_T cmd,
    const uint8_t *inputData,
    uint8_t *outputData,
    uint32_t totalInputSize)
{
    g_csecState->isCmdBusy = true;
    g_csecState->cmd = cmd;
    g_csecState->cmdStatus = STATUS_SUCCESS;
    g_csecState->keyId = keyId;
    g_csecState->callSeq = CSEC_CALLSEQ_FIRST;
    g_csecState->inputData = inputData;
    g_csecState->totalInputSize = totalInputSize;
    g_csecState->inputIndex = 0U;
    g_csecState->outputData = outputData;
}

/*!
 * @brief  Start the generate MAC command
 */
 void CSEC_StartGenerateMac()
{
    uint8_t dataBytes = (uint8_t)(g_csecState->totalInputSize - g_csecState->inputIndex);
    if (dataBytes > CSEC_DATA_BYTES_AVAILABLE)
    {
        dataBytes = CSEC_DATA_BYTES_AVAILABLE;
    }

    /* Write the input data */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          &g_csecState->inputData[g_csecState->inputIndex],
                          dataBytes);

    /* Write the message size (in bits) */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_MESSAGE_LENGTH_OFFSET,
                          &g_csecState->msgSize,
                          1U);
    g_csecState->currentInputSize = dataBytes;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(g_csecState->cmd,
                           CSEC_CMD_FORMAT_COPY,
                           g_csecState->callSeq,
                           g_csecState->keyId);
}

/*!
 * @brief  Continue the generate MAC command
 */
 void CSEC_ContinueGenerateMac(void)
{
    /* Get the result of the command */
    g_csecState->cmdStatus = CSEC_HW_GetCmdResult();

    if (g_csecState->cmdStatus == STATUS_SUCCESS)
    {
        if (g_csecState->callSeq == CSEC_CALLSEQ_FIRST)
        {
            g_csecState->callSeq = CSEC_CALLSEQ_FOLLOWING;
        }

        g_csecState->inputIndex += (uint8_t)g_csecState->currentInputSize;

        /* Check if more commands are needed */
        if (g_csecState->inputIndex < g_csecState->totalInputSize)
        {
            /* Continue launching commands */
            CSEC_StartGenerateMac();
        }
        else
        {
            g_csecState->isCmdBusy = false;
            CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2,
                                 g_csecState->outputData,
                                 CSEC_PAGE_SIZE_IN_BYTES);
        }
    }
    else
    {
        /* Error occurred, stop now */
        g_csecState->isCmdBusy = false;
    }
}

/*!
 * @brief   Start the verify MAC command
 */
 void CSEC_StartVerifyMac()
{
    uint8_t dataBytes = (uint8_t)(g_csecState->totalInputSize - g_csecState->inputIndex);
    if (dataBytes > CSEC_DATA_BYTES_AVAILABLE)
    {
        dataBytes = CSEC_DATA_BYTES_AVAILABLE;
    }
    uint8_t macOffset = (uint8_t)CSEC_RoundTo(dataBytes, 0x10);

    /* Write the input data */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          &g_csecState->inputData[g_csecState->inputIndex],
                          dataBytes);

    /* Write the message size (in bits) */
    CSEC_HW_WriteCmdWords(FEATURE_CSEC_MESSAGE_LENGTH_OFFSET,
                          &g_csecState->msgSize,
                          1U);

    /* Write the MAC size (in bits) to be compared */
    CSEC_HW_WriteCmdHalfWord(FEATURE_CSEC_MAC_LENGTH_OFFSET,
                             (uint16_t)g_csecState->macLen);

    /* Write the MAC if there is available space in CSE_PRAM */
    if ((macOffset + CSEC_PAGE_SIZE_IN_BYTES) <= CSEC_DATA_BYTES_AVAILABLE)
    {
        CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1 + macOffset,
                              g_csecState->macBuf,
                              CSEC_PAGE_SIZE_IN_BYTES);
        g_csecState->isMacWritten = true;
    }
    g_csecState->currentInputSize = dataBytes;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(g_csecState->cmd,
                           CSEC_CMD_FORMAT_COPY,
                           g_csecState->callSeq,
                           g_csecState->keyId);
}

/*!
 * @brief   Continue the verify MAC command
 */
 void CSEC_ContinueVerifyMac(void)
{
    /* Get the result of the command */
    g_csecState->cmdStatus = CSEC_HW_GetCmdResult();

    if (g_csecState->cmdStatus == STATUS_SUCCESS)
    {
        if (g_csecState->callSeq == CSEC_CALLSEQ_FIRST)
        {
            g_csecState->callSeq = CSEC_CALLSEQ_FOLLOWING;
        }

        g_csecState->inputIndex += (uint8_t)g_csecState->currentInputSize;

        /* Check if more commands are needed */
        g_csecState->isCmdBusy = !g_csecState->isMacWritten;

        if (g_csecState->isCmdBusy)
        {
            /* Continue launching commands */
            CSEC_StartVerifyMac();
        }
        else
        {
            uint32_t verifyStatus = CSEC_HW_ReadCmdWord(FEATURE_CSEC_VERIFICATION_STATUS_OFFSET);
            *(g_csecState->macVerifyResult) = ((verifyStatus & CSEC_UPPER_HALF_MASK) == 0UL);
        }
    }
    else
    {
        /* Error occurred, stop now */
        g_csecState->isCmdBusy = false;
    }
}

/*!
 * @brief   Start the encryption/decryption using CBC mode command
 */
 void CSEC_StartEncryptDecryptCbc()
{
    uint32_t pagesLeft = (g_csecState->totalInputSize - g_csecState->inputIndex)
                         >> CSEC_BYTES_TO_PAGES_SHIFT;
    uint16_t pagesCount = (uint16_t)((pagesLeft > CSEC_DATA_PAGES_AVAILABLE) ?
                                    CSEC_DATA_PAGES_AVAILABLE : pagesLeft);
    uint8_t dataBytes = (uint8_t)(pagesCount << CSEC_BYTES_TO_PAGES_SHIFT);

    if (g_csecState->callSeq == CSEC_CALLSEQ_FIRST)
    {
        pagesCount = (uint16_t)((pagesLeft > (CSEC_DATA_PAGES_AVAILABLE - 1U)) ?
                                (CSEC_DATA_PAGES_AVAILABLE - 1U) : pagesLeft);
        dataBytes = (uint8_t)(pagesCount << CSEC_BYTES_TO_PAGES_SHIFT);

        /* Write initialization vector */
        CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                              g_csecState->iv,
                              CSEC_PAGE_SIZE_IN_BYTES);
        /* Write the input data */
        CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2,
                              &g_csecState->inputData[g_csecState->inputIndex],
                              dataBytes);
    }
    else
    {
        /* Write the input data */
        CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                              &g_csecState->inputData[g_csecState->inputIndex],
                              dataBytes);

    }

    /* Write the size of the input data (in pages) */
    CSEC_HW_WriteCmdHalfWord(FEATURE_CSEC_PAGE_LENGTH_OFFSET,
                             (uint16_t)(g_csecState->totalInputSize
                                        >> CSEC_BYTES_TO_PAGES_SHIFT));
    g_csecState->currentInputSize = dataBytes;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(g_csecState->cmd,
                           CSEC_CMD_FORMAT_COPY,
                           g_csecState->callSeq,
                           g_csecState->keyId);
}

/*!
 * @brief   Continue the encryption/decryption using CBC mode command
 */
 void CSEC_ContinueEncryptDecryptCbc(void)
{
    /* Get the result of the command */
    g_csecState->cmdStatus = CSEC_HW_GetCmdResult();

    if (g_csecState->cmdStatus == STATUS_SUCCESS)
    {
        if (g_csecState->callSeq == CSEC_CALLSEQ_FIRST)
        {
            CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE2,
                                 &g_csecState->outputData[g_csecState->inputIndex],
                                 (uint8_t)g_csecState->currentInputSize);
            g_csecState->callSeq = CSEC_CALLSEQ_FOLLOWING;
        }
        else
        {
            CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                                 &g_csecState->outputData[g_csecState->inputIndex],
                                 (uint8_t)g_csecState->currentInputSize);
        }

        g_csecState->inputIndex += (uint8_t)g_csecState->currentInputSize;
        if (g_csecState->inputIndex < g_csecState->totalInputSize)
        {
            /* Continue launching commands */
            CSEC_StartEncryptDecryptCbc();
        }
        else
        {
            g_csecState->isCmdBusy = false;
        }
    }
    else
    {
        /* Error occurred, stop now */
        g_csecState->isCmdBusy = false;
    }
}

/*!
 * @brief   Start the encryption/decryption using ECB mode command
 */
 void CSEC_StartEncryptDecryptEcb(void)
{
    uint32_t pagesLeft = (g_csecState->totalInputSize - g_csecState->inputIndex)
                         >> CSEC_BYTES_TO_PAGES_SHIFT;
    uint16_t pagesCount = (uint16_t)((pagesLeft > CSEC_DATA_PAGES_AVAILABLE) ?
                                     CSEC_DATA_PAGES_AVAILABLE : pagesLeft);
    uint8_t dataBytes = (uint8_t)(pagesCount << CSEC_BYTES_TO_PAGES_SHIFT);

    /* Write the input data */
    CSEC_HW_WriteCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                          &g_csecState->inputData[g_csecState->inputIndex],
                          dataBytes);

    /* Write the size of the input data (in pages) */
    CSEC_HW_WriteCmdHalfWord(FEATURE_CSEC_PAGE_LENGTH_OFFSET,
                             pagesCount);

    g_csecState->currentInputSize = dataBytes;

    /* Write command header, which will trigger the command execution. */
    CSEC_HW_WriteCmdHeader(g_csecState->cmd,
                           CSEC_CMD_FORMAT_COPY,
                           g_csecState->callSeq,
                           g_csecState->keyId);
}

/*!
 * @brief   Continue the encryption/decryption using ECB mode command
 */
 void CSEC_ContinueEncryptDecryptEcb(void)
{
    /* Get the result of the command */
    g_csecState->cmdStatus = CSEC_HW_GetCmdResult();

    if (g_csecState->cmdStatus == STATUS_SUCCESS)
    {
        CSEC_HW_ReadCmdBytes(FEATURE_CSEC_OFFSET_FOR_PAGE1,
                             &g_csecState->outputData[g_csecState->inputIndex],
                             (uint8_t)g_csecState->currentInputSize);

        g_csecState->inputIndex += (uint8_t)g_csecState->currentInputSize;

        /* Check if more commands are needed */
        if (g_csecState->inputIndex < g_csecState->totalInputSize)
        {
            /* Continue launching commands */
            CSEC_StartEncryptDecryptEcb();
        }
        else
        {
            g_csecState->isCmdBusy = false;
        }
    }
    else
    {
        /* Error occurred, stop now */
        g_csecState->isCmdBusy = false;
    }
}

/*!
 * @brief   Round up the value to the first number multiple of roundTo
 */
uint32_t CSEC_RoundTo(uint32_t value, uint32_t roundTo)
{
    return (value + (roundTo - 1U)) & ~(roundTo - 1U);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief   Enable or disable the command completion interrupt
 *
 * @param   enable  Enable or disable
 *
 * @retval  None
 */
void CSEC_HW_SetInterruptEnable(bool enable)
{
    FLASH->CFG.bit.CMDCIEN = (enable ? 1U : 0U);
}

/*!
 * @brief   Read command bytes from CSE_PRAM from a 32-bit aligned offset
 *
 * @param   offset      Offset (in bytes) from which the bytes shall be read
 * @param   bytesBuf    Buffer containing the bytes read
 * @param   bytesCount  Number of bytes to be read
 *
 * @retval  None
 */
void CSEC_HW_ReadCmdBytes(uint8_t offset, uint8_t *bytesBuf, uint8_t bytesCount)
{
    uint8_t i = 0U;

    while ((i + 3U) < bytesCount)
    {
        uint32_t tmp = CSE_PRAM->RAMn[(offset + i) >> 2U].DATA_32;

        bytesBuf[i]      = (uint8_t)((tmp & 0xFF000000u) >> 24u);
        bytesBuf[i + 1U] = (uint8_t)((tmp & 0xFF0000u) >> 16u);
        bytesBuf[i + 2U] = (uint8_t)((tmp & 0xFF00u) >> 8u);
        bytesBuf[i + 3U] = (uint8_t)((tmp & 0xFFu) >> 0u);
        i = (uint8_t)(i + 4U);
    }

    while (i < bytesCount)
    {
        bytesBuf[i] = CSEC_HW_ReadCmdByte(offset + i);
        i++;
    }
}

/*!
 * @brief   Read one byte from CSE_PRAM
 *
 * @param   offset  Offset (in bytes) from which the byte shall be read
 *
 * @retval  The byte read
 */
uint8_t CSEC_HW_ReadCmdByte(uint8_t offset)
{
    uint8_t byte = 0;
    uint32_t tmp = 0U;

    tmp = CSE_PRAM->RAMn[offset >> 2U].DATA_32;

    if ((offset & 0x3U) == 0x0U)
    {
        byte = (uint8_t)((tmp & 0xFF000000u) >> 24u);
    }
    else if ((offset & 0x3U) == 0x1U)
    {
        byte = (uint8_t)((tmp & 0xFF0000u) >> 16u);
    }
    else if ((offset & 0x3U) == 0x2U)
    {
        byte = (uint8_t)((tmp & 0xFF00u) >> 8u);
    }
    else if ((offset & 0x3U) == 0x3U)
    {
        byte = (uint8_t)((tmp & 0xFFu) >> 0u);
    }
    else
    {
    }

    return byte;
}

/*!
 * @brief   Read a command word from CSE_PRAM from a 32-bit aligned offset
 *
 * @param   offset  Offset (in bytes) from which the word shall be read
 *
 * @retval  The word read
 */
uint32_t CSEC_HW_ReadCmdWord(uint8_t offset)
{
    return CSE_PRAM->RAMn[offset >> 2U].DATA_32;
}

/*!
 * @brief   Get the error bits reported after running a CSEc command
 *
 * @param   None
 *
 * @retval  Command execution result
 */
STATUS_T CSEC_HW_GetCmdResult(void)
{
    uint32_t errorWord;
    uint16_t errorBits;
    STATUS_T errorStatus;

    errorWord = CSEC_HW_ReadCmdWord(FEATURE_CSEC_ERROR_BITS_OFFSET);
    errorBits = (uint16_t)((errorWord & CSEC_UPPER_HALF_MASK) >> CSEC_UPPER_HALF_SHIFT);

    if (errorBits == CSEC_ERC_NO_ERROR)
    {
        errorStatus = STATUS_SUCCESS;
    }
    else if (errorBits == CSEC_ERC_SEQUENCE_ERROR)
    {
        errorStatus = STATUS_CSEC_SEQUENCE_ERROR;
    }
    else if (errorBits == CSEC_ERC_KEY_NOT_AVAILABLE)
    {
        errorStatus = STATUS_CSEC_KEY_NOT_AVAILABLE;
    }
    else if (errorBits == CSEC_ERC_KEY_INVALID)
    {
        errorStatus = STATUS_CSEC_KEY_INVALID;
    }
    else if (errorBits == CSEC_ERC_KEY_EMPTY)
    {
        errorStatus = STATUS_CSEC_KEY_EMPTY;
    }
    else if (errorBits == CSEC_ERC_NO_SECURE_BOOT)
    {
        errorStatus = STATUS_CSEC_NO_SECURE_BOOT;
    }
    else if (errorBits == CSEC_ERC_KEY_WRITE_PROTECTED)
    {
        errorStatus = STATUS_CSEC_KEY_WRITE_PROTECTED;
    }
    else if (errorBits == CSEC_ERC_KEY_UPDATE_ERROR)
    {
        errorStatus = STATUS_CSEC_KEY_UPDATE_ERROR;
    }
    else if (errorBits == CSEC_ERC_RNG_SEED)
    {
        errorStatus = STATUS_CSEC_RNG_SEED;
    }
    else if (errorBits == CSEC_ERC_NO_DEBUGGING)
    {
        errorStatus = STATUS_CSEC_NO_DEBUGGING;
    }
    else if (errorBits == CSEC_ERC_MEMORY_FAILURE)
    {
        errorStatus = STATUS_CSEC_MEMORY_FAILURE;
    }
    else if (errorBits == CSEC_ERC_GENERAL_ERROR)
    {
        errorStatus = STATUS_ERROR;
    }
    else
    {
        errorStatus = STATUS_ERROR;
    }
    return errorStatus;
}

/*!
 * @brief   Wait until the command is completed
 */
void CSEC_HW_WaitCmdComplete(void)
{
    /* Wait until the operation completed flag is set */
    while ((FLASH->STS.bit.OCIFLG) == 0U) {}
}

/*!
 * @brief   Write command header which will trigger the command execution
 *
 * @param   cmd         The command to be started
 * @param   cmdFormat   Command format
 * @param   callSeq     Call sequence
 * @param   keyId       The key to be used by the command
 *
 * @retval  None
 */
void CSEC_HW_WriteCmdHeader(
    CSEC_CMD_T cmd,
    CSEC_CMD_FORMAT_T cmdFormat,
    CSEC_CALL_SEQUENCE_T callSeq,
    CSEC_KEY_ID_T keyId)
{
    CSE_PRAM->RAMn[0].DATA_32 =
        (((uint32_t)(((uint32_t)(cmd)) << 24u)) & 0xFF000000u) |
        (((uint32_t)(((uint32_t)(cmdFormat)) << 16u)) & 0xFF0000u) |
        (((uint32_t)(((uint32_t)(callSeq)) << 8u)) & 0xFF00u) |
        (((uint32_t)(((uint32_t)(keyId)) << 0u)) & 0xFFu);
}

/*!
 * @brief   Write command bytes to CSE_PRAM at a 32-bit aligned offset
 *
 * @param   offset      Offset (in bytes) at which the bytes shall be written
 * @param   bytesBuf    Buffer containing the bytes to be written
 * @param   bytesCount  Number of bytes to be written
 *
 * @retval  None
 */
void CSEC_HW_WriteCmdBytes(uint8_t offset, const uint8_t *bytesBuf, uint8_t bytesCount)
{
    uint8_t i = 0U;

    while ((i + 3U) < bytesCount)
    {
        CSE_PRAM->RAMn[(offset + i) >> 2U].DATA_32 =
            (((uint32_t)(((uint32_t)(bytesBuf[i])) << 24u)) & 0xFF000000u) |
            (((uint32_t)(((uint32_t)(bytesBuf[i + 1U])) << 16u)) & 0xFF0000u) |
            (((uint32_t)(((uint32_t)(bytesBuf[i + 2U])) << 8u)) & 0xFF00u) |
            (((uint32_t)(((uint32_t)(bytesBuf[i + 3U])) << 0u)) & 0xFFu);
        i = (uint8_t)(i + 4U);
    }

    while (i < bytesCount)
    {
        CSEC_HW_WriteCmdByte(offset + i, bytesBuf[i]);
        i++;
    }
}

/*!
 * @brief   Write one byte to CSE_PRAM
 *
 * @param   offset  Offset (in bytes) at which the byte shall be written
 *
 * @retval  None
 */
void CSEC_HW_WriteCmdByte(uint8_t offset, uint8_t byte)
{
    uint32_t tmp = CSE_PRAM->RAMn[offset >> 2U].DATA_32;

    if ((offset & 0x3U) == 0x0U)
    {
        tmp = tmp & ~0xFF000000u;
        tmp = tmp | ((((uint32_t)(byte) << 24u)) & 0xFF000000u);
    }
    else if ((offset & 0x3U) == 0x1U)
    {
        tmp = tmp & ~0xFF0000u;
        tmp = tmp | ((((uint32_t)(byte) << 16u)) & 0xFF0000u);
    }
    else if ((offset & 0x3U) == 0x2U)
    {
        tmp = tmp & ~0xFF00u;
        tmp = tmp | ((((uint32_t)(byte) << 8u)) & 0xFF00u);
    }
    else if ((offset & 0x3U) == 0x3U)
    {
        tmp = tmp & ~0xFFu;
        tmp = tmp | ((((uint32_t)(byte) << 0u)) & 0xFFu);
    }
    else
    {
    }
    CSE_PRAM->RAMn[offset >> 2U].DATA_32 = tmp;
}

/*!
 * @brief   Write command words to CSE_PRAM at a 32-bit aligned offset
 *
 * @param   offset      Offset (in bytes) at which the words shall be written
 * @param   wordsBuf    Buffer containing the words to be written
 * @param   wordsCount  Number of words to be written
 *
 * @retval  None
 */
void CSEC_HW_WriteCmdWords(uint8_t offset, const uint32_t *wordsBuf, uint8_t wordsCount)
{
    uint8_t i = 0U;
    uint8_t offsetAligned = (uint8_t)(offset >> 2U);

    while (i < wordsCount)
    {
        CSE_PRAM->RAMn[offsetAligned + i].DATA_32 = wordsBuf[i];
        i++;
    }
}

/*!
 * @brief   Writes a command half word to CSE_PRAM at a 16-bit aligned offset
 *
 * @param   offset      Offset (in bytes) at which the half word shall be written
 * @param   halfWord    The half word to be written
 *
 * @retval  None
 */
void CSEC_HW_WriteCmdHalfWord(uint8_t offset, uint16_t halfWord)
{
    uint32_t tmp = CSE_PRAM->RAMn[(offset >> 2U)].DATA_32;

    if ((offset & 2U) != 0U)
    {
        tmp = tmp & ~CSEC_LOWER_HALF_MASK;
        tmp = tmp | ((((uint32_t) halfWord) << CSEC_LOWER_HALF_SHIFT)
                     & CSEC_LOWER_HALF_MASK);
    }
    else
    {
        tmp = tmp & ~CSEC_UPPER_HALF_MASK;
        tmp = tmp | ((((uint32_t) halfWord) << CSEC_UPPER_HALF_SHIFT)
                     & CSEC_UPPER_HALF_MASK);
    }

    CSE_PRAM->RAMn[(offset >> 2U)].DATA_32 = tmp;
}

/*!
 * @brief   Write the command header to CSE_PRAM and wait for completion
 * @details This function is always located in RAM, and is used for CSEc
 *          commands using pointer methods, in order to allow reading from
 *          Flash without causing a read collision.
 *
 * @param   cmd         The command to be started
 * @param   cmdFormat   Command format
 * @param   callSeq     Call sequence
 * @param   keyId       The key to be used by the command
 *
 * @retval  None
 */
FUNCTION_DEFINITION_AT_RAMSECTION_START
void CSEC_HW_WriteCmdAndWait(
    CSEC_CMD_T cmd,
    CSEC_CMD_FORMAT_T cmdFormat,
    CSEC_CALL_SEQUENCE_T callSeq,
    CSEC_KEY_ID_T keyId)
{
    CSE_PRAM->RAMn[0].DATA_32 =
        (((uint32_t)(((uint32_t)(cmd)) << 24u)) & 0xFF000000u) |
        (((uint32_t)(((uint32_t)(cmdFormat)) << 16u)) & 0xFF0000u) |
        (((uint32_t)(((uint32_t)(callSeq)) << 8u)) & 0xFF00u) |
        (((uint32_t)(((uint32_t)(keyId)) << 0u)) & 0xFFu);

    /* Wait until the operation completed flag is set */
    while ((FLASH->STS.bit.OCIFLG) == 0U) {}
}
FUNCTION_DEFINITION_AT_RAMSECTION_END

/**@} end of group CSEC_Functions*/
/**@} end of group CSEC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
