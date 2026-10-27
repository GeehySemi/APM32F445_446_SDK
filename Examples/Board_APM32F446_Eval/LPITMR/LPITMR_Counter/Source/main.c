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
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LPITMR_Counter
  @{
  */

/** @defgroup LPITMR_Counter_Marcos Marcos
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

/**@} end of group LPITMR_Counter_Marcos */

/** @defgroup LPITMR_Counter_Functions Functions
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

        printf("couter has been reduced to zero\r\n");
        /* Toggle LED status per 500 ms */
        LED_Toggle(LED_BLUE);
    }
}

/*!
 * @brief LPITMR instance initialize.
 * SYSPLL is selected as the clock source of LPITMR :
 * VCO_CLK = SYSPLL_SRC / prediv * Multiple ,
 * SYSPLLDIV2_CLK = VCO_CLK / 2 / SYSPLL_DIV2 ,
 */
void LPITMR_Initialize(void)
{
    /* Initialize LPITMR */
    LPITMR_DeInit(LPITMR_INSTANCE);
    LPITMR_Init(LPITMR_INSTANCE, &g_lpitmrConfig);
    /* Initialize LPITMR channel1 */
    LPITMR_ChannelInit(LPITMR_INSTANCE, LPITMR_CHANNEL1, &g_lpitmrChannelConfig);

    /* Enable LPITMR channel1 */
    LPITMR_StartTimerChannels(LPITMR_INSTANCE, LPITMR_CHANNEL1);

    INT_SYS_InstallHandler(LPITMR_Ch1_IRQn, LPITMR_Irq, NULL);
    INT_SYS_EnableIRQ(LPITMR_Ch1_IRQn);
}

/**@} end of group LPITMR_Counter_Functions */
/**@} end of group LPITMR_Counter */
/**@} end of group Examples */
