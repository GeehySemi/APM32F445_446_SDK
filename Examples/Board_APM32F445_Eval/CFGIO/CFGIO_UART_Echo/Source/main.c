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
#include <string.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CFGIO_UART_Echo
  @{
  */

/** @defgroup CFGIO_UART_Echo_Macros Macros
  @{
  */
/**
 * 1: Use DMA for data transfer
 * 0: Use interrupts for data transfer
 */
#define CFGIO_UART_USE_DMA  1

/* Messages */
#define MSG_WELCOME     "\r\nHello Geehy!\r\n"
#define MSG_NEW_LINE    "\r\n> "
#define MSG_ERROR       "\r\nUnknown command!"
#define MSG_HELP        "\r\nEnter the following commands to switch on LED:\r\n red\r\n green\r\n blue\r\n"
#define MSG_RED_ON      "\r\nSwitched on red LED"
#define MSG_GREEN_ON    "\r\nSwitched on green LED"
#define MSG_BLUE_ON     "\r\nSwitched on blue LED"

/* Commands */
#define CMD_RED         "red\r"
#define CMD_GREEN       "green\r"
#define CMD_BLUE        "blue\r"

/* Receive buffer */
#define RX_BUFFER_SIZE 256

/**@} end of group CFGIO_UART_Echo_Macros*/
/** @defgroup CFGIO_UART_Echo_Variables Variables
  @{
  */
uint8_t g_rxBuffer[RX_BUFFER_SIZE];

/* Current receive index of the receive buffer */
uint32_t g_rxIndex = 0;

/* Device states */
CFGIO_DEVICE_STATE_T g_cfgioDeviceState;
CFGIO_UART_STATE_T   g_uartStateRx;
CFGIO_UART_STATE_T   g_uartStateTx;

/**@} end of group CFGIO_UART_Echo_Variables*/

/** @defgroup CFGIO_UART_Echo_Functions Functions
  @{
  */

static void ProcessCommand(void);
static void ClearRxBuffer(void);

/*!
 * @brief   Main function
 * @details This example shows the usage of the CFGIO_UART driver.
 *          The following connections must be done for this example to work:
 *          CFGIO_UART:
 *            - RX: PTA1  (CFGIO_D3)
 *            - TX: PTA0  (CFGIO_D2)
 *
 * @param   None
 * @retval  Exit code
 */
int main(void)
{
    STATUS_T result = STATUS_ERROR;
    bool stop = false;

    /* Initialize clock */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr,
                               CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr,
                               CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);

    /* Initialize pins, LEDs */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);
    LED_Init();

#if CFGIO_UART_USE_DMA
    /* If DMA shall be used for data transfer */
    DMA_Init(&g_dmaControllerState,
             &g_dmaConfig,
             g_dmaChannelStates,
             g_dmaChannelConfigs,
             DMA_CHANNEL_COUNT);
    g_uartRxConfig.transferType = CFGIO_USE_DMA;
    g_uartRxConfig.dmaChannel = 0;
    g_uartTxConfig.transferType = CFGIO_USE_DMA;
    g_uartTxConfig.dmaChannel = 1;
