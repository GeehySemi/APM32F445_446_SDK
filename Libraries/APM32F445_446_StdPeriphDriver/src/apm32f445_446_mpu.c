/*!
 * @file        apm32f445_446_mpu.c
 *
 * @brief       This file provides all the MPU firmware functions
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
#include "apm32f445_446_mpu.h"

/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup MPU_Driver MPU Driver
  @{
*/

/** @defgroup MPU_Macros Macros
  @{
*/

/*******************************************************************************
 *                              MACRO DEFINES
 ******************************************************************************/
/* Reset value of all master access perssion in region 0 */
#define REGION_0_ACCESS_RIGHT_RESET_VALUE (0x0061F7DFU)
/* Reset value of end address */
#define END_ADDRESS_RESET_VALUE           (0x1FU)
/* Default value of privilege right */
#define DEFAULT_PRIVILEGE_RIGHT           (MPU_SUPV_RWX_USER_RWX)
/* Default value of access perssion */
#define DEFAULT_ACCESS_RIGHT              (MPU_RW)
/* Mask and Shift of access perssion */
#define MPU_USER_MASK                     (0x07U)
#define MPU_USER_SHIFT                    (0U)
#define MPU_SUPV_MASK                     (0x18U)
#define MPU_SUPV_SHIFT                    (3U)
#define MPU_WRITE_MASK                    (0x20U)
#define MPU_WRITE_SHIFT                   (5U)
#define MPU_READ_MASK                     (0x40U)
#define MPU_READ_SHIFT                    (6U)

#define MPU_INSTANCE_COUNT                (1U)

#define MPU_INSTANCE_VALIDITY(__instance__) if(__instance__ >= MPU_INSTANCE_COUNT){while(1);}

/**@} end of group MPU_Macros*/

/** @defgroup MPU_Variables Variables
  @{
*/

/*******************************************************************************
 *                              GLOBAL VARIABLES
 ******************************************************************************/

/*! @brief Table of base addresses for MPU instances. */
static MPU_T *const g_mpuBasePointer[MPU_INSTANCE_COUNT] = { MPU };

/**@} end of group MPU_Variables*/

/** @defgroup MPU_Functions Functions
  @{
*/

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Initializes the memory protection unit by setting the access
 *        configurations of all available masters, process identifier and the memory
 *        location for the given regions; and activate module finally.
 *
 * @param ins: The MPU peripheral ins number.
 * @param regionCount: The number of configured regions.
 * @param userConfigArray: The pointer to the array of MPU user configure structure.
 *
 * @retval operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - STATUS_ERROR:   Operation failed due to master number is out of range supported by hardware.
 */
STATUS_T MPU_Init(uint32_t ins,
                  uint8_t regionCount,
                  const MPU_USER_CONFIG_T *userConfigArray)
{
    MPU_INSTANCE_VALIDITY(ins);

    STATUS_T retVal = STATUS_SUCCESS;
    MPU_T *mpuBase = g_mpuBasePointer[ins];
    uint8_t regionIndex = 0U;

    /* Resets all region descriptor */
    MPU_DeInit(ins);

    while((regionIndex < regionCount) && (retVal == STATUS_SUCCESS))
    {
        retVal = MPU_UpdateRegionCfg(ins, regionIndex, &userConfigArray[regionIndex]);
        regionIndex++;
    }


    if (retVal == STATUS_SUCCESS)
    {
        /* Enables the MPU module operation */
        MPU_HW_Enable(mpuBase);
    }

    return retVal;
}

/*!
 * @brief Resets the memory protection unit by reseting all regions to default
 *        and disable module.
 *
 * @param ins: The MPU peripheral ins number.
 *
 * @retval None.
 */
void MPU_DeInit(uint32_t ins)
{
    MPU_INSTANCE_VALIDITY(ins);

    uint8_t slaveIndex = 0U;
    uint8_t regionIndex = 0U;
    MPU_T *mpuBase = g_mpuBasePointer[ins];

    MPU_HW_Disable(mpuBase);

    /* Resets all region descriptors */
    while(regionIndex < MPU_REGD_COUNT)
    {
        MPU_HW_ResetRegion(mpuBase, regionIndex);
        regionIndex++;
    }

    /* Clears all slave error flag */
    while(slaveIndex < FEATURE_MPU_SLAVE_NUM)
    {
        MPU_HW_ClearErrorFlag(mpuBase, slaveIndex);
        slaveIndex++;
    }
}

