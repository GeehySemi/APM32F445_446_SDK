/*!
 * @file        apm32f445_446_pins.c
 *
 * @brief       This file provides all the PINS firmware functions
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
#include "apm32f445_446_pins.h"
/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup PINS_Driver PINS Driver
  @{
*/

/** @defgroup PINS_Functions Functions
  @{
*/
/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
 /*!
 * @brief   This function is used to initialize pins
 *
 * @details This function uses the given configuration set to configure pins
 *
 * @param   pinCnt  : Number of pins configured
 * @param   cfg     : the configuration structure
 *
 * @retval  The status of the operation
 */

STATUS_T PINS_Init(uint32_t pinCnt, const PIN_SETTINGS_CFG_T cfg[])
{
    uint32_t k;
    for (k = 0U; k < pinCnt; k++)
    {
        PINS_HW_Cfg(&cfg[k]);
    }

    return STATUS_SUCCESS;
}

 /*!
 * @brief   This function configures internal resistance
 *
 * @details Set pin pull-up resistor or pull-down resistance or float
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 * @param   pullCfg : internal resistance status
 *
 */
void PINS_SetPullSel(PM_T * const pmBase, uint32_t pin, PM_PULL_CFG_T pullCfg)
{
    PINS_HW_SetPullSel(pmBase, pin, pullCfg);
}

 /*!
 * @brief   This function configures internal resistance
 *
 * @details This feature configures pin currentMux and supports configuration for
 *          pins with ADC interleaved channels
 *
 * @param   pmBase   : pm base pointer.
 * @param   pin      : port pin.
 * @param   mux      : port pin muxing.
 *
 */
void PINS_SetMuxModeSel(PM_T * const pmBase, uint32_t pin, PM_MUX_T mux)
{
    PINS_HW_SetMuxModeSel(pmBase, pin, mux);
}

 /*!
 * @brief   Configure port pin interrupt/DMA requests
 *
 * @details This function configures the port pin interrupt/DMA request.
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 * @param   intCfg  : interrupt configuration
 *
 */
void PINS_SetPinIntSel(PM_T * const pmBase, uint32_t pin, PM_INTERRUPT_CFG_T intCfg)
{
    PINS_HW_SetPinIntSel(pmBase, pin, intCfg);
}

 /*!
 * @brief   get port pin interrupt/DMA request
 *
 * @details This function get port pin interrupt/DMA request
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 * @retval  interrupt configuration
 *
 */

PM_INTERRUPT_CFG_T PINS_ReadPinIntSel(const PM_T * const pmBase, uint32_t pin)
{
    return PINS_HW_ReadPinIntSel(pmBase, pin);
}

 /*!
 * @brief   Clear single pin interrupt status flag
 *
 * @details This function clears the individual pin-interrupt status flag.
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_ClrPinIntFlgCmd(PM_T * const pmBase, uint32_t pin)
{
    PINS_HW_ClrPinIntFlgCmd(pmBase, pin);
}


 /*!
 * @brief   Enable digital filters for digital pins
 *
 * @details This function enable digital filters for digital pins
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_EnableDigitFilt(PM_T * const pmBase, uint32_t pin)
{
    PINS_HW_EnableDigitFilt(pmBase, pin);
}


 /*!
 * @brief   Disable digital filters for digital pins
 *
 * @details This function disable digital filters for digital pins
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_DisableDigitFilt(PM_T * const pmBase, uint32_t pin)
{
    PINS_HW_DisableDigitFilt(pmBase, pin);
}

 /*!
 * @brief   Configure a digital filter clock for the given configured port
 *
 * @details This function configure a digital filter clock for the given configured port,
 *          Filter configuration can only be updated after all filters are disabled
 *
 * @param   pmBase  : pm base pointer.
 * @param   cfg     : digital filter configuration struct
 *
 */
void PINS_CfgDigitFilt(PM_T * const pmBase, const PM_DIGIT_FILT_CFG_T * const cfg)
{
    PINS_HW_CfgDigitFilt(pmBase, cfg);
}


 /*!
 * @brief   Reads the entire port interrupt status flag.
 *
 * @details This function reads the entire port interrupt status flag.
 *
 * @param   pmBase  : pm base pointer.
 *
 * @retval  port interrupt status flags:
 *          - 0: interrupt is not detected.
 *          - 1: interrupt is detected.
 *
 */
