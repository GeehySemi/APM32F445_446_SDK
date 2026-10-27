/*!
 * @file        apm32f445_446_rtc.h
 *
 * @brief       Header of apm32f445_446_rtc.c
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

#ifndef APM32F445_446_RTC_H
#define APM32F445_446_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "device.h"
#include "apm32f445_446_interrupt.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup RTC_Driver RTC Driver
  @{
*/

/** @defgroup RTC_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
#define SECONDS_IN_A_DAY     (86400UL)
#define SECONDS_IN_A_HOUR    (3600U)
#define SECONDS_IN_A_MIN     (60U)
#define MINS_IN_A_HOUR       (60U)
#define HOURS_IN_A_DAY       (24U)
#define DAYS_IN_A_YEAR       (365U)
#define DAYS_IN_A_LEAP_YEAR  (366U)
#define YEAR_RANGE_START     (1970U)
#define YEAR_RANGE_END       (2099U)

/**@} end of group RTC_Macros*/

/** @defgroup RTC_Enumerations Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief RTC seconds interrupt configuration
 */
typedef enum
{
    RTC_SECOND_INT_128HZ = 0x07U,   /* RTC seconds interrupt occurs at 128 Hz */
    RTC_SECOND_INT_64HZ  = 0x06U,  /* RTC seconds interrupt occurs at 64 Hz  */
    RTC_SECOND_INT_32HZ  = 0x05U,  /* RTC seconds interrupt occurs at 32 Hz  */
    RTC_SECOND_INT_16HZ  = 0x04U,  /* RTC seconds interrupt occurs at 16 Hz  */
    RTC_SECOND_INT_8HZ   = 0x03U,  /* RTC seconds interrupt occurs at 8 Hz   */
    RTC_SECOND_INT_4HZ   = 0x02U,  /* RTC seconds interrupt occurs at 4 Hz   */
    RTC_SECOND_INT_2HZ   = 0x01U,  /* RTC seconds interrupt occurs at 2 Hz   */
    RTC_SECOND_INT_1HZ   = 0x00U  /* RTC seconds interrupt occurs at 1 Hz   */
} RTC_SECOND_INT_CFG_T;

/*!
 * @brief RTC clkout pin configuration
 */
typedef enum
{
    RTC_CLKOUT_SRC_32KHZ = 0x02U,   /* Output on RTC_CLKOUT of the 32KHz clock                      */
    RTC_CLKOUT_SRC_TSIC  = 0x01U,  /* Output on RTC_CLKOUT as configured on Time seconds interrupt */
    RTC_CLKOUT_DISABLED  = 0x00U  /* Clock out pin is disabled                                    */
} RTC_CLKOUT_CFG_T;

/*!
 * @brief RTC clock source select
 */
typedef enum
{
    RTC_CLK_SRC_LPO_1KHZ  = 0x01U,  /* RTC Prescaler increments using 1KHz LPO        */
    RTC_CLK_SRC_OSC_32KHZ = 0x00U /* RTC Prescaler increments using 32 KHz crystal  */
} RTC_CLK_SRC_SEL_T;

/*!
 * @brief RTC register lock
 */
typedef enum
{
    RTC_LOCK_REG_TCL    = 0x03U,    /* RTC Time Compensation Reg lock */
    RTC_LOCK_REG_CTRL   = 0x02U,   /* RTC Control Register lock      */
    RTC_LOCK_REG_STATUS = 0x01U,   /* RTC Status Register lock       */
    RTC_LOCK_REG_LOCK   = 0x00U   /* RTC Lock Register lock         */
} RTC_LOCK_REG_SEL_T;

/**@} end of group RTC_Enumerations*/

/** @defgroup RTC_Structures Structures
  @{
*/

/*******************************************************************************
 *                              STRUCTS
 ******************************************************************************/

/*!
 * @brief RTC timeDate structure
 */
typedef struct
{
    uint8_t seconds;    /* Seconds */
    uint16_t minutes;   /* Minutes */
    uint16_t hour;      /* Hour    */
    uint16_t day;       /* Day     */
    uint16_t month;     /* Month   */
    uint16_t year;      /* Year    */
} RTC_TIMEDATE_T;

/*!
 * @brief RTC init structure
 */
