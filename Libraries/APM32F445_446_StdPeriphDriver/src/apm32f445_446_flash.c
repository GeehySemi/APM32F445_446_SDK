/*!
 * @file        apm32f445_446_flash.c
 *
 * @brief       This file provides all the FLASH firmware functions
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
#include "apm32f445_446_flash.h"
/** @addtogroup APM32F445_446_StdPeriphDriver
  @{
*/

/** @addtogroup FLASH_Driver FLASH Driver
  @{
*/

/** @defgroup FLASH_Functions Functions
  @{
*/

/*******************************************************************************
                        PRIVATE FUNCTION DECLARATIONS
*******************************************************************************/
FUNCTION_DECLARATION_AT_RAMSECTION_START
static STATUS_T FLASH_CommandSequence(const FLASH_SSD_CONFIG_T * ssdCfg)
FUNCTION_DECLARATION_AT_RAMSECTION_END

static STATUS_T FLASH_ProgramCheckExecute(const FLASH_SSD_CONFIG_T *ssdCfg,
                                            uint32_t dest, uint32_t size,
                                            const uint8_t *expectedData,
                                            uint32_t *failAddr, uint8_t marginLevel);
static STATUS_T FLASH_WaitEEWriteToFinish(const FLASH_SSD_CONFIG_T *ssdCfg,
                                            uint32_t dest,
                                            const uint8_t *data,
                                            uint8_t step);

/*******************************************************************************
 *                          PUBLIC DRIVER FUNCTIONS
 ******************************************************************************/
/*!
 * @brief Read default flash user configuration.
 *
 * @retval STATUS_T Code.
 */
void FLASH_DefaultConfig(FLASH_USER_CONFIG_T * const cfg)
{
    cfg->eeramBase   = 0x14000000U;
    cfg->dflashBase  = 0x10000000U;
    cfg->pflashBase  = 0x00000000U;
    cfg->pflashSize  = FEATURE_FLASH_PF_BLOCK_SIZE;
    cfg->callBack    = CALLBACK_IS_NULL;
}

/*!
 * @brief Read DFlash size from CFGNVM Partition Code.
 *
 * @param ssdCfg:  The SSD configuration structure pointer.
 * @param departCode: The value of CFGNVM Partition Code.
 * @retval None.
 */
void FLASH_ReadDEPartitionCode(FLASH_SSD_CONFIG_T* const ssdCfg, uint8_t *departCode)
{
    /* Select D-Flash size */
    if (*departCode == 0x00U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0000;
    }
    else if (*departCode == 0x01U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0001;
    }
    else if (*departCode == 0x02U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0010;
    }
    else if (*departCode == 0x03U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0011;
    }
    else if (*departCode == 0x04U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0100;
    }
    else if (*departCode == 0x05U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0101;
    }
    else if (*departCode == 0x06U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0110;
    }
    else if (*departCode == 0x07U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_0111;
    }
    else if (*departCode == 0x08U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1000;
    }
    else if (*departCode == 0x09U)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1001;
    }
    else if (*departCode == 0x0AU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1010;
    }
    else if (*departCode == 0x0BU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1011;
    }
    else if (*departCode == 0x0CU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1100;
    }
    else if (*departCode == 0x0DU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1101;
    }
    else if (*departCode == 0x0EU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1110;
    }
    else if (*departCode == 0x0FU)
    {
        ssdCfg->dflashSize = (uint32_t)FEATURE_FLASH_DEPART_1111;
    }
    else
    {}
}

/*!
 * @brief Initializes Flash.
 *
 * @details This API initializes Flash module by reporting the memory configuration
 * via SSD configuration structure.
 *
 * @param userConfig: The user configuration structure pointer.
 * @param ssdCfg:  The SSD configuration structure pointer.
 * @returnValue operation status
 *              - STATUS_SUCCESS:         Operation was successful.
 */
STATUS_T FLASH_Init(const FLASH_USER_CONFIG_T* const userConf, FLASH_SSD_CONFIG_T* const ssdCfg)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint8_t departitionCode = 0;
    uint8_t eeeramSizeCode = 0;

    ssdCfg->dflashBase = userConf->dflashBase;
    ssdCfg->pflashSize = userConf->pflashSize;
    ssdCfg->pflashBase = userConf->pflashBase;
    ssdCfg->eeramBase = userConf->eeramBase;
    ssdCfg->callBack = userConf->callBack;

    departitionCode = (uint8_t)SIM->FLASHCFG1.bit.CFGNVMP;
    eeeramSizeCode = (uint8_t)SIM->FLASHCFG1.bit.ERAMDS;

    /* Read DFlash size */
    FLASH_ReadDEPartitionCode(ssdCfg, &departitionCode);
    if(ssdCfg->dflashSize >= FEATURE_FLASH_DF_BLOCK_SIZE)
    {
        ssdCfg->eeeSize = 0U;
    }
    else
    {
        ssdCfg->eeeSize = FEATURE_FLASH_CFGRAM_SIZE;
    }

    (void)eeeramSizeCode;
    return returnValue;
}


/*!
 * @brief Erases one or more sectors.
 *
 * @details This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 *
 * @param ssdCfg: The pointer of FLASH_SSD_CONFIG_T.
 *
 * @retval None.
 */