uint32_t PINS_ReadPmIntFlg(const PM_T * const pmBase)
{
    return PINS_HW_ReadPmIntFlg(pmBase);
}


 /*!
 * @brief   Clear the entire port interrupt status flag.
 *
 * @details This function clear the entire port interrupt status flag.
 *
 * @param   pmBase  : pm base pointer.
 *
 */
void PINS_ClrPmIntFlgCmd(PM_T * const pmBase)
{
    PINS_HW_ClrPmIntFlgCmd(pmBase);
}


 /*!
 * @brief   Gets the orientation configuration for all pins of the port.
 *
 * @details This function is used to gets the orientation configuration for all pins of the port,
 *          returning bits of 1 as output and 0 as input.
 *
 * @param   gpioBase  : GPIO base pointer.
 *
 * @retval  Each bit represents a pin:
 *          - 0: The pin direction is input.
 *          - 1: The pin direction is output.
 *
 */
PINS_CHANNEL_TYPE_T PINS_ReadPinsDir(const GPIO_T * const gpioBase)
{
    return PINS_HW_GPIO_ReadPinsDir(gpioBase);
}


 /*!
 * @brief   Configure the direction for a pin of the port.
 *
 * @details This function is used to Configure the direction for a pin of the port,
 *          1 for pin to be configured as output and 0 for pin to be configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pin       : pm pin number
 * @param   dir       : the pin direction
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_SetPinDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T dir)
{
    PINS_HW_GPIO_SetPinDir(gpioBase, pin, dir);
}

 /*!
 * @brief   Set the orientation of the pins used by the entire port.
 *
 * @details This function is used to set the orientation of the pins used by the entire port,
 *          1 for pin to be configured as output and 0 for pin to be configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pins      : Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_SetPinsDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_SetPinsDir(gpioBase, pins);
}

#if FEATURE_PM_WITH_INPUT_DISABLE

 /*!
 * @brief   Set the pin input disable state for the port
 *
 * @details This function is used to set the pin input disable state for the port,
 *          The pin corresponding to the 1 bit cannot be configured as input,
 *          and the pin corresponding to the 0 bit is configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pins      : Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_SetPmInputDisable(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_SetPmInputDisable(gpioBase, pins);
}


 /*!
 * @brief   Get the pin input disable state for the port
 *
 * @details This function is used to Get the pin input disable state for the port,
 *          The pin corresponding to the 1 bit cannot be configured as input,
 *          and the pin corresponding to the 0 bit is configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 *
 * @retval  Input status of GPIO , Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
PINS_CHANNEL_TYPE_T PINS_ReadPmInputDisable(const GPIO_T * const gpioBase)
{
    return PINS_HW_GPIO_ReadPmInputDisable(gpioBase);
}
#endif /* FEATURE_PM_WITH_INPUT_DISABLE */


 /*!
 * @brief   Quickly configures multiple pins with the same pin configuration.
 *
 * @details This function is used to quickly configures multiple pins with the same pin configuration,
 *          Up to 16 pins are supported,with the lower or upper part of the pin register located on the
 *          same port.
 *
 * @param   pmBase   : pm base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: pins corresponding to bits with val of '1' is updated with the val input
 *                - 1: pins corresponding to bits with val of '0' is not updated with the val input
 * @param   val      : When the pin is set to 1,the configuration value will be updated
 * @param   halfPm   : half the pin of the register.
 *
 */

void PINS_SetGlbPinCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm)
{
    PINS_HW_SetGlobalPinCtrl(pmBase, pins, val, halfPm);
}

 /*!
 * @brief   Quickly configures multiple pins with the same interrupt configuration.
 *
 * @details This function is used to quickly configures multiple pins with the same interrupt configuration.,
 *          Up to 16 pins are supported,with the lower or upper part of the pin register located on the
 *          same port.
 *
 * @param   pmBase   : pm base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: pins corresponding to bits with value of '1' is updated with the value input
 *                - 1: pins corresponding to bits with value of '0' is not updated with the value input
 * @param   val      : When the pin is set to 1,the configuration value will be updated
 * @param   halfPm   : half the pin of the register.
 *
 */