typedef struct
{
    RTC_CLK_SRC_SEL_T clockSelect;                /* RTC clock select                                                                  */
    RTC_CLKOUT_CFG_T  clockOutConfig;             /* RTC clock out source                                                              */
    uint8_t           compensationInterval;       /* Compensation interval                                                             */
    int8_t            compensation;               /* Compensation value                                                                */
    bool              nonSupervisorAccessEnable;  /* Enable writes to the registers in non supervisor mode                             */
    bool              updateEnable;               /* Enable changing the time counter enable bit even if the status register is locked */
} RTC_INIT_CONFIG_T;

/*!
 * @brief RTC alarm configuration
 */
typedef struct
{
    uint32_t       repetitionInterval;                    /* Interval of repetition in sec                */
    RTC_TIMEDATE_T alarmTime;                             /* Alarm timeDate                               */
    bool           alarmIntEnable;                        /* Enable alarm interrupt                       */
    uint32_t       numberOfRepeats;                       /* Number of alarm repeats                      */
    bool           repeatForever;                         /* Repeat forever, and ignore number of repeats */
    void           *callbackParams;                       /* Pointer to the callback parameters.          */
    void           (*alarmCallback)(void *callbackParam); /* Pointer to the user callback method.         */
} RTC_ALARM_CONFIG_T;

/*!
 * @brief RTC interrupt configuration. It is used to configure interrupt
 *        other than Time Alarm and Time Seconds interrupt
 */
typedef struct
{
    bool timeInvalidIntEnable;                /* Enable time invalid interrupt        */
    bool overflowIntEnable;                   /* Enable time overflow interrupt       */
    void *callbackParams;                     /* Pointer to the callback parameters.  */
    void (*rtcCallback)(void *callbackParam); /* Pointer to the user callback method. */
} RTC_INTERRUPT_CONFIG_T;

/*!
 * @brief RTC Seconds Interrupt Configuration
 */
typedef struct
{
    bool                 secondIntEnable;                            /* Seconds interrupt enable            */
    RTC_SECOND_INT_CFG_T secondIntCfg;                               /* Seconds interrupt frequency         */
    void                 *secondsCallbackParams;                     /* Pointer to the callback parameters  */
    void                 (*rtcSecondsCallback)(void *callbackParam); /* Pointer to the user callback method */
} RTC_SECONDS_INT_CONFIG_T;

/*!
 * @brief RTC Register Lock Configuration
 */
typedef struct
{
    bool ctrlRegLock;                  /* Lock state of the Control Register           */
    bool statusRegLock;                /* Lock state of the Status Register            */
    bool lockRegLock;                  /* Lock state of the Lock Register              */
    bool timeCompensationRegisterLock; /* Lock state of the Time Compensation Register */
} RTC_REGISTER_LOCK_CONFIG_T;

/**@} end of group RTC_Structures*/

