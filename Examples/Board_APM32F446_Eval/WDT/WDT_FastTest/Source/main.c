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
#include "user_config.h"
#include "apm32f445_446_power.h"
/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup WDT_FastTest
  @{
  */

/** @defgroup WDT_FastTest_Functions Functions
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
    uint32_t timeout = 0xFFFFFFFFU;

    /* Init clock config */

    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                                g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    LED_Init();
    COM_Init();
    BTN_Init();

    if((POWER_SYS_ReadResetSrcStatus(RMU, RMU_BY_POWER_ON) == true) && ((WDT->CSTS.reg & 0x00000018)== 0x00U))
    {
        status = WDT_Init(WDT_INSTANCE, &wdtConfig);
        if(status == STATUS_SUCCESS)
        {
            LED_On(LED_BLUE);

            /* LOW test */
            printf("WDT_TEST_LOW\r\n");
            WDT_ConfigTestMode(WDT_INSTANCE, WDT_TEST_LOW);
            WDT_ConfigTimeout(WDT_INSTANCE, 0xAAAAU);
            while(timeout--);  /* CNT counts from 0xXX00 to 0xXXAA, and resets the MCU. */

        }
    }
    else if((POWER_SYS_ReadResetSrcStatus(RMU, RMU_BY_WATCH_DOG) == true))
    {
        if (((WDT->CSTS.reg & 0x00000018)>>3U)== 0x02U) //LOW test finished
        {

            status = WDT_Init(WDT_INSTANCE, &wdtConfig);
            if(status == STATUS_SUCCESS)
            {
                LED_On(LED_BLUE);

                /* HIGH test */
                printf("WDT_TEST_HIGH\r\n");
                WDT_ConfigTestMode(WDT_INSTANCE, WDT_TEST_HIGH);
                WDT_ConfigTimeout(WDT_INSTANCE, 0xAAAAU);
                while(timeout--);  /* CNT counts from 0x00XX to 0xAAXX, and resets the MCU. */

            }

        }
        else if (((WDT->CSTS.reg & 0x00000018)>>3U)== 0x03U) /* LOW test and HIGH test finished */
        {
            status = WDT_Init(WDT_INSTANCE, &wdtConfig);
            if(status == STATUS_SUCCESS)
            {
                LED_On(LED_GREEN);

                /* HIGH test */
                printf("WDT_TEST_USER\r\n");
                WDT_ConfigTestMode(WDT_INSTANCE, WDT_TEST_USER);
                WDT_ConfigTimeout(WDT_INSTANCE, 0x0AAAU);
                SysTick_Init();
            }
        }
        else if (((WDT->CSTS.reg & 0x00000018)>>3U)== 0x01U)
        {
            printf("******************************** \r\n");
            printf("Reset by WDT_TEST_USER, test finished, disable WDT now \r\n");
            printf("\r\n");
            WDT_DeInit(WDT_INSTANCE);

            LED_On(LED_RED);
        }

    }

    while(1)
    {
        /* Put down KEY1, disable the systick */
        if (BTN_GetState(BUTTON_KEY1) == 0)
        {
            while(BTN_GetState(BUTTON_KEY1) == 0);/* wait for realse key */
            APM32_SysTick->CSR.reg = 0U;
            printf("Stop to feed WDT.\r\n");
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
    SystemCoreClockUpdate();
    /** SystemFrequency / 1000 = 1ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1);
    }
}

/**@} end of group WDT_FastTest_Functions */
/**@} end of group WDT_FastTest */
/**@} end of group Examples */
