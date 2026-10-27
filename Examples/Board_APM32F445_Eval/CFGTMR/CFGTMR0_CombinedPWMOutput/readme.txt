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

This example shows how to configure the CFGTMR0 peripheral in Combined PWM mode,
and then generate 4 different PWM waveform.

The CFGTMR_CLOCK frequency is set to system clock (48MHz), the prescaler is 2.
So the CFGTMR0 counter clock is 48MHz / 2 = 24MHz.

The phenomenon:
The CFGTMR0_CH0/CFGTMR0_CH1 is the first channel pair, the CFGTMR0_CH2/CFGTMR0_CH3 is the
second channel pair, they are work in the complementary mode with a 50% duty cycle
and 20KHz frequency.
The second channel pair (CFGTMR0_CH2/CFGTMR0_CH3) is phase-shifted by 90° to the first
channel pair (CFGTMR0_CH0/CFGTMR0_CH1).

&par Hardware Description

  - using CFGTMR0 Channel 0 (PB12) to output waveform.
  - using CFGTMR0 Channel 1 (PB13) to output waveform.
  - using CFGTMR0 Channel 2 (PB14) to output waveform.
  - using CFGTMR0 Channel 3 (PB15) to output waveform.

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F445 EVAL Devices.