void PINS_SetGlbIntCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm)
{
    PINS_HW_SetGlobalIntCtrl(pmBase, pins, val, halfPm);
}



 /*!
 * @brief   Write specific values to set port pin levels
 *
 * @details This function is used to write specific values to set port pin levels,
 *          (0 is low level, 1 is high level)
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pin      : pm pin number
 * @param   val      : Written level value:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
void PINS_WritePin(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T val)
{
    PINS_HW_GPIO_WritePin(gpioBase, pin, val);
}


 /*!
 * @brief   Write all pins of a port
 *
 * @details This function is used to write all pins of a port,
 *          (0 is low level, 1 is high level)
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
void PINS_WritePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_WritePins(gpioBase, pins);
}


 /*!
 * @brief   Get current output from port
 *
 * @details This function is used to get current output from port,
 *          only pins configured as outputs are meaningful.
 *
 * @param   gpioBase : GPIO base pointer.
 *
 * @retval  output status of gpio, each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
PINS_CHANNEL_TYPE_T PINS_ReadPinsOutput(const GPIO_T * const gpioBase)
{
    return PINS_HW_GPIO_ReadPinsOutput(gpioBase);
}


 /*!
 * @brief   Write pins with 'Set' value
 *
 * @details Configure the pins configured as outputs in the parameters as high levels,
 *          and the pins configured as inputs are not affected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: is high level
 *
 */
void PINS_SetPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_SetPins(gpioBase, pins);
}


 /*!
 * @brief   Write pins with 'Clear' value
 *
 * @details Configure the pins configured as outputs in the parameters as low levels,
 *          and the pins configured as inputs are not affected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: is low level
 *
 */
void PINS_ClrPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_ClrPins(gpioBase, pins);
}


 /*!
 * @brief   Toggle pins value
 *
 * @details This function toggles output pins listed in parameter pins (bits that are
 *          1). Pins corresponding to 0 will be unaffected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: Switched pins
 *
 */
void PINS_TogglePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    PINS_HW_GPIO_TogglePins(gpioBase, pins);
}


 /*!
 * @brief   Get current input from port
 *
 * @details This function is used to get current input from port,
 *          only pins configured as inputs are meaningful.
 *
 * @param   gpioBase : GPIO base pointer.
 *
 * @retval  output status of gpio, each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
PINS_CHANNEL_TYPE_T PINS_ReadPinsInput(const GPIO_T * const gpioBase)
{
    return PINS_HW_GPIO_ReadPinsInput(gpioBase);
}


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
 /*!
 * @brief   This function configures internal resistance
 *
 * @details Set pin pull-up resistor or pull-down resistance or float
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 * @param   pullCfg : internal resistance status
 *
 */
void PINS_HW_SetPullSel(PM_T * const pmBase, uint32_t pin, PM_PULL_CFG_T pullCfg)
{
    if(pullCfg == PM_INTERNAL_PULL_NOT_ENABLED)
    {
        pmBase->PINCTRL[pin].bit.IREN = 0U;
    }
    else if(pullCfg == PM_INTERNAL_PULL_DOWN_ENABLED)
    {
        pmBase->PINCTRL[pin].bit.RSEL = 0U;
        pmBase->PINCTRL[pin].bit.IREN = 1U;
    }
    else if(pullCfg == PM_INTERNAL_PULL_UP_ENABLED)
    {
        pmBase->PINCTRL[pin].bit.RSEL = 1U;
        pmBase->PINCTRL[pin].bit.IREN = 1U;
    }
}

 /*!
 * @brief   Configure port pin interrupt/DMA requests
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 * @param   intCfg  : interrupt configuration
 *
 */
void PINS_HW_SetPinIntSel(PM_T * const pmBase, uint32_t pin, PM_INTERRUPT_CFG_T intCfg)
{
    pmBase->PINCTRL[pin].bit.INTREQCFG = intCfg;
}


 /*!
 * @brief   get port pin interrupt/DMA request
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 * @retval  interrupt configuration
 *
 */
PM_INTERRUPT_CFG_T PINS_HW_ReadPinIntSel(const PM_T * const pmBase, uint32_t pin)
{
    return (PM_INTERRUPT_CFG_T) (pmBase->PINCTRL[pin].bit.INTREQCFG);
}


 /*!
 * @brief   Clear single pin interrupt status flag
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_HW_ClrPinIntFlgCmd(PM_T * const pmBase, uint32_t pin)
{
    pmBase->PINCTRL[pin].bit.INTFLG = 0U;
    pmBase->PINCTRL[pin].bit.INTFLG = 1U;
}

 /*!
 * @brief   Enable digital filters for digital pins
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_HW_EnableDigitFilt(PM_T * const pmBase, uint32_t pin)
{
      (pmBase->DFEN.reg) |= (uint32_t)1U << pin;
}

 /*!
 * @brief   Disable digital filters for digital pins
 *
 * @param   pmBase  : pm base pointer.
 * @param   pin     : pm pin number
 *
 */
