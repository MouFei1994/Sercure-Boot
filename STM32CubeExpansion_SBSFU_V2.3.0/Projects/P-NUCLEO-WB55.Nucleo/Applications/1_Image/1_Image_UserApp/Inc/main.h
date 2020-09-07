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
#include "stm32wbxx_hal.h"
#include "stm32wbxx_nucleo.h"
#include "com.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor UARTx instance used and associated
   resources */

#define BUTTON_INIT()         BSP_PB_Init(BUTTON_SW1,BUTTON_MODE_GPIO);
#define BUTTON_PUSHED()      (BSP_PB_GetState(BUTTON_SW1) == GPIO_PIN_RESET)

/* Definition for USARTx clock resources */
#define SFU_UART                                USART1
#define SFU_UART_CLK_ENABLE()                   __HAL_RCC_USART1_CLK_ENABLE()
#define SFU_UART_CLK_DISABLE()                  __HAL_RCC_USART1_CLK_DISABLE()

#define SFU_UART_TX_AF                          GPIO_AF7_USART1
#define SFU_UART_TX_GPIO_PORT                   GPIOB
#define SFU_UART_TX_PIN                         GPIO_PIN_6
#define SFU_UART_TX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SFU_UART_TX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

#define SFU_UART_RX_AF                          GPIO_AF7_USART1
#define SFU_UART_RX_GPIO_PORT                   GPIOB
#define SFU_UART_RX_PIN                         GPIO_PIN_7
#define SFU_UART_RX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SFU_UART_RX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/* Maximum Timeout values for flags waiting loops.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define SFU_UART_TIMEOUT_MAX                   1000U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
