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

/** @addtogroup EWDT_Interrupt
  @{
  */

/** @defgroup EWDT_Interrupt_Marcos Marcos
  @{
  */

#define EWDT_INSTANCE    (0U)

/**@} end of group EWDT_Interrupt_Marcos */

/** @defgroup EWDT_Interrupt_Functions Functions
  @{
  */

void EWDT_Initialize(void);
void PIN_Initialize(void);
void SysTick_Initialize(void);

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
    PIN_Initialize();
    SysTick_Initialize();
    EWDT_Initialize();

    while(1)
    {

    }
}

/*!
 * @brief SysTick interrupt server
 *
 * @param None
 *
 * @retval None
 */
void SysTick_Irq(void)
{
    EWDT_Reload(EWDT_INSTANCE);
    LED_Toggle(LED_BLUE);
}

/*!
 * @brief SysTick initialize
 *
 * @param None
 *
 * @retval None
 */
void SysTick_Initialize(void)
{
    /* update System Core Clock info */
    SystemCoreClockUpdate();
    /* SystemFrequency / 4 = 250ms */
    if (SysTick_Config(SystemCoreClock / 4))
    {
        /* Capture error */
        while (1);
    }

    INT_SYS_InstallHandler(SysTick_IRQn, SysTick_Irq, NULL);
    INT_SYS_EnableIRQ(SysTick_IRQn);
}

/*!
 * @brief SysTick disable
 *
 * @param None
 *
 * @retval None
 */
void SysTick_Disable(void)
{
    APM32_SysTick->CSR.reg = 0U;
}

/*!
 * @brief EWDT interrupt server
 *
 * @param None
 *
 * @retval None
 */
void OutTimeIrq(void)
{
    printf("EWDT time out!\r\n");
    LED_On(LED_RED);
    while(1);
}

/*!
 * @brief EWDT instance initialize
 *
 * @param None
 *
 * @retval None
 */
void EWDT_Initialize(void)
{
    EWDT_Init(EWDT_INSTANCE, &g_ewdtConfig);
    printf("EWDT ready!\r\n");

    INT_SYS_ConfigPriority(WDT_EWDT_IRQn, 0x01);
    INT_SYS_InstallHandler(WDT_EWDT_IRQn, OutTimeIrq, NULL);
    INT_SYS_EnableIRQ(WDT_EWDT_IRQn);
}

/*!
 * @brief KEY interrupt server
 *
 * @param None
 *
 * @retval None
 */
void KEY_Irq(void)
{
    SysTick_Disable();
    LED_Off(LED_BLUE);
    printf("Stop feed dog\r\n");
    /* Wait for EWDT time out */
    while(1);
}

/*!
 * @brief Button and EWDT output pin initialize
 *
 * @param None
 *
 * @retval None
 */
void PIN_Initialize(void)
{
    /* Button Initialize */
    BTN_Init();

    BTN_InstallKey1Handler(KEY_Irq);
    BTN_InstallKey2Handler(KEY_Irq);

    INT_SYS_ConfigPriority(PMC_IRQn, 0x02);

    /* EWDT output pin initialize */
    PINS_SetPinDir(GPIOE, 4U, 1U);
    PINS_SetPinDir(GPIOE, 5U, 0U);
    PINS_SetMuxModeSel(PME, 4U, PM_MUX_ALT7);
    PINS_SetMuxModeSel(PME, 5U, PM_MUX_ALT7);
    PINS_SetPinIntSel(PME, 4U, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(PME, 5U, PM_DMA_INT_DISABLED);
}

/**@} end of group EWDT_Interrupt_Functions */
/**@} end of group EWDT_Interrupt */
/**@} end of group Examples */
