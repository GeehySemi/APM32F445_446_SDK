/*!
 * @file        system_apm32f445_446.h
 *
 * @brief       Header of System Source File
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


#ifndef SYSTEM_APM32F445_446_H_
#define SYSTEM_APM32F445_446_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup STARTUP
  @{
*/

/** @addtogroup APM32F445_446_System
  * @brief APM32F445_446 system configuration
  @{
*/

/** @defgroup APM32F445_446_System_Macros Macros
  @{
*/

/**
 * CPU Settings.
 */

/* Cache enablement  */
#ifndef ICACHE_ENABLE
#define ICACHE_ENABLE                 0U
#endif

/* Watchdog disable */
#ifndef WDT_DISABLE
  #define WDT_DISABLE                 1U
#endif

/* System clock default value */
#ifndef DEFAULT_SYSTEM_CLOCK
 #define DEFAULT_SYSTEM_CLOCK                48000000U
#endif

/* The external crystal or oscillator clock frequency */
#ifndef CPU_EXTERNAL_CLK_HZ
  #define CPU_EXTERNAL_CLK_HZ                8000000U
#endif

/* The high speed internal oscillator clock frequency */
#ifndef CPU_INTERNAL_HSI_CLK_HZ
  #define CPU_INTERNAL_HSI_CLK_HZ            48000000U
#endif


/**@} end of group APM32F445_446_System_Macros*/

/** @defgroup APM32F445_446_System_Functions Functions
  @{
*/

/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;


void SystemInit(void);
void SystemCoreClockUpdate(void);
void SystemCoreClockConfigure(void);
void SystemSoftwareReset(void);

/**@} end of group APM32F445_446_System_Functions */
/**@} end of group APM32F445_446_System */
/**@} end of group CMSIS */

#ifdef __cplusplus
}
#endif


#endif  /* #if !defined(SYSTEM_APM32F445_446_H_) */
