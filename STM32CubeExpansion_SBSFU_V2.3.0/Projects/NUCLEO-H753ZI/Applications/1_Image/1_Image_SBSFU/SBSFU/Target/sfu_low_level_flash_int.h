/**
  ******************************************************************************
  * @file    sfu_low_level_flash_int.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update low level
  *          interface for internal flash.
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
#ifndef SFU_LOW_LEVEL_FLASH_INT_H
#define SFU_LOW_LEVEL_FLASH_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_low_level_flash.h"
#include "main.h"
#include "sfu_def.h"

/** @addtogroup SFU Secure Secure Boot / Firmware Update
  * @{
  */

/** @addtogroup SFU_LOW_LEVEL
  * @{
  */

/** @defgroup SFU_LOW_LEVEL_FLASH Flash Low Level Interface
  * @{
  */

/** @defgroup SFU_FLASH_Exported_Constants Exported Constants
  * @{
  */

/**
  * @brief Flash Write Access Constraints (size and alignment)
  *
  * For instance, on L4, it is only possible to program double word (2 x 32-bit data).
  * See http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf
  *
  * @note This type is very important for the FWIMG module (see @ref SFU_IMG).
  * \li This is the type to be used for an atomic write in FLASH: see @ref AtomicWrite.
  * \li The size of this type changes the size of the TRAILER area at the end of slot #1,
  *     as it is used to tag if a Firmware Image chunk has been swapped or not (see @ref SFU_IMG_CheckTrailerValid).
  */

/* STM32H7: The write access type is 256 bits */
typedef struct
{
  uint64_t tmp[4];
} SFU_LL_FLASH_write_t;


#if defined(SFU_FWIMG_CORE_C)
/*
 * Only sfu_fwimg_core.c should need this.
 * These elements are declared here because they are FLASH dependent so HW dependent.
 * Otherwise they would be internal items of sfu_fwimg_core.c
 */

/* Trailer pattern : sizeof of write access type */

const int8_t SWAPPED[sizeof(SFU_LL_FLASH_write_t)]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                      };
const int8_t NOT_SWAPPED[sizeof(SFU_LL_FLASH_write_t)] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                          -1, -1
                                                         };

/**
  * Length of a MAGIC tag (32 bytes).
  * This must be a multiple of @ref SFU_LL_FLASH_write_t with a minimum value of 32.
  */
#define MAGIC_LENGTH ((uint32_t)32U)

/**
  * Specific tag definition
  * This is used to erase the MAGIC patterns.
  */
const uint8_t MAGIC_NULL[MAGIC_LENGTH]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0
                                          };
#endif /* SFU_FWIMG_CORE_C */


/** @defgroup SFU_FLASH_Exported_Macros Exported Macros
  * @{
  */

/**
  * @brief Macro to make sure the FWIMG slots are properly aligned with regards to flash constraints
  * Each slot shoud start at the begining of a page/sector
  */
#define IS_ALIGNED(address) (0U == ((address) % FLASH_SECTOR_SIZE))

/**
  * @}
  */

/** @defgroup SFU_FLASH_Exported_Functions Exported Functions
  * @{
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Init(void);
SFU_ErrorStatus SFU_LL_FLASH_INT_Erase_Size(SFU_FLASH_StatusTypeDef *pxFlashStatus, void *pStart, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_INT_Write(SFU_FLASH_StatusTypeDef *pxFlashStatus, void *pDestination, const void *pSource,
                                       uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_INT_Read(void *pDestination, const void *pSource, uint32_t Length);
uint32_t SFU_LL_FLASH_INT_GetSector(uint32_t Addr);
uint32_t SFU_LL_FLASH_INT_GetBank(uint32_t Addr);
void NMI_Handler(void);
SFU_ErrorStatus ECC_Fault_Handler(void);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_INT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
