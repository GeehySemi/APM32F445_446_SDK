/*!
 * @file        apm32f445_446_interrupt.c
 *
 * @brief       This file provides all the INTERRUPT firmware functions
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

/* Includes */
#include "apm32f445_446_interrupt.h"
#include "startup.h"
/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup INTERRUPT_Driver INTERRUPT Driver
  @{
*/

/** @defgroup INTERRUPT_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

static int32_t g_interruptDisableCount = 0;

#if defined(__ARMCC_VERSION)
    extern uint32_t __VECTOR_RAM;
    extern uint32_t __VECTOR_TABLE;
    extern uint32_t __DATA_ROM;
    extern uint32_t __RAM_START;
    #define VECTOR_RAM                 (uint32_t *)__VECTOR_RAM
    #define VECTOR_TABLE               (uint32_t *)__VECTOR_TABLE
    #define DATA_ROM                   (uint32_t *)__DATA_ROM
    #define RAM_START                  (uint32_t *)__RAM_START

#elif defined(__GNUC__)
    extern uint32_t __VECTOR_RAM;
    extern uint32_t __VECTOR_TABLE11;
    extern uint32_t __ROM_DATA_START;
    extern uint32_t __RAM_DATA_START;
    #define VECTOR_RAM                 (uint32_t *)&__VECTOR_RAM
    #define VECTOR_TABLE               (uint32_t *)&__VECTOR_TABLE11
    #define DATA_ROM                   (uint32_t *)&__ROM_DATA_START
    #define RAM_START                  (uint32_t *)&__RAM_DATA_START

#elif defined(__ICCARM__)
    extern uint32_t __RAM_START[];
    extern uint32_t __VECTOR_RAM[((uint32_t)(FEATURE_INTERRUPT_MAX_IRQ)) + 16U + 1U];
    extern uint32_t __VECTOR_TABLE[((uint32_t)(FEATURE_INTERRUPT_MAX_IRQ)) + 16U + 1U];
    extern uint32_t __DATA_ROM[];
    extern uint32_t __DATA_END[];
    #define RAM_START                  (uint32_t *)__RAM_START
    #define VECTOR_RAM                 (uint32_t *)__VECTOR_RAM
    #define VECTOR_TABLE               (uint32_t *)__VECTOR_TABLE
    #define DATA_ROM                   (uint32_t *)__DATA_ROM
    #define DATA_END                  (uint32_t *)__DATA_END
#endif

/**@} end of group INTERRUPT_Variables*/

/** @defgroup INTERRUPT_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Install an interrupt handler routine for a given IRQ number.
 *
 * @param irqNum: IRQ number
 * @param newHandler: New interrupt handler routine address pointer
 * @param oldHandler: Pointer to a location to store current interrupt handler
 *
 * @retval None
 *
 * @note This method is applicable only if interrupt vector is copied in RAM.
 */
void INT_SYS_InstallHandler(IRQn_Type irqNum, const ISR_T newHandler, ISR_T* const oldHandler)
{
    const uint32_t *vectorRam = VECTOR_RAM;
    const uint32_t *vectorTable = VECTOR_TABLE;


    /* Check whether the vector table is in RAM */
#if ((defined(__ARMCC_VERSION)) || (defined(__GNUC__)))
    if ((vectorRam != vectorTable) || (DATA_ROM > RAM_START))
#elif (defined(__ICCARM__))
    if ((vectorRam != vectorTable) || ((uint32_t)CODE_ROM_SECTION_START > (uint32_t)__RAM_START))
#else
    const uint32_t *dataRom = DATA_ROM;
    const uint32_t *dataRam = DATA_END;
    if ((vectorRam != vectorTable) || (dataRom == dataRam))
#endif
    {
        uint32_t *vectorRam = VECTOR_RAM;

        if (oldHandler != (ISR_T *) 0)
        {
            *oldHandler = (ISR_T)vectorRam;
        }

        /* Set new handler into vector table */
        vectorRam[((int32_t)irqNum) + 16] = (uint32_t)newHandler;
    }
}

/*!
 * @brief Enables an interrupt for a given IRQ number.
 *
 * @param irqNum: IRQ number
 *
 * @retval None
 */
void INT_SYS_EnableIRQ(IRQn_Type irqNum)
{
    uint32_t offset = irqNum >> 5U;
    uint32_t mask = 1UL << ((uint32_t)irqNum & 0x1FUL);
    APM32_NVIC->NVICISER[offset].reg = mask;
}

/*!
 * @brief Disables an interrupt for a given IRQ number.
 *
 * @param irqNum: IRQ number
 *
 * @retval None
 */
void INT_SYS_DisableIRQ(IRQn_Type irqNum)
{
    uint32_t offset = irqNum >> 5U;
    uint32_t mask = 1UL << ((uint32_t)irqNum & 0x1FUL);
    APM32_NVIC->NVICICER[offset].reg = mask;
}

/*!
 * @brief Enables system interrupt.
 *
 * @param None
 *
 * @retval None
 */
void INT_SYS_EnableIRQGlobal(void)
{
    if (g_interruptDisableCount > 0)
    {
        g_interruptDisableCount--;

        if (g_interruptDisableCount <= 0)
        {
            /* Enable the global interrupt */
            INTERRUPTS_ENABLE();
        }
    }
}

/*!
 * @brief Disable system interrupt.
 *
 * @param None
 *
 * @retval None
 */
void INT_SYS_DisableIRQGlobal(void)
{
    /* Disable the global interrupt */
    INTERRUPTS_DISABLE();

    g_interruptDisableCount++;
}

/*!
 * @brief  Configure Interrupt Priority
 *
 * @param  irqNum: Interrupt number.
 * @param  priority: Priority to configure.
 *
 * @retval Node
 */
void INT_SYS_ConfigPriority(IRQn_Type irqNum, uint8_t priority)
{
    uint8_t shift = (uint8_t)(8U - FEATURE_NVIC_PRIO_BITS);

    if ((int32_t)irqNum < 0)
    {
        uint32_t intVector = ((uint32_t)(irqNum) & 0xFU);
        uint32_t reg = intVector >> 2U;

        /* Set pointer to SHPR register */
        volatile uint8_t *shprReg = ((reg == 1U) ? (volatile uint8_t *)&APM32_SCB->SHPR1 :
          ((reg == 2U) ? (volatile uint8_t *)&APM32_SCB->SHPR2 : (volatile uint8_t *)&APM32_SCB->SHPR3));

        /* Set Priority for Cortex-M4 System Interrupts */
        shprReg[intVector & 3U] = (uint8_t)((((uint32_t)priority) << shift) & 0xFFUL);
    }
    else
    {
        /* Set Priority for device specific Interrupts */
        APM32_NVIC->NVICIP[irqNum].reg = (uint8_t)((((uint32_t)priority) << shift) & 0xFFUL);
    }
}

/*!
 * @brief  Read Interrupt Priority
 *
 * @param  irqNum: Interrupt number.
 *
 * @retval Priority of the interrupt.
 */
uint8_t INT_SYS_ReadPriority(IRQn_Type irqNum)
{
    uint8_t priority = 0U;
    uint8_t shift = (uint8_t)(8U - FEATURE_NVIC_PRIO_BITS);

    if ((int32_t)irqNum < 0)
    {
        uint32_t intVector = ((uint32_t)(irqNum) & 0xFU);
        uint32_t reg = intVector >> 2U;

        /* Set pointer to SHPR register */
        volatile const uint8_t *shprReg = ((reg == 1U) ?
          (volatile uint8_t *)&APM32_SCB->SHPR1 : ((reg == 2U) ?
            (volatile uint8_t *)&APM32_SCB->SHPR2 : (volatile uint8_t *)&APM32_SCB->SHPR3));

        /* Get Priority from Cortex-M System Interrupts */
        priority = (uint8_t)(shprReg[intVector & 0x3U] >> (shift));
    }
    else
    {
        /* Get Priority for device specific Interrupts */
        priority = (uint8_t)((APM32_NVIC->NVICIP[(uint32_t)irqNum].reg) >> shift);
    }

    return priority;
}

/*!
 * @brief Configure Pending Interrupt
 *
 * @param irqNum: IRQ number
 *
 * @retval None
 */
void INT_SYS_ConfigPending(IRQn_Type irqNum)
{
    uint32_t offset = (uint32_t)irqNum >> 5U;
    uint32_t mask = (uint32_t)(1UL << ((uint32_t)irqNum & 0x1FUL));
    APM32_NVIC->NVICISPR[offset].reg = mask;
}

/*!
 * @brief Clear Pending Interrupt
 *
 * @param irqNum: IRQ number
 *
 * @retval None
 */
void INT_SYS_ClearPending(IRQn_Type irqNum)
{
    uint32_t offset = (uint32_t)irqNum >> 5U;
    uint32_t mask = (uint32_t)(1UL << ((uint32_t)irqNum & 0x1FUL));
    APM32_NVIC->NVICICPR[offset].reg = mask;
}

/*!
 * @brief Read Pending Interrupt
 *
 * @param irqNum: IRQ number
 *
 * @retval Pending status:
 *         - 0: interrupt is not pending
 *         - 1: interrupt is pending
 */
uint32_t INT_SYS_ReadPending(IRQn_Type irqNum)
{
    uint32_t offset = (uint32_t)irqNum >> 5U;
    uint32_t mask = (uint32_t)(1UL << ((uint32_t)irqNum & 0x1FUL));

    if(((APM32_NVIC->NVICISPR[offset].reg) & mask) != 0UL)
    {
        return 1UL;
    }
    else
    {
        return 0UL;
    }
}

#if FEATURE_INTERRUPT_WITH_ACTIVE_STATE

/*!
 * @brief Read Active Interrupt
 *
 * @param irqNum IRQ number
 *
 * @retval Active status:
 *         - 0: interrupt is not active
 *         - 1: interrupt is active
 */
uint32_t INT_SYS_ReadActive(IRQn_Type irqNum)
{
    uint32_t offset = (uint32_t)irqNum >> 5U;
    uint32_t mask = (uint32_t)(1UL << ((uint32_t)irqNum & 0x1FUL));

    if(((APM32_NVIC->NVICIABR[offset << 2UL].reg) & mask) != 0UL)
    {
        return 1UL;
    }
    else
    {
        return 0UL;
    }
}

#endif /* FEATURE_INTERRUPT_WITH_ACTIVE_STATE */

/**@} end of group INTERRUPT_Functions*/
/**@} end of group INTERRUPT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
