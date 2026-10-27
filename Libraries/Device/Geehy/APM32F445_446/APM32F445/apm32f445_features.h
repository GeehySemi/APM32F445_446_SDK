/*!
 * @file        apm32f445_features.h
 *
 * @brief       Features for APM32F445 device
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

#ifndef APM32F445_FEATURES_H
#define APM32F445_FEATURES_H

/** @addtogroup FEATURES
  @{
*/

/** @addtogroup APM32F445
  @{
*/

/** @defgroup Features_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief   Error codes
 * @details Error codes will be a unified enumeration which contains all error
 *          codes (common and specific). There will be separate spaces, each of
 *          256 positions, allocated for each functionality.
 */
typedef enum
{
    /* Generic error codes */
    STATUS_SUCCESS                          = 0x0000U,  /* Generic success status */
    STATUS_ERROR                            = 0x0001U,  /* Generic failure status */
    STATUS_BUSY                             = 0x0002U,  /* Generic busy status */
    STATUS_TIMEOUT                          = 0x0003U,  /* Generic timeout status */
    STATUS_UNSUPPORTED                      = 0x0004U,  /* Generic unsupported status */

    /* MCU error codes */
    STATUS_GATE_OFF                         = 0x0100U,   /* Module is gated off */
    STATUS_TRANSITION_FAILED                = 0x0101U,  /* Error occurs during transition */
    STATUS_STATE_INVALID                    = 0x0102U,  /* Unsupported in current state */
    STATUS_NOTIFY_BEFORE_ERROR              = 0x0103U,  /* Error occurs during send "BEFORE" notification */
    STATUS_NOTIFY_AFTER_ERROR               = 0x0104U,  /* Error occurs during send "AFTER" notification */

    /* CAN error codes */
    STATUS_CAN_NO_TRANSFER_IN_PROGRESS      = 0x0200U,  /* There is no transmission or reception in progress */
    STATUS_CAN_MB_OUT_OF_RANGE              = 0x0201U,  /* The specified MB index is out of the configurable range */

    /* I2C error codes */
    STATUS_I2C_NACK_ERROR                   = 0x0300U,  /* NACK signal received */
    STATUS_I2C_RX_OVERRUN_ERROR             = 0x0302U,  /* RX overrun error */
    STATUS_I2C_TX_UNDERRUN_ERROR            = 0x0301U,  /* TX underrun error */
    STATUS_I2C_ABORTED                      = 0x0304U,  /* A transfer was aborted */
    STATUS_I2C_ARBITRATION_LOST_ERROR       = 0x0303U,  /* Arbitration lost */
    STATUS_I2C_BUS_BUSY                     = 0x0305U,  /* I2C bus is busy, cannot start transfer */

    /* SPI error codes */
    STATUS_SPI_ABORTED                      = 0x0600U,  /* A transfer was aborted */
    STATUS_SPI_RX_OVERRUN                   = 0x0601U,  /* RX overrun error */
    STATUS_SPI_TX_UNDERRUN                  = 0x0602U,  /* TX underrun error */

    /* UART error codes */
    STATUS_UART_ABORTED                     = 0x0500U,  /* A transfer was aborted */
    STATUS_UART_RX_OVERRUN                  = 0x0501U,  /* RX overrun error */
    STATUS_UART_TX_UNDERRUN                 = 0x0502U,  /* TX underrun error */
    STATUS_UART_NOISE_ERROR                 = 0x0503U,  /* Noise error */
    STATUS_UART_PARITY_ERROR                = 0x0504U,  /* Parity error */
    STATUS_UART_FRAMING_ERROR               = 0x0505U,  /* Framing error */

    /* CSEC error codes */
    STATUS_CSEC_INVALID_COMMAND             = 0x0600U,  /* Command value out of range */
    STATUS_CSEC_INTERNAL_CMD_ERROR          = 0x0601U,  /* An internal command processor error while executing a command */
    STATUS_CSEC_MAC_LENGTH_ERROR            = 0x0602U,  /* MAC/Message length out of range */
    STATUS_CSEC_SEQUENCE_ERROR              = 0x0603U,  /* The sequence of commands or subcommands is out of sequence */
    STATUS_CSEC_NO_SECURE_BOOT              = 0x0604U,  /* The conditions for a secure boot process are not met */
    STATUS_CSEC_KEY_INVALID                 = 0x0605U,  /* A function is called to perform an operation with a key that is not allowed for the given operation */
    STATUS_CSEC_KEY_NOT_AVAILABLE           = 0x0606U,  /* A key is locked due to failed boot measurement or an active debugger */
    STATUS_CSEC_KEY_UPDATE_ERROR            = 0x0607U,  /* Key update did not succeed due to errors in verification of the messages */
    STATUS_CSEC_KEY_EMPTY                   = 0x0608U,  /* Attempt to use a key that has not been initialized yet */
    STATUS_CSEC_KEY_WRITE_PROTECTED         = 0x0609U,  /* Request for updating a write protected key slot, or activating debugger with write protected key(s) */
    STATUS_CSEC_HSM_FLASH_BLOCK_ERROR       = 0x060AU,  /* Error reading, programming or erasing one of the HSM flash blocks */
    STATUS_CSEC_HSM_INTERNAL_MEMORY_ERROR   = 0x060BU,  /* An internal memory error encountered while executing the command */
    STATUS_CSEC_MEMORY_FAILURE              = 0x060CU,  /* General memory technology failure (multi-bit ECC error, common fault detected) */
    STATUS_CSEC_TRNG_CLOCK_ERROR            = 0x060DU,  /* TRNG not provided with a stable clock */
    STATUS_CSEC_TRNG_ERROR                  = 0x060EU,  /* One or more statistical tests run on the TRNG output failed */
    STATUS_CSEC_RNG_SEED                    = 0x060FU,  /* Returned by CMD_RND and CMD_DEBUG if the seed has not been initialized before */
    STATUS_CSEC_INVALID_ARG                 = 0x0610U,  /* Invalid command argument */
    STATUS_CSEC_NO_DEBUGGING                = 0x0611U,  /* DEBUG command authentication failed */
} STATUS_T;

/*!
 * @brief Clock names
 */
typedef enum {
    /* Main clocks */
    CORE_CLK                        = 0U,   /* Core clock */
    BUS_CLK                         = 1U,   /* Bus clock */
    SLOW_CLK                        = 2U,   /* Slow clock */
    CLKOUT_CLK                      = 3U,   /* CLKOUT clock */

    /* Other internal clocks used by peripherals */
    LSICLK_CLK                      = 4U,   /* LSICLK clock */
    HSICLK_CLK                      = 5U,   /* HSICLK clock */
    SYSOSC_CLK                        = 6U,   /* SYSOSC clock */
    SYSPLL_CLK                      = 7U,   /* SYSPLL clock */
    RTC_CLKIN_CLK                   = 8U,   /* RTC_CLKIN clock */
    SCG_CLKOUT_CLK                  = 9U,   /* SCG CLK_OUT clock */

    LSIDIV1_CLK                     = 10U,  /* LSIDIV1 clock */
    LSIDIV2_CLK                     = 11U,  /* LSIDIV2 clock */
    HSIDIV1_CLK                     = 12U,  /* HSIDIV1 clock */
    HSIDIV2_CLK                     = 13U,  /* HSIDIV2 clock */
    SYSOSCDIV1_CLK                    = 14U,  /* SYSOSCDIV1 clock */
    SYSOSCDIV2_CLK                    = 15U,  /* SYSOSCDIV2 clock */
    SYSPLLDIV1_CLK                  = 16U,  /* SYSPLLDIV1 clock */
    SYSPLLDIV2_CLK                  = 17U,  /* SYSPLLDIV2 clock */

    SCG_CLOCKS_END                  = 18U,  /* End of SCG clocks */

    /* SIM clocks */
    SIM_CFGTMR0_CLK                   = 21U,  /* CFGTMR0 External Clock */
    SIM_CFGTMR1_CLK                   = 22U,  /* CFGTMR1 External Clock */
    SIM_CFGTMR2_CLK                   = 23U,  /* CFGTMR2 External Clock */
    SIM_CFGTMR3_CLK                   = 24U,  /* CFGTMR3 External Clock */
    SIM_CLKOUT_CLK                  = 25U,  /* CLKOUT clock */
    SIM_RTCCLK_CLK                  = 26U,  /* RTCCLK clock */
    SIM_LPO_CLK                     = 27U,  /* LPO clock */
    SIM_LPO_1K_CLK                  = 28U,  /* LPO 1KHz clock */
    SIM_LPO_32K_CLK                 = 29U,  /* LPO 32KHz clock */
    SIM_LPO_128K_CLK                = 30U,  /* LPO 128KHz clock */
    SIM_EINJ_CLK                    = 31U,  /* EINJ clock source */
    SIM_EREP_CLK                    = 32U,  /* EREP clock source */
    SIM_DMA_CLK                     = 33U,  /* DMA clock source */
    SIM_MPU_CLK                     = 34U,  /* MPU clock source */
    SIM_MSC_CLK                     = 35U,  /* MSC clock source */
    SIM_CLOCKS_END                  = 36U,  /* End of SIM clocks */

    /* PCM clocks */
    COMP_CLK                        = 41U,  /* CMP clock source */
    CRC_CLK                         = 42U,  /* CRC clock source */
    DMACHM_CLK                      = 43U,  /* DMACHM clock source */
    EWDT_CLK                        = 44U,  /* EWDT clock source */
    PMA_CLK                         = 45U,  /* PMA clock source */
    PMB_CLK                         = 46U,  /* PMB clock source */
    PMC_CLK                         = 47U,  /* PMC clock source */
    PMD_CLK                         = 48U,  /* PMD clock source */
    PME_CLK                         = 49U,  /* PME clock source */
    RTC_CLK                         = 50U,  /* RTC clock source */
    PCM_END_OF_BUS_CLOCKS           = 51U,  /* End of BUS clocks */
    CAN0_CLK                    = 52U,  /* CAN0 clock source */
    CAN1_CLK                    = 53U,  /* CAN1 clock source */
    CAN2_CLK                    = 54U,  /* CAN2 clock source */
    PDU0_CLK                        = 55U,  /* PDU0 clock source */
    PDU1_CLK                        = 56U,  /* PDU1 clock source */
    PCM_SYS_CLOCKS_END              = 57U,  /* End of SYS clocks */
    FLASH_CLK                       = 58U,  /* FLASH clock source */
    PCM_SLOW_CLOCKS_END             = 59U,  /* End of SLOW clocks */
    CFGTMR0_CLK                       = 60U,  /* CFGTMR0 clock source */
    CFGTMR1_CLK                       = 61U,  /* CFGTMR1 clock source */
    CFGTMR2_CLK                       = 62U,  /* CFGTMR2 clock source */
    CFGTMR3_CLK                       = 63U,  /* CFGTMR3 clock source */
    PCM_ASYNCH_DIV1_CLOCKS_END      = 64U,  /* End of ASYNCH DIV1 clocks */
    ADC0_CLK                        = 65U,  /* ADC0 clock source */
    ADC1_CLK                        = 66U,  /* ADC1 clock source */
    CFGIO_CLK                     = 67U,  /* CFGIO_CLK clock source */
    LPI2C_CLK                       = 68U,  /* LPI2C clock source */
    LPITMR_CLK                      = 69U,  /* LPITMR clock source */
    LPSPI0_CLK                      = 70U,  /* LPSPI0 clock source */
    LPSPI1_CLK                      = 71U,  /* LPSPI1 clock source */
    LPSPI2_CLK                      = 72U,  /* LPSPI2 clock source */
    LPTMR_CLK                       = 73U,  /* LPTMR clock source */
    LPUART0_CLK                     = 74U,  /* LPUART0 clock source */
    LPUART1_CLK                     = 75U,  /* LPUART1 clock source */
    LPUART2_CLK                     = 76U,  /* LPUART2 clock source */
    PCM_ASYNCH_DIV2_CLOCKS_END      = 77U,  /* End of ASYNCH DIV2 clocks */
    PCM_CLOCKS_END                  = 78U,  /* End of PCM clocks */
    CLOCK_NAME_MAX_COUNT            = 79U,  /* The total number of entries */
} CLOCK_NAMES_T;

