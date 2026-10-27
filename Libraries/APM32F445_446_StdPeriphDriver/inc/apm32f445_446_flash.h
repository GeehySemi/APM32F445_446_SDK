/*!
 * @file        apm32f445_446_flash.h
 *
 * @brief       This file provides all the FLASH firmware functions
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

#ifndef APM32F445_446_FLASH_H
#define APM32F445_446_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup FLASH_Driver FLASH Driver
  @{
*/

/** @defgroup FLASH_Macros Macros
  @{
*/


/* Word size 2 bytes */
#define FTFx_WORD_SIZE     0x0002U
/* Long word size 4 bytes */
#define FTFx_LONGWORD_SIZE 0x0004U
/* Phrase size 8 bytes */
#define FTFx_PHRASE_SIZE   0x0008U
/* Double-phrase size 16 bytes */
#define FTFx_DPHRASE_SIZE  0x0010U

/*!
 * @brief Operation Commands
 */
#define FTFx_VERIFY_BLOCK             0x00U
#define FTFx_VERIFY_SECTION           0x01U
#define FTFx_PROGRAM_CHECK            0x02U
#define FTFx_READ_RESOURCE            0x03U
#define FTFx_PROGRAM_LONGWORD         0x06U
#define FTFx_PROGRAM_PHRASE           0x07U
#define FTFx_ERASE_BLOCK              0x08U
#define FTFx_ERASE_SECTOR             0x09U
#define FTFx_PROGRAM_SECTION          0x0BU
#define FTFx_VERIFY_ALL_BLOCK         0x40U
#define FTFx_READ_ONCE                0x41U
#define FTFx_PROGRAM_ONCE             0x43U
#define FTFx_ERASE_ALL_BLOCK          0x44U
#define FTFx_SECURITY_BY_PASS         0x45U
#define FTFx_PFLASH_SWAP              0x46U
#define FTFx_ERASE_ALL_BLOCK_UNSECURE 0x49U
#define FTFx_PROGRAM_PARTITION        0x80U
#define FTFx_SET_EERAM                0x81U

/* Resume wait count used in FLASH_EraseResume function */
#define RESUME_WAIT_CNT         0x20U
/* Suspend wait count used in FLASH_EraseSuspend function */
#define SUSPEND_WAIT_CNT        0x40U

#define GET_BIT_0_7(value)   ((uint8_t)(((uint32_t)(value)) & 0xFFU))
#define GET_BIT_8_15(value)  ((uint8_t)((((uint32_t)(value)) >> 8) & 0xFFU))
#define GET_BIT_16_23(value) ((uint8_t)((((uint32_t)(value)) >> 16) & 0xFFU))
#define GET_BIT_24_31(value) ((uint8_t)(((uint32_t)(value)) >> 24))

/* Flash security status */
#define FLASH_SECURITY_STATE_KEYEN     0x80U
#define FLASH_SECURITY_STATE_UNSECURED 0x02U

#define CSE_KEY_SIZE_CODE_MAX 0x03U

/*!
 * @brief Flash security status
 */
/* Flash currently not in secure state */
#define FLASH_NOT_SECURE               0x01U
/* Flash is secured and backdoor key access enabled */
#define FLASH_SECURE_BACKDOOR_ENABLED  0x02U
/* Flash is secured and backdoor key access disabled */
#define FLASH_SECURE_BACKDOOR_DISABLED 0x04U

#ifndef FLASH_CALLBACK_CS
/*! @brief  Callback period count for FlashCheckSum
 *
 * This value is only relevant for FlashCheckSum operation, where a high rate of calling back can impair performance.
 * The rest of the flash operations invoke the callback as often as possible while waiting for the flash controller
 * to finish the requested operation.
 */
#define FLASH_CALLBACK_CS 0x0AU
#endif

/**@} end of group FLASH_Macros*/


/** @defgroup FLASH_Enumerations Enumerations
  @{
*/

/*!
 * @brief CFGRAM Function control Code
 */
