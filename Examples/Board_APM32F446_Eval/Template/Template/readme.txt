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

This demo is based on the APM32F446 EVAL board. It provides a template project.
  - Template_FLASH: Debug in Flash.
  - Template_SRAM: Debug in SRAM.

Note (Keil MDK SRAM Debug):
If you want to debug in SRAM using Keil MDK, additional settings are required.
In the project "Debug" options:
1. Make sure to un-check "Load Application at Startup".
2. Use "Initialization File": Debug_RAM.ini
   This file can be found in the SDK:
   ..\..\..\..\..\..\Libraries\Device\Geehy\APM32F445_446\APM32F446\linker\armc\Debug_RAM.ini
3. If you are using J-Link, in the "Flash Download" settings:
   - Select "Do not Erase"
   - Un-check "Program", "Verify" and "Reset and Run"

&par Hardware Description

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.