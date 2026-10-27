/*!
 * @file        apm32f445_446_erep.c
 *
 * @brief       This file provides all the EREP firmware functions
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
#include "apm32f445_446_erep.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup EREP_Driver CLOCK Driver
  @{
*/

/** @defgroup EREP_Macros Macros
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/* ERM - Size of Registers Arrays */
#define EREP_COUNT                (2U)

/* The distance between channels */
#define EREP_CHANNELS_OFFSET_SIZE (4U)

/**@} end of group EREP_Macros*/

/** @defgroup EREP_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/* Table of base addresses for EREP instances */
static EREP_T *const s_erepBasePointer[1] = { EREP };

/**@} end of group EREP_Variables*/

/** @defgroup EREP_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configures the EREP module.
 *
 * @param ins: The EREP instance number
 * @param channelCnt: The number of channels
 * @param userConfigArray: The pointer to the array of EREP user configure structure
 *
 * @retval None
 */
void EREP_Init(uint32_t ins, uint8_t channelCnt, const EREP_USER_CONFIG_T *userConfigArray)
{
    EREP_INSTANCE_VALIDITY(ins);

    EREP_T *erepBase = s_erepBasePointer[ins];
    uint8_t idx = 0;

    /* Initializes the module */
    EREP_HW_Setup(erepBase);

    /* Config interrupt notification from user configuration input */
    for (idx = 0U; idx < channelCnt; idx++)
    {
        EREP_ConfigInterruptCfg(ins, userConfigArray[idx].channel, *userConfigArray[idx].interruptCfg);
    }
}

/*!
 * @brief Reset EREP to default configuration
 *
 * @param ins: The EREP instance number
 *
 * @retval None
 */
void EREP_DeInit(uint32_t ins)
{
    EREP_INSTANCE_VALIDITY(ins);

    EREP_T *erepBase = s_erepBasePointer[ins];

    /* Reset the default configuration */
    EREP_HW_Setup(erepBase);
}

/*!
 * @brief Config interrupt notification.
 *
 * @param ins: The EREP instance number
 * @param channel: The configured memory channel
 * @param interruptCfg: The EREP interrupt configuration structure
 *
 * @retval None
 */
void EREP_ConfigInterruptCfg(uint32_t ins, uint8_t channel, EREP_INTERRUPT_CONFIG_T interruptCfg)
{
    EREP_INSTANCE_VALIDITY(ins);

    EREP_T *erepBase = s_erepBasePointer[ins];

    /* Set interrupt notification base on interrupt notification configuration input */
    EREP_HW_StartEventInterrupt(erepBase,
                                channel,
                                EREP_EVENT_NON_CORRECTABLE,
                                interruptCfg.enableNonCorrectable);
    EREP_HW_StartEventInterrupt(erepBase,
                                channel,
                                EREP_EVENT_SINGLE_BIT,
                                interruptCfg.enableSingleCorrection);
}

/*!
 * @brief Read interrupt notification.
 *
 * @param ins: The EREP instance number
 * @param channel: The examined memory channel
 * @param interruptPtr The pointer to the EREP interrupt configuration structure
 *
 * @retval None
 */
void EREP_ReadInterruptCfg(uint32_t ins, uint8_t channel, EREP_INTERRUPT_CONFIG_T *const interruptPtr)
{
    EREP_INSTANCE_VALIDITY(ins);

    const EREP_T *erepBase = s_erepBasePointer[ins];

    /* Read interrupt notification into interrupt notification configuration input */
    interruptPtr->enableNonCorrectable =
      EREP_HW_EventInterruptVerify(erepBase, channel, EREP_EVENT_NON_CORRECTABLE);
    interruptPtr->enableSingleCorrection =
      EREP_HW_EventInterruptVerify(erepBase, channel, EREP_EVENT_SINGLE_BIT);
}

/*!
 * @brief Clears error event and the corresponding interrupt notification.
 *
 * @param ins: The EREP instance number
 * @param channel: The configured memory channel
 * @param eccEvent: The types of ECC events
 *
 * @retval None
 */
void EREP_ClearEvent(uint32_t ins, uint8_t channel, EREP_ECC_ENEVT_T eccEvent)
{
    EREP_INSTANCE_VALIDITY(ins);

    EREP_T *erepBase = s_erepBasePointer[ins];

    /* Non-correctable */
    if (eccEvent == EREP_EVENT_NON_CORRECTABLE)
    {
        EREP_HW_ClearEventDouble(erepBase, channel);
    }
    /* Single-bit correction */
    else if (eccEvent == EREP_EVENT_SINGLE_BIT)
    {
        EREP_HW_ClearEventSingle(erepBase, channel);
    }
    else
    {
        /* Do nothing */
    }
}

/*!
 * @brief Read the address of the last ECC event in Memory n and ECC event.
 *
 * @param ins: The ERM instance number
 * @param channel: The examined memory channel
 * @param addressPtr The pointer to address of the last ECC event in Memory n with ECC event
 *
 * @retval The last occurred ECC event
 */