/*!
 * @brief Gets default region configuration.
 *        Grants all access rights for masters and disables PID on entire memory.
 *
 * @param masterAccessPerssion: The pointer to master configuration structure.
 *        The length of array should be defined by number of masters supported by hardware.
 *
 * @retval The default region configuration.
 */
MPU_USER_CONFIG_T MPU_RegionDefaultConfig(MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion)
{
    uint8_t idx = 0U;
    MPU_USER_CONFIG_T regionCfg;
    uint8_t numOfMaster[FEATURE_MPU_MASTER_NUM] = FEATURE_MPU_MASTER;

    if (POINTER_IS_NULL(masterAccessPerssion) == false)
    {
        regionCfg.endAddr = 0xFFFFFFFFU;
        regionCfg.startAddr = 0x0U;

        while(idx < FEATURE_MPU_MASTER_NUM)
        {
            MPU_HW_MasterAccessRightDefaultConfig(numOfMaster[idx], &masterAccessPerssion[idx]);
            idx++;
        }

        regionCfg.masterAccessPerssion = masterAccessPerssion;

        regionCfg.processID = 0U;
        regionCfg.processIdMask = 0U;
    }
    else
    {
        while(1);
    }
    return regionCfg;
}

/*!
 * @brief Enables/Disables region descriptor.
 *
 * @param ins: The MPU peripheral ins number.
 * @param regionNum: The region number.
 * @param enable Valid state
 *         - true  : Enable region.
 *         - false : Disable region.
 *
 * @retval None.
 *
 * @note Please note that region 0 should not be disabled.
 */
void MPU_EnableRegion(uint32_t ins, uint8_t regionNum, bool enable)
{
    MPU_INSTANCE_VALIDITY(ins);

    MPU_T *mpuBase = g_mpuBasePointer[ins];

    /* Enables/Disables region descriptor */
    if (enable == false)
    {
        MPU_HW_DisableRegionValid(mpuBase, regionNum);
    }
    else
    {
        MPU_HW_EnableRegionValid(mpuBase, regionNum);
    }
}


/*!
 * @brief Updates the access configuration of all available masters,
 *        process identifier and memory location in a given region.
 *
 * @param ins: The MPU peripheral ins number.
 * @param regionNum: The region number.
 * @param userConfigPtr: The region configuration structure pointer.
 *
 * @retval operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - STATUS_ERROR:   Operation failed due to master number is out of range supported by hardware.
 */
STATUS_T MPU_UpdateRegionCfg(uint32_t ins,
                             uint8_t regionNum,
                             const MPU_USER_CONFIG_T *userConfigPtr)
{
    MPU_INSTANCE_VALIDITY(ins);

    STATUS_T retVal = STATUS_SUCCESS;
    uint8_t idx = 0U;
    MPU_T *mpuBase = g_mpuBasePointer[ins];

    /* Config access perssion for masters */
    while((idx < FEATURE_MPU_MASTER_NUM) && (retVal == STATUS_SUCCESS))
    {
        if (userConfigPtr->masterAccessPerssion[idx].numOfMaster > FEATURE_MPU_MAX_HIGH_MASTER_NUM)
        {
            retVal = STATUS_ERROR;
        }
        else
        {
            MPU_HW_ConfigMasterAccessRight(mpuBase, regionNum, &userConfigPtr->masterAccessPerssion[idx]);
        }
        idx++;
    }

    /* Config address and process identifier except region 0 */
    if (regionNum > 0U)
    {
        if (retVal == STATUS_SUCCESS)
        {
            mpuBase->REGD[regionNum].WORD0.reg = userConfigPtr->startAddr;
            mpuBase->REGD[regionNum].WORD1.reg = userConfigPtr->endAddr;

            MPU_HW_ConfigProcessIdentifier(mpuBase, regionNum, userConfigPtr->processID);
            MPU_HW_ConfigProcessIdentifierMask(mpuBase, regionNum, userConfigPtr->processIdMask);
            MPU_HW_EnableRegionValid(mpuBase, regionNum);
        }
    }

    return retVal;
}

