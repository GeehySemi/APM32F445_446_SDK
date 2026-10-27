/*!
 * @file        main.c
 *
 * @brief       This file provides all the function of this demo.
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

/* Included */
#include "user_config.h"
#include "apm32f445_446_interrupt.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
*/

/** @addtogroup FLASH_ProgramSection
  @{
*/

/** @defgroup FLASH_ProgramSection_Variables Variables
  @{
*/

FLASH_SSD_CONFIG_T flashSSDConfig;

/* Data source for program operation */
#define BUFFER_SIZE     (FEATURE_FLASH_CFGRAM_SIZE/4)
uint8_t sourceBuffer[BUFFER_SIZE];

/**@} end of group FLASH_ProgramSection_Variables*/

/** @defgroup FLASH_ProgramSection_Functions Functions
  @{
*/
/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None.
 */
int main(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t address;
    uint32_t size;
    uint32_t failAddr;
    uint8_t *EERAMPtr;
    uint32_t i;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    printf("COM_Init OK\r\n");

    /* Disable cache */
    MSC->OCMD0.reg |= (uint32_t)((0x3U<<4)&0x30U);
    MSC->OCMD1.reg |= (uint32_t)((0x3U<<4)&0x30U);

    INT_SYS_DisableIRQGlobal();
    returnStatus = FLASH_Init(&g_flashConfig, &flashSSDConfig);

    if (flashSSDConfig.eeeSize != 0)
    {
        /* As traditional RAM */
        FLASH_ConfigCFGRamFunction(&flashSSDConfig, EEE_DISABLE, 0x00u, NULL);
    }

    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        sourceBuffer[i] = i;
    }

    EERAMPtr = (uint8_t *)flashSSDConfig.eeramBase;
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        *(EERAMPtr+i) = *(sourceBuffer+i);
    }

    /* Erase the last sector of PFlash */
    address = FEATURE_FLASH_PF_BLOCK_SIZE - (FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE);
    size = FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE;
    returnStatus = FLASH_EraseSector(&flashSSDConfig, address, size);

    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Erase Sector Successful\r\n");

        /* Verify the erase operation */
        returnStatus = FLASH_VerifySection(&flashSSDConfig, address, size / FTFx_DPHRASE_SIZE, 1u);

        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Verify Section Successful\r\n");
        }
        else
        {
            printf("Verify Section Failed\r\n");
        }
    }
    else
    {
        printf("Erase Sector Failed\r\n");
    }

    for (i = 0U; i < FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE/BUFFER_SIZE; i++)
    {
        returnStatus = FLASH_ProgramSection(&flashSSDConfig, address, BUFFER_SIZE/16);
        address += BUFFER_SIZE;
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Program Section Successful\r\n");
            LED_On(LED_GREEN);
        }
        else
        {
            printf("Program Section Failed\r\n");
            LED_On(LED_RED);
        }
    }

    if (returnStatus == STATUS_SUCCESS)
    {
        address = FEATURE_FLASH_PF_BLOCK_SIZE - (FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE);

        for (i = 0U; i < FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE/BUFFER_SIZE; i++)
        {
            returnStatus = FLASH_ProgramCheck(&flashSSDConfig, address, BUFFER_SIZE, sourceBuffer, &failAddr, 1u);
            address += BUFFER_SIZE;
            if (returnStatus == STATUS_SUCCESS)
            {
                printf("Verify Program Successful\r\n");
                LED_On(LED_GREEN);
            }
            else
            {
                printf("Verify Program Failed\r\n");
                printf("Failed Address is %08X\r\n",failAddr);
                LED_Off(LED_GREEN);
                LED_On(LED_RED);
            }
        }
    }
    else
    {
        printf("Program Section Failed\r\n");
    }

    INT_SYS_EnableIRQGlobal();
    /* Remove warning for unused variable */

    while(1)
    {}
}


/**@} end of group FLASH_ProgramSection_Functions*/

/**@} end of group FLASH_ProgramSection*/
/**@} end of group Examples*/

