/*!
 * @file        apm32f445_446_cfgtmr.h
 *
 * @brief       This file contains all the functions prototypes for the CFGTMR firmware library.
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

#ifndef APM32F445_446_CFGTMR_H
#define APM32F445_446_CFGTMR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"
#include "apm32f445_446_clock.h"
#include "callbacks.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup CFGTMR_Driver CFGTMR Driver
  @{
*/

/** @defgroup CFGTMR_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/

/* Channel 0 number */
#define CHAN0_IDX (0U)
/* Channel 1 number */
#define CHAN1_IDX (1U)
/* Channel 2 number */
#define CHAN2_IDX (2U)
/* Channel 3 number */
#define CHAN3_IDX (3U)
/* Channel 4 number */
#define CHAN4_IDX (4U)

/* PWM duty cycle maximum value */
#define CFGTMR_MAX_DUTY_CYCLE              (0x8000U)
/* Shift value which converts duty to ticks */
#define CFGTMR_DUTY_TO_TICKS_SHIFT         (15U)

#define CFGTMR_INSTANCE_VALIDITY(__instance__)    if(__instance__ >= CFGTMR_INSTANCE_COUNT){while(1);}

/**@} end of group CFGTMR_Macros*/

/** @defgroup CFGTMR_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief Selects mode operation in the input capture
 */
typedef enum
{
    CFGTMR_EDGE_DETECT         = 0U,    /* CFGTMR edge detect */
    CFGTMR_SIGNAL_MEASUREMENT  = 1U,    /* CFGTMR signal measurement */
    CFGTMR_NO_OPERATION        = 2U     /* CFGTMR no operation */
} CFGTMR_INPUT_OP_MODE_T;

/*!
 * @brief CFGTMR input capture measurement type for dual edge input capture
 */
typedef enum
{
    CFGTMR_NO_MEASUREMENT                  = 0x00U,    /* No measurement */
    CFGTMR_RISING_EDGE_PERIOD_MEASUREMENT  = 0x01U,    /* Period measurement between two consecutive rising edges */
    CFGTMR_FALLING_EDGE_PERIOD_MEASUREMENT = 0x02U,    /* Period measurement between two consecutive falling edges */
    CFGTMR_PERIOD_ON_MEASUREMENT           = 0x03U,    /* The time measurement taken for the pulse to remain ON or HIGH state */
    CFGTMR_PERIOD_OFF_MEASUREMENT          = 0x04U     /* The time measurement taken for the pulse to remain OFF or LOW state */
} CFGTMR_SIGNAL_MEASUREMENT_T;

/*!
 * @brief CFGTMR input capture edge mode as rising edge or falling edge
 */
typedef enum
{
    CFGTMR_NO_PIN_CONTROL          = 0x00U,       /* No trigger */
    CFGTMR_RISING_EDGE             = 0x01U,       /* Rising edge trigger */
    CFGTMR_FALLING_EDGE            = 0x02U,       /* Falling edge trigger */
    CFGTMR_BOTH_EDGES              = 0x03U        /* Rising and falling edge trigger */
} CFGTMR_EDGE_ALIGNMENT_MODE_T;

/*!
 * @brief The measurement type for input capture mode
 */
typedef enum
{
    CFGTMR_DISABLE_OPERATION           = 0x00U, /* Have no operation */
    CFGTMR_TIMESTAMP_RISING_EDGE       = 0x01U, /* Rising edge trigger */
    CFGTMR_TIMESTAMP_FALLING_EDGE      = 0x02U, /* Falling edge trigger */
    CFGTMR_TIMESTAMP_BOTH_EDGES        = 0x03U, /* Rising and falling edge trigger */
    CFGTMR_MEASURE_RISING_EDGE_PERIOD  = 0x04U, /* Period measurement between two consecutive rising edges */
    CFGTMR_MEASURE_FALLING_EDGE_PERIOD = 0x05U, /* Period measurement between two consecutive falling edges */
    CFGTMR_MEASURE_PULSE_HIGH          = 0x06U, /* The time measurement taken for the pulse to remain ON or HIGH state */
    CFGTMR_MEASURE_PULSE_LOW           = 0x07U  /* The time measurement taken for the pulse to remain OFF or LOW state */
} CFGTMR_IC_OP_MODE_T;

/*!
 * @brief CFGTMR output compare match mode
 */
typedef enum
{
    CFGTMR_DISABLE_OUTPUT    = 0x00U,    /* No action on output pin */
    CFGTMR_TOGGLE_ON_MATCH   = 0x01U,    /* Toggle on match */
    CFGTMR_CLEAR_ON_MATCH    = 0x02U,    /* Clear on match */
    CFGTMR_SET_ON_MATCH      = 0x03U     /* Set on match */
} CFGTMR_OC_MATCH_MODE_T;

/*!
 * @brief CFGTMR input capture type of the next output compare value
 */
typedef enum
{
    CFGTMR_RELATIVE_VALUE = 0x00U,  /* Next compared value is relative to current value,
                                     it will be appended to current counter value */
    CFGTMR_ABSOLUTE_VALUE = 0x01U   /* Next compared value is absolute, it will be written
                                     in counter register as it is */
} CFGTMR_OC_UPDATE_TYPE_T;

/*!
 * @brief CFGTMR Configure type of PWM update in the duty cycle or in ticks
 */
typedef enum
{
    CFGTMR_PWM_UPDATE_IN_DUTY_CYCLE = 0x00U, /* The type of PWM update in the duty cycle/pulse or also use in frequency update */
    CFGTMR_PWM_UPDATE_IN_TICKS      = 0x01U  /* The type of PWM update in ticks */
} CFGTMR_PWM_UPDATE_TYPE_T;

/*!
 * @brief The polarity of the channel output is configured in PWM signal
 */
typedef enum
{
    CFGTMR_POLARITY_LOW  = 0x00U,  /* The channel polarity is active LOW which is defined again */
    CFGTMR_POLARITY_HIGH = 0x01U   /* The channel polarity is active HIGH which is defined again */
} CFGTMR_POLARITY_T;

/*!
 * @brief CFGTMR PWM channel (n+1) polarity for combine mode
 */
