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
in interrupt mode. Run Slave board first, after that Run Master board.

First, Master sends data to the slave, and then wait the ACK of the slave.
Then Master receive data from the slave to the master buffer.
If the transfer is successful, Green LED will turn on, otherwise red LED will turn.
You can view the transmission information through the UART.

&par Hardware Description

LPI2C         Master           Slave
LPI2C_SCL     PA3              PA3        (Internal Pull up resistance)
LPI2C_SDA     PA2              PA2        (Internal Pull up resistance)
GND           Master GND       Slave GND

LED_RED:     PD0
LED_GREEN:   PD15
LED_BLUE:    PD16
KEY2:        PC13
KEY1:        PC12

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
