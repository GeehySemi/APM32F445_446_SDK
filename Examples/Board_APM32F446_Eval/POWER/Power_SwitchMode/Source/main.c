/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include <string.h>
#include "stdio.h"
#include "main.h"
#include "board.h"
#include "system_apm32f445_446.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_lpuart.h"
#include "user_config.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup Power_SwitchMode
  @{
  */

/** @defgroup Power_SwitchMode_Macros Macros
  @{
*/
#define MENU_MESSAGE "Press:\r\n\
\t 1) for HSRUN\r\n\
\t 2) for RUN\r\n\
\t 3) for VLPR\r\n\
\t 4) for STOP1\r\n\
\t 5) for STOP2\r\n\
\t 6) for VLPS\r\n\
---->Press KEY1/KEY2 to wake up the CPU from STOP1,STOP2 or VLPS mode\r\n\
Enter your input:\r\n"

#define LPUART1_INSTANCE 1

#define HSRUN (0u) /* High speed run      */
#define RUN   (1u) /* Run                 */
#define VLPR  (2u) /* Very low power run  */
#define STOP1 (3u) /* Stop option 1       */
#define STOP2 (4u) /* Stop option 2       */
#define VLPS  (5u) /* Very low power stop */

/**@} end of group Power_SwitchMode_Macros*/
/** @defgroup Power_SwitchMode_Functions Functions
  @{
  */
/*!
 * @brief       Read user command
 *
 * @param       None
 *
 * @retval      None
 */
void ReadUserCmd(uint8_t * userCmd)
{
    uint32_t bytesRemain;
    bool receivedStatus = false;
    uint8_t buffer;

    while(receivedStatus == false)
    {
        LPUART_RxData(LPUART1_INSTANCE, &buffer, 1UL);
        while(LPUART_GetRxStatus(LPUART1_INSTANCE, &bytesRemain) != STATUS_SUCCESS)
        {}
        receivedStatus = true;
    }

    /* Check for invalid cmd */
    if (buffer < '1' || buffer > '6')
    {
        printf((const char *)"Invalid CMD!\r\n");
        *userCmd = 0;
    }
    else
    {
        *userCmd = buffer;
    }
}

/*!
 * @brief       Button IRQ
 *
 * @param       None
 *
 * @retval      None
 */
