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

/** @addtogroup CAN_ClassicFrames
  @{
  */

/** @defgroup CAN_ClassicFrames_Macros Macros
  @{
  */
/* CAN instance */
#define CAN0_INSTANCE       (0U)

/* MB0 is used to receive standard CAN frames */
#define RX_MB_0             (0U)

/* MB1 is used to receive extended CAN frames */
#define RX_MB_1             (1U)

/* The MB used to transmit CAN frames */
#define TX_MB               (2U)

/* CAN message IDs */
#define RX_CAN_MESSAGE_ID   (1U)
#define TX_CAN_MESSAGE_ID_1 (1U)
#define TX_CAN_MESSAGE_ID_2 (2U)
/**@} end of group CAN_ClassicFrames_Macros*/
/** @defgroup CAN_ClassicFrames_Variables Variables
  @{
  */
/* Indicate MB0 receive is completed */
volatile bool g_mb0ReceiveCompleted = false;

/* Indicate MB1 receive is completed */
volatile bool g_mb1ReceiveCompleted = false;

/**@} end of group CAN_ClassicFrames_Variables*/

/** @defgroup CAN_ClassicFrames_Functions Functions
  @{
  */

/*!
 * @brief This function is called when a CAN event occurred
 */
void CanEventCallback(
    uint8_t instance,
    CAN_EVENT_TYPE_T eventType,
    uint32_t mbIndex,
    CAN_STATE_T *driverState)
{
    /* A frame was received in the configured Rx MB */
    if (eventType == CAN_EVENT_RX_COMPLETE)
    {
        /* Set the receive complete flags */
        if (mbIndex == RX_MB_0)
        {
            g_mb0ReceiveCompleted = true;
        }
        if (mbIndex == RX_MB_1)
        {
            g_mb1ReceiveCompleted = true;
        }
    }
}

/*!
 * @brief This function is called when the KEY1 button is pressed
 */
void Key1Handler(void)
{
    /* Send standard CAN frame */
    uint8_t payload[6] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_STANDARD,  /* Standard CAN ID */
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
    uint8_t payload[7] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
    CAN_DATA_INFO_T txDataInfo = {
        .msgIdType  = CAN_ID_EXTENDED,  /* Extended CAN ID */
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
    bool exit = false;
    CAN_STATE_T canState;
    CAN_TIME_SEGMENT_T canBitrate;

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

    /* Initialize the CAN controller, bitrate is 250K */
    CAN_Init(CAN0_INSTANCE, &canState, &g_canConfig);

    /* Get the current CAN bitrate */
    CAN_GetBitrate(CAN0_INSTANCE, &canBitrate);

    /* Configure Tx MB */
    CAN_DATA_INFO_T txMbInfo = {
        .msgIdType  = CAN_ID_STANDARD,   /* Standard CAN ID */
        .dataLen    = 8,        /* Used only to initialize the MB, will be updated when send data */
        .fdEnable   = false,    /* Disable CAN FD */
        .isRemote   = false
    };
    CAN_ConfigureTxMb(CAN0_INSTANCE, TX_MB, &txMbInfo, 0);

    /* Use individual Rx masking for the MBs */
    CAN_SetRxMaskType(CAN0_INSTANCE, CAN_RX_MASK_INDIVIDUAL);

    /* Configure Rx MB0, it will receive standard frames with ID 1 */
    CAN_DATA_INFO_T rxMb0Info = {
        .msgIdType  = CAN_ID_STANDARD,   /* Standard CAN ID */
        .dataLen    = 8,        /* Max data length for classic CAN is 8 bytes */
        .fdEnable   = false,    /* Disable CAN FD */
        .isRemote   = false
    };
    CAN_ConfigureRxMb(CAN0_INSTANCE, RX_MB_0, &rxMb0Info, RX_CAN_MESSAGE_ID);
    CAN_SetRxIndividualMask(CAN0_INSTANCE, CAN_ID_STANDARD, RX_MB_0, 0xFFFFFFFF);

    /* Configure Rx MB1, it will receive extended frames with ID 1 */
    CAN_DATA_INFO_T rxMb1Info = {
        .msgIdType  = CAN_ID_EXTENDED,   /* Extended CAN ID */
        .dataLen    = 8,        /* Max data length for classic CAN is 8 bytes */
        .fdEnable   = false,    /* Disable CAN FD */
        .isRemote   = false
    };
    CAN_ConfigureRxMb(CAN0_INSTANCE, RX_MB_1, &rxMb1Info, RX_CAN_MESSAGE_ID);
    CAN_SetRxIndividualMask(CAN0_INSTANCE, CAN_ID_EXTENDED, RX_MB_1, 0xFFFFFFFF);

    /* Set the CAN event callback handler */
    CAN_InstallEventCallback(CAN0_INSTANCE, &CanEventCallback, NULL);

    /* Receive CAN frames from MB0 and MB1 */
    CAN_MSG_BUF_T rxFrame0;
    CAN_MSG_BUF_T rxFrame1;
    while (!exit)
    {
        /* Reset the receive complete flags */
        g_mb0ReceiveCompleted = false;
        g_mb1ReceiveCompleted = false;

        /* Start non-blocking receiving */
        CAN_ReceiveNonBlocking(CAN0_INSTANCE, RX_MB_0, &rxFrame0);
        CAN_ReceiveNonBlocking(CAN0_INSTANCE, RX_MB_1, &rxFrame1);

        /* Wait until one of the MBs received a CAN frame */
        while ((g_mb0ReceiveCompleted == false)
            && (g_mb1ReceiveCompleted == false));

        /* Toggle the green LED */
        LED_Toggle(LED_GREEN);
    }

    CAN_DeInit(CAN0_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/**@} end of group CAN_ClassicFrames_Functions */
/**@} end of group CAN_ClassicFrames */
/**@} end of group Examples */
