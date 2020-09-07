/**
  ******************************************************************************
  * @file    loader_low_level_flash.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update flash
  *          low level interface.
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
#ifndef LOADER_LOW_LEVEL_FLASH_H
#define LOADER_LOW_LEVEL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define INVALID_SECTOR 0xFFFFFFFFU
#define INVALID_PAGE   0xFFFFFFFFU
#define INVALID_BANK   0xFFFFFFFFU

/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef LOADER_LL_FLASH_Init(void);
HAL_StatusTypeDef LOADER_LL_FLASH_Erase_Size(void *pStart, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_Write(void *pDestination, const void *pSource, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length);

uint32_t LOADER_LL_FLASH_GetSector(uint32_t Addr);
uint32_t LOADER_LL_FLASH_GetBank(uint32_t Addr);
/* Includes using previous definition -------------------------*/
#include "loader_low_level_flash_int.h"
#include "loader_low_level_flash_ext.h"

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