STATUS_T FLASH_EraseSector(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint32_t size)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t sectorSize;
    uint32_t temp;
    uint32_t tempSize = size;

    temp = ssdCfg->dflashBase;
    if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
    {
        if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_DF_SECTOR_CMD) != 0U)
        {
            returnValue = STATUS_ERROR;
        }
        else
        {
            dest += 0x800000U - temp;
            sectorSize = (uint32_t)FEATURE_FLASH_DF_BLOCK_SECTOR_SIZE;
        }
    }
    else
    {
        temp = ssdCfg->pflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
        {
            if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SECTOR_CMD) != 0U)
            {
                returnValue = STATUS_ERROR;
            }
            else
            {
                dest -= temp;
                sectorSize = (uint32_t)FEATURE_FLASH_PF_BLOCK_SECTOR_SIZE;
            }
        }
        else
        {
            returnValue = STATUS_ERROR;
            tempSize = 0U;
            sectorSize = 0U;
        }
    }

    /* Check if the size is sector alignment or not */
    if((tempSize & (sectorSize - 1U)) != 0U)
    {
        /* Return an error code */
        returnValue = STATUS_ERROR;
    }

    while ((STATUS_SUCCESS == returnValue) && (tempSize > 0U))
    {
        /* Check OCIFLG to verify the previous command is completed */
        if(0U != (FLASH->STS.reg & 0x80U)>>7U)
        {
            /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
            FLASH->STS.reg = (uint8_t)0x70U;

            FLASH->CCMDDATA0.reg = FTFx_ERASE_SECTOR;
            FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
            FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
            FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);

            /* Execute the command */
            returnValue = FLASH_CommandSequence(ssdCfg);

            /* Update size and destination address */
            tempSize -= sectorSize;
            dest += sectorSize;
        }
        else
        {
            returnValue = STATUS_BUSY;
        }
    }

    return returnValue;
}

/*!
 * @brief Verify one or more sectors.
 *
 * @details Checks if a section of the P-Flash or the D-Flash memory
 * is erased to the specified read margin level.
 *
 * @param ssdCfg: The pointer of FLASH_SSD_CONFIG_T.
 *
 * @retval None.
 */
STATUS_T FLASH_VerifySection(const FLASH_SSD_CONFIG_T * ssdCfg,
                            uint32_t dest,
                            uint16_t number,
                            uint8_t marginLevel)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;

    /* Check if the destination is aligned or not */
    temp = ssdCfg->dflashBase;
    if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
    {
        if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_DF_SECTION_CMD) != 0U)
        {

            returnValue = STATUS_ERROR;
        }
        else
        {
            dest += 0x800000U - temp;
        }
    }
    else
    {
        temp = ssdCfg->pflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
        {
            if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SECTION_CMD) != 0U)
            {
                returnValue = STATUS_ERROR;
            }
            else
            {
                dest -= temp;
            }

        }
        else
        {
            returnValue = STATUS_ERROR;
        }
    }

    if(STATUS_SUCCESS == returnValue)
    {
        /* Check OCIFLG to verify the previous command is completed */
        if(0U != (FLASH->STS.reg & 0x80U)>>7U)
        {
            /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
            FLASH->STS.reg = (uint8_t)0x70U;

            FLASH->CCMDDATA0.reg = FTFx_VERIFY_SECTION;
            FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
            FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
            FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);
            FLASH->CCMDDATA4.reg = GET_BIT_8_15(number);
            FLASH->CCMDDATA5.reg = GET_BIT_0_7(number);
            FLASH->CCMDDATA6.reg = marginLevel;

            /* Execute the command */
            returnValue = FLASH_CommandSequence(ssdCfg);
        }
        else
        {
            returnValue = STATUS_BUSY;
        }
    }

    return returnValue;
}

/*!
 * @brief Erases all addresses in an individual P-Flash or D-Flash block.
 *
 * @details For the derivatives including multiply logical P-Flash or D-Flash blocks,
 * this API erases a single block in a single call.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EraseBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;

    /* Check if the destination is aligned or not */
    temp = ssdCfg->dflashBase;
    if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
    {
        if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_DF_BLOCK_CMD) != 0U)
        {
            returnValue = STATUS_ERROR;
        }
        else
        {
            dest += 0x800000U - temp;
        }
    }
    else
    {
        temp = ssdCfg->pflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
        {
            if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_PF_BLOCK_CMD) != 0U)
            {
                returnValue = STATUS_ERROR;
            }
            else
            {
                dest -= temp;
            }
        }
        else
        {
            returnValue = STATUS_ERROR;
        }
    }

    if(STATUS_SUCCESS == returnValue)
    {
        /* Check OCIFLG to verify the previous command is completed */
        if(0U != (FLASH->STS.reg & 0x80U)>>7U)
        {
            /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
            FLASH->STS.reg = (uint8_t)0x70U;

            FLASH->CCMDDATA0.reg = FTFx_ERASE_BLOCK;
            FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
            FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
            FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);

            /* Execute the command */
            returnValue = FLASH_CommandSequence(ssdCfg);
        }
        else
        {
            returnValue = STATUS_BUSY;
        }
    }

    return returnValue;
}

/*!
 * @brief Verify whether erase entire P-Flash or D-Flash block.
 *
 * @details For the derivatives including multiply
 * logical P-Flash or D-Flash blocks, this API erases a single block in a single call.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_VerifyBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint8_t marginLevel)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;

    /* Check if the destination is aligned or not */
    temp = ssdCfg->dflashBase;
    if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
    {
        if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_DF_BLOCK_CMD) != 0U)
        {
            returnValue = STATUS_ERROR;
        }
        else
        {
            dest += 0x800000U - temp;
        }
    }
    else
    {
        temp = ssdCfg->pflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
        {
            if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_PF_BLOCK_CMD) != 0U)
            {
                returnValue = STATUS_ERROR;
            }
            else
            {
                dest -= temp;
            }
        }
        else
        {
            returnValue = STATUS_ERROR;
        }
    }

    if(returnValue == STATUS_SUCCESS)
    {
        /* Check OCIFLG to verify the previous command is completed */
        if(0U != (FLASH->STS.reg & 0x80U)>>7U)
        {
            /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
            FLASH->STS.reg = (uint8_t)0x70U;

            FLASH->CCMDDATA0.reg = FTFx_VERIFY_BLOCK;
            FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
            FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
            FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);
            FLASH->CCMDDATA4.reg = marginLevel;

            /* Execute the command */
            returnValue = FLASH_CommandSequence(ssdCfg);
        }
        else
        {
            returnValue = STATUS_BUSY;
        }
    }

    return returnValue;
}

