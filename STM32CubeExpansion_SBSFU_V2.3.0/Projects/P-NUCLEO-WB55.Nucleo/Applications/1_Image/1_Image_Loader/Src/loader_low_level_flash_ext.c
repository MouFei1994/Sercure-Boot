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


/* Functions Definition ------------------------------------------------------*/


/* No external flash available on this product
   ==> return HAL_ERROR except for LOADER_LL_FLASH_EXT_Init which is called systematically during startup phase */

HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Init(void)
{
  return HAL_OK;
}

HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Erase_Size(void *pStart, uint32_t Length)
{
  return HAL_ERROR;
}

HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Write(void  *pDestination, const void *pSource, uint32_t Length)
{
  return HAL_ERROR;
}

HAL_StatusTypeDef LOADER_LL_FLASH_EXT_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  return HAL_ERROR;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
