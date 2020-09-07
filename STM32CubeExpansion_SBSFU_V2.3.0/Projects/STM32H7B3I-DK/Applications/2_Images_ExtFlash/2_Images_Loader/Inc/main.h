/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for main application file.
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7b3i_discovery.h"
#include "stdio.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_fwimg.h"
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */

/* Exported constants --------------------------------------------------------*/
#define SLOT_0_REGION_SIZE          ((uint32_t)(REGION_SLOT_0_END - REGION_SLOT_0_START + 1U))
#define SLOT_1_REGION_SIZE          ((uint32_t)(REGION_SLOT_1_END - REGION_SLOT_1_START + 1U))
#define SWAP_REGION_SIZE            ((uint32_t)(REGION_SWAP_END - REGION_SWAP_START + 1U))
#define SLOT_DWL_REGION_START       REGION_SLOT_1_START
#define SLOT_DWL_REGION_SIZE        SLOT_1_REGION_SIZE

/* Cortex-M7: align the init vectors on 0x400 instead of 0x200 */
#define IMAGE_OFFSET ((uint32_t)1024U)

#define LOADER_STATUS_LED (LED_BLUE)          /*!< LED to be used to provide the status to the end-user */

/* Exported macros -----------------------------------------------------------*/
/* #define DEBUG_MODE */                                 /* Debug printf on console */

#ifdef DEBUG_MODE
#define TRACE   printf
#else
#define TRACE(...)
#endif /* DEBUG_MODE */

/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