/*!
 * @brief   DMA hardware request
 * @details The user can configure the hardware request into DMACHM to trigger
 *          the DMA transfer accordingly. The index of the hardware request
 *          varies according to the to SoC.
 */
typedef enum {
    DMA_REQUEST_DISABLED                    = 0U,
    DMA_REQUEST_LPUART0_RX                  = 2U,
    DMA_REQUEST_LPUART0_TX                  = 3U,
    DMA_REQUEST_LPUART1_RX                  = 4U,
    DMA_REQUEST_LPUART1_TX                  = 5U,
    DMA_REQUEST_LPUART2_RX                  = 6U,
    DMA_REQUEST_LPUART2_TX                  = 7U,
    DMA_REQUEST_CFGIO_SHIFTER0            = 10U,
    DMA_REQUEST_CFGIO_SHIFTER1            = 11U,
    DMA_REQUEST_CFGIO_SHIFTER2            = 12U,
    DMA_REQUEST_CFGIO_SHIFTER3            = 13U,
    DMA_REQUEST_LPSPI0_RX                   = 14U,
    DMA_REQUEST_LPSPI0_TX                   = 15U,
    DMA_REQUEST_LPSPI1_RX                   = 16U,
    DMA_REQUEST_LPSPI1_TX                   = 17U,
    DMA_REQUEST_LPSPI2_RX                   = 18U,
    DMA_REQUEST_LPSPI2_TX                   = 19U,
    DMA_REQUEST_CFGTMR1_CHANNEL_0             = 20U,
    DMA_REQUEST_CFGTMR1_CHANNEL_1             = 21U,
    DMA_REQUEST_CFGTMR1_CHANNEL_2             = 22U,
    DMA_REQUEST_CFGTMR1_CHANNEL_3             = 23U,
    DMA_REQUEST_CFGTMR1_CHANNEL_4             = 24U,
    DMA_REQUEST_CFGTMR1_CHANNEL_5             = 25U,
    DMA_REQUEST_CFGTMR1_CHANNEL_6             = 26U,
    DMA_REQUEST_CFGTMR1_CHANNEL_7             = 27U,
    DMA_REQUEST_CFGTMR2_CHANNEL_0             = 28U,
    DMA_REQUEST_CFGTMR2_CHANNEL_1             = 29U,
    DMA_REQUEST_CFGTMR2_CHANNEL_2             = 30U,
    DMA_REQUEST_CFGTMR2_CHANNEL_3             = 31U,
    DMA_REQUEST_CFGTMR2_CHANNEL_4             = 32U,
    DMA_REQUEST_CFGTMR2_CHANNEL_5             = 33U,
    DMA_REQUEST_CFGTMR2_CHANNEL_6             = 34U,
    DMA_REQUEST_CFGTMR2_CHANNEL_7             = 35U,
    DMA_REQUEST_CFGTMR0_OR_CH0_CH7            = 36U,
    DMA_REQUEST_CFGTMR3_OR_CH0_CH7            = 37U,
    DMA_REQUEST_ADC0                        = 42U,
    DMA_REQUEST_ADC1                        = 43U,
    DMA_REQUEST_LPI2C_RX                    = 44U,
    DMA_REQUEST_LPI2C_TX                    = 45U,
    DMA_REQUEST_PDU0                        = 46U,
    DMA_REQUEST_PDU1                        = 47U,
    DMA_REQUEST_COMP                        = 48U,
    DMA_REQUEST_PMA                         = 49U,
    DMA_REQUEST_PMB                         = 50U,
    DMA_REQUEST_PMC                         = 51U,
    DMA_REQUEST_PMD                         = 52U,
    DMA_REQUEST_PME                         = 53U,
    DMA_REQUEST_CAN0                        = 54U,
    DMA_REQUEST_CAN1                        = 55U,
    DMA_REQUEST_CAN2                        = 56U,
    DMA_REQUEST_LPTMR                       = 59U,
    DMA_REQUEST_DMACHM_ALWAYS_ENABLED0      = 62U,
    DMA_REQUEST_DMACHM_ALWAYS_ENABLED1      = 63U
} DMA_REQUEST_SOURCE_T;

/*!
 * @brief   Trigger source of TMC
 * @details All possible inputs (trigger sources) of the TMC IP. This
 *          enumeration depends on the supported instances in device.
 */