void PINS_HW_DisableDigitFilt(PM_T * const pmBase, uint32_t pin)
{
      pmBase->DFEN.reg &= ~((uint32_t)1U << pin);
}


 /*!
 * @brief   Configure a digital filter clock for the given configured port
 *
 * @param   pmBase  : pm base pointer.
 * @param   cfg     : configuration struct
 *
 */
void PINS_HW_CfgDigitFilt(PM_T * const pmBase, const PM_DIGIT_FILT_CFG_T * const cfg)
{
    pmBase->DFCLKS.bit.DFCLKS = cfg->clock;
    pmBase->DFWID.bit.DFWID = cfg->width;
}


 /*!
 * @brief   Reads the entire port interrupt status flag.
 *
 * @param   pmBase  : pm base pointer.
 *
 * @retval  port interrupt status flags:
 *          - 0: interrupt is not detected.
 *          - 1: interrupt is detected.
 *
 */
uint32_t PINS_HW_ReadPmIntFlg(const PM_T * const pmBase)
{

    return (pmBase->INTSTS.reg);
}


 /*!
 * @brief   Clear the entire port interrupt status flag.
 *
 * @param   pmBase  : pm base pointer.
 *
 */
void PINS_HW_ClrPmIntFlgCmd(PM_T * const pmBase)
{
    pmBase->INTSTS.reg = 0xFFFFFFFFU;
}

 /*!
 * @brief   Gets the orientation configuration for all pins of the port.
 *
 * @details This function is used to gets the orientation configuration for all pins of the port,
 *          returning bits of 1 as output and 0 as input.
 *
 * @param   gpioBase  : GPIO base pointer.
 *
 * @retval  Each bit represents a pin:
 *          - 0: The pin direction is input.
 *          - 1: The pin direction is output.
 *
 */
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsDir(const GPIO_T * const gpioBase)
{
    return (PINS_CHANNEL_TYPE_T)gpioBase->PLLTDS.reg;
}

 /*!
 * @brief   Configure the direction for a pin of the port.
 *
 * @details This function is used to Configure the direction for a pin of the port,
 *          1 for pin to be configured as output and 0 for pin to be configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pin       : pm pin number
 * @param   dir       : the pin direction
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_HW_GPIO_SetPinDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T dir)
{
    PINS_CHANNEL_TYPE_T pinsDires = (PINS_CHANNEL_TYPE_T)gpioBase->PLLTDS.reg;
    pinsDires &= (PINS_CHANNEL_TYPE_T)(~((PINS_CHANNEL_TYPE_T)1U << pin));
    pinsDires |= (PINS_CHANNEL_TYPE_T)((PINS_CHANNEL_TYPE_T)dir << pin);
    gpioBase->PLLTDS.reg = pinsDires;
}

 /*!
 * @brief   Set the orientation of the pins used by the entire port.
 *
 * @details This function is used to set the orientation of the pins used by the entire port,
 *          1 for pin to be configured as output and 0 for pin to be configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pins      : Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_HW_GPIO_SetPinsDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    gpioBase->PLLTDS.reg = pins;
}

#if FEATURE_PM_WITH_INPUT_DISABLE
 /*!
 * @brief   Set the pin input disable state for the port
 *
 * @details This function is used to set the pin input disable state for the port,
 *          The pin corresponding to the 1 bit cannot be configured as input,
 *          and the pin corresponding to the 0 bit is configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 * @param   pins      : Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
void PINS_HW_GPIO_SetPmInputDisable(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    gpioBase->PLLID.reg = pins;
}

 /*!
 * @brief   Get the pin input disable state for the port
 *
 * @details This function is used to Get the pin input disable state for the port,
 *          The pin corresponding to the 1 bit cannot be configured as input,
 *          and the pin corresponding to the 0 bit is configured as input.
 *
 * @param   gpioBase  : GPIO base pointer
 *
 * @retval  Input status of GPIO , Each bit represents a pin:
 *                    - 0: The pin direction is input.
 *                    - 1: The pin direction is output.
 *
 */
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPmInputDisable(const GPIO_T * const gpioBase)
{
    return (PINS_CHANNEL_TYPE_T)gpioBase->PLLID.reg;
}
#endif /* FEATURE_PM_WITH_INPUT_DISABLE */

 /*!
 * @brief   Write specific values to set port pin levels
 *
 * @details This function is used to Write specific values to set port pin levels,
 *          (0 is low level, 1 is high level)
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pin      : pm pin number
 * @param   val      : Written level value:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
void PINS_HW_GPIO_WritePin(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T val)
{
    PINS_CHANNEL_TYPE_T pinsValues = (PINS_CHANNEL_TYPE_T)gpioBase->PLLO.bit.PLLO;
    pinsValues &= (PINS_CHANNEL_TYPE_T)(~((PINS_CHANNEL_TYPE_T)1U << pin));
    pinsValues |= (PINS_CHANNEL_TYPE_T)((PINS_CHANNEL_TYPE_T)val << pin);
    gpioBase->PLLO.bit.PLLO = pinsValues;
}

 /*!
 * @brief   Write all pins of a port
 *
 * @details This function is used to write all pins of a port,
 *          (0 is low level, 1 is high level)
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
void PINS_HW_GPIO_WritePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
      gpioBase->PLLO.reg = pins;
}

 /*!
 * @brief   Get current output from port
 *
 * @details This function is used to get current output from port,
 *          only pins configured as outputs are meaningful.
 *
 * @param   gpioBase : GPIO base pointer.
 *
 * @retval  output status of gpio, each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsOutput(const GPIO_T * const gpioBase)
{
    PINS_CHANNEL_TYPE_T rtnVal = 0U;
    rtnVal = (PINS_CHANNEL_TYPE_T)(gpioBase->PLLO.reg);

    return rtnVal;
}

 /*!
 * @brief   Write pins with 'Set' value
 *
 * @details Configure the pins configured as outputs in the parameters as high levels,
 *          and the pins configured as inputs are not affected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: is high level
 *
 */