/*!
 * @brief Erases all Flash memory.
 *
 * @details Initializes the CFGRAM, verifies all memory contents, and then releases the MCU security.
 *
 * @param ssdCfg: The pointer of ssdCfg variable.
 *
 * @retval None.
 */
STATUS_T FLASH_EraseAllBlock(const FLASH_SSD_CONFIG_T * ssdCfg)
{
    STATUS_T returnValue;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        FLASH->CCMDDATA0.reg = FTFx_ERASE_ALL_BLOCK;

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Verify all Flash memory.
 *
 * @details Checks to see if the P-Flash and/or D-Flash, EEPROM
 * backup area, and D-Flash IFR have been erased to the specified read
 * margin level, if applicable, and releases security if the readout passes.
 *
 * @param ssdCfg: The pointer of FLASH_SSD_CONFIG_T.
 *
 * @retval None.
 */
STATUS_T FLASH_VerifyAllBlock(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t marginLevel)
{
    STATUS_T returnValue;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        FLASH->CCMDDATA0.reg = FTFx_VERIFY_ALL_BLOCK;
        FLASH->CCMDDATA1.reg = marginLevel;

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Suspend a current operation of Flash erase sector command.
 *
 * @details This function must be located in RAM memory or different Flash blocks which are
 * targeted for writing to avoid the RWW error.
 *
 * @retval None.
 */
void FLASH_EraseSuspend(void)
{
    uint32_t cnt = SUSPEND_WAIT_CNT;    /* Counter variable */

    if(FLASH->STS.bit.OCIFLG == 0U)
    {
        FLASH->CFG.reg |= (uint8_t)0x01U;

        /* Wait till OCIFLG bit is set */
        while ((((FLASH->STS.reg >> 7U) & 0x01U) == 0U) && (cnt > 0U))
        {
            cnt--;
        }
    }
}

/*!
 * @brief Resume a previous suspended operation of Flash erase sector command.
 *
 * @details This function must be located in RAM memory or different Flash blocks which are targeted
 * for writing to avoid RWW error.
 *
 * @retval None.
 */
void FLASH_EraseResume(void)
{
    uint16_t cnt = 0U;    /* Counter variable */

    /* Check ERACMDSUS bit of the flash configuration register */
    if(((FLASH->CFG.reg >> 4U) & 0x01U) == FLASH_CFG_ERACMDSUS_1)
    {
        /* Clear OCIFLG to launch command */
        FLASH->STS.reg |= (uint8_t)(0x80U);
        /* Wait for completion of this command */
        while ((0U == (FLASH->STS.reg & 0x80U)>>7U) && (cnt < RESUME_WAIT_CNT))
        {
            cnt++;
        }
    }
}

/*!
 * @brief Program 4 consecutive bytes (for program long word command)
 * and 8 consecutive bytes (for program phrase command) on P-Flash or D-Flash block.
 *
 * @details Returns STATUS_SUCCESS if size provided by user is zero regardless of the input validation.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_Program(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, const uint8_t *data)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;
    uint8_t i;

    if((size & (FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE - 1U)) == 0U)
    {
        temp = ssdCfg->dflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
        {
            dest += 0x800000U - temp;
        }
        else
        {
            temp = ssdCfg->pflashBase;
            if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
            {
                dest -= temp;
            }
            else
            {
                returnValue = STATUS_ERROR;
            }
        }

        while ((STATUS_SUCCESS == returnValue) && (size > 0U))
        {
            /* Check OCIFLG to verify the previous command is completed */
            if(0U != (FLASH->STS.reg & 0x80U)>>7U)
            {
                /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
                FLASH->STS.reg = (uint8_t)0x70U;

                #if(FEATURE_FLASH_DF_BLOCK_WRITE_UNIT_SIZE != FTFx_PHRASE_SIZE)
                FLASH->CCMDDATA0.reg = FTFx_PROGRAM_LONGWORD;
                #else
                FLASH->CCMDDATA0.reg = FTFx_PROGRAM_PHRASE;
                #endif
                FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
                FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
                FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);

                for (i = 0U; i < FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE; i++)
                {
                    temp = FLASH_BASE + i + 0x08U;
                    *(uint8_t *)(temp) = data[i];
                }

                /* Execute the command */
                returnValue = FLASH_CommandSequence(ssdCfg);

                /* Increment the source address by 1 */
                data += FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE;
                /* Update destination address for next iteration */
                dest += FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE;
                /* Update size for next iteration */
                size -= FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE;
            }
            else
            {
                returnValue = STATUS_BUSY;
            }
        }
    }
    else
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}

/*!
 * @brief Tests a previously programmed P-Flash or D-Flash long word
 * to see if it reads correctly at the specified margin level.
 *
 * @details Returns STATUS_SUCCESS if size provided by user is zero regardless of the input validation.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ProgramCheck(const FLASH_SSD_CONFIG_T * ssdCfg,
                            uint32_t dest, uint32_t size,
                            const uint8_t *expectedData,
                            uint32_t *failAddr,
                            uint8_t marginLevel)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t failAddress = 0U;

    if((size & (FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD - 1U)) == 0U)
    {
        returnValue = FLASH_ProgramCheckExecute(ssdCfg,dest,size,expectedData,&failAddress,marginLevel);
        *failAddr = failAddress;
    }
    else
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}

/*!
 * @brief Performs 32 bit sum of each byte data over a specified Flash
 * memory range without carry which provides rapid method for checking data integrity.
 *
 * @details The callback time period of this API is determined via FLASH_CALLBACK_CS macro in the
 * flash_driver.h which is used as a counter value for the callBack() function calling in
 * this API. This value can be changed as per the user requirement. User can change this value
 * to obtain the maximum permissible callback time period.
 * This API always returns STATUS_SUCCESS if size provided by user is zero regardless of the input
 * validation.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_CheckSum(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, uint32_t *sum)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t cnt = 0U;
    uint32_t data;
    uint32_t endAddress;
    uint32_t tempSize = size;

    /* Calculating Flash end address */
    endAddress = dest + tempSize;

    /* Check for valid range of the target addresses */
    if((dest < ssdCfg->pflashBase) || (endAddress > (ssdCfg->pflashBase + ssdCfg->pflashSize)))
    {
        if((dest < ssdCfg->dflashBase) || (endAddress > (ssdCfg->dflashBase + ssdCfg->dflashSize)))
        {
            returnValue = STATUS_ERROR;
            tempSize = 0U;
        }
    }

    *sum = 0U;
    /* Doing sum operation */
    while (tempSize > 0U)
    {
        data = *(uint8_t *)(dest);
        *sum = *sum + data;
        dest = dest + 1U;
        tempSize = tempSize - 1U;
        ++cnt;

        /* Check if need to serve callback function */
        if(FLASH_CALLBACK_CS <= cnt)
        {
            /* Serve callback function if cnt reaches limitation */
            if(ssdCfg->callBack != CALLBACK_IS_NULL)
            {
                ssdCfg->callBack();
            }

            cnt = 0U;
        }
    }

    return returnValue;
}