typedef enum
{
    EEE_ENABLE                              = 0x00U,    /* Make CFGRAM available for emulated EEPROM */
    EEE_QUICK_WRITE                         = 0x55U,    /* Make CFGRAM available for EEPROM quick writes */
    EEE_STATUS_QUERY                        = 0x77U,    /* EEPROM quick write status query */
    EEE_COMPLETE_INTERRUPT_QUICK_WRITE      = 0xAAU,    /* Complete interrupted EEPROM quick write process */
    EEE_DISABLE                             = 0xFFU,    /* Make CFGRAM available as RAM */
} FLASH_CFGRAM_FUNCTION_CONTROL_CODE_T;


/*! @brief Call back function pointer data type
 *
 *   If using callback in the application, any code reachable from this function
 *   must not be placed in a Flash block targeted for a program/erase operation.
 *   Functions can be placed in RAM section by using the START/END_FUNCTION_DEFINITION/DECLARATION_RAMSECTION macros.
 */
typedef void (* FLASH_CALLBACK_T)(void);

/**@} end of group FLASH_Enumerations*/

/** @defgroup FLASH_Macros Macros
  @{
*/

/*!
 * @brief Null Callback function definition
 */
#define CALLBACK_IS_NULL      ((FLASH_CALLBACK_T)0xFFFFFFFFU)

/**@} end of group FLASH_Macros*/


/** @defgroup FLASH_Structures Structures
  @{
*/

/*!
 * @brief Flash User Configuration Structure
 */
typedef struct
{
    uint32_t pflashBase;         /* The base address of P-Flash memory */
    uint32_t pflashSize;         /* The size in byte of P-Flash memory */
    uint32_t dflashBase;         /* For CFGNVM device, this is the base address of D-Flash memory
                                  * (CFGNVM memory); For non-CFGNVM device, this field is unused */
    uint32_t eeramBase;          /* The base address of CFGRAM (for CFGNVM device)
                                  *    or acceleration RAM memory (for non-CFGNVM device) */
    FLASH_CALLBACK_T callBack;   /* Call back function to service the time critical events. Any code reachable from
                                  * this function must not be placed in a Flash block targeted for a program/erase operation */
} FLASH_USER_CONFIG_T;

/*!
 * @brief Flash SSD Configuration Structure
 *
 * The structure includes the static parameters for C90TFS/FTFx which are
 * device-dependent. The fields including
 * PFlashBlockBase, PFlashBlockSize, DFlashBlockBase, EERAMBlockBase,
 * and callBack are passed via FLASH_USER_CONFIG_T.
 * The rest of parameters such as DFlashBlockSize, and EEEBlockSize will be
 * initialized in FLASH_Init() automatically.
 */
typedef struct
{
    uint32_t pflashBase;       /* The base address of P-Flash memory */
    uint32_t pflashSize;       /* The size in byte of P-Flash memory */
    uint32_t dflashBase;       /* For CFGNVM device, this is the base address of D-Flash memory (CFGNVM memory);
                                *    For non-CFGNVM device, this field is unused */
    uint32_t dflashSize;       /* For CFGNVM device, this is the size in byte of area
                                *    which is used as D-Flash from CFGNVM memory;
                                *    For non-CFGNVM device, this field is unused */
    uint32_t eeramBase;        /* The base address of CFGRAM (for CFGNVM device)
                                *    or acceleration RAM memory (for non-CFGNVM device) */
    uint32_t eeeSize;          /* For CFGNVM device, this is the size in byte of EEPROM area which was partitioned
                                *    from CFGRAM; For non-CFGNVM device, this field is unused */
    FLASH_CALLBACK_T callBack; /* Call back function to service the time critical events. Any code reachable from
                                * this function must not be placed in a Flash block targeted for a program/erase operation */
} FLASH_SSD_CONFIG_T;

/*!
 * @brief EEPROM status structure
 */
typedef struct
{
    uint16_t sectorEraseCount;          /* EEPROM sector erase count */
    uint16_t numOfRecordReqMaintain;    /* Number of EEPROM quick write records requiring maintenance */
    uint8_t brownOutCode;               /* Brown-out detection code */

} FLASH_EEPROM_STATUS_T;

/**@} end of group FLASH_Structures*/