enum TMC_TRIGGER_E
{
    TMC_TRIGGER_DISABLED            = 0U,
    TMC_TRIGGER_VDD                 = 1U,
    TMC_TRIGGER_TMC_IN0             = 2U,
    TMC_TRIGGER_TMC_IN1             = 3U,
    TMC_TRIGGER_TMC_IN2             = 4U,
    TMC_TRIGGER_TMC_IN3             = 5U,
    TMC_TRIGGER_TMC_IN4             = 6U,
    TMC_TRIGGER_TMC_IN5             = 7U,
    TMC_TRIGGER_TMC_IN6             = 8U,
    TMC_TRIGGER_TMC_IN7             = 9U,
    TMC_TRIGGER_TMC_IN8             = 10U,
    TMC_TRIGGER_TMC_IN9             = 11U,
    TMC_TRIGGER_TMC_IN10            = 12U,
    TMC_TRIGGER_TMC_IN11            = 13U,
    TMC_TRIGGER_COMP_OUT            = 14U,
    TMC_TRIGGER_LPITMR_CH0          = 17U,
    TMC_TRIGGER_LPITMR_CH1          = 18U,
    TMC_TRIGGER_LPITMR_CH2          = 19U,
    TMC_TRIGGER_LPITMR_CH3          = 20U,
    TMC_TRIGGER_LPTMR               = 21U,
    TMC_TRIGGER_CFGTMR0_INIT_TRIG     = 22U,
    TMC_TRIGGER_CFGTMR0_EXT_TRIG      = 23U,
    TMC_TRIGGER_CFGTMR1_INIT_TRIG     = 24U,
    TMC_TRIGGER_CFGTMR1_EXT_TRIG      = 25U,
    TMC_TRIGGER_CFGTMR2_INIT_TRIG     = 26U,
    TMC_TRIGGER_CFGTMR2_EXT_TRIG      = 27U,
    TMC_TRIGGER_CFGTMR3_INIT_TRIG     = 28U,
    TMC_TRIGGER_CFGTMR3_EXT_TRIG      = 29U,
    TMC_TRIGGER_ADC0_SC1A_COCO      = 30U,
    TMC_TRIGGER_ADC0_SC1B_COCO      = 31U,
    TMC_TRIGGER_ADC1_SC1A_COCO      = 32U,
    TMC_TRIGGER_ADC1_SC1B_COCO      = 33U,
    TMC_TRIGGER_PDU0_CH0_TRIG       = 34U,
    TMC_TRIGGER_PDU0_PULSE_OUT      = 36U,
    TMC_TRIGGER_PDU1_CH0_TRIG       = 37U,
    TMC_TRIGGER_PDU1_PULSE_OUT      = 39U,
    TMC_TRIGGER_RTC_ALARM           = 43U,
    TMC_TRIGGER_RTC_SECOND          = 44U,
    TMC_TRIGGER_CFGIO_TRIG0       = 45U,
    TMC_TRIGGER_CFGIO_TRIG1       = 46U,
    TMC_TRIGGER_CFGIO_TRIG2       = 47U,
    TMC_TRIGGER_CFGIO_TRIG3       = 48U,
    TMC_TRIGGER_LPUART0_RX_DATA     = 49U,
    TMC_TRIGGER_LPUART0_TX_DATA     = 50U,
    TMC_TRIGGER_LPUART0_RX_IDLE     = 51U,
    TMC_TRIGGER_LPUART1_RX_DATA     = 52U,
    TMC_TRIGGER_LPUART1_TX_DATA     = 53U,
    TMC_TRIGGER_LPUART1_RX_IDLE     = 54U,
    TMC_TRIGGER_LPI2C_MASTER_TRIG   = 55U,
    TMC_TRIGGER_LPI2C_SLAVE_TRIG    = 56U,
    TMC_TRIGGER_LPSPI0_FRAME        = 59U,
    TMC_TRIGGER_LPSPI0_RX_DATA      = 60U,
    TMC_TRIGGER_LPSPI1_FRAME        = 61U,
    TMC_TRIGGER_LPSPI1_RX_DATA      = 62U,
    TMC_TRIGGER_SIM_SW_TRIG         = 63U
};

/*!
 * @brief   Enumeration for target module of TMC
 * @details All possible outputs (target modules) of the TMC IP. This
 *          enumeration depends on the supported instances in device.
 */
enum TMC_TARGET_E
{
    TMC_TARGET_DMA_CH0           = 0U,
    TMC_TARGET_DMA_CH1           = 1U,
    TMC_TARGET_DMA_CH2           = 2U,
    TMC_TARGET_DMA_CH3           = 3U,
    TMC_TARGET_TMC_OUT0          = 4U,
    TMC_TARGET_TMC_OUT1          = 5U,
    TMC_TARGET_TMC_OUT2          = 6U,
    TMC_TARGET_TMC_OUT3          = 7U,
    TMC_TARGET_TMC_OUT4          = 8U,
    TMC_TARGET_TMC_OUT5          = 9U,
    TMC_TARGET_TMC_OUT6          = 10U,
    TMC_TARGET_TMC_OUT7          = 11U,
    TMC_TARGET_ADC0_ADHWT_TLA0   = 12U,
    TMC_TARGET_ADC0_ADHWT_TLA1   = 13U,
    TMC_TARGET_ADC0_ADHWT_TLA2   = 14U,
    TMC_TARGET_ADC0_ADHWT_TLA3   = 15U,
    TMC_TARGET_ADC1_ADHWT_TLA0   = 16U,
    TMC_TARGET_ADC1_ADHWT_TLA1   = 17U,
    TMC_TARGET_ADC1_ADHWT_TLA2   = 18U,
    TMC_TARGET_ADC1_ADHWT_TLA3   = 19U,
    TMC_TARGET_COMP_SAMPLE       = 28U,
    TMC_TARGET_CFGTMR0_HWTRIG0     = 40U,
    TMC_TARGET_CFGTMR0_FAULT0      = 41U,
    TMC_TARGET_CFGTMR0_FAULT1      = 42U,
    TMC_TARGET_CFGTMR0_FAULT2      = 43U,
    TMC_TARGET_CFGTMR1_HWTRIG0     = 44U,
    TMC_TARGET_CFGTMR1_FAULT0      = 45U,
    TMC_TARGET_CFGTMR1_FAULT1      = 46U,
    TMC_TARGET_CFGTMR1_FAULT2      = 47U,
    TMC_TARGET_CFGTMR2_HWTRIG0     = 48U,
    TMC_TARGET_CFGTMR2_FAULT0      = 49U,
    TMC_TARGET_CFGTMR2_FAULT1      = 50U,
    TMC_TARGET_CFGTMR2_FAULT2      = 51U,
    TMC_TARGET_CFGTMR3_HWTRIG0     = 52U,
    TMC_TARGET_CFGTMR3_FAULT0      = 53U,
    TMC_TARGET_CFGTMR3_FAULT1      = 54U,
    TMC_TARGET_CFGTMR3_FAULT2      = 55U,
    TMC_TARGET_PDU0_TRG_IN       = 56U,
    TMC_TARGET_PDU1_TRG_IN       = 60U,
    TMC_TARGET_CFGIO_TRG_TIM0  = 68U,
    TMC_TARGET_CFGIO_TRG_TIM1  = 69U,
    TMC_TARGET_CFGIO_TRG_TIM2  = 70U,
    TMC_TARGET_CFGIO_TRG_TIM3  = 71U,
    TMC_TARGET_LPITMR_TRG_CH0    = 72U,
    TMC_TARGET_LPITMR_TRG_CH1    = 73U,
    TMC_TARGET_LPITMR_TRG_CH2    = 74U,
    TMC_TARGET_LPITMR_TRG_CH3    = 75U,
    TMC_TARGET_LPUART0_TRG       = 76U,
    TMC_TARGET_LPUART1_TRG       = 80U,
    TMC_TARGET_LPI2C_TRG         = 84U,
    TMC_TARGET_LPSPI0_TRG        = 92U,
    TMC_TARGET_LPSPI1_TRG        = 96U,
    TMC_TARGET_LPTMR_ALT0        = 100U
};

/**@} end of group Features_Enumerations*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/** @defgroup Features_Macros Macros
  @{
*/

#ifndef bool
    #define bool    _Bool
    #define true    (bool)1U
    #define false   (bool)0U
#endif

#ifndef NULL
    #define NULL    0
#endif

#define POINTER_IS_NULL(__ptr__)    ((__ptr__ == NULL) ? true : false)

/*! @brief Number of cores */
#define CORES_COUNT (1U)

/*! @brief Number of alternative clocks available */
#define ALT_CLOCKS_COUNT  ADC_INPUT_CLK_1

/* PCM index offsets */
#define PCM_UNUSEFUL_INDEX       0
#define PCM_FLASH_INDEX         32
#define PCM_DMACHM_INDEX        33
#define PCM_CAN0_INDEX      36
#define PCM_CAN1_INDEX      37
#define PCM_CFGTMR3_INDEX         38
#define PCM_ADC1_INDEX          39
#define PCM_CAN2_INDEX      43
#define PCM_LPSPI0_INDEX        44
#define PCM_LPSPI1_INDEX        45
#define PCM_LPSPI2_INDEX        46
#define PCM_PDU1_INDEX          49
#define PCM_CRC_INDEX           50
#define PCM_PDU0_INDEX          54
#define PCM_LPITMR_INDEX        55
#define PCM_CFGTMR0_INDEX         56
#define PCM_CFGTMR1_INDEX         57
#define PCM_CFGTMR2_INDEX         58
#define PCM_ADC0_INDEX          59
#define PCM_RTC_INDEX           61
#define PCM_LPTMR_INDEX         64
#define PCM_PMA_INDEX           73
#define PCM_PMB_INDEX           74
#define PCM_PMC_INDEX           75
#define PCM_PMD_INDEX           76
#define PCM_PME_INDEX           77
#define PCM_CFGIO_INDEX       90
#define PCM_EWDT_INDEX          97
#define PCM_LPI2C_INDEX         102
#define PCM_LPUART0_INDEX       106
#define PCM_LPUART1_INDEX       107
#define PCM_LPUART2_INDEX       108
#define PCM_COMP_INDEX          115

/*!
 * @brief   PCM clock name mappings
 * @details Mappings between clock names and peripheral clock control indexes.
 *          If there is no peripheral clock control index for a clock name,
 *          then the corresponding value is PCM_UNUSEFUL_INDEX.
 */
