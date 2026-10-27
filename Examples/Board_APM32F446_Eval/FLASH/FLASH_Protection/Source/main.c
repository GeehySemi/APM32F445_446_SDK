/*!
 * @file        main.c
 *
 * @brief       This file provides all the function of this demo.
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

/* Included */
#include "user_config.h"
#include "apm32f445_446_interrupt.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup FLASH_Protection
  @{
*/

/** @defgroup FLASH_Protection_Macros Macros
  @{
*/
/* Indicates the protected region's type, which CFGNVM work as dflash or eeprom*/
#define DFLASH_PROTECTED        1U

/**@} end of group FLASH_Protection_Variables*/

/** @defgroup FLASH_Protection_Variables Variables
  @{
*/
FLASH_SSD_CONFIG_T flashSSDConfig;

/* Data source for program operation */
uint8_t sourceBuffer[8];

/**@} end of group FLASH_Protection_Variables*/

/** @defgroup FLASH_Protection_Functions Functions
  @{
*/
static void PFlashProtect(void);
static void DFlashProtect(void);
static void EERAMProtect(void);

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None.
 */
int main(void)
{
    uint32_t i;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    COM_Init();
    printf("COM_Init OK\r\n");

    /* Init source data */
    for (i = 0U; i < 8U; i++)
    {
        sourceBuffer[i] = i;
    }

    /* Disable cache */
    MSC->OCMD0.reg |= (uint32_t)((0x3U<<4)&0x30U);
    MSC->OCMD1.reg |= (uint32_t)((0x3U<<4)&0x30U);

    INT_SYS_DisableIRQGlobal();

    FLASH_Init(&g_flashConfig, &flashSSDConfig);

    PFlashProtect();
    if(DFLASH_PROTECTED == 1U)
    {
        DFlashProtect();
    }
    else
    {
        EERAMProtect();
    }

    INT_SYS_EnableIRQGlobal();

    while(1)
    {}
}

/*!
 * @brief     PFlash Protect
 *
 * @param     None
 *
 * @retval    None.
 */
static void PFlashProtect(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t address;
    uint32_t size;
    uint32_t pflashProtectValue;
    printf("********************************************\r\n");
    /* Config PFlash Region 31 is Protected */
    returnStatus = FLASH_ConfigPFlashProtection(0x7fffffff);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Config PFlash Region 31 is Protected Successful\r\n");
    }
    else
    {
        printf("Config PFlash Region 31 is Protected Failed\r\n");
    }

    FLASH_ReadPFlashProtection(&pflashProtectValue);
    printf("PFlash ProtectValue is 0x%08x\r\n",pflashProtectValue);

    if (returnStatus == STATUS_SUCCESS)
    {
        /* Program data to Region 31 */
        address = 0x0007FFF0U;
        size = 8U;
        returnStatus = FLASH_Program(&flashSSDConfig, address, size, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Program PFlash data Successful\r\n");
        }
        else
        {
            printf("Program PFlash data Failed\r\n");
            if ((FLASH->STS.reg & 0x10U) != 0U)
            {
               printf("PFlash is Protected\r\n");
            }
        }
    }
    printf("********************************************\r\n");
}

/*!
 * @brief     DFlash Protect
 *
 * @param     None
 *
 * @retval    None.
 */
static void DFlashProtect(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t address;
    uint32_t size;
    uint8_t dflashProtectValue;

    printf("********************************************\r\n");
    /* Config DFlash Region 0 is Protected */
    returnStatus = FLASH_ConfigDFlashProtection(&flashSSDConfig, 0xfe);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Config DFlash Region 0 is Protected Successful\r\n");
    }
    else if (returnStatus == STATUS_UNSUPPORTED)
    {
        printf("DFlashSize is 0x%04x\r\n",flashSSDConfig.dflashSize);
        printf("Config DFlash Region 0 is Protected Failed\r\n");
    }
    else
    {
        printf("Config DFlash Region 0 is Protected Failed\r\n");
    }

    FLASH_ReadDFlashProtection(&flashSSDConfig, &dflashProtectValue);
    printf("DFlash ProtectValue is 0x%02x\r\n",dflashProtectValue);

    if (returnStatus == STATUS_SUCCESS)
    {
        /* Program data to Region 0 */
        address = 0x10000000U;
        size = 8U;
        returnStatus = FLASH_Program(&flashSSDConfig, address, size, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Program DFlash data Successful\r\n");
        }
        else
        {
            printf("Program DFlash data Failed\r\n");
            if ((FLASH->STS.reg & 0x10U) != 0U)
            {
               printf("DFlash is Protected\r\n");
            }
        }
    }
    printf("********************************************\r\n");
}

