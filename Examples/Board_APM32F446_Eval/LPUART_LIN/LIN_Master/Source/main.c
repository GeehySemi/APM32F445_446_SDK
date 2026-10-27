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


/** @addtogroup APM32F446_Examples
  @{
  */

/** @addtogroup LIN_Master
  @{
  */

 /** @defgroup LIN_Master_Variables Variables
  @{
  */

/* (CLK (MHz) * timer period (us) / Prescaler) */
#define TMR_COMPARE_VAL         (uint16_t)(2000U)
#define TMR_TICKS_1US           (uint16_t)(4U)

#define FRAME_SLAVE_RX_DATA     (1U)
#define FRAME_MASTER_RX_DATA    (2U)
#define FRAME_ENTER_SLEEP       (3U)

#define BUFFER_SIZE             (8U)

/* Data buffer */
uint8_t g_rxBuffer[BUFFER_SIZE] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t g_txBuffer1[BUFFER_SIZE] = {0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21};
uint8_t g_txBuffer2[BUFFER_SIZE] = {0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a};

volatile bool g_wakeupFlag = false;
uint16_t g_timerOverflowCount = 0U;

/**@} end of group LIN_Master_Variables*/

/** @defgroup LIN_Master_Functions Functions
  @{
  */

/*!
 * @brief This function is called when the KEY1 button is pressed
 */
void Key1Handler(void)
{
    static uint8_t cnt = 0;
    cnt++;

    /* Send header to lin bus */
    if ((cnt % 2U) == 0U)
    {
        LIN_MasterTxHeader(LIN1_INSTANCE, FRAME_SLAVE_RX_DATA);
    }
    else
    {
        LIN_MasterTxHeader(LIN1_INSTANCE, FRAME_MASTER_RX_DATA);
    }
}

/*!
 * @brief This function is called when the KEY2 button is pressed
 */
void Key2Handler(void)
{
    if (LIN_NODE_SLP_MODE == LIN_GetNodeState(LIN1_INSTANCE))
    {
        /* Send wakeup signal */
        LIN_SendWakeupSignal(LIN1_INSTANCE);

        /* Set wakeup signal flag */
        g_wakeupFlag = true;
    }
    else
    {
        /* Send header to set the node to sleep mode */
        LIN_MasterTxHeader(LIN1_INSTANCE, FRAME_ENTER_SLEEP);
    }
}

/*!
 * @brief   LPTMR Interrupt Service Routine
 * @details The ISR will call LIN timeout service every 500 us
 */
void LPTMR_ISR(void)
{
    /* Timer Interrupt Handler */
    LIN_TimeoutProcess(LIN1_INSTANCE);

    /* Increment overflow count */
    g_timerOverflowCount++;

    /* Clear compare flag */
    LPTMR_ClearCompareFlag(INST_LPTMR1);
}

/*!
 * @brief Callback function to get time interval in nano seconds
 *
 * @param ns Number of nanoseconds passed since the last call of the function
 */
uint32_t LinGetTimeIntervalCallback(uint32_t *ns)
{
    static uint32_t lastCount = 0UL;
    uint32_t value = LPTMR_ReadCounterValueByCount(INST_LPTMR1);

    *ns = ((uint32_t)(value + g_timerOverflowCount * TMR_COMPARE_VAL - lastCount))
           * 1000U / TMR_TICKS_1US;

    g_timerOverflowCount = 0UL;
    lastCount = value;

    return 0UL;
}