#define PCM_CLOCK_NAME_MAPPINGS \
{ \
    PCM_UNUSEFUL_INDEX,      /* Core clock                      0 */  \
    PCM_UNUSEFUL_INDEX,      /* Bus clock                       1 */  \
    PCM_UNUSEFUL_INDEX,      /* Slow clock                      2 */  \
    PCM_UNUSEFUL_INDEX,      /* CLKOUT clock                    3 */  \
    PCM_UNUSEFUL_INDEX,      /* LSICLK clock                    4 */  \
    PCM_UNUSEFUL_INDEX,      /* HSICLK clock                    5 */  \
    PCM_UNUSEFUL_INDEX,      /* SYSOSC clock                      6 */  \
    PCM_UNUSEFUL_INDEX,      /* SYSPLL clock                    7 */  \
    PCM_UNUSEFUL_INDEX,      /* RTC_CLKIN clock                 8 */  \
    PCM_UNUSEFUL_INDEX,      /* SCG CLK_OUT clock               9 */  \
    PCM_UNUSEFUL_INDEX,      /* LSIDIV1 functional clock        10 */ \
    PCM_UNUSEFUL_INDEX,      /* LSIDIV2 functional clock        11 */ \
    PCM_UNUSEFUL_INDEX,      /* HSIDIV1 functional clock        12 */ \
    PCM_UNUSEFUL_INDEX,      /* HSIDIV2 functional clock        13 */ \
    PCM_UNUSEFUL_INDEX,      /* SYSOSCDIV1 functional clock       14 */ \
    PCM_UNUSEFUL_INDEX,      /* SYSOSCDIV2 functional clock       15 */ \
    PCM_UNUSEFUL_INDEX,      /* SYSPLLDIV1 functional clock     16 */ \
    PCM_UNUSEFUL_INDEX,      /* SYSPLLDIV2 functional clock     17 */ \
    PCM_UNUSEFUL_INDEX,      /* End of SCG clocks               18 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 19 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 20 */ \
    PCM_UNUSEFUL_INDEX,      /* CFGTMR0 External Clock Pin Select 21 */ \
    PCM_UNUSEFUL_INDEX,      /* CFGTMR1 External Clock Pin Select 22 */ \
    PCM_UNUSEFUL_INDEX,      /* CFGTMR2 External Clock Pin Select 23 */ \
    PCM_UNUSEFUL_INDEX,      /* CFGTMR3 External Clock Pin Select 24 */ \
    PCM_UNUSEFUL_INDEX,      /* CLKOUT Select                   25 */ \
    PCM_UNUSEFUL_INDEX,      /* CLK32K clock                    26 */ \
    PCM_UNUSEFUL_INDEX,      /* LPO clock                       27 */ \
    PCM_UNUSEFUL_INDEX,      /* LPO 1KHz clock                  28 */ \
    PCM_UNUSEFUL_INDEX,      /* LPO 32KHz clock                 29 */ \
    PCM_UNUSEFUL_INDEX,      /* LPO 128KHz clock                30 */ \
    PCM_UNUSEFUL_INDEX,      /* EINJ clock source               31 */ \
    PCM_UNUSEFUL_INDEX,      /* EREP clock source               32 */ \
    PCM_UNUSEFUL_INDEX,      /* DMA clock source                33 */ \
    PCM_UNUSEFUL_INDEX,      /* MPU clock source                34 */ \
    PCM_UNUSEFUL_INDEX,      /* MSC clock source                35 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 36 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 37 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 38 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 39 */ \
    PCM_UNUSEFUL_INDEX,      /* No clock entry in CLOCK_NAMES_T 40 */ \
    PCM_COMP_INDEX,         /* COMP clock source               41 */ \
    PCM_CRC_INDEX,          /* CRC clock source                42 */ \
    PCM_DMACHM_INDEX,       /* DMACHM clock source             43 */ \
    PCM_EWDT_INDEX,         /* EWDT clock source               44 */ \
    PCM_PMA_INDEX,          /* PMA clock source                45 */ \
    PCM_PMB_INDEX,          /* PMB clock source                46 */ \
    PCM_PMC_INDEX,          /* PMC clock source                47 */ \
    PCM_PMD_INDEX,          /* PMD clock source                48 */ \
    PCM_PME_INDEX,          /* PME clock source                49 */ \
    PCM_RTC_INDEX,          /* RTC clock source                50 */ \
    PCM_UNUSEFUL_INDEX,      /* End of BUS clocks               51 */ \
    PCM_CAN0_INDEX,     /* CAN0 clock source               52 */ \
    PCM_CAN1_INDEX,     /* CAN1 clock source               53 */ \
    PCM_CAN2_INDEX,     /* CAN2 clock source               54 */ \
    PCM_PDU0_INDEX,         /* PDU0 clock source               55 */ \
    PCM_PDU1_INDEX,         /* PDU1 clock source               56 */ \
    PCM_UNUSEFUL_INDEX,      /* End of SYS clocks               57 */ \
    PCM_FLASH_INDEX,        /* FLASH clock source              58 */ \
    PCM_UNUSEFUL_INDEX,      /* End of SLOW clocks              59 */ \
    PCM_CFGTMR0_INDEX,        /* CFGTMR0 clock source              60 */ \
    PCM_CFGTMR1_INDEX,        /* CFGTMR1 clock source              61 */ \
    PCM_CFGTMR2_INDEX,        /* CFGTMR2 clock source              62 */ \
    PCM_CFGTMR3_INDEX,        /* CFGTMR3 clock source              63 */ \
    PCM_UNUSEFUL_INDEX,      /* End of ASYNCH DIV1 clocks       64 */ \
    PCM_ADC0_INDEX,         /* ADC0 clock source               65 */ \
    PCM_ADC1_INDEX,         /* ADC1 clock source               66 */ \
    PCM_CFGIO_INDEX,      /* CFGIO clock source            67 */ \
    PCM_LPI2C_INDEX,        /* LPI2C clock source              68 */ \
    PCM_LPITMR_INDEX,       /* LPITMR clock source             69 */ \
    PCM_LPSPI0_INDEX,       /* LPSPI0 clock source             70 */ \
    PCM_LPSPI1_INDEX,       /* LPSPI1 clock source             71 */ \
    PCM_LPSPI2_INDEX,       /* LPSPI2 clock source             72 */ \
    PCM_LPTMR_INDEX,        /* LPTMR clock source              73 */ \
    PCM_LPUART0_INDEX,      /* LPUART0 clock source            74 */ \
    PCM_LPUART1_INDEX,      /* LPUART1 clock source            75 */ \
    PCM_LPUART2_INDEX,      /* LPUART2 clock source            76 */ \
    PCM_UNUSEFUL_INDEX,      /* End of ASYNCH DIV2 clocks       77 */ \
    PCM_UNUSEFUL_INDEX,      /* End of PCM clocks               78 */ \
}

/*!
 * @brief   Peripheral instance features
 * @details List of features that are supported by a peripheral instance
 */
#define WITH_INTERFACE_CLOCK_FROM_SLOW_CLOCK   (1U << 7U)
#define WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK    (1U << 6U)
#define WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK    (1U << 5U)
#define WITH_PROTOCOL_CLOCK_FROM_ASYNC2        (1U << 4U)
#define WITH_PROTOCOL_CLOCK_FROM_ASYNC1        (1U << 3U)
#define WITH_DIVIDER                           (1U << 2U)
#define WITH_MULTIPLIER                        (1U << 1U)
#define WITH_CLOCK_GATING_IN_SIM               (1U << 0U)
#define NOT_PERIPHERAL_INSTANCE                (0U)

/*!
 * @brief   Peripheral features
 * @details List of features for each clock name. If a clock name is not a
 *          peripheral, no feature is supported.
 */
#define PERIPHERAL_FEATURES \
{ \
    (NOT_PERIPHERAL_INSTANCE),    /* Core clock                      0 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* Bus clock                       1 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* Slow clock                      2 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* CLKOUT clock                    3 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* LSICLK clock                    4 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* HSICLK clock                    5 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* SYSOSC clock                      6 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* SYSPLL clock                    7 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* RTC_CLKIN clock                 8 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* SCG CLK_OUT clock               9 */  \
    (NOT_PERIPHERAL_INSTANCE),    /* End of SCG clocks               10 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 11 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 12 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 13 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 14 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 15 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 16 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 17 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 18 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 19 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* No clock entry in CLOCK_NAMES_T 20 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CFGTMR0 External Clock Pin Select 21 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CFGTMR1 External Clock Pin Select 22 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CFGTMR2 External Clock Pin Select 23 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CFGTMR3 External Clock Pin Select 24 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CLKOUT Select                   25 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* CLK32K clock                    26 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* LPO clock                       27 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* LPO 1KHz clock                  28 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* LPO 32KHz clock                 29 */ \
    (NOT_PERIPHERAL_INSTANCE),    /* LPO 128KHz clock                30 */ \
    (WITH_CLOCK_GATING_IN_SIM | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),   /* EINJ clock source 31 */ \
    (WITH_CLOCK_GATING_IN_SIM | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),   /* EREP clock source 32 */ \
    (WITH_CLOCK_GATING_IN_SIM | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),   /* DMA clock source  33 */ \
    (WITH_CLOCK_GATING_IN_SIM | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),   /* MPU clock source  34 */ \
    (WITH_CLOCK_GATING_IN_SIM | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),   /* MSC clock source  35 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* No clock entry in CLOCK_NAMES_T  36 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* No clock entry in CLOCK_NAMES_T  37 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* No clock entry in CLOCK_NAMES_T  38 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* No clock entry in CLOCK_NAMES_T  39 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* No clock entry in CLOCK_NAMES_T  40 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* COMP clock source                41 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* CRC clock source                 42 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* DMACHM clock source              43 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* EWDT clock source                44 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* PMA clock source                 45 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* PMB clock source                 46 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* PMC clock source                 47 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* PMD clock source                 48 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* PME clock source                 49 */ \
    (WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),     /* RTC clock source                 50 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* End of BUS clocks                51 */ \
    (WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),     /* CAN0 clock source                52 */ \
    (WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),     /* CAN1 clock source                53 */ \
    (WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),     /* CAN2 clock source                54 */ \
    (WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),     /* PDU0 clock source                55 */ \
    (WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),     /* PDU1 clock source                56 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* End of SYS clocks                57 */ \
    (WITH_INTERFACE_CLOCK_FROM_SLOW_CLOCK),    /* FLASH clock source               58 */ \
    (NOT_PERIPHERAL_INSTANCE),                 /* End of SLOW clocks               59 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),    /* CFGTMR0 clock source           60 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),    /* CFGTMR1 clock source           61 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),    /* CFGTMR2 clock source           62 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC1 | WITH_INTERFACE_CLOCK_FROM_SYS_CLOCK),    /* CFGTMR3 clock source           63 */ \
    (NOT_PERIPHERAL_INSTANCE),                                                  /* End of ASYNCH DIV1 clocks    64 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* ADC0 clock source            65 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* ADC1 clock source            66 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* CFGIO clock source         67 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPI2C clock source           68 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPITMR clock source          69 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPSPI0 clock source          70 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPSPI1 clock source          71 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPSPI2 clock source          72 */ \
    (WITH_MULTIPLIER | WITH_DIVIDER | WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK), \
                                                                                /* LPTMR clock source           73 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPUART0 clock source         74 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPUART1 clock source         75 */ \
    (WITH_PROTOCOL_CLOCK_FROM_ASYNC2 | WITH_INTERFACE_CLOCK_FROM_BUS_CLOCK),    /* LPUART2 clock source         76 */ \
    (NOT_PERIPHERAL_INSTANCE),                  /* End of ASYNCH DIV2 clocks    77 */ \
    (NOT_PERIPHERAL_INSTANCE),                  /* End of PCM clocks            78 */ \
}

