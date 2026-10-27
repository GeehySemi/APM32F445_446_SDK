/*!
 * @file        main.c
 *
 * @brief       Main program
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
#include "user_config.h"
#include "board.h"
#include "apm32f445_446_csec.h"
#include "csec_utils.h"
#include "csec_test_data.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CSEC_Security
  @{
  */

/** @defgroup CSEC_Security_Macros Macros
  @{
  */

/**
 * This project has three functions:
 *  - TEST_SECURE_BOOT: Used to test Secure Boot functions
 *  - TEST_ENCRYPT_DECRYPT: Used to test encryption/decryption functions
 *  - FLASH_FACTORY_RESET: Rest the Flash to factory state and erase all keys
 *
 * Set the following options to 1 to enable corresponding functions.
 */
#define TEST_SECURE_BOOT        0
#define TEST_ENCRYPT_DECRYPT    1
#define FLASH_FACTORY_RESET     0

/**@} end of group CSEC_Security_Variables*/
/** @defgroup CSEC_Security_Functions Functions
  @{
  */
STATUS_T InitFlashForCsec(void);
STATUS_T TestSecureBoot(void);
STATUS_T TestCescFunctions(void);
void CsecCallback(uint32_t completedCmd, void *callbackParam);
void Print(const char* msg, const uint8_t* buf, uint32_t size);

/*!
 * @brief   Main function
 */
int main(void)
{
    STATUS_T result = STATUS_ERROR;
    CSEC_STATE_T csecState;
    uint8_t uid[15] = {0};

    /* Initialize clock */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr,
                               CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr,
                               CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);

    /* Initialize pins, LEDs and UART1 */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);
    LED_Init();
    COM_Init();
    printf("\r\nCSEC started\r\n");

    /**
     * To enable CSEc functionality, the device must be configured for
     * emulated-EEPROM operation.
     */
    result = InitFlashForCsec();
    if (result != STATUS_SUCCESS)
    {
        LED_On(LED_RED);
        goto end;
    }

    /* Initialize CSEc driver */
    CSEC_Init(&csecState);
    CSEC_InstallCallback(CsecCallback, NULL);
    CSEC_InitRng();
    printf("CSEC driver initialized\r\n");

    /* Get the UID */
    GetUID(uid);
    Print("UID", uid, sizeof(uid));

    /**
     * Load the MASTER_ECU key with a known value, which will be used as
     * authorization key (a secret key known by the application in order to
     * configure other user keys).
     */
    LoadMasterEcuKey();

    /**
     * Load the user key CSEC_KEY_1. We will use it for later encryption and
     * decryption operations. The key counter is 1 as this is the first load.
     * Set boot protection flag to 0.
     */
    printf("Load CSEC_KEY_1\r\n");
    LoadKey(CSEC_KEY_1, g_userKey, 1, 0);

    /**
     * Load a key into the RAM_KEY slot. We will use it for later encryption and
     * decryption operations, we also use it for MAC generation and verification
     * operations.
     */
    printf("Load RAM_KEY\r\n");
    CSEC_LoadPlainKey(g_userKey);

#if TEST_SECURE_BOOT
    /* Test secure boot functions */
    result = TestSecureBoot();
    if (result == STATUS_SUCCESS)
    {
        LED_On(LED_BLUE);
    }
    else
    {
        LED_On(LED_RED);
    }
#endif /* TEST_SECURE_BOOT */

#if TEST_ENCRYPT_DECRYPT
    /* Test encryption, decryption and CMAC functions */
    result = TestCescFunctions();
    if (result == STATUS_SUCCESS)
    {
        LED_On(LED_GREEN);
    }
    else
    {
        LED_On(LED_RED);
    }
#endif /* TEST_ENCRYPT_DECRYPT */

#if FLASH_FACTORY_RESET
    /* Reset the flash to factory state, this will erase all keys */
    printf("Reset flash to factory state and erase all keys!");
    if (FlashFactoryReset())
    {
        printf(" [OK]\r\n");
    }
    else
    {
        printf(" [Failed]\r\n");
    }
    LED_On(LED_GREEN);
#endif /* FLASH_FACTORY_RESET */

end:
    CSEC_DeInit();
    while (1);
}

/*!
 * @brief   The callback function will be called when an asynchronous command
 *          is completed.
 */
void CsecCallback(uint32_t completedCmd, void *callbackParam)
{
}

