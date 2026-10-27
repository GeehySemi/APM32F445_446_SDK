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

This example shows how to configure the fault-control feature of the CFGTMR.
In this example, two CFGTMR modules are initiated, while CFGTMR0 is configured
to generate the edge-align PWM signal. The CFGTMR1 is used to generate the fault
PWM signal, and then to affects the PWM output of the CFGTMR0 channels externally
through the fault input pin PE3.

&par Hardware Description

  - using CFGTMR0 Channel 0 (PB12) to output waveform.
  - using CFGTMR0 Channel 1 (PB13) to output waveform.
  - using CFGTMR0 Channel 2 (PB14) to output waveform.
  - using CFGTMR0 Channel 3 (PB15) to output waveform.

  - using CFGTMR0 fault input channel 0 (PE3).

  - using CFGTMR1 Channel 0 (PB2) to output fault signal.

How to connect:
  - PE3 <-----> PB2

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F445 EVAL Devices.
