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

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPI2C_TwoBoardsInterrupt_Slave
  @{
  */

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Macros Macros
  @{
*/

/* LPI2C TX/RX buffer size */
#define BUFF_SIZE                 (128U)

/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Macros*/

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Variables Variables
  @{
  */

/* LPI2C Master TX/RX buffers  */
uint8_t g_slaveTxBuff[BUFF_SIZE];
uint8_t g_slaveRxBuff[BUFF_SIZE];

/* Compare buffers */
uint8_t g_compareBuff[BUFF_SIZE];

/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Variables */

/** @defgroup LPI2C_TwoBoardsInterrupt_Slave_Functions Functions
  @{
  */

void InitSlaveBuffer(void);
bool CompareBuffer(uint8_t *buff1, uint8_t *buff2);
void PrintBuffer(const uint8_t *buff, uint32_t size);

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
    LPI2C_SLAVE_STATE_T lpi2cSlaveState;

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

    /* Initialize slave data buffer */
    InitSlaveBuffer();

    /* Initialize LPI2C as slave */
    g_lpi2cSlaveUserConfig.callbackParam = (void *)LPI2C_INSTANCE;
    statusCode = LPI2C_SlaveInit(LPI2C_INSTANCE, &g_lpi2cSlaveUserConfig, &lpi2cSlaveState);
    if (statusCode != STATUS_SUCCESS)
    {
        printf("\r\nFailed to initialize LPI2C as Slave!\r\n");
        goto end;
    }

    printf("\r\nLPI2C slave initialized!\r\n");

    while (1)
    {
    }

end:
    LED_On(LED_RED);
    return 0;
}


/*!
 * @brief   LPI2C slave callback
 *
 * @details This function will be called by LPI2C interrupt handler and it will
 *          assign the buffer for TX or RX events.
 *
 * @param   slaveEvent, value of I2C_SLAVE_EVENT_T
 *
 * @param   userData, pointer of user data buffer
 *
 * @retval  None
 */
void LPI2C_SlaveCallback(I2C_SLAVE_EVENT_T slaveEvent,void *userData)
{
    uint32_t instance = (uint32_t)userData;

    switch(slaveEvent)
    {
    case I2C_SLAVE_EVENT_RX_REQUEST:
        printf("\r\nLPI2C Slave receiving data......\r\n");
        /* Master requests data, set the Rx buffer */
        LPI2C_SlaveSetRxBuffer(instance, g_slaveRxBuff, BUFF_SIZE);
        break;
    case I2C_SLAVE_EVENT_TX_REQUEST:
        printf("\r\nLPI2C Slave sending data......\r\n");
        /* Master sends data, set the Tx buffer */
        LPI2C_SlaveSetTxBuffer(instance, g_slaveTxBuff, BUFF_SIZE);
        break;
    case I2C_SLAVE_EVENT_TX_BUFFER_EMPTY:
    case I2C_SLAVE_EVENT_RX_BUFFER_FULL:
        /* Print slave RX buffer */
        printf("\r\nLPI2C Slave RX data complete, RX data:\r\n");
        PrintBuffer(g_slaveRxBuff, BUFF_SIZE);

        /* Slave RX buffer is full, compare g_slaveRxBuff with g_compareBuff */
        if (CompareBuffer(g_slaveRxBuff, g_compareBuff))
        {
            printf("LPI2C Slave RX data successful!\r\n");
            LED_On(LED_GREEN);
        }
        else
        {
            printf("LPI2C Slave RX data failed!\r\n");
            LED_On(LED_RED);
        }
        break;
    case I2C_SLAVE_EVENT_STOP:
        /* Stop condition is on the bus. Nothing to do in the example. */
        break;
    default:
        break;
    }
}

/*!
 * @brief   Initialize slave data buffers
 *
 * @param   None
 *
 * @retval  None
 */
void InitSlaveBuffer(void)
{
    for(uint32_t idx = 0U; idx < BUFF_SIZE; idx++)
    {
        g_slaveTxBuff[idx] = (uint8_t)(BUFF_SIZE - idx - 1U);
        g_slaveRxBuff[idx] = 0U;
        g_compareBuff[idx] = (uint8_t)idx;
    }
}

/*!
 * @brief   Print an array of bytes
 *
 * @param   buff,    pointer of buffer data.
 *
 * @param   size,    size of buffer data.
 *
* @retval  None.
 */
void PrintBuffer(const uint8_t *buff, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        printf(" %02X", buff[i]);

        if ((i+1) % 16 == 0)
        {
            printf("\r\n");
        }
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


/**@} end of group LPI2C_TwoBoardsInterrupt_Slave_Functions */
/**@} end of group LPI2C_TwoBoardsInterrupt_Slave */
/**@} end of group Examples */
