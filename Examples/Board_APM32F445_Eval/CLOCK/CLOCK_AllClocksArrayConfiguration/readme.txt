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

This example describes how to config serval clocks_configuration of MCU device.
All the clocks of user configuration are displayed on serial assistant through LPUART1.
LED_BLUE blinks.

clockConfig0:
 ***************Normal Run Mode *********************
 *           SourceSelect : HSI_CLK(48 MHz)
 *           SYS_CLK.clkFreq : 48 MHz
 *           CORE_CLK.clkFreq : 48 MHz
 *           BUS_CLK.clkFreq : 48 MHz
 *           FLASH_CLK.clkFreq : 24 MHz
 ****************************************************
 ***************High Speed Run Mode *********************
 *           SourceSelect : SYSPLL(112 MHz)
 *           SYS_CLK.clkFreq : 112 MHz
 *           CORE_CLK.clkFreq : 112 MHz
 *           BUS_CLK.clkFreq : 56 MHz
 *           FLASH_CLK.clkFreq : 28 MHz
 ****************************************************

clockConfig1:
 ***************Normal Run Mode *********************
 *           SourceSelect : SYSPLL(160 MHz)
 *           SYS_CLK.clkFreq : 80 MHz
 *           CORE_CLK.clkFreq : 80 MHz
 *           BUS_CLK.clkFreq : 40 MHz
 *           FLASH_CLK.clkFreq : 26.67 MHz
 ****************************************************
 ***************High Speed Run Mode *********************
 *           SourceSelect : SYSPLL(160 MHz)
 *           SYS_CLK.clkFreq : 80 MHz
 *           CORE_CLK.clkFreq : 80 MHz
 *           BUS_CLK.clkFreq : 40 MHz
 *           FLASH_CLK.clkFreq : 26.67 MHz
 ****************************************************

&par Hardware Description

LED_BLUE:   PD16
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