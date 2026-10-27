/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
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

&par Example Description

This demo shows how to Configure the WDT with Window mode, and feed dog by systick interrupt.
If put down KEY1, stop to feed dog, and it will reset the system by two ways as follows:
    1. #define RCM_WDT_INT 0
        Triggered on WDOG timeout, reset delayed by 128 BUS_CLK cycles(2.6us, 48MHz).
    2. #define RCM_WDT_INT 1
        Triggered after WDOG interrupt delay, reset delayed by 514 LPO cycles(~4ms, 128kHz).

Phenomenon:
If Configure successful, turn on LED_GREEN.
If reset by WDT, turn on LED_RED.
PD16 will Toggles when CNT overflows, check it with analyzer.

&par Hardware Description.

LPUART1 is configured as follows:
    - TX:PC7, RX:PC6
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Transmit enabled


&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.