/*!
 * @brief Config the region start and end address.
 *
 * @param ins: The MPU peripheral ins number.
 * @param regionNum: The region number.
 * @param startAddr: The region start address.
 * @param endAddr: The region end address.
 *
 * @retval None.
 *
 * @note Please note that using this function will clear the valid bit of the region,
 *       and a further validation might be needed.
 */
void MPU_ConfigRegionAddr(uint32_t ins,
                          uint8_t regionNum,
                          uint32_t startAddr,
                          uint32_t endAddr)
{
    MPU_INSTANCE_VALIDITY(ins);

    MPU_T *mpuBase = g_mpuBasePointer[ins];

    /* Config region start and end addresses */
    mpuBase->REGD[regionNum].WORD0.reg = startAddr;
    mpuBase->REGD[regionNum].WORD1.reg = endAddr;

    /* Re-enables the region descriptor valid bit */
    MPU_HW_EnableRegionValid(mpuBase, regionNum);
}

/*!
 * @brief Checks and gets the MPU access error detail information for a slave port.
 *        Clears bus error flag if an error occurs.
 *
 * @param ins: The MPU peripheral ins number.
 * @param slavePortNum: The slave port number to get Error Detail.
 * @param errInfoPtr: The pointer to access error info structure.
 *
 * @retval operation status
 *         - true:  An error has occurred.
 *         - false: No error has occurred.
 */
bool MPU_ReadDetailErrorAccessInfo(uint32_t ins,
                                   uint8_t slavePortNum,
                                   MPU_ACCESS_ERROR_INFO_T *errInfoPtr)
{
    MPU_INSTANCE_VALIDITY(ins);

    MPU_T *mpuBase = g_mpuBasePointer[ins];
    bool retVal = false;
    uint8_t slaveNum = slavePortNum;

    /* Read slaver port error status */
    retVal = MPU_HW_ReadErrorStatus(mpuBase, slaveNum);

    /* Check if there is access violation in the slave port */
    if (retVal == true)
    {
        /* Get the slave port detail error */
        MPU_HW_ReadErrorInfo(mpuBase, slaveNum, errInfoPtr);

        /* Clears slave port error flag */
        MPU_HW_ClearErrorFlag(mpuBase, slaveNum);
    }

    return retVal;
}

/*!
 * @brief Config access permission for bus master in region.
 *
 * @param ins: The MPU peripheral ins number.
 * @param regionNum: The MPU region number.
 * @param accessRightsPtr: The pointer to access permission structure.
 *
 * @retval operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - STATUS_ERROR:   Operation failed due to master number is out of range supported by hardware.
 */
STATUS_T MPU_ConfigMasterAccessPerssion(uint32_t ins,
                                      uint8_t regionNum,
                                      const MPU_MASTER_ACCESS_PERMISSION_T *accessRightsPtr)
{
    MPU_INSTANCE_VALIDITY(ins);

    MPU_T *mpuBase = g_mpuBasePointer[ins];
    STATUS_T retVal = STATUS_SUCCESS;

    /* Config access rights for master */
    if (accessRightsPtr->numOfMaster > FEATURE_MPU_MAX_HIGH_MASTER_NUM)
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        MPU_HW_ConfigMasterAccessRight(mpuBase, regionNum, accessRightsPtr);
    }

    return retVal;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Enables the MPU module.
 *
 * @param mpuBase: The MPU peripheral base address.
 *
 * @retval None.
 */
void MPU_HW_Enable(MPU_T *const mpuBase)
{
    mpuBase->CESTS.reg |= (1U << 0U);
}

/*!
 * @brief Disables the MPU module.
 *
 * @param mpuBase: The MPU peripheral base address.
 *
 * @retval None.
 */
void MPU_HW_Disable(MPU_T *const mpuBase)
{
    mpuBase->CESTS.reg &= ~(1U << 0U);
}

/*!
 * @brief Enables the region descriptor.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number 0 ~ 7.
 *
 * @retval None.
 */
void MPU_HW_EnableRegionValid(MPU_T *const mpuBase, uint8_t regionNum)
{
    mpuBase->REGD[regionNum].WORD3.reg |= (1U << 0U);
}

/*!
 * @brief Disables the region descriptor.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number 0 ~ 7.
 *
 * @retval None.
 */