void PINS_HW_GPIO_SetPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    gpioBase->PLLOS.reg = pins;
}

 /*!
 * @brief   Write pins with 'Clear' value
 *
 * @details Configure the pins configured as outputs in the parameters as low levels,
 *          and the pins configured as inputs are not affected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: is low level
 *
 */
void PINS_HW_GPIO_ClrPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    gpioBase->PLLOC.reg = pins;
}

 /*!
 * @brief   Toggle pins value
 *
 * @details This function toggles output pins listed in parameter pins (bits that are
 *          1). Pins corresponding to 0 will be unaffected.
 *
 * @param   gpioBase : GPIO base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: Not affected
 *                - 1: Switched pins
 *
 */
void PINS_HW_GPIO_TogglePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins)
{
    gpioBase->PLLOT.reg = pins;
}

 /*!
 * @brief   Get current input from port
 *
 * @details This function is used to get current input from port,
 *          only pins configured as inputs are meaningful.
 *
 * @param   gpioBase : GPIO base pointer.
 *
 * @retval  output status of gpio, each bit represents a pin:
 *                - 0: is low level
 *                - 1: is high level
 *
 */
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsInput(const GPIO_T * const gpioBase)
{
    PINS_CHANNEL_TYPE_T rtnVal = 0U;

    rtnVal = (PINS_CHANNEL_TYPE_T)(gpioBase->PLLI.reg);

    return rtnVal;
}


#if FEATURE_SOC_PM_COUNT > 0

#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
 /*!
 * @brief   Reads ADC inter leaveVal.
 *
 * @param   pmBase      : pm base pointer.
 * @param   pinPmIdx    : port pin index.
 * @param   currentVal  : current Value.
 *
 * @retval  ADC inter leaveVal.
 *
 */
 uint32_t PINS_ReadAdcInterleaveVal(const PM_T * pmBase, const uint32_t pinPmIdx, const uint32_t currentVal)
{
    uint32_t adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_INVALID;
    /* calculate appropriate val to enable or disable in SIM_CHIPCTL[ADC_INTERLEAVE_EN] */
    if ((uint32_t)pmBase == (uint32_t)PMB)
    {
      if(pinPmIdx == 0)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_ENABLE0 | currentVal;
      }
      else if(pinPmIdx == 1)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_ENABLE1 | currentVal;
      }
      else if(pinPmIdx == 13)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_ENABLE2 | currentVal;
      }
      else if(pinPmIdx == 14)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_ENABLE3 | currentVal;
      }
      else if(pinPmIdx == 15)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_DISABLE0 & currentVal;
      }
      else if(pinPmIdx == 16)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_DISABLE1 & currentVal;
      }
    }
    else if ((uint32_t)pmBase == (uint32_t)PMC)
    {
      if(pinPmIdx == 0)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_DISABLE2 & currentVal;
      }
      else if(pinPmIdx == 1)
      {
        adcInterleaveVal = (uint32_t)PIN_ADC_INTERLEAVE_DISABLE3 & currentVal;
      }
    }
    else
    {
        /* invalid */
    }
    return adcInterleaveVal;
}
#endif /* FEATURE_PINS_WITH_ADC_INTERLEAVE_EN */



 /*!
 * @brief   This function is used to initialize pins
 *
 * @details This function uses the given configuration set to configure pins
 *
 * @param   cfg : the configuration structure
 *
 */

