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
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup EREP_Report
  @{
  */

/** @defgroup EREP_Report_Marcos Marcos
  @{
  */

#define EINJ_INSTANCE   (0U)
#define EREP_INSTANCE   (0U)
#define TEST_SRAML_ADDRESS0  (*(uint32_t *)0x1FFFFF0C)
#define TEST_SRAML_ADDRESS1  (*(uint32_t *)0x1FFFFFFC)

/**@} end of group EREP_Report_Marcos */

/** @defgroup EREP_Report_Functions Functions
  @{
  */

void EINJ_Initialize(const EINJ_USER_CHANNEL_CONFIG_T *config);
void EREP_Initialize(void);
void KEY1_Irq(void);
void KEY2_IRQ(void);

/*!
 * @brief Main function
 */
int main(void)
{
    CLOCK_SYS_Init(&g_clockConfig);

    COM_Init();
    LED_Init();
    BTN_Init();
    BTN_InstallKey1Handler(KEY1_Irq);
    BTN_InstallKey2Handler(KEY2_IRQ);
    LED_On(LED_GREEN);

    /* Initialize SRAML test area */
    TEST_SRAML_ADDRESS0 = 0;
    TEST_SRAML_ADDRESS1 = 0;

    /* Initialize EREP module */
    EREP_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief EINJ instance initialize
 */
void EINJ_Initialize(const EINJ_USER_CHANNEL_CONFIG_T *config)
{
    EINJ_DeInit(EINJ_INSTANCE);
    EINJ_Init(EINJ_INSTANCE, EINJ_CHANNEL_COUNTER, config);
}

/*!
 * @brief EREP single fault interrupt routine server
 */
void SingleFaultIrq(void)
{
    /* Only LED BLUE On */
    LED_On(LED_BLUE);
    LED_Off(LED_GREEN);
    LED_Off(LED_RED);
    /* Clear single bit error event flagure */
    EREP_ClearEvent(EREP_INSTANCE, EREP_CHANNEL0, EREP_EVENT_SINGLE_BIT);
}

/*!
 * @brief EREP double fault interrupt routine server
 */
void DoubleFaultIrq(void)
{
    /* Only LED RED On */
    LED_On(LED_RED);
    LED_Off(LED_GREEN);
    LED_Off(LED_BLUE);
    /* Clear non correctable error event flagure */
    EREP_ClearEvent(EREP_INSTANCE, EREP_CHANNEL0, EREP_EVENT_NON_CORRECTABLE);
}

/*!
 * @brief EREP instance initialize
 */
void EREP_Initialize(void)
{
    EREP_DeInit(EREP_INSTANCE);
    EREP_Init(EREP_INSTANCE, EINJ_CHANNEL_COUNTER, g_erepConfig);

    /* Install single error irq and double error irq */
    INT_SYS_InstallHandler(EREP_single_fault_IRQn, SingleFaultIrq, NULL);
    INT_SYS_InstallHandler(EREP_double_fault_IRQn, DoubleFaultIrq, NULL);
    /* Enable single error and double error interrupt */
    INT_SYS_EnableIRQ(EREP_single_fault_IRQn);
    INT_SYS_EnableIRQ(EREP_double_fault_IRQn);
}

/*!
 * @brief KEY1 interrupt
 */
void KEY1_Irq(void)
{
    volatile uint32_t test;
    uint32_t addr;

    EINJ_Initialize(g_einjChannel0SingleBitConfig);

    /* Access test area 0 */
    test = TEST_SRAML_ADDRESS0;
    /* Clear "test is declared but not used" waring */
    (void)test;

    /* Get the error event and address caught by EREP */
    EREP_ReadErrorDetail(EREP_INSTANCE, EREP_CHANNEL0, &addr);

    printf("Single bit error occur in 0x%X\r\n", addr);

    EINJ_DeInit(EINJ_INSTANCE);
    PINS_ClrPmIntFlgCmd(PMC);
}

/*!
 * @brief KEY2 interrupt
 */
void KEY2_IRQ(void)
{
    volatile uint32_t test;
    uint32_t addr;

    EINJ_Initialize(g_einjChannel0MulitBitConfig);

    /* Access test area 1 */
    test = TEST_SRAML_ADDRESS1;
    /* Clear "test is declared but not used" waring */
    (void)test;

    /* Get the error event and address caught by EREP */
    EREP_ReadErrorDetail(EREP_INSTANCE, EREP_CHANNEL0, &addr);

    printf("Non correntable error occur in 0x%X\r\n", addr);

    EINJ_DeInit(EINJ_INSTANCE);
    PINS_ClrPmIntFlgCmd(PMC);
}

/**@} end of group EREP_Report_Functions */
/**@} end of group EREP_Report */
/**@} end of group Examples */
