/*!
 * @file        callbacks.h
 *
 * @brief       Callback functions
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

#ifndef CALLBACKS_H
#define CALLBACKS_H

/** @addtogroup COMMON
  @{
*/

/** @addtogroup CallBack
  @{
*/

/** @defgroup CallBack_Enumerations
  @{
*/

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/

/*!
 * @brief Events for UART callback
 */
typedef enum
{
    UART_EVENT_RX_BUFFER_FULL       = 0x00U,
    UART_EVENT_TX_BUFFER_EMPTY      = 0x01U,
    UART_EVENT_TRANSFER_COMPLETE    = 0x02U,
    UART_EVENT_ERROR                = 0x03U,
} UART_EVENT_T;

/*!
 * @brief Events for I2C master callback
 */
typedef enum
{
    I2C_MASTER_EVENT_TRANSFER_COMPLETE  = 0x4U,
} I2C_MASTER_EVENT_T;

/*!
 * @brief Events for I2C slave callback
 */
typedef enum
{
    I2C_SLAVE_EVENT_RX_BUFFER_FULL     = 0x00U,
    I2C_SLAVE_EVENT_TX_BUFFER_EMPTY    = 0x01U,
    I2C_SLAVE_EVENT_TX_REQUEST         = 0x02U,
    I2C_SLAVE_EVENT_RX_REQUEST         = 0x03U,
    I2C_SLAVE_EVENT_STOP               = 0X04U,
} I2C_SLAVE_EVENT_T;

/*!
 * @brief Events for SPI callback
 */
typedef enum
{
    SPI_EVENT_TRANSFER_COMPLETE = 0
} SPI_EVENT_T;

/*!
 * @brief Events for input capture callback
 */
typedef enum
{
    IC_EVENT_MEASUREMENT_COMPLETE = 0x00U   /* Capture input signal is completed */
} IC_EVENT_T;

/**@} end of group CallBack_Enumerations*/

/** @defgroup CallBack_Typedefs
  @{
*/
/*******************************************************************************
 *                              TYPEDEFS
 ******************************************************************************/

/*!
 * @brief UART callback function
 */
typedef void (*UART_CALLBACK_T)(void *state, UART_EVENT_T event, void *data);

/*!
 * @brief I2C master callback function
 */
typedef void (*I2C_MASTER_CALLBACK_T)(I2C_MASTER_EVENT_T event, void *data);

/*!
 * @brief I2C slave callback function
 */
typedef void (*I2C_SLAVE_CALLBACK_T)(I2C_SLAVE_EVENT_T event, void *data);

/*!
 * @brief SPI callback function
 */
typedef void (*SPI_CALLBACK_T)(void *state, SPI_EVENT_T event, void *data);

/*!
 * @brief CSEC callback function
 */
typedef void (*CSEC_CALLBACK_T)(uint32_t completedCmd, void *callbackParam);

/*!
 * @brief Input capture callback function
 */
typedef void (*IC_CALLBACK_T)(IC_EVENT_T event, void *data);

/**@} end of group CallBack_Typedefs*/
/**@} end of group CallBack*/
/**@} end of group COMMON*/

#endif /* CALLBACKS_H */