typedef enum
{
    CFGTMR_MAIN_INVERTED   = 0x01U,  /* The channel (n+1) output is the inverse of the channel (n) output */
    CFGTMR_MAIN_DUPLICATED = 0x00U   /* The channel (n+1) output is the same as the channel (n) output */
} CFGTMR_SECOND_CHANNEL_POLARITY_T;

/*!
 * @brief CFGTMR fault control
 */
typedef enum
{
    CFGTMR_FAULT_CONTROL_DISABLED  = 0x00U,    /* Fault control is disabled for all channels */
    CFGTMR_FAULT_CONTROL_MAN_EVEN  = 0x01U,    /* Fault control is enabled for even channels
                                                only (channels 0, 2, 4, and 6), and the selected
                                                mode is the manual fault clearing */
    CFGTMR_FAULT_CONTROL_MAN_ALL   = 0x02U,    /* Fault control is enabled for all channels,
                                                and the selected mode is the manual fault clearing */
    CFGTMR_FAULT_CONTROL_AUTO_ALL  = 0x03U     /* Fault control is enabled for all channels, and
                                                the selected mode is the automatic fault clearing */
} CFGTMR_FAULT_MODE_T;

/*!
 * @brief Select level of the channel (n) output at the beginning
 */
typedef enum
{
    CFGTMR_LOW_STATE   = 0x00U,    /* When fault is detected PWM channel is low. */
    CFGTMR_HIGH_STATE  = 0x01U     /* When fault is detected PWM channel is high. */
} CFGTMR_SAFE_STATE_POLARITY_T;

/*!
 * @brief CFGTMR operation mode
 */
typedef enum
{
    CFGTMR_MODE_NOT_INITIALIZED                    = 0x00U,    /* The driver is not initialized */
    CFGTMR_MODE_INPUT_CAPTURE                      = 0x01U,    /* Input capture */
    CFGTMR_MODE_OUTPUT_COMPARE                     = 0x02U,    /* Output compare */
    CFGTMR_MODE_EDGE_ALIGNED_PWM                   = 0x03U,    /* Edge aligned PWM */
    CFGTMR_MODE_CEN_ALIGNED_PWM                    = 0x04U,    /* Center aligned PWM */
    CFGTMR_MODE_QUADRATURE_DECODER                 = 0x05U,    /* Quadrature decoder */
    CFGTMR_MODE_UP_TIMER                           = 0x06U,    /* Timer with up counter */
    CFGTMR_MODE_UP_DOWN_TIMER                      = 0x07U,    /* timer with up-down counter */
    CFGTMR_MODE_EDGE_ALIGNED_PWM_AND_INPUT_CAPTURE = 0x08U,    /* Edge aligned PWM and input capture */
} CFGTMR_CONFIG_MODE_T;

/*!
 * @brief CFGTMR clock source selection
 */
typedef enum
{
    CFGTMR_CLOCK_SRC_NONE           = 0x00U,    /* None use clock for CFGTMR */
    CFGTMR_CLOCK_SRC_SYSTEMCLK      = 0x01U,    /* System clock            */
    CFGTMR_CLOCK_SRC_FIXEDCLK       = 0x02U,    /* Fixed clock             */
    CFGTMR_CLOCK_SRC_EXTERNALCLK    = 0x03U     /* External clock          */
} CFGTMR_CLOCK_SRC_T;

/*!
 * @brief CFGTMR pre-scaler factor selection for the clock source.
 * In quadrature decoder mode set CFGTMR_CLOCK_DIV_1
 */
typedef enum
{
    CFGTMR_CLOCK_DIV_1    = 0x00U,    /* Divide by 1   */
    CFGTMR_CLOCK_DIV_2    = 0x01U,    /* Divide by 2   */
    CFGTMR_CLOCK_DIV_4    = 0x02U,    /* Divide by 4   */
    CFGTMR_CLOCK_DIV_8    = 0x03U,    /* Divide by 8   */
    CFGTMR_CLOCK_DIV_16   = 0x04U,    /* Divide by 16  */
    CFGTMR_CLOCK_DIV_32   = 0x05U,    /* Divide by 32  */
    CFGTMR_CLOCK_DIV_64   = 0x06U,    /* Divide by 64  */
    CFGTMR_CLOCK_DIV_128  = 0x07U     /* Divide by 128 */
} CFGTMR_CLOCK_PSC_T;

/*!
 * @brief CFGTMR interrupts source
 */
typedef enum
{
    CFGTMR_CHAN0_INT         = 0x00000001U,    /* Channel 0 interrupt */
    CFGTMR_CHAN1_INT         = 0x00000002U,    /* Channel 1 interrupt */
    CFGTMR_CHAN2_INT         = 0x00000004U,    /* Channel 2 interrupt */
    CFGTMR_CHAN3_INT         = 0x00000008U,    /* Channel 3 interrupt */
    CFGTMR_CHAN4_INT         = 0x00000010U,    /* Channel 4 interrupt */
    CFGTMR_CHAN5_INT         = 0x00000020U,    /* Channel 5 interrupt */
    CFGTMR_CHAN6_INT         = 0x00000040U,    /* Channel 6 interrupt */
    CFGTMR_CHAN7_INT         = 0x00000080U,    /* Channel 7 interrupt */
    CFGTMR_FAULT_INT         = 0x00000100U,    /* Fault interrupt */
    CFGTMR_TIME_OVERFLOW_INT = 0x00000200U,    /* Time overflow interrupt */
    CFGTMR_RELOAD_INT        = 0x00000400U     /* Reload interrupt */
} CFGTMR_INTERRUPT_T;

/*!
 * @brief CFGTMR flags type
 */
