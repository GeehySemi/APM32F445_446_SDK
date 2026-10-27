/*!
 * @file        board_apm32f446_eval.h
 *
 * @brief       Header file for board_apm32f446_eval.c.
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

#ifndef BOARD_APM32F446_EVAL_H
#define BOARD_APM32F446_EVAL_H

#include "device.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup Board
  @{
*/
/** @addtogroup APM32F446_EVAL
  @{
*/
/** @defgroup APM32F446_EVAL_Macros Macros
  @{
  */

#define LED_RED_PIN             (0U)
#define LED_RED_PORT_BASE       PMD
#define LED_RED_GPIO_BASE       GPIOD

#define LED_GREEN_PIN           (15U)
#define LED_GREEN_PORT_BASE     PMD
#define LED_GREEN_GPIO_BASE     GPIOD

#define LED_BLUE_PIN            (16U)
#define LED_BLUE_PORT_BASE      PMD
#define LED_BLUE_GPIO_BASE      GPIOD

#define KEY1_PIN                (12U)
#define KEY1_PORT_BASE          PMC
#define KEY1_GPIO_BASE          GPIOC

#define KEY2_PIN                (13U)
#define KEY2_PORT_BASE          PMC
#define KEY2_GPIO_BASE          GPIOC

#define KEY_PORT_IRQn           PMC_IRQn

#define COM_TX_PIN              (7U)
#define COM_TX_PORT_BASE        PMC
#define COM_TX_GPIO_BASE        GPIOC

#define COM_RX_PIN              (6U)
#define COM_RX_PORT_BASE        PMC
#define COM_RX_GPIO_BASE        GPIOC

#define USE_LIN_SLP             (1)
#define LIN_SLP_ENABLE_PIN      (9UL)
#define LIN_SLP_ENABLE_MASK     (0x1u << LIN_SLP_ENABLE_PIN)
#define LIN_SLP_ENABLE_GPIO     (GPIOE)

#define LPUART_INSTANCE_1  1

/* Timeout in ms for blocking operations */
#define TIMEOUT     500U

/**@} end of group APM32F446_EVAL_Macros */

/** @defgroup APM32F446_EVAL_Enumerations Enumerations
  @{
  */
typedef void (*BTN_CALLBACK_T)(void);

typedef enum
{
    LED_RED     = 0,
    LED_GREEN   = 1,
    LED_BLUE    = 2
} Led_T;

typedef enum
{
    BUTTON_KEY1 = 0,
    BUTTON_KEY2 = 1
} Button_T;

/**@} end of group APM32F446_EVAL_Enumerations */
/** @defgroup APM32F446_EVAL_Functions Functions
  @{
  */
void  LED_Init(void);
void  LED_On(uint32_t num);
void  LED_Off(uint32_t num);
void  LED_Toggle(uint32_t num);

void BTN_Init(void);
uint8_t BTN_GetState(Button_T button);
uint8_t BTN_GetIntState(Button_T button);
void BTN_InstallKey1Handler(BTN_CALLBACK_T handler);
void BTN_InstallKey2Handler(BTN_CALLBACK_T handler);

void COM_Init(void);

void LIN_EnableTransceiver(void);

/**@} end of group APM32F446_EVAL_Functions */
/**@} end of group APM32F446_EVAL */
/**@} end of group Board */

#endif /* BOARD_APM32F446_EVAL_H */



