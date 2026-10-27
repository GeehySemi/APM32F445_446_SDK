/*!
 * @file        csec_utils.c
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

#include <stdint.h>
#include <stdbool.h>
#include "csec_utils.h"
#include "csec_test_data.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CSEC_Security
  @{
*/

/* Constants defined by the SHE spec */
static uint8_t KEY_UPDATE_ENC_C[16] = {
    0x01, 0x01, 0x53, 0x48, 0x45, 0x00, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0};

static uint8_t KEY_UPDATE_MAC_C[16] = {
    0x01, 0x02, 0x53, 0x48, 0x45, 0x00, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0};

static uint8_t KEY_DEBUG_KEY_C[16] = {
    0x01, 0x03, 0x53, 0x48, 0x45, 0x00, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0};

/** @defgroup CSEC_Security_Functions Functions
  @{
*/
/* Derive a key with a given constant */
STATUS_T DeriveKey(const uint8_t *key, uint8_t *constant, uint8_t *derivedKey)
{
    uint8_t concat[32];

    for (int i = 0; i < 16; i++)
    {
        concat[i] = key[i];
        concat[i+16] = constant[i];
    }

    return CSEC_MpCompress(concat, 2U, derivedKey, 1U);
}

/* Calculate the M1-M3 values */
STATUS_T CalculateM1M2M3(
    uint8_t *authKey,
    CSEC_KEY_ID_T authId,
    CSEC_KEY_ID_T keyId,
    const uint8_t *key,
    uint32_t counter,
    uint8_t *uid,
    uint8_t *m1,
    uint8_t *m2,
    uint8_t *m3,
    bool bootProtection)
{
    STATUS_T stat;
    int i;
    uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t k1[16];
    uint8_t k2[16];
    uint8_t m2Plain[32];
    uint8_t m1m2[48];

    /* Derive K1 and K2 from AuthID */
    DeriveKey(authKey, KEY_UPDATE_ENC_C, k1);
    DeriveKey(authKey, KEY_UPDATE_MAC_C, k2);

    /* Calculate M1 = UID | ID | AuthID */
    for (i = 0; i < 15; i++)
    {
        m1[i] = uid[i];
    }
    m1[15] = ((keyId & 0xF) << 4) | (authId & 0xF);

    /* Calculate M2 (C = counter, F = 0) */
    for(i = 0; i < 16; i++)
    {
        m2Plain[i] = 0;
        m2Plain[16 + i] = key[i];
    }
    m2Plain[0] = (counter & 0xFF00000) >> 20;
    m2Plain[1] = (counter & 0xFF000) >> 12;
    m2Plain[2] = (counter & 0xFF0) >> 4;
    if(!bootProtection)
    {
        m2Plain[3] = (counter & 0xF) << 4;
    }
    else
    {
        m2Plain[3] = ((counter & 0xF) << 4) | 0x4;
    }

    /* Encrypt M2 */
    stat = CSEC_LoadPlainKey(k1);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = CSEC_EncryptCbcSync(CSEC_RAM_KEY, m2Plain, 32U, iv, m2, 1U);
    if (stat != STATUS_SUCCESS)
        return stat;

    /* Calculate M3 as CMAC(key=k2, m1|m2) */
    for (i = 0; i < 16; i++)
    {
        m1m2[i] = m1[i];
    }
    for(i = 0; i < 32; i++)
    {
        m1m2[16 + i] = m2[i];
    }

    stat = CSEC_LoadPlainKey(k2);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = CSEC_GenerateMacSync(CSEC_RAM_KEY, m1m2, 384U, m3, 1U);
    if (stat != STATUS_SUCCESS)
        return stat;

    return STATUS_SUCCESS;
}

