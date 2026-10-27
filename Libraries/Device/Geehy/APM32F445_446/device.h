/*!
 * @file        device.h
 *
 * @brief       Includes the mcu specific register header files.
 *
 * @details     This file includes the mcu specific register header files.
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

#ifndef DEVICE_H
#define DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446
  @{
*/

/** @addtogroup DEVICE
  * @{
  */
#if defined (APM32F445)
    #include "APM32F445/apm32f445.h"
    #include "APM32F445/apm32f445_features.h"

#elif defined (APM32F446)
    #include "APM32F446/apm32f446.h"
    #include "APM32F446/apm32f446_features.h"
#else
    #error "No valid MCU defined!"
#endif


#ifdef __cplusplus
}
#endif
/**@} end of group DEVICE*/
/**@} end of group APM32F445_446*/

#endif /* DEVICE_H */





