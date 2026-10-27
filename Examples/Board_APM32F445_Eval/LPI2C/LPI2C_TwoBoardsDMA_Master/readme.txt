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

This example shows how to configure LPI2C peripherals to communicate between two different boards
in DMA mode. In this example, LPI2C is operated as the Master, then wait for the user to input
information in the serial terminal, and choose whether to send or receive data based on user's
input information.

For Master board:
  Please enter 1 or 2:
    1) LPI2C Master write data.
    2) LPI2C Master read data.
  Enter your input:

You must first select Send/Receive on Slave board, after that select Receive/Send on Master board.

If master and slave transfer is successfully completed, slave buffer and master buffer will be checked.
If the data buffer is equal, turn on the green LED, otherwise turn on the red LED.
You can view the transfer information through the serial terminal.

&par Hardware Description

LPI2C         Master           Slave
LPI2C_SCL     PA3              PA3        (Internal Pull up resistance)
LPI2C_SDA     PA2              PA2        (Internal Pull up resistance)
GND           Master GND       Slave GND

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
