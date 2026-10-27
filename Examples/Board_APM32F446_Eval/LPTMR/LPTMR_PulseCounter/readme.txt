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

This example shows how to configure the LPTMR peripheral in pulse counter mode,
and the detect the number of pulses, this pulse signal comes from the PD1 PIN.
When the KEY1(PC12) is pressed 5 times, an interrupt is triggered, then green LED
is toggled, and changes the pulse counter compare value to 2. The press KEY1 3 times
again, green LED is toggled again. Repeat the process.
If you press KEY2(PC13), it will change the pulse input source pin to PD5.

&par Hardware Description

  - LPTMR the pulse input PIN: PE11
  - LPTMR the pulse input PIN: PD5 (If KEY2 is pressed)
  - LED_RED:     PD0
  - LED_GREEN:   PD15
  - LED_BLUE:    PD16
  - KEY1:        PC12
  - KEY2:        PC13

LPUART1 is configured as follows:
    - TX:PC7, RX:PC6
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Transmit enabled

How to connect:
  - PD1 <-----> PE11
  - PD1 <-----> PD5 (If KEY2 is pressed)

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
