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

/** @addtogroup CFGIO_SPI_Dual
  @{
  */

/** @defgroup CFGIO_SPI_Dual_Macros Macros
  @{
  */
/* SPI time out */
#define SPITIMEOUT  1000U

/* SPI master/slave selection: 1->master ; 0->salve */
#define CFGIOSPI_MASTER  0U
#if CFGIOSPI_MASTER
    #define SPI_MASTER  /* Configure as master */
#else
    #define SPI_SLAVE   /* Configure as slave */
#endif

/* SPI rx/tx buffer length */
#define BUF_NUM    10U
/* Declare transfer size */
#define TRANSFER_SIZE 16u

/**@} end of group CFGIO_SPI_Dual_Macros*/
/** @defgroup CFGIO_SPI_Dual_Variables Variables
  @{
  */
uint8_t g_spiTxBuf[BUF_NUM] = {0,1,2,3,4,5,6,7,8,9};
uint8_t g_spiRxBuf[BUF_NUM] = {0,0,0,0,0,0,0,0,0,0};

/**@} end of group CFGIO_SPI_Dual_Variables*/
/** @defgroup CFGIO_SPI_Dual_Functions Functions
  @{
  */

/*!
 * @brief   Spi pins init
 */
void SpiPinsInit()
{
    PINS_Init(NUM_OF_CONFIGURED_PINS0,g_CFGIOSpiPinsConfig);
}

/*!
 * @brief   Main function
 */
int main(void)
{
    uint8_t i;
    bool compareStatus = true;
    CFGIO_DEVICE_STATE_T cfgioDeviceState;
#if CFGIOSPI_MASTER
    CFGIO_SPI_MASTER_STATE_T cfgio_spi_1MState;
#else
    CFGIO_SPI_SLAVE_STATE_T cfgio_spi_1SState;
#endif

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

#if defined(SPI_MASTER)

    /* initialize SPI to master mode based on the configuration set */
    CFGIO_SPI_MasterInit(CFGIO_SPI_1, &cfgio_spi_1_MasterCfg, &cfgio_spi_1MState);

#elif defined(SPI_SLAVE)

    /* initialize SPI to slave mode based on the configuration set */
    CFGIO_SPI_SlaveInit(CFGIO_SPI_1, &cfgio_spi_1_SlaveCfg, &cfgio_spi_1SState);

#endif
    while (1)
    {
#if defined(SPI_MASTER)
        /* Start master the blocking transfer */
        CFGIO_SPI_MasterTransferBlocking(&cfgio_spi_1MState, g_spiTxBuf, g_spiRxBuf, BUF_NUM, SPITIMEOUT);
        printf("******************************************************************\r\n");
        printf("####master rx data####\r\n");
        for(i = 0; i < BUF_NUM; i++)
        {
            printf("master receiving data %d = %d\r\n",i,g_spiRxBuf[i]);
            if(g_spiRxBuf[i] != g_spiTxBuf[i])
            {
                compareStatus = false;
            }
        }
        printf("******************************************************************\r\n\r\n");
#elif defined(SPI_SLAVE)
        /* Start slave the blocking transfer */
        CFGIO_SPI_SlaveTransferBlocking(&cfgio_spi_1SState, g_spiTxBuf,g_spiRxBuf, BUF_NUM, SPITIMEOUT);
        printf("******************************************************************\r\n");
        printf("####slave rx data####\r\n");
        for(i = 0; i < BUF_NUM; i++)
        {
            printf("slave receiving data %d = %d\r\n",i,g_spiRxBuf[i]);
            if(g_spiRxBuf[i] != g_spiTxBuf[i])
            {
                compareStatus = false;
            }
        }
        printf("******************************************************************\r\n\r\n");
#endif
        for(i = 0; i < BUF_NUM; i++)
        {
            g_spiRxBuf[i] = 0;
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
        compareStatus = true;
    }
}


/**@} end of group CFGIO_SPI_Dual_Functions */
/**@} end of group CFGIO_SPI_Dual */
/**@} end of group Examples */
