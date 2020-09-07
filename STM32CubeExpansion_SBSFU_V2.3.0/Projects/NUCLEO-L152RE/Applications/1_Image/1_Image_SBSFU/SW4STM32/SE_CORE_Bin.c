/**
  ******************************************************************************
  * @file    SE_CORE_Bin.c
  * @author  MCD Application Team
  * @brief   SFU FLASH Interface STM32L1xx module
  *          This file provides set of firmware functions to manage SFU FLASH
  *          Interface for STM32L1xx.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright(c) 2017 STMicroelectronics International N.V.
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
#include "string.h"

#if defined (__GNUC__)
asm(".section SE_CORE_Bin,\"a\";"
                             ".incbin \"./../../1_Image_SECoreBin/SW4STM32/NUCLEO-L152RE_1_Image_SECoreBin/Debug/SECoreBin.bin\";"
);
#endif

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/