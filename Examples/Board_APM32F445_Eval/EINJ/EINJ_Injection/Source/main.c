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
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup EINJ_Injection
  @{
  */

/** @defgroup EINJ_Injection_Marcos Marcos
  @{
  */

#define EINJ_INSTANCE   (0U)
#define EREP_INSTANCE   (0U)
#define TEST_SRAML_ADDRESS0  (*(uint32_t *)0x1FFFFF04)
#define TEST_SRAML_ADDRESS1  (*(uint32_t *)0x1FFFFFFC)

/**@} end of group EINJ_Injection_Marcos */

/** @defgroup EINJ_Injection_Functions Functions
  @{
  */

void EINJ_Initialize(const EINJ_USER_CHANNEL_CONFIG_T *config);
void EREP_Initialize(void);
void KEY1_Irq(void);
void KEY2_IRQ(void);

/*!
 * @brief Main function
 *
 * @param None
 *
 * @retval None
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

    /* Initialize Test SRAML area */
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
 *
 * @param None
 *
 * @retval None
 */
void EINJ_Initialize(const EINJ_USER_CHANNEL_CONFIG_T *config)
{
    EINJ_DeInit(EINJ_INSTANCE);
    EINJ_Init(EINJ_INSTANCE, EINJ_CHANNEL_COUNTER, config);
}

/*!
 * @brief EREP instance initialize
 *
 * @param None
 *
 * @retval None
 */
void EREP_Initialize(void)
{
    EREP_DeInit(EREP_INSTANCE);
    EREP_Init(EREP_INSTANCE, EINJ_CHANNEL_COUNTER, g_erepConfig);
}

/*!
 * @brief KEY1 interrupt
 *
 * @param None
 *
 * @retval None
 */
void KEY1_Irq(void)
{
    volatile uint32_t test = 0U;
    uint32_t addr = 0U;
    volatile EREP_ECC_ENEVT_T event = EREP_EVENT_NONE;

    /* Close all LED */
    LED_Off(LED_RED);
    LED_Off(LED_BLUE);
    LED_Off(LED_GREEN);

    /* Initialize EINJ module with single bit error injection */
    EINJ_Initialize(g_einjChannel0SingleBitConfig);

    /* Access test area 0 */
    test = TEST_SRAML_ADDRESS0;
    /* Clear "test is declared but not used" waring */
    (void)test;

    /* Get the error event and address caught by EREP */
    event = EREP_ReadErrorDetail(EREP_INSTANCE, EREP_CHANNEL0, &addr);

    /* If event is single bit error */
    if(event == EREP_EVENT_SINGLE_BIT)
    {
        LED_On(LED_BLUE);

        printf("Single bit error occur in 0x%X\r\n", addr);
        /* Clear single bit event flagure */
        EREP_ClearEvent(EREP_INSTANCE, EREP_CHANNEL0, EREP_EVENT_SINGLE_BIT);
    }

    EINJ_DeInit(EINJ_INSTANCE);
    PINS_ClrPmIntFlgCmd(PMC);
}

/*!
 * @brief KEY2 interrupt
 *
 * @param None
 *
 * @retval None
 */
void KEY2_IRQ(void)
{
    volatile uint32_t test = 0U;
    uint32_t addr = 0U;
    volatile EREP_ECC_ENEVT_T event = EREP_EVENT_NONE;

    /* Close all LED */
    LED_Off(LED_RED);
    LED_Off(LED_BLUE);
    LED_Off(LED_GREEN);

    /* Initialize EINJ module with single bit error injection */
    EINJ_Initialize(g_einjChannel0MultBitConfig);

    /* Access test area 1 */
    test = TEST_SRAML_ADDRESS1;
    /* Clear "test is declared but not used" waring */
    (void)test;

    /* Get the error event and address caught by EREP */
    event = EREP_ReadErrorDetail(EREP_INSTANCE, EREP_CHANNEL0, &addr);

    /* If event is non correctable error */
    if(event == EREP_EVENT_NON_CORRECTABLE)
    {
        LED_On(LED_RED);

        printf("Non correntable error occur in 0x%X\r\n", addr);
        /* Clear non correctable event flagure */
        EREP_ClearEvent(EREP_INSTANCE, EREP_CHANNEL0, EREP_EVENT_NON_CORRECTABLE);
    }

    EINJ_DeInit(EINJ_INSTANCE);
    PINS_ClrPmIntFlgCmd(PMC);
}

/**@} end of group EINJ_Injection_Functions */
/**@} end of group EINJ_Injection */
/**@} end of group Examples */