#endif /* CFGIO_UART_USE_DMA */

    /* Initialize CFGIO_UART. 115200 baudrate, 8N1 */
    CFGIO_InitDevice(CFGIO_UART_INSTANCE, &g_cfgioDeviceState);
    CFGIO_UART_Init(CFGIO_UART_INSTANCE, &g_uartRxConfig, &g_uartStateRx);
    CFGIO_UART_Init(CFGIO_UART_INSTANCE, &g_uartTxConfig, &g_uartStateTx);

    /* Send the welcome messages */
    CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_WELCOME, strlen(MSG_WELCOME), 100);
    CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_HELP, strlen(MSG_HELP), 100);
    CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_NEW_LINE, strlen(MSG_NEW_LINE), 100);

     while (!stop)
     {
        /* Start receiving */
        result = CFGIO_UART_RxDataBlocking(&g_uartStateRx, &g_rxBuffer[g_rxIndex], 1, 1000);

        /* Timeout, continue waiting */
        if (result == STATUS_TIMEOUT)
        {
            continue;
        }

        /* A new byte is received */
        uint8_t rxByte = g_rxBuffer[g_rxIndex];
        if (rxByte == '\r')
        {
            /* If ENTER key is pressed, process the received command */
            ProcessCommand();
            ClearRxBuffer();
        }
        else
        {
            /* Otherwise, send back the received char */
            CFGIO_UART_TxDataBlocking(&g_uartStateTx, &rxByte, 1, 100);

            /* Increase the receive index */
            g_rxIndex++;
            if (g_rxIndex == RX_BUFFER_SIZE)
            {
                ClearRxBuffer();
            }
        }
    }

    CFGIO_UART_Deinit(&g_uartStateTx);
    CFGIO_UART_Deinit(&g_uartStateRx);

    LED_On(LED_RED);
}

/*!
 * @brief   UART Rx callback function
 */
void CFGIOUartRxCallback(void *driverState, UART_EVENT_T event, void *userData)
{
    switch (event)
    {
    case UART_EVENT_RX_BUFFER_FULL:
        /* Rx buffer is full */
        break;
    case UART_EVENT_TRANSFER_COMPLETE:
        /* Rx complete */
        break;
    default:
        break;
    }
}

/*!
 * @brief   UART Tx callback function
 */
void CFGIOUartTxCallback(void *driverState, UART_EVENT_T event, void *userData)
{
    switch (event)
    {
    case UART_EVENT_TX_BUFFER_EMPTY:
        /* Tx buffer is empty */
        break;
    case UART_EVENT_TRANSFER_COMPLETE:
        /* Tx complete */
        break;
    default:
        break;
    }
}

/*!
 * @brief   Process commands
 */
void ProcessCommand(void)
{
    if (strncmp((char *)g_rxBuffer, CMD_RED, strlen(CMD_RED)) == 0)
    {
        /* Switch on red LED */
        LED_On(LED_RED);
        LED_Off(LED_GREEN);
        LED_Off(LED_BLUE);
        CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_RED_ON, strlen(MSG_RED_ON), 100);
    }
    else if (strncmp((char *)g_rxBuffer, CMD_GREEN, strlen(CMD_GREEN)) == 0)
    {
        /* Switch on green LED */
        LED_On(LED_GREEN);
        LED_Off(LED_RED);
        LED_Off(LED_BLUE);
        CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_GREEN_ON, strlen(MSG_GREEN_ON), 100);
    }
    else if (strncmp((char *)g_rxBuffer, CMD_BLUE, strlen(CMD_BLUE)) == 0)
    {
        /* Switch on blue LED */
        LED_On(LED_BLUE);
        LED_Off(LED_RED);
        LED_Off(LED_GREEN);
        CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_BLUE_ON, strlen(MSG_BLUE_ON), 100);
    }
    else if (g_rxBuffer[0] == '\r')
    {
        /* Pressed ENTER directly, do nothing */
    }
    else
    {
        /* Unknown command, switch on red and green LEDs */
        LED_On(LED_RED);
        LED_On(LED_GREEN);
        LED_Off(LED_BLUE);
        CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_ERROR, strlen(MSG_ERROR), 100);
    }

    /* Print a new line */
    CFGIO_UART_TxDataBlocking(&g_uartStateTx, (uint8_t *)MSG_NEW_LINE, strlen(MSG_NEW_LINE), 100);
}

/*!
 * @brief   Clear the Rx buffer
 */
void ClearRxBuffer(void)
{
    for (uint32_t i = 0; i < RX_BUFFER_SIZE; i++)
    {
        g_rxBuffer[i] = 0x00;
    }
    g_rxIndex = 0;
}

/**@} end of group CFGIO_UART_Echo_Functions */
/**@} end of group CFGIO_UART_Echo */
/**@} end of group Examples */