/** @defgroup FLASH_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
void FLASH_DefaultConfig(FLASH_USER_CONFIG_T * const cfg);
void FLASH_ReadDEPartitionCode(FLASH_SSD_CONFIG_T* const ssdCfg, uint8_t *departCode);
STATUS_T FLASH_Init(const FLASH_USER_CONFIG_T* const userConf, FLASH_SSD_CONFIG_T* const ssdCfg);
STATUS_T FLASH_EraseSector(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint32_t size);
STATUS_T FLASH_VerifySection(const FLASH_SSD_CONFIG_T * ssdCfg,
                            uint32_t dest,
                            uint16_t number,
                            uint8_t marginLevel);
STATUS_T FLASH_EraseBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest);
STATUS_T FLASH_VerifyBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint8_t marginLevel);
STATUS_T FLASH_EraseAllBlock(const FLASH_SSD_CONFIG_T * ssdCfg);
STATUS_T FLASH_VerifyAllBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t marginLevel);
void FLASH_EraseSuspend(void);
void FLASH_EraseResume(void);
STATUS_T FLASH_Program(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, const uint8_t *data);
STATUS_T FLASH_ProgramCheck(const FLASH_SSD_CONFIG_T * ssdCfg,
                            uint32_t dest, uint32_t size,
                            const uint8_t *expectedData,
                            uint32_t *failAddr,
                            uint8_t marginLevel);
STATUS_T FLASH_CheckSum(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, uint32_t *sum);
STATUS_T FLASH_ProgramSection(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint16_t number);
STATUS_T FLASH_ProgramOnce(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t recordIndex, const uint8_t * dataArray);
STATUS_T FLASH_ReadOnce(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t recordIndex, uint8_t * dataArray);
void FLASH_ReadPFlashProtection(uint32_t * protectStatus);
STATUS_T FLASH_ConfigPFlashProtection(uint32_t protectStatus);
STATUS_T FLASH_ReadDFlashProtection(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t * protectStatus);
STATUS_T FLASH_ConfigDFlashProtection(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t protectStatus);
STATUS_T FLASH_ReadEERAMProtection(uint8_t * protectStatus);
STATUS_T FLASH_ConfigEERAMProtection(uint8_t protectStatus);
STATUS_T FLASH_DEFlashPartition(const FLASH_SSD_CONFIG_T *ssdCfg,
                                uint8_t eeeDataSizeCode,
                                uint8_t departitionCode,
                                uint8_t csecKeySize,
                                bool uSFE,
                                bool CFGRamLoadEEEData);
STATUS_T FLASH_ConfigCFGRamFunction(const FLASH_SSD_CONFIG_T * ssdCfg,
                                    FLASH_CFGRAM_FUNCTION_CONTROL_CODE_T CFGRamFunctionCode,
                                    uint16_t byteOfQuickWrite,
                                    FLASH_EEPROM_STATUS_T * const eepromStatus);
STATUS_T FLASH_EEEWrite(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, const uint8_t *data);
void FLASH_ReadSecurityState(uint8_t * securityState);
STATUS_T FLASH_SecurityByPass(const FLASH_SSD_CONFIG_T *ssdCfg, const uint8_t *keyBuffer);
STATUS_T FLASH_EraseAllBlockUnsecure(const FLASH_SSD_CONFIG_T * ssdCfg);
STATUS_T FLASH_EnableIntForCmdComplete(void);
void FLASH_DisableIntForCmdComplete(void);
STATUS_T FLASH_EnableIntForReadCollision(void);
void FLASH_DisableIntForReadCollision(void);
STATUS_T FLASH_EnableIntDoubleBitFault(void);
void FLASH_DisableIntDoubleBitFault(void);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
bool FLASH_HW_ReadCmdCompleteFlag(void);
bool FLASH_HW_ReadCollisionFlag(void);
void FLASH_HW_ClearReadCollisionFlag(void);
bool FLASH_HW_ReadDoubleBitFaultFlag(void);
void FLASH_HW_ClearDoubleBitFaultFlag(void);
void FLASH_HW_ForceDoubleBitFaultDetectCmd(bool isEnable);

/**@} end of group FLASH_Functions*/
/**@} end of group FLASH_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/


#endif /* APM32F445_446_FLASH_H */