typedef enum
{
    CFGTMR_CHAN0_FLAG         = 0x00000001U,    /* Channel 0 Flag */
    CFGTMR_CHAN1_FLAG         = 0x00000002U,    /* Channel 1 Flag */
    CFGTMR_CHAN2_FLAG         = 0x00000004U,    /* Channel 2 Flag */
    CFGTMR_CHAN3_FLAG         = 0x00000008U,    /* Channel 3 Flag */
    CFGTMR_CHAN4_FLAG         = 0x00000010U,    /* Channel 4 Flag */
    CFGTMR_CHAN5_FLAG         = 0x00000020U,    /* Channel 5 Flag */
    CFGTMR_CHAN6_FLAG         = 0x00000040U,    /* Channel 6 Flag */
    CFGTMR_CHAN7_FLAG         = 0x00000080U,    /* Channel 7 Flag */
    CFGTMR_FAULT_FLAG         = 0x00000100U,    /* Fault Flag */
    CFGTMR_TIME_OVERFLOW_FLAG = 0x00000200U,    /* Time overflow Flag */
    CFGTMR_RELOAD_FLAG        = 0x00000400U,    /* Reload Flag */
    CFGTMR_CHANNEL_TRG_FLAG   = 0x00000800U     /* Channel trigger Flag */
} CFGTMR_FLAG_T;

/*!
 * @brief CFGTMR sync source
 */
typedef enum
{
    CFGTMR_SYSTEM_CLOCK    = 0U,       /* Register is updated with its buffer value at all rising
                                        edges of system clock */
    CFGTMR_PWM_SYNC        = 1U        /* Register is updated with its buffer value at the
                                        CFGTMR synchronization */
} CFGTMR_REG_UPDATE_T;

/*!
 * @brief CFGTMR update register
 */
typedef enum
{
    CFGTMR_WAIT_LOADING_POINTS = 0U,   /* CFGTMR register is updated at first loading point */
    CFGTMR_UPDATE_NOW          = 1U    /* CFGTMR register is updated immediately */
} CFGTMR_PWM_SYNC_MODE_T;

/*!
 * @brief CFGTMR pre-scaler factor for the dead-time insertion
 */
typedef enum
{
    CFGTMR_DEADTIME_DIV_1  = 0x01U, /* Divide by 1 */
    CFGTMR_DEADTIME_DIV_4  = 0x02U, /* Divide by 4 */
    CFGTMR_DEADTIME_DIV_16 = 0x03U  /* Divide by 16 */
} CFGTMR_DEADTIME_PSC_T;

/*!
 * @brief Options for the CFGTMR behavior in BDM Mode
 */
typedef enum
{
    CFGTMR_BDM_MODE_00 = 0x00U,    /* CFGTMR counter stopped, CH(n)CSTS registers CH(n)EVFLG bit can be set,
                                    CFGTMR channels in functional mode, writes to MOD,INITV and CH(n)V
                                    registers bypass the register buffers */
    CFGTMR_BDM_MODE_01 = 0x01U,    /* CFGTMR counter stopped, CH(n)CSTS registers CH(n)EVFLG is not set,
                                    CFGTMR channels outputs are forced to their safe value , writes to MOD,INITV and
                                    CH(n)V registers bypass the register buffers */
    CFGTMR_BDM_MODE_10 = 0x02U,    /* CFGTMR counter stopped, CH(n)CSTS registers CH(n)EVFLG is not set,
                                    CFGTMR channels outputs are frozen when chip enters in BDM mode, writes to MOD,
                                    INITV and CH(n)V registers bypass the register buffers */
    CFGTMR_BDM_MODE_11 = 0x03U     /* CFGTMR counter in functional mode, CH(n)CSTS registers CH(n)EVFLG can be set,
                                    CFGTMR channels in functional mode, writes to MOD,INITV and CH(n)V
                                    registers is in fully functional mode */
} CFGTMR_BDM_MODE_T;

/*!
 * @brief CFGTMR quadrature decode modes, phase encode or count and direction mode
 */
typedef enum
{
    CFGTMR_QUAD_PHASE_ENCODE   = 0x00U,   /* Phase encoding mode                 */
    CFGTMR_QUAD_COUNT_AND_DIR  = 0x01U    /* Counter and direction encoding mode */
} CFGTMR_QUAD_DECODE_MODE_T;

/*!
 * @brief CFGTMR quadrature phase polarities, normal or inverted polarity
 */
typedef enum
{
    CFGTMR_QUAD_PHASE_NORMAL = 0x00U,  /* Phase input signal is not inverted before identifying
                                        the rising and falling edges of this signal */
    CFGTMR_QUAD_PHASE_INVERT = 0x01U   /* Phase input signal is inverted before identifying
                                        the rising and falling edges of this signal */
} CFGTMR_QUAD_PHASE_POLARITY_T;

/**@} end of group CFGTMR_Enumerations*/

/** @defgroup CFGTMR_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief CFGTMR driver Input capture parameters for each channel
 */
typedef struct
{
    uint8_t hwChannelId;                         /* Physical hardware channel ID */
    CFGTMR_EDGE_ALIGNMENT_MODE_T edgeMode;         /* Edge alignment Mode for signal measurement */
    CFGTMR_INPUT_OP_MODE_T inputCaptureMode;       /* Input capture operation mode */
    CFGTMR_SIGNAL_MEASUREMENT_T measurementMode;   /* Measurement Mode for signal measurement */
    bool enableFilter;                           /* Input capture filter state */
    uint16_t filterVal;                          /* Filter Value */
    bool enableContinuousMode;                   /* Continuous measurement state */
    void *channelsCallbacksParams;               /* The parameters of callback functions for channels events */
    IC_CALLBACK_T channelsCallbacks;             /* The callback function for channels events */
} CFGTMR_INPUT_CHAN_PARAM_T;

/*!
 * @brief CFGTMR driver input capture parameters
 */
typedef struct
{
    uint8_t numOfChannels;                        /* Number of input capture channel used */
    uint16_t maxCountVal;                         /* Maximum counter value. 0x0U ~ 0xFFFFU */
    CFGTMR_INPUT_CHAN_PARAM_T *inputChnConfig;      /* Input capture channels configuration */
} CFGTMR_INPUT_PARAM_T;

/*!
 * @brief The configuration structure in timer mode
 */
typedef struct
{
    uint16_t initValue;             /* Initial counter value */
    uint16_t finalValue;            /* Final counter value */
    CFGTMR_CONFIG_MODE_T mode;        /* CFGTMR timer mode */
} CFGTMR_TIMER_PARAM_T;