/*!
 * @brief Program the data found in the Section Program Buffer to previously erased locations in the Flash memory.
 *
 * @details Data is preloaded into the Section Program Buffer by writing to the acceleration Ram and CFGRam
 * while it is set to function as a RAM. The Section Program Buffer is limited
 * to the value of CFGRam divides by a ratio. Refer to the associate reference
 * manual to get correct value of this ratio.
 * For derivatives including swap feature, the swap indicator address is encountered
 * during FlashProgramSection, it is bypassed without setting FPVIOL but the content
 * are not be programmed. In addition, the content of source data used to program to
 * swap indicator will be re-initialized to 0xFF after completion of this command.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ProgramSection(const FLASH_SSD_CONFIG_T * ssdCfg, uint32_t dest, uint16_t number)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;

    /* Check RAMCSTS bit of the flash configuration register */
    if(0U == ((FLASH->CFG.reg >> 1U) & 0x01U))
    {
        /* Return an error code */
        returnValue = STATUS_UNSUPPORTED;
    }
    else
    {
        temp = ssdCfg->dflashBase;
        if((dest >= temp) && (dest <= (temp + ssdCfg->dflashSize)))
        {
            if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_DF_SECTION_CMD) != 0U)
            {
                returnValue = STATUS_ERROR;
            }
            else
            {
                dest += 0x800000U - temp;
            }
        }
        else
        {
            temp = ssdCfg->pflashBase;
            if((dest >= temp) && (dest <= (temp + ssdCfg->pflashSize)))
            {
                if((dest % FEATURE_FLASH_ADDRESS_ALIGMENT_PF_SECTION_CMD) != 0U)
                {
                    returnValue = STATUS_ERROR;
                }
                else
                {
                    dest -= temp;
                }
            }
            else
            {
                returnValue = STATUS_ERROR;
            }
        }

        if(returnValue == STATUS_SUCCESS)
        {
            /* Check OCIFLG to verify the previous command is completed */
            if(0U != (FLASH->STS.reg & 0x80U)>>7U)
            {
                /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
                FLASH->STS.reg = (uint8_t)0x70U;

                FLASH->CCMDDATA0.reg = FTFx_PROGRAM_SECTION;
                FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
                FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
                FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);
                FLASH->CCMDDATA4.reg = GET_BIT_8_15(number);
                FLASH->CCMDDATA5.reg = GET_BIT_0_7(number);

                /* Execute the command */
                returnValue = FLASH_CommandSequence(ssdCfg);
            }
            else
            {
                returnValue = STATUS_BUSY;
            }
        }
    }

    return returnValue;
}

/*!
 * @brief Program to a reserved 64 byte field located in the P-Flash IFR via given number of record.
 *
 * @details See the corresponding reference manual to get correct value of this number.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ProgramOnce(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t recordIndex, const uint8_t * dataArray)
{
    STATUS_T returnValue;
    uint32_t temp;
    uint8_t i;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        FLASH->CCMDDATA0.reg = FTFx_PROGRAM_ONCE;
        FLASH->CCMDDATA1.reg = recordIndex;

        for (i = 0U; i < FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE; i++)
        {
            temp = FLASH_BASE + i + 0x08U;
            *(uint8_t *)temp = dataArray[i];
        }

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}


/*!
 * @brief Read out a reserved 64 byte field located in the P-Flash IFR via given number of record.
 *
 * @details See the corresponding reference manual to get the correct value of this number.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ReadOnce(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t recordIndex, uint8_t * dataArray)
{
    STATUS_T returnValue;
    uint32_t temp;
    uint8_t i;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        FLASH->CCMDDATA0.reg = FTFx_READ_ONCE;
        FLASH->CCMDDATA1.reg = recordIndex;

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);

        if(STATUS_SUCCESS == returnValue)
        {
            /* Read the data from the FCCOB registers into the dataArray */
            for (i = 0U; i < FEATURE_FLASH_PF_BLOCK_WRITE_UNIT_SIZE; i++)
            {
                temp = FLASH_BASE + i + 0x08U;
                dataArray[i] = *(uint8_t *)temp;
            }
        }
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Read the current P-Flash protection status.
 *
 * @details Considering the time consumption for getting protection is very low and
 * even can be ignored. It is not necessary to utilize the Callback function to
 * support the time-critical events.
 *
 * @param protectStatus: The pointer of protectStatus variable.
 *
 * @retval None.
 */
void FLASH_ReadPFlashProtection(uint32_t * protectStatus)
{
    uint32_t reg0, reg1, reg2, reg3;

    reg3 = (uint32_t)FLASH->PFPRO3.reg;
    reg2 = (uint32_t)FLASH->PFPRO2.reg;
    reg1 = (uint32_t)FLASH->PFPRO1.reg;
    reg0 = (uint32_t)FLASH->PFPRO0.reg;

    *protectStatus = (uint32_t)((reg0 << 24U) | (reg1 << 16U) | (reg2 << 8U) | reg3);
}

