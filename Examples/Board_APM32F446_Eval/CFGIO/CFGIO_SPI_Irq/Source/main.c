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
#include "apm32f445_446_cfgio_spi.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_dma.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup CFGIO_SPI_Irq
  @{
  */

/** @defgroup CFGIO_SPI_Irq_Macros Macros
  @{
  */

/* SPI time out */
#define SPITIMEOUT  1000U

/* SPI rx/tx buffer length */
#define BUF_NUM    10U

/**@} end of group CFGIO_SPI_Irq_Macros*/
/** @defgroup CFGIO_SPI_Irq_Variables Variables
  @{
  */
uint8_t g_SpiMTxBuf[BUF_NUM] = {0,1,2,3,4,5,6,7,8,9};
uint8_t g_SpiMRxBuf[BUF_NUM] = {0,0,0,0,0,0,0,0,0,0};

uint8_t g_SpiSTxBuf[BUF_NUM] = {9,8,7,6,5,4,3,2,1,0};
uint8_t g_SpiSRxBuf[BUF_NUM] = {0,0,0,0,0,0,0,0,0,0};

/**@} end of group CFGIO_SPI_Irq_Variables*/
/** @defgroup CFGIO_SPI_Irq_Functions Functions
  @{
  */
/*!
 * @brief   Spi pins init
 *
 * @param   None
 *
 * @retval  None
 */
void SpiPinsInit()
{
    PINS_Init(NUM_OF_CONFIGURED_PINS0,g_CFGIOSpiPinsConfig);
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
    uint8_t i;
    uint32_t setBaudVal = 100000U;
    uint32_t actualBaudVal;
    bool compareStatus = true;
    CFGIO_DEVICE_STATE_T cfgioDeviceState;
    CFGIO_SPI_MASTER_STATE_T cfgioSpi1MState;
    CFGIO_SPI_SLAVE_STATE_T cfgioSpi1SState;
    CFGIO_SPI_MASTER_CFG_T cfgioSpiTempMCfg;
    CFGIO_SPI_SLAVE_CFG_T cfgioSpiTempSCfg;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Led init */
    LED_Init();

    /* uart init */
    COM_Init();

    /* spi pins init */
    SpiPinsInit();

    /* CFGIO initialization */
    CFGIO_InitDevice(CFGIO_SPI_1,&cfgioDeviceState);

    /* Call default configuration */
    CFGIO_SPI_MasterDefaultConfig(&cfgioSpiTempMCfg);

    /* Call default configuration */
    CFGIO_SPI_SlaveDefaultConfig(&cfgioSpiTempSCfg);
    cfgioSpiTempSCfg.ssPin = 7U;
    cfgioSpiTempSCfg.sckPin = 6U;
    cfgioSpiTempSCfg.misoPin = 5U;
    cfgioSpiTempSCfg.mosiPin = 4U;

    /* initialize SPI to master mode based on the configuration set */
    CFGIO_SPI_MasterInit(CFGIO_SPI_1,&cfgioSpiTempMCfg,&cfgioSpi1MState);

    /* Set master baud rate */
    CFGIO_SPI_MasterSetBaudrate(&cfgioSpi1MState,setBaudVal);

    /* Obtain baud rate */
    CFGIO_SPI_MasterReadBaudrate(&cfgioSpi1MState,&actualBaudVal);

    if(setBaudVal == actualBaudVal)
    {
        printf("Baud rate is OK!\r\n");
    }
    else
    {
        printf("Baud rate is NG!\r\n");
    }

    /* initialize SPI to slave mode based on the configuration set */
    CFGIO_SPI_SlaveInit(CFGIO_SPI_1, &cfgioSpiTempSCfg, &cfgioSpi1SState);

    /* Start slave the blocking transfer */
    CFGIO_SPI_SlaveTransferNonBlocking(&cfgioSpi1SState, g_SpiSTxBuf, g_SpiSRxBuf, BUF_NUM);

    /* Start master the blocking transfer */
    CFGIO_SPI_MasterTransferBlocking(&cfgioSpi1MState, g_SpiMTxBuf, g_SpiMRxBuf, BUF_NUM, SPITIMEOUT);

    for(i = 0; i < BUF_NUM; i++)
    {
        if((g_SpiMTxBuf[i] == g_SpiSRxBuf[i]) && (g_SpiSTxBuf[i] == g_SpiMRxBuf[i]))
        {
            printf("master receiving data %d = %d     slave receiving data %d = %d    data status is OK!\r\n",i,g_SpiMRxBuf[i],i,g_SpiSRxBuf[i]);
        }
        else
        {
            compareStatus = false;
            printf("master receiving data %d = %d     slave receiving data %d = %d    data status is NG!\r\n",i,g_SpiMRxBuf[i],i,g_SpiSRxBuf[i]);
        }
    }
    if(compareStatus == true)
    {
        /* Communication successfully opened green LED */
        LED_On(LED_GREEN);
        LED_Off(LED_RED);
    }
    else
    {
        LED_On(LED_RED);
        LED_Off(LED_GREEN);
    }
    /* Call de init function */
    CFGIO_SPI_MasterDeinit(&cfgioSpi1MState);

    CFGIO_SPI_SlaveDeinit(&cfgioSpi1SState);
}

/**@} end of group CFGIO_SPI_Irq_Functions */
/**@} end of group CFGIO_SPI_Irq */
/**@} end of group Examples */