STATUS_T InitFlashForCsec(void)
{
    STATUS_T result;
    FLASH_SSD_CONFIG_T flashSsdConfig;

    /* Initialize flash */
    printf("Initialize flash for CESC\r\n");
    result = FLASH_Init(&g_flashConfig, &flashSsdConfig);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to initialize flash driver\r\n");
        return result;
    }

    /**
     * Check if the emulated-EEPROM is enabled. If not, enable it now such that
     * we can use the CSEc functionality.
     */
    if (flashSsdConfig.eeeSize == 0)
    {
        printf("EEPROM was not enabled yet, enable it now.\r\n");

        /* Ensure the IFR region is blank before partitioning CFGNVM and CFGRAM */
        FLASH_EraseBlock(&flashSsdConfig, flashSsdConfig.dflashBase);

        uint32_t address = 0x408u;
        uint32_t size = FTFx_PHRASE_SIZE;
        uint8_t unsecureKey[FTFx_PHRASE_SIZE] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu};

        FLASH_Program(&flashSsdConfig, address, size, unsecureKey);

        FLASH_DEFlashPartition(&flashSsdConfig, 0x2, 0x4, 0x3, false, true);
    }
    else
    {
        printf("EEPROM was already enabled. Size=%d bytes\r\n", flashSsdConfig.eeeSize);

    }
    return STATUS_SUCCESS;
}

/*!
* @brief    Print an array of bytes
*/
void Print(const char* msg, const uint8_t* buf, uint32_t size)
{
    printf("%s:", msg);
    for (uint32_t i = 0; i < size; i++)
    {
        printf(" %02X", buf[i]);
    }
    printf("\r\n");
}

/*!
* @brief    Compare two buffers
*/
STATUS_T CompareBuffer(const uint8_t *src, const uint8_t *dest, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (src[i] != dest[i])
        {
            return STATUS_ERROR;
        }
    }
    return STATUS_SUCCESS;
}

STATUS_T TestGenerateRandomNumber(void)
{
    STATUS_T result;
    uint8_t randomNumber1[16] = {0};
    uint8_t randomNumber2[16] = {0};
    uint8_t entropy[16] = {
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};

    printf("Test: Generate random number\r\n");

    result = CSEC_GenerateRandomNumber(randomNumber1);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to generate random number 1!\r\n");
        return result;
    }
    Print("Random Number 1", randomNumber1, sizeof(randomNumber1));

    result = CSEC_ExtendSeed(entropy);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to extend the seed!\r\n");
        return result;
    }

    result = CSEC_GenerateRandomNumber(randomNumber2);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to generate random number 2!\r\n");
        return result;
    }
    Print("Random Number 2", randomNumber2, sizeof(randomNumber2));

    return result;
}

STATUS_T TestEncryptEcbSync(CSEC_KEY_ID_T keyId)
{
    STATUS_T result;
    uint8_t cipherText[ECB_TEXT_SIZE] = {0};

    printf("Test: AES-128 ECB encryption sync\r\n");

    result = CSEC_EncryptEcbSync(keyId,
                                 g_ecbPlainText,
                                 sizeof(g_ecbPlainText),
                                 cipherText,
                                 1);
    if (result == STATUS_SUCCESS)
    {
        /* Check if the output cipher text is expected */
        result = CompareBuffer(cipherText, g_ecbCipherText, sizeof(cipherText));
    }
    return result;
}

STATUS_T TestEncryptEcbAsync(void)
{
    STATUS_T result;
    uint8_t cipherText[ECB_TEXT_SIZE] = {0};

    printf("Test: AES-128 ECB encryption async\r\n");

    result = CSEC_EncryptEcbAsync(CSEC_KEY_1,
                                  g_ecbPlainText,
                                  sizeof(g_ecbPlainText),
                                  cipherText);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        /* Check if the output cipher text is expected */
        result = CompareBuffer(cipherText, g_ecbCipherText, sizeof(cipherText));
    }
    return result;
}

STATUS_T TestDecryptEcbSync(void)
{
    STATUS_T result;
    uint8_t plainText[ECB_TEXT_SIZE] = {0};

    printf("Test: AES-128 ECB decryption sync\r\n");

    result = CSEC_DecryptEcbSync(CSEC_KEY_1,
                                 g_ecbCipherText,
                                 sizeof(g_ecbCipherText),
                                 plainText,
                                 1);
    if (result == STATUS_SUCCESS)
    {
        /* Check if the output plain text is expected */
        result = CompareBuffer(plainText, g_ecbPlainText, sizeof(plainText));
    }
    return result;
}

STATUS_T TestDecryptEcbAsync(void)
{
    STATUS_T result;
    uint8_t plainText[ECB_TEXT_SIZE] = {0};

    printf("Test: AES-128 ECB decryption async\r\n");

    result = CSEC_DecryptEcbAsync(CSEC_KEY_1,
                                  g_ecbCipherText,
                                  sizeof(g_ecbCipherText),
                                  plainText);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        /* Check if the output plain text is expected */
        result = CompareBuffer(plainText, g_ecbPlainText, sizeof(plainText));
    }
    return result;
}

