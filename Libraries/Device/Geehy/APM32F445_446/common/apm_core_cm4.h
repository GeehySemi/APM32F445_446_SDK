/*!
 * @file        apm_core_cm4.h
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

/*
 * Tool Chains:
 *   GNUC flag is defined also by ARM compiler - it shows the current major version of the compatible GCC version
 *   __GNUC__   : GNU Compiler Collection
 *   __ghs__    : Green Hills ARM Compiler
 *   __ICCARM__ : IAR ARM Compiler
 *   __DCC__    : Wind River Diab Compiler
 *   __ARMCC_VERSION:  ARM Compiler
 */

#if !defined (CORE_CM4_H)
#define CORE_CM4_H


#ifdef __cplusplus
extern "C" {
#endif

/* Get Core ID */
#define GET_CORE_ID()   0U

/* Endianness */
#define CORE_LITTLE_ENDIAN

/*  NOP */
#define NOP() __asm volatile ("nop")

/* Data alignment */
#if defined ( __ICCARM__ )
    #define stringify(s) tostring(s)
    #define tostring(s) #s
    #define ALIGN(x)      _Pragma(stringify(data_alignment=x))
#elif defined (__ARMCC_VERSION)
    #define ALIGN(x)      __attribute__((aligned(x)))
#else

#endif

/* BKPT_ASM  Macro to be used to trigger an debug interrupt */
#define BKPT_ASM __asm("BKPT #0\n\t")

/* Interrupts Enable */
#if defined (__GNUC__)
#define INTERRUPTS_ENABLE() __asm volatile ("cpsie i" : : : "memory");
#else
#define INTERRUPTS_ENABLE() __asm("cpsie i")
#endif


/* Interrupts Disable */
#if defined (__GNUC__)
#define INTERRUPTS_DISABLE() __asm volatile ("cpsid i" : : : "memory");
#else
#define INTERRUPTS_DISABLE() __asm("cpsid i")
#endif

/* Enable FPU  ENABLE_FPU indicates whether SystemInit will enable the Floating point unit (FPU) */
#if defined (__ICCARM__)
#if defined __ARMVFP__
#define ENABLE_FPU
#endif

#elif defined (__ARMCC_VERSION) || defined (__GNUC__)
#if defined (__VFP_FP__) && !defined (__SOFTFP__)
#define ENABLE_FPU
#endif
#endif /* if defined (__ICCARM__) */


/* Enter low-power standby state  WFI (Wait For Interrupt) makes the processor suspend execution (Clock is stopped)
 * until an IRQ interrupts.
 */
#if defined (__GNUC__)
#define WFI() __asm volatile ("wfi")
#else
#define WFI() __asm("wfi")
#endif

/* Reverse byte order in each halfword independently */
#if defined (__ARMCC_VERSION) || defined (__ICCARM__) || defined (__GNUC__)
#define REV_BYTES_16(a, b) __asm volatile ("rev16 %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_16(a, b) (b = ((a & 0xFF000000U) >> 8U) | ((a & 0xFF0000U) << 8U) \
                                | ((a & 0xFF00U) >> 8U) | ((a & 0xFFU) << 8U))
#endif

/* Reverse byte order in a word */
#if defined (__ARMCC_VERSION) || defined (__ICCARM__) || defined (__GNUC__)
#define REV_BYTES_32(a, b) __asm volatile ("rev %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_32(a, b) (b = ((a & 0xFF000000U) >> 24U) | ((a & 0xFF0000U) >> 8U) \
                                | ((a & 0xFF00U) << 8U) | ((a & 0xFFU) << 24U))
#endif

/* Places a function in RAM */
    /* Declaration */
#if defined (__ARMCC_VERSION) || defined ( __GNUC__ )
    #define FUNCTION_DECLARATION_AT_RAMSECTION_START
    #define FUNCTION_DECLARATION_AT_RAMSECTION_END        __attribute__((section (".code_ram")));
#elif defined ( __ICCARM__ )
    #define FUNCTION_DECLARATION_AT_RAMSECTION_START      __ramfunc
    #define FUNCTION_DECLARATION_AT_RAMSECTION_END        ;
#else
    /* Keep compatibility with software analysis tools */
    #define FUNCTION_DECLARATION_AT_RAMSECTION_START
    #define FUNCTION_DECLARATION_AT_RAMSECTION_END        ;
#endif

    /* Definition */
    #define FUNCTION_DEFINITION_AT_RAMSECTION_START
    #define FUNCTION_DEFINITION_AT_RAMSECTION_END

#if defined (__ICCARM__)
    #define FUNCTION_CALL_AT_RAMSECTION_CHECK_DISABLE     _Pragma("diag_suppress=Ta022")
    #define FUNCTION_CALL_AT_RAMSECTION_CHECK_ENABLE      _Pragma("diag_default=Ta022")
#else
    #define FUNCTION_CALL_AT_RAMSECTION_CHECK_DISABLE
    #define FUNCTION_CALL_AT_RAMSECTION_CHECK_ENABLE
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORE_CM4_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
