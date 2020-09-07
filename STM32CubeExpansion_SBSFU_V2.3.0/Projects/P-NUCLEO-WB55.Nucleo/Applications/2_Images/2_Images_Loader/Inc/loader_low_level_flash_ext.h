/**
  ******************************************************************************
  * @file    loader_low_level_flash_ext.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update low level
  *          interface for external flash.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOADER_LOW_LEVEL_FLASH_EXT_H
#define LOADER_LOW_LEVEL_FLASH_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "loader_low_level_flash.h"
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define EXTERNAL_FLASH_ADDRESS  0x90000000U

/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Init(void);
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Erase_Size(void *pStart, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Write(void *pDestination, const void *pSource, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Read(void *pDestination, const void *pSource, uint32_t Length);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_EXT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
