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

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup LPITMR_Timer
  @{
  */

/** @defgroup LPITMR_Timer_Marcos Marcos
  @{
  */

#define LPITMR_INSTANCE        (0U)

#define LPITMR_CHANNEL0        (0x0U)
#define LPITMR_CHANNEL1        (0x1U)
#define LPITMR_CHANNEL2        (0x2U)
#define LPITMR_CHANNEL3        (0x3U)

#define LPITMR_CHANNEL0_MASK   (0x1U)
#define LPITMR_CHANNEL1_MASK   (0x2U)
#define LPITMR_CHANNEL2_MASK   (0x4U)
#define LPITMR_CHANNEL3_MASK   (0x8U)

/**@} end of group LPITMR_Timer_Marcos */

/** @defgroup LPITMR_Timer_Functions Functions
  @{
  */

void LPITMR_Initialize(void);
void LPITMR_Irq(void);

/*!
 * @brief Main function
 */
int main(void)
{
    CLOCK_SYS_Init(&g_clockConfig);

    COM_Init();
    LED_Init();
    LPITMR_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief LPITMR interrupt server routine
 */
void LPITMR_Irq(void)
{
    if(LPITMR_ReadInterruptFlagTimerChannels(LPITMR_INSTANCE, LPITMR_CHANNEL1_MASK))
    {
        /* Clear LPITMR channel1 interrupt flagure */
        LPITMR_ClearInterruptFlagTimerChannels(LPITMR_INSTANCE, LPITMR_CHANNEL1_MASK);

        printf("timer has reached!\r\n");

        /* Toggle LED status per 500 ms */
        LED_Toggle(LED_BLUE);
    }
}

/*!
 * @brief LPITMR instance initialize
 */
void LPITMR_Initialize(void)
{
    LPITMR_DeInit(LPITMR_INSTANCE);
    LPITMR_Init(LPITMR_INSTANCE, &g_lpitmrConfig);
    LPITMR_ChannelInit(LPITMR_INSTANCE, LPITMR_CHANNEL1, &g_lpitmrChannelConfig);

    LPITMR_StartTimerChannels(LPITMR_INSTANCE, LPITMR_CHANNEL1);

    INT_SYS_InstallHandler(LPITMR_Ch1_IRQn, LPITMR_Irq, NULL);
    INT_SYS_EnableIRQ(LPITMR_Ch1_IRQn);
}

/**@} end of group LPITMR_Timer_Functions */
/**@} end of group LPITMR_Timer */
/**@} end of group Examples */
