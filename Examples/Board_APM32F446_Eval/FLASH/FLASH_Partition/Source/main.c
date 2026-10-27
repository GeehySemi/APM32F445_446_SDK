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

/** @addtogroup FLASH_Partition
  @{
*/

/** @defgroup FLASH_Partition_Variables Variables
  @{
*/
FLASH_SSD_CONFIG_T flashSSDConfig;

/* Data source for program operation */
#define BUFFER_SIZE     (0x100u)
uint8_t sourceBuffer[BUFFER_SIZE];

/**@} end of group FLASH_Partition_Variables*/

/** @defgroup FLASH_Partition_Functions Functions
  @{
*/
/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    exit_code.
 */
int main(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t address;
    uint32_t size;
    uint32_t i;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    printf("COM_Init OK\r\n");

    /* Init source data */
    for (i = 0u; i < BUFFER_SIZE; i++)
    {
        sourceBuffer[i] = i;
    }

    /* Disable cache */
    MSC->OCMD0.reg |= (uint32_t)((0x3U<<4)&0x30U);
    MSC->OCMD1.reg |= (uint32_t)((0x3U<<4)&0x30U);

    INT_SYS_DisableIRQGlobal();

    returnStatus = FLASH_Init(&g_flashConfig, &flashSSDConfig);
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
                 * DEPartitionCode = 0x08u: EEPROM backup size = 32 Kbytes */
                returnStatus = FLASH_DEFlashPartition(&flashSSDConfig, 0x02u, 0x03u, 0x0u, false, true);
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
                    printf("Config CFGRam available for EEPROM successful.\r\n");
                    LED_On(LED_GREEN);
                }
                else
                {
                    printf("Config CFGRam available for EEPROM failed.\r\n");
                    LED_On(LED_RED);
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
            printf("Config CFGRam available for EEPROM successful.\r\n");
            LED_On(LED_GREEN);
        }
        else
        {
            printf("Config CFGRam available for EEPROM failed.\r\n");
            LED_On(LED_RED);
        }
    }

    /* Try to write data to EEPROM if CFGRAM is configured as EEPROM */
    if (flashSSDConfig.eeeSize != 0u)
    {
        address = flashSSDConfig.eeramBase;
        size = sizeof(uint32_t);
        returnStatus = FLASH_EEEWrite(&flashSSDConfig, address, size, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("EEWrite Successful\r\n");
            LED_On(LED_BLUE);
        }
        else
        {
            printf("EEWrite Failed\r\n");
            LED_On(LED_RED);
        }

        /* Verify the written data */
        if (*((uint32_t *)sourceBuffer) != *((uint32_t *)address))
        {
            printf("Verify : EEWrite data is wrong\r\n");
            LED_Off(LED_BLUE);
            LED_On(LED_RED);
        }
        else
        {
            printf("Verify : EEWrite data is right\r\n");
        }

        /* Try to update one byte in an EEPROM address which isn't aligned */
        address = flashSSDConfig.eeramBase + 1u;
        size = sizeof(uint8_t);
        sourceBuffer[0u] = 0xFFu;
        returnStatus = FLASH_EEEWrite(&flashSSDConfig, address, size, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("EEWrite Successful\r\n");
            LED_On(LED_BLUE);
        }
        else
        {
            printf("EEWrite Failed\r\n");
            LED_On(LED_RED);
        }

        /* Then verify */
        if (sourceBuffer[0u] != *((uint8_t *)address))
        {
            printf("Verify : Failed to update data to EEPROM\r\n");
            LED_Off(LED_BLUE);
            LED_On(LED_RED);
        }
        else
        {
            printf("Verify : EEWrite data is right\r\n");
        }
    }

    INT_SYS_EnableIRQGlobal();
    /* Remove warning for unused variable */

    while(1)
    {}
}


/**@} end of group FLASH_Partition_Functions*/

/**@} end of group FLASH_Partition*/
/**@} end of group Examples*/

