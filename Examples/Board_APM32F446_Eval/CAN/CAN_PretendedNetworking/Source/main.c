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

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CAN_PretendedNetworking
  @{
  */

/** @defgroup CAN_PretendedNetworking_Macros Macros
  @{
  */

/* CAN instance */
#define CAN0_INSTANCE       (0U)

/* Definition of power modes indexes, as configured in g_powerConfigs */
#define POWER_MODE_HSRUN    (0U)    /* High speed run */
#define POWER_MODE_RUN      (1U)    /* Run */
#define POWER_MODE_VLPR     (2U)    /* Very low power run */
#define POWER_MODE_STOP1    (3U)    /* Stop option 1 */
#define POWER_MODE_STOP2    (4U)    /* Stop option 2 */
#define POWER_MODE_VLPS     (5U)    /* Very low power stop */

/**@} end of group CAN_PretendedNetworking_Macros*/

/** @defgroup CAN_PretendedNetworking_Functions Functions
  @{
  */

/*!
 * @brief   CAN event callback function
 * @details This function is called when a CAN event occurred.
 */
void CanEventCallback(
    uint8_t instance,
    CAN_EVENT_TYPE_T eventType,
    uint32_t mbIndex,
    CAN_STATE_T *driverState)
{
    /* Check if this is a wakeup event */
    if (eventType == CAN_EVENT_WAKEUP_MATCH)
    {
        /* Wakeup event received: blue LED = OFF, green LED = ON */
        LED_Off(LED_BLUE);
        LED_On(LED_GREEN);
    }
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

    /* Initialize LEDs */
    LED_Init();

    /* Initialize the power manager */
    POWER_SYS_Init(&g_powerConfigs,
                   POWER_CONFIG_COUNT,
                   &g_powerManagerCallbacks,
                   POWER_CALLBACK_CNT);

    /* Initialize the CAN controller, bitrate is 250K */
    CAN_Init(CAN0_INSTANCE, &canState, &g_canConfig);

    /* Set the CAN event callback handler */
    CAN_InstallEventCallback(CAN0_INSTANCE, &CanEventCallback, NULL);

    /**
     * Enable pretended networking.
     * The device will be woke up when this CAN frame is received:
     * - CAN ID = 1
     * - Payload = 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
     */
    CAN_PN_CONFIG_T pnConfig = {
        .wakeupByTimeout    = false,    /* Wakeup by timeout event is disabled */
        .wakeupByMatch      = true,     /* Wakeup by message match event is enabled */
        .matchCount         = 1,        /* Match once */
        .matchTimeout       = 0,        /* No timeout */
        .filterType         = CAN_PN_FILTER_ID_PAYLOAD,    /* Match both CAN ID and payload */
        .idFilterScheme     = CAN_PN_MATCH_EXACT,   /* Match the exact CAN ID */
        .payloadFilterScheme = CAN_PN_MATCH_EXACT,   /* Match the exact payload */
        .idFilter1 =
        {
            .isExtended = false,    /* Standard CAN ID */
            .isRemote = false,
            .idFilter = 1           /* CAN ID = 1 */
        },
        .idFilter2 =
        {
            .isExtended = false,
            .isRemote = false,
            .idFilter = 0xFFFFFFFF  /* Masks for CAN ID */
        },
        .payloadFilter =
        {
            .minPayloadLen = 8, /* Lower limit of the payload size: 8 byte */
            .maxPayloadLen = 8, /* Upper limit of the payload size: 8 byte */
            .payloadFilter1 = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}, /* Match the complete 8 bytes of payload */
            .payloadFilter2 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, /* Masks for the payload */
        }
    };
    CAN_ConfigurePretendedNetworking(CAN0_INSTANCE, true, &pnConfig);

    /* Go to sleep and wait */
    CAN_MSG_BUF_T wakeupFrame;
    while (1)
    {
        /* Before going to sleep mode: blue LED = ON, green LED = OFF */
        LED_On(LED_BLUE);
        LED_Off(LED_GREEN);

        /**
         * Go to sleep mode now. This function will not return until the device
         * is woke up by a matching CAN frame.
         */
        status = POWER_SYS_ConfigPowerMode(POWER_MODE_STOP2, POWER_POLICY_FORCIBLE);
        if (status != STATUS_SUCCESS)
        {
            break;
        }

        /* The device was woke up, get the CAN frame which triggerred the wakeup event */
        CAN_GetWakeupMessage(CAN0_INSTANCE, 0, &wakeupFrame);

        /* Wait some time and go to sleep again */
        OSIF_TimeDelay(2000);
    }

    CAN_DeInit(CAN0_INSTANCE);
    LED_On(LED_RED);
    return 0;
}

/**@} end of group CAN_PretendedNetworking_Functions */
/**@} end of group CAN_PretendedNetworking */
/**@} end of group Examples */
