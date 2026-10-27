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
#include "board.h"
#include "apm32f445_446_lpi2c.h"
#include "apm32f445_446_cfgio_i2c.h"
#include <stdio.h>

/** @addtogroup APM32F445_Examples
  @{
  */

/** @addtogroup CFGIO_I2C_Master
  @{
  */

/** @defgroup CFGIO_I2C_Master_Macros Macros
  @{
  */

/**
 * 1: I2C master will use DMA for data transfer
 * 0: I2C master will use interrupts for data transfer
 */
#define CFGIO_I2C_MASTER_USE_DMA  1

/* Master and slave buffers */
#define BUFFER_SIZE 8U

/**@} end of group CFGIO_I2C_Master_Macros*/
/** @defgroup CFGIO_I2C_Master_Variables Variables
  @{
  */

uint8_t g_masterBuffer[BUFFER_SIZE];
uint8_t g_slaveBuffer[BUFFER_SIZE];

/* Indicate that a non-blocking transfer is completed */
volatile bool g_transferComplete = false;

/**@} end of group CFGIO_I2C_Master_Variables*/

/** @defgroup CFGIO_I2C_Master_Functions Functions
  @{
  */

void InitMasterBuffer(void);
void InitSlaveBuffer(void);
void CompareBuffer(void);

/*!
 * @brief   Main function
 * @details This example shows the usage of the CFGIO_I2C driver. It uses
 *          the CFGIO_I2C as I2C master, and the LPI2C as I2C slave. The
 *          following connections must be done for this example to work:
 *          I2C master:
 *            - SDA: PTA1  (CFGIO_D3)
 *            - SCL: PTA0  (CFGIO_D2)
 *          I2C slave:
 *            - SDA: PTA2
 *            - SCL: PTA3
 *
 * @param   None
 * @retval  Exit code
 */
int main(void)
{
    STATUS_T result = STATUS_ERROR;
    CFGIO_DEVICE_STATE_T cfgioDeviceState;
    CFGIO_I2C_MASTER_STATE_T i2cMasterState;
    LPI2C_SLAVE_STATE_T i2cSlaveState;
    uint32_t baudrate;
    bool sdaStatus;
    bool sclStatus;

    /* Initialize clock */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr,
                               CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr,
                               CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);

    /* Initialize pins, LEDs and UART1 */
    PINS_Init(NUM_OF_CONFIGURED_PINS0, g_pinsConfig);
    LED_Init();
    COM_Init();
    printf("\r\nStarted CFGIO_I2C!\r\n");

    /* Initialzie LPI2C as slave */
    g_i2cSlaveConfig.callbackParam = (void *)LPI2C_INSTANCE;
    result = LPI2C_SlaveInit(LPI2C_INSTANCE, &g_i2cSlaveConfig, &i2cSlaveState);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to initialize LPI2C as slave!\r\n");
        goto end;
    }
    printf("LPI2C slave initialized!\r\n");

#if CFGIO_I2C_MASTER_USE_DMA
    printf("Using DMA for I2C mster!\r\n");
    /* If DMA shall be used for data transfer */
    DMA_Init(&g_dmaControllerState,
                      &g_dmaConfig,
                      g_dmaChannelStates,
                      g_dmaChannelConfigs,
                      DMA_CHANNEL_COUNT);
    g_i2cMasterConfig.transferType = CFGIO_USE_DMA;
