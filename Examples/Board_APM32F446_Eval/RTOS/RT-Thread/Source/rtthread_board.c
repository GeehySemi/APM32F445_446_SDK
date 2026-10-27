/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "Board.h"
#include "apm32f445_446_pins.h"
#include "apm32f445_446_clock.h"
#include "apm32f445_446_lpuart.h"
#include "user_clock_config.h"

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

// Holds the system core clock, which is the system clock
// frequency supplied to the SysTick timer and the processor
// core clock.
extern uint32_t SystemCoreClock;

static rt_uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }

    _SYSTICK_LOAD = ticks - 1;
    _SYSTICK_PRI = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;

    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (10 * 1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void* rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void* rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void rt_os_tick_callback(void)
{
    rt_interrupt_enter();

    rt_tick_increase();

    rt_interrupt_leave();
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
//#error "TODO 1: OS Tick Configuration."
    /*
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND.
     */
    CLOCK_SYS_Init(&g_clockConfig);
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
    rt_os_tick_callback();
}

int uart_init(void)
{
    const MODULE_CLK_CONFIG_T uartClkCfg =
    {
        .gating = true,
        .src = LSICLK_CLK,
        .mul = 1U,
        .div = 1U
    };

    /* Enable PMC clock */
    CLOCK_SYS_ConfigModuleClock(PMC_CLK, NULL);

    /* Enable LPUART1 clock */
    CLOCK_SYS_ConfigModuleClock(LPUART1_CLK, &uartClkCfg);

    /* GPIO config */
    PINS_SetMuxModeSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_MUX_ALT2);
    PINS_SetMuxModeSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_MUX_ALT2);
    PINS_SetPinIntSel(COM_TX_PORT_BASE, COM_TX_PIN, PM_DMA_INT_DISABLED);
    PINS_SetPinIntSel(COM_RX_PORT_BASE, COM_RX_PIN, PM_DMA_INT_DISABLED);

    /* Initialize the LPUART instance */
    LPUART_HW_Init(LPUART1);

    /* USARTx configuration */
    /* USARTx configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    LPUART_SetBaudrate(LPUART_INSTANCE_1, 115200);
    LPUART_HW_SetDataBits(LPUART1, LPUART_8_BITS_DATA, false);
    LPUART_HW_SetStopBitCnt(LPUART1, LPUART_ONE_STOP_BIT);
    LPUART_HW_SetParityMode(LPUART1, LPUART_PARITY_DISABLED);

    LPUART_HW_EnableTx(LPUART1);
    LPUART_HW_EnableRx(LPUART1);

    return 0;
}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char* str)
{
    rt_enter_critical();

    while (*str != '\0')
    {
        if (*str == '\n')
        {
            while(LPUART_HW_GetStatusFlag(LPUART1, LPUART_TX_DATA_REG_EMPTY) != true);
            LPUART_HW_TxChar(LPUART1, '\r');
        }

        while(LPUART_HW_GetStatusFlag(LPUART1, LPUART_TX_DATA_REG_EMPTY) != true);
        LPUART_HW_TxChar(LPUART1, *str++);
    }

    rt_exit_critical();
}

char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (LPUART_HW_GetStatusFlag(LPUART1, LPUART_RX_DATA_REG_FULL) != false)
    {
        LPUART_HW_RxChar(LPUART1, (uint8_t *)&ch);
        LPUART_HW_ClearStatusFlag(LPUART1, LPUART_RX_DATA_REG_FULL);
    }
    else
    {
        if (LPUART_HW_GetStatusFlag(LPUART1, LPUART_RX_OVERRUN) != false)
        {
            LPUART_HW_ClearStatusFlag(LPUART1, LPUART_RX_OVERRUN);
        }

        rt_thread_mdelay(10);
    }

    return ch;
}
