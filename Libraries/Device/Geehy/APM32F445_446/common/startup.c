/*
 * modified by ARM
 */
/*!
 * @file        startup.c
 *
 * @brief       Startup File
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


#include "startup.h"

/** @addtogroup STARTUP
  @{
*/

/** @addtogroup Startup
  @{
*/

/** @defgroup Startup_Variables Variables
  @{
*/
static volatile uint32_t * const g_vectorsRegAddr[CORES_COUNT] = FEATURE_INTERRUPT_INT_VECTORS_REG;

/**@} end of group Startup_Variables*/

/** @defgroup Startup_Functions Functions
  @{
*/

/**
 * @brief Make necessary initializations for RAM.
 *
 * @param None.
 *
 * @retval None.
 */
void bss_data_init(void)
{
    uint32_t i;
    uint8_t coreIndex;

#if !(defined(__ARMCC_VERSION))
    const uint8_t * endBssPtr;
    const uint8_t * romCodePtr, * romCodeEndPtr;
    const uint8_t * romDataPtr, * romDataEndPtr;
    const uint8_t * romUserPtr, * romUserEndPtr;
    uint8_t * startBssPtr;
    uint8_t * ramCodePtr;
    uint8_t * ramDataPtr;
    uint8_t * ramUserPtr;

#endif

#if ((defined(__ARMCC_VERSION)) || (defined(__GNUC__)))
    extern uint32_t __VECTOR_ROM;
    extern uint32_t __RAM_VECTOR_TABLE_SIZE;
    extern uint32_t __VECTOR_RAM;
#else
    extern uint32_t __VECTOR_TABLE[];
    extern uint32_t __RAM_VECTOR_TABLE_SIZE[];
    extern uint32_t __VECTOR_RAM[];
#endif


#if defined(__ARMCC_VERSION)
    /* VECTOR TABLE*/
    uint32_t * romVector    = (uint32_t *)__VECTOR_ROM;
    uint8_t * sizeVectorTable = (uint8_t *)__RAM_VECTOR_TABLE_SIZE;
    uint32_t * ramVector    = (uint32_t *)__VECTOR_RAM;
#elif defined(__GNUC__)
    extern uint32_t __BSS_END;
    extern uint32_t __BSS_START;
    extern uint32_t __ROM_DATA_END;
    extern uint32_t __ROM_DATA_START;
    extern uint32_t __RAM_DATA_START;
    extern uint32_t __USER_ROM_END;
    extern uint32_t __USER_ROM_START;
    extern uint32_t __USER_SECTION_RAM_START;
    extern uint32_t __ROMCODE_END;
    extern uint32_t __ROMCODE_START;
    extern uint32_t __RAM_CODE_START;
    /* BSS */
    startBssPtr       = (uint8_t *)&__BSS_START;
    endBssPtr         = (uint8_t *)&__BSS_END;

    /* CODE RAM */
    romCodePtr        = (uint8_t *)&__ROMCODE_START;
    romCodeEndPtr    = (uint8_t *)&__ROMCODE_END;
    ramCodePtr        = (uint8_t *)&__RAM_CODE_START;

    /* Data */
    romDataPtr        = (uint8_t *)&__ROM_DATA_START;
    romDataEndPtr    = (uint8_t *)&__ROM_DATA_END;
    ramDataPtr        = (uint8_t *)&__RAM_DATA_START;

    /* Custom section */
    romUserEndPtr  = (uint8_t *)&__USER_ROM_END;
    romUserPtr      = (uint8_t *)&__USER_ROM_START;
    ramUserPtr      = (uint8_t *)&__USER_SECTION_RAM_START;

    /* VECTOR TABLE*/
    uint32_t * romVector    = (uint32_t *)&__VECTOR_ROM;
    uint8_t * sizeVectorTable = (uint8_t *)&__RAM_VECTOR_TABLE_SIZE;
    uint32_t * ramVector    = (uint32_t *)&__VECTOR_RAM;

#elif defined (__ICCARM__)
    /* Data */
    romDataPtr        = __section_begin(".data_init");
    romDataEndPtr     = __section_end(".data_init");
    ramDataPtr        = __section_begin(".data");

    /* RAM CODE */
    #pragma section = "__RAM_CODE"
    #pragma section = "__ROM_CODE"
    romCodePtr        = __section_begin("__ROM_CODE");
    romCodeEndPtr    = __section_end("__ROM_CODE");
    ramCodePtr        = __section_begin("__RAM_CODE");

    /* BSS */
    startBssPtr       = __section_begin(".bss");
    endBssPtr         = __section_end(".bss");

    romUserPtr      = __section_begin(".userSection_init");
    romUserEndPtr  = __section_end(".userSection_init");
    ramUserPtr      = __section_begin(".userSection");

#else

#endif

#if !(defined(__ARMCC_VERSION))
    while(endBssPtr != startBssPtr)
    {
        *startBssPtr = 0;
        startBssPtr++;
    }

    while (romCodeEndPtr != romCodePtr)
    {
        *ramCodePtr = *romCodePtr;
        ramCodePtr++;
        romCodePtr++;
    }

    while (romDataEndPtr != romDataPtr)
    {
        *ramDataPtr = *romDataPtr;
        ramDataPtr++;
        romDataPtr++;
    }

    while(romUserEndPtr != romUserPtr)
    {
        *ramUserPtr = *romUserPtr;
        romUserPtr++;
        ramUserPtr++;
    }
#endif

    coreIndex = (uint8_t)GET_CORE_ID();

#if defined(__ARMCC_VERSION)
    if (ramVector == romVector)
    {
        *g_vectorsRegAddr[coreIndex] = (uint32_t) __VECTOR_ROM;
    }
    else
    {
        for (i = 0; i < (((uint32_t)(sizeVectorTable))/sizeof(uint32_t)); i++)
        {
            ramVector[i] = romVector[i];
        }
        *g_vectorsRegAddr[coreIndex] = (uint32_t) __VECTOR_RAM;
    }
#elif defined(__GNUC__)
    if (ramVector == romVector)
        {
            *g_vectorsRegAddr[coreIndex] = (uint32_t) &__VECTOR_ROM;
        }
        else
        {
            for (i = 0; i < (((uint32_t)(sizeVectorTable))/sizeof(uint32_t)); i++)
            {
                ramVector[i] = romVector[i];
            }
            *g_vectorsRegAddr[coreIndex] = (uint32_t) &__VECTOR_RAM;
        }
#else
    if (__VECTOR_RAM == __VECTOR_TABLE)
    {
        *g_vectorsRegAddr[coreIndex] = (uint32_t)__VECTOR_TABLE;
    }
    else
    {
        for (i = 0; i < (((uint32_t)__RAM_VECTOR_TABLE_SIZE)/sizeof(uint32_t)); i++)
        {
            __VECTOR_RAM[i] = __VECTOR_TABLE[i];
        }
        *g_vectorsRegAddr[coreIndex] = (uint32_t)__VECTOR_RAM;
    }
#endif

}

/**@} end of group Startup_Functions*/
/**@} end of group Startup*/
/**@} end of group STARTUP*/

