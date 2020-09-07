/**
  ******************************************************************************
  * @file    fw_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage Firmware
  *          Update functionalities.
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
#include "se_def.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"
#include "stm32h7xx_hal.h"
#include "stm32h7b3i_discovery.h"
#include "fw_update_app.h"
#include "sfu_fwimg_regions.h"
#include "string.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */

/* Private defines -----------------------------------------------------------*/

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  FW_UPDATE Firmware Update Example
  * @{
  */

/**
  * @}
  */
/** @defgroup  FW_UPDATE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  FW_UPDATE_Control_Functions Control Functions
  * @{
   */

/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
#if defined(EXTERNAL_LOADER)
HAL_StatusTypeDef FW_UPDATE_Run(void)
{
  /* Print Firmware Update welcome message */
  printf("\r\n================ New Fw Download =========================\r\n\n");

  /* Standalone loader communication : execution requested */
  (*(uint32_t *)LOADER_COM_REGION_RAM_START) = STANDALONE_LOADER_BYPASS_REQ;

  NVIC_SystemReset();

  return HAL_OK;
}

#else
HAL_StatusTypeDef FW_UPDATE_Run(void)
{
  printf("  Feature not supported ! \r\n");
  return HAL_ERROR;
}

#endif


/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  FW_UPDATE_Private_Functions
  * @{
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
