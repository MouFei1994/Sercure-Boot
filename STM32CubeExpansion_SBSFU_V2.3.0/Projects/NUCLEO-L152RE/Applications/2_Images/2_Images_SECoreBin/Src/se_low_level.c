/**
  ******************************************************************************
  * @file    se_low_level.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE low level
  *          interface functionalities.
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
#include "se_low_level.h"
#if defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __CC_ARM */
#include "se_exception.h"
#include "string.h"

/** @addtogroup SE Secure Engine
  * @{
  */
/** @defgroup  SE_HARDWARE SE Hardware Interface
  * @{
  */

/** @defgroup SE_HARDWARE_Private_Variables Private Variables
  * @{
  */
static CRC_HandleTypeDef    CrcHandle;                  /*!< SE Crc Handle*/


/**
  * @}
  */

/** @defgroup SE_HARDWARE_Private_Functions Private Functions
  * @{
  */

static uint32_t SE_LL_GetPage(uint32_t Address);
/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_Variables Exported Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup SE_HARDWARE_Exported_CRC_Functions CRC Exported Functions
  * @{
  */

/**
  * @brief  Set CRC configuration and call HAL CRC initialization function.
  * @param  None.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_LL_CRC_Config(void)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  CrcHandle.Instance = CRC;
  /* CRC Init*/
  if (HAL_CRC_Init(&CrcHandle) == HAL_OK)
  {
    e_ret_status = SE_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  Wrapper to HAL CRC initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_Init(void)
{
  /* CRC Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();

  return SE_LL_CRC_Config();
}

/**
  * @brief  Wrapper to HAL CRC de-initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_DeInit(void)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  if (HAL_CRC_DeInit(&CrcHandle) == HAL_OK)
  {
    /* Initialization OK */
    e_ret_status = SE_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  Wrapper to HAL CRC Calculate function.
  * @param  pBuffer: pointer to data buffer.
  * @param  uBufferLength: buffer length in 32-bits word.
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
uint32_t SE_LL_CRC_Calculate(uint32_t pBuffer[], uint32_t uBufferLength)
{
  return HAL_CRC_Calculate(&CrcHandle, pBuffer, uBufferLength);
}

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_FLASH_Functions FLASH Exported Functions
  * @{
  */

/**
  * @brief  This function does an erase of nb pages in user flash area
  * @param  pStart: pointer to  user flash area
  * @param  Length: number of bytes.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Erase(void *pStart, uint32_t Length)
{
  uint32_t page_error = 0U;
  FLASH_EraseInitTypeDef p_erase_init;
  SE_ErrorStatus e_ret_status = SE_SUCCESS;

  /* Unlock the Flash to enable the flash control register access *************/
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    /* Fill EraseInit structure*/
    p_erase_init.TypeErase     = FLASH_TYPEERASE_PAGES;
    p_erase_init.PageAddress   = (uint32_t) pStart;
    p_erase_init.NbPages       = SE_LL_GetPage(((uint32_t) pStart) + Length - 1U) -
                                 SE_LL_GetPage(p_erase_init.PageAddress) + 1U;
    if (HAL_FLASHEx_Erase(&p_erase_init, &page_error) != HAL_OK)
    {
      e_ret_status = SE_ERROR;
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    (void)HAL_FLASH_Lock();
  }
  else
  {
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Write in Flash  protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  pSource pointer to input buffer
  * @param  Length number of bytes to be written
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_LL_FLASH_Write(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret = SE_SUCCESS;
  uint32_t i;
  uint32_t pdata = (uint32_t)pSource;
  uint32_t areabegin = (uint32_t)pDestination;

  /* Test if access is in this range : SLOT 0 header */
  if (Length == 0U)
  {
    return SE_ERROR;
  }

  if ((areabegin < SFU_IMG_SLOT_0_REGION_HEADER_VALUE) ||
      ((areabegin + Length) > (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    return SE_ERROR;
  }

  /* Unlock the Flash to enable the flash control register access *************/
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    for (i = 0U; i < Length; i += 4U)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (areabegin + i), *(uint32_t *)(pdata + i)) != HAL_OK)
      {
        ret = SE_ERROR;
        break;
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) */
    (void)HAL_FLASH_Lock();
  }
  else
  {
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Read in Flash protected area
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to read
  * @param  Length: Length in bytes of data buffer
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  uint32_t areabegin = (uint32_t)pSource;
  SE_ErrorStatus e_ret_status = SE_ERROR;

  /* Test if access is in this range : SLOT 0 header or SWAP area */
  if (Length == 0U)
  {
    return e_ret_status;
  }

  if ((areabegin < SFU_IMG_SLOT_0_REGION_HEADER_VALUE) ||
      ((areabegin + Length) > (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    return e_ret_status;
  }

  (void)memcpy(pDestination, pSource, Length);
  e_ret_status = SE_SUCCESS;
  return e_ret_status;
}

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_FLASH_EXT_Functions External FLASH Exported Functions
  * @{
  */

/**
  * @brief Initilisation of external flash On The Fly DECryption (OTFDEC)
  * @param pxSE_Metadata: Firmware metadata.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_EXT_Decrypt_Init(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pxSE_Metadata);
  return SE_SUCCESS;
}

/**
  * @}
  */

/**
  * @brief Check if an array is inside the RAM of the product
  * @param Addr : address  of array
  * @param Length : legnth of array in byte
  */
SE_ErrorStatus SE_LL_Buffer_in_ram(void *pBuff, uint32_t Length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + Length - 1U;
  if ((Length != 0U) && ((addr_end  <= SB_REGION_RAM_END) && (addr_start >= (SE_REGION_RAM_END + 1))))
  {
    e_ret_status = SE_SUCCESS;
  }
  else
  {
    e_ret_status = SE_ERROR;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  return e_ret_status;
}

/**
  * @brief function checking if a buffer is in sbsfu ram.
  * @param pBuff: Secure Engine protected function ID.
  * @param length: length of buffer in bytes
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_Buffer_in_SBSFU_ram(const void *pBuff, uint32_t length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + length - 1U;
  if ((length != 0U) && ((addr_end  <= SB_REGION_RAM_END) && (addr_start >= SB_REGION_RAM_START)))
  {
    e_ret_status = SE_SUCCESS;
  }
  else
  {
    e_ret_status = SE_ERROR;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  return e_ret_status;
}

/**
  * @brief function checking if a buffer is PARTLY in se ram.
  * @param pBuff: Secure Engine protected function ID.
  * @param length: length of buffer in bytes
  * @retval SE_ErrorStatus SE_SUCCESS for buffer in se ram, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_Buffer_part_of_SE_ram(const void *pBuff, uint32_t length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + length - 1U;
  if ((length != 0U) && (!(((addr_start < SE_REGION_RAM_START) && (addr_end < SE_REGION_RAM_START)) ||
                           ((addr_start > SE_REGION_RAM_END) && (addr_end > SE_REGION_RAM_END)))))
  {
    e_ret_status = SE_SUCCESS;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  else
  {
    e_ret_status = SE_ERROR;
  }
  return e_ret_status;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SE_HARDWARE_Private_Functions
  * @{
  */

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t SE_LL_GetPage(uint32_t Address)
{
  uint32_t page;

  page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE;

  return page;
}


/**
  * @brief  NMI Handler redirection to SE NMI handler
  * @param  None.
  * @retval None.
  */
void NMI_Handler(void)
{
  SE_NMI_ExceptionHandler();
}

/**
  * @brief  Cleanup SE CORE
  * The fonction is called  during SE_LOCK_RESTRICT_SERVICES.
  *
  */
void  SE_LL_CORE_Cleanup(void)
{
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
