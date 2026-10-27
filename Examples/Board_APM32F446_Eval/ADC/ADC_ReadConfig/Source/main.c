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

/** @addtogroup ADC_ReadConfig
  @{
  */

/** @defgroup ADC_ReadConfig_Macros Macros
  @{
  */
#define INSTANCE    (0U)
#define START_STR   "\r\nADC configuration values:\r\n\r\n"

/* Compare cvalue */
#define COMPVAL1    (0U)
#define COMPVAL2    (4095U)

/**@} end of group ADC_ReadConfig_Macros*/

/** @defgroup ADC_ReadConfig_Variables Variables
  @{
  */
float g_adcMaxValue = 4096;
float g_adcDifference = 3.3f;

/**@} end of group ADC_ReadConfig_Variables*/
/** @defgroup ADC_ReadConfig_Functions Functions
  @{
  */
/*!
 * @brief   ADC software conversion initialization configuration
 *
 * @param   None
 *
 * @retval  None
 */
void ADC_init(void)
{
    ADC_CONV_CFG_T adcConvCfg0;
    ADC_AVG_CFG_T adcAverCfg0;
    ADC_COMP_CFG_T adcCompareCfg0;
    ADC_CALIBRATION_T adcCalibration0;

    /* Reset adc module */
    ADC_Reset(INSTANCE);

    /* Calling ADC default initialization values */
    ADC_ConfigConverterStruct(&adcConvCfg0);
    adcConvCfg0.clockDivision = ADC_CLK_DIVISION_4;
    adcConvCfg0.resolution = ADC_RESOLUTION_RATIO_12BIT;

    /* Calling hardware average default initialization values */
    ADC_ConfigHwAverageStruct(&adcAverCfg0);
    adcAverCfg0.hwAvgEnable = true;
    adcAverCfg0.hwAverage = ADC_HW_AVERAGE_32;

    /* Call default calibration initial value*/
    ADC_ConfigUserCalibrationStruct(&adcCalibration0);
    adcCalibration0.userGain = ADC_DEFAULT_SAMPLE_TIME;
    adcCalibration0.userOffset = 20000u;

    /* Initialize ADC based on configuration values */
    ADC_ConfigConverter(INSTANCE,&adcConvCfg0);

    /* Initialize ADC hardware averaging function based on configuration values */
    ADC_ConfigHwAverage(INSTANCE,&adcAverCfg0);

    /* Enable calibration function */
    ADC_ConfigUserCalibration(INSTANCE,&adcCalibration0);

    /* Verify ADC */
    ADC_AutoCalibration(INSTANCE);

    /* Call default configuration */
    ADC_ConfigHwCompareStruct(&adcCompareCfg0);

    /* Configure hardware comparison function */
    adcCompareCfg0.compEnable = true;
    adcCompareCfg0.compGreaterThanEnable = true;
    adcCompareCfg0.compRangeFuncEnable = true;
    adcCompareCfg0.compValue1 = COMPVAL1;
    adcCompareCfg0.compValue2 = COMPVAL2;

    /* Enable hardware comparison function */
    ADC_ConfigHwCompare(INSTANCE,&adcCompareCfg0);
}

/*!
 * @brief   Read ADC conversion value
 *
 * @param   None
 *
 * @retval  ADC conversion value
 */
float ADC_ConvValue(void)
{
    uint16_t partAdcValue;
    ADC_CHAN_CONFIG_T adcChanCfg0;

    ADC_ConfigChanStruct(&adcChanCfg0);
    adcChanCfg0.interruptEnable = false;
    adcChanCfg0.channel = ADC_INPUT_CHANNEL_EXT12;

    /* Trigger using software */
    ADC_ConfigChan(INSTANCE,0u,&adcChanCfg0);
    /* waiting for conversion to complete */
    ADC_WaitConvDone(INSTANCE);
    /* Obtain conversion results */
    ADC_ReadChanResult(INSTANCE, 0u, &partAdcValue);

    return (float)partAdcValue;
}

/*!
 * @brief   Read configuration information
 *
 * @param   None
 *
 * @retval  None
 */
void ADC_ReadCfgInformation(void)
{
    ADC_CONV_CFG_T readAdcConvCfg;
    ADC_AVG_CFG_T readAdcAverCfg;
    ADC_COMP_CFG_T readCompareCfg;
    ADC_CHAN_CONFIG_T readAdcChanCfg;
    ADC_CALIBRATION_T readAdcCalibration0;

    printf(START_STR);

    /* Obtain converter config */
    ADC_ReadConverterConfig(INSTANCE,&readAdcConvCfg);
    printf("ADC clock Divider : %d\r\n",readAdcConvCfg.clockDivision);
    printf("ADC conversion resolution : %d\r\n",readAdcConvCfg.resolution);
    printf("\r\n");

    /* Obtain hardware Average config */
    ADC_ReadHwAverageConfig(INSTANCE,&readAdcAverCfg);
    printf("Enable hardware averaging function : %d\r\n",readAdcAverCfg.hwAvgEnable);
    printf("ADC hardware average : %d\r\n",readAdcAverCfg.hwAverage);
    printf("\r\n");

    /* Obtain hardware Compare config */
    ADC_ReadHwCompareConfig(INSTANCE,&readCompareCfg);
    printf("Enable the compare feature : %d\r\n",readCompareCfg.compEnable);
    printf("Enable Greater-Than functionality : %d\r\n",readCompareCfg.compGreaterThanEnable);
    printf("Enable Range functionality : %d\r\n",readCompareCfg.compRangeFuncEnable);
    printf("First Compare Value : %d\r\n",readCompareCfg.compValue1);
    printf("Second Compare Value : %d\r\n",readCompareCfg.compValue2);
    printf("\r\n");

    /* Obtain user configured calibration values */
    ADC_ReadUserCalibration(INSTANCE,&readAdcCalibration0);
    printf("User-configurable gain : %d\r\n",readAdcCalibration0.userGain);
    printf("User-configurable Offset : %d\r\n",readAdcCalibration0.userOffset);
    printf("\r\n");

    /* Obtain ADC conversion channel */
    ADC_ReadChanConfig(INSTANCE,0u,&readAdcChanCfg);
    printf("Enable interrupts for this channel : %d\r\n",readAdcChanCfg.interruptEnable);
    printf("ADC conversion channel : %d\r\n",readAdcChanCfg.channel);
    printf("\r\n");

    /* Obtain ADC interrupt number */
    printf("ADC interrupt number : %d\r\n",ADC_ReadInterruptNumber(INSTANCE));
    printf("\r\n");
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

    /* Calculated voltage value */
    adcValue = ADC_ConvValue() / g_adcMaxValue * g_adcDifference;

    /* read configuration information */
    ADC_ReadCfgInformation();

    /* serial output ADC conversion value */
    printf("ADC conversion voltage value = %fV\r\n", adcValue);

    return 0;
}

/**@} end of group ADC_ReadConfig_Functions */
/**@} end of group ADC_ReadConfig */
/**@} end of group Examples */
