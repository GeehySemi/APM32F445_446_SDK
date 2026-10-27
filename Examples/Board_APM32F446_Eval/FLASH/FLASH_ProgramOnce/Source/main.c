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

/** @addtogroup FLASH_ProgramOnce
  @{
*/

/** @defgroup FLASH_ProgramOnce_Variables Variables
  @{
*/
FLASH_SSD_CONFIG_T flashSSDConfig;

/* Data source for program operation */
uint8_t sourceBuffer[8];
uint8_t readBuffer[8];

/**@} end of group FLASH_ProgramOnce_Variables*/

/** @defgroup FLASH_ProgramOnce_Functions Functions
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
    uint32_t i;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    printf("COM_Init OK\r\n");

    /* Init source data */
    for (i = 0U; i < 8U; i++)
    {
        sourceBuffer[i] = 0x70U + i;
    }
     /* Init read data */
    for (i = 0U; i < 8U; i++)
    {
        readBuffer[i] = 0U;
    }

    /* Disable cache */
    MSC->OCMD0.reg |= (uint32_t)((0x3U<<4)&0x30U);
    MSC->OCMD1.reg |= (uint32_t)((0x3U<<4)&0x30U);

    INT_SYS_DisableIRQGlobal();

    returnStatus = FLASH_Init(&g_flashConfig, &flashSSDConfig);

    if (returnStatus == STATUS_SUCCESS)
    {

        returnStatus = FLASH_ProgramOnce(&flashSSDConfig, 0x07U, sourceBuffer);
        if (returnStatus == STATUS_SUCCESS)
        {
            printf("Program Once Successful\r\n");
        }
        else
        {
            printf("Program Once Failed\r\n");
        }
        returnStatus = FLASH_ReadOnce(&flashSSDConfig, 0x07U, readBuffer);
        for (i = 0u; i < 8U; i++)
        {
            printf("readBuffer[%d] = %02x\r\n",i,readBuffer[i]);
        }

    }

    INT_SYS_EnableIRQGlobal();

    while(1)
    {}
}


/**@} end of group FLASH_ProgramOnce_Functions*/

/**@} end of group FLASH_ProgramOnce*/
/**@} end of group Examples*/

