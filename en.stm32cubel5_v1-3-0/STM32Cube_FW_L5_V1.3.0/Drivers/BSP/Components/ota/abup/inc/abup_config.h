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
 * @file        abup_config.h
 *
 * @brief       The abup config header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_CONFIG_H__
#define __ABUP_CONFIG_H__

#include "abup_os.h"

#ifdef OS_USING_FAL
#define ABUP_ONEOS_APP        OS_APP_NAME
#define ABUP_ONEOS_FLASH      OS_DL_PART_NAME
#define ABUP_DEFAULT_SEGMENT_SIZE_INDEX 3
#define ABUP_FLASH_BASE_ADDR      MCU_ROM_BEGIN
#define ABUP_BL_SIZE              MCU_BOOT_SIZE
#define ABUP_FLASH_APP_ADDR       OS_APP_PART_ADDR
#define ABUP_APP_SIZE             OS_APP_PART_SIZE
#define ABUP_UPDATE_ADDR          OS_DL_PART_ADDR
#define ABUP_UPDATE_SIZE          OS_DL_PART_SIZE
#endif

#define ABUP_SECTOR_MAX_NUM (MCU_ROM_SIZE/ABUP_DEFAULT_SECTOR_SIZE)


#define ABUP_SLIM_RES

/* ram size for restore, if use wosun algorithm set it to all available ram, else lusun set it to no more than a sector and no more than 0xFFFF
 */
#ifndef ABUP_WORKING_BUFFER_LEN
#ifdef ABUP_ALGORITHM_WOSUN 
#define ABUP_WORKING_BUFFER_LEN  (STM32_SRAM_SIZE-10)*1024
#else 
#define ABUP_WORKING_BUFFER_LEN 0x400
#endif
#endif

/* driver macro, user to configure */
#ifndef ABUP_ONEOS
/* 0 means lusun algorithm, 1 means low-level wosun algorithm, low-level wosun for ram less than 512k, 2 means advanced
 * wosun algorithm, advanced wosun for ram greater than 512k
 */
#define ABUP_FOTA_ALGORITHM 0
#define STM32L5xx
/* if set debug, open debug mode, else user mode */
#if (ABUP_BOOTLOADER_DEBUG == 1) || (ABUP_APP_DEBUG == 1)
#ifndef ABUP_DEBUG_MODE
#define ABUP_DEBUG_MODE
#endif
#endif
/* Download segment, config by user */
#define ABUP_DEFAULT_SEGMENT_SIZE_INDEX ABUP_SEGMENT_SIZE_512_INDEX
#ifndef ABUP_ONEOS_FLASH
#define ABUP_ONEOS_FLASH "download"
#endif
#ifndef ABUP_ONEOS_APP
#define ABUP_ONEOS_APP "app"
#endif
/* Config device */
/* Sector size */
#ifdef FLASH_PAGE_SIZE
#define ABUP_DEFAULT_SECTOR_SIZE  FLASH_PAGE_SIZE
#else
#define ABUP_DEFAULT_SECTOR_SIZE 0x00000800
#endif
/* Flash base address */
#define ABUP_FLASH_BASE_ADDR 0x08000000
/* Bootloader size */
#define ABUP_BL_SIZE 0x00010000
/* App address */
#define ABUP_FLASH_APP_ADDR 0x08010000
/* App size */
#define ABUP_APP_SIZE 0x40000
/* update partition address */
#define ABUP_UPDATE_ADDR 0x08050000
/* update partition size, include diff package size and 2 sectors size */
#define ABUP_UPDATE_SIZE 0x30000

/* OEM */
#define ADUPS_FOTA_SERVICE_OEM "L452RE"
/* Device model*/
#define ADUPS_FOTA_SERVICE_MODEL "L452RE"
/* Product id */
#define ADUPS_FOTA_SERVICE_PRODUCT_ID "1551332713"
/* Product secret */
#define ADUPS_FOTA_SERVICE_PRODUCT_SEC "43d1d10afb934ec997f8a3ac2c2dde77"
/* Device type */
#define ADUPS_FOTA_SERVICE_DEVICE_TYPE "box"
/* Platform */
#define ADUPS_FOTA_SERVICE_PLATFORM "stm32l4"
/* Firmware version*/
#define ABUP_FIRMWARE_VERSION "1.0"
/* Network type */
#define ABUP_NETWORK_TYPE "NB"
#define ABUP_KEY_VERSION  2

/* IOT5.0_LUSUN11_R50426 is lusun algorithm, else if wosun algorithm, wosun default is IOT4.0_R42641 */
#define ADUPS_FOTA_LUSUN_VERSION "IOT5.0_LUSUN11_R50426"
#define ADUPS_FOTA_WOSUN_VERSION "IOT4.0_R42641"
#if (ABUP_FOTA_ALGORITHM == 0)
#define ADUPS_FOTA_SDK_VER ADUPS_FOTA_LUSUN_VERSION
#else
#define ADUPS_FOTA_SDK_VER ADUPS_FOTA_WOSUN_VERSION
#endif
#define ADUPS_FOTA_APP_VERSION "ADUPS_V4.0"
#else
#if (ABUP_BOOTLOADER_DEBUG == 1)
#ifndef ABUP_DEBUG_MODE
#define ABUP_DEBUG_MODE
#endif
#endif
#define ADUPS_FOTA_RTT_VERSION "RTT_V1.0"
#define ABUP_KEY_VERSION       3
#endif

#define ABUP_HW_VERSION "HW01"
#define ABUP_SW_VERSION "SW01"

#define ABUP_SEGMENT_SIZE_16_INDEX  0
#define ABUP_SEGMENT_SIZE_32_INDEX  1
#define ABUP_SEGMENT_SIZE_64_INDEX  2
#define ABUP_SEGMENT_SIZE_128_INDEX 3
#define ABUP_SEGMENT_SIZE_256_INDEX 4
#define ABUP_SEGMENT_SIZE_512_INDEX 5

typedef enum
{
    /* Quectel */
    ABUP_MODULE_BC28 = 0,
    /* CMCC */
    ABUP_MODULE_M5310 = 20,
    /* ESPRESSIF */
    ABUP_MODULE_ESP8266 = 40,
    ABUP_MODULE_ESP07S  = 41,
    /* SIMCom */
    ABUP_MODULE_SIM7020C = 60,
    ABUP_MODULE_SIM7600  = 61,
    /* Gosuncn */
    ABUP_MODULE_ME3630 = 80,
    /* Meig */
    ABUP_MODULE_SLM152 = 100,

    ABUP_MODULE_MAX = 0xFFFFF
} ABUP_MODULE;

/* ABUP_DEFAULT_NETWORK_PROTOCOL can be set to 0 or 1 */
#define ABUP_PROTOCOL_NONE      0
#define ABUP_PROTOCOL_COAP      1
#define ABUP_PROTOCOL_HTTP      2
#define ABUP_PROTOCOL_COAP_HTTP (ABUP_PROTOCOL_COAP | ABUP_PROTOCOL_HTTP)

/* Try count */
#define ABUP_TRY_COUNT 6
#if defined(ABUP_MODULE_ESP8266_ENABLE) || defined(ABUP_MODULE_ESP07S_ENABLE)
#define ABUP_WIFI_SSID_PWD
#endif

#endif