/*!
 * @brief Config the P-Flash protection to the intended protection status.
 *
 * @details Setting P-Flash protection status is subject to a protection transition
 * restriction. If there is a setting violation, it returns an error code
 * and the current protection status will not be changed.
 *
 * @param protectStatus: The protectStatus variable.
 *
 * @retval None.
 */
STATUS_T FLASH_ConfigPFlashProtection(uint32_t protectStatus)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint8_t reg0, reg1, reg2, reg3;
    bool flg0, flg1, flg2, flg3;

    /* Get register */
    reg3 = GET_BIT_0_7(protectStatus);
    reg2 = GET_BIT_8_15(protectStatus);
    reg1 = GET_BIT_16_23(protectStatus);
    reg0 = GET_BIT_24_31(protectStatus);

    /* Write to register */
    FLASH->PFPRO3.reg = reg3;
    FLASH->PFPRO2.reg = reg2;
    FLASH->PFPRO1.reg = reg1;
    FLASH->PFPRO0.reg = reg0;

    /* Compare changes */
    flg3 = (FLASH->PFPRO3.reg != reg3) ? true : false;
    flg2 = (FLASH->PFPRO2.reg != reg2) ? true : false;
    flg1 = (FLASH->PFPRO1.reg != reg1) ? true : false;
    flg0 = (FLASH->PFPRO0.reg != reg0) ? true : false;

    /* Read the value of PGPRO registers */
    if(flg3 || flg2 || flg1 || flg0)
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}

/*!
 * @brief Read current P-Flash protection status.
 *
 *@details Considering the time consumption for getting protection is very low and even can be ignored,
 * it is not necessary to utilize the Callback function to support the time-critical events.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ReadDFlashProtection(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t * protectStatus)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    /* Check if size of DFlash = 0 */
    if(ssdCfg->dflashSize != 0U)
    {
        *protectStatus = FLASH->DFPRO.reg;
    }
    else
    {
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief Config the D-Flash protection to the intended protection status.
 *
 *@details Setting D-Flash protection status is subject to a protection transition restriction.
 * If there is a setting violation, it returns failed information
 * and the current protection status will not be changed.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ConfigDFlashProtection(const FLASH_SSD_CONFIG_T * ssdCfg, uint8_t protectStatus)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    /* Check if size of DFlash = 0 */
    if(ssdCfg->dflashSize != 0U)
    {
        FLASH->DFPRO.reg = protectStatus;
        if(protectStatus == FLASH->DFPRO.reg)
        {
            /* Do nothing */
        }
        else
        {
            returnValue = STATUS_ERROR;
        }
    }
    else
    {
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief Read which EEPROM sections of CFGRAM are protected against program and erase operations.
 *
 * @details Considering the time consumption
 * for getting protection is very low and even can be ignored, it is not necessary
 * to utilize the Callback function to support the time-critical events.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ReadEERAMProtection(uint8_t * protectStatus)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    /* Check if EERAM is set for EEPROM */
    if((FLASH->CFG.reg & 0x01U) != 1U)
    {
        returnValue = STATUS_UNSUPPORTED;
    }
    else
    {
        *protectStatus = FLASH->EEPRO.reg;
    }

    return returnValue;
}

/*!
 * @brief Config protection to the intended protection status for EEPROM us area of CFGRam.
 *
 * @details This is subject to a protection transition restriction.
 * If there is a setting violation, it returns failed information and
 * the current protection status will not be changed.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ConfigEERAMProtection(uint8_t protectStatus)
{
    STATUS_T returnValue = STATUS_SUCCESS;

    /* Check if CFGRAM is set for EEPROM */
    if(0U != (FLASH->CFG.reg & 0x01U))
    {
        FLASH->EEPRO.reg = protectStatus;
        if(protectStatus != FLASH->EEPRO.reg)
        {
            returnValue = STATUS_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* CFGRAM is not set for EEPROM */
        returnValue = STATUS_UNSUPPORTED;
    }

    return returnValue;
}

/*!
 * @brief Prepares the CFGNVM block for use as D-Flash, EEPROM backup,
 * or a combination of both and initializes the CFGRAM.
 *
 *@details The single partition choice should be used through entire life time of a given
 * application to guarantee the Flash endurance and data retention of Flash module.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_DEFlashPartition(const FLASH_SSD_CONFIG_T *ssdCfg,
                                uint8_t eeeDataSizeCode,
                                uint8_t departitionCode,
                                uint8_t csecKeySize,
                                bool uSFE,
                                bool CFGRamLoadEEEData)
{
    STATUS_T returnValue;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        /* Passing parameter to the command */
        FLASH->CCMDDATA0.reg = FTFx_PROGRAM_PARTITION;
        FLASH->CCMDDATA1.reg = csecKeySize;
        FLASH->CCMDDATA2.reg = (uint8_t)(uSFE ? 1U : 0U);
        FLASH->CCMDDATA3.reg = (uint8_t)(CFGRamLoadEEEData ? 0U : 1U);
        FLASH->CCMDDATA4.reg = eeeDataSizeCode;
        FLASH->CCMDDATA5.reg = departitionCode;

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Config to change the function of the CFGRAM.
 *
 * @details When not partitioned for emulated EEPROM,
 * the CFGRAM is typically used as traditional RAM.
 * Otherwise, the CFGRam is typically used to store EEPROM data,
 * the writing to EEPROM is normal write or quick write.
 * In addition, this function may be used to get EEPROM status
 * or complete interrupted EEPROM quick write process.
 * For example, after partitioning to have EEPROM backup, CFGRAM is used for EEPROM
 * use accordingly and if want to change CFGRAM to traditional RAM for FlashProgramSection() use,
 * call this API with the function control code is 0xFFU.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_ConfigCFGRamFunction(const FLASH_SSD_CONFIG_T * ssdCfg,
                                    FLASH_CFGRAM_FUNCTION_CONTROL_CODE_T CFGRamFunctionCode,
                                    uint16_t byteOfQuickWrite,
                                    FLASH_EEPROM_STATUS_T * const eepromStatus)
{
    STATUS_T returnValue;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        /* Passing parameter to the command */
        FLASH->CCMDDATA0.reg = FTFx_SET_EERAM;
        FLASH->CCMDDATA1.reg = (uint8_t)CFGRamFunctionCode;

        if(CFGRamFunctionCode == EEE_QUICK_WRITE)
        {
            FLASH->CCMDDATA4.reg = (uint8_t)(byteOfQuickWrite >> 0x8U);
            FLASH->CCMDDATA5.reg = (uint8_t)(byteOfQuickWrite & 0xFFU);
        }

        /* Execute the command */
        returnValue = FLASH_CommandSequence(ssdCfg);

        if((CFGRamFunctionCode == EEE_STATUS_QUERY) && (returnValue == STATUS_SUCCESS))
        {
            if(POINTER_IS_NULL(eepromStatus) == false)
            {
                eepromStatus->brownOutCode = FLASH->CCMDDATA5.reg;
                eepromStatus->sectorEraseCount = (uint16_t)((uint16_t)FLASH->CCMDDATA8.reg << 8U);
                eepromStatus->sectorEraseCount |= (uint16_t)FLASH->CCMDDATA9.reg;
                eepromStatus->numOfRecordReqMaintain = (uint16_t)((uint16_t)FLASH->CCMDDATA6.reg << 8U);
                eepromStatus->numOfRecordReqMaintain |= (uint16_t)FLASH->CCMDDATA7.reg;
            }
            else
            {
                returnValue = STATUS_ERROR;
            }
        }
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}


/*!
 * @brief Write data to CFGRAM section which is partitioned as EEPROM use for EEPROM operation.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EEEWrite(const FLASH_SSD_CONFIG_T *ssdCfg, uint32_t dest, uint32_t size, const uint8_t *data)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint8_t i = 0U;

    /* Check if EERAM bit is enabled */
    if((FLASH->CFG.reg & 0x01U) != 1U)
    {
        returnValue = STATUS_UNSUPPORTED;
    }
    else
    {
        /* Check range */
        if(((dest + size) > (ssdCfg->eeramBase + ssdCfg->eeeSize)) || (dest < ssdCfg->eeramBase))
        {
            returnValue = STATUS_ERROR;
        }

        while ((size > 0U) && (returnValue == STATUS_SUCCESS))
        {
            /* Dest is 32bit-aligned and size is not less than 4 */
            if(size >= 4U)
            {
                if (0U == (dest & 3U))
                {
                    i = 4U;
                }
            }
            else if(size >= 2U)
            {
                if (0U == (dest & 1U))
                {
                    i = 2U;
                }
            }
            else
            {
                i = 1U;
            }

            returnValue = FLASH_WaitEEWriteToFinish(ssdCfg, dest, data, i);

            /* Update data for next iteration */
            data += i;
            /* Update size for next iteration */
            size -= i;
            /* Update destination address for next iteration */
            dest += i;
        }
    }

    return returnValue;
}

