/*!
 * @file        apm32f445_446_pins.h
 *
 * @brief       This file contains all the functions prototypes for the PINS firmware library.
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


#ifndef APM32F445_446_PINS_H
#define APM32F445_446_PINS_H

/* Includes */
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup PINS_Driver PINS Driver
  @{
*/

/** @defgroup PINS_Macros Macros
  @{
*/

/**@} end of group PINS_Macros*/

/** @defgroup PINS_Enumerations Enumerations
  @{
*/
/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/
/* Configures the port data directory */
typedef enum
{
    GPIO_INPUT_DIR       = 0x0U,  /* Universal input directory */
    GPIO_OUTPUT_DIR      = 0x1U,  /* Universal output directory */
    GPIO_UNSPECIFIED_DIR = 0x2U   /* Universal unspecified directory */
} PM_DATA_DIR_T;

/* Internal resistance selection */
typedef enum
{
    PM_INTERNAL_PULL_NOT_ENABLED   = 0U,  /* Disable internal resistance */
    PM_INTERNAL_PULL_DOWN_ENABLED  = 1U,  /* Configure a pull-down resistor */
    PM_INTERNAL_PULL_UP_ENABLED    = 2U   /* Configure a pull-up resistor */
} PM_PULL_CFG_T;

/* Configures the drive strength */
typedef enum
{
    PM_LOW_DRIVE_STRENGTH     = 0U, /* Configure low drive strength */
    PM_HIGH_DRIVE_STRENGTH    = 1U  /* Configure high drive strength */
} PM_DRIVE_STRENGTH_T;


/* Configures the Pin mux selection */
typedef enum
{
    PM_PIN_DISABLED         = 0U,  /* Disable pins, but is used as an analog pin */
    PM_MUX_AS_GPIO          = 1U,  /* The pin configuration is GPIO */
    PM_MUX_ALT2             = 2U,  /* Reflection 2 */
    PM_MUX_ALT3             = 3U,  /* Reflection 3 */
    PM_MUX_ALT4             = 4U,  /* Reflection 4 */
    PM_MUX_ALT5             = 5U,  /* Reflection 5 */
    PM_MUX_ALT6             = 6U,  /* Reflection 6 */
    PM_MUX_ALT7             = 7U,  /* Reflection 7 */
#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
    PM_MUX_ADC_INTERLEAVE   = 8U   /* when selected, ADC Interleaved channel is connected to current pin
                                         and disconnected to opposed pin
                                         ADC1_SE14-PMB15 | ADC1_SE15-PMB16 | ADC0_SE8-PMC0  | ADC0_SE9-PMC1
                                         ADC1_SE14-PMB0  | ADC1_SE15-PMB1  | ADC0_SE8-PMB13 | ADC0_SE9-PMB14 */
#endif /* FEATURE_PINS_WITH_ADC_INTERLEAVE_EN */
} PM_MUX_T;


/* Configures interrupt generation conditions */
typedef enum
{
    PM_DMA_INT_DISABLED  = 0x0U,  /* Interrupt/DMA request is disabled */
    PM_DMA_RISING_EDGE   = 0x1U,  /* Rising edge DMA request */
    PM_DMA_FALLING_EDGE  = 0x2U,  /* Falling edge DMA request */
    PM_DMA_EITHER_EDGE   = 0x3U,  /* Either edge DMA request */
    PM_INT_LOGIC_ZERO    = 0x8U,  /* Interrupt when logic 0 */
    PM_INT_RISING_EDGE   = 0x9U,  /* Rising edge interrupt */
    PM_INT_FALLING_EDGE  = 0xAU,  /* Falling edge interrupt */
    PM_INT_EITHER_EDGE   = 0xBU,  /* Either edge interrupt */
    PM_INT_LOGIC_ONE     = 0xCU,  /* Interrupt when logic 1 */
} PM_INTERRUPT_CFG_T;

/* Clock source the digital input filter */
typedef enum
{
    PM_DIGIT_FILT_BUS_CLK  = 0U,  /* The digital filter is clocked by the bus clock */
    PM_DIGIT_FILT_LPO_CLK  = 1U   /* The digital filter is clocked by the LPO clock */
} PM_DIGIT_FILT_CLK_T;

/* port global pin/interuppt control register */
typedef enum
{
    PM_GLOBAL_CTRL_LOWER_HALF_PINS = 0U,    /* Lower end of configuration pin */
    PM_GLOBAL_CTRL_UPPER_HALF_PINS = 1U     /* Upper end of configuration pin */
} PM_GLOBAL_CTRL_PINS_T;


#if FEATURE_SOC_PM_COUNT > 0

#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
/* ADC Interleave currentMux selectionF */
typedef enum
{
    PIN_ADC_INTERLEAVE_DISABLE0 = 0xEu, /* xxx0b ADC1_SE14 channel is connected to PMB15 */
    PIN_ADC_INTERLEAVE_DISABLE1 = 0xDu, /* xx0xb ADC1_SE15 channel is connected to PMB16 */
    PIN_ADC_INTERLEAVE_DISABLE2 = 0xBu, /* x0xxb ADC0_SE8  channel is connected to PMC0  */
    PIN_ADC_INTERLEAVE_DISABLE3 = 0x7u, /* 0xxxb ADC0_SE9  channel is connected to PMC1  */
    PIN_ADC_INTERLEAVE_ENABLE0  = 0x1u, /* xxx1b ADC1_SE14 channel is connected to PMB0  */
    PIN_ADC_INTERLEAVE_ENABLE1  = 0x2u, /* xx1xb ADC1_SE15 channel is connected to PMB1  */
    PIN_ADC_INTERLEAVE_ENABLE2  = 0x4u, /* x1xxb ADC0_SE8  channel is connected to PMB13 */
    PIN_ADC_INTERLEAVE_ENABLE3  = 0x8u, /* 1xxxb ADC0_SE9  channel is connected to PMB14 */
    PIN_ADC_INTERLEAVE_INVALID  = 0xFFu /* ADC interleave is invalid */
} PIN_ADC_INTERLEAVE_MUX_T;

#endif /* FEATURE_PINS_WITH_ADC_INTERLEAVE_EN */

#endif /* FEATURE_SOC_PM_COUNT */

/**@} end of group PINS_Enumerations*/


/** @defgroup PINS_Structures Structures
  @{
*/
/*******************************************************************************
 *                              TYPE DEFINES
 ******************************************************************************/
/* Type of a GPIO channel representation */
typedef uint32_t PINS_CHANNEL_TYPE_T;

/*Type of a port levels representation */
typedef uint8_t PINS_LEV_TYPE_T;

/* The digital filter configuration */
typedef struct
{
    uint8_t             width;  /* The digital filter width value */
    PM_DIGIT_FILT_CLK_T clock;  /* The digital filter clock for port */
} PM_DIGIT_FILT_CFG_T;



/* Defines the converter configuration */
typedef struct
{
    PINS_LEV_TYPE_T         initVal;        /*Type of a port levels representation */
    PM_DATA_DIR_T           dir;            /* Configures the port data directory */
    GPIO_T *                gpioBase;       /* GPIO base pointer */
    PM_INTERRUPT_CFG_T      intCfg;         /* Configures interrupt generation conditions */
    bool                    clrIntFlg;      /* Clears the interrupt status flag */
    bool                    digitFilt;      /* Enables digital filter */
    bool                    pinLk;          /* Lock pin control register or not */
    PM_DRIVE_STRENGTH_T     drvSel;         /* Configures the drive strength */
    PM_MUX_T                mux;            /* Configures the Pin mux selection */
    bool                    passiveFilt;    /* Passive filter configuration */
    PM_PULL_CFG_T           pullCfg;        /* Internal resistance selection */
    uint32_t                pinPmIdx;       /* PM pin number */
    PM_T *                  base;           /* PM base pointer */
} PIN_SETTINGS_CFG_T;

/**@} end of group PINS_Structures*/

/** @defgroup PINS_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
STATUS_T PINS_Init(uint32_t pinCnt, const PIN_SETTINGS_CFG_T cfg[]);
void PINS_SetPullSel(PM_T * const pmBase, uint32_t pin, PM_PULL_CFG_T pullCfg);
void PINS_SetMuxModeSel(PM_T * const pmBase, uint32_t pin, PM_MUX_T mux);
void PINS_SetPinIntSel(PM_T * const pmBase, uint32_t pin, PM_INTERRUPT_CFG_T intCfg);
PM_INTERRUPT_CFG_T PINS_ReadPinIntSel(const PM_T * const pmBase, uint32_t pin);
void PINS_ClrPinIntFlgCmd(PM_T * const pmBase, uint32_t pin);
void PINS_EnableDigitFilt(PM_T * const pmBase, uint32_t pin);
void PINS_DisableDigitFilt(PM_T * const pmBase, uint32_t pin);
void PINS_CfgDigitFilt(PM_T * const pmBase, const PM_DIGIT_FILT_CFG_T * const cfg);
uint32_t PINS_ReadPmIntFlg(const PM_T * const pmBase);
void PINS_ClrPmIntFlgCmd(PM_T * const pmBase);
void PINS_SetGlbPinCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);
void PINS_SetGlbIntCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);
PINS_CHANNEL_TYPE_T PINS_ReadPinsDir(const GPIO_T * const gpioBase);
void PINS_SetPinDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T dir);
void PINS_SetPinsDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);

#if FEATURE_PM_WITH_INPUT_DISABLE
void PINS_SetPmInputDisable(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_ReadPmInputDisable(const GPIO_T * const gpioBase);
#endif /* FEATURE_PM_WITH_INPUT_DISABLE */

void PINS_WritePin(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T val);
void PINS_WritePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_ReadPinsOutput(const GPIO_T * const gpioBase);
void PINS_SetPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
void PINS_ClrPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
void PINS_TogglePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_ReadPinsInput(const GPIO_T * const gpioBase);


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/
void PINS_HW_SetPullSel(PM_T * const pmBase, uint32_t pin, PM_PULL_CFG_T pullCfg);
void PINS_HW_SetMuxModeSel(PM_T * const pmBase, uint32_t pin, PM_MUX_T mux);;
void PINS_HW_SetPinIntSel(PM_T * const pmBase, uint32_t pin, PM_INTERRUPT_CFG_T intCfg);
PM_INTERRUPT_CFG_T PINS_HW_ReadPinIntSel(const PM_T * const pmBase, uint32_t pin);
void PINS_HW_ClrPinIntFlgCmd(PM_T * const pmBase, uint32_t pin);
void PINS_HW_EnableDigitFilt(PM_T * const pmBase, uint32_t pin);
void PINS_HW_DisableDigitFilt(PM_T * const pmBase, uint32_t pin);
void PINS_HW_CfgDigitFilt(PM_T * const pmBase, const PM_DIGIT_FILT_CFG_T * const cfg);
uint32_t PINS_HW_ReadPmIntFlg(const PM_T * const pmBase);
void PINS_HW_ClrPmIntFlgCmd(PM_T * const pmBase);
void PINS_HW_SetGlobalPinCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);
void PINS_HW_SetGlobalIntCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);

PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsDir(const GPIO_T * const gpioBase);
void PINS_HW_GPIO_SetPinDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T dir);
void PINS_HW_GPIO_SetPinsDir(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);

#if FEATURE_PM_WITH_INPUT_DISABLE
void PINS_HW_GPIO_SetPmInputDisable(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPmInputDisable(const GPIO_T * const gpioBase);
#endif /* FEATURE_PM_WITH_INPUT_DISABLE */

void PINS_HW_GPIO_WritePin(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pin, PINS_LEV_TYPE_T val);
void PINS_HW_GPIO_WritePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsOutput(const GPIO_T * const gpioBase);
void PINS_HW_GPIO_SetPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
void PINS_HW_GPIO_ClrPins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
void PINS_HW_GPIO_TogglePins(GPIO_T * const gpioBase, PINS_CHANNEL_TYPE_T pins);
PINS_CHANNEL_TYPE_T PINS_HW_GPIO_ReadPinsInput(const GPIO_T * const gpioBase);

#if FEATURE_SOC_PM_COUNT > 0

#if FEATURE_PINS_WITH_ADC_INTERLEAVE_EN
uint32_t PINS_ReadAdcInterleaveVal(const PM_T * pmBase, const uint32_t pinPmIdx, const uint32_t currentVal);
#endif /* FEATURE_PINS_WITH_ADC_INTERLEAVE_EN */

void PINS_HW_Cfg(const PIN_SETTINGS_CFG_T * cfg);
void PINS_HW_SetMuxModeSel(PM_T * const pmBase, uint32_t pin, PM_MUX_T mux);
void PINS_HW_SetGlobalPinCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);
void PINS_HW_SetGlobalIntCtrl(PM_T * const pmBase, uint16_t pins, uint16_t val, PM_GLOBAL_CTRL_PINS_T halfPm);

#endif /* FEATURE_SOC_PM_COUNT > 0 */



/**@} end of group PINS_Functions*/
/**@} end of group PINS_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/


#endif /* APM32F445_446_PINS_H */

