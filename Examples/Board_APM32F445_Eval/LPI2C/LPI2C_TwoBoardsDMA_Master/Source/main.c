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
#include "apm32f445_446_lpuart.h"
#include "osif.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPI2C_TwoBoardsDMA_Master
  @{
  */

/** @defgroup LPI2C_TwoBoardsDMA_Master_Macros Macros
  @{
*/

/* User menu message */
#define USER_MENU_MESSAGE \
"\r\nPlease enter 1 or 2:\r\n\
  1) LPI2C Master write data.\r\n\
  2) LPI2C Master read data.\r\n\
Enter your input:"

/* LPI2C TX/RX buffer size */
#define BUFF_SIZE                 (128U)

/**@} end of group LPI2C_TwoBoardsDMA_Master_Macros*/

/** @defgroup LPI2C_TwoBoardsDMA_Master_Variables Variables
  @{
  */

/* LPI2C Master TX/RX buffer */
uint8_t g_masterTxBuff[BUFF_SIZE];
uint8_t g_masterRxBuff[BUFF_SIZE];

/* Compare buffer */
uint8_t g_compareBuff[BUFF_SIZE];

/* Indicate that a non-blocking transfer is completed */
bool g_transferComplete = false;

/**@} end of group LPI2C_TwoBoardsDMA_Master_Variables */

/** @defgroup LPI2C_TwoBoardsDMA_Master_Functions Functions
  @{
  */

void InitMasterBuffer(void);
bool CompareBuffer(uint8_t *buff1, uint8_t *buff2);
void PrintBuffer(const uint8_t *buff, uint32_t size);
void readOptionFromConsole(uint8_t *option);

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
    LPI2C_MASTER_STATE_T lpi2cMasterState;

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

    /* Initialize master data buffer */
    InitMasterBuffer();

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

    /* Initialize LPI2C as master in DMA transfer mode */
    statusCode = LPI2C_MasterInit(LPI2C_INSTANCE, &g_lpi2cMasterUserConfig, &lpi2cMasterState);
    if (statusCode != STATUS_SUCCESS)
    {
        printf("Failed to initialize LPI2C as Master! statusCode = 0x%04X\r\n", statusCode);
        goto end;
    }

    while (1)
    {
        uint8_t ch;
        printf("\r\n=============================== start ===============================\r\n");

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
                printf("\r\nLPI2C master writing data in DMA mode......\r\n");
                /* LPI2C master blocking write data to the slave */
                statusCode = LPI2C_MasterSendDataBlocking(LPI2C_INSTANCE,
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
                }
                break;
            case '2':
                printf("\r\nLPI2C master reading data in DMA mode......\r\n");

                memset(g_masterRxBuff, 0, BUFF_SIZE);
                /* LPI2C master blocking read data from the slave */
                statusCode = LPI2C_MasterReceiveDataBlocking(LPI2C_INSTANCE,
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
                }

                printf("LPI2C Master RX data: \r\n");
                PrintBuffer(g_masterRxBuff, BUFF_SIZE);

                /* Compare g_masterRxBuff with g_compareBuff */
                LED_Off(LED_GREEN);
                LED_Off(LED_RED);
                if (CompareBuffer(g_masterRxBuff, g_compareBuff))
                {
                    printf("OK! LPI2C slave sends data and master receive data are equal!\r\n");
                    LED_On(LED_GREEN);
                }
                else
                {
                    printf("Error! LPI2C slave sends data and master receive data are not equal!\r\n");
                    LED_On(LED_RED);
                }
                break;
            default :
                break;
        }

        printf("\r\n=============================== end =================================\r\n");
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

/**@} end of group LPI2C_TwoBoardsDMA_Master_Functions */
/**@} end of group LPI2C_TwoBoardsDMA_Master */
/**@} end of group Examples */
