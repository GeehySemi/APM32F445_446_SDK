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
#include "rtthread.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup RTOS_RT-Thread
  @{
  */

/** @defgroup RTOS_RT-Thread_Variables Variables
  @{
*/

static rt_thread_t led_thread = RT_NULL;

/**@} end of group RTOS_RT-Thread_Variables*/

/** @defgroup RTOS_RT-Thread_Functions Functions
  @{
  */

void led_thread_entry(void* parameter);

/*!
 * @brief   Main function
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    /* Initialize LEDs */
    LED_Init();

    /* Create thread */
    led_thread = rt_thread_create("led1",
                               led_thread_entry,
                               RT_NULL,
                               256,
                               3,
                               20);

    /* Startup thread */
    rt_thread_startup(led_thread);
}

/*!
 * @brief   LED thread entry function
 *
 * @param   parameter: the parameter of thread enter function
 *
 * @retval  None
 */
void led_thread_entry(void* parameter)
{
    while (1)
    {
        LED_Toggle(LED_GREEN);
        rt_thread_delay(1000);
    }
}

/**@} end of group RTOS_RT-Thread_Functions */
/**@} end of group RTOS_RT-Thread */
/**@} end of group Examples */
