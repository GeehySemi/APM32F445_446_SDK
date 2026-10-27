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
#include "user_lpspi_config.h"
#include "apm32f445_446_lpspi.h"
#include "apm32f445_446_dma.h"
#include "board.h"
#include <stdio.h>

/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup SPI_Isr
  @{
  */

/** @defgroup SPI_Isr_Macros Macros
  @{
  */

/* SPI master/slave selection: 1->master ; 0->salve */
#define LPSPI_MASTER  0U
#if LPSPI_MASTER
    #define SPI_MASTER  /* Configure as master */
#else
    #define SPI_SLAVE   /* Configure as slave */
#endif

/* SPI time out */
#define SPI_TIMEOUT     1000U

/* SPI rx/tx buffer length */
#define BUF_LEN         10U

/**@} end of group SPI_Isr_Macros*/
/** @defgroup SPI_Isr_Variables Variables
  @{
  */
uint8_t g_spiTxBuf[BUF_LEN] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint8_t g_spiRxBuf[BUF_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**@} end of group SPI_Isr_Variables*/

/** @defgroup SSPI_Isr_Functions Functions
  @{
  */

/*!
 * @brief   Main function
 */
int main(void)
{
    uint8_t i;
    bool compareStatus = true;
    uint32_t bytesRemained;
    STATUS_T status;

    /* Initialize clock */
    CLOCK_SYS_Init(&g_clockConfig);

    /* Led init */
    LED_Init();

    /* uart init */
    COM_Init();

    /* spi pins init */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_spiPinsConfig);

#if defined(SPI_MASTER)
    /* initialize SPI to master mode based on the configuration set */
    LPSPI_MasterInit(LPSPI_1, &g_spiState, &g_spiMasterCfg);

    /* Configure delay between transfer */
    LPSPI_MasterConfigDelay(LPSPI_1, 1, 1, 1);

    /* Select the chip to communicate with */
    LPSPI_MasterConfigPcs(LPSPI_1, LPSPI_PER_PCS0, LPSPI_SIGNAL_ACTIVE_HIGH);

#elif defined(SPI_SLAVE)
    /* initialize SPI to slave mode based on the configuration set */
    LPSPI_SlaveInit(LPSPI_1, &g_spiState, &g_spiSlaveCfg);

#endif

    while (1)
    {
#if defined(SPI_MASTER)
        /* Start master the blocking transfer */
        LPSPI_MasterTransferBlocking(LPSPI_1, g_spiTxBuf, g_spiRxBuf, BUF_LEN, SPI_TIMEOUT);

        /* Obtain the status of whether the last transmission was completed */
        status = LPSPI_MasterGetTransferStatus(LPSPI_1, &bytesRemained);
        printf("******************************************************************\r\n");
        printf("####tx status flag####\r\n");
        printf("tx status flag = %d\r\n", status);
        printf("\r\n####master rx data####\r\n");
        for (i = 0; i < BUF_LEN; i++)
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
        LPSPI_SlaveTransferBlocking(LPSPI_1, g_spiTxBuf, g_spiRxBuf, BUF_LEN, SPI_TIMEOUT);

        /* Obtain the status of whether the last transmission was completed */
        status = LPSPI_SlaveGetTransferStatus(LPSPI_1, &bytesRemained);
        printf("******************************************************************\r\n");
        printf("####tx status flag####\r\n");
        printf("tx status flag = %d\r\n",status);
        printf("\r\n####slave rx data####\r\n");
        for (i = 0; i < BUF_LEN; i++)
        {
            printf("slave receiving data %d = %d\r\n", i, g_spiRxBuf[i]);
            if(g_spiRxBuf[i] != g_spiTxBuf[i])
            {
                compareStatus = false;
            }
        }
        printf("******************************************************************\r\n\r\n");
#endif

        for (i = 0; i < BUF_LEN; i++)
        {
            g_spiRxBuf[i] = 0;
        }

        if (compareStatus == true)
        {
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

/**@} end of group SPI_Isr_Functions */
/**@} end of group SPI_Isr */
/**@} end of group Examples */