/*!
 * @brief Read the current Flash security status.
 *
 * @param securityState: The pointer of securityState variable.
 *
 * @retval None.
 */
void FLASH_ReadSecurityState(uint8_t * securityState)
{
    /* Store data read from flash register */
    uint8_t regValue;

    /* Read flash security register value */
    regValue = FLASH->SEC.reg;
    (void)regValue;
    /* Check the status of the flash security bits in the security register */
    if(FLASH_SECURITY_STATE_UNSECURED != FLASH->SEC.bit.SECFLG)
    {
        /* Flash in secured state
         * Check for backdoor key security enable bit
         */
        if(FLASH_SEC_BKEYACCEN_10 == FLASH->SEC.bit.BKEYACCEN)
        {
            /* Backdoor key security enabled */
            *securityState = FLASH_SECURE_BACKDOOR_ENABLED;
        }
        else
        {
            /* Backdoor key security disabled */
            *securityState = FLASH_SECURE_BACKDOOR_DISABLED;
        }
    }
    else
    {
        /* Flash in unsecured state */
        *securityState = FLASH_NOT_SECURE;
    }
}

/*!
 * @brief Un-secures the device.
 *
 * @details Comparing the user's provided back door key with the ones in the Flash Configuration Field.
 * If they are matched, the security is released. Otherwise, an error code is returned.
 *
 * @param pSSDConfig: The pointer of FLASH_SSD_CONFIG_T.
 * @param keyBuffer: The pointer of keyBuffer variable.
 *
 * @retval None.
 */