#endif

    /* Initialize CFGIO_I2C as master */
    result = CFGIO_InitDevice(CFGIO_I2C_INSTANCE, &cfgioDeviceState);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to initialize CFGIO device!\r\n");
        goto end;
    }

    result = CFGIO_I2C_MasterInit(CFGIO_I2C_INSTANCE,
                                    &g_i2cMasterConfig,
                                    &i2cMasterState);
    if (result != STATUS_SUCCESS)
    {
        printf("Failed to initialize CFGIO_I2C as master!\r\n");
        goto end;
    }
    printf("CFGIO_I2C master initialized!\r\n");

    /* Set I2C slave address */
    CFGIO_I2C_MasterSetSlaveAddr(&i2cMasterState, I2C_SLAVE_ADDR);

    /* Set I2C baudrate to 100K */
    CFGIO_I2C_MasterSetBaudrate(&i2cMasterState, 100000);
    CFGIO_I2C_MasterGetBaudrate(&i2cMasterState, &baudrate);
    printf("Current baudrate: %d Hz\r\n", baudrate);

    /* Send data to the slave (blocking) */
    InitMasterBuffer();
    result = CFGIO_I2C_MasterWriteBlocking(&i2cMasterState,
                                              g_masterBuffer,
                                              sizeof(g_masterBuffer),
                                              true,
                                              50UL);
    if (result == STATUS_SUCCESS)
    {
        printf("CFGIO_I2C master write data success!\r\n");
        CompareBuffer();
    }
    else
    {
        printf("CFGIO_I2C master write data failed! error=0x%04X\r\n", result);
        goto end;
    }

    /* Send data to the slave (non-blocking) */
    InitMasterBuffer();
    g_transferComplete = false;
    result = CFGIO_I2C_MasterWriteNonBlocking(&i2cMasterState,
                                                 g_masterBuffer,
                                                 sizeof(g_masterBuffer),
                                                 true);
    if (result == STATUS_SUCCESS)
    {
        printf("CFGIO_I2C master write data success!\r\n");

        /* Wait for the data transfer complete callback */
        while (!g_transferComplete);

        CompareBuffer();
    }
    else
    {
        printf("CFGIO_I2C master write data failed! error=0x%04X\r\n", result);
        goto end;
    }

    /* Read data from slave (blocking) */
    InitSlaveBuffer();
    CFGIO_I2C_MasterReadBlocking(&i2cMasterState,
                                     g_masterBuffer,
                                     sizeof(g_masterBuffer),
                                     true,
                                     50UL);
    if (result == STATUS_SUCCESS)
    {
        printf("CFGIO_I2C master read data success!\r\n");
        CompareBuffer();
    }
    else
    {
        printf("CFGIO_I2C master read data failed! error=0x%04X\r\n", result);
        goto end;
    }

    /* Read data from slave (non-blocking) */
    InitSlaveBuffer();
    g_transferComplete = false;
    CFGIO_I2C_MasterReadNonBlocking(&i2cMasterState,
                                      g_masterBuffer,
                                      sizeof(g_masterBuffer),
                                      true);
    if (result == STATUS_SUCCESS)
    {
        printf("CFGIO_I2C master read data success!\r\n");

        /* Wait for the data transer complete callback */
        while (!g_transferComplete);

        CompareBuffer();
    }
    else
    {
        printf("CFGIO_I2C master read data failed! error=0x%04X\r\n", result);
        goto end;
    }

    /* Send data to the slave (non-blocking) and abort it immediately */
    InitMasterBuffer();
    g_transferComplete = false;
    CFGIO_I2C_MasterWriteNonBlocking(&i2cMasterState,
                                       g_masterBuffer,
                                       sizeof(g_masterBuffer),
                                       true);
    CFGIO_I2C_MasterAbortTransfer(&i2cMasterState);
    OSIF_TimeDelay(100);
    if (g_transferComplete)
    {
        printf("Data transfer was done, failed to abort the transfer!\r\n");
    }
    else
    {
        printf("Data transfer aborted successfully!\r\n");
    }

    /* Get SDA line status */
    sdaStatus = CFGIO_I2C_GetBusStatus(&i2cMasterState, CFGIO_I2C_SDA_LINE);
    printf("SDA Status: %s\r\n", sdaStatus ? "High" : "Low");

    /* Get SCL line status */
    sclStatus = CFGIO_I2C_GetBusStatus(&i2cMasterState, CFGIO_I2C_SCL_LINE);
    printf("SCL Status: %s\r\n", sclStatus ? "High" : "Low");

    /**
     * Generate nine clock on SCL line to free SDA line.
     * This function should be called when SDA line be stuck in low.
     */
    result = CFGIO_I2C_GenerateNineClock(&i2cMasterState);
    if (result == STATUS_SUCCESS)
    {
        while (CFGIO_I2C_StatusGenerateNineClock(&i2cMasterState) == STATUS_BUSY);
        printf("Generate 9 clock done!\r\n");
    }

    printf("CFGIO_I2C completed successfully!\r\n");
    CFGIO_I2C_MasterDeinit(&i2cMasterState);

    LED_On(LED_GREEN);
    while (1);