/*!
 * @brief CFGTMR driver PWM parameters each channel in the output compare mode
 */
typedef struct
{
    uint8_t hwChannelId;                    /* Physical hardware channel ID */
    uint16_t cmpValue;                      /* The compared value, When the CFGTMR counter matches the value of
                                               CH(n)V registers, the channel output is changed (toggle/set/clear). */
    CFGTMR_OC_MATCH_MODE_T chnMatchMode;      /* Channel output mode in output compare mode */
    bool isExternalTrigger;                 /* true: enable the generation of a trigger is used for on-chip modules
                                               false: disable the generation of a trigger */
} CFGTMR_OUTPUT_CMP_CHAN_PARAM_T;

/*!
 * @brief CFGTMR driver PWM parameters which is configured for the list of channels
 */
typedef struct
{
    uint8_t numOfOutputChannels;                        /* Number of output compare channels */
    uint16_t maxCountVal;                               /* Maximum count value in ticks */
    CFGTMR_CONFIG_MODE_T mode;                            /* CFGTMR PWM operation mode */
    CFGTMR_OUTPUT_CMP_CHAN_PARAM_T *outputChnCfg;         /* Output compare channels configuration */
} CFGTMR_OUTPUT_CMP_PARAM_T;

/*!
 * @brief CFGTMR driver PWM Fault channel parameters
 */
typedef struct
{
    bool isFaultChannel;                    /* Enable/disable fault channel */
    bool isFaultFilter;                     /* Enable/disable fault channel filter */
    CFGTMR_POLARITY_T faultPinPolarity;       /* Channel output state on fault */
} CFGTMR_PWM_CHAN_FAULT_PARAM_T;

/*!
 * @brief CFGTMR driver PWM Fault parameter
 */
typedef struct
{
    CFGTMR_FAULT_MODE_T faultMode;            /* Fault mode */
    uint8_t faultFilterVal;                 /* Fault filter value */
    bool outputPINStateOnFault;             /* Output pin state on fault. true: safe state, false: tri-state */
    bool isPWMFaultInterrupt;               /* Enable/disable PWM fault interrupt */
    CFGTMR_PWM_CHAN_FAULT_PARAM_T faultChnParam[FEATURE_CFGTMR_FAULT_CHANNELS]; /* Fault channels configuration */
} CFGTMR_PWM_FAULT_PARAM_T;

/*!
 * @brief CFGTMR driver independent PWM parameter
 */
typedef struct
{
    uint8_t hwChannelId;                                 /* Physical hardware channel ID */
    uint16_t dutyCycle;                                  /* PWM pulse width, value should be between
                                                            0 (0%) to CFGTMR_MAX_DUTY_CYCLE (100%) */
    CFGTMR_POLARITY_T polarity;                            /* Polarity of the PWM signal generated on MCU pin. */
    CFGTMR_SAFE_STATE_POLARITY_T safeState;                /* Logical state of the PWM channel n when an fault is detected
                                                            and to set up the polarity of PWM signal on the channel (n+1) */
    bool enableSecondChnOutput;                          /* Enable complementary mode on next channel */
    CFGTMR_SECOND_CHANNEL_POLARITY_T secondChnPolarity;    /* Polarity of the channel n+1 relative to channel n in the complementary mode */
    bool enableDeadTime;                                 /* Enable/disable dead time for channel */
    bool isExternalTrigger;                              /* true: enable the generation of a trigger is used for on-chip modules
                                                            false: disable the generation of a trigger */
} CFGTMR_INDEPENDENT_CHAN_PARAM_T;

/*!
 * @brief CFGTMR driver combined PWM parameter
 */
typedef struct
{
    uint8_t hwChannelId;                                 /* Physical hardware channel ID for channel (n) */
    bool enableModifiedCombine;                          /* Enable/disable the modified combine mode for channels (n) and (n+1) */
    uint16_t firstEdge;                                  /* First edge time. This time is relative to signal period. The value for this parameter is
                                                            between 0 and CFGTMR_MAX_DUTY_CYCLE(0 = 0% from period and CFGTMR_MAX_DUTY_CYCLE = 100% from period) */
    uint16_t secondEdge;                                 /* Second edge time. This time is relative to signal period. The value for this parameter is
                                                            between 0 and CFGTMR_MAX_DUTY_CYCLE(0 = 0% from period and CFGTMR_MAX_DUTY_CYCLE = 100% from period) */
    CFGTMR_POLARITY_T mainChnPolarity;                     /* Polarity of the PWM signal generated on MCU pin for channel n.*/
    bool enableSecondChnOutput;                          /* Select if channel (n+1)  output is enabled/disabled for the complementary mode */
    CFGTMR_SECOND_CHANNEL_POLARITY_T secondChnPolarity;    /* Select channel (n+1) polarity relative to channel (n) in the complementary mode */
    bool isExternalTrigger;                              /* The generation of the channel (n) trigger
                                                            true: enable the generation of a trigger on the channel (n)
                                                            false: disable the generation of a trigger on the channel (n) */
    bool isExternalTriggerOnNextChn;                     /* The generation of the channel (n+1) trigger
                                                            true: enable the generation of a trigger on the channel (n+1)
                                                            false: disable the generation of a trigger on the channel (n+1) */
    CFGTMR_SAFE_STATE_POLARITY_T mainChnSafeState;         /* The selection of the channel (n) state when fault is detected */
    CFGTMR_SAFE_STATE_POLARITY_T secondChnSafeState;       /* The selection of the channel (n+1) state when fault is detected
                                                            and set up the polarity of PWM signal on the channel (n+1) */
    bool enableDeadTime;                                 /* Enable/disable dead time for channel */
} CFGTMR_COMBINED_CHAN_PARAM_T;

/*!
 * @brief CFGTMR driver PWM parameters
 */
