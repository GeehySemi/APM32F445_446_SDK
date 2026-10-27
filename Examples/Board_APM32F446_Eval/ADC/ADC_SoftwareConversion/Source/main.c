/*!
 * @file        main.c
 *
 * @brief       Main program
 *
 * @version     V1.0.0
 *
 * @date        2024-03-20
 *
 * @attention
 *
 *  Copyright (C) 2024 Geehy Semiconductor
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
#include "user_config.h"
#include "apm32f445_446_adc.h"
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup ADC_SoftwareConversion
  @{
  */

/** @defgroup ADC_SoftwareConversion_Macros Macros
  @{
*/
#define ADC_INSTANCE    (0U)

/**@} end of group ADC_SoftwareConversion_Macros*/

/** @defgroup ADC_SoftwareConversion_Variables Variables
  @{
*/
static __IO uint32_t g_timingDelay;
float g_adcMaxValue = 4096;
float g_adcDifference = 3.3f;

/**@} end of group ADC_SoftwareConversion_Variables*/

/** @defgroup ADC_SoftwareConversion_Functions Functions
  @{
  */
/*!
 * @brief       Start SysTick
 *
 * @param       None
 *
 * @retval      None
 */
void SysTick_Init(void)
{
    /* Update System Core Clock info */
    SystemCoreClockUpdate();

    /* SystemFrequency / 1000 = 1ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */
        while (1);
    }
}

/*!
 * @brief       Precise Delay
 *
 * @param       nTime in milliseconds
 *
 * @retval      None
 */
void SysTick_Delay_ms(__IO uint32_t nTime)
{
    g_timingDelay = nTime;
    while (g_timingDelay != 0);
}

/*!
 * @brief       Decrements the TimingDelay
 *
 * @param       None
 *
 * @retval      None
 */
void TimingDelay_Decrement(void)
{
    if (g_timingDelay != 0)
    {
        g_timingDelay--;
    }
}

/*!
 * @brief   ADC software conversion initialization configuration
 *
 * @param   None
 *
 * @retval  None
 */
void ADC_init(void)
{
    ADC_CONV_CFG_T tempAdcConvCfg0;
    ADC_AVG_CFG_T tempAdcAverCfg0;
    ADC_CALIBRATION_T tempAdcCalibration0;

    /* Reset adc module */
    ADC_Reset(ADC_INSTANCE);

    /* Call default configuration */
    ADC_ConfigConverterStruct(&tempAdcConvCfg0);
    tempAdcConvCfg0.clockDivision = ADC_CLK_DIVISION_4;
    tempAdcConvCfg0.resolution = ADC_RESOLUTION_RATIO_12BIT;

    /* Call default configuration */
    ADC_ConfigHwAverageStruct(&tempAdcAverCfg0);
    tempAdcAverCfg0.hwAvgEnable = true;
    tempAdcAverCfg0.hwAverage = ADC_HW_AVERAGE_32;

    /* Call default configuration */
    ADC_ConfigUserCalibrationStruct(&tempAdcCalibration0);
    tempAdcCalibration0.userGain = ADC_DEFAULT_SAMPLE_TIME;
    tempAdcCalibration0.userOffset = 20000u;

    /* Set software pre triggering */
    ADC_SetSwPretrigger(ADC_INSTANCE,ADC_SW_PRE_TRIGGER_0);

    /* Initialize ADC based on configuration values */
    ADC_ConfigConverter(ADC_INSTANCE,&tempAdcConvCfg0);

    /* Initialize ADC hardware averaging function based on configuration values */
    ADC_ConfigHwAverage(ADC_INSTANCE,&tempAdcAverCfg0);

    /* Enable calibration function */
    ADC_ConfigUserCalibration(ADC_INSTANCE,&tempAdcCalibration0);

    /* Verify ADC */
    ADC_AutoCalibration(ADC_INSTANCE);
}

/*!
 * @brief   Read ADC value
 *
 * @param   None
 *
 * @retval  ADC value
 */
float ADC_readValue(void)
{
    uint16_t partAdcValue;
    ADC_CHAN_CONFIG_T tempAdcChanCfg0;

    /* Call default configuration */
    ADC_ConfigChanStruct(&tempAdcChanCfg0);
    tempAdcChanCfg0.interruptEnable = false;
    tempAdcChanCfg0.channel = ADC_INPUT_CHANNEL_EXT12;

    /* Trigger using software */
    ADC_ConfigChan(ADC_INSTANCE, 0u, &tempAdcChanCfg0);

    /* waiting for conversion to complete */
    ADC_WaitConvDone(ADC_INSTANCE);

    /* Obtain conversion results */
    ADC_ReadChanResult(ADC_INSTANCE, 0u, &partAdcValue);
    return (float)partAdcValue;
}

/*!
 * @brief   Main function
 *
 * @param   None
 *
 * @retval  None
 */
int main(void)
{
    float adcValue;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Led init */
    LED_Init();

    /* UART init */
    COM_Init();

    /* ADC init */
    ADC_init();

    SysTick_Init();

    while (1)
    {
        SysTick_Delay_ms(250);

        /* Calculated voltage value */
        adcValue = ADC_readValue() / g_adcMaxValue * g_adcDifference;

        /* Output ADC conversion value */
        printf("ADC conversion voltage value = %fV\r\n",adcValue);
    }
}

/**@} end of group ADC_SoftwareConversion_Functions */
/**@} end of group ADC_SoftwareConversion */
/**@} end of group Examples */
