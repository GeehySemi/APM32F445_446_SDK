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
#include "apm32f445_446_lpuart.h"
#include "osif.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPI2C_TwoBoardsDMA_Slave
  @{
  */

/** @defgroup LPI2C_TwoBoardsDMA_Slave_Macros Macros
  @{
*/

/* User menu message */
#define USER_MENU_MESSAGE \
"\r\nPlease enter 1 or 2:\r\n\
  1) LPI2C Slave write data.\r\n\
  2) LPI2C Slave read data.\r\n\
Enter your input:"

/*
 * 1: blocking transfer
 * 0: non-blocking transfer
 */
#define LPI2C_SLAVE_TRANSFER_MODE        1

/* LPI2C TX/RX buffer size */
#define BUFF_SIZE                        (128U)

/**@} end of group LPI2C_TwoBoardsDMA_Slave_Macros*/

/** @defgroup LPI2C_TwoBoardsDMA_Slave_Variables Variables
  @{
  */

/* LPI2C Master TX/RX buffers */
uint8_t g_slaveTxBuff[BUFF_SIZE];
uint8_t g_slaveRxBuff[BUFF_SIZE];

/* Compare buffers */
uint8_t g_compareBuff[BUFF_SIZE];

/**@} end of group LPI2C_TwoBoardsDMA_Slave_Variables */

/** @defgroup LPI2C_TwoBoardsDMA_Slave_Functions Functions
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
    uint32_t bytesRemaining;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize pins */
    PINS_Init(CONFIGURED_PINS_CNT, g_pinsConfig);

    /* Initialize LEDs */
    LED_Init();

    /* Initialize Buttons */
    BTN_Init();

    /* Initialize UART */
    COM_Init();

    /* Initialize slave data buffer */
    InitSlaveBuffer();

    /* Initialize DMA */
    statusCode = DMA_Init(&g_dmaControllerState,
                  &g_dmaConfig,
                  g_dmaChannelStates,
                  g_dmaChannelConfigs,
                  DMA_CHANNEL_COUNT);
    if (statusCode != STATUS_SUCCESS)
    {
        printf("Failed to initialize DMA! statusCode = 0x%04X\r\n", statusCode);
        goto end;
    }

    while (1)
    {
        uint8_t ch;
        printf("\r\n=========================== start ============================\r\n");

        /* Print user menu message */
        printf("%s", USER_MENU_MESSAGE);

        /* Get user input */
        LPUART_RxData(LPUART_INSTANCE_1, &ch, 1UL);
        /* Wait for transfer to be completed */
        while (LPUART_GetRxStatus(LPUART_INSTANCE_1, &bytesRemaining) != STATUS_SUCCESS);

        printf("%c", ch);

        if (ch != '1' && ch != '2')
        {
            printf("\r\nInvalid input......\r\n");
        }

        switch (ch)
        {
            case '1':
                /* Initialize LPI2C as slave in interrupt transfer mode */
                g_lpi2cSlaveUserConfig.transferType = LPI2C_USE_INTERRUPTS;
                LPI2C_SlaveInit(LPI2C_INSTANCE, &g_lpi2cSlaveUserConfig, &lpi2cSlaveState);

                printf("\r\nLPI2C Slave writing data in interrupt mode......\r\n");

            #if LPI2C_SLAVE_TRANSFER_MODE
                /* LPI2C slave blocking write data to the master */
                statusCode = LPI2C_SlaveSendDataBlocking(LPI2C_INSTANCE,
                                                          g_slaveTxBuff,
                                                          BUFF_SIZE,
                                                          100000);
            #else
                /* LPI2C slave non-blocking write data to the master */
                statusCode = LPI2C_SlaveSendDataNonBlocking(LPI2C_INSTANCE, g_slaveTxBuff, BUFF_SIZE);
                /* Wait for the data transfer complete */
                while (LPI2C_SlaveGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining) != STATUS_SUCCESS);
            #endif

                if (statusCode == STATUS_SUCCESS)
                {
                    printf("LPI2C slave write data success!\r\n");
                }
                else
                {
                    printf("LPI2C slave write data failed! statusCode = 0x%04X\r\n", statusCode);
                }
                break;
            case '2':
                /* Initialize LPI2C as slave in DMA transfer mode */
                g_lpi2cSlaveUserConfig.transferType = LPI2C_USE_DMA;
                LPI2C_SlaveInit(LPI2C_INSTANCE, &g_lpi2cSlaveUserConfig, &lpi2cSlaveState);

                printf("\r\nLPI2C Slave reading data in DMA mode......\r\n");

                memset(g_slaveRxBuff, 0, BUFF_SIZE);

            #if LPI2C_SLAVE_TRANSFER_MODE
                /* LPI2C slave blocking read data from the master */
                statusCode = LPI2C_SlaveReceiveDataBlocking(LPI2C_INSTANCE,
                                                             g_slaveRxBuff,
                                                             BUFF_SIZE,
                                                             100000);
            #else
                /* LPI2C slave non-blocking read data from the master */
                statusCode = LPI2C_SlaveReceiveDataNonBlocking(LPI2C_INSTANCE, g_slaveRxBuff, BUFF_SIZE);
                /* Wait for the data transfer complete */
                while (LPI2C_SlaveGetTransferStatus(LPI2C_INSTANCE, &bytesRemaining) != STATUS_SUCCESS);
            #endif

                if (statusCode == STATUS_SUCCESS)
                {
                    printf("LPI2C master read data success!\r\n");
                }
                else
                {
                    printf("LPI2C master read data failed! statusCode = 0x%04X\r\n", statusCode);
                }

                printf("LPI2C Slave RX data: \r\n");
                PrintBuffer(g_slaveRxBuff, BUFF_SIZE);

                /* Compare g_slaveRxBuff with g_compareBuff */
                LED_Off(LED_GREEN);
                LED_Off(LED_RED);
                if (CompareBuffer(g_slaveRxBuff, g_compareBuff))
                {
                    printf("OK! LPI2C master sends data and slave receive data are equal!\r\n");
                    LED_On(LED_GREEN);
                }
                else
                {
                    printf("Error! LPI2C master sends data and slave receive data are not equal!\r\n");
                    LED_On(LED_RED);
                }
                break;
            default :
                break;
        }

        printf("\r\n============================ end ==============================\r\n");
    }

end:
    LED_On(LED_RED);
    return 0;
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


/**@} end of group LPI2C_TwoBoardsDMA_Slave_Functions */
/**@} end of group LPI2C_TwoBoardsDMA_Slave */
/**@} end of group Examples */