/*!
* @brief Time to wait for SYSOSC to stabilize
*/
#define SYSOSC_STABILIZATION_TIMEOUT 3205000U;

/*!
* @brief Time to wait for LSICLK to stabilize
*/
#define LSICLK_STABILIZATION_TIMEOUT 100U

/*!
* @brief Time to wait for HSICLK to stabilize
*/
#define HSICLK_STABILIZATION_TIMEOUT 100U

/*!
* @brief Time to wait for SYSPLL to stabilize
*/
#define SYSPLL_STABILIZATION_TIMEOUT 1000U;


/*!
 * @brief   Temporary system clock source configurations.
 * @details Each line represents the SYS(CORE), BUS and SLOW(FLASH) dividers
 *          for LSICLK, HSICLK, SYSOSC and SYSPLL clock sources.
 *
 *          SYS_CLK  BUS_CLK  SLOW_CLK
 *  LSICLK     *        *         *
 *  HSICLK     *        *         *
 *  SYSOSC     *        *         *
 *  SYSPLL     *        *         *
 */
#define TEMP_LSICLK_CLK      0U
#define TEMP_HSICLK_CLK      1U
#define TEMP_SYSOSC_CLK      2U
#define TEMP_SYSPLL_CLK      3U

#define TEMP_SYS_DIV         0U
#define TEMP_BUS_DIV         1U
#define TEMP_SLOW_DIV        2U

#define TEMP_SYS_CLK_INDEX   4U
#define TEMP_SYS_DIV_INDEX   3U

#define TEMP_SYSTEM_CLOCK_CONFIGS \
{ /*      SYS_CLK                 BUS_CLK                 SLOW_CLK */ \
{ SCG_SYS_CLOCK_DIV_BY_1, SCG_SYS_CLOCK_DIV_BY_1, SCG_SYS_CLOCK_DIV_BY_2},  /* Dividers for LSICLK */ \
{ SCG_SYS_CLOCK_DIV_BY_1, SCG_SYS_CLOCK_DIV_BY_2, SCG_SYS_CLOCK_DIV_BY_4},  /* Dividers for HSICLK */ \
{ SCG_SYS_CLOCK_DIV_BY_1, SCG_SYS_CLOCK_DIV_BY_2, SCG_SYS_CLOCK_DIV_BY_2},  /* Dividers for SYSOSC */ \
{ SCG_SYS_CLOCK_DIV_BY_3, SCG_SYS_CLOCK_DIV_BY_2, SCG_SYS_CLOCK_DIV_BY_2},  /* Dividers for SYSPLL */ \
}

/*!
 * @brief template system clock configuration in VLPR mode
 */
#define FEATURE_VLPR_SYS_CLK SCG_SYS_CLOCK_DIV_BY_2
#define FEATURE_VLPR_BUS_CLK SCG_SYS_CLOCK_DIV_BY_1
#define FEATURE_VLPR_SLOW_CLK SCG_SYS_CLOCK_DIV_BY_4

/*!
 * @brief Constant array storing the value of all TMC output (target module) identifiers
 */
#define FEATURE_TMC_TARGET_MODULE \
{ \
    TMC_TARGET_DMA_CH0,          \
    TMC_TARGET_DMA_CH1,          \
    TMC_TARGET_DMA_CH2,          \
    TMC_TARGET_DMA_CH3,          \
    TMC_TARGET_TMC_OUT0,         \
    TMC_TARGET_TMC_OUT1,         \
    TMC_TARGET_TMC_OUT2,         \
    TMC_TARGET_TMC_OUT3,         \
    TMC_TARGET_TMC_OUT4,         \
    TMC_TARGET_TMC_OUT5,         \
    TMC_TARGET_TMC_OUT6,         \
    TMC_TARGET_TMC_OUT7,         \
    TMC_TARGET_ADC0_ADHWT_TLA0,  \
    TMC_TARGET_ADC0_ADHWT_TLA1,  \
    TMC_TARGET_ADC0_ADHWT_TLA2,  \
    TMC_TARGET_ADC0_ADHWT_TLA3,  \
    TMC_TARGET_ADC1_ADHWT_TLA0,  \
    TMC_TARGET_ADC1_ADHWT_TLA1,  \
    TMC_TARGET_ADC1_ADHWT_TLA2,  \
    TMC_TARGET_ADC1_ADHWT_TLA3,  \
    TMC_TARGET_COMP_SAMPLE,      \
    TMC_TARGET_CFGTMR0_HWTRIG0,    \
    TMC_TARGET_CFGTMR0_FAULT0,     \
    TMC_TARGET_CFGTMR0_FAULT1,     \
    TMC_TARGET_CFGTMR0_FAULT2,     \
    TMC_TARGET_CFGTMR1_HWTRIG0,    \
    TMC_TARGET_CFGTMR1_FAULT0,     \
    TMC_TARGET_CFGTMR1_FAULT1,     \
    TMC_TARGET_CFGTMR1_FAULT2,     \
    TMC_TARGET_CFGTMR2_HWTRIG0,    \
    TMC_TARGET_CFGTMR2_FAULT0,     \
    TMC_TARGET_CFGTMR2_FAULT1,     \
    TMC_TARGET_CFGTMR2_FAULT2,     \
    TMC_TARGET_CFGTMR3_HWTRIG0,    \
    TMC_TARGET_CFGTMR3_FAULT0,     \
    TMC_TARGET_CFGTMR3_FAULT1,     \
    TMC_TARGET_CFGTMR3_FAULT2,     \
    TMC_TARGET_PDU0_TRG_IN,      \
    TMC_TARGET_PDU1_TRG_IN,      \
    TMC_TARGET_CFGIO_TRG_TIM0, \
    TMC_TARGET_CFGIO_TRG_TIM1, \
    TMC_TARGET_CFGIO_TRG_TIM2, \
    TMC_TARGET_CFGIO_TRG_TIM3, \
    TMC_TARGET_LPITMR_TRG_CH0,   \
    TMC_TARGET_LPITMR_TRG_CH1,   \
    TMC_TARGET_LPITMR_TRG_CH2,   \
    TMC_TARGET_LPITMR_TRG_CH3,   \
    TMC_TARGET_LPUART0_TRG,      \
    TMC_TARGET_LPUART1_TRG,      \
    TMC_TARGET_LPI2C_TRG,        \
    TMC_TARGET_LPSPI0_TRG,       \
    TMC_TARGET_LPSPI1_TRG,       \
    TMC_TARGET_LPTMR_ALT0        \
}

/**@} end of group Features_Macros*/

/** @defgroup Features_Macros Macros
  @{
*/

/*******************************************************************************
 *                          PM Module Features
 ******************************************************************************/
/*! @brief Has port input disable control bits*/
#define FEATURE_PM_WITH_INPUT_DISABLE (1)

/*! @brief SIM_CHIPCTRL[ADCISMEN] bit */
#define FEATURE_PINS_WITH_ADC_INTERLEAVE_EN (1)

/*******************************************************************************
 *                          SCG Module Features
 ******************************************************************************/
/*! @brief PM availability on the SoC */
#define FEATURE_SOC_PM_COUNT (5)

/*! @brief Low speed internal high range clock frequency 8MHz*/
#define FEATURE_SCG_LSICLK_HIGH_RANGE_FREQUENCY (8000000U)