STATUS_T TestEncryptCbcSync(void)
{
    STATUS_T result;
    uint8_t output[CBC_TEXT_SIZE] = {0};

    printf("Test: AES-128 CBC encryption sync\r\n");

    result = CSEC_EncryptCbcSync(CSEC_RAM_KEY,
                                 g_cbcPlainText,
                                 sizeof(g_cbcPlainText),
                                 g_cbcIv,
                                 output,
                                 1);
    if (result == STATUS_SUCCESS)
    {
        /* Check if the output cipher text is expected */
        result = CompareBuffer(output, g_cbcCipherText, sizeof(output));
    }
    return result;
}

STATUS_T TestEncryptCbcAsync(void)
{
    STATUS_T result;
    uint8_t output[CBC_TEXT_SIZE] = {0};

    printf("Test: AES-128 CBC encryption async\r\n");

    result = CSEC_EncryptCbcAsync(CSEC_RAM_KEY,
                                  g_cbcPlainText,
                                  sizeof(g_cbcPlainText),
                                  g_cbcIv,
                                  output);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        /* Check if the output cipher text is expected */
        result = CompareBuffer(output, g_cbcCipherText, sizeof(output));
    }
    return result;
}

STATUS_T TestDecryptCbcSync(void)
{
    STATUS_T result;
    uint8_t output[CBC_TEXT_SIZE] = {0};

    printf("Test: AES-128 CBC decryption sync\r\n");

    result = CSEC_DecryptCbcSync(CSEC_RAM_KEY,
                                 g_cbcCipherText,
                                 sizeof(g_cbcCipherText),
                                 g_cbcIv,
                                 output,
                                 1);
    if (result == STATUS_SUCCESS)
    {
        /* Check if the output plain text is expected */
        result = CompareBuffer(output, g_cbcPlainText, sizeof(output));
    }
    return result;
}

STATUS_T TestDecryptCbcAsync(void)
{
    STATUS_T result;
    uint8_t output[CBC_TEXT_SIZE] = {0};

    printf("Test: AES-128 CBC decryption async\r\n");

    result = CSEC_DecryptCbcAsync(CSEC_RAM_KEY,
                                 g_cbcCipherText,
                                 sizeof(g_cbcCipherText),
                                 g_cbcIv,
                                 output);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        /* Check if the output plain text is expected */
        result = CompareBuffer(output, g_cbcPlainText, sizeof(output));
    }
    return result;
}

STATUS_T TestGenerateMac(void)
{
    STATUS_T result;
    uint8_t generatedMac[MAC_SIZE] = {0};

    printf("Test: Generate CMAC sync\r\n");

    result = CSEC_GenerateMacSync(CSEC_RAM_KEY,
                                  g_macMessage,
                                  sizeof(g_macMessage) * 8, // Number of bits
                                  generatedMac,
                                  1);
    if (result == STATUS_SUCCESS)
    {
        /* Check if the MAC is the one expected */
        result = CompareBuffer(generatedMac, g_macValues, sizeof(generatedMac));
    }
    return result;
}

STATUS_T TestGenerateMacAsync(void)
{
    STATUS_T result;
    uint8_t generatedMac[MAC_SIZE] = {0};

    printf("Test: Generate CMAC async\r\n");

    result = CSEC_GenerateMacAsync(CSEC_RAM_KEY,
                                   g_macMessage,
                                   sizeof(g_macMessage) * 8, // Number of bits
                                   generatedMac);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        /* Check if the MAC is the one expected */
        result = CompareBuffer(generatedMac, g_macValues, sizeof(generatedMac));
    }
    return result;
}

STATUS_T TestVerifyMac(void)
{
    STATUS_T result;
    bool verifyResult = false;

    printf("Test: Veriry CMAC sync\r\n");

    result = CSEC_VerifyMacSync(CSEC_RAM_KEY,
                                g_macMessage,
                                sizeof(g_macMessage) * 8, // Number of bits
                                g_macValues,
                                sizeof(g_macValues) * 8,  // Number of bits
                                &verifyResult,
                                1);
    if (result == STATUS_SUCCESS)
    {
        result = verifyResult ? STATUS_SUCCESS : STATUS_ERROR;
    }
    return result;
}

STATUS_T TestVerifyMacAsync(void)
{
    STATUS_T result;
    bool verifyResult = false;

    printf("Test: Veriry CMAC async\r\n");

    result = CSEC_VerifyMacAsync(CSEC_RAM_KEY,
                                 g_macMessage,
                                 sizeof(g_macMessage) * 8, // Number of bits
                                 g_macValues,
                                 sizeof(g_macValues) * 8,  // Number of bits
                                 &verifyResult);
    if (result == STATUS_SUCCESS)
    {
        /* Wait unitl the command is completed */
        while (CSEC_GetAsyncCommandStatus() == STATUS_BUSY);

        result = verifyResult ? STATUS_SUCCESS : STATUS_ERROR;
    }
    return result;
}

