/*!
 * @file        apm32f445_446_tmc.c
 *
 * @brief       This file provides all the TMC firmware functions
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
#include "apm32f445_446_tmc.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup TMC_Driver TMC Driver
  @{
*/

/** @defgroup TMC_Macros Macros
  @{
*/

/* Number of possible outputs (target module) for TMC IP */
#define TMC_NUM_TARGET_MODULES        ((uint8_t)(sizeof(s_tmcTargetModule) / sizeof(TMC_TARGET_T)))
/* Number of SEL bitfields in one TMC register */
#define TMC_NUM_SEL_BITFIELDS_PER_REG (4U)
/* Get the index of the TMC register */
#define TMC_IDX_REG(x)                ((uint8_t)((uint8_t)(x) / TMC_NUM_SEL_BITFIELDS_PER_REG))
/* Get the index of the SEL bitfield inside TMC register */
#define TMC_IDX_SEL_BITFIELD_REG(x)   ((uint8_t)((uint8_t)(x) % TMC_NUM_SEL_BITFIELDS_PER_REG))

#define TMC_INSTANCE_VALIDITY(__instance__) if(__instance__ >= TMC_INSTANCE_COUNT){while(1);}

/**@} end of group TMC_Macros*/

/** @defgroup TMC_Variables Variables
  @{
*/

/*! @brief Table of base addresses for TMC instances. */
TMC_T * const g_tmcBaseAddress[TMC_INSTANCE_COUNT] = TMC_BASE_PTRS;

/**@} end of group TMC_Variables*/

/** @defgroup TMC_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Initialize a TMC instance for operation.
 *
 * @param instance: The TMC instance number.
 * @param tmcUserCfg: Pointer to the user configuration structure.
 *
 * @retval Execution returnStatus:
 *         - STATUS_SUCCESS
 *         - STATUS_ERROR - if at least one of the target module register is locked
 */
STATUS_T TMC_Init(const uint32_t instance, const TMC_USER_CONFIG_T * const tmcUserCfg)
{
    TMC_INSTANCE_VALIDITY(instance);

    TMC_T * baseAddress = g_tmcBaseAddress[instance];
    STATUS_T returnStatus;
    uint8_t cnt;

    /* Reset source triggers of all TMC target modules to default. */
    returnStatus = TMC_HW_Config(baseAddress);

    if (returnStatus == STATUS_SUCCESS)
    {
        for (cnt = 0U; cnt < tmcUserCfg->inOutMapCfgsCount; cnt++)
        {
            TMC_HW_ConfigTrigSourceForTargetModule(baseAddress,
                                                    tmcUserCfg->inOutMapCfg[cnt].triggeSrc,
                                                    tmcUserCfg->inOutMapCfg[cnt].targetModule);

            if (tmcUserCfg->inOutMapCfg[cnt].lockTargetModuleReg)
            {
                TMC_HW_ConfigLockForTargetModule(baseAddress,
                                                  tmcUserCfg->inOutMapCfg[cnt].targetModule);
            }
        }
    }

    return returnStatus;
}

/*!
 * @brief Reset to default values the source triggers corresponding to all target modules,
 *        if none of the target modules is locked.
 *
 * @param instance: The TMC instance number.
 *
 * @retval Execution returnStatus:
 *         - STATUS_SUCCESS
 *         - STATUS_ERROR - if at least one of the target module register is locked.
 */
STATUS_T TMC_DeInit(const uint32_t instance)
{
    TMC_INSTANCE_VALIDITY(instance);

    TMC_T * baseAddress = g_tmcBaseAddress[instance];
    STATUS_T returnStatus;

    returnStatus = TMC_HW_Config(baseAddress);

    return returnStatus;
}

/*!
 * @brief Configure a source trigger for a selected target module.
 *
 * @param instance: The TMC instance number.
 * @param triggeSrc: Trigger source.
 * @param targetModule: Trigger module.
 *
 * @retval Execution returnStatus:
 *         - STATUS_SUCCESS
 *         - STATUS_ERROR - if requested target module is locked
 */
