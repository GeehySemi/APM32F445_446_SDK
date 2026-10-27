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
#include <stdio.h>
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup MPU_MemoryProtection
  @{
  */

/** @defgroup MPU_MemoryProtection_Marcos Marcos
  @{
  */

#define MPU_INSTANCE   (0U)
#define TEST_ADDRESS   (0x7FF0CU)

/**@} end of group MPU_MemoryProtection_Marcos */

/** @defgroup MPU_MemoryProtection_Variables Variables
  @{
  */

MPU_ACCESS_ERROR_INFO_T expectErrInfo =
{
    .master = 0U,
    .attributes = MPU_DATA_ACCESS_IN_SUPERVISOR_MODE,
    .accessType = MPU_ERR_TYPE_READ,
    .accessCtr = 0xA000,
    .addr = TEST_ADDRESS,
    .processorIdentification = 0U
};

/**@} end of group MPU_MemoryProtection_Variables */

/** @defgroup MPU_MemoryProtection_Functions Functions
  @{
  */

void MPU_Initialize(void);
void KEY1_Irq(void);
bool InfoCompare(MPU_ACCESS_ERROR_INFO_T *errInfo, MPU_ACCESS_ERROR_INFO_T* expectErrInfo);

/*!
 * @brief Main function
 */
int main(void)
{
    CLOCK_SYS_Init(&g_clockConfig);

    LED_Init();
    BTN_Init();

    BTN_InstallKey1Handler(KEY1_Irq);

    LED_On(LED_GREEN);

    MPU_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief MPU instance initialize
 */
void MPU_Initialize(void)
{
    MPU_DeInit(MPU_INSTANCE);
    MPU_Init(MPU_INSTANCE, MPU_REGION_NUMBER, g_mpuUserConfig);
}

/*!
 * @brief This function handles Hard Fault exception
 */
void HardFault_Handler(void)
{
    /* Grant the read right of the test region */
    MPU_EnableRegion(MPU_INSTANCE, 3U, true);
}

/*!
 * @brief BTN interrupt
 */
void KEY1_Irq(void)
{
    MPU_ACCESS_ERROR_INFO_T errInfo;
    volatile uint32_t data;

    /* Deny read right of the test region */
    MPU_EnableRegion(MPU_INSTANCE, 3U, false);

    /* Read protect region */
    data = *((uint32_t*)TEST_ADDRESS);
    (void)data;

    /* Access fault */
    if(MPU_ReadDetailErrorAccessInfo(MPU_INSTANCE, FEATURE_MPU_SLAVE_FLASH_BOOTROM, &errInfo) == true)
    {
        /* compare error information with expect error information */
        if(InfoCompare(&errInfo, &expectErrInfo))
        {
            LED_Off(LED_GREEN);
            LED_On(LED_RED);
        }
    }

    PINS_ClrPmIntFlgCmd(PMC);
}

/*!
 * @brief compare error information with expect error information
 */
bool InfoCompare(MPU_ACCESS_ERROR_INFO_T *errInfo, MPU_ACCESS_ERROR_INFO_T* expectErrInfo)
{
    bool result = true;
    result &= (errInfo->master == expectErrInfo->master ? true : false);
    result &= (errInfo->attributes == expectErrInfo->attributes ? true : false);
    result &= (errInfo->accessType == expectErrInfo->accessType ? true : false);
    result &= (errInfo->accessCtr == expectErrInfo->accessCtr ? true : false);
    result &= (errInfo->addr == expectErrInfo->addr ? true : false);
    result &= (errInfo->processorIdentification == expectErrInfo->processorIdentification ? true : false);
    return result;
}

/**@} end of group MPU_MemoryProtection_Functions */
/**@} end of group MPU_MemoryProtection */
/**@} end of group Examples */