/*!
 * @brief     EERAM Protect
 *
 * @param     None
 *
 * @retval    None.
 */
static void EERAMProtect(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t address;
    uint32_t size;
    uint8_t EERamProtectValue;

    printf("********************************************\r\n");
    if (flashSSDConfig.eeeSize == 0u)
    {
         /* Make sure that the IFR region is blank */
        returnStatus = FLASH_EraseBlock(&flashSSDConfig, flashSSDConfig.dflashBase);

        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Erase DFlash Block Successful\r\n");

            /* Verify the erase operation */
            returnStatus = FLASH_VerifyBlock(&flashSSDConfig, flashSSDConfig.dflashBase, 1u);

            if (returnStatus == STATUS_SUCCESS)
            {
                printf("Verify DFlash Block Successful\r\n");
            }
            else
            {
                printf("Verify DFlash Block Failed\r\n");
            }
        }
        else
        {
            printf("Erase DFlash Block Failed\r\n");
        }

        if (returnStatus == STATUS_SUCCESS)
        {
            if (returnStatus == STATUS_SUCCESS)
            {
                /* Configure Partition
                 * EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
                 * DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes */
                returnStatus = FLASH_DEFlashPartition(&flashSSDConfig, 0x02u, 0x08u, 0x0u, false, true);
                if (returnStatus == STATUS_SUCCESS)
                {
                    printf("DEFlash Partition Successful\r\n");
                }
                else
                {
                    printf("DEFlash Partition Failed\r\n");
                }
            }

            if (returnStatus == STATUS_SUCCESS)
            {
                /* Update the new EEPROM configuration */
                returnStatus = FLASH_Init(&g_flashConfig, &flashSSDConfig);
                if (returnStatus == STATUS_SUCCESS)
                {
                    printf("FLASH Init Successful\r\n");
                    printf("EEPROM backup size is %d\r\n", (FEATURE_FLASH_DF_BLOCK_SIZE - flashSSDConfig.dflashSize));
                    printf("EEPROM size is %d\r\n", flashSSDConfig.eeeSize);
                }
                else
                {
                    printf("FLASH Init Failed\r\n");
                }
            }

            if (returnStatus == STATUS_SUCCESS)
            {
                /* Make CFGRAM available for EEPROM */
                returnStatus = FLASH_ConfigCFGRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
                if (returnStatus == STATUS_SUCCESS)
                {
                    printf("Config CFGRam available for EEPROM Successful.\r\n");
                }
                else
                {
                    printf("Config CFGRam available for EEPROM Failed.\r\n");
                }
            }
        }
    }
    else
    {
        printf("CFGRAM is already configured as EEPROM\r\n");

        /* Make CFGRAM available for EEPROM */
        returnStatus = FLASH_ConfigCFGRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Config CFGRam available for EEPROM Successful.\r\n");
        }
        else
        {
            printf("Config CFGRam available for EEPROM Failed.\r\n");
        }
    }

    /* Config DFlash Region 0 is Protected */
    returnStatus = FLASH_ConfigEERAMProtection(0xfe);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Config EERAM Region 0 is Protected Successful\r\n");
    }
    else
    {
        printf("Config EERAM Region 0 is Protected Failed\r\n");
    }

    FLASH_ReadEERAMProtection(&EERamProtectValue);
    printf("EERAM ProtectValue is 0x%02x\r\n",EERamProtectValue);

    if (returnStatus == STATUS_SUCCESS)
    {
        /* EERAM Write data to Region 0 */
        address = flashSSDConfig.eeramBase + 1u;
        size = sizeof(uint8_t);
        sourceBuffer[0u] = 0xFFu;
        returnStatus = FLASH_EEEWrite(&flashSSDConfig, address, size, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("EEWrite Successful\r\n");
        }
        else
        {
            printf("EEWrite Failed\r\n");
            if ((FLASH->STS.reg & 0x10U) != 0U)
            {
               printf("EERAM is Protected\r\n");
            }
        }
    }
    printf("********************************************\r\n");
}
/**@} end of group FLASH_Protection_Functions*/

/**@} end of group FLASH_Protection*/
/**@} end of group Examples*/