void PINS_HW_Cfg(const PIN_SETTINGS_CFG_T * cfg)
{

    uint32_t regVal = cfg->base->PINCTRL[cfg->pinPmIdx].reg;
    uint32_t dirs;
    uint32_t digitFilts;
    PM_MUX_T currentMux;

    switch (cfg->pullCfg)
    {
        case PM_INTERNAL_PULL_UP_ENABLED:
            {
                regVal |= (((uint32_t)(((uint32_t)(1U))<<1U))&0x2U);
                regVal |= (((uint32_t)(((uint32_t)(1U))<<0U))&0x1U);
            }
            break;
        case PM_INTERNAL_PULL_DOWN_ENABLED:
            {
                regVal &= ~(0x1U);
                regVal |= (((uint32_t)(((uint32_t)(1U))<<1U))&0x2U);
            }
            break;
        case PM_INTERNAL_PULL_NOT_ENABLED:
            {
                regVal &= ~(0x2U);
            }
            break;
        default:

            break;
    }

    regVal &= ~(0x10U);
    regVal |= (((uint32_t)(((uint32_t)(cfg->passiveFilt))<<4U))&0x10U);
    regVal &= ~(0x40U);
    regVal |= (((uint32_t)(((uint32_t)(cfg->drvSel))<<6U))&0x40U);
    regVal &= ~(0x700U);
    currentMux = cfg->mux;
#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
    if (currentMux == PM_MUX_ADC_INTERLEAVE)
    {
        uint32_t chipCtrlReg = (uint32_t)(SIM->CHIPCTRL.bit.ADCIPSEL);

              uint32_t interleaveVal = PINS_ReadAdcInterleaveVal(cfg->base, cfg->pinPmIdx, chipCtrlReg);
        if (interleaveVal != (uint32_t)PIN_ADC_INTERLEAVE_INVALID)
        {
                        SIM->CHIPCTRL.bit.ADCIPSEL = interleaveVal;
        }
        /* return real currentMux for pin */
        currentMux = PM_PIN_DISABLED;
    }
#endif
    regVal |= (((uint32_t)(((uint32_t)(currentMux))<<8U))&0x700U);
    regVal &= ~(0xF0000U);
    regVal |= (((uint32_t)(((uint32_t)(cfg->intCfg))<<16U))&0xF0000U);
    if (cfg->clrIntFlg)
    {
        regVal &= ~(0x1000000U);
        regVal |= (((uint32_t)(((uint32_t)(1U))<<24U))&0x1000000U);
    }
    cfg->base->PINCTRL[cfg->pinPmIdx].reg = regVal;

    /* Read current digital filter of port */
    digitFilts = (uint32_t)(cfg->base->DFEN.reg);
    digitFilts &= ~(1UL << (cfg->pinPmIdx));
    digitFilts |= (((uint32_t)(cfg->digitFilt)) << (cfg->pinPmIdx));
    /* Write to digital filter enable register */
    cfg->base->DFEN.reg = digitFilts;

    /* If gpioBase address not null setup the dir of pin */
    if (PM_MUX_AS_GPIO == cfg->mux)
    {
        /* Read current dir */
        dirs = (uint32_t)(cfg->gpioBase->PLLTDS.reg);
        switch (cfg->dir)
        {
            case GPIO_INPUT_DIR:
                dirs &= ~(1UL << cfg->pinPmIdx);
                break;
            case GPIO_OUTPUT_DIR:
                dirs |= (1UL << cfg->pinPmIdx);
                break;
            case GPIO_UNSPECIFIED_DIR:
            /* pass-through */
            default:
                /* nothing to configure */

                break;
        }
        /* Configure initial val for output */
        if (cfg->dir == GPIO_OUTPUT_DIR)
        {
            PINS_HW_GPIO_WritePin(cfg->gpioBase, cfg->pinPmIdx, cfg->initVal);
        }
        /* Configure dir */
        cfg->gpioBase->PLLTDS.bit.PLLTDS = dirs;
    }
}


 /*!
 * @brief   This feature configures pin currentMux and supports configuration for
 *          pins with ADC interleaved channels
 *
 * @param   pmBase   : pm base pointer.
 * @param   pin      : port pin.
 * @param   mux      : port pin muxing.
 *
 */
