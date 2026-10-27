/*!
 * @file        main.c
 *
 * @brief       Main program
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
#include "user_config.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CRC_Checksum
  @{
  */

/** @defgroup CRC_Checksum_Macros Macros
  @{
  */

/* CRC data size */
#define CRC_DATA_LENGTH     (137U)

/* CRC calculation results */
#define RESULT_CRC_16       (0x502BU)
#define RESULT_CRC_32       (0x0597117AU)

/**@} end of group CRC_Checksum_Macros */

/** @defgroup CRC_Checksum_Variables Variables
  @{
  */

uint32_t g_crcDataInput[] = {
    0x14567DF0, 0xDD3423CC, 0xABCDEF32, 0x8877EE22, 0x66DD22AA,
    0x234512EF, 0x87D3258D, 0x2C111133, 0x4C418200, 0x30000800,
    0x10900008, 0x00308038, 0x05400001, 0x42004184, 0x30000816,
    0x0680543E, 0xC4500201, 0x80B0000C, 0x43A50000, 0x90B0000C,
    0x00408228, 0x00816300, 0xB080000C, 0x054007BF, 0x20418184,
    0x003FE020, 0x0148007C, 0x30816000, 0xB3800004, 0x10900008,
    0x00816108, 0x00800B00, 0xB05407BD, 0x24180014, 0xBE540FFE,
    0x00380B02, 0x0090010C, 0x00480014, 0x05400739, 0x2041800C,
    0x00380001, 0x0090010C, 0x1080000C, 0x054007FF, 0x20418048,
    0x00816108, 0x00800B00, 0x0054056B, 0x2418FFF4, 0x10816008,
    0xFF3C00FF, 0x1F6000F7, 0x10818008, 0xC8180000, 0xC0618001,
    0x007D8C38, 0x00918B00, 0x10818008, 0x18160008, 0xB0816000,
    0x587C0038, 0x00900C00, 0x1080000C, 0x054007BD, 0x20418080,
    0x00816108, 0xFF3C00FF, 0x01600FFE, 0x18180008, 0xC0818000,
    0x607C0038, 0x00900B00, 0x10816008, 0xB8000000, 0x0054056B,
    0xFF4182F4, 0x00816108, 0x0F3C0FFF, 0x06001FEF, 0x0F3FCFFF,
    0x1F63DEFD, 0x00818108, 0xC0818000, 0x07C06038, 0xB0900000,
    0x00818108, 0x00816108, 0xB0800000, 0x06000001, 0x0F7C0038,
    0x00900C00, 0x00818108, 0x10816008, 0xB8000000, 0x0F7C0038,
    0x00900C00, 0x00816108, 0x0F3C0FFF, 0x06001FFE, 0x10818008,
    0x00818C00, 0x607C0038, 0xB0900000, 0x18160008, 0xB0800000,
    0x0554006B, 0xFF4182F4, 0x10816008, 0x03FCFFFF, 0xE163DFEF,
    0xFF3C00FF, 0x1F6000FB, 0x10818008, 0xC8180000, 0xCF7D8038,
    0x00918B00, 0x00818108, 0x10816008, 0xB8160000, 0x057C0838,
    0x00900C00, 0x0088033C, 0x002C0000, 0x24180014, 0x3F7FEB78,
    0x00600000, 0x00000000, 0x00600000, 0x37FEFB78, 0x10800024,
    0x00BBC118, 0x037C08A6, 0x10382020, 0x04E80020, 0x044D5335,
    0x41354631, 0x00313045
};

/**@} end of group CRC_Checksum_Variables */

/** @defgroup CRC_Checksum_Functions Functions
  @{
  */

int main(void)
{
    uint32_t result;
    bool exit = false;

    /* Initializes clocks for CRC instance used */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr, CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    /* Initialize pins for Keys and LEDs */
    LED_Init();
    BTN_Init();

    /* Initialize CRC module */
    CRC_Init(INST_CRC1, &g_crcConfig0);

    /* Infinite loop */
    for (;;)
    {
        if (BTN_GetState(BUTTON_KEY1) == 0)
        {
            /* Calculate CRC value with 16 bit width mode */
            CRC_WriteData(INST_CRC1, (const uint32_t *)g_crcDataInput, CRC_DATA_LENGTH);

            /* If CRC calculation result is correct */
            result = CRC_GetCrcResult(INST_CRC1);
            if (result == RESULT_CRC_16)
            {
                /* Calculate CRC value with 32 bit width mode */
                CRC_Init(INST_CRC1, &g_crcConfig1);
                CRC_WriteData(INST_CRC1, (const uint32_t *)g_crcDataInput, CRC_DATA_LENGTH);

                result = CRC_GetCrcResult(INST_CRC1);
                if (result == RESULT_CRC_32)
                {
                    LED_On(LED_GREEN);
                }
                else
                {
                    LED_On(LED_RED);
                }
            }
            else
            {
                LED_On(LED_RED);
            }
            break;
        }
    }

    while (!exit);
}

/**@} end of group CRC_Checksum_Functions */
/**@} end of group CRC_Checksum */
/**@} end of group Examples */
