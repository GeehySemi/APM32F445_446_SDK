/*!
 * @file        apm32f445_446_ewdt.c
 *
 * @brief       This file provides all the EWDT firmware functions
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
#include "apm32f445_446_ewdt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup EWDT_Driver EWDT Driver
  @{
*/

/** @defgroup EWDT_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/*! @brief Local table used to store EWDT base pointers */
static EWDT_T *const g_ewdtBasePointer[] = {EWDT};

/**@} end of group EWDT_Variables*/

/** @defgroup EWDT_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Configures the EWDT.
 *
 * @param ins: EWDT ins number
 * @param cfg: Pointer to the module configuration structure.
 *
 * @retval status_t Will return the status of the operation:
 *         - STATUS_SUCCESS: if the operation is successful
 *         - STATUS_ERROR:   if the windows values are not
 *                           correct or if the ins is
 *                           already enabled
 */
STATUS_T EWDT_Init(uint32_t ins, const EWDT_INIT_CONFIG_T *cfg)
{
    EWDT_INSTANCE_VALIDITY(ins);

    /* Return status variable */
    STATUS_T stateCode = STATUS_SUCCESS;
    /* Flag to store if the module is enabled */
    bool isEnabled;
    uint8_t temporValue = 0U;
    /* Base pointer */
    EWDT_T *ewdtBase = g_ewdtBasePointer[ins];

    /* Read the enable status of the module */
    isEnabled = EWDT_HW_IsEnabled(ewdtBase);
    /* Check if the EWDT ins is already enabled or if the windows values are not correct */
    if ((isEnabled == true) ||
        (cfg->compareHigh <= cfg->compareLow) ||
        (cfg->compareHigh > FEATURE_EWDT_COMPH_MAX_VALUE))
    {
        /* If conditions are met change the status code to error */
        stateCode = STATUS_ERROR;
    }
    else
    {
        /* Config compare high and low values */
        EWDT_HW_ConfigCompareHigh(ewdtBase, cfg->compareHigh);
        EWDT_HW_ConfigCompareLow(ewdtBase, cfg->compareLow);
        /* Config clock prescaler */
        EWDT_HW_ConfigPrescaler(ewdtBase, cfg->prescaler);

        /* Configure the Control register and enable the ins.
         * Config the values that are not affected by the input pin
         */
        temporValue = (uint8_t)(((uint8_t)(1U << 0U) & 0x1U) |
                                (((cfg->interruptEnable ? 1U : 0U) << 3U) & 0x8U));

        /* Depending how the input pin is configured set the values into the
         * temporary variable
         */
        if(cfg->assertLogic == EWDT_IN_ASSERT_ON_LOGIC_ONE )
        {
            temporValue |= (uint8_t)(0x4U | 0x2U);

        }
        else if(cfg->assertLogic == EWDT_IN_ASSERT_ON_LOGIC_ZERO )
        {
            temporValue |= 0x4U;
        }
        /* Write the configuration into the Control register */
        EWDT_HW_ConfigControl(ewdtBase, temporValue);
    }

    /* Return the status code */
    return stateCode;
}

/*!
 * @brief Initialize configuration structure to default values.
 *
 * @param cfg: Pointer to the configuration structure to initialize
 *
 * @retval None
 */
void EWDT_DefaultConfig(EWDT_INIT_CONFIG_T *cfg)
{
    /* Disable interrupts */
    cfg->interruptEnable = false;
    /* Maximum prescaler */
    cfg->prescaler = 255U;
    /* Maximum service window */
    cfg->compareHigh = FEATURE_EWDT_COMPH_MAX_VALUE;
    cfg->compareLow = FEATURE_EWDT_COMPL_MIN_VALUE;
    /* Input pin enabled and configured to assert on logic 0 */
    cfg->assertLogic = EWDT_IN_ASSERT_ON_LOGIC_ZERO;
}

/*!
 * @brief Reloads the EWDT.
 *
 * @param ins: EWDT ins number
 *
 * @retval None
 *
 * @note This method needs to be called within the window period
 * specified by the Compare Low and Compare High registers.
 */
void EWDT_Reload(uint32_t ins)
{
    EWDT_INSTANCE_VALIDITY(ins);

    /* Base pointer */
    EWDT_T *ewdtBase = g_ewdtBasePointer[ins];
    /* Disable global IRQ's to avoid disturbing the refresh process */
    INT_SYS_DisableIRQGlobal();
    /* Write the refresh values */
    EWDT_HW_Reload(ewdtBase);
    /* Re-enable all IRQ's */
    INT_SYS_EnableIRQGlobal();
}