typedef struct
{
    uint8_t numOfIndependentPwmChns;                            /* Number of independent PWM channels */
    uint8_t numOfCombinedPwmChns;                               /* Number of combined PWM channels */
    CFGTMR_CONFIG_MODE_T mode;                                    /* CFGTMR mode */
    uint32_t uFreqHZ;                                           /* PWM period in Hz */
    uint8_t deadTimeVal;                                        /* Dead time value in [ticks] */
    CFGTMR_DEADTIME_PSC_T deadTimePsc;                            /* Dead time pre-scaler value[ticks] */
    CFGTMR_INDEPENDENT_CHAN_PARAM_T *pwmIndependentChnCfg;        /* Configuration for independent PWM channels */
    CFGTMR_COMBINED_CHAN_PARAM_T *pwmCombinedChnCfg;              /* Configuration for combined PWM channels */
    CFGTMR_PWM_FAULT_PARAM_T *pwmFaultCfg;                        /* Configuration for PWM fault */
} CFGTMR_PWM_PARAM_T;

/*!
 * @brief CFGTMR quadrature decoder channel parameters
 */
typedef struct
{
    uint8_t phaseFilterVal;                         /* Filter value (if input filter is enabled)*/
    bool isPhaseInputFilter;                        /* Enable/Disable phase filter */
    CFGTMR_QUAD_PHASE_POLARITY_T phasePolarity;       /* Phase polarity */
} CFGTMR_PHASE_PARAMS_T;

/*!
 * @brief CFGTMR quadrature configure structure
 */
typedef struct
{
    uint16_t initVal;                   /* Counter initial value */
    uint16_t maxVal;                    /* Counter maximum value */
    CFGTMR_QUAD_DECODE_MODE_T mode;       /* Quadrature decode modes */
    CFGTMR_PHASE_PARAMS_T phaseACfg;      /* Input phase A parameter config */
    CFGTMR_PHASE_PARAMS_T phaseBCfg;      /* Input phase B parameter config */
} CFGTMR_QUAD_DECODE_CONFIG_T;

/*!
 * @brief CFGTMR quadrature state(counter value and flags)
 */
typedef struct
{
    uint16_t counterVal;        /* Counter value */
    bool overflowFlag;          /* True: if overflow occurred,
                                   False: if overflow doesn't occurred */
    bool counterDir;            /* True: CFGTMR counter is increasing,
                                   False: CFGTMR counter is decreasing */
    bool overflowDir;           /* True: if overflow occurred at maximum value,
                                   False: if overflow occurred at minimum value */
} CFGTMR_QUAD_DECODE_STATE_T;

/*!
 * @brief CFGTMR state structure of the driver
 */
typedef struct
{
    CFGTMR_CONFIG_MODE_T cfgtmrMode;                                  /* Mode of operation for CFGTMR */
    CFGTMR_CLOCK_SRC_T cfgtmrClkSrc;                                  /* Clock source used by CFGTMR counter */
    uint32_t cfgtmrClkSrcFreq;                                      /* The clock frequency is used for counting */
    uint16_t cfgtmrModVal;                                          /* This field is used only in input capture mode to store MOD value */
    uint16_t cfgtmrPeriod;                                          /* This field is used only in PWM mode to store signal period */
    uint16_t measurementResults[FEATURE_CFGTMR_CHANNEL_NUM];      /* This field is used only in input capture mode to store edges time stamps */
    bool enableNotification[FEATURE_CFGTMR_CHANNEL_NUM];          /* To save channels enable the notification on the callback application */
    void *channelsCallbacksParams[FEATURE_CFGTMR_CHANNEL_NUM];    /* The parameters of callback function for channels events */
    IC_CALLBACK_T channelsCallbacks[FEATURE_CFGTMR_CHANNEL_NUM];  /* The callback function for channels events */
} CFGTMR_STATE_T;

/*!
 * @brief CFGTMR Registers sync parameters
 * Please don't use software and hardware trigger simultaneously
 */
typedef struct
{
    bool softSync;                      /* True - enable software sync, False - disable software sync */
    bool hwSync0;                       /* True - enable hardware 0 sync, False - disable hardware 0 sync */
    bool hwSync1;                       /* True - enable hardware 1 sync, False - disable hardware 1 sync */
    bool hwSync2;                       /* True - enable hardware 2 sync, False - disable hardware 2 sync */
    bool maxLoadingPoint;               /* True - enable maximum loading point, False - disable maximum loading point */
    bool minLoadingPoint;               /* True - enable minimum loading point, False - disable minimum loading point */
    CFGTMR_REG_UPDATE_T initCounterSync;  /* Configures INITV sync */
    CFGTMR_REG_UPDATE_T inverterSync;     /* Configures INVCTRL sync */
    CFGTMR_REG_UPDATE_T maskRegSync;      /* Configures OMASK sync */
    CFGTMR_REG_UPDATE_T outRegSync;       /* Configures SWOCTRL sync */
    CFGTMR_PWM_SYNC_MODE_T syncPoint;     /* Configure synchronization method (waiting next loading point or immediate) */
    bool autoClrTrigger;                /* Available only for hardware trigger */
} CFGTMR_PWM_SYNC_T;

/*!
 * @brief Configuration structure that the user needs to set
 */
typedef struct
{
    CFGTMR_PWM_SYNC_T syncMethod;           /* Register sync options available in the CFGTMR_PWM_SYNC_T enumeration */
    CFGTMR_CONFIG_MODE_T cfgtmrMode;          /* Mode of operation for CFGTMR */
    CFGTMR_BDM_MODE_T BDMMode;              /* Select CFGTMR behavior in BDM mode */
    CFGTMR_CLOCK_PSC_T cfgtmrPsc;             /* Register pre-scaler options available in the CFGTMR_CLOCK_PSC_T enumeration */
    CFGTMR_CLOCK_SRC_T cfgtmrClkSrc;          /* Select clock source for CFGTMR */
    bool isEnableInitTrg;                 /* true: enable the generation of initialization trigger
                                             false: disable the generation of initialization trigger */
    bool isEnableInterrupt;               /* true: enable interrupt, false: write interrupt is disabled */
} CFGTMR_USER_CONFIG_T;

/**@} end of group CFGTMR_Structures*/

/** @defgroup CFGTMR_Functions Functions
  @{
*/

