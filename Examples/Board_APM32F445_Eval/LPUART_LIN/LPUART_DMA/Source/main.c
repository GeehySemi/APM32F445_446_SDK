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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPUART_DMA
  @{
  */
/** @defgroup LPUART_DMA_Macros Macros
  @{
  */

/* Example message sent to the console */
#define EXAMPLE_MSG "This example is an simple echo using LPUART based on DMA\r\n\
it echos back any character you send to it.\r\n\
The board will greet you if you send 'Hi Geehy'\r\n\
Now you can input some text and press ENTER:\r\n"

#define WELCOME_MSG "Welcome\r\n"

/* Error message showed at the console, in case data is received incorrectly */
#define ERROR_MSG "Error happened! The application stops!\r\n"

/* Tx timeout in ms */
#define TX_TIMEOUT  1000U

/* RX buffer length */
#define BUFFER_LEN  256U

/**@} end of group LPUART_DMA_Macros */

/** @defgroup LPUART_DMA_Variables Variables
  @{
  */

/**
 * Buffer stores data received from the console.
 * Three more bytes to store new line characters and string terminating '\0'.
 */
uint8_t  g_rxBuffer[BUFFER_LEN + 3];
uint32_t g_rxIndex;

/* Flag updated in DMA ISR for data transfer completion */
volatile bool g_transferComplete;

/**@} end of group LPUART_DMA_Variables */

/** @defgroup LPUART_DMA_Functions Functions
  @{
  */

/*!
 * @brief Callback for DMA channels
 */
void DmaCallback(void *parameter, DMA_CHANNEL_STATUS_T status)
{
    (void)status;
    (void)parameter;

    g_transferComplete = true;
}

/*!
 * @brief LPUART rx callback
 */
void UartRxCallback(void *driverState, UART_EVENT_T event, void *userData)
{
    (void)driverState;
    (void)userData;

    if (event == UART_EVENT_RX_BUFFER_FULL)
    {
        /* Reception stops until newline received or buffer full */
        if ((g_rxBuffer[g_rxIndex] != '\r') && (g_rxIndex <= BUFFER_LEN))
        {
            /* Update the rx index and the rx buffer */
            g_rxIndex++;
            LPUART_SetRxBuffer(LPUART1_INSTANCE, &g_rxBuffer[g_rxIndex], 1U);
        }
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    STATUS_T status;
    uint32_t bytesRemaining;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    /* Initialize pins */
    PINS_Init(NUM_OF_CONFIGURED_PINS, g_pinsConfig);

    /* Initialize LPUART */
    LPUART_Init(LPUART1_INSTANCE, &g_lpuart1Status, &g_lpuart1UserConfig);

    /* Register the callback for rx events */
    LPUART_InstallRxCallback(LPUART1_INSTANCE, UartRxCallback, NULL);

    /* Initialize DMA */
    DMA_Init(&g_dmaControllerState, &g_dmaConfig, g_dmaChannelStates,
             g_dmaChannelConfigs, DMA_CHANNEL_COUNT);

    /* Send greeting message */
    LPUART_TxDataBlocking(LPUART1_INSTANCE,
                          (uint8_t *)EXAMPLE_MSG,
                          strlen(EXAMPLE_MSG),
                          TX_TIMEOUT);

    /* Receive data from console and echo back */
    for (;;)
    {
        /* Receive data until new line character received or buffer full */
        LPUART_RxData(LPUART1_INSTANCE, g_rxBuffer, 1U);

        /* Wait for transfer finished */
        while (LPUART_GetRxStatus(LPUART1_INSTANCE, &bytesRemaining) == STATUS_BUSY);

        /* Check RX status */
        status = LPUART_GetRxStatus(LPUART1_INSTANCE, &bytesRemaining);
        if (status != STATUS_SUCCESS)
        {
            /* If error happened, send error message and exit the loop */
            LPUART_TxDataBlocking(LPUART1_INSTANCE,
                                  (uint8_t *)ERROR_MSG,
                                  strlen(ERROR_MSG),
                                  TX_TIMEOUT);
            break;
        }

        /*  Send back "Welcome" when "Hi Geehy" is received*/
        if (strcmp((char *)g_rxBuffer, "Hi Geehy\r") == 0)
        {
            /* Send welcome message */
            LPUART_TxDataBlocking(LPUART1_INSTANCE,
                                  (uint8_t *)WELCOME_MSG,
                                  strlen(WELCOME_MSG),
                                  TX_TIMEOUT);
        }
        else
        {
            /* Append new line and string terminator to the received data */
            g_rxBuffer[g_rxIndex] = '\r';
            g_rxBuffer[g_rxIndex + 1] = '\n';
            g_rxBuffer[g_rxIndex + 2] = 0U;
            g_rxIndex += 3;

            /* Send received data back */
            LPUART_TxDataBlocking(LPUART1_INSTANCE, g_rxBuffer, g_rxIndex, TX_TIMEOUT);
        }

        /* Reset rx index before starting a new reception */
        g_rxIndex = 0U;
    }
    while(1)
    {
    }
}

/**@} end of group LPUART_DMA_Functions */
/**@} end of group LPUART_DMA */
/**@} end of group Examples */
