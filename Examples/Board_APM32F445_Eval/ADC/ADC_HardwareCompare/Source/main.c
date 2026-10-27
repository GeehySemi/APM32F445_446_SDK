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

/* Includes */
#include "user_config.h"
#include "apm32f445_446_adc.h"
#include "apm32f445_446_pins.h"

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup ADC_HardwareCompare
  @{
  */
 /** @defgroup ADC_HardwareCompare_Macros Macros
  @{
*/

/* ADC instance */
#define ADC_INSTANCE    (0U)

/**@} end of group ADC_HardwareCompare_Macros*/

 /** @defgroup ADC_HardwareCompare_Variables Variables
  @{
*/
static __IO uint32_t g_timingDelay;
float g_adcMaxValue = 4096;
float g_adcDifference = 3.3f;
bool g_compareFlag;

/**@} end of group ADC_HardwareCompare_Variables*/

/** @defgroup ADC_HardwareCompare_Functions Functions
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
    while(g_timingDelay != 0);
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
    if(g_timingDelay != 0)
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
    /* Reset adc module */
    ADC_Reset(ADC_INSTANCE);

    /* Initialize ADC based on configuration values */
    ADC_ConfigConverter(ADC_INSTANCE,&adcConvCfg0);

    /* Verify ADC */
    ADC_AutoCalibration(ADC_INSTANCE);

    /* Enable hardware comparison function */
    ADC_ConfigHwCompare(0,&adcCompareCfg0);
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

    /* Trigger using software */
    ADC_ConfigChan(ADC_INSTANCE, 0u, &adcChanCfg0);

    /* waiting for conversion to complete */
    ADC_WaitConvDone(ADC_INSTANCE);

    /* Obtain conversion flag bits */
    g_compareFlag = ADC_ReadConvCompleteFlag(ADC_INSTANCE,0);

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

    /* LED init */
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

        /* serial output ADC conversion value ,Compare flag bits */
        printf("ADC conversion voltage value = %fV    Compare flag bits = %d\r\n",adcValue,g_compareFlag);

        if(g_compareFlag == 0)
        {
            LED_On(LED_RED);
            LED_Off(LED_GREEN);
        }
        else
        {
            LED_On(LED_GREEN);
            LED_Off(LED_RED);
        }
    }
}

/**@} end of group ADC_HardwareCompare_Functions */
/**@} end of group ADC_HardwareCompare */
/**@} end of group Examples */