STATUS_T FLASH_SecurityByPass(const FLASH_SSD_CONFIG_T *ssdCfg, const uint8_t *keyBuffer)
{
    STATUS_T returnValue;
    uint32_t temp;   /* Temporary variable */
    uint8_t i;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        /* Passing parameter to the command */
        FLASH->CCMDDATA0.reg = FTFx_SECURITY_BY_PASS;
        for (i = 0U; i < 8U; i++)
        {
            temp = FLASH_BASE + i + 0x08U;
            *(uint8_t *)temp = keyBuffer[i];
        }

        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Erases all Flash memory.
 *
 *@details Initializes the CFGRAM, verifies all memory contents, and then releases the MCU security.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EraseAllBlockUnsecure(const FLASH_SSD_CONFIG_T * ssdCfg)
{
    STATUS_T returnValue;

    /* Check OCIFLG to verify the previous command is completed */
    if(0U != (FLASH->STS.reg & 0x80U)>>7U)
    {
        /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
        FLASH->STS.reg = (uint8_t)0x70U;

        FLASH->CCMDDATA0.reg = FTFx_ERASE_ALL_BLOCK_UNSECURE;

        returnValue = FLASH_CommandSequence(ssdCfg);
    }
    else
    {
        returnValue = STATUS_BUSY;
    }

    return returnValue;
}

/*!
 * @brief Enable the command complete interrupt.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EnableIntForCmdComplete(void)
{
    const IRQn_Type flashIrqId = FLASH_IRQn;

    /* Enable the command complete interrupt */
    FLASH->CFG.reg |= (uint8_t)0x80;
    INT_SYS_EnableIRQ(flashIrqId);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the command complete interrupt.
 *
 * @retval STATUS_T Code.
 */
void FLASH_DisableIntForCmdComplete(void)
{
    const IRQn_Type flashIrqId = FLASH_IRQn;

    /* Disable the command complete interrupt */
    FLASH->CFG.reg &= ~(uint8_t)0x80;
    INT_SYS_DisableIRQ(flashIrqId);
}

/*!
 * @brief Enable the read collision error interrupt generation when an FTFC read collision error occurs.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EnableIntForReadCollision(void)
{
    const IRQn_Type flashIrqId = Read_Collision_IRQn;

    /* Enable the read collision error interrupt */
    FLASH->CFG.reg |= (uint8_t)0x40;
    INT_SYS_EnableIRQ(flashIrqId);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the read collision error interrupt generation when an FTFC read collision error occurs.
 *
 * @retval STATUS_T Code.
 */
void FLASH_DisableIntForReadCollision(void)
{
    const IRQn_Type flashIrqId = Read_Collision_IRQn;

    /* Disable the read collision error interrupt */
    FLASH->CFG.reg &= ~(uint8_t)0x40;
    INT_SYS_DisableIRQ(flashIrqId);
}

/*!
 * @brief Enable the platform Flash double bit fault detect interrupt
 *
 * @details Enable the platform Flash double bit fault detect interrupt
 * generation when an uncorrectable ECC fault is detected during a valid flash
 * read access from the platform flash controller.
 *
 * @retval STATUS_T Code.
 */
STATUS_T FLASH_EnableIntDoubleBitFault(void)
{
    /* Enable the double bit fault detect interrupt */
    FLASH->ECFG.reg |= (uint8_t)0x02U;
#if (FEATURE_FLASH_INTERRUPT_DOUBLE_BIT_FAULT_IRQ == 0U)
    INT_SYS_EnableIRQ(FLASH_IRQn);
#else
    INT_SYS_EnableIRQ(FLASH_Fault_IRQn);
#endif
    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the platform Flash double bit fault detect interrupt
 *
 * @retval STATUS_T Code.
 */
void FLASH_DisableIntDoubleBitFault(void)
{
    /* Disable the double bit fault detect interrupt */
    FLASH->ECFG.reg &= ~(uint8_t)0x02U;
#if (FEATURE_FLASH_INTERRUPT_DOUBLE_BIT_FAULT_IRQ == 0U)
    INT_SYS_DisableIRQ(FLASH_IRQn);
#else
    INT_SYS_DisableIRQ(FLASH_Fault_IRQn);
#endif
}




/*!
 * @brief P-Flash read protection status.
 *
 * @details This API read the current P-Flash protection status.
 * Considering the time consumption for getting protection is very low
 * and even can be ignored. It is not necessary to utilize the Callback
 * function to support the time-critical events.
 *
 * @param protectStatus: To return the current value of the P-Flash Protection.
 *                          Each bit is corresponding to protection of 1/32 of the total P-Flash.
 *                          The least significant bit is corresponding
 *                          to the lowest address area of P-Flash. The most significant bit
 *                          is corresponding to the highest address area of P-Flash
 *                          and so on. There are two possible cases as below:
 *                          - 0: this area is protected.
 *                          - 1: this area is unprotected.
 */
FUNCTION_DEFINITION_AT_RAMSECTION_START
static STATUS_T FLASH_CommandSequence(const FLASH_SSD_CONFIG_T * ssdCfg)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    /* Clear OCIFLG to launch command */
    FLASH->STS.reg = (uint8_t)(0x80U);

    while (((FLASH->STS.reg >> 7U) & 0x01U) == 0)
    {
        /* Wait till OCIFLG bit is set
         * Serve callback function as often as possible
         */
        if(ssdCfg->callBack != CALLBACK_IS_NULL)
        {
            /* Temporarily disable compiler's check for ROM access call from within a ram function.
             * The use of a function pointer type makes this check irrelevant.
             * Nevertheless, it is imperative that the user-provided callback be defined in RAMSECTION */
            FUNCTION_CALL_AT_RAMSECTION_CHECK_DISABLE
            (ssdCfg->callBack)();
            FUNCTION_CALL_AT_RAMSECTION_CHECK_ENABLE
        }
    }
    /* Check for protection violation error */
    if ((FLASH->STS.reg & (0x01U | 0x10U |0x20U | 0x40U)) != 0U)
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}
FUNCTION_DEFINITION_AT_RAMSECTION_END


/*******************************************************************************
                        PRIVATE FUNCTIONS
*******************************************************************************/
/*!
 * @brief Tests a previously programmed P-Flash or D-Flash long word.
 *
 * @details Check if it reads correctly at the specified margin level. This
 * API always returns STATUS_SUCCESS if size provided by user is zero
 * regardless of the input validation
 *
 * @retval STATUS_T Code.
 */
static STATUS_T FLASH_ProgramCheckExecute(const FLASH_SSD_CONFIG_T *ssdCfg,
                                            uint32_t dest, uint32_t size,
                                            const uint8_t *expectedData,
                                            uint32_t *failAddr, uint8_t marginLevel)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t offsetAddress;
    uint32_t temp;
    uint32_t tempSize = size;
    uint8_t i;

    /* Check if the destination is aligned or not */
    offsetAddress = ssdCfg->dflashBase;
    if((dest >= offsetAddress) && (dest < (offsetAddress + ssdCfg->dflashSize)))
    {
        dest += 0x800000U - offsetAddress;
    }
    else
    {
        offsetAddress = ssdCfg->pflashBase;
        if((dest >= offsetAddress) && (dest < (offsetAddress + ssdCfg->pflashSize)))
        {
            dest -= offsetAddress;
        }
        else
        {
            tempSize = 0U;
            returnValue = STATUS_ERROR;
        }
    }

    while ((STATUS_SUCCESS == returnValue) && (tempSize > 0U))
    {
        /* Check OCIFLG to verify the previous command is completed */
        if(0U != (FLASH->STS.reg & 0x80U)>>7U)
        {
            /* Clear COLEFLG & ACCEFLG & PROVFLG flag in flash status register. Write 1 to clear */
            FLASH->STS.reg = 0x70U;

            FLASH->CCMDDATA0.reg = FTFx_PROGRAM_CHECK;
            FLASH->CCMDDATA1.reg = GET_BIT_16_23(dest);
            FLASH->CCMDDATA2.reg = GET_BIT_8_15(dest);
            FLASH->CCMDDATA3.reg = GET_BIT_0_7(dest);
            FLASH->CCMDDATA4.reg = marginLevel;

            for (i = 0U; i < FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD; i++)
            {
                temp = FLASH_BASE + i + 0x0CU;
                *(uint8_t *)(temp) = expectedData[i];
            }

            returnValue = FLASH_CommandSequence(ssdCfg);

            if(STATUS_SUCCESS != returnValue)
            {
                if(dest < 0x800000U)
                {
                    *failAddr = dest + offsetAddress;
                }
                else
                {
                    *failAddr = dest + offsetAddress - 0x800000U;
                }
            }
            else
            {
                /* Update destination address for next iteration */
                dest += FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD;
                /* Increment the source address by 1 */
                expectedData += FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD;
                /* Update size for next iteration */
                tempSize -= FEATURE_FLASH_ADDRESS_ALIGMENT_PF_CHECK_CMD;
            }
        }
        else
        {
            returnValue = STATUS_BUSY;
        }
    }

    return returnValue;
}

