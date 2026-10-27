/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "stdio.h"
#include "main.h"
#include "board.h"
#include "system_apm32f445_446.h"
#include "user_clock_config.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_lpuart.h"
/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CLOCK_AllClocksArrayConfiguration
  @{
  */
/** @defgroup CLOCK_AllClocksArrayConfiguration_Macros Macros
  @{
*/

#define CLOCKCONFIG_0 0U
#define CLOCKCONFIG_1 1U

/**@} end of group CLOCK_AllClocksArrayConfiguration_Macros*/

/** @defgroup CLOCK_AllClocksArrayConfiguration_Variables Variables
  @{
*/
static __IO uint32_t TimingDelay;

/**@} end of group CLOCK_AllClocksArrayConfiguration_Variables*/

/** @defgroup CLOCK_AllClocksArrayConfiguration_Functions Functions
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

    /* Config all clock at the same time, refer to user_clock_config.c */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr, CLOCK_CONFIG_CNT,
            g_clockCallbacksArr, CLOCK_CONFIG_CNT);
    CLOCK_SYS_UpdateConfiguration(CLOCKCONFIG_0, CLOCK_MANAGER_POLICY_AGREEMENT);

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
    printf("*************Clock Config 0 ***************\r\n");
    ReadConfigClockFreq();
    printf("*******************************************\r\n");

    printf("*************Clock Config 1 ***************\r\n");
    SysTick_Delay_ms(10);
    CLOCK_SYS_UpdateConfiguration(CLOCKCONFIG_1, CLOCK_MANAGER_POLICY_AGREEMENT);
    ReadConfigClockFreq();
    printf("*******************************************\r\n");

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
    uint32_t freq[65];
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

    returnStatus = CLOCK_SYS_ReadFreq(LSICLK_CLK, freq+1);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LSICLK_CLK Read Freq Successful \r\n");
        printf("LSICLK_CLK Freq : %d \r\n", freq[1]);
    }
    else
    {
        printf("LSICLK_CLKRead Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(HSICLK_CLK, freq+2);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("HSICLK_CLK Read Freq Successful \r\n");
        printf("HSICLK_CLK Freq : %d \r\n", freq[2]);
    }
    else
    {
        printf("HSICLK_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SYSPLL_CLK, freq+3);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSPLL_CLK Read Freq Successful \r\n");
        printf("SYSPLL_CLK Freq : %d \r\n", freq[3]);
    }
    else
    {
        printf("SYSPLL_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CORE_CLK, freq+4);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CORE_CLK Read Freq Successful \r\n");
        printf("CORE_CLK Freq : %d \r\n", freq[4]);
    }
    else
    {
        printf("CORE_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(BUS_CLK, freq+5);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("BUS_CLK Read Freq Successful \r\n");
        printf("BUS_CLK Freq : %d \r\n", freq[5]);
    }
    else
    {
        printf("BUS_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SLOW_CLK, freq+6);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SLOW_CLK Read Freq Successful \r\n");
        printf("SLOW_CLK Freq : %d \r\n", freq[6]);
    }
    else
    {
        printf("SLOW_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(RTC_CLKIN_CLK, freq+7);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("RTC_CLKIN_CLK Read Freq Successful \r\n");
        printf("RTC_CLKIN_CLK Freq : %d \r\n", freq[7]);
    }
    else
    {
        printf("RTC_CLKIN_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SCG_CLKOUT_CLK, freq+8);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SCG_CLKOUT_CLK Read Freq Successful \r\n");
        printf("SCG_CLKOUT_CLK Freq : %d \r\n", freq[8]);
    }
    else
    {
        printf("SCG_CLKOUT_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LSIDIV1_CLK, freq+9);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LSIDIV1_CLK Read Freq Successful \r\n");
        printf("LSIDIV1_CLK Freq : %d \r\n", freq[9]);
    }
    else
    {
        printf("LSIDIV1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LSIDIV2_CLK, freq+10);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LSIDIV2_CLK Read Freq Successful \r\n");
        printf("LSIDIV2_CLK Freq : %d \r\n", freq[10]);
    }
    else
    {
        printf("LSIDIV2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(HSIDIV1_CLK, freq+11);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("HSIDIV1_CLK Read Freq Successful \r\n");
        printf("HSIDIV1_CLK Freq : %d \r\n", freq[11]);
    }
    else
    {
        printf("HSIDIV1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(HSIDIV2_CLK, freq+12);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("HSIDIV2_CLK Read Freq Successful \r\n");
        printf("HSIDIV2_CLK Freq : %d \r\n", freq[12]);
    }
    else
    {
        printf("HSIDIV2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SYSOSCDIV1_CLK, freq+13);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSOSCDIV1_CLK Read Freq Successful \r\n");
        printf("SYSOSCDIV1_CLK Freq : %d \r\n", freq[13]);
    }
    else
    {
        printf("SYSOSCDIV1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SYSOSCDIV2_CLK, freq+14);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSOSCDIV2_CLK Read Freq Successful \r\n");
        printf("SYSOSCDIV2_CLK Freq : %d \r\n", freq[14]);
    }
    else
    {
        printf("SYSOSCDIV2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SYSPLLDIV1_CLK, freq+15);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSPLLDIV1_CLK Read Freq Successful \r\n");
        printf("SYSPLLDIV1_CLK Freq : %d \r\n", freq[15]);
    }
    else
    {
        printf("SYSPLLDIV1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SYSPLLDIV2_CLK, freq+16);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SYSPLLDIV2_CLK Read Freq Successful \r\n");
        printf("SYSPLLDIV2_CLK Freq : %d \r\n", freq[16]);
    }
    else
    {
        printf("SYSPLLDIV2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_CFGTMR0_CLK, freq+17);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_CFGTMR0_CLK Read Freq Successful \r\n");
        printf("SIM_CFGTMR0_CLK Freq : %d \r\n", freq[17]);
    }
    else
    {
        printf("SIM_CFGTMR0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_CFGTMR1_CLK, freq+18);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_CFGTMR1_CLK Read Freq Successful \r\n");
        printf("SIM_CFGTMR1_CLK Freq : %d \r\n", freq[18]);
    }
    else
    {
        printf("SIM_CFGTMR1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_CFGTMR2_CLK, freq+19);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_CFGTMR2_CLK Read Freq Successful \r\n");
        printf("SIM_CFGTMR2_CLK Freq : %d \r\n", freq[19]);
    }
    else
    {
        printf("SIM_CFGTMR2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_CFGTMR3_CLK, freq+20);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_CFGTMR3_CLK Read Freq Successful \r\n");
        printf("SIM_CFGTMR3_CLK Freq : %d \r\n", freq[20]);
    }
    else
    {
        printf("SIM_CFGTMR3_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_CLKOUT_CLK, freq+21);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_CLKOUT_CLK Read Freq Successful \r\n");
        printf("SIM_CLKOUT_CLK Freq : %d \r\n", freq[21]);
    }
    else
    {
        printf("SIM_CLKOUT_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_RTCCLK_CLK, freq+22);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_RTCCLK_CLK Read Freq Successful \r\n");
        printf("SIM_RTCCLK_CLK Freq : %d \r\n", freq[22]);
    }
    else
    {
        printf("SIM_RTCCLK_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_LPO_CLK, freq+23);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_LPO_CLK Read Freq Successful \r\n");
        printf("SIM_LPO_CLK Freq : %d \r\n", freq[23]);
    }
    else
    {
        printf("SIM_LPO_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_LPO_1K_CLK, freq+24);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_LPO_1K_CLK Read Freq Successful \r\n");
        printf("SIM_LPO_1K_CLK Freq : %d \r\n", freq[24]);
    }
    else
    {
        printf("SIM_LPO_1K_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_LPO_32K_CLK, freq+25);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_LPO_32K_CLK Read Freq Successful \r\n");
        printf("SIM_LPO_32K_CLK Freq : %d \r\n", freq[25]);
    }
    else
    {
        printf("SIM_LPO_32K_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_LPO_128K_CLK, freq+26);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_LPO_128K_CLK Read Freq Successful \r\n");
        printf("SIM_LPO_128K_CLK Freq : %d \r\n", freq[26]);
    }
    else
    {
        printf("SIM_LPO_128K_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_EINJ_CLK, freq+27);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_EINJ_CLK Read Freq Successful \r\n");
        printf("SIM_EINJ_CLK Freq : %d \r\n", freq[27]);
    }
    else
    {
        printf("SIM_EINJ_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_EREP_CLK, freq+28);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_EREP_CLK Read Freq Successful \r\n");
        printf("SIM_EREP_CLK Freq : %d \r\n", freq[28]);
    }
    else
    {
        printf("SIM_EREP_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_DMA_CLK, freq+29);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_DMA_CLK Read Freq Successful \r\n");
        printf("SIM_DMA_CLK Freq : %d \r\n", freq[29]);
    }
    else
    {
        printf("SIM_DMA_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_MPU_CLK, freq+30);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_MPU_CLK Read Freq Successful \r\n");
        printf("SIM_MPU_CLK Freq : %d \r\n", freq[30]);
    }
    else
    {
        printf("SIM_MPU_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(SIM_MSC_CLK, freq+31);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("SIM_MSC_CLK Read Freq Successful \r\n");
        printf("SIM_MSC_CLK Freq : %d \r\n", freq[31]);
    }
    else
    {
        printf("SIM_MSC_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(COMP_CLK, freq+32);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("COMP_CLK Read Freq Successful \r\n");
        printf("COMP_CLK Freq : %d \r\n", freq[32]);
    }
    else
    {
        printf("COMP_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CRC_CLK, freq+33);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CRC_CLK Read Freq Successful \r\n");
        printf("CRC_CLK Freq : %d \r\n", freq[33]);
    }
    else
    {
        printf("CRC_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(DMACHM_CLK, freq+34);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("DMACHM_CLK Read Freq Successful \r\n");
        printf("DMACHM_CLK Freq : %d \r\n", freq[34]);
    }
    else
    {
        printf("DMACHM_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(EWDT_CLK, freq+35);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("EWDT_CLK Read Freq Successful \r\n");
        printf("EWDT_CLK Freq : %d \r\n", freq[35]);
    }
    else
    {
        printf("EWDT_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PMA_CLK, freq+36);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PMA_CLK Read Freq Successful \r\n");
        printf("PMA_CLK Freq : %d \r\n", freq[36]);
    }
    else
    {
        printf("PMA_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PMB_CLK, freq+37);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PMB_CLK Read Freq Successful \r\n");
        printf("PMB_CLK Freq : %d \r\n", freq[37]);
    }
    else
    {
        printf("PMB_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PMC_CLK, freq+38);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PMC_CLK Read Freq Successful \r\n");
        printf("PMC_CLK Freq : %d \r\n", freq[38]);
    }
    else
    {
        printf("PMC_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PMD_CLK, freq+39);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PMD_CLK Read Freq Successful \r\n");
        printf("PMD_CLK Freq : %d \r\n", freq[39]);
    }
    else
    {
        printf("PMD_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PME_CLK, freq+40);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PME_CLK Read Freq Successful \r\n");
        printf("PME_CLK Freq : %d \r\n", freq[40]);
    }
    else
    {
        printf("PME_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(RTC_CLK, freq+41);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("RTC_CLK Read Freq Successful \r\n");
        printf("RTC_CLK Freq : %d \r\n", freq[41]);
    }
    else
    {
        printf("RTC_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CAN0_CLK, freq+42);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CAN0_CLK Read Freq Successful \r\n");
        printf("CAN0_CLK Freq : %d \r\n", freq[42]);
    }
    else
    {
        printf("CAN0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CAN1_CLK, freq+43);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CAN1_CLK Read Freq Successful \r\n");
        printf("CAN1_CLK Freq : %d \r\n", freq[43]);
    }
    else
    {
        printf("CAN0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CAN2_CLK, freq+44);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CAN2_CLK Read Freq Successful \r\n");
        printf("CAN2_CLK Freq : %d \r\n", freq[44]);
    }
    else
    {
        printf("CAN2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PDU0_CLK, freq+45);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PDU0_CLK Read Freq Successful \r\n");
        printf("PDU0_CLK Freq : %d \r\n", freq[45]);
    }
    else
    {
        printf("PDU0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(PDU1_CLK, freq+46);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("PDU1_CLK Read Freq Successful \r\n");
        printf("PDU1_CLK Freq : %d \r\n", freq[46]);
    }
    else
    {
        printf("PDU1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(FLASH_CLK, freq+47);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("FLASH_CLK Read Freq Successful \r\n");
        printf("FLASH_CLK Freq : %d \r\n", freq[47]);
    }
    else
    {
        printf("FLASH_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CFGTMR0_CLK, freq+48);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CFGTMR0_CLK Read Freq Successful \r\n");
        printf("CFGTMR0_CLK Freq : %d \r\n", freq[48]);
    }
    else
    {
        printf("CFGTMR0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CFGTMR1_CLK, freq+49);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CFGTMR1_CLK Read Freq Successful \r\n");
        printf("CFGTMR1_CLK Freq : %d \r\n", freq[49]);
    }
    else
    {
        printf("CFGTMR1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CFGTMR2_CLK, freq+50);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CFGTMR2_CLK Read Freq Successful \r\n");
        printf("CFGTMR2_CLK Freq : %d \r\n", freq[50]);
    }
    else
    {
        printf("CFGTMR2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CFGTMR3_CLK, freq+51);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CFGTMR3_CLK Read Freq Successful \r\n");
        printf("CFGTMR3_CLK Freq : %d \r\n", freq[51]);
    }
    else
    {
        printf("CFGTMR3_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(ADC0_CLK, freq+52);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("ADC0_CLK Read Freq Successful \r\n");
        printf("ADC0_CLK Freq : %d \r\n", freq[52]);
    }
    else
    {
        printf("ADC0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(ADC1_CLK, freq+53);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("ADC1_CLK Read Freq Successful \r\n");
        printf("ADC1_CLK Freq : %d \r\n", freq[53]);
    }
    else
    {
        printf("ADC1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(CFGIO_CLK, freq+54);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("CFGIO_CLK Read Freq Successful \r\n");
        printf("CFGIO_CLK Freq : %d \r\n", freq[54]);
    }
    else
    {
        printf("CFGIO_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPI2C_CLK, freq+55);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPI2C_CLK Read Freq Successful \r\n");
        printf("LPI2C_CLK Freq : %d \r\n", freq[55]);
    }
    else
    {
        printf("LPI2C_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPITMR_CLK, freq+56);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPITMR_CLK Read Freq Successful \r\n");
        printf("LPITMR_CLK Freq : %d \r\n", freq[56]);
    }
    else
    {
        printf("LPITMR_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPSPI0_CLK, freq+57);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPSPI0_CLK Read Freq Successful \r\n");
        printf("LPSPI0_CLK Freq : %d \r\n", freq[57]);
    }
    else
    {
        printf("LPSPI0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPSPI1_CLK, freq+58);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPSPI1_CLK Read Freq Successful \r\n");
        printf("LPSPI1_CLK Freq : %d \r\n", freq[58]);
    }
    else
    {
        printf("LPSPI1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPSPI2_CLK, freq+59);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPSPI2_CLK Read Freq Successful \r\n");
        printf("LPSPI2_CLK Freq : %d \r\n", freq[59]);
    }
    else
    {
        printf("LPSPI2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPTMR_CLK, freq+60);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPTMR_CLK Read Freq Successful \r\n");
        printf("LPTMR_CLK Freq : %d \r\n", freq[60]);
    }
    else
    {
        printf("LPTMR_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPUART0_CLK, freq+61);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPUART0_CLK Read Freq Successful \r\n");
        printf("LPUART0_CLK Freq : %d \r\n", freq[61]);
    }
    else
    {
        printf("LPUART0_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPUART1_CLK, freq+62);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPUART1_CLK Read Freq Successful \r\n");
        printf("LPUART1_CLK Freq : %d \r\n", freq[62]);
    }
    else
    {
        printf("LPUART1_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

    returnStatus = CLOCK_SYS_ReadFreq(LPUART2_CLK, freq+63);
    if (returnStatus == STATUS_SUCCESS)
    {
        printf("LPUART2_CLK Read Freq Successful \r\n");
        printf("LPUART2_CLK Freq : %d \r\n", freq[63]);
    }
    else
    {
        printf("LPUART2_CLK Read Freq Failed \r\n");
        printf("STATUS_CODE : %03X",returnStatus);
    }

}

/**@} end of group CLOCK_AllClocksArrayConfiguration_Functions */
/**@} end of group CLOCK_AllClocksArrayConfiguration */
/**@} end of group Examples */
