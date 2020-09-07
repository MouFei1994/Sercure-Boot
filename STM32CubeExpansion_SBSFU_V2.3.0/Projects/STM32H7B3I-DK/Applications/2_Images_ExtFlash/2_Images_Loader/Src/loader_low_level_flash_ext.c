/**
  ******************************************************************************
  * @file    loader_low_level_flash_ext.c
  * @author  MCD Application Team
  * @brief   SFU Flash Low Level Interface module
  *          This file provides set of firmware functions to manage SFU external
  *          flash low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "loader_low_level_flash.h"
#include "stm32h7b3i_discovery_ospi.h"


/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  This function initialize OSPI interface in DTR OPI mode
  * @note   DTR OPI : instruction, address and data on eight lines with sampling on both edges of clock
  * @param  none
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Init(void)
{
  BSP_OSPI_NOR_Init_t flash;

  /* OSPI device configuration
     DTR OPI : instruction, address and data on eight lines with sampling on both edgaes of clock */
  flash.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
  flash.TransferRate  = BSP_OSPI_NOR_DTR_TRANSFER;
  if (BSP_OSPI_NOR_Init(0, &flash) != BSP_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in external OSPI flash
  * @param  pStart: flash address to be erased
  * @param  Length: number of bytes
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Erase_Size(void *pStart, uint32_t Length)
{
  HAL_StatusTypeDef e_ret_status = HAL_OK;
  uint32_t i;
  uint32_t address;

  /* flash address to erase is the offset from begin of external flash */
  address = (uint32_t) pStart - EXTERNAL_FLASH_ADDRESS;

  /* Loop on 64KBytes block */
  for (i = 0 ; i < ((Length - 1U) / 0x10000U) + 1U; i++)
  {
    if (BSP_OSPI_NOR_Erase_Block(0, address, BSP_OSPI_NOR_ERASE_64K) != BSP_ERROR_NONE)
    {
      e_ret_status = HAL_ERROR;
    }

    /* next 64KBytes block */
    address += 0x10000U;

    /* reload watch dog */
    WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);
  }
  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in external OSPI flash.
  * @param  pDestination: flash address to write
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: number of bytes
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Write(void  *pDestination, const void *pSource, uint32_t Length)
{
  /* Do nothing if Length equal to 0 */
  if (Length == 0)
  {
    return HAL_OK;
  }

  /* flash address to write is the offset from begin of external flash */
  if (BSP_OSPI_NOR_Write(0, (uint8_t *) pSource, (uint32_t) pDestination - EXTERNAL_FLASH_ADDRESS,
                         Length) != BSP_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  This function reads external OSPI flash
  * @param  pDestination: pointer on buffer to store data
  * @param  pSource: flash address to read
  * @param  Length: number of bytes
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  /* ensure previous operation is finished (erase/write) : GetStatus()
     such verification is done (inside BSP driver) at the begining of erase or write operation  but
     not for read operation ==> in order to maximise BSP driver execution timing efficency */
  while (BSP_OSPI_NOR_GetStatus(0) !=  BSP_ERROR_NONE)
  {
    HAL_Delay(1);
  }

  /* flash address to read is the offset from begin of external flash */
  if (BSP_OSPI_NOR_Read(0, (uint8_t *) pDestination, (uint32_t) pSource - EXTERNAL_FLASH_ADDRESS,
                        Length) != BSP_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
