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
This example shows the usage of the CAN Rx FIFO.
The Rx FIFO is configured with 8 ID filters:
  - Standard CAN ID: 1, 2, 3, 4
  - Extended CAN ID: 1, 2, 3, 4
It will toggle the green LED in case CAN frames are received.
Using the two buttons it will send data to the CAN bus with standard CAN ID 1,
or extended CAN ID 2.

&par Hardware Description

  - CAN bitrate: 250 Kbit/s

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