void PINS_HW_SetMuxModeSel(PM_T * const pmBase, uint32_t pin, PM_MUX_T mux)
{
    uint32_t regVal = pmBase->PINCTRL[pin].reg;
    PM_MUX_T currentMux = mux;

#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
    if (currentMux == PM_MUX_ADC_INTERLEAVE)
    {
        /* Get ADC Interleave from SIM and enable/disable desired bit */
        uint32_t chipCtrlReg = (uint32_t) (SIM->CHIPCTRL.bit.ADCIPSEL);

        uint32_t interleaveVal = PINS_ReadAdcInterleaveVal(pmBase, pin, chipCtrlReg);
        if (interleaveVal != (uint32_t)PIN_ADC_INTERLEAVE_INVALID)
        {
            SIM->CHIPCTRL.bit.ADCIPSEL = interleaveVal;
        }
        /* return real currentMux for pin */
        currentMux = PM_PIN_DISABLED;
    }
#endif
    regVal &= ~(0x700u);
    regVal |= (((uint32_t)(((uint32_t)(currentMux))<<8u))&0x700u);
    pmBase->PINCTRL[pin].reg = regVal;
}



 /*!
 * @brief   Quickly configures multiple pins with the same pin configuration.
 *
 * @param   pmBase   : pm base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: pins corresponding to bits with val of '1' is updated with the val input
 *                - 1: pins corresponding to bits with val of '0' is not updated with the val input
 * @param   val      : When the pin is set to 1,the configuration value will be updated
 * @param   halfPm   : half the pin of the register.
 *
 */
void PINS_HW_SetGlobalPinCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm)
{
    uint16_t maskTmp = 0;
    /* keep only available fields */
    maskTmp |= 0x1u;
    maskTmp |= 0x2u;
    maskTmp |= 0x10u;
    maskTmp |= 0x40u;
    maskTmp |= 0x700u;
    maskTmp |= 0x8000u;
    maskTmp &= val;

    if(halfPm == PM_GLOBAL_CTRL_LOWER_HALF_PINS)
    {
        pmBase->GPINCTRLL.reg = (((uint32_t)pins) << 16u) | (uint32_t)maskTmp;
    }
    else if(halfPm == PM_GLOBAL_CTRL_UPPER_HALF_PINS)
    {
        pmBase->GPINCTRLH.reg = (((uint32_t)pins) << 16u) | (uint32_t)maskTmp;
    }
}



 /*!
 * @brief   Quickly configures multiple pins with the same interrupt configuration.
 *
 * @param   pmBase   : pm base pointer.
 * @param   pins     : Each bit represents a pin:
 *                - 0: pins corresponding to bits with value of '1' is updated with the value input
 *                - 1: pins corresponding to bits with value of '0' is not updated with the value input
 * @param   val      : When the pin is set to 1,the configuration value will be updated
 * @param   halfPm   : half the pin of the register.
 *
 */
void PINS_HW_SetGlobalIntCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm)
{
    uint32_t maskTmp;
    maskTmp = (((uint32_t)val) << 16u) & 0xF0000u;

    if(halfPm == PM_GLOBAL_CTRL_LOWER_HALF_PINS)
    {
        pmBase->GINTCTRLL.reg = ((uint32_t)pins) | maskTmp;
    }
    else if(halfPm == PM_GLOBAL_CTRL_UPPER_HALF_PINS)
    {
        pmBase->GINTCTRLH.reg = ((uint32_t)pins) | maskTmp;
    }
}

#endif /* FEATURE_SOC_PM_COUNT > 0*/


/**@} end of group PINS_Functions*/
/**@} end of group PINS_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