void BIN_ISR(void)
{
    /* Clear KEY1/KEY2 IRQ flag */
    PINS_ClrPinIntFlgCmd(PMC, KEY1_PIN);
    PINS_ClrPinIntFlgCmd(PMC, KEY2_PIN);
    LED_Off(LED_RED);
    LED_On(LED_GREEN);

}

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    STATUS_T status;
    uint8_t userCmd;
    uint32_t frequency;
    uint8_t powerModeIdx;

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    BTN_Init();

    POWER_SYS_Init(&g_powerConfigsArr, POWER_CONFIG_COUNT, &g_powerCallbacksArr, POWER_CALLBACK_COUNT);

    INT_SYS_InstallHandler(PMC_IRQn, BIN_ISR, NULL);
    INT_SYS_EnableIRQ(PMC_IRQn);

    printf(MENU_MESSAGE);

    while(1)
    {
        ReadUserCmd(&userCmd);

        switch (userCmd)
        {
        case '1':
            /* Switch power mode to HSRUN */
            printf("*************************************\r\n");
            status = POWER_SYS_ConfigPowerMode(HSRUN, POWER_POLICY_AGREEMENT);
            if (status == STATUS_SUCCESS)
            {
                CLOCK_SYS_ReadFreq(CORE_CLK, &frequency);
                printf("Switch to HSRUN mode successful.\r\n");
                printf("Core frequency: %d Hz\r\n",frequency);
            }
            else
            {
                printf("Switch to HSRUN mode failed\r\n");
            }
            printf("*************************************\r\n");
            break;
        case '2':
            /* Switch power mode to RUN */
            printf("*************************************\r\n");
            status = POWER_SYS_ConfigPowerMode(RUN, POWER_POLICY_AGREEMENT);
            if (status == STATUS_SUCCESS)
            {
                CLOCK_SYS_ReadFreq(CORE_CLK, &frequency);
                printf("Switch to RUN mode successful.\r\n");
                printf("Core frequency: %d Hz\r\n",frequency);
            }
            else
            {
                printf((const char*)"Switch to RUN mode failed\r\n");
            }

            POWER_SYS_ReadPowerModeIndex(&powerModeIdx);
            printf("powerModeIdx is %d\r\n",powerModeIdx);
            if (POWER_SYS_ReadCurrentRunMode() == POWER_MODE_RUN)
            {
                printf("Current mode is RUN mode.\r\n");
            }
            printf("*************************************\r\n");
            break;
        case '3':
            /* Switch power mode to VLPR */
            printf("*************************************\r\n");
            status = POWER_SYS_ConfigPowerMode(VLPR, POWER_POLICY_AGREEMENT);
            while ((SCG->VLPRCLKCTRL.reg & 0x0F000000) != 0x02000000);
            if (status == STATUS_SUCCESS)
            {
                CLOCK_SYS_ReadFreq(CORE_CLK, &frequency);
                printf("Switch to VLPR mode successful.\r\n");
                printf("Core frequency: %d Hz\r\n",frequency);
            }
            else
            {
                printf((const char*)"Switch to VLPR mode failed\r\n");
            }

            POWER_SYS_ReadPowerModeIndex(&powerModeIdx);
            printf("powerModeIdx is %d\r\n",powerModeIdx);
            if (POWER_SYS_ReadCurrentRunMode() == POWER_MODE_VLPR)
            {
                printf("Current mode is VLPR mode.\r\n");
            }
            printf("*************************************\r\n");
            break;
        case '4':
            printf("*************************************\r\n");
            LED_Off(LED_GREEN);
            LED_On(LED_RED);

            printf("CPU is entering STOP1 mode...\r\n");
            printf("When the LED_REN is on, that indicates CPU had entered STOP1 mode.\r\n");
            printf("Then put down KEY1/KEY2 to exit STOP1 mode.\r\n");
            /* Switch power mode to STOP1 */
            status = POWER_SYS_ConfigPowerMode(STOP1, POWER_POLICY_AGREEMENT);
            printf("*************************************\r\n");
            break;
        case '5':
            printf("*************************************\r\n");
            LED_Off(LED_GREEN);
            LED_On(LED_RED);

            printf("CPU is entering STOP2 mode...\r\n");
            printf("When the LED_REN is on, that indicates CPU had entered STOP2 mode.\r\n");
            printf("Then put down KEY1/KEY2 to exit STOP2 mode.\r\n");
            /* Switch power mode to STOP2 */
            status = POWER_SYS_ConfigPowerMode(STOP2, POWER_POLICY_AGREEMENT);
            printf("*************************************\r\n");
            break;
        case '6':
            printf("*************************************\r\n");
            LED_Off(LED_GREEN);
            LED_On(LED_RED);

            printf("CPU is entering VLPS mode...\r\n");
            printf("When the LED_REN is on, that indicates CPU had entered VLPS mode.\r\n");
            printf("Then put down KEY1/KEY2 to exit VLPS mode.\r\n");
            /* Switch power mode to VLPS */
            status = POWER_SYS_ConfigPowerMode(VLPS, POWER_POLICY_AGREEMENT);
            if (status == STATUS_SUCCESS)
            {
                if (POWER_SYS_ReadCurrentRunMode() == POWER_MODE_RUN)
                {
                    printf("Current mode is RUN mode.\r\n");
                    CLOCK_SYS_ReadFreq(CORE_CLK, &frequency);
                    printf("Core frequency: %d Hz\r\n",frequency);
                }
                else if (POWER_SYS_ReadCurrentRunMode() == POWER_MODE_VLPR)
                {
                    COM_Init();
                    printf("Current mode is VLPR mode.\r\n");
                    CLOCK_SYS_ReadFreq(CORE_CLK, &frequency);
                    printf("Core frequency: %d Hz\r\n",frequency);
                }
            }
            else
            {
                printf("Switch to VLPS mode failed\r\n");
            }
            printf("*************************************\r\n");
            break;
        default:
            break;
        }
    }
}

/**@} end of group Power_SwitchMode_Functions */
/**@} end of group Power_SwitchMode */
/**@} end of group Examples */