EREP_ECC_ENEVT_T EREP_ReadErrorDetail(uint32_t ins, uint8_t channel, uint32_t *addressPtr)
{
    EREP_INSTANCE_VALIDITY(ins);

    const EREP_T *erepBase = s_erepBasePointer[ins];
    EREP_ECC_ENEVT_T eccEvent;

    /* Non-correctable ECC events */
    if (EREP_HW_EventIdentified(erepBase, channel, EREP_EVENT_NON_CORRECTABLE) != false)
    {
        eccEvent = EREP_EVENT_NON_CORRECTABLE;
    }
    /* Single-bit correction ECC events */
    else if (EREP_HW_EventIdentified(erepBase, channel, EREP_EVENT_SINGLE_BIT) != false)
    {
        eccEvent = EREP_EVENT_SINGLE_BIT;
    }
    /* None ECC events */
    else
    {
        eccEvent = EREP_EVENT_NONE;
    }

    /* Gets address */
    *addressPtr = EREP_HW_ReadLastErrorAddress(erepBase, channel);

    return eccEvent;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enables or disable Memory n interrupt event.
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The configured memory channel
 * @param eccEvent: The configured event
 * @param enable: Enable or disable interrupt events
 *
 * @retval None
 */
void EREP_HW_StartEventInterrupt(EREP_T *const erepBase,
                                 uint8_t channel,
                                 EREP_ECC_ENEVT_T eccEvent,
                                 bool enable)
{
    uint32_t tempCtrl = erepBase->CFG0.reg;
    /* Single-bit correction */
    if (eccEvent == EREP_EVENT_SINGLE_BIT)
    {
        if (enable)
        {
            tempCtrl |= 0x80000000U >>
              (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE));
        }
        else
        {
            tempCtrl &= ~(0x80000000U >>
                          ~(channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
        }
    }
    /* Non-correctable */
    else if (eccEvent == EREP_EVENT_NON_CORRECTABLE)
    {
        if (enable)
        {
            tempCtrl |= 0x40000000U >>
              (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE));
        }
        else
        {
            tempCtrl &= ~(0x40000000U >>
                          ~(channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
        }
    }
    else
    {
        /* Do nothing */
    }
    erepBase->CFG0.reg = tempCtrl;
}

/*!
 * @brief Checks if the Memory n interrupt event is enabled.
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The examined memory channel
 * @param eccEvent: The examined event
 *
 * @retval Interrupt event
 *         - true: Interrupt event is enabled
 *         - false: Interrupt event is disabled
 */
uint8_t EREP_HW_EventInterruptVerify(const EREP_T *const erepBase, uint8_t channel, EREP_ECC_ENEVT_T eccEvent)
{
    uint32_t retVal = 0U;

    /* Single-bit correction */
    if (eccEvent == EREP_EVENT_SINGLE_BIT)
    {
        retVal = erepBase->CFG0.reg &
          (0x80000000U >> (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
    }
    /* Non-correctable */
    else if (eccEvent == EREP_EVENT_NON_CORRECTABLE)
    {
        retVal = erepBase->CFG0.reg &
          (0x40000000u >> (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
    }
    else
    {
        /* Do nothing */
    }
    return retVal != 0U;
}

/*!
 * @brief Checks if the Memory n error event is detected.
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The examined memory channel
 * @param eccEvent: The examined event
 *
 * @retval The status of Memory n error event
 *         - true: Interrupt event is enabled
 *         - false: Interrupt event is disabled
 */
bool EREP_HW_EventIdentified(const EREP_T *const erepBase, uint8_t channel, EREP_ECC_ENEVT_T eccEvent)
{
    uint32_t retVal = 0U;
    /* Single-bit correction */
    if (eccEvent == EREP_EVENT_SINGLE_BIT)
    {
        retVal = erepBase->STS0.reg &
          (0x80000000U >> (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
    }
    /* Non-correctable */
    else if (eccEvent == EREP_EVENT_NON_CORRECTABLE)
    {
        retVal = erepBase->STS0.reg &
          (0x40000000u >> (channel ? (1U * EREP_CHANNELS_OFFSET_SIZE) : (0U * EREP_CHANNELS_OFFSET_SIZE)));
    }
    else
    {
        /* Do nothing */
    }
    return retVal != 0U;
}

/*!
 * @brief Clears error event single and the corresponding
 *        interrupt notification.
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The configured memory channel
 *
 * @retval None
 */
void EREP_HW_ClearEventSingle(EREP_T *const erepBase, uint8_t channel)
{
    erepBase->STS0.reg = 0x80000000U >> (channel * EREP_CHANNELS_OFFSET_SIZE);

    /* Read-after-write sequence to guarantee required serialization of memory operations */
    (void)erepBase->STS0.reg;
}

/*!
 * @brief Clears error event double and the corresponding
 *        interrupt notification.
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The configured memory channel
 *
 * @retval None
 */
void EREP_HW_ClearEventDouble(EREP_T *const erepBase, uint8_t channel)
{
    erepBase->STS0.reg = 0x40000000U >> (channel * EREP_CHANNELS_OFFSET_SIZE);

    /* Read-after-write sequence to guarantee required serialization of memory operations */
    (void)erepBase->STS0.reg;
}

/*!
 * @brief Reads the address of the last ECC event in Memory n
 *
 * @param erepBase: The EREP peripheral base address
 * @param channel: The examined memory channel
 *
 * @retval Address of the last ECC event
 */
uint32_t EREP_HW_ReadLastErrorAddress(const EREP_T *const erepBase, uint8_t channel)
{
#if(EREP_COUNT >= 2u)
    return (channel ? (erepBase->EADDR1.bit.EADDR) : (erepBase->EADDR0.bit.EADDR));
#else
    return (channel ? (erepBase->EADDR1.reg) : (erepBase->EADDR0.reg));
#endif
}

/*!
 * @brief Resets the EREP module.
 *
 * @param erepBase: The EREP peripheral base address
 *
 * @retval None
 */
void EREP_HW_Setup(EREP_T *const erepBase)
{
    erepBase->CFG0.reg = 0UL;
    /* Write 1 to clear flags */
    erepBase->STS0.reg = 0xFFFFFFFFUL;
}

/**@} end of group EREP_Functions*/
/**@} end of group EREP_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
