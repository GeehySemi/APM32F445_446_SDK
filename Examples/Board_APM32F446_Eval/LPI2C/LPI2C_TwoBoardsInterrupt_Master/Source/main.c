/*!
 * @file        main.c
 *
 * @brief       Main program
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

/* Includes */
#include "user_config.h"
#include "board.h"
#include "osif.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPI2C_TwoBoardsInterrupt_Master
  @{
  */

/** @defgroup LPI2C_TwoBoardsInterrupt_Master_Macros Macros
  @{
*/

/* LPI2C TX/RX buffer size */
#define BUFF_SIZE                 (128U)

/**@} end of group LPI2C_TwoBoardsInterrupt_Master_Macros*/

/** @defgroup LPI2C_TwoBoardsInterrupt_Master_Variables Variables
  @{
  */

/* LPI2C Master TX/RX buffer */
uint8_t g_masterTxBuff[BUFF_SIZE];
uint8_t g_masterRxBuff[BUFF_SIZE];

/* Compare buffer */
uint8_t g_compareBuff[BUFF_SIZE];

/**@} end of group LPI2C_TwoBoardsInterrupt_Master_Variables */

/** @defgroup LPI2C_TwoBoardsInterrupt_Master_Functions Functions
  @{
  */

void InitMasterBuffer(void);
bool CompareBuffer(uint8_t *buff1, uint8_t *buff2);

/*!
 * @brief   Main function
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    STATUS_T statusCode = STATUS_ERROR;
    uint32_t bytesRemaining = 0;
    LPI2C_BAUDRATE_PARAMS_T baudRateParams;
    LPI2C_MASTER_STATE_T lpi2cMasterState;

    /* LPI2C Master baudrate 400K */
    baudRateParams.baudrate = 400000U;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize pins */
    PINS_Init(CONFIGURED_PINS_CNT, g_pinsConfig);

    /* Initialize LEDs */
    LED_Init();

    /* Initialize Buttons */
    BTN_Init();

    /* Initialize LPUART1 */
    COM_Init();

    printf("\r\n******************** LPI2C test start! ********************\r\n");

    /* Initialize master data buffer */
    InitMasterBuffer();

    /* Initialize LPI2C as master */
    statusCode = LPI2C_MasterInit(LPI2C_INSTANCE, &g_lpi2cMasterUserConfig, &lpi2cMasterState);
    if (statusCode != STATUS_SUCCESS)
    {
        printf("Failed to initialize LPI2C as Master!\r\n");
        goto end;
    }
    printf("LPI2C master initialized!\r\n");

    /* Set LPI2C slave address */
    LPI2C_MasterSetSlaveAddr(LPI2C_INSTANCE, LPI2C_SLAVE_ADDR, false);

    /* Set LPI2C baudrate to 400K */
    LPI2C_MasterSetBaudrate(LPI2C_INSTANCE, LPI2C_FAST_MODE, baudRateParams);

    /* Read LPI2C current baudrate */
    memset(&baudRateParams, 0, sizeof(baudRateParams));
    LPI2C_MasterGetBaudrate(LPI2C_INSTANCE, &baudRateParams);
    printf("LPI2C current baudrate: %dKHz, Slave address: 0x%02X.\r\n",
        baudRateParams.baudrate / 1000, LPI2C_SLAVE_ADDR);

    /* LPI2C master blocking write and read data test */
    printf("\r\nLPI2C master blocking write and read data test start!\r\n");

    /* LPI2C master blocking write data to the slave */
    statusCode = LPI2C_MasterSendDataBlocking(
        LPI2C_INSTANCE,
        g_masterTxBuff,
        BUFF_SIZE,
        true,
        OSIF_WAIT_FOREVER);

    if (statusCode == STATUS_SUCCESS)
    {
        printf("LPI2C master blocking write data success!\r\n");
    }
    else
    {
        printf("LPI2C master blocking write data failed! statusCode = 0x%04X\r\n", statusCode);
        goto end;
    }

    /* LPI2C master blocking read data from the slave */
    statusCode = LPI2C_MasterReceiveDataBlocking(
        LPI2C_INSTANCE,
        g_masterRxBuff,
        BUFF_SIZE,
        true,
        OSIF_WAIT_FOREVER);

    if (statusCode == STATUS_SUCCESS)
    {
        printf("LPI2C master blocking read data success!\r\n");
    }
    else
    {
        printf("LPI2C master blocking read data failed! statusCode = 0x%04X\r\n", statusCode);
        goto end;
    }

    /* Check remaining bytess, it should be 0 */
    LPI2C_MasterGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining);
    printf("Remaining bytes: %d\r\n", bytesRemaining);

    /* Compare g_masterRxBuff with g_compareBuff */
    if (CompareBuffer(g_masterRxBuff, g_compareBuff))
    {
        printf("LPI2C master blocking write and read data successful!\r\n");
        LED_On(LED_GREEN);
    }
    else
    {
        printf("LPI2C master blocking write and read data failed!\r\n");
        LED_On(LED_RED);
    }



    /* LPI2C master non-blocking write and read data test */
    printf("\r\nLPI2C master non-blocking write and read data test start!\r\n");
    LED_Off(LED_GREEN);
    LED_Off(LED_RED);

    /* LPI2C master non-blocking write data to the slave */
    statusCode = LPI2C_MasterSendDataNonBlocking(LPI2C_INSTANCE, g_masterTxBuff, BUFF_SIZE, true);

    /* Wait for the data transfer complete */
    while (LPI2C_MasterGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining) != STATUS_SUCCESS);

    if (statusCode == STATUS_SUCCESS)
    {
        printf("LPI2C master non-blocking write data success!\r\n");
    }
    else
    {
        printf("LPI2C master non-blocking write data failed! statusCode = 0x%04X\r\n", statusCode);

        goto end;
    }

    /* LPI2C master non-blocking read data from the slave */
    statusCode = LPI2C_MasterReceiveDataNonBlocking(LPI2C_INSTANCE, g_masterRxBuff, BUFF_SIZE, true);

    /* Wait for the data transfer complete */
    while (LPI2C_MasterGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining) != STATUS_SUCCESS);

    if (statusCode == STATUS_SUCCESS)
    {
        printf("LPI2C master non-blocking read data success!\r\n");
    }
    else
    {
        printf("LPI2C master non-blocking read data failed! statusCode = 0x%04X\r\n", statusCode);
        goto end;
    }

    /* Check remaining bytess, it should be 0 */
    LPI2C_MasterGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining);
    printf("Remaining bytes: %d\r\n", bytesRemaining);

    /* Compare g_masterRxBuff with g_compareBuff */
    if (CompareBuffer(g_masterRxBuff, g_compareBuff))
    {
        printf("LPI2C master non-blocking write and read data successful!\r\n");
        LED_On(LED_GREEN);
    }
    else
    {
        printf("LPI2C master non-blocking write and read data failed!\r\n");
        LED_On(LED_RED);
    }

    printf("******************** LPI2C test end! **********************\r\n");

    while (1)
    {
    }

end:
    LPI2C_MasterDeInit(LPI2C_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/*!
 * @brief   Initialize master data buffers
 *
 * @param   None
 *
 * @retval  None
 */
void InitMasterBuffer(void)
{
    for(uint32_t idx = 0U; idx < BUFF_SIZE; idx++)
    {
        g_masterTxBuff[idx] = (uint8_t)idx;
        g_masterRxBuff[idx] = 0U;
        g_compareBuff[idx] = (uint8_t)(BUFF_SIZE - idx - 1U);
    }
}


/*!
 * @brief   Compare buffer1 and buffer2
 *
 * @param   buff1,    pointer of buffer1.
 *
 * @param   buff2,    pointer of buffer2.
 *
* @retval  result of Comparision: true or false.
 */
bool CompareBuffer(uint8_t *buff1, uint8_t *buff2)
{
    bool result = true;

    for (uint32_t i = 0; i < BUFF_SIZE; i++)
    {
        if (buff1[i] != buff2[i])
        {
            result = false;
            break;
        }
    }

    return result;
}

/**@} end of group LPI2C_TwoBoardsInterrupt_Master_Functions */
/**@} end of group LPI2C_TwoBoardsInterrupt_Master */
/**@} end of group Examples */
