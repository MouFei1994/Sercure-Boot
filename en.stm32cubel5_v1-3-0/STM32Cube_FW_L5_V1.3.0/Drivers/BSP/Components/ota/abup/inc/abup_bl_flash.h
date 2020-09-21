/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *use this file except in compliance with the License. You may obtain a copy of
 *the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *License for the specific language governing permissions and limitations under
 *the License.
 *
 * @file        abup_bl_flash.h
 *
 * @brief       The abup bootloader flash header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_BL_FLASH_H__
#define __ABUP_BL_FLASH_H__

#include "abup_typedef.h"

#ifdef ABUP_DEFAULT_SECTOR_SIZE
#define ABUP_TRUE_BLOCK_SIZE ABUP_DEFAULT_SECTOR_SIZE
#define ABUP_BLOCK_SIZE      ABUP_DEFAULT_SECTOR_SIZE
#define ABUP_DIFF_PARAM_SIZE ABUP_DEFAULT_SECTOR_SIZE
#define NOR_PAGE_SIZE        ABUP_DEFAULT_SECTOR_SIZE
#elif defined BLOCK_SIZE_64
#define ABUP_TRUE_BLOCK_SIZE (4 * 1024)
#define ABUP_BLOCK_SIZE      (64 * 1024)
#define ABUP_DIFF_PARAM_SIZE (64 * 1024) /* 64*1024, modified for mt2523 */
#elif defined BLOCK_SIZE_32
#define ABUP_TRUE_BLOCK_SIZE (32 * 1024)
#define ABUP_BLOCK_SIZE      (32 * 1024)
#define ABUP_DIFF_PARAM_SIZE (32 * 1024)
#elif defined BLOCK_SIZE_8
#define ABUP_TRUE_BLOCK_SIZE (4 * 1024)
#define ABUP_BLOCK_SIZE      (8 * 1024)
#define ABUP_DIFF_PARAM_SIZE (8 * 1024)
#else
#define ABUP_BLOCK_SIZE      (4 * 1024)
#define ABUP_DIFF_PARAM_SIZE (4 * 1024) /* 64*1024, modified for mt2523 */
#define ABUP_TRUE_BLOCK_SIZE (4 * 1024)
#endif

#ifndef NOR_PAGE_SIZE
#ifdef __SERIAL_FLASH_EN__
#define NOR_PAGE_SIZE (512) /* 512 bytes */
#else
#define NOR_PAGE_SIZE (4 * 1024) /* 2048 bytes, modified for reduce rom */
/* region */
#endif
#endif

abup_uint32 abup_bl_flash_block_size(void);
abup_uint32 abup_bl_flash_backup_size(void);
abup_uint32 abup_bl_fota_get_block_size_length(abup_uint32 addr);
abup_uint32 abup_bl_flash_backup_base(void);
abup_uint32 abup_bl_flash_delta_base(void);
abup_uint32 abup_hal_get_delta_size(void);
abup_uint32 AbupGetFlashDiskSize(void);
abup_uint32 abup_bl_get_rtos_len(void);
abup_uint32 abup_bl_get_rtos_base(void);
abup_uint32 abup_bl_get_rom_base(void);
abup_int32  abup_bl_read_block(abup_uint8 type, abup_uint8* dest, abup_uint32 start, abup_uint32 size);
abup_int32  abup_bl_write_block(abup_uint8 type, abup_uint8* src, abup_uint32 start, abup_uint32 size);
abup_uint32 abup_bl_read_flash(abup_uint8 type, abup_uint32 offset, abup_uint8* dest, abup_uint32 size);
abup_int32  abup_bl_erase_block(abup_uint8 type, abup_uint32 addr);
abup_uint32 abup_bl_erase_delata(void);
void        abup_bl_erase_backup_region(void);
abup_uint32 abup_bl_init_flash(void);
abup_int32  abup_bl_write_backup_region(abup_uint8* data_ptr, abup_uint32 len);
abup_int32  abup_bl_read_backup_region(abup_uint8* data_ptr, abup_uint32 len);
#endif