void MPU_HW_DisableRegionValid(MPU_T *const mpuBase, uint8_t regionNum)
{
    mpuBase->REGD[regionNum].WORD3.reg &= ~(1U << 0U);
}

/*!
 * @brief Sets the process identifier.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number.
 * @param pid: The process identifier.
 *
 * @retval None.
 */
void MPU_HW_ConfigProcessIdentifier(MPU_T *const mpuBase, uint8_t regionNum, uint8_t pid)
{
    mpuBase->REGD[regionNum].WORD3.reg &= ~(0xffU << 24U);
    mpuBase->REGD[regionNum].WORD3.reg |= (uint32_t)((pid & 0xFFU) << 24U);
}

/*!
 * @brief Sets the process identifier mask.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number.
 * @param pidMask: The process identifier mask.
 *
 * @retval None.
 */
void MPU_HW_ConfigProcessIdentifierMask(MPU_T *const mpuBase,
                                        uint8_t regionNum,
                                        uint8_t pidMask)
{
    mpuBase->REGD[regionNum].WORD3.reg &= ~(0xffU << 16U);
    mpuBase->REGD[regionNum].WORD3.reg |= (uint32_t)((pidMask & 0xFFU) << 16U);
}

/*!
 * @brief Read the error status of a specified slave port.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param slaveNum: The slave port number.
 *
 * @retval The slave ports error status:
 *         - true:  error happens in this slave port.
 *         - false: error didn't happen in this slave port.
 */
bool MPU_HW_ReadErrorStatus(const MPU_T *const mpuBase, uint8_t slaveNum)
{
    return ((mpuBase->CESTS.reg &
             (((uint32_t)((uint32_t)(1U << ((4U - 1U) - slaveNum))))<<28U))&0xF0000000U);
}

/*!
 * @brief Clears the error flag of a specified slave port.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param slaveNum: The slave port number.
 *
 * @retval None.
 */
void MPU_HW_ClearErrorFlag(MPU_T *const mpuBase, uint8_t slaveNum)
{
    mpuBase->CESTS.reg = (mpuBase->CESTS.reg & ~0xF0000000U) |
                            (((uint32_t)(((uint32_t)(1U << ((4U - 1U) - slaveNum)))<<28U))&0xF0000000U);
}

/*!
 * @brief Sets access permission for master in region descriptor.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number.
 * @param masterAccessPerssion: The pointer to the master access rights
 *
 * @retval None.
 */
void MPU_HW_ConfigMasterAccessRight(MPU_T *const mpuBase,
                                    uint8_t regionNum,
                                    const MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion)
{
    uint32_t accessPerssion;
    uint32_t accessMask;
    uint32_t accessShift;
    uint32_t tempReg;
    uint32_t numOfMaster = masterAccessPerssion->numOfMaster;

    if (numOfMaster <= FEATURE_MPU_MAX_LOW_MASTER_NUM)
    {
        /* Prepare Supervisor Mode Access Control and User Mode Access Control value */
        accessPerssion = (((uint32_t)masterAccessPerssion->accessPerssion & MPU_USER_MASK)
                      >> MPU_USER_SHIFT) << 0U;
        accessPerssion |= ((((uint32_t)masterAccessPerssion->accessPerssion & MPU_SUPV_MASK)
                       >> MPU_SUPV_SHIFT) << 3U);
        accessMask = (0x07U << 0U) | (0x03U << 3U);
        /* Enables/Disables process identifier */
        if (numOfMaster < FEATURE_MPU_MASTER_WITH_PROCESS_IDENTIFIER_NUM)
        {
            accessPerssion |= ((masterAccessPerssion->isProcessIdentifier ? 1U : 0U) << 5U);
        }
        accessMask |= (0x01U << 5U);
        /* Shift FEATURE_MPU_LOW_MASTER_CONTROL_WIDTH-bit field defining
         * separate privilege rights depend on bus master number
         */
        accessShift = (numOfMaster * FEATURE_MPU_LOW_MASTER_CONTROL_WIDTH);
    }
    else
    {
        /* Prepare Read Enable and Write Enable value */
        accessPerssion = (((uint32_t)masterAccessPerssion->accessPerssion & MPU_WRITE_MASK)
                      >> MPU_WRITE_SHIFT) << 24U;
        accessPerssion |= ((((uint32_t)masterAccessPerssion->accessPerssion & MPU_READ_MASK)
                       >> MPU_READ_SHIFT) << 25U);
        accessMask = (0x01U << 24U) | (0x01U << 25U);
        /* Low master number is numbered from 0
           so master number count will be FEATURE_MPU_MAX_LOW_MASTER_NUM added to 1 */
        accessShift = FEATURE_MPU_HIGH_MASTER_CONTROL_WIDTH *
                      (numOfMaster - (FEATURE_MPU_MAX_LOW_MASTER_NUM + 1U));
    }

    accessPerssion = accessPerssion << accessShift;
    accessMask  = accessMask << accessShift;

    /* Set access rights */
    tempReg = mpuBase->RDAACC[regionNum].reg;
    tempReg = (tempReg & ~accessMask) | accessPerssion;
    mpuBase->RDAACC[regionNum].reg = tempReg;
}