STATUS_T TestCescFunctions(void)
{
    STATUS_T result;

    result = TestGenerateRandomNumber();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestEncryptEcbSync(CSEC_KEY_1);
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestEncryptEcbAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestEncryptCbcSync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestEncryptCbcAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestDecryptEcbSync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestDecryptEcbAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestDecryptCbcSync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestDecryptCbcAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestGenerateMac();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestGenerateMacAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestVerifyMac();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    result = TestVerifyMacAsync();
    if (result != STATUS_SUCCESS)
    {
        printf("Test failed! Status=0x%04X\r\n", result);
        return result;
    }

    printf("All tests were OK!\r\n");
    return STATUS_SUCCESS;
}

STATUS_T ActivateSecureBoot(void)
{
    STATUS_T result;
    uint32_t bootSize = 0x00018000U;

    /* Load the BOOT_MAC_KEY */
    LoadKey(CSEC_BOOT_MAC_KEY, g_userKey, 1, 0);

    /* Enable serial secure boot */
    result = CSEC_BootDefine(bootSize, CSEC_BOOT_MODE_SERIAL);
    if (result != STATUS_SUCCESS)
    {
        return result;
    }

    /**
     * Load the user key CSEC_KEY_2. The key counter is 1 as this is the first
     * load. Set boot protection flag to 1, which means the key will not be
     * available if Secure Boot was failed. We will verify Secure Boot
     * functions with this key.
     */
    LoadKey(CSEC_KEY_2, g_userKey, 1, 1);

    return result;
}

STATUS_T TestSecureBoot(void)
{
    STATUS_T result = STATUS_ERROR;
    CSEC_MODULE_STATE_T moduleState;

    /**
     * Status of the Flash CSEc Status Register. Here we use 4 bits:
     * bit 1: Secure Boot Activated Status
     * bit 2: Secure Boot Initialization Status
     * bit 3: Secure Boot Finished Status
     * bit 4: Secure Boot Successful Flag
     */
    moduleState = CSEC_GetModuleState();
    moduleState &= 0x1E;
    printf("CSEC module state: 0x%02X\r\n", moduleState);

    switch (moduleState)
    {
    case 0x00:
        printf("Secure Boot was not activated, activate it now!\r\n");
        result = ActivateSecureBoot();
        if (result == STATUS_SUCCESS)
        {
            printf("Secure Boot activated successfully, please reset the device!\r\n");
        }
        else
        {
            printf("Failed to activate Secure Boot!\r\n");
        }
        break;
    case 0x08:
        printf("BOOT_MAC_KEY is empty, please load BOOT_MAC_KEY!\r\n");
        break;
    case 0x0E:
        printf("BOOT_MAC is empty, please reset the device again!\r\n");
        printf("The BOOT_MAC will be calculated and stored automatically!\r\n");
        break;
    case 0x0A:
        printf("BOOT_MAC does not match, Secure Boot was failed!\r\n");

        /**
         * Secure Boot was failed, KEY_2 is not available for encryption/decryption.
         * That is, the following test shall be failed.
         */
        result = TestEncryptEcbSync(CSEC_KEY_2);
        if (result == STATUS_SUCCESS)
        {
            printf("Encryption success using CSEC_KEY_2\r\n");
        }
        else
        {
            printf("Encryption failed using CSEC_KEY_2\r\n");
        }
        break;
    case 0x12:
        printf("BOOT_MAC matches, Secure Boot was OK!\r\n");

        /**
         * Secure Boot was OK, KEY_2 shall be available for encryption/decryption.
         * That is, the following test shall be successful.
         */
        result = TestEncryptEcbSync(CSEC_KEY_2);
        if (result == STATUS_SUCCESS)
        {
            printf("Encryption success using CSEC_KEY_2\r\n");
        }
        else
        {
            printf("Encryption failed using CSEC_KEY_2\r\n");
        }

        /**
         * Secure Boot was OK, but we can manually set the failure state by
         * calling CSEC_BootFailure(), such that the KEY_2 will not be available
         * anymore. That is, the following test shall be failed.
         */
        printf("Set boot failure status!\r\n");
        CSEC_BootFailure();
        result = TestEncryptEcbSync(CSEC_KEY_2);
        if (result == STATUS_SUCCESS)
        {
            printf("Encryption success using CSEC_KEY_2\r\n");
        }
        else
        {
            printf("Encryption failed using CSEC_KEY_2\r\n");
        }
        break;
    default:
        break;
    }

    return result;
}

/**@} end of group CSEC_Security_Functions */
/**@} end of group CSEC_Security */
/**@} end of group Examples */