/*! @brief High speed internal trimmed clock frequency(48MHz) */
#define FEATURE_SCG_HSICLK_FREQUENCY  (48000000U)

/*! @brief Supports high speed run mode */
#define FEATURE_WITH_HIGH_SPEED_RUN_MODE     (1U)

/*! @brief Supports SYSPLL clock source */
#define FEATURE_WITH_SYSPLL_CLK                (1U)

/*! @brief Supports LPO peripheral clock source */
#define FEATURE_WITH_LPO_PERIPHERAL_CLOCK_SOURCE (0U)

/*******************************************************************************
 *                          COMP Module Features
 ******************************************************************************/
/*! @brief Number of instances of the COMP module */
#define COMP_INSTANCE_COUNT (1U)

/*! @brief COMP peripheral instance base addresses */
#define COMP_BASE_ADDRS { COMP_BASE }

/*! @brief Array initializer of COMP peripheral base pointers */
#define COMP_BASE_PTRS { COMP }

/*! @brief Number of interrupt vector arrays for the COMP module */
#define COMP_IRQS_ARR_COUNT (1U)

/*! @brief Number of interrupt channels for the COMP module */
#define COMP_IRQS_CH_COUNT (1U)

/*! @brief Interrupt vectors for the COMP peripheral type */
#define COMP_IRQS { COMP_IRQn }

/*******************************************************************************
 *                          LPITMR Module Features
 ******************************************************************************/
/*! @brief Number of instances of the LPITMR module */
#define LPITMR_INSTANCE_COUNT   (1U)
#define LPITMR_COUNT          (4U)

/*! @brief Peripheral LPITMR0 base address */
#define LPITMR0_BASE            (0x40037000U)

/*! @brief Peripheral LPITMR0 base pointer */
#define LPITMR0                 ((LPITMR_T *)LPITMR0_BASE)

/*! @brief Array initializer of LPITMR peripheral base addresses */
#define LPITMR_BASE_ADDRS       { LPITMR0_BASE }

/*! @brief Array initializer of LPITMR peripheral base pointers */
#define LPITMR_BASE_PTRS        { LPITMR0 }

/*! @brief Number of interrupt vector arrays for the LPITMR module */
#define LPITMR_IRQS_ARR_COUNT   (1U)

/*! @brief Number of interrupt channels for the LPITMR module */
#define LPITMR_IRQS_CH_COUNT    (4U)

/*! @brief Interrupt vectors for the LPITMR peripheral type */
#define LPITMR_IRQS { LPITMR_Ch0_IRQn, LPITMR_Ch1_IRQn, LPITMR_Ch2_IRQn, LPITMR_Ch3_IRQn }

/*! @brief Number of interrupt vector for channels of the LPITMR module */
#define FEATURE_LPITMR_IRQS_NUM_CHANS   (4U)

/*! @brief Clock names for LPITMR */
#define LPITMR_CLOCK_NAMES      {LPITMR_CLK}

/*******************************************************************************
 *                          OSIF Module Features
 ******************************************************************************/
#define FEATURE_OSIF_USE_SYSTICK                    (1)
#define FEATURE_OSIF_USE_PIT                        (0)
#define FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD    (1)

/*******************************************************************************
 *                          LPSPI Module Features
 ******************************************************************************/
/*! @brief Initial value for state structure */
#define FEATURE_LPSPI_STATUS_NULL {NULL, NULL, NULL}

/*******************************************************************************
 *                          LPTMR Module Features
 ******************************************************************************/
/*! @brief LPTMR pulse counter input options */
#define FEATURE_LPTMR_WITH_INPUT_ALT1_SELECTION  (1U)

/*******************************************************************************
 *                          TMC Module Features
 ******************************************************************************/
/*! @brief Number of instances of the TMC module */
#define TMC_INSTANCE_COUNT  (1U)

/*! @brief Array initializer of TMC peripheral base addresses */
#define TMC_BASE_ADDRS      { TMC_BASE }

/*! @brief Array initializer of TMC peripheral base pointers */
#define TMC_BASE_PTRS       { TMC }

/*******************************************************************************
 *                          FLASH Module Features
 ******************************************************************************/
/*! @brief CFGNVM start address */
#define FEATURE_FLASH_DF_START_ADDRESS (0x10000000U)
/*! @brief CFGRAM start address */
#define FEATURE_FLASH_CFGRAM_START_ADDRESS (0x14000000U)
/*! @brief PFlash block count */
#define FEATURE_FLASH_PF_BLOCK_NUM (1U)
/*! @brief PFlash block size */
#define FEATURE_FLASH_PF_BLOCK_SIZE (0x80000U)
/*! @brief PFlash sector size */
#define FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE (8192U)
/*! @brief PFlash write unit size */
#define FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE (8U)
/*! @brief CFGNVM block count */
#define FEATURE_FLASH_DF_BLOCK_NUM (1U)
/*! @brief CFGNVM block size */
#define FEATURE_FLASH_DF_BLOCK_SIZE (65536U)
/*! @brief CFGNVM sector size */
#define FEATURE_FLASH_DF_BLOCK_SECTOR_SIZE (2048U)
/*! @brief CFGNVM write unit size */
#define FEATURE_FLASH_DF_BLOCK_WRITE_UNIT_SIZE (8U)
/*! @brief CFGRAM size */
#define FEATURE_FLASH_CFGRAM_SIZE (4096U)

/*! @brief PFlash Erase/Read 1st all block command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_BLOCK_CMD (16U)
/*! @brief PFlash Erase sector command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SECTOR_CMD (16U)
/*! @brief PFlash Program/Verify section command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SECTION_CMD (16U)
/*! @brief PFlash Read resource command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_RESOURCE_CMD (8U)
/*! @brief PFlash Program check command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD (4U)
/*! @brief PFlash Program check command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SWAP_CONTROL_CMD (0U)
/*! @brief CFGNVM Erase/Read 1st all block command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_DF_BLOCK_CMD (8U)
/*! @brief CFGNVM Erase sector command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_DF_SECTOR_CMD (8U)
/*! @brief CFGNVM Program/Verify section command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_DF_SECTION_CMD (8U)
/*! @brief CFGNVM Read resource command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_DF_RESOURCE_CMD (8U)
/*! @brief CFGNVM Program check command address alignment */
#define FEATURE_FLASH_ADDRESS_ALIGMENT_DF_CHECK_CMD (4U)

/*! @brief CFGNVM partition code */
#define FEATURE_FLASH_DEPART_0000 (0x00010000U)
#define FEATURE_FLASH_DEPART_0001 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_0010 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_0011 (0x00008000U)
#define FEATURE_FLASH_DEPART_0100 (0x00000000U)
#define FEATURE_FLASH_DEPART_0101 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_0110 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_0111 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_1000 (0x00000000U)
#define FEATURE_FLASH_DEPART_1001 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_1010 (0x00004000U)
#define FEATURE_FLASH_DEPART_1011 (0x00008000U)
#define FEATURE_FLASH_DEPART_1100 (0x00010000U)
#define FEATURE_FLASH_DEPART_1101 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_1110 (0xFFFFFFFFU)
#define FEATURE_FLASH_DEPART_1111 (0x00010000U)

/*! @brief The interrupt double bit fault detect */
#define FEATURE_FLASH_INTERRUPT_DOUBLE_BIT_FAULT_IRQ (1)

/*******************************************************************************
 *                          CAN Module Features
 ******************************************************************************/
/*! @brief Frames available in Rx FIFO flag shift */
#define FEATURE_CAN_RXFIFO_FRAME_AVAILABLE  (5U)
/*! @brief Rx FIFO warning flag shift */
#define FEATURE_CAN_RXFIFO_WARNING          (6U)
/*! @brief Rx FIFO overflow flag shift */
#define FEATURE_CAN_RXFIFO_OVERFLOW         (7U)
/*! @brief The list contains definitions of the FD feature support on all instances */
#define FEATURE_CAN_INSTANCES_WITH_FD        {true, true, true}

/* @brief Configurable Data Rate for CAN0 */
#define FEATURE_CAN0_WITH_FD                 (1)
/* @brief Configurable Data Rate for CAN1 */
#define FEATURE_CAN1_WITH_FD                 (0)
/* @brief Configurable Data Rate for CAN2 */
#define FEATURE_CAN2_WITH_FD                 (0)

/*! @brief Max number of Message Buffers */
#define FEATURE_CAN0_MB_MAX_NUM             (32U)
#define FEATURE_CAN1_MB_MAX_NUM             (16U)
#define FEATURE_CAN2_MB_MAX_NUM             (16U)
#define FEATURE_CAN_MAX_MB_NUM                  (32U)
#define FEATURE_CAN_MAX_MB_NUM_ARRAY            { FEATURE_CAN0_MB_MAX_NUM, \
                                                FEATURE_CAN1_MB_MAX_NUM, \
                                                FEATURE_CAN2_MB_MAX_NUM }

