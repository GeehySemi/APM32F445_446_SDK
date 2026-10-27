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

/** @addtogroup CAN_FdFrames
  @{
  */

/** @defgroup CAN_FdFrames_Macros Macros
  @{
  */

/* CAN instance */
#define CAN0_INSTANCE       (0U)

/* CAN mailbox */
#define RX_MB               (0U)
#define TX_MB               (1U)

/* CAN message ID */
#define RX_CAN_MESSAGE_ID   (1U)
#define TX_CAN_MESSAGE_ID_1 (1U)
#define TX_CAN_MESSAGE_ID_2 (2U)

#define CANFD_TDC_OFFSET    (5U)

/**@} end of group CAN_FdFrames_Macros*/

/** @defgroup CAN_FdFrames_Functions Functions
  @{
  */
/*!
 * @brief This function is called when the KEY1 button is pressed
 */
void Key1Handler(void)
{
    uint8_t payload[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_STANDARD,  /* Standard CAN ID */
        .dataLen    = sizeof(payload),      /* Payload size */
        .fdEnable   = true,                 /* Enable CAN FD */
        .fdPadding  = 0x00,                 /* Use 0x00 for FD padding */
        .brsEnable  = true,                 /* Enable bitrate switch */
        .isRemote   = false
    };
    CAN_SendNonBlocking(CAN0_INSTANCE, TX_MB, &txDataInfo, TX_CAN_MESSAGE_ID_1, payload);
}

/*!
 * @brief This function is called when the KEY2 button is pressed
 */
void Key2Handler(void)
{
    uint8_t payload[26] = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_EXTENDED,  /* Extended CAN ID */
        .dataLen    = sizeof(payload),      /* Payload size */
        .fdEnable   = true,                 /* Enable CAN FD */
        .fdPadding  = 0xFF,                 /* Use 0xFF for FD padding */
        .brsEnable  = true,                 /* Enable bitrate switch */
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
    CAN_TIME_SEGMENT_T canBitrate;
    CAN_TIME_SEGMENT_T canBitrateFd;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize pins */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);

    /* Initialize LEDs */
    LED_Init();
    LED_On(LED_GREEN);

    /* Initialize buttons */
    BTN_Init();
    BTN_InstallKey1Handler(Key1Handler);
    BTN_InstallKey2Handler(Key2Handler);

    /**
     * Initialize the CAN controller. According to the configuration,
     * arbitration phase bitrate is 500K, data phase bitrate is 1M.
     */
    CAN_Init(CAN0_INSTANCE, &canState, &g_canConfig);

    /* Get the current CAN arbitration phase bitrate */
    CAN_GetBitrate(CAN0_INSTANCE, &canBitrate);

    /* Get the current CAN data phase bitrate */
    CAN_GetBitrateFd(CAN0_INSTANCE, &canBitrateFd);

    /* Enable transceiver delay compensation */
    CAN_SetTdcOffset(CAN0_INSTANCE, true, CANFD_TDC_OFFSET);

    /* Configure Tx mailbox */
    CAN_DATA_INFO_T txMbInfo = {
        .msgIdType  = CAN_ID_STANDARD,   /* Standard CAN ID */
        .dataLen    = 8,    /* Used only to initialize the MB, will be updated when send data */
        .fdEnable   = true, /* Enable CAN FD */
        .fdPadding  = 0x00, /* Use 0x00 for FD padding */
        .brsEnable  = true, /* Enable bitrate switch */
        .isRemote   = false
    };
    CAN_ConfigureTxMb(CAN0_INSTANCE, TX_MB, &txMbInfo, 0);

    /* Configure Rx mailbox */
    CAN_DATA_INFO_T rxMbInfo = {
        .msgIdType  = CAN_ID_STANDARD,   /* Standard CAN ID */
        .dataLen    = 64,   /* Max data length for CAN FD is 64 bytes */
        .fdEnable   = true, /* Enable CAN FD */
        .fdPadding  = 0x00, /* Use 0x00 for FD padding */
        .brsEnable  = true, /* Enable bitrate switch */
        .isRemote   = false
    };
    CAN_ConfigureRxMb(CAN0_INSTANCE, RX_MB, &rxMbInfo, RX_CAN_MESSAGE_ID);

    /* Receive CAN frames */
    CAN_MSG_BUF_T rxFrame;
    while (1)
    {
        /* Start receiving data */
        status = CAN_ReceiveNonBlocking(CAN0_INSTANCE, RX_MB, &rxFrame);
        if (status != STATUS_SUCCESS)
        {
            break;
        }

        /* Wait until the previous receive is completed */
        while(CAN_GetTransferStatus(CAN0_INSTANCE, RX_MB) == STATUS_BUSY);

        /* Check the received message ID and payload */
        if (rxFrame.messageId == RX_CAN_MESSAGE_ID)
        {
            /* Toggle the green LED */
            LED_Toggle(LED_GREEN);
        }
    }

    CAN_DeInit(CAN0_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/**@} end of group CAN_FdFrames_Functions */
/**@} end of group CAN_FdFrames */
/**@} end of group Examples */
