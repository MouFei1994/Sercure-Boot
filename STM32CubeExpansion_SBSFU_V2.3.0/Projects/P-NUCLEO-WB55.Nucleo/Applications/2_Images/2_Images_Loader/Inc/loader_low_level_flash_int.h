/**
  ******************************************************************************
  * @file    loader_low_level_flash_int.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for internal flash low level interface.
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
#ifndef LOADER_LOW_LEVEL_FLASH_INT_H
#define LOADER_LOW_LEVEL_FLASH_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* double-word is the default setting for most platforms */
typedef uint64_t LOADER_LL_FLASH_write_t;


/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Init(void);
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Erase_Size(void *pStart, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Write(void *pDestination, const void *pSource, uint32_t Length);
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Read(void *pDestination, const void *pSource, uint32_t Length);
uint32_t LOADER_LL_FLASH_INT_GetPage(uint32_t Addr);
void NMI_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* LOADER_LOW_LEVEL_FLASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
