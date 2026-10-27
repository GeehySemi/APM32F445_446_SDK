/*!
 * @file       board_apm32f445_eval.c
 *
 * @brief      This file provides apm32f445_eval board support
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

#include "string.h"
#include "board.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_lpuart.h"

/** @addtogroup Board
  @{
*/
/** @addtogroup APM32F445_EVAL
  @{
*/
/** @defgroup APM32F445_EVAL_Macros Macros
  @{
  */
/**
 * The board has a 3-color LED as follows:
 *  LED Red
 *  LED Green
 *  LED Blue
 */

#define LED_COUNT  (3U)

/**@} end of group APM32F445_EVAL_Macros */
/** @defgroup APM32F445_EVAL_Variables Variables
  @{
  */
uint32_t  ledPIN[3] = {LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN};
PM_T* ledPM[3] = {LED_RED_PORT_BASE, LED_GREEN_PORT_BASE, LED_BLUE_PORT_BASE};
GPIO_T* ledGPIO[3] = {LED_RED_GPIO_BASE, LED_GREEN_GPIO_BASE, LED_BLUE_GPIO_BASE};

LPUART_STATE_T g_lpuart1State;

BTN_CALLBACK_T key1Callback = NULL;
BTN_CALLBACK_T key2Callback = NULL;

/**@} end of group APM32F445_EVAL_Variables */
/** @defgroup APM32F445_EVAL_Enumerations Enumerations
  @{
  */
/*!
 * @brief Initialize LEDs
 *
 * @param None.
 *
 * @retval None.
 */
void LED_Init(void)
{
    uint32_t n;

    /* Enable Clock to Port D */
    CLOCK_SYS_ConfigModuleClock(PMD_CLK, NULL);

    /* Port Initialization */
    for (n = 0; n < LED_COUNT; n++)
    {
        PINS_SetMuxModeSel(ledPM[n], ledPIN[n], PM_MUX_AS_GPIO);
        PINS_SetPinIntSel(ledPM[n], ledPIN[n], PM_DMA_INT_DISABLED);
    }

    /* GPIO Initialization */
    for (n = 0; n < LED_COUNT; n++)
    {
        PINS_SetPins(ledGPIO[n], 1U << ledPIN[n]);
        PINS_SetPinDir(ledGPIO[n], ledPIN[n], 1U);
    }
}

/*!
 * @brief Turn on requested LED
 *
 * @param num  LED number
 *
 * @retval None.
 */
void LED_On(uint32_t num)
{
    if (num < LED_COUNT)
    {
        PINS_ClrPins(ledGPIO[num], 1U << ledPIN[num]);
    }
}

/*!
 * @brief Turn off requested LED
 *
 * @param num  LED number
 *
 * @retval None.
 */
void LED_Off(uint32_t num)
{
    if (num < LED_COUNT)
    {
        PINS_SetPins(ledGPIO[num], 1U << ledPIN[num]);
    }
}

/*!
 * @brief Toggle the requested LED
 *
 * @param num  LED number
 *
 * @retval None.
 */
void  LED_Toggle(uint32_t num)
{
    if (num < LED_COUNT)
    {
        PINS_TogglePins(ledGPIO[num], (1 << ledPIN[num]));
    }
}

/*!
 * @brief ISR for the buttons
 */
void BTN_IsrHandler(void)
{
    /* Check if the KEY1 button was pressed */
    uint32_t status = PINS_ReadPmIntFlg(KEY1_PORT_BASE);
    uint32_t buttonPressed = status & (1 << KEY1_PIN);

    if (buttonPressed != 0)
    {
        /* Clear interrupt flag */
        PINS_ClrPinIntFlgCmd(KEY1_PORT_BASE, KEY1_PIN);

        if (key1Callback != NULL)
        {
            key1Callback();
        }
    }

    /* Check if the KEY2 button was pressed */
    status = PINS_ReadPmIntFlg(KEY2_PORT_BASE);
    buttonPressed = status & (1 << KEY2_PIN);
    if (buttonPressed != 0)
    {
        /* Clear interrupt flag */
        PINS_ClrPinIntFlgCmd(KEY2_PORT_BASE, KEY2_PIN);

        if (key2Callback != NULL)
        {
            key2Callback();
        }
    }
}

/*!
 * @brief Install a handler for KEY1 pressed event
 */
void BTN_InstallKey1Handler(BTN_CALLBACK_T handler)
{
    key1Callback = handler;
}

/*!
 * @brief Install a handler for KEY2 pressed event
 */
void BTN_InstallKey2Handler(BTN_CALLBACK_T handler)
{
    key2Callback = handler;
}

