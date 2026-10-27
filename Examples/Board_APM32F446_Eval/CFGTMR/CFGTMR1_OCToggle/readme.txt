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

This example shows how to configure the CFGTMR1 peripheral in output compare
toggle mode, and then to generate the toggle signal.
The CFGTMR_CLOCK frequency is set to system clock (48MHz), the prescaler is 1.
So the CFGTMR1 counter clock is 48MHz / 1 = 48MHz.

The phenomenon:
Channel update rate = CFGTMR1 counter clock / maxCountValue / 2 = 48MHz / 24000 / 2 = 1KHz.

CFGTMR1_CH0(PB2)  delay: Compare value / CFGTMR1 counter clock = 0    / 48MHz = 0us.
CFGTMR1_CH1(PB3)  delay: Compare value / CFGTMR1 counter clock = 1200 / 48MHz = 25us.
CFGTMR1_CH2(PA15) delay: Compare value / CFGTMR1 counter clock = 2400 / 48MHz = 50us.

&par Hardware Description

  - using CFGTMR1 Channel 1 (PB2)  to output signal.
  - using CFGTMR1 Channel 2 (PB3)  to output signal.
  - using CFGTMR1 Channel 3 (PA15) to output signal.

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