/*! @brief Max number of Message Buffers IRQs */
#define FEATURE_CAN_MB_IRQS_MAX       (2U)
/*! @brief Message Buffers IRQs */
#define CAN_ORed_0_15_MB_IRQS           { CAN0_ORed_0_15_MB_IRQn, CAN1_ORed_0_15_MB_IRQn, CAN2_ORed_0_15_MB_IRQn }
#define CAN_ORed_16_31_MB_IRQS          { CAN0_ORed_16_31_MB_IRQn, NotAvail_IRQn, NotAvail_IRQn }
#define FEATURE_CAN_MB_IRQS             { CAN_ORed_0_15_MB_IRQS, \
                                              CAN_ORed_16_31_MB_IRQS }

/*! @brief The oscillator clock source */
#define FEATURE_CAN_OSC_CLK_NAME         SYSOSC_CLK

/*******************************************************************************
 *                          LPUART Module Features
 ******************************************************************************/
/*! @brief Clock names for LPUART */
#define LPUART_CLOCK_NAMES {LPUART0_CLK, LPUART1_CLK, LPUART2_CLK}

/*******************************************************************************
 *                          CFGIO Module Features
 ******************************************************************************/
/*! @brief Number of instances of the CFGIO module */
#define CFGIO_INSTANCE_COUNT      (1U)
/*! @brief Array initializer of CFGIO peripheral base addresses */
#define CFGIO_BASE_ADDRS          { CFGIO_BASE }
/*! @brief Array initializer of CFGIO peripheral base pointers */
#define CFGIO_BASE_PTRS           { CFGIO }
/*! @brief Number of interrupt vector arrays for the CFGIO module */
#define CFGIO_IRQS_ARR_COUNT      (1U)
/*! @brief Number of interrupt channels for the CFGIO module */
#define CFGIO_IRQS_CH_COUNT       (1U)
/*! @brief Interrupt vectors for the CFGIO peripheral type */
#define CFGIO_IRQS                { CFGIO_IRQn }

/*! @brief Define the maximum number of shifters for any CFGCFGIO instance */
#define FEATURE_CFGIO_MAX_SHIFTER_COUNT  (4U)

/*! @brief Define DMA request names for CFGIO */
#define FEATURE_CFGIO_DMA_REQUEST_0    DMA_REQUEST_CFGIO_SHIFTER0
#define FEATURE_CFGIO_DMA_REQUEST_1    DMA_REQUEST_CFGIO_SHIFTER1
#define FEATURE_CFGIO_DMA_REQUEST_2    DMA_REQUEST_CFGIO_SHIFTER2
#define FEATURE_CFGIO_DMA_REQUEST_3    DMA_REQUEST_CFGIO_SHIFTER3

/*******************************************************************************
 *                          LPI2C Module Features
 ******************************************************************************/
/*! @brief DMA requests for LPI2C module */
#define LPI2C_DMA_REQUEST       {{(uint8_t)DMA_REQUEST_LPI2C_TX, (uint8_t)DMA_REQUEST_LPI2C_RX}}

/*! @brief PCM clocks for LPI2C module */
#define LPI2C_PCM_CLOCKS    {LPI2C_CLK}

/*******************************************************************************
 *                          INTERRUPT Module Features
 ******************************************************************************/
/*! @brief Lowest interrupt request number */
#define FEATURE_INTERRUPT_MIN_IRQ               (NonMaskableInt_IRQn)

/*! @brief Highest interrupt request number */
#define FEATURE_INTERRUPT_MAX_IRQ               (CFGTMR3_Ovf_Reload_IRQn)

/**< Number of priority bits implemented in the NVIC */
#define FEATURE_NVIC_PRIO_BITS                  (4U)

/*! @brief Active interrupt state */
#define FEATURE_INTERRUPT_WITH_ACTIVE_STATE      (1U)

/*! @brief Registers in which the start of interrupt vector table needs to be configured */
#define FEATURE_INTERRUPT_INT_VECTORS_REG           {&APM32_SCB->VTOR.reg}

/*******************************************************************************
 *              SCB (System Control Block) Module Features
 ******************************************************************************/
/*! @brief VECTKEY value so that AIRCR register write is not ignored */
#define FEATURE_SCB_VECTKEY     (0x05FAU)

/*******************************************************************************
 *                          SYSMC Module Features
 ******************************************************************************/

/*! @brief Has high speed run mode (register bit PMPRO[ALLOWHS]) */
#define FEATURE_SYSMC_WITH_HIGH_SPEED_RUN_MODE (1U)

/*******************************************************************************
 *                          RMU Module Features
 ******************************************************************************/
/*! @brief CMU loss of clock as reset source */
#define FEATURE_RMU_WITH_CMU_LOSS_OF_CLOCK (0)

/*******************************************************************************
 *                          MPU Module Features
 ******************************************************************************/

/*! @brief The number of master has process identifier */
#define FEATURE_MPU_MASTER_WITH_PROCESS_IDENTIFIER_NUM    (2U)

/*! @brief Specifies total number of bus masters */
#define FEATURE_MPU_MASTER_NUM                (3U)

/*!
 * @brief   Specifies max number of masters which have separated
 *          privilege rights for user and supervisor mode accesses.
 */
#define FEATURE_MPU_MAX_LOW_MASTER_NUM     (3U)

/*!
 * @brief   Specifies maximum number of masters which have only
 *          read and write permissions.
 */
#define FEATURE_MPU_MAX_HIGH_MASTER_NUM    (7U)

/*!
 * @brief   Specifies number of set access control right bits for
 *          masters which have separated privilege rights for user and
 *          supervisor mode accesses.
 */
#define FEATURE_MPU_LOW_MASTER_CONTROL_WIDTH  (6U)

/*!
 * @brief   Specifies number of set access control right bits for
 *          masters which have only read and write permissions.
 */
#define FEATURE_MPU_HIGH_MASTER_CONTROL_WIDTH (2U)

/*! @brief The MPU Logical Bus Master Number for core bus master */
#define FEATURE_MPU_MASTER_CORE               (0U)

/*! @brief The MPU Logical Bus Master Number for Debugger master */
#define FEATURE_MPU_MASTER_DEBUGGER           (1U)

/*! @brief The MPU Logical Bus Master Number for DMA master */
#define FEATURE_MPU_MASTER_DMA                (2U)

/*! @brief Specifies master number */
#define FEATURE_MPU_MASTER \
{ \
    FEATURE_MPU_MASTER_CORE,     /* CORE */     \
    FEATURE_MPU_MASTER_DEBUGGER, /* DEBUGGER */ \
    FEATURE_MPU_MASTER_DMA,      /* DMA */      \
}

/*! @brief Specifies total number of slave ports */
#define FEATURE_MPU_SLAVE_NUM               (4U)

/*! @brief The MPU Slave Port Assignment for Flash Controller and boot ROM */
#define FEATURE_MPU_SLAVE_FLASH_BOOTROM       (0U)

/*! @brief The MPU Slave Port Assignment for SRAM back door */
#define FEATURE_MPU_SLAVE_SRAM_BACKDOOR       (1U)

/*! @brief The MPU Slave Port Assignment for SRAM_L front door */
#define FEATURE_MPU_SLAVE_SRAM_L_FRONTDOOR    (2U)

/*! @brief The MPU Slave Port Assignment for SRAM_U front door */
#define FEATURE_MPU_SLAVE_SRAM_U_FRONTDOOR    (3U)

/*******************************************************************************
 *                          WDT Module Features
 ******************************************************************************/
/*! @brief The 32-bit value used for unlocking the WDT */
#define FEATURE_WDT_UNLOCK_VALUE            (0xD928C520U)

/*! @brief The 32-bit value used for resetting the WDT counter */
#define FEATURE_WDT_TRIGGER_VALUE           (0xB480A602U)

/*! @brief The reset value of the timeout register */
#define FEATURE_WDT_TIMEOUT_VALUE          (0x400U)

/*! @brief The value minimum of the timeout register */
#define FEATURE_WDT_TIMEOUT_VALUE_MIN   (0x0U)

/*! @brief The reset value of the window register */
#define FEATURE_WDT_WIN_RESET_VALUE         (0x0U)

/*! @brief The first 16-bit value used for unlocking the WDT */
#define FEATURE_WDT_UNLOCK16_FIRST_VALUE    (0xC520U)

/*! @brief The second 16-bit value used for unlocking the WDT */
#define FEATURE_WDT_UNLOCK16_SECOND_VALUE   (0xD928U)

/*! @brief The first 16-bit value used for resetting the WDT counter */
#define FEATURE_WDT_TRIGGER16_FIRST_VALUE   (0xA602U)

/*! @brief The second 16-bit value used for resetting the WDT counter */
#define FEATURE_WDT_TRIGGER16_SECOND_VALUE  (0xB480U)

/*! @brief Default reset value of the CSTS register */
#define FEATURE_WDT_CSTS_RESET_VALUE        (0x2520U)

/*******************************************************************************
 *                          CRC Module Features
 ******************************************************************************/
/*! @brief Number of instances of the CRC module */
#define CRC_INSTANCE_COUNT      (1U)

/*! @brief Array initializer of CRC peripheral base addresses */
#define CRC_BASE_ADDRS          { CRC_BASE }

/*! @brief Array initializer of CRC peripheral base pointers */
#define CRC_BASE_PTRS           { CRC }

/*! @brief Default CRC bit width */
#define FEATURE_CRC_DEFAULT_WIDTH       CRC_16BIT_WIDTH_MODE