/*!
 * @brief Initialize buttons for KEY1/KEY2
 *
 * @param None.
 *
 * @retval None.
 */
void BTN_Init(void)
{
    /* Enable PMC clock*/
    CLOCK_SYS_ConfigModuleClock(PMC_CLK, NULL);

    PINS_SetMuxModeSel(KEY1_PORT_BASE, KEY1_PIN, PM_MUX_AS_GPIO);
    PINS_SetMuxModeSel(KEY2_PORT_BASE, KEY2_PIN, PM_MUX_AS_GPIO);

    PINS_SetPinIntSel(KEY1_PORT_BASE, KEY1_PIN, PM_INT_RISING_EDGE);
    PINS_SetPinIntSel(KEY2_PORT_BASE, KEY2_PIN, PM_INT_RISING_EDGE);

    /* Set input direction*/
    PINS_SetPinDir(KEY1_GPIO_BASE, KEY1_PIN, 0);
    PINS_SetPinDir(KEY2_GPIO_BASE, KEY2_PIN, 0);

    PINS_ClrPinIntFlgCmd(KEY1_PORT_BASE, KEY1_PIN);
    PINS_ClrPinIntFlgCmd(KEY2_PORT_BASE, KEY2_PIN);

    /* Install buttons ISR */
    INT_SYS_InstallHandler(KEY_PORT_IRQn, &BTN_IsrHandler, NULL);

    /* Enable buttons interrupt */
    INT_SYS_EnableIRQ(KEY_PORT_IRQn);
}

/*!
 * @brief Get button state for BOARD_KEY1/KEY2
 *
 * @param None.
 *
 * @retval None.
 */
uint8_t BTN_GetState(Button_T Button)
{
    uint8_t state = 0U;

    if (Button == BUTTON_KEY1)
    {
        state = (uint8_t)((PINS_ReadPinsInput(KEY1_GPIO_BASE) >> KEY1_PIN) & 0x01U);
    }
    else if (Button == BUTTON_KEY2)
    {
        state = (uint8_t)((PINS_ReadPinsInput(KEY2_GPIO_BASE) >> KEY2_PIN) & 0x01U);
    }

    return state;
}

/*!
 * @brief Get button interrupt state for BOARD_KEY1/KEY2
 *
 * @param None.
 *
 * @retval None.
 */
uint8_t BTN_GetIntState(Button_T Button)
{
    uint8_t state = 0U;

    if (Button == BUTTON_KEY1)
    {
        state = (uint8_t)((PINS_ReadPmIntFlg(KEY1_PORT_BASE) >> KEY1_PIN) & 0x01U);
    }
    else if (Button == BUTTON_KEY2)
    {
        state = (uint8_t)((PINS_ReadPmIntFlg(KEY2_PORT_BASE) >> KEY2_PIN) & 0x01U);
    }

    return state;
}

/*!
 * @brief Initialize LPUART1
 *
 * @param None.
 *
 * @retval None.
 */
void COM_Init(void)
{
    const MODULE_CLK_CONFIG_T uartClkCfg =
    {
        .gating = true,
        .src = LSICLK_CLK,
        .mul = 1U,
        .div = 1U
    };
    LPUART_CFG_T uartCfg =
    {
        .transferType = LPUART_USE_INTERRUPTS,
        .baudrate = 115200U,
        .dataBits = LPUART_8_BITS_DATA,
        .stopBits = LPUART_ONE_STOP_BIT,
        .parityMode = LPUART_PARITY_DISABLED,
        .txDmaChannel = 0U,
        .rxDmaChannel = 0U
    };

    /* Enable PMC clock */
    CLOCK_SYS_ConfigModuleClock(PMC_CLK, NULL);

    /* Enable LPUART1 clock */
    CLOCK_SYS_ConfigModuleClock(LPUART1_CLK, &uartClkCfg);

    PINS_SetMuxModeSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_MUX_ALT2);
    PINS_SetMuxModeSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_MUX_ALT2);
    PINS_SetPinIntSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_DMA_INT_DISABLED);

    LPUART_Init(LPUART_INSTANCE_1, &g_lpuart1State, &uartCfg);
    LPUART_HW_EnableTx(LPUART1);
}

void LIN_EnableTransceiver(void)
{
    /* Set LIN transceiver sleep pin direction */
    PINS_SetPinsDir(LIN_SLP_ENABLE_GPIO, LIN_SLP_ENABLE_MASK);

    /* Wake up LIN transceiver */
    PINS_SetPins(LIN_SLP_ENABLE_GPIO, LIN_SLP_ENABLE_MASK);
}

/**@} end of group APM32F445_EVAL_Functions */
/**@} end of group APM32F445_EVAL */
/**@} end of group Board */

