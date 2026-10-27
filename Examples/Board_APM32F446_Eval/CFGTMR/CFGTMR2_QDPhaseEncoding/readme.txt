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

This example shows how to configure the CFGTMR2 peripheral in the quadrature decoder
mode of the Phase A and Phase B encoding sub-mode.

Configuration step:
  - Configurate CFGTMR0 to generate the quadrature signals, as CFGTMR2 A/B phase input signals.
  - Configurate CFGTMR2 in the phase encoding mode of the quadrature decoder mode.
  - CFGTMR2 Counter initial value 0, maximum value is 10.
  - Every time 10 rising/falling edges of the Phase A and Phase B signals are detected,
    the CFGTMR2 counter generate overflow interrupt. Then PD1 PIN output toggle.

The phenomenon:
You can use an oscilloscope to observe the relationship between the A/B phase signal
and the PD1 PIN output signal.

&par Hardware Description

  - PB12: CFGTMR0_CH0 output Phase A signal.
  - PB14: CFGTMR0_CH2 output Phase B signal.
  - PD10: CFGTMR2 Phase B input.
  - PD11: CFGTMR2 Phase A input.
  - PD1 : Output level toggle PIN, used to detect the rising/falling edges
          of the Phase A and Phase B signals.

How to connect:
  - PB12 <-----> PD11
  - PB14 <-----> PD10

&par Directory contents

&par IDE environment

  - MDK-ARM V5.36.0.0
  - EWARM V8.50.5.26295
  - Eclipse V2022-06(4.24.0)

&par Hardware and Software environment

  - This example runs on APM32F446 EVAL Devices.