end:
    LED_On(LED_RED);
    while (1);
}

/*!
 * @brief   CFGIO_I2C master callback
 * @details This function will be called by CFGIO_I2C driver when a
 *          non-blocking transfer is completed.
 */
void I2cMasterCallback(I2C_MASTER_EVENT_T masterEvent, void *userData)
{
    (void)userData;

    if (masterEvent == I2C_MASTER_EVENT_TRANSFER_COMPLETE)
    {
        g_transferComplete = true;
    }
}

/*!
 * @brief   LPI2C slave callback
 * @details This function will be called by LPI2C interrupt handler and it will
 *          assign the buffer for TX or RX events.
 */
void I2cSlaveCallback(I2C_SLAVE_EVENT_T slaveEvent, void *userData)
{
    uint32_t instance = (uint32_t)userData;

    switch(slaveEvent)
    {
    case I2C_SLAVE_EVENT_RX_REQUEST:
        /* Master requests data, set the Rx buffer */
        LPI2C_SlaveSetRxBuffer(instance, g_slaveBuffer, BUFFER_SIZE);
        break;
    case I2C_SLAVE_EVENT_TX_REQUEST:
        /* Master sends data, set the Tx buffer */
        LPI2C_SlaveSetTxBuffer(instance, g_slaveBuffer, BUFFER_SIZE);
        break;
    case I2C_SLAVE_EVENT_TX_BUFFER_EMPTY:
    case I2C_SLAVE_EVENT_RX_BUFFER_FULL:
        /* Abort the current transfer if the Tx buffer is empty or the Rx buffer is full */
        LPI2C_SlaveAbortTransfer(instance);
        break;
    case I2C_SLAVE_EVENT_STOP:
        /* Stop condition is on the bus. Nothing to do in the example. */
        break;
    default:
        break;
    }
}

/*!
* @brief    Initialize master and slave buffers for master write sequence
*/
void InitMasterBuffer(void)
{
    for(uint32_t n = 0; n < sizeof(g_masterBuffer); n++)
    {
        g_masterBuffer[n] = (uint8_t)n;
        g_slaveBuffer[n] = 0U;
    }
}

/*!
* @brief    Initialize master and slave buffers for master read sequence
*/
void InitSlaveBuffer(void)
{
    for(uint32_t n = 0; n < sizeof(g_masterBuffer); n++)
    {
        g_masterBuffer[n] = 0U;
        g_slaveBuffer[n] = (uint8_t)(BUFFER_SIZE - n);
    }
}

/*!
* @brief    Compare the master and the slave buffer
*/
void CompareBuffer(void)
{
    bool result = true;

    for (uint32_t i = 0; i < BUFFER_SIZE; i++)
    {
        if (g_masterBuffer[i] != g_slaveBuffer[i])
        {
            result = false;
            break;
        }
    }

    if (result)
    {
        printf("OK: Master buffer and slave buffer are identical!\r\n");
    }
    else
    {
        printf("Error: Master buffer and slave buffer are different!\r\n");
    }
}

/**@} end of group CFGIO_I2C_Master_Functions */
/**@} end of group CFGIO_I2C_Master */
/**@} end of group Examples */