STATUS_T TMC_ConfigTrigSourceForTargetModule(const uint32_t instance,
                                                const TMC_TRIGGER_T triggeSrc,
                                                const TMC_TARGET_T targetModule)
{
    TMC_INSTANCE_VALIDITY(instance);

    TMC_T * baseAddress = g_tmcBaseAddress[instance];
    STATUS_T returnStatus;
    bool lockStatus;

    lockStatus = TMC_HW_ReadLockForTargetModule(baseAddress, targetModule);

    if (lockStatus == false)
    {
        TMC_HW_ConfigTrigSourceForTargetModule(baseAddress, triggeSrc, targetModule);
        returnStatus = STATUS_SUCCESS;
    }
    else
    {
        returnStatus = STATUS_ERROR;
    }

    return returnStatus;
}

/*!
 * @brief Get the source trigger configured for a target module.
 *
 * @param instance: The TMC instance number.
 * @param targetModule: target module.
 *
 * @retval Enum value corresponding to the trigger source configured
 *         for the selected target module.
 */
TMC_TRIGGER_T TMC_ReadTrigSourceForTargetModule(const uint32_t instance,
                                                        const TMC_TARGET_T targetModule)
{
    TMC_INSTANCE_VALIDITY(instance);

    const TMC_T *baseAddress = g_tmcBaseAddress[instance];

    return TMC_HW_ReadTrigSourceForTargetModule(baseAddress, targetModule);
}

/*!
 * @brief Locks the TMC register of a target module.
 *
 * This function sets the LK bit of the TMC register corresponding to
 * the selected target module. Please note that some TMC registers can contain up to 4
 * SEL bitfields, meaning that these registers can be used to configure up to 4 target
 * modules independently. Because the LK bit is only one per register, the configuration
 * of all target modules referred from that register will be locked.
 *
 * @param instance: The TMC instance number.
 * @param targetModule: Target module.
 *
 * @retval None.
 */
void TMC_ConfigLockForTargetModule(const uint32_t instance, const TMC_TARGET_T targetModule)
{
    TMC_INSTANCE_VALIDITY(instance);

    TMC_T * baseAddress = g_tmcBaseAddress[instance];

    TMC_HW_ConfigLockForTargetModule(baseAddress, targetModule);
}

/*!
 * @brief Get the Lock bit status of the TMC register of a target module.
 *
 * @param instance: The TMC instance number.
 * @param targetModule: Target module.
 *
 * @retval true - if the selected targetModule register is locked
 *         false - if the selected targetModule register is not locked
 */
