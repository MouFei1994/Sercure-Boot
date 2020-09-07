/**
  ******************************************************************************
  * @file    app_hw.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update hardware
  *          interface.
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
#ifndef APP_HW_H
#define APP_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @addtogroup SFU_APP SFU Application Configuration
  * @{
  */

/** @defgroup SFU_CONFIG_BUTTON Button Configuration
  * @{
  */

#define BUTTON_INIT() do { \
                           GPIO_InitTypeDef  GPIO_InitStruct; \
                           GPIO_InitStruct.Pin = GPIO_PIN_15; \
                           GPIO_InitStruct.Pull = GPIO_PULLUP; \
                           GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
                           GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
                           __HAL_RCC_GPIOA_CLK_ENABLE(); \
                           HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); \
                         } while (0)
#define BUTTON_PUSHED()      (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)\
                              == GPIO_PIN_SET)

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

#endif /* APP_HW_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