/*******************************************************************************
 *                          CFGTMR COMMON PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGTMR_Init(uint32_t ins, const CFGTMR_USER_CONFIG_T *userconfig, CFGTMR_STATE_T *cfgtmrState);
STATUS_T CFGTMR_Deinit(uint32_t ins);
void CFGTMR_DefaultConfig(CFGTMR_USER_CONFIG_T * const cfg);
STATUS_T CFGTMR_MaskOutputChannels(uint32_t ins, uint32_t channelsMask, bool softTrigger);
STATUS_T CFGTMR_ConfigCounterInitailValue(uint32_t ins, uint16_t counterVal, bool softTrigger);
STATUS_T CFGTMR_ConfigHalfCycleReloadPoint(uint32_t ins, uint16_t reloadPoint, bool softTrigger);
STATUS_T CFGTMR_ConfigSoftOutChnValue(uint32_t ins, uint8_t channelsValues, bool softTrigger);
STATUS_T CFGTMR_ConfigSoftwareOutputChannelControl(uint32_t ins, uint8_t channelsMask, bool softTrigger);

STATUS_T CFGTMR_ConfigAllChnSoftwareOutputControl(
    uint32_t ins,
    uint8_t chnMask,
    uint8_t chnValueMask,
    bool softTrigger);

STATUS_T CFGTMR_ConfigInvertingControl(uint32_t ins, uint8_t channelsPairMask, bool softTrigger);
STATUS_T CFGTMR_ConfigModuloCounterValue(uint32_t ins, uint16_t counterVal, bool softTrigger);
STATUS_T CFGTMR_ConfigOutputlevel(uint32_t ins, uint8_t channel, uint8_t level);
STATUS_T CFGTMR_ConfigSync(uint32_t ins, const CFGTMR_PWM_SYNC_T *pwmParam);
STATUS_T CFGTMR_EnableHardwareTrigger(uint32_t ins);
STATUS_T CFGTMR_EnableInterrupts(uint32_t ins, uint32_t interrupts);
void CFGTMR_DisableInterrupts(uint32_t ins, uint32_t interrupts);
uint32_t CFGTMR_ReadEnabledInterrupts(uint32_t ins);
uint32_t CFGTMR_ReadStatusFlags(uint32_t ins);
void CFGTMR_ClearStatusFlags(uint32_t ins, uint32_t flg);
uint32_t CFGTMR_ReadFrequency(uint32_t ins);
uint16_t CFGTMR_ConvertFreqToPeriodTicks(uint32_t ins, uint32_t freqHz);
STATUS_T CFGTMR_ResetCounter(uint32_t ins, bool softTrigger);

STATUS_T CFGTMR_InitCounter(uint32_t ins, const CFGTMR_TIMER_PARAM_T *timer);
STATUS_T CFGTMR_StartCounter(uint32_t ins);
STATUS_T CFGTMR_StopCounter(uint32_t ins);
uint32_t CFGTMR_ReadCounter(uint32_t ins);
void CFGTMR_ReadTimerDefaultConfig(CFGTMR_TIMER_PARAM_T * const cfg);

/*******************************************************************************
 *                          CFGTMR PWM PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGTMR_InitPWM(uint32_t ins, const CFGTMR_PWM_PARAM_T *pwmParam);
STATUS_T CFGTMR_DeInitPWM(uint32_t ins);

STATUS_T CFGTMR_UpdatePwmChannel(
    uint32_t ins,
    uint8_t channelID,
    CFGTMR_PWM_UPDATE_TYPE_T updateType,
    uint16_t firstEdge,
    uint16_t secondEdge,
    bool softTrigger);

STATUS_T CFGTMR_UpdatePwmPeriod(
    uint32_t ins,
    CFGTMR_PWM_UPDATE_TYPE_T updateType,
    uint32_t newValue,
    bool softTrigger);

STATUS_T CFGTMR_UpdatePwmMultipleChannels(
    uint32_t ins,
    uint8_t numOfChannels,
    const uint8_t *channelIDArray,
    const uint16_t *dutyArray,
    bool softTrigger);

STATUS_T CFGTMR_ControlChannelOutput(uint32_t ins, uint8_t channel, bool enable);

#if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
STATUS_T CFGTMR_UpdatePwmPeriodDither(uint32_t ins, uint8_t newMMVVal, bool isSoftwareTrig);
STATUS_T CFGTMR_UpdatePwmEdgeChannelDither(uint32_t ins, uint8_t channel, uint8_t newMMVVal, bool isSoftwareTrig);
#endif

void CFGTMR_PWM_IRQHandler(uint32_t ins, uint32_t chnOutCtrlVal);

/*******************************************************************************
 *                          CFGTMR OUTPUT COMPARE PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGTMR_InitOutputCompare(uint32_t ins, const CFGTMR_OUTPUT_CMP_PARAM_T *param);
STATUS_T CFGTMR_DeinitOutputCompare(uint32_t ins, const CFGTMR_OUTPUT_CMP_PARAM_T *param);

STATUS_T CFGTMR_UpdateOutputCompareChannel(
    uint32_t ins,
    uint8_t channelID,
    uint16_t nextCmpMatchValue,
    CFGTMR_OC_UPDATE_TYPE_T updateType,
    bool softTrigger);

/*******************************************************************************
 *                          CFGTMR INPUT CAPTURE PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGTMR_InitInputCapture(uint32_t ins, const CFGTMR_INPUT_PARAM_T *param);
STATUS_T CFGTMR_DeinitInputCapture(uint32_t ins, const CFGTMR_INPUT_PARAM_T *param);
uint16_t CFGTMR_ReadInputCaptureMeasurement(uint32_t ins, uint8_t channelNum);
STATUS_T CFGTMR_StartNewSignalMeasurement(uint32_t ins, uint8_t channelNum);

STATUS_T CFGTMR_ConfigChannelInputCaptureMode(
    uint32_t ins,
    uint8_t channelNum,
    CFGTMR_IC_OP_MODE_T inputCaptureMode,
    bool enableContinuousCapture);

/*******************************************************************************
 *                          CFGTMR Quadrature Decoder PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T CFGTMR_ConfigQuadDecodeStart(uint32_t ins, const CFGTMR_QUAD_DECODE_CONFIG_T *cfg);
STATUS_T CFGTMR_ConfigQuadDecodeStop(uint32_t ins);
CFGTMR_QUAD_DECODE_STATE_T CFGTMR_ReadQuadDecoderStatus(uint32_t ins);
void CFGTMR_ReadQuadDecoderDefaultConfig(CFGTMR_QUAD_DECODE_CONFIG_T * const cfg);


/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

void CFGTMR_HW_ConfigHalfCycleValue(CFGTMR_T *const cfgtmrBase, uint16_t value);
void CFGTMR_HW_ConfigClockSource(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_SRC_T clock);
void CFGTMR_HW_ConfigClockPs(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_PSC_T ps);
uint8_t CFGTMR_HW_ReadClockPs(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigTimerOverflowInt(CFGTMR_T *const cfgtmrBase, bool state);
void CFGTMR_HW_EnablePwmChannelOutputs(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_DisablePwmChannelOutputs(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ClearTimerOverflow(CFGTMR_T *const cfgtmrBase);
bool CFGTMR_HW_ReadOverflowIntStatus(const CFGTMR_T *cfgtmrBase);
bool CFGTMR_HW_ReadTimerOverflowedStatus(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigCpwmsMode(CFGTMR_T *const cfgtmrBase, bool mode);
bool CFGTMR_HW_ReadCpwmsMode(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigReIntCmd(CFGTMR_T *const cfgtmrBase, bool enable);
bool CFGTMR_HW_ReadReloadFlag(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ClearReloadFlag(CFGTMR_T *const cfgtmrBase);
bool CFGTMR_HW_ReadReloadIntStatus(const CFGTMR_T *cfgtmrBase);
uint8_t CFGTMR_HW_ReadClockSource(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigCounter(CFGTMR_T *const cfgtmrBase, uint16_t value);
void CFGTMR_HW_ConfigMod(CFGTMR_T *const cfgtmrBase, uint16_t value);
void CFGTMR_HW_InitCounterValue(CFGTMR_T *const cfgtmrBase, uint16_t value);
void CFGTMR_HW_ConfigChnMSnBAMode(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t selection);
void CFGTMR_HW_ConfigChnEdgeLevel(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t level);
void CFGTMR_HW_EnableChnInt(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_DisableChnInt(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ClearChnEventFlag(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigChnCountVal(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint16_t value);
bool CFGTMR_HW_ReadChnIntStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
bool CFGTMR_HW_ReadChnEventHasOccurred(const CFGTMR_T *cfgtmrBase, uint8_t channel);
uint8_t CFGTMR_HW_ReadChnMode(const CFGTMR_T *cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigOutmaskReg(CFGTMR_T *const cfgtmrBase, uint32_t regVal);
void CFGTMR_HW_ConfigChnOutputPolarityCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool polarity);
void CFGTMR_HW_ConfigChnFaultInputPolarityCmd(CFGTMR_T *const cfgtmrBase, uint8_t faultChannelID, bool polarity);
void CFGTMR_HW_ConfigFaultInt(CFGTMR_T *const cfgtmrBase, bool state);
bool CFGTMR_HW_ReadFaultIntStatus(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ClearFaultsIsr(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigFaultControlMode(CFGTMR_T *const cfgtmrBase, uint32_t mode);
void CFGTMR_HW_InitChnOutputCmd(CFGTMR_T *const cfgtmrBase, bool enable);
bool CFGTMR_HW_ReadDetectedFaultInput(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigWriteProtectionCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_Enable(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigPwmSyncMode(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigSoftwareTriggerCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigHardwareSyncTriggerSrc(CFGTMR_T *const cfgtmrBase, uint8_t trigger_num, bool enable);
void CFGTMR_HW_ConfigOutmaskPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigMaxLoadingCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigMinLoadingCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigDualChnMofCombineCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigDualChnFaultCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigDualChnPwmSyncCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigDualChnDeadtimeCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigDualChnDecapCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigDualEdgeCaptureCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
bool CFGTMR_HW_ReadDualEdgeCaptureBit(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum);
void CFGTMR_HW_ConfigDualChnCompCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool polarity);
void CFGTMR_HW_ConfigDualChnCombineCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
bool CFGTMR_HW_ReadDualChnCombineCmd(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum);
bool CFGTMR_HW_ReadDualChnMofCombineCmd(const CFGTMR_T *cfgtmrBase, uint8_t chnlPairNum);
void CFGTMR_HW_ConfigExtDeadtimeValue(CFGTMR_T *const cfgtmrBase, uint8_t value);
void CFGTMR_HW_ConfigDeadtimePrescale(CFGTMR_T *const cfgtmrBase, CFGTMR_DEADTIME_PSC_T divider);
void CFGTMR_HW_ConfigDeadtimeCount(CFGTMR_T *const cfgtmrBase, uint8_t count);
void CFGTMR_HW_ConfigInitTriggerCmd(CFGTMR_T *const cfgtmrBase, bool enable);
bool CFGTMR_HW_ReadChnTriggerGenerated(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ClearChnTriggerFlag(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigQuadDecoderCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigQuadPhaseAFilterCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigQuadPhaseAPolarity(CFGTMR_T *const cfgtmrBase, uint8_t mode);
void CFGTMR_HW_ConfigQuadPhaseBPolarity(CFGTMR_T *const cfgtmrBase, uint8_t mode);
void CFGTMR_HW_ConfigQuadMode(CFGTMR_T *const cfgtmrBase, uint8_t quadMode);
bool CFGTMR_HW_ReadQuadDir(const CFGTMR_T *cfgtmrBase);
bool CFGTMR_HW_ReadQuadTimerOverflowDir(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigQuadPhaseBFilterCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigFaultInputFilterVal(CFGTMR_T *const cfgtmrBase, uint32_t value);
void CFGTMR_HW_ConfigFaultInputFilterCmd(CFGTMR_T *const cfgtmrBase, uint8_t inputNum, bool enable);
void CFGTMR_HW_ClearFaultControl(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigFaultInputCmd(CFGTMR_T *const cfgtmrBase, uint8_t inputNum, bool enable);
void CFGTMR_HW_ConfigPwmFaultBehavior(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigInvctrlReg(CFGTMR_T *const cfgtmrBase, uint32_t regVal);
void CFGTMR_HW_ConfigAllChnSoftwareCtrlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channelsCfg);
void CFGTMR_HW_ConfigAllChnSoftwareCtrlVal(CFGTMR_T *const cfgtmrBase, uint8_t channelsValues);
void CFGTMR_HW_ConfigSoftwareOutputControl(CFGTMR_T *const cfgtmrBase, uint16_t value);
void CFGTMR_HW_ConfigBdmMode(CFGTMR_T *const cfgtmrBase, CFGTMR_BDM_MODE_T val);
void CFGTMR_HW_ConfigSwoctrlHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigInvctrlHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigOutmaskHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigModCntinCvHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigCounterHardwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigSwoctrlSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigInvctrlSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigOutmaskSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigModCntinCvSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigHwTriggerSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigCounterSoftwareSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_PWM_SYNC_MODE_T update_mode);
void CFGTMR_HW_ConfigPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, bool mode);
void CFGTMR_HW_ConfigSwoctrlPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode);
void CFGTMR_HW_ConfigInvctrlPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode);
void CFGTMR_HW_ConfigCntinPwmSyncModeCmd(CFGTMR_T *const cfgtmrBase, CFGTMR_REG_UPDATE_T mode);
#if FEATURE_CFGTMR_WITH_SUPPORTED_DITHERING
void CFGTMR_HW_ConfigMMVVal(CFGTMR_T * const cfgtmrBase, uint8_t value);
void CFGTMR_HW_ConfigMCHMVal(CFGTMR_T * const cfgtmrBase, uint8_t channel, uint8_t value);
#endif
void CFGTMR_HW_ConfigSyncBit(SIM_T * const simBase, uint32_t ins, bool enable);
void CFGTMR_HW_ConfigClockFilterPs(CFGTMR_T *const cfgtmrBase, uint8_t filterPrescale);
uint8_t CFGTMR_HW_ReadClockFilterPs(const CFGTMR_T *cfgtmrBase);
uint16_t CFGTMR_HW_ReadCounterValue(const CFGTMR_T *cfgtmrBase);
uint16_t CFGTMR_HW_ReadMod(const CFGTMR_T *cfgtmrBase);
uint16_t CFGTMR_HW_ReadCounterInitVal(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ClearChSC(CFGTMR_T *const cfgtmrBase, uint8_t channel);
uint8_t CFGTMR_HW_ReadChnEdgeLevel(const CFGTMR_T *cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigChnIcrstCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
bool CFGTMR_HW_ReadChnIcrstStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigChnDmaCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
bool CFGTMR_HW_ReadChnDmaStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigTrigModeControlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
bool CFGTMR_HW_ReadTriggerControled(const CFGTMR_T *cfgtmrBase, uint8_t channel);
bool CFGTMR_HW_ReadChInputStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
bool CFGTMR_HW_ReadChOutputValue(const CFGTMR_T *cfgtmrBase, uint8_t channel);
uint16_t CFGTMR_HW_ReadChnCountVal(const CFGTMR_T *cfgtmrBase, uint8_t channel);
bool CFGTMR_HW_ReadChnEventStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
uint32_t CFGTMR_HW_ReadEventStatus(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ClearChnEventStatus(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigChnOutputMask(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool mask);
void CFGTMR_HW_ConfigChnOutputInitStateCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool state);
void CFGTMR_HW_DisableFaultInt(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigCaptureTestCmd(CFGTMR_T *const cfgtmrBase, bool enable);
bool CFGTMR_HW_ReadCFGtmrEnableStatus(const CFGTMR_T *cfgtmrBase);
void CFGTMR_HW_ConfigCountReinitSyncCmd(CFGTMR_T *const cfgtmrBase, bool enable);
bool CFGTMR_HW_ReadWriteProtectionEnabledStatus(const CFGTMR_T *cfgtmrBase);
bool CFGTMR_HW_ReadFaultInputEnabledStatus(const CFGTMR_T *cfgtmrBase);
bool CFGTMR_HW_ReadFaultFlagDetectedStatus(const CFGTMR_T *cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ClearFaultFlagDetectedStatus(CFGTMR_T *const cfgtmrBase, uint8_t channel);
void CFGTMR_HW_ConfigDualChnInvertCmd(CFGTMR_T *const cfgtmrBase, uint8_t chnlPairNum, bool enable);
void CFGTMR_HW_ConfigChnSoftwareCtrlCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
void CFGTMR_HW_ConfigChnSoftwareCtrlVal(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
void CFGTMR_HW_EnableGlobalLoadCmd(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigLoadCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigHalfCycleCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigPwmLoadCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigPwmLoadChnSelCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
void CFGTMR_HW_ConfigInitTrigOnReloadCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigGlobalTimeBaseOutputCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigGlobalTimeBaseCmd(CFGTMR_T *const cfgtmrBase, bool enable);
void CFGTMR_HW_ConfigLoadFreq(CFGTMR_T *const cfgtmrBase, uint8_t val);
void CFGTMR_HW_ConfigExtPairDeadtimeValue(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, uint8_t value);
void CFGTMR_HW_ConfigPairDeadtimePrescale(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, CFGTMR_DEADTIME_PSC_T divider);
void CFGTMR_HW_ConfigPairDeadtimeCount(CFGTMR_T *const cfgtmrBase, uint8_t channelPair, uint8_t count);
void CFGTMR_HW_ConfigModule(CFGTMR_T *const cfgtmrBase, CFGTMR_CLOCK_PSC_T ftmClockPrescaler);
void CFGTMR_HW_Reset(CFGTMR_T *const cfgtmrBase);
void CFGTMR_HW_ConfigChnTriggerCmd(CFGTMR_T *const cfgtmrBase, uint8_t channel, bool enable);
void CFGTMR_HW_ConfigChnInputCaptureFilter(CFGTMR_T *const cfgtmrBase, uint8_t channel, uint8_t value);

/**@} end of group CFGTMR_Functions*/
/**@} end of group CFGTMR_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* APM32F445_446_CFGTMR_H */