/*! @brief Default CRC read transpose */
#define FEATURE_CRC_DEFAULT_RD_TRS      CRC_TRS_NONE

/*! @brief Default CRC write transpose */
#define FEATURE_CRC_DEFAULT_WR_TRS      CRC_TRS_NONE

/*! @brief Default polynomial 0x1021U */
#define FEATURE_CRC_DEFAULT_POLY        (0x1021U)

/*! @brief Default seed value is 0xFFFFU */
#define FEATURE_CRC_DEFAULT_SEED        (0xFFFFU)

/*******************************************************************************
 *                          DMA Module Features
 ******************************************************************************/
/*! @brief Number of instances of the DMA module */
#define DMA_INSTANCE_COUNT      (1U)

/*! @brief DMA supports the following particular transfer size: */
#define FEATURE_DMA_TRANSFER_SIZE_16B
#define FEATURE_DMA_TRANSFER_SIZE_32B

/*! @brief Number of DMA channels */
#define FEATURE_DMA_CHNS    (16U)

/*! @brief Number of DMA virtual channels */
#define FEATURE_DMA_VIRTUAL_CHANNELS (FEATURE_DMA_CHNS * DMA_INSTANCE_COUNT)

/*! @brief Number of DMA interrupt lines */
#define FEATURE_DMA_CHNS_INTERRUPT_LINES (16U)

/*! @brief Number of DMA error interrupt lines */
#define FEATURE_DMA_ERROR_INTERRUPT_LINES (1U)

/*! @brief Number of DMA virtual interrupt lines */
#define FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES ((uint32_t)FEATURE_DMA_CHNS_INTERRUPT_LINES * (uint32_t)DMA_INSTANCE_COUNT)

/*! @brief Number of DMA virtual error interrupt lines */
#define FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES ((uint32_t)FEATURE_DMA_ERROR_INTERRUPT_LINES * (uint32_t)DMA_INSTANCE_COUNT)

/*! @brief Conversion from channel index to CHPRI index */
#define FEATURE_DMA_CHN_TO_CHPRI_INDEX(x) ((x) ^ 3U)

/*! @brief DMA channel width based on number of TCDs: 2^N, N=4,5,.. */
#define FEATURE_DMA_CHN_WIDTH (4U)

/*! @brief DMA channel to instance */
#define FEATURE_DMA_VCHN_TO_INSTANCE(x)  ((x) >> (uint32_t)FEATURE_DMA_CHN_WIDTH)

/*! @brief DMA virtual channel to channel */
#define FEATURE_DMA_VCHN_TO_CHN(x)        ((x) & ((uint32_t)FEATURE_DMA_CHNS - 1U))

/*******************************************************************************
 *                          DMACHM Module Features
 ******************************************************************************/
/*! @brief Number of instances of the DMAMUX module */
#define DMACHM_INSTANCE_COUNT   (1U)

/*! @brief Number of DMA channels */
#define FEATURE_DMACHM_CHANNELS (16U)

/*! @brief Conversion from DMACHM channel to DMACHM register index */
#define FEATURE_DMACHM_CHN_TO_REG_INDEX(x) (x)

/*! @brief Conversion from DMA channel index to DMACHM channel */
#define FEATURE_DMACHM_DMA_CHN_TO_CHN(x) (x)

/*! @brief Mapping between request source and DMACHM instance */
#define FEATURE_DMACHM_REQUEST_SRC_TO_INSTANCE(x) (0U)

/*! @brief Conversion from request source to the actual DMACHM channel */
#define FEATURE_DMACHM_REQUEST_SRC_TO_CHN(x) (x)

/*******************************************************************************
 *                          CFGTMR Module Features
 ******************************************************************************/
/*! @brief Number of instances of the CFGTMR module */
#define CFGTMR_INSTANCE_COUNT                     (4U)

/*! @brief Number of PWM channels */
#define FEATURE_CFGTMR_CHANNEL_NUM              (8U)

/*! @brief Number of fault channels */
#define FEATURE_CFGTMR_FAULT_CHANNELS             (4U)

/*! @brief Width of control channel */
#define FEATURE_CFGTMR_COMBINE_CHN_CTRL_WIDTH    (8U)

/*! @brief Output channel offset */
#define FEATURE_CFGTMR_OUTPUT_CHN_OFFSET      (16U)

/*! @brief Input capture for single shot */
#define FEATURE_CFGTMR_INPUT_CAPTURE_SINGLE_SHOT  (2U)

/* @brief Dithering has supported on the generated PWM signals */
#define FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING     (0U)

/*******************************************************************************
 *                          EWDT Module Features
 ******************************************************************************/
/*! @brief First byte of the EWDT Service key */
#define FEATURE_EWDT_KEY_FIRST_BYTE         (0xB4U)

/*! @brief Second byte of the EWDT Service key */
#define FEATURE_EWDT_KEY_SECOND_BYTE        (0x2CU)

/*! @brief EWDT Compare Low register min value */
#define FEATURE_EWDT_COMPL_MIN_VALUE        (0x00U)

/*! @brief EWDT Compare High register max value */
#define FEATURE_EWDT_COMPH_MAX_VALUE        (0xFEU)

/*******************************************************************************
 *                          PDU Module Features
 ******************************************************************************/
/*! @brief PDU has instance back to back mode between PDU0 CH0 and PDU1 CH0 pre-triggers */
#define FEATURE_PDU_WITH_INSTANCE_BACKTOBACK     (1)

/*******************************************************************************
 *                          CSEC Module Features
 ******************************************************************************/
/*! @brief Number of instances of the CSE_PRAM module */
#define CSEC_PRAM_INSTANCE_COUNT     (1U)

/** Array initializer of CSE_PRAM peripheral base addresses */
#define CSEC_PRAM_BASE_ADDRS         { CSE_PRAM_BASE }

/** Array initializer of CSE_PRAM peripheral base pointers */
#define CSEC_PRAM_BASE_PTRS          { CSE_PRAM }

#define FEATURE_CSEC_ERROR_BITS_OFFSET              (0x04U)
#define FEATURE_CSEC_MAC_LENGTH_OFFSET              (0x08U)
#define FEATURE_CSEC_MESSAGE_LENGTH_OFFSET          (0x0CU)
#define FEATURE_CSEC_PAGE_LENGTH_OFFSET             (0x0EU)
#define FEATURE_CSEC_FLASH_START_ADDRESS_OFFSET     (0x10U)
#define FEATURE_CSEC_VERIFICATION_STATUS_OFFSET     (0x14U)
#define FEATURE_CSEC_BOOT_FLAVOR_OFFSET             (0x1BU)
#define FEATURE_CSEC_BOOT_SIZE_OFFSET               (0x1CU)
#define FEATURE_CSEC_SREG_OFFSET                    (0x2FU)

/*! @brief CSEC_PRAM offset of page */
#define FEATURE_CSEC_OFFSET_FOR_PAGE0      (0x00U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE1      (0x10U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE2      (0x20U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE3      (0x30U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE4      (0x40U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE5      (0x50U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE6      (0x60U)
#define FEATURE_CSEC_OFFSET_FOR_PAGE7      (0x70U)

/*******************************************************************************
 *                          ADC Module Features
 ******************************************************************************/
/*! @brief Number of instances of the ADC module */
#define ADC_INSTANCE_COUNT  (2U)

/*! @brief Array initializer of ADC peripheral base addresses */
#define ADC_BASE_ADDRS      { ADC0_BASE, ADC1_BASE }

/*! @brief Array initializer of ADC peripheral base pointers */
#define ADC_BASE_PTRS       { ADC0, ADC1 }

/*! @brief Number of interrupt vector arrays for the ADC module */
#define ADC_IRQS_ARR_COUNT  (1U)

/*! @brief Number of interrupt channels for the ADC module */
#define ADC_IRQS_CH_COUNT   (1U)

/** Interrupt vectors for the ADC peripheral type */
#define ADC_IRQS            { ADC0_IRQn, ADC1_IRQn }

/*!
 * @brief   ADC feature flag for defining number of external ADC channels
 * @details If each ADC instance has different number of external channels, then
 *          this define is set with the max value.
 */
#define FEATURE_ADC_WITH_EXTRA_NUM_REGS  (0)
#define FEATURE_ADC_EXT_CHNS_MAX_NUM   (16)
#define FEATURE_ADC_WITH_CHANNEL_2       (1)
#define FEATURE_ADC_WITH_CHANNEL_8       (1)
#define ADC_CLOCKS                      {ADC0_CLK, ADC1_CLK}
#define ADC_DEFAULT_USER_GAIN           (0x04U)
#define ADC_DEFAULT_SAMPLE_TIME         (0x0CU)
#define ADC_CLOCK_RUNTIME_MAX_FREQ      (50000000U)
#define ADC_CLOCK_RUNTIME_MIN_FREQ      (2000000U)

#if FEATURE_ADC_WITH_EXTRA_NUM_REGS
    #define ADC_CTRL_CHNS_COUNT        ADC_ACSTS1_COUNT
#else
    #define ADC_CTRL_CHNS_COUNT        ADC_CSTS1_COUNT
#endif

/**@} end of group Features_Macros*/
/**@} end of group APM32F445*/
/**@} end of group FEATURES*/

#endif /* APM32F445_FEATURES_H */
