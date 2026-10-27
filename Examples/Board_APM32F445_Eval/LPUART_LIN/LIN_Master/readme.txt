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
LIN MASTER
Example that shows the usage of the LIN driver in master mode.

Application description:
This example demonstrates the LIN communication between APM32F445 EVAL Master
and Slave using LIN driver.

A frame contains header and data. The Master node can send header and data, but
Slave node only can send data. Based on header, Master node or Slave node will
take corresponding action. On Master node:

Press KEY1:
  - For the first time, Master node sends FRAME_MASTER_RX_DATA header and require
    slave node responds by sending data.
  - For the second time, Master sends FRAME_SLAVE_RX_DATA header, then continue
    sending data and slave node will receive the data.
  - If node successful receives data, this node will turn on GREEN_LED,
    otherwise turn on RED_LED.

Press KEY2:
Master node will check current node state. If the state is LIN_NODE_SLP_MODE,
Master node will send wakeup signal and BLUE_LED will be turned on both nodes.
Otherwise Master node will send header to set Master node and Slave node to
sleep mode and all LEDs will be turned off on both nodes.

&par Hardware Description

The following connections must be done to for this example application to work:

KEY 2 (PTC12)
KEY 3 (PTC13)
RED_LED (PTD0)
GREEN_LED (PTD15)
BLUE_LED (PTD16)
GND (GND)
LIN
+12V DC Power Supply


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
