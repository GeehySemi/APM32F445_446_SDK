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

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CAN_RxFifo
  @{
  */

/** @defgroup CAN_RxFifo_Macros Macros
  @{
  */

/* CAN instance */
#define CAN0_INSTANCE       (0U)

/* CAN mailbox. The 0 - 7 MBs are used by RxFIFO, so MB 8 can be used for transmit. */
#define TX_MB               (8U)

/* CAN message ID */
#define TX_CAN_MESSAGE_ID_1 (1U)
#define TX_CAN_MESSAGE_ID_2 (2U)

/**@} end of group CAN_RxFifo_Macros*/

/** @defgroup CAN_RxFifo_Functions Functions
  @{
  */
/*!
 * @brief This function is called when the KEY1 button is pressed
 */
void Key1Handler(void)
{
    /* Send standard CAN frame */
    uint8_t payload[4] = {0x30, 0x31, 0x32, 0x33};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_STANDARD,   /* Standard CAN ID */
        .dataLen    = sizeof(payload),      /* Payload size */
        .fdEnable   = false,
        .isRemote   = false
    };
    CAN_SendNonBlocking(CAN0_INSTANCE, TX_MB, &txDataInfo, TX_CAN_MESSAGE_ID_1, payload);
}

/*!
 * @brief This function is called when the KEY2 button is pressed
 */
void Key2Handler(void)
{
    /* Send extended CAN frame */
    uint8_t payload[5] = {0x31, 0x32, 0x33, 0x34, 0x35};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_EXTENDED,   /* Extended CAN ID */
        .dataLen    = sizeof(payload),      /* Payload size */
        .fdEnable   = false,
        .isRemote   = false
    };
    CAN_SendNonBlocking(CAN0_INSTANCE, TX_MB, &txDataInfo, TX_CAN_MESSAGE_ID_2, payload);
}

/*!
 * @brief   Main function
 * @param   None
 * @retval  Exit code
 */
int main(void)
{
    STATUS_T status = STATUS_ERROR;
    CAN_STATE_T canState;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize pins */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);

    /* Initialize the DMA controller, the RxFIFO is configured to use DMA */
    DMA_Init(&g_dmaControllerState,
                      &g_dmaConfig,
                      g_dmaChannelStates,
                      g_dmaChannelConfigs,
                      DMA_CHANNEL_COUNT);

    /* Initialize LEDs */
    LED_Init();
    LED_On(LED_GREEN);

    /* Initialize buttons */
    BTN_Init();
    BTN_InstallKey1Handler(Key1Handler);
    BTN_InstallKey2Handler(Key2Handler);

    /* Initialize the CAN controller, bitrate is 250K */
    CAN_Init(CAN0_INSTANCE, &canState, &g_canConfig);

    /**
     * Configure the RxFIFO with 8 filters:
     * Standard CAN ID: 1 - 4
     * Extended CAN ID: 1 - 4
     */
    CAN_RXFIFO_ID_FILTER_T rxFifoFilterTables[8] = {
        {
            .isExtended = false,   /* Standard CAN ID */
            .isRemote = false,
            .idFilter = 1
        },
        {
            .isExtended = false,   /* Standard CAN ID */
            .isRemote = false,
            .idFilter = 2
        },
        {
            .isExtended = false,   /* Standard CAN ID */
            .isRemote = false,
            .idFilter = 3
        },
        {
            .isExtended = false,   /* Standard CAN ID */
            .isRemote = false,
            .idFilter = 4
        },
        {
            .isExtended = true,    /* Extended CAN ID */
            .isRemote = false,
            .idFilter = 1
        },
        {
            .isExtended = true,    /* Extended CAN ID */
            .isRemote = false,
            .idFilter = 2
        },
        {
            .isExtended = true,    /* Extended CAN ID */
            .isRemote = false,
            .idFilter = 3
        },
        {
            .isExtended = true,    /* Extended CAN ID */
            .isRemote = false,
            .idFilter = 4
        }
    };
    CAN_ConfigureRxFifo(CAN0_INSTANCE, CAN_RXFIFO_ID_FORMAT_A, rxFifoFilterTables);

    /* Use global Rx masking for the RxFIFO */
    CAN_SetRxMaskType(CAN0_INSTANCE, CAN_RX_MASK_GLOBAL);
    CAN_SetRxFifoGlobalMask(CAN0_INSTANCE, CAN_ID_EXTENDED, 0xFFFFFFFF);

    CAN_MSG_BUF_T rxFrame;
    while (1)
    {
        /* Receive CAN frames from the RxFIFO*/
        status = CAN_ReceiveFifoBlocking(CAN0_INSTANCE, &rxFrame, 2000);

        if (status == STATUS_SUCCESS)
        {
            /* Received a new message, toggle the green LED */
            LED_Toggle(LED_GREEN);
        }
        else if (status == STATUS_TIMEOUT)
        {
            /* Do nothing in case of timeout */
        }
        else
        {
            /* Error receiving message */
            break;
        }
    }

    CAN_DeInit(CAN0_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/**@} end of group CAN_RxFifo_Functions */
/**@} end of group CAN_RxFifo */
/**@} end of group Examples */
