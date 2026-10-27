/*!
 * @file        user_can_config.c
 *
 * @brief       CAN configurations
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

/* Includes */
#include "user_can_config.h"

/** @addtogroup APM32F446_Examples
  @{
*/

/** @addtogroup CAN_FdFrames
  @{
*/

/** @defgroup CAN_FdFrames_Variables Variables
  @{
*/

/* Configuration for the CAN controller */
CAN_USER_CONFIG_T g_canConfig = {
    .maxMbNumber = 2,
    .operationMode = CAN_MODE_NORMAL,
    .rxFifoEnable = false,
    .rxFifoFilterNum = CAN_RXFIFO_ID_FILTERS_8,
    .rxFifoTransferType = CAN_RXFIFO_USE_INTERRUPT,
    .rxFifoDmaChannel = 0,
    .fdEnable = true,
    .fdPayloadSize = CAN_FD_PAYLOAD_SIZE_64,
    .clockSource = CAN_CLOCK_SOURCE_OSC,
    .bitrate = {
        .preDiv = 0,
        .propSeg = 7,
        .phaseSeg1 = 4,
        .phaseSeg2 = 1,
        .resyncJumpWidth = 1
    },
    .fdBitrate = {
        .preDiv = 0,
        .propSeg = 3,
        .phaseSeg1 = 1,
        .phaseSeg2 = 1,
        .resyncJumpWidth = 1
    }
};

/**@} end of group CAN_FdFrames_Variables*/
/**@} end of group CAN_FdFrames*/
/**@} end of group Examples*/
