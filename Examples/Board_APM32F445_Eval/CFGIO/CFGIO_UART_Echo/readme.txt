/*!
 * @file        readme.txt
 *
 * @brief       Description for the example
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

This example shows the usage of the CFGIO_UART driver.
It uses CFGIO_D2 and CFGIO_D3 as UART port (115200, 8N1).
It prints a welcome message on startup, and accepts user input commands to
switch on the LEDs. User can enter "blue", "green", "red" commands, then the
blue, green, or red LEDs will be switched on.

The following connections must be done for this example to work:
  - RX: PTA1  (CFGIO_D3)
  - TX: PTA0  (CFGIO_D2)
  - GND

&par Hardware Description

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F445 EVAL Devices.