/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
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

&par Example Description

By adjusting the potentiometer on the development board, and the voltage value converted by the ADC
channel can be printed in real-time through the serial port.

&par Hardware Description

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

  - This example runs on APM32F445 EVAL Devices.