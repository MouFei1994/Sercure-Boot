/**
  ******************************************************************************
  * @file    se_user_code.c
  * @author  MCD Application Team
  * @brief   Secure Engine User code example module.
  *          This file demonstrates how to call user defined services running
  *          in Secure Engine.
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
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "se_user_code.h"
#include "se_interface_application.h"
#include <string.h> /* needed for memset */


/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  SE_USER_CODE Secure Engine User Code Example
  * @brief Example of user defined code running in Secure Engine.
  *        This code provides user defined services to the user application.
  * @{
  */

/** @defgroup  SE_USER_CODE_Private_Variables Private Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup  SE_USER_CODE_Private_Functions Private Functions
  * @{
  */
static void SE_USER_CODE_PrintWelcome(void);

/**
  * @brief  Display the SE_USER_CODE Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void SE_USER_CODE_PrintWelcome(void)
{
  printf("\r\n================ Call User Defined Code running in Secure Engine =========================\r\n\n");
}


/**
  * @}
  */

/** @addtogroup  SE_USER_CODE_Exported_Functions
  * @{
  */

/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
HAL_StatusTypeDef SE_USER_CODE_Run(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  SE_ErrorStatus se_retCode = SE_ERROR;
  SE_StatusTypeDef se_Status = SE_KO;
  SE_APP_ActiveFwInfo_t sl_FwInfo;

  memset(&sl_FwInfo, 0xFF, sizeof(SE_APP_ActiveFwInfo_t));

  /* Print Firmware Update welcome message */
  SE_USER_CODE_PrintWelcome();


  /* Get FW info */
  se_retCode = SE_APP_GetActiveFwInfo(&se_Status, &sl_FwInfo);

  if ((SE_SUCCESS == se_retCode) &&
      (SE_OK == se_Status))
  {
    /* Return success */
    ret = HAL_OK;

    /* Print the result */
    printf("Firmware Info:\r\n");
    printf("\tActiveFwVersion: %d\r\n", sl_FwInfo.ActiveFwVersion);
    printf("\tActiveFwSize: %d bytes\r\n", sl_FwInfo.ActiveFwSize);
  }
  else
  {
    /* Failure */
    printf("  -- !!Operation failed!! \r\n\n");
  }

  return ret;
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
