/*!
 * @file        readme.txt
 *
 * @brief       Description for the example
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
This example shows the usage of the CAN Pretended Networking functionality.
It enables Pretended Networking and then go to sleep mode. Once a matching
CAN frame is received the device will be woke up, and two seconds later it
will go to sleep mode again.

Blue LED On: The device is in SLEEP mode
Green LED On: The device is in RUN mode
The device will be woke up when the following CAN frame is received:
  - Standard CAN ID = 1
  - Payload = 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88

&par Hardware Description

  - CAN bitrate: 250 Kbit/s

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