/*!
 * @brief Write to EEPROM with data was aligned and wait until operation finish.
 *
 * @retval STATUS_T Code.
 */
static STATUS_T FLASH_WaitEEWriteToFinish(const FLASH_SSD_CONFIG_T *ssdCfg,
                                            uint32_t dest,
                                            const uint8_t *data,
                                            uint8_t step)
{
    STATUS_T returnValue = STATUS_SUCCESS;
    uint32_t temp;
    uint32_t exit;

    if(step == 0x01U)
    {
        *(uint8_t *)dest = *data;
    }
    if(step == 0x02U)
    {
        temp = (uint32_t)(data[1]) << 8U;
        temp |= (uint32_t)(data[0]);
        *(volatile uint16_t *)dest = (uint16_t)temp;
    }
    if(step == 0x04U)
    {
        temp =  (uint32_t)(data[3]) << 24U;
        temp |= (uint32_t)(data[2]) << 16U;
        temp |= (uint32_t)(data[1]) << 8U;
        temp |= (uint32_t)(data[0]);
        *(volatile uint32_t *)dest = temp;
    }

    exit = 0U;
    while (((FLASH->CFG.reg & 0x01U) == 0U) && (exit == 0U))
    {
        /* Wait till EEERDY bit is set
         * Serve callback function as often as possible
         */
        if(ssdCfg->callBack != CALLBACK_IS_NULL)
        {
            (ssdCfg->callBack)();
        }

        /* Check for protection violation error */
        if ((FLASH->STS.reg & (0x01U | 0x10U |0x20U | 0x40U)) != 0U)
        {
            exit = 1U;
        }

    }

    /* Check for protection violation error */
    if ((FLASH->STS.reg & (0x01U | 0x10U |0x20U | 0x40U)) != 0U)
    {
        returnValue = STATUS_ERROR;
    }

    return returnValue;
}

/*******************************************************************************
 *                          HARDWARE ACCESS FUNCTIONS
 ******************************************************************************/

/*!
 * @brief Read the command complete flag has completed or not.
 *
 * @retval the command complete flag
 *        - true: The FTFC command has completed.
 *        - false: The FTFC command is in progress.
 */
bool FLASH_HW_ReadCmdCompleteFlag(void)
{
    return (((FLASH->STS.reg >> 7U) & 0x01U) != 0U) ? true : false;
}


/*!
 * @brief Read the read collision error flag is detected or not.
 *
 * @retval the read collision error flag
 *        - true: Collision error detected.
 *        - false: No collision error detected.
 */
bool FLASH_HW_ReadCollisionFlag(void)
{
    return (((FLASH->STS.reg >> 6U) & 0x01U) != 0U) ? true : false;
}

/*!
 * @brief Clear the read collision error flag.
 *
 * @retval None.
 */
void FLASH_HW_ClearReadCollisionFlag(void)
{
    FLASH->STS.reg |= (uint8_t)(0x40U);
}

/*!
 * @brief Check the double bit fault flag is detected during a valid
 * flash read access from the platform flash controller
 *
 * @retval the platform flash error status
 *        - true: Double bit fault detected.
 *        - false: Double bit fault not detected.
 */
bool FLASH_HW_ReadDoubleBitFaultFlag(void)
{
    return (((FLASH->ESTS.reg >> 1U) & 0x01U) != 0U) ? true : false;
}

/*!
 * @brief Clear the platform Flash double bit fault detect flag.
 * @retval None.
 */
void FLASH_HW_ClearDoubleBitFaultFlag(void)
{
    FLASH->ESTS.reg |= (uint8_t)(0x02U);
}

/*!
 * @brief Force Double Bit Fault Detect.
 *
 * This API will enable the user to emulate the setting of the double bit fault
 * detect interrupt flag to check the associated interrupt routine.
 *
 * @param isEnable Enable/disable the user to emulate the double bit fault detect flag.
 */
void FLASH_HW_ForceDoubleBitFaultDetectCmd(bool isEnable)
{
    if(false == isEnable)
    {
        FLASH->ECFG.reg &= ~(uint8_t)(0x20U);
    }
    else
    {
        FLASH->ECFG.reg |= (uint8_t)(0x02U);
    }
}


/**@} end of group FLASH_Functions*/

/**@} end of group FLASH_Driver*/
/**@} end of group APM32F445_446_StdPeriphDriver*/

