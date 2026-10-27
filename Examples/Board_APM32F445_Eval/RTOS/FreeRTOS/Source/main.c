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

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup RTOS_FreeRTOS
  @{
  */

/** @defgroup RTOS_FreeRTOS_Variables Variables
  @{
  */

/* Task handle */
TaskHandle_t xHandleTaskLedToggle = NULL;
TaskHandle_t xHandleTaskPrint     = NULL;

/**@} end of group RTOS_FreeRTOS_Variables */

/** @defgroup RTOS_FreeRTOS_Functions Functions
  @{
  */

void UserTaskCreate(void);
void vTaskLedToggle(void *pvParameters);
void vTaskPrint(void *pvParameters);

/*!
 * @brief   Main function
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Initialize LEDs */
    LED_Init();

    /* Initialize Buttons */
    BTN_Init();

    /* Initialize UART */
    COM_Init();

    /* Create task */
    UserTaskCreate();

    /* Startup FreeRTOS */
    vTaskStartScheduler();

    while (1)
    {
    }
}

/*!
 * @brief       User Create Task
 *
 * @param       None
 *
 * @retval      None
 */
void UserTaskCreate(void)
{
    xTaskCreate((TaskFunction_t )vTaskLedToggle,
                (const char*    )"Led Toggle Task",
                (uint16_t       )256,
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )&xHandleTaskLedToggle);

    xTaskCreate((TaskFunction_t )vTaskPrint,
                (const char*    )"Print Task",
                (uint16_t       )256,
                (void*          )NULL,
                (UBaseType_t    )7,
                (TaskHandle_t*  )&xHandleTaskPrint);
}

/*!
 * @brief       Led toggle task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskLedToggle(void *pvParameters)
{
    while(1)
    {
        /* Toggle LED green */
        LED_Toggle(LED_GREEN);
        /* Task blocking time Delay */
        vTaskDelay(500);
    }
}

/*!
 * @brief       Print task
 *
 * @param       pvParameters: passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskPrint(void *pvParameters)
{
    while(1)
    {
        /* Print information */
        printf("Hello world!\r\n");
        /* Task blocking time Delay */
        vTaskDelay(1000);
    }
}

/**@} end of group RTOS_FreeRTOS_Functions */
/**@} end of group RTOS_FreeRTOS */
/**@} end of group Examples */