/*!
 * @brief Reads the Input pin assert logic.
 *
 * @param ins: EWDT ins number
 *
 * @retval The input pin assert logic:
 *      - EWM_IN_ASSERT_DISABLED:      EWDT in disabled
 *      - EWM_IN_ASSERT_ON_LOGIC_ZERO: EWDT is asserted when EWDT_in is logic 0
 *      - EWM_IN_ASSERT_ON_LOGIC_ONE:  EWDT is asserted when EWDT_in is logic 1
 */
EWDT_IN_ASSERT_LOGIC_T EWDT_ReadInputPinAssertLogic(uint32_t ins)
{
    EWDT_INSTANCE_VALIDITY(ins);

    /* Base pointer */
    const EWDT_T *const ewdtBase = g_ewdtBasePointer[ins];
    /* Variable where to save the retrieved configuration */
    EWDT_IN_ASSERT_LOGIC_T retVal;

    /* Check if input pin is enabled */
    if (POINTER_IS_NULL(ewdtBase->CTRL.bit.INPEN) != false)
    {
        /* If true read the assert logic into the returnValue variable */
        retVal = ((POINTER_IS_NULL(ewdtBase->CTRL.bit.INSIGSEL) != false) ?
                    EWDT_IN_ASSERT_ON_LOGIC_ONE : EWDT_IN_ASSERT_ON_LOGIC_ZERO);
    }
    else
    {
        /* Pin is disabled, return the corresponding value */
        retVal = EWDT_IN_ASSERT_DISABLED;
    }

    return retVal;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Reloads the EWDT
 *
 * @param ewdtBase: EWDT base pointer
 *
 * @retval None
 */
void EWDT_HW_Reload(EWDT_T *const ewdtBase)
{
    /* Write first byte of the service key */
    ewdtBase->SEQ.reg = (uint8_t)FEATURE_EWDT_KEY_FIRST_BYTE;
    /* Write second byte of the service key */
    ewdtBase->SEQ.reg = (uint8_t)FEATURE_EWDT_KEY_SECOND_BYTE;
}

/*!
 * @brief Reads the EWDT enable bit.
 *
 * @param ewdtBase: EWDT base pointer
 *
 * @retval The state of the device enable bit:
 *         - true:  EWDT enabled
 *         - false: EWDT disabled
 */
bool EWDT_HW_IsEnabled(const EWDT_T *const ewdtBase)
{
    return (((ewdtBase->CTRL.reg & 0x1U) >> 0U) != 0U) ? true : false;
}

/*!
 * @brief Configures the Control Value.
 *
 * @param ewdtBase: EWDT base pointer
 * @param value: Value to write into Control register
 *
 * @retval None
 */
void EWDT_HW_ConfigControl(EWDT_T *const ewdtBase, uint8_t value)
{
    ewdtBase->CTRL.reg = value;
}

/*!
 * @brief Read the Control register Value.
 *
 * @param ewdtBase: EWDT base pointer
 *
 * @retval Value stored in Control register
 */
uint8_t EWDT_HW_ReadControl(const EWDT_T *const ewdtBase)
{
    return (ewdtBase->CTRL.reg);
}

/*!
 * @brief Configures the Compare Low Value.
 *
 * @param ewdtBase: EWDT base pointer
 * @param value: Value to write into Compare Low register
 *
 * @retval None
 */
void EWDT_HW_ConfigCompareLow(EWDT_T *const ewdtBase, uint8_t value)
{
    ewdtBase->COMPL.reg = value;
}

/*!
 * @brief Configures the Compare High Value.
 *
 * @param ewdtBase: EWDT base pointer
 * @param value: Value to write into Compare High register
 *
 * @retval None
 */
void EWDT_HW_ConfigCompareHigh(EWDT_T *const ewdtBase, uint8_t value)
{
    ewdtBase->COMPH.reg = value;
}

/*!
 * @brief Configures the Clock Prescaler Value.
 *
 * @param ewdtBase: EWDT base pointer
 * @param value: Prescaler Value
 *
 * @retval None
 */
void EWDT_HW_ConfigPrescaler(EWDT_T *const ewdtBase, uint8_t value)
{
    ewdtBase->CLKPSC.reg = value;
}

/**@} end of group EWDT_Functions*/
/**@} end of group EWDT_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
