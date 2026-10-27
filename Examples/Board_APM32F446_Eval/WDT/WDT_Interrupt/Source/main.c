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
#include "stdio.h"
#include "main.h"
#include "board.h"
#include "system_apm32f445_446.h"
#include "apm32f445_446_power.h"
#include "user_config.h"
/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup WDT_Interrupt
  @{
  */
/** @defgroup WDT_Interrupt_Macros Macros
  @{
*/
#define RCM_WDT_INT 0

/**@} end of group WDT_Interrupt_Macros*/

/** @defgroup WDT_Interrupt_Functions Functions
  @{
  */
static void SysTick_Init(void);

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

    /* Init clock config */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    BTN_Init();

    if(POWER_SYS_ReadResetSrcStatus(RMU, RMU_BY_WATCH_DOG) == true)
    {
        printf("******************************** \r\n");
        printf("Reset by WDT, disable WDT now \r\n");
        WDT_DeInit(WDT_INSTANCE);

        LED_On(LED_RED);
    }
    else
    {
        SysTick_Init();
        status = WDT_Init(WDT_INSTANCE, &wdtConfig);
        if(status == STATUS_SUCCESS)
        {
            LED_On(LED_GREEN);
        }
#if RCM_WDT_INT
        INT_SYS_DisableIRQ(WDT_EWDT_IRQn);
        RMU->RSTIEN.reg = 0x00A3;
        INT_SYS_EnableIRQ(RMU_IRQn);
#endif
    }

    while(1)
    {
        /* Put down KEY1, disable the systick */
        if (BTN_GetState(BUTTON_KEY1) == 0)
        {
            APM32_SysTick->CSR.reg = 0U;
        }
    }
}

/*!
 * @brief       Start SysTick
 *
 * @param       None
 *
 * @retval      None
 */
void SysTick_Init(void)
{
    SystemCoreClockUpdate();    /* update System Core Clock info */
    /** SystemFrequency / 1000 = 1ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /** Capture error */
        while (1);
    }
}

/**@} end of group WDT_Interrupt_Functions */
/**@} end of group WDT_Interrupt */
/**@} end of group Examples */