/** @defgroup RTC_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

STATUS_T RTC_DeInit(uint32_t ins);
void RTC_DefaultConfig(RTC_INIT_CONFIG_T *const rtcInitConfig);
STATUS_T RTC_Init(uint32_t ins, const RTC_INIT_CONFIG_T *const rtcInitConfig);

STATUS_T RTC_StartCounter(uint32_t ins);
STATUS_T RTC_StopCounter(uint32_t ins);

STATUS_T RTC_ConfigTimeCompensation(uint32_t ins, uint8_t compInterval, int8_t compensation);
void RTC_ReadTimeCompensation(uint32_t ins, uint8_t *compInterval, int8_t *compensation);

bool RTC_CanWriteTCE(uint32_t ins);

STATUS_T RTC_AssignTimeDate(uint32_t ins, const RTC_TIMEDATE_T *const timeDate);
STATUS_T RTC_ReadCurrentTimeDate(uint32_t ins, RTC_TIMEDATE_T *const currentTime);

bool RTC_TimeDateCorrectFormatVerify(const RTC_TIMEDATE_T *const timeDate);
bool RTC_YearLeapVerify(uint16_t year);

void RTC_ConvertSecondsToTimeDate(const uint32_t *seconds, RTC_TIMEDATE_T *const timeDate);
void RTC_ConvertTimeDateToSeconds(const RTC_TIMEDATE_T *const timeDate, uint32_t *const seconds);

STATUS_T RTC_AlarmConfig(uint32_t ins, RTC_ALARM_CONFIG_T *const alarmConfig);
void RTC_ReadAlarmConfig(uint32_t ins, RTC_ALARM_CONFIG_T *alarmConfig);
bool RTC_AlarmPendingVerify(uint32_t ins);
STATUS_T RTC_ReadNextAlarmTime(uint32_t ins,RTC_TIMEDATE_T *const alarmTime);

STATUS_T RTC_ConfigClockOut(RTC_T *const rtcBase, RTC_CLKOUT_CFG_T clkOutConfig);

STATUS_T RTC_ConfigRegisterLock(uint32_t ins, const RTC_REGISTER_LOCK_CONFIG_T *const lockConfig);
void RTC_ReadRegisterLock(uint32_t ins, RTC_REGISTER_LOCK_CONFIG_T *const lockConfig);

void RTC_SecondsIntConfig(uint32_t ins, RTC_SECONDS_INT_CONFIG_T *const intConfig);
void RTC_FaultIntConfig(uint32_t ins, RTC_INTERRUPT_CONFIG_T *const intConfig);

void RTC_IRQHandlerControl(void);
void RTC_Seconds_IRQHandlerControl(void);

void RTC_IRQHandler(uint32_t ins);
void RTC_SecondsIRQHandler(uint32_t ins);

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

STATUS_T RTC_HW_Enable(RTC_T *const rtcBase);
STATUS_T RTC_HW_Disable(RTC_T *const rtcBase);

STATUS_T RTC_HW_AssignTimeSecondsRegister(RTC_T *const rtcBase, uint32_t seconds);
uint32_t RTC_HW_ReadTimeSecondsRegister(const RTC_T *const rtcBase);

void RTC_HW_AssignTimeAlarmRegister(RTC_T *const rtcBase, uint32_t seconds);
uint32_t RTC_HW_ReadTimeAlarmRegister(const RTC_T *const rtcBase);

void RTC_HW_AssignTimeCompensation(RTC_T *const rtcBase, int8_t compensationValue, uint8_t compensationInterval);
void RTC_HW_ReadCurrentTimeCompensation(const RTC_T *const rtcBase, int8_t *compensationValue, uint8_t *compensationInterval);

void RTC_HW_PickLPOSelect(RTC_T *const rtcBase, RTC_CLK_SRC_SEL_T clkSelect);

void RTC_HW_PickUpdateMode(RTC_T *const rtcBase, bool isUpdateEnable);
bool RTC_HW_ReadUpdateMode(const RTC_T *const rtcBase);

void RTC_HW_PickNonSupervisorAccess(RTC_T *const rtcBase, bool enable);

void RTC_HW_SoftwareReset(RTC_T *const rtcBase);

void RTC_HW_EnableTimeCounter(RTC_T *const rtcBase);
void RTC_HW_DisableTimeCounter(RTC_T *const rtcBase);
bool RTC_HW_ReadTimeCounterState(const RTC_T *const rtcBase);

bool RTC_HW_ReadTimeAlarmFlag(const RTC_T *const rtcBase);
bool RTC_HW_ReadTimeOverflowFlag(const RTC_T *const rtcBase);
bool RTC_HW_ReadTimeInvalidFlag(const RTC_T *const rtcBase);

void RTC_HW_LockRegLockIn(RTC_T *const rtcBase);
bool RTC_HW_ReadLockRegLock(const RTC_T *const rtcBase);
void RTC_HW_StatusRegLockIn(RTC_T *const rtcBase);
bool RTC_HW_ReadStatusRegLock(const RTC_T *const rtcBase);
void RTC_HW_CtrlRegLockIn(RTC_T *const rtcBase);
bool RTC_HW_ReadCtrlRegLock(const RTC_T *const rtcBase);
void RTC_HW_TimeCompensationLockIn(RTC_T *const rtcBase);
bool RTC_HW_ReadTimeCompensationLock(const RTC_T *const rtcBase);

void RTC_HW_ConfigTimeSecondsIntConf(RTC_T *const rtcBase, RTC_SECOND_INT_CFG_T intConfig);
void RTC_HW_ConfigTimeSecondsIntEnable(RTC_T *const rtcBase, bool enable);
void RTC_HW_ConfigTimeAlarmIntEnable(RTC_T *const rtcBase, bool enable);
void RTC_HW_ConfigTimeOverflowIntEnable(RTC_T *const rtcBase, bool enable);
void RTC_HW_ConfigTimeInvalidIntEnable(RTC_T *const rtcBase, bool enable);

/**@} end of group RTC_Functions*/
/**@} end of group RTC_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif /* APM32F445_446_RTC_H */