LIN_CALLBACK_T LinCallbackHandler(uint32_t instance, LIN_STATE_T *linState)
{
    switch (linState->eventId)
    {
    case LIN_EVENT_PID_OK:
        /* Set timeout */
        LIN_SetTimeoutCounter(LIN1_INSTANCE, TIMEOUT);

        /* If PID is FRAME_SLAVE_RX_DATA, salve node will receive data from master node */
        if (FRAME_SLAVE_RX_DATA == linState->currentId)
        {
            /* Call to Send Frame DATA Function */
            LIN_TxData(LIN1_INSTANCE, g_txBuffer1, sizeof(g_txBuffer1));
        }

        /* If PID is FRAME_MASTER_RX_DATA, master node will receive data */
        if (FRAME_MASTER_RX_DATA == linState->currentId)
        {
            /* Call to Receive Frame DATA Function */
            LIN_RxData(LIN1_INSTANCE, g_rxBuffer, sizeof(g_rxBuffer));
        }

        /* If PID is FRAME_ENTER_SLEEP, salve node will go to sleep mode */
        if (FRAME_ENTER_SLEEP == linState->currentId)
        {
            /* Go to sleep mode */
            LIN_EnterSleepMode(LIN1_INSTANCE);
        }
        break;
    case LIN_EVENT_PID_ERROR:
        /* Go to idle mode */
        LIN_EnterSleepMode(LIN1_INSTANCE);
        break;
    case LIN_EVENT_TX_FINISHED:
    case LIN_EVENT_RX_FINISHED:
        /* Go to idle mode */
        LIN_EnterIdleState(LIN1_INSTANCE);
        break;
    case LIN_EVENT_CHKSUM_ERROR:
    case LIN_EVENT_READBACK_ERROR:
    case LIN_EVENT_FRAME_ERROR:
    case LIN_EVENT_RX_BRK_FIELD_OK:
        /* Set timeout */
        LIN_SetTimeoutCounter(LIN1_INSTANCE, TIMEOUT);
        break;
    case LIN_EVENT_WAKEUP_SIGNAL:
        /* Set wakeup signal flag */
        g_wakeupFlag = true;
        break;
    case LIN_EVENT_SYNC_ERROR:
    case LIN_EVENT_BAUDRATE_ADJUSTED:
    case LIN_NO_EVENT:
    case LIN_EVENT_SYNC_OK:
    default:
        /* do nothing */
        break;
    }
    return linState->callback;
}

/*!
 * @brief   LIN master task
 */
void LinMasterTask(void)
{
    STATUS_T status = STATUS_ERROR;
    bool rxStatus = false;
    uint8_t remainBytes = 1U;
    uint8_t i;

    /* Initialize LIN interface */
    LIN_Init(LIN1_INSTANCE, &g_lin1UserConfig, &g_lin1State);

    /* Install callback function */
    LIN_InstallCallback(LIN1_INSTANCE, (LIN_CALLBACK_T)LinCallbackHandler);

    /* Infinite loop */
    for (;;)
    {
        status = LIN_GetRxStatus(LIN1_INSTANCE, &remainBytes);
        if (  (status == STATUS_SUCCESS)
           && (remainBytes == 0U)
           && (g_lin1State.currentId == FRAME_MASTER_RX_DATA))
        {
            /* Check received data */
            rxStatus = true;
            for (i = 0; i < BUFFER_SIZE; i++)
            {
                if (g_rxBuffer[i] != g_txBuffer2[i])
                {
                    rxStatus = false;
                    break;
                }
            }

            if (rxStatus)
            {
                /* Toggle green LED if received data is correct */
                LED_Off(LED_RED);
                LED_Off(LED_BLUE);
                LED_Toggle(LED_GREEN);
            }
            else
            {
                /* Turn on red LED if received data is not correct */
                LED_Off(LED_GREEN);
                LED_Off(LED_BLUE);
                LED_On(LED_RED);
            }
        }

        /* Turn on blue LED if woke-up */
        if (g_wakeupFlag)
        {
            /* Clear wakeup signal */
            g_wakeupFlag = false;

            LED_Off(LED_GREEN);
            LED_Off(LED_RED);
            LED_On(LED_BLUE);
        }

        /* Turn off all LEDs in sleep mode */
        if (LIN_NODE_SLP_MODE == LIN_GetNodeState(LIN1_INSTANCE))
        {
            LED_Off(LED_RED);
            LED_Off(LED_BLUE);
            LED_Off(LED_GREEN);
        }
    }
}

/*!
 * @brief The main function for the project
*/
int main(void)
{
    bool exit = false;

    /* Initialize clocks */
    CLOCK_SYS_ClockManagerInit(g_clockConfigsArr,
                               CLOCK_CONFIG_CNT,
                               g_clockCallbacksArr,
                               CLOCK_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    /* Init pins */
    PINS_Init(NUM_OF_CONFIGURED_PINS, g_pinsConfig);

    /* Initialize LEDs */
    LED_Init();

    /* Initialize buttons */
    BTN_Init();
    BTN_InstallKey1Handler(Key1Handler);
    BTN_InstallKey2Handler(Key2Handler);

    LIN_EnableTransceiver();

    /* Initialize LPTMR */
    LPTMR_Init(INST_LPTMR1, &g_lptmrConfig, false);
    INT_SYS_InstallHandler(LPTMR_IRQn, LPTMR_ISR, (ISR_T *)NULL);
    INT_SYS_EnableIRQ(LPTMR_IRQn);
    LPTMR_StartCounter(INST_LPTMR1);

    /* Start LIN master task */
    LinMasterTask();

    while (!exit);
}

/**@} end of group LIN_Master_Functions */
/**@} end of group LIN_Master */
/**@} end of group Examples */
