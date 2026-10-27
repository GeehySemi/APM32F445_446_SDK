/*!
 * @file        startup.h
 *
 * @brief       Header of Startup File
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

#ifndef STARTUP_H
#define STARTUP_H

#include "stdint.h"
#include "device.h"
#include "apm_core_cm4.h"

/** @addtogroup STARTUP
  @{
*/

/** @addtogroup Startup
  @{
*/

/** @defgroup Startup_Macros Macros
  @{
*/

/*!
 * @brief define symbols that specific start and end addres of some basic sections.
 */
#if (defined(APM32F445_446_SERIES))
    #if (defined(__ARMCC_VERSION))
        extern uint32_t __ROM_VECTOR_END;
        extern uint32_t __ROM_VECTOR_START;
        extern uint32_t __BSS_END;
        extern uint32_t __BSS_START;
        extern uint32_t __RAM_DATA_END;
        extern uint32_t __RAM_DATA_START;
        extern uint32_t __USER_SECTION_END;
        extern uint32_t __USER_SECTION_START;
        extern uint32_t __RAM_CODE_END;
        extern uint32_t __RAM_CODE_START;

        #define END_INTERRUPTS_SECTION                 (uint8_t *)__ROM_VECTOR_END
        #define START_INTERRUPTS_SECTION               (uint8_t *)__ROM_VECTOR_START
        #define END_BSS_SECTION                        (uint8_t *)__BSS_END
        #define START_BSS_SECTION                      (uint8_t *)__BSS_START
        #define END_DATA_SECTION                       (uint8_t *)__RAM_DATA_END
        #define START_DATA_SECTION                     (uint8_t *)__RAM_DATA_START
        #define END_USERSECTION_SECTION                (uint8_t *)__USER_SECTION_END
        #define START_USERSECTION_SECTION              (uint8_t *)__USER_SECTION_START
        #define END_CODE_RAM_SECTION                   (uint8_t *)__RAM_CODE_END
        #define START_CODE_RAM_SECTION                 (uint8_t *)__RAM_CODE_START

    #elif (defined(__ICCARM__))
        #define END_INTERRUPTS_SECTION                 __section_end(".intvector")
        #define START_INTERRUPTS_SECTION               __section_begin(".intvector")
        #define END_BSS_SECTION                        __section_end(".bss")
        #define START_BSS_SECTION                      __section_begin(".bss")
        #define END_DATA_SECTION                       __section_end(".data")
        #define START_DATA_SECTION                     __section_begin(".data")
        #define END_USERSECTION_SECTION                __section_end(".userSection")
        #define START_USERSECTION_SECTION              __section_begin(".userSection")
        #define END_CODE_RAM_SECTION                   __section_end("__RAM_CODE")
        #define START_CODE_RAM_SECTION                 __section_begin("__RAM_CODE")
        #define DATA_INIT_SECTION_END                  __section_end(".data_init")
        #define DATA_INIT_SECTION_START                __section_begin(".data_init")
        #define CODE_ROM_SECTION_END                   __section_end("__ROM_CODE")
        #define CODE_ROM_SECTION_START                 __section_begin("__ROM_CODE")

    #else

    #endif
#endif

#if (defined(__ICCARM__))
    #pragma section = ".intvector"
    #pragma section = ".bss"
    #pragma section = "__ROM_CODE"
    #pragma section = "__RAM_CODE"
    #pragma section = "__ROM_CODE"
    #pragma section = ".data_init"
    #pragma section = ".data"
    #pragma section = ".userSection_init"
    #pragma section = ".userSection"
#endif

/**@} end of group Startup_Macros*/

/** @defgroup Startup_Functions Functions
  @{
*/

/*!
 * @brief Make necessary initializations for RAM.
 *
 * - Copy initialized data from ROM to RAM.
 * - Clear the zero-initialized data section.
 * - Copy the vector table from ROM to RAM. This could be an option.
 */
void bss_data_init(void);

/**@} end of group Startup_Functions*/
/**@} end of group Startup*/
/**@} end of group STARTUP*/

#endif /* STARTUP_H*/