/*!
 * @brief Resets the region descriptor to default.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param regionNum: The region number.
 *
 * @retval None.
 */
void MPU_HW_ResetRegion(MPU_T *const mpuBase, uint8_t regionNum)
{
    if (regionNum != 0U)
    {
        mpuBase->REGD[regionNum].WORD3.reg = 0U;
        mpuBase->REGD[regionNum].WORD2.reg = 0U;
        mpuBase->REGD[regionNum].WORD1.reg = END_ADDRESS_RESET_VALUE;
        mpuBase->REGD[regionNum].WORD0.reg = 0U;
    }
    else
    {
        /* Config default access perssion for region 0 */
        mpuBase->RDAACC[0].reg = REGION_0_ACCESS_RIGHT_RESET_VALUE;
    }
}

/*!
 * @brief Read MPU detail error access info.
 *
 * @param mpuBase: The MPU peripheral base address.
 * @param slaveNum: The slave port number 0 ~ 4.
 * @param errInfoPtr: The pointer to the MPU access error information.
 *
 * @retval None.
 */
void MPU_HW_ReadErrorInfo(const MPU_T *const mpuBase,
                          uint8_t slaveNum,
                          MPU_ACCESS_ERROR_INFO_T *errInfoPtr)
{
    uint32_t temp;

    /* Read the Error Detail Register for the slave port */
    temp = mpuBase->EADDR_EDETAIL[slaveNum].EDETAIL.reg;

    /* Report Error Access control */
    errInfoPtr->accessCtr = (uint16_t)((temp >> 16U) & 0xFFFFU);

    /* Report Error Master Number to user */
    errInfoPtr->master = (uint8_t)((temp >> 4U) & 0x0FU);

    /* Report Error Attributes to user */
    errInfoPtr->attributes = (MPU_ERR_ATTRIBUTES_T)((temp >> 1U) & 0x07U);

    /* Report Error Read/Write to user */
    errInfoPtr->accessType = (MPU_ERR_ACCESS_TYPE_T)((temp >> 0U) & 0x01U);

    /* Report Error Address to user */
    errInfoPtr->addr = mpuBase->EADDR_EDETAIL[slaveNum].EADDR.reg;

    /* Report Error Process Identification to user */
    errInfoPtr->processorIdentification = (uint8_t)((temp >> 8U) & 0xFFU);
}

/*!
 * @brief Gets the default master access rights.
 *
 * @param numOfMaster: The master number.
 * @param masterAccessPerssion: The pointer to the master access rights
 *
 * @retval None.
 */
void MPU_HW_MasterAccessRightDefaultConfig(uint8_t numOfMaster,
                                           MPU_MASTER_ACCESS_PERMISSION_T *masterAccessPerssion)
{
    masterAccessPerssion->numOfMaster = numOfMaster;

    if (numOfMaster <= FEATURE_MPU_MAX_LOW_MASTER_NUM)
    {
        masterAccessPerssion->accessPerssion = DEFAULT_PRIVILEGE_RIGHT;
    }
    else
    {
        masterAccessPerssion->accessPerssion = DEFAULT_ACCESS_RIGHT;
    }

    masterAccessPerssion->isProcessIdentifier = false;
}

/**@} end of group MPU_Functions*/
/**@} end of group MPU_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/
