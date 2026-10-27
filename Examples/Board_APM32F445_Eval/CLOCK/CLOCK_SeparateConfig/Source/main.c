/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "stdio.h"
#include "main.h"
#include "board.h"
#include "system_apm32f445_446.h"
#include "user_config.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_lpuart.h"
/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CLOCK_SeparateConfig
  @{
  */
/** @defgroup CLOCK_SeparateConfig_Variables Variables
  @{
*/
static __IO uint32_t TimingDelay;

/**@} end of group CLOCK_SeparateConfig_Variables*/

/** @defgroup CLOCK_SeparateConfig_Functions Functions
  @{
  */
static void ReadConfigClockFreq(void);

/*!
 * @brief       Start SysTick
 *
 * @param       None
 *
 * @retval      None
 */
void SysTick_Init(void)
{
    SystemCoreClockUpdate();    /* update System Core Clock info */
    /** SystemFrequency / 1000 = 1ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /** Capture error */
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
    TimingDelay = nTime;
    while(TimingDelay != 0);
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
    if(TimingDelay != 0)
    {
        TimingDelay--;
    }
}
/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    PWR_MODES_T powerMode = RUN_MODE;
    LPUART_STATE_T g_lpuart1State;
    LPUART_CFG_T uartCfg =
    {
        .transferType = LPUART_USE_INTERRUPTS,
        .baudrate = 115200U,
        .dataBits = LPUART_8_BITS_DATA,
        .stopBits = LPUART_ONE_STOP_BIT,
        .parityMode = LPUART_PARITY_DISABLED,
        .txDmaChannel = 0U,
        .rxDmaChannel = 0U
    };

    CLOCK_SYS_ConfigClockSource(SYSOSC_CLK, &SoscClockSource);
    CLOCK_SYS_ConfigClockSource(HSICLK_CLK, &HsiClockSource);
    CLOCK_SYS_ConfigSystemClock(&powerMode, &systemClockCfg);
    CLOCK_SYS_ConfigModuleClock(PMD_CLK, &PMDClockCfg);
    CLOCK_SYS_ConfigModuleClock(PMC_CLK, &PMCClockCfg);
    CLOCK_SYS_ConfigModuleClock(LPUART1_CLK, &lpuart1ClockCfg);

    SysTick_Init();
    /* Initalize Blue LEDs */
    PINS_SetMuxModeSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_MUX_AS_GPIO);
    PINS_SetPinIntSel(LED_BLUE_PORT_BASE, LED_BLUE_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinDir(LED_BLUE_GPIO_BASE, LED_BLUE_PIN, 1U);
    PINS_SetPins(LED_BLUE_GPIO_BASE, 1U << LED_BLUE_PIN);
    /* Initalize LPUART1 TX */
    PINS_SetMuxModeSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_MUX_ALT2);
    PINS_SetMuxModeSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_MUX_ALT2);
    PINS_SetPinIntSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_DMA_INT_DISABLED);
    LPUART_Init(LPUART_INSTANCE_1, &g_lpuart1State, &uartCfg);
    LPUART_HW_EnableTx(LPUART1);

    /* Read user clock configuration frequency*/
    ReadConfigClockFreq();

    while(1)
    {
        PINS_ClrPins(LED_BLUE_GPIO_BASE, 1U << LED_BLUE_PIN);
        SysTick_Delay_ms(500);
        PINS_SetPins(LED_BLUE_GPIO_BASE, 1U << LED_BLUE_PIN);
        SysTick_Delay_ms(500);
    }
}
/*!
 * @brief       Read Clock Frequency
 *
 * @param       None
 *
 * @retval      None
 */
void ReadConfigClockFreq(void)
{
    uint32_t freq[10];
    STATUS_T returnStatus = STATUS_SUCCESS;

    returnStatus = CLOCK_SYS_ReadFreq(SYSOSC_CLK, freq);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSOSC_CLK Read Freq Successful \r\n");
        printf("SYSOSC_CLK Freq : %d \r\n", freq[0]);
    }
    else
    {
        printf("SYSOSC_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(HSICLK_CLK, freq+1);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("HSICLK_CLK Read Freq Successful \r\n");
        printf("HSICLK_CLK Freq : %d \r\n", freq[1]);
    }
    else
    {
        printf("HSICLK_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CORE_CLK, freq+2);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CORE_CLK Read Freq Successful \r\n");
        printf("CORE_CLK Freq : %d \r\n", freq[2]);
    }
    else
    {
        printf("CORE_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(BUS_CLK, freq+3);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("BUS_CLK Read Freq Successful \r\n");
        printf("BUS_CLK Freq : %d \r\n", freq[3]);
    }
    else
    {
        printf("BUS_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SLOW_CLK, freq+4);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SLOW_CLK Read Freq Successful \r\n");
        printf("SLOW_CLK Freq : %d \r\n", freq[4]);
    }
    else
    {
        printf("SLOW_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPUART1_CLK, freq+5);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPUART1_CLK Read Freq Successful \r\n");
        printf("LPUART1_CLK Freq : %d \r\n", freq[5]);
    }
    else
    {
        printf("LPUART1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PMD_CLK, freq+6);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PMD_CLK Read Freq Successful \r\n");
        printf("PMD_CLK Freq : %d \r\n", freq[6]);
    }
    else
    {
        printf("PMD_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }
}

/**@} end of group CLOCK_SeparateConfig_Functions */
/**@} end of group CLOCK_SeparateConfig */
/**@} end of group Examples */