bool TMC_ReadLockForTargetModule(const uint32_t instance,const TMC_TARGET_T targetModule)
{
    TMC_INSTANCE_VALIDITY(instance);

    const TMC_T * baseAddress = g_tmcBaseAddress[instance];

    return TMC_HW_ReadLockForTargetModule(baseAddress, targetModule);
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
/*!
 * @brief  This function restores the TMC module to reset value.
 *
 * @param  base: Base address for current TMC instance.
 *
 * @retval STATUS_T Code.
 */
STATUS_T TMC_HW_Config(TMC_T * const base)
{
    /* Constant array storing the value of all TMC output(target module) identifiers */
    static const TMC_TARGET_T s_tmcTargetModule[] = FEATURE_TMC_TARGET_MODULE;
    uint8_t cnt = 0U;
    bool lockStatus = false;
    STATUS_T retStatus = STATUS_ERROR;

    for (cnt = 0; ((cnt < TMC_NUM_TARGET_MODULES) && (lockStatus == false)); cnt++)
    {
        /* Check if any of the TMC registers is locked */
        lockStatus = TMC_HW_ReadLockForTargetModule(base, s_tmcTargetModule[cnt]);
    }

    /* Abort operations if at least one of the target module is locked. */
    if (lockStatus != true)
    {
        /* Set all SEL bitfields of all TMC registers to default value */
        cnt = 0U;
        while (cnt < TMC_NUM_TARGET_MODULES)
        {
            /* Write the TMC register */
            base->TMCn[TMC_IDX_REG(s_tmcTargetModule[cnt])] &=
                ~((uint32_t)0x3FU << (8U * TMC_IDX_SEL_BITFIELD_REG(s_tmcTargetModule[cnt])));
            cnt++;
        }

        retStatus = STATUS_SUCCESS;
    }

    return retStatus;
}

/*!
 * @brief  This function configures a TMC link between a source trigger
 *         and a target module, if the requested target module is not locked.
 *
 * @param  base: Base address for current TMC instance.
 * @param  triggeSrc: trigger source.
 * @param  targetModule: module of TMC instance
 *
 * @retval None
 */
void TMC_HW_ConfigTrigSourceForTargetModule(TMC_T * const base,
                                             const TMC_TRIGGER_T triggeSrc,
                                             const TMC_TARGET_T targetModule)
{
    uint32_t tmpReg;
    /* Read value of entire TMC register in a temp variable */
    tmpReg = base->TMCn[TMC_IDX_REG(targetModule)];
    /* Clear first the SEL bitfield inside the TMC register */
    tmpReg &= ~((uint32_t)0x3FU << (8U * TMC_IDX_SEL_BITFIELD_REG(targetModule)));
    /* Configure the SEL bitfield to the desired value */
    tmpReg |=  ((uint32_t)triggeSrc) << ((uint8_t)(8U * TMC_IDX_SEL_BITFIELD_REG(targetModule)));
    /* Write back the TMC register */
    base->TMCn[TMC_IDX_REG(targetModule)] = tmpReg;
}

/*!
 * @brief  This function returns the TMC source trigger linked to
 *         a selected target module.
 *
 * @param  base: Base address for current TMC instance.
 * @param  targetModule: Module of TMC instance
 *
 * @retval trigger source for the module of TMC instance.
 */
TMC_TRIGGER_T TMC_HW_ReadTrigSourceForTargetModule(const TMC_T *const base, const TMC_TARGET_T targetModule)
{
    uint32_t trigSource;
    /* Perform the update operation */
    trigSource =
        ((base->TMCn[TMC_IDX_REG(targetModule)] >> (8U * TMC_IDX_SEL_BITFIELD_REG(targetModule))) & 0x3FU);

    return (TMC_TRIGGER_T)(trigSource);
}

/*!
 * @brief  This function sets the LK bit of the TMC register corresponding
 *         to the selected target module.
 *
 * @param  base: Base address for current TMC instance.
 * @param  targetModule: Module of TMC instance
 *
 * @retval None.
 */
void TMC_HW_ConfigLockForTargetModule(TMC_T *const base, const TMC_TARGET_T targetModule)
{
    /* Perform the update operation */
    base->TMCn[TMC_IDX_REG(targetModule)] |= (((uint32_t)1U) << 31U);
}

/*!
 * @brief  Read the Lock bit status of the TMC register of a target module.
 *
 * @param  base: Base address for current TMC instance.
 * @param  targetModule: Module of TMC instance.
 *
 * @retval lock current Lock bit status.
 */
bool TMC_HW_ReadLockForTargetModule(const TMC_T *const base, const TMC_TARGET_T targetModule)
{
    uint32_t lockVal;
    bool lock;

    /* Get the lock bit value */
    lockVal = ((base->TMCn[TMC_IDX_REG(targetModule)] & 0x80000000U) >> 31U);

    lock = (lockVal == 0U) ? false : true;

    return lock;
}

/*!
 * @brief Generate software triggers
 *
 * @param None.
 *
 * @retval None.
 *
 * @note This function uses a SIM register in order to generate a
 *       software triggers to the target peripherals selected in TMC
 */
void TMC_HW_GenerateSWTrigger(void)
{
    /* The trigger is generated only when writing from 0 to 1*/
    SIM->MCTRL1.reg = 0U;
    SIM->MCTRL1.reg = 0x01U;
}

/**@} end of group TMC_Functions*/
/**@} end of group TMC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
