/*!
 * @file       Board.c
 *
 * @brief      This file provides firmware functions to manage Leds and push-buttons
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
#include "board.h"
#include "stdio.h"

#if defined (APM32F445_EVAL)
#include "Board_APM32F445_EVAL\src\board_apm32f445_eval.c"
#elif defined (APM32F446_EVAL)
#include "Board_APM32F446_EVAL\src\board_apm32f446_eval.c"

#else
#error "Please select first the APM32F445_446 board to be used (in board.c)"
#endif

/** @addtogroup Board
  @{

*/
/** @addtogroup PRINTF
  @{
*/
/** @defgroup PRINTF_Functions Functions
  @{
  */

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{

    while(LPUART1->STS.bit.TXBEFLG == LPUART_STS_TXBEFLG_0)
    {
    }
    LPUART1->DATA.reg = ch;

    return (ch);
}
#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    LPUART1->DATA.reg = ch;

    /* wait for the data to be send */
    while(LPUART1->STS.bit.TXBEFLG == LPUART_STS_TXBEFLG_0)
    {
    }

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group PRINTF_Functions */
/**@} end of group PRINTF */
/**@} end of group Board */
