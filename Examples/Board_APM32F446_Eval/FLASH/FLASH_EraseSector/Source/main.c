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

/** @addtogroup FLASH_EraseSector
  @{
*/

/** @defgroup FLASH_EraseSector_Variables Variables
  @{
*/

FLASH_SSD_CONFIG_T flashSSDConfig;

/* Data source for program operation */
#define BUFFER_SIZE         0x100u          /* Size of data source */
uint8_t sourceBuffer[BUFFER_SIZE];

/**@} end of group FLASH_EraseSector_Variables*/

/** @defgroup FLASH_EraseSector_Functions Functions
  @{
*/
/*!
 * @brief     Main program
 *
 * @param     None.
 *
 * @retval    None.
 */
int main(void)
{
    STATUS_T returnStatus = STATUS_SUCCESS;
    uint32_t verifyCheckSum = 0;
    uint32_t buffCheckSum = 0;
    uint32_t address;
    uint32_t size;
    uint32_t failAddr;
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
        buffCheckSum += i;
    }

    /* Disable cache */
    MSC->OCMD0.reg |= (uint32_t)((0x3U<<4)&0x30U);
    MSC->OCMD1.reg |= (uint32_t)((0x3U<<4)&0x30U);

    INT_SYS_DisableIRQGlobal();
    returnStatus = FLASH_Init(&g_flashConfig, &flashSSDConfig);

    /* Erase the last sector of PFlash */
    address = FEATURE_FLASH_PF_BLOCK_SIZE - (FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE);
    size = FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE;
    returnStatus = FLASH_EraseSector(&flashSSDConfig, address, size);

    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Erase Sector Successful\r\n");
        LED_On(LED_BLUE);

        /* Verify the erase operation */
        returnStatus = FLASH_VerifySection(&flashSSDConfig, address, size / FTFx_DPHRASE_SIZE, 1u);

        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Verify Section Successful\r\n");
        }
        else
        {
            printf("Verify Section Failed\r\n");
            LED_Off(LED_BLUE);
        }
    }
    else
    {
        printf("Erase Sector Failed\r\n");
    }



    /* Write some data to the erased PFlash sector */
    size = BUFFER_SIZE;
    returnStatus = FLASH_Program(&flashSSDConfig, address, size, sourceBuffer);

    if (returnStatus == STATUS_SUCCESS)
    {
        printf("Program Successful\r\n");
        LED_On(LED_RED);

        /* Verify the program operation */
        returnStatus = FLASH_ProgramCheck(&flashSSDConfig, address, size, sourceBuffer, &failAddr, 1u);

        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Verify Program Successful\r\n");
        }
        else
        {
            printf("Verify Program Failed\r\n");
            printf("Failed Address is %08X\r\n",failAddr);
            LED_Off(LED_RED);
        }
    }
    else
    {
        printf("Program Failed\r\n");
    }

    returnStatus = FLASH_CheckSum(&flashSSDConfig, address, size, &verifyCheckSum);
    if (returnStatus == STATUS_SUCCESS)
    {
        if (verifyCheckSum == buffCheckSum)
        {
            printf("CheckSum is right\r\n");
        }
    }
    else
    {
        printf("CheckSum is wrong\r\n");
    }

    INT_SYS_EnableIRQGlobal();
    /* Remove warning for unused variable */

    while(1)
    {}
}


/**@} end of group FLASH_EraseSector_Functions*/

/**@} end of group FLASH_EraseSector*/
/**@} end of group Examples*/

