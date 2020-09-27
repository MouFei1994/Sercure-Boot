/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        abup_hal_flash.h
 *
 * @brief       The abup hal flash header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_HAL_FLASH_H__
#define __ABUP_HAL_FLASH_H__

#include "abup_typedef.h"

#if defined(STM8L05xx8)
#include "stm8l15x_flash.h"
#endif

#if defined(STM32L0_DISCO) || defined(STM32L0_NUCLEO)
#include "stm32l0xx_hal.h"
#endif

#if defined(STM32F030CC)
#include "stm32f0xx_hal.h"
#endif

#if defined(STM32L4_NUCLEO) || defined(STM32L452RCT6)
#include "stm32l4xx_hal.h"
#endif

#if defined(STM32G0_NUCLEO)
#include "stm32g0xx_hal.h"
#endif

#ifdef STM32L4_DISCO
#include "stm32l4xx_hal.h"
#include "stm32l4r9i_discovery.h"
#endif

#if defined(STM32F103RC)
#include "stm32f1xx_hal.h"
#endif

#ifdef STM32F4_NUCLEO
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#endif

#ifdef STM32F4_DISCO
#include "stm32f4xx_hal.h"
/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0  ((abup_uint32)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1  ((abup_uint32)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2  ((abup_uint32)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3  ((abup_uint32)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4  ((abup_uint32)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5  ((abup_uint32)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6  ((abup_uint32)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7  ((abup_uint32)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8  ((abup_uint32)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9  ((abup_uint32)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10 ((abup_uint32)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11 ((abup_uint32)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12 ((abup_uint32)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13 ((abup_uint32)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14 ((abup_uint32)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15 ((abup_uint32)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16 ((abup_uint32)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17 ((abup_uint32)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18 ((abup_uint32)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19 ((abup_uint32)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20 ((abup_uint32)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21 ((abup_uint32)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22 ((abup_uint32)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23 ((abup_uint32)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */
#endif

#ifdef STM32F7_DISCO
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#endif

#ifdef STM32L5xx
#include "stm32l5xx.h"
#endif

#define ABUP_FLASH_BASE_END 0x08080000
extern abup_int abup_erase_sector(abup_uint32 addr, abup_uint size);
extern abup_int abup_flash_write(abup_uint32 addr, abup_uint8* buf, abup_uint size);
extern abup_int abup_flash_read(abup_uint32 addr, abup_uint8* buf, abup_uint size);

#endif
