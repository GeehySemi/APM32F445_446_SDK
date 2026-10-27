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

This example shows how to configure the CFGTMR0 peripheral to generate a PWM signal,
and then CFGTMR1 peripheral to measure the frequency of the PWM signal in input
capture mode.

After Reset, the initial frequency of PWM output is 1KHz. Then you can press KEY1 to
change PWM signal frequency. The frequency will range from 1KHz to 50KHz, each time
the button is pressed, the frequency increases by 1KHz.You can view the measurable
frequency value through serial termainal.

CFGTMR count clock configuration:
  - CFGTMR0 counter clock: cfgtmrClockSource / cfgtmrPrescaler = 48MHz / 1 = 48MHz.
  - CFGTMR1 counter clock: cfgtmrClockSource / cfgtmrPrescaler = 48MHz / 1 = 48MHz.

Note:
  - The input signal frequency must be winthin the measurable range.
  - When the CFGTMR counter clock is 48MHz, the measurable range of the
    input signal frequency is as follows:
  -------------------------------------------------------------------------
 |Clock source prescaler | Maximum frequency (Hz) | Minimum frequency (Hz) |
 |-----------------------|------------------------|------------------------|
 |           1           |         48,000,000     |          732.42        |
 |           2           |         24,000,000     |          366.21        |
 |           4           |         12,000,000     |          183.10        |
 |           8           |          6,000,000     |           91.55        |
 |          16           |          3,000,000     |           45.77        |
 |          32           |          1,500,000     |           22.88        |
 |          64           |            750,000     |           11.44        |
 |         128           |            375,000     |            5.72        |
  -------------------------------------------------------------------------

&par Hardware Description

  - using CFGTMR0 Channel 3 (PD1) to output waveform.
  - using CFGTMR1 Channel 0 (PB2) as input capture channel.

LPUART1 is configured as follows:
    - TX:PC7, RX:PC6
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Transmit enabled

How to connect:
  - PD1 <-----> PB2

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
