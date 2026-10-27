/*!
 * @file        main.c
 *
 * @brief       Main program
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

/*!
 *  Use case considerations:
 *  Development board pin D16 connected to C14.
 *
 *  Use case phenomenon:
 *  The sliding rheostat on the development board can be used to adjust the color of the LED (from white to pink),
 *  and the voltage value converted by the ADC channel can be printed in real-time
 *  through the serial port(the Baud of the serial port is 9600).
 */

/* Includes */
#include "user_config.h"
#include "apm32f445_446_adc.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_pdu.h"


/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup ADC_HardwareConversion
  @{
  */
/** @defgroup ADC_HardwareConversion_Macors Macors
  @{
*/
#define ADC_INSTANCE    (0U)
#define PDU_INSTANCE    (0U)

/**@} end of group ADC_HardwareConversion_Macors*/

/** @defgroup ADC_HardwareConversion_Variables Variables
  @{
*/
float g_adcMaxValue = 4096;
float g_adcDifference = 3.3f;
bool g_adcIrqStart;
uint16_t g_adcRawValue;

/**@} end of group ADC_HardwareConversion_Variables*/

/** @defgroup ADC_HardwareConversion_Functions Functions
  @{
  */
/*!
 * @brief   ADC interrupt interrupt callback function
 *
 * @param   None
 *
 * @retval  None
 */
void ADC_IRQHandler(void)
{
    /* Obtain conversion results */
    ADC_ReadChanResult(ADC_INSTANCE, 0u, (uint16_t *)&g_adcRawValue);

    g_adcIrqStart = true;
}

/*!
 * @brief   PDU init
 *
 * @param   None
 *
 * @retval  None
 */
void PDU_init(void)
{
    uint16_t delayValue = 0xafff;

    /* Initialize PDU */
    PDU_Init(PDU_INSTANCE,&pduTimerCfg0);

    /* Enable PDU */
    PDU_Enable(PDU_INSTANCE);

    /* Configure ADC pre triggering */
    PDU_ConfigAdcPreTrigger(PDU_INSTANCE,0,&pduAdcTriggerCfg0);

    /* Configures the value of timer modulus */
    PDU_ConfigPeriodCountValue(PDU_INSTANCE,delayValue);

    /* Configure ADC pre triggering delay value */
    PDU_ConfigAdcPreTriggerDelayValue(PDU_INSTANCE,0,0,delayValue);
    PDU_LoadValuesCmd(PDU_INSTANCE);
    PDU_EnableSoftTrigger(PDU_INSTANCE);
}

/*!
 * @brief   ADC hardware conversion initialization configuration
 *
 * @param   None
 *
 * @retval  None
 */
void ADC_init(void)
{
    ADC_CONV_CFG_T tempAdcConvCfg0;
    ADC_CALIBRATION_T tempAdcCalibration0;
    ADC_CHAN_CONFIG_T tempAdcChanCfg0;

    /* Reset adc module */
    ADC_Reset(ADC_INSTANCE);

    /* Call default configuration */
    ADC_ConfigConverterStruct(&tempAdcConvCfg0);
    tempAdcConvCfg0.clockDivision = ADC_CLK_DIVISION_2;
    tempAdcConvCfg0.resolution = ADC_RESOLUTION_RATIO_12BIT;
    tempAdcConvCfg0.trigger = ADC_HARDWARE_TRIGGER;

    /* Call default configuration */
    ADC_ConfigUserCalibrationStruct(&tempAdcCalibration0);
    tempAdcCalibration0.userGain = ADC_DEFAULT_SAMPLE_TIME;
    tempAdcCalibration0.userOffset = 20000u;

    /* Call default configuration */
    ADC_ConfigChanStruct(&tempAdcChanCfg0);
    tempAdcChanCfg0.interruptEnable = true;
    tempAdcChanCfg0.channel = ADC_INPUT_CHANNEL_EXT12;

    /* Reset adc module */
    ADC_Reset(ADC_INSTANCE);

    /* Initialize ADC based on configuration values */
    ADC_ConfigConverter(ADC_INSTANCE,&tempAdcConvCfg0);

    /* Enable calibration function */
    ADC_ConfigUserCalibration(ADC_INSTANCE,&tempAdcCalibration0);

    /* Verify ADC */
    ADC_AutoCalibration(ADC_INSTANCE);

    /* Trigger using hardware */
    ADC_ConfigChan(ADC_INSTANCE, 0u, &tempAdcChanCfg0);
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

    g_adcIrqStart = false;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* LED init */
    LED_Init();

    /* UART init */
    COM_Init();

    /* ADC init */
    ADC_init();

    /* Set ADC interrupt callback function */
    INT_SYS_InstallHandler(ADC0_IRQn,&ADC_IRQHandler,(ISR_T*) 0);

    /* PDU init */
    PDU_init();

    /* Enable ADC interrupt */
    INT_SYS_EnableIRQ(ADC0_IRQn);

    while (1)
    {
        if(g_adcIrqStart == true)
        {
            /* Calculated voltage value */
            adcValue = (float)g_adcRawValue / g_adcMaxValue * g_adcDifference;

            /* Output ADC conversion value */
            printf("ADC conversion voltage value = %fV\r\n",adcValue);

            g_adcIrqStart = false;
            PDU_EnableSoftTrigger(PDU_INSTANCE);
        }
    }
}

/**@} end of group ADC_HardwareConversion_Functions */
/**@} end of group ADC_HardwareConversion */
/**@} end of group Examples */