/* Calculate the M4 and M5 values */
STATUS_T CalculateM4M5(
    CSEC_KEY_ID_T authId,
    CSEC_KEY_ID_T keyId,
    const uint8_t *key,
    uint32_t counter,
    uint8_t *uid,
    uint8_t *m4,
    uint8_t *m5)
{
    STATUS_T stat;
    int i;
    uint8_t k3[16];
    uint8_t k4[16];
    uint8_t m4StarPlain[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t m4StarCipher[16];

    /* Derive K4 and K5 from key ID */
    DeriveKey(key, KEY_UPDATE_ENC_C, k3);
    DeriveKey(key, KEY_UPDATE_MAC_C, k4);

    m4StarPlain[0] = (counter & 0xFF00000) >> 20;
    m4StarPlain[1] = (counter & 0xFF000) >> 12;
    m4StarPlain[2] = (counter & 0xFF0) >> 4;
    m4StarPlain[3] = ((counter & 0xF) << 4) | 0x8;

    /* Encrypt M4* */
    stat = CSEC_LoadPlainKey(k3);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = CSEC_EncryptEcbSync(CSEC_RAM_KEY, m4StarPlain, 16U, m4StarCipher, 1U);
    if (stat != STATUS_SUCCESS)
        return stat;

    /* Calculate M4 = UID | ID | AuthID | M4* */
    for (i = 0; i < 15; i++)
    {
        m4[i] = uid[i];
    }
    m4[15] = ((keyId & 0xF) << 4) | (authId & 0xF);
    for (i = 0; i < 16; i++)
    {
        m4[16 + i] = m4StarCipher[i];
    }

    stat = CSEC_LoadPlainKey(k4);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = CSEC_GenerateMacSync(CSEC_RAM_KEY, m4, 256U, m5, 1U);
    if (stat != STATUS_SUCCESS)
        return stat;

    return STATUS_SUCCESS;
}

/* Set the AuthID key (MASTER_ECU_KEY) for the first time */
bool LoadMasterEcuKey(void)
{
    uint8_t uid[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t m1[16];
    uint8_t m2[32];
    uint8_t m3[16];
    uint8_t m4[32];
    uint8_t m5[16];

    STATUS_T stat;

    stat = CalculateM1M2M3(g_emptyKey,
                           CSEC_MASTER_ECU,
                           CSEC_MASTER_ECU,
                           g_masterEcuKey,
                           1,
                           uid,
                           m1,
                           m2,
                           m3,
                           0);
    if (stat != STATUS_SUCCESS)
        return false;

    stat = CSEC_LoadKey(CSEC_MASTER_ECU, m1, m2, m3, m4, m5);
    if (stat != STATUS_SUCCESS)
        return false;

    return true;
}

/* Get the UID */
STATUS_T GetUID(uint8_t *uid)
{
    STATUS_T stat;
    uint8_t challenge[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t sreg;
    uint8_t mac[16];
    uint8_t verif[32];
    bool verifyResult = false;
    uint8_t i;

    stat = CSEC_GetUid(challenge, uid, &sreg, mac);
    if (stat != STATUS_SUCCESS)
        return stat;

    for (i = 0; i < 16; i++)
    {
        verif[i] = challenge[i];
    }
    for (i = 0; i < 15; i++)
    {
        verif[16 + i] = uid[i];
    }
    verif[31] = CSEC_GetModuleState() | CSEC_STATE_BUSY;

    stat = CSEC_LoadPlainKey(g_masterEcuKey);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = CSEC_VerifyMacSync(CSEC_RAM_KEY, verif, 256U, mac, 128U, &verifyResult, 1U);
    if (stat != STATUS_SUCCESS)
        return stat;

    stat = verifyResult ? STATUS_SUCCESS : STATUS_ERROR;
    return stat;
}

/* Erases all the keys. */
bool FlashFactoryReset(void)
{
    STATUS_T stat;
    uint8_t challenge[16];
    uint8_t auth[16];
    uint8_t authPlain[31];
    uint8_t k[16];
    uint8_t uid[15];

    uint8_t i;

    CSEC_InitRng();
    GetUID(uid);

    DeriveKey(g_masterEcuKey, KEY_DEBUG_KEY_C, k);

    stat = CSEC_LoadPlainKey(k);
    if (stat != STATUS_SUCCESS)
        return false;

    stat = CSEC_DebugChallenge(challenge);
    if (stat != STATUS_SUCCESS)
        return false;

    for (i = 0; i < 16; i++)
    {
        authPlain[i] = challenge[i];
    }
    for (i = 0; i < 15; i++)
    {
        authPlain[i + 16] = uid[i];
    }

    stat = CSEC_GenerateMacSync(CSEC_RAM_KEY, authPlain, 248U, auth, 1U);
    if (stat != STATUS_SUCCESS)
        return false;

    stat = CSEC_DebugAuthorization(auth);
    if (stat != STATUS_SUCCESS)
        return false;

    return true;
}

/* Load or update a non-volatile key */
bool LoadKey(CSEC_KEY_ID_T keyId, uint8_t *newKey, uint8_t counter, bool bootProtection)
{
    uint8_t uid[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t m1[16];
    uint8_t m2[32];
    uint8_t m3[16];
    uint8_t m4[32];
    uint8_t m5[16];

    STATUS_T stat;

    stat = CalculateM1M2M3(g_masterEcuKey,
                           CSEC_MASTER_ECU,
                           keyId,
                           newKey,
                           counter,
                           uid,
                           m1,
                           m2,
                           m3,
                           bootProtection);
    if (stat != STATUS_SUCCESS)
        return false;

    stat = CSEC_LoadKey(keyId, m1, m2, m3, m4, m5);
    if (stat != STATUS_SUCCESS)
        return false;

    return true;
}

/**@} end of group CSEC_Security_Functions*/
/**@} end of group CSEC_Security*/
/**@} end of group Examples*/

