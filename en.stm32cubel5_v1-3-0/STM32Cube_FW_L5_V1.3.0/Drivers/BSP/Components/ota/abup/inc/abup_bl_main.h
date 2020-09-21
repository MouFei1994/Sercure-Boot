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
 * @file        abup_bl_main.h
 *
 * @brief       The abup bootloader main header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_BL_MAIN_H__
#define __ABUP_BL_MAIN_H__

#define __MULTI_PACKAGE__
#include "abup_hal_uart.h"
#include "abup_typedef.h"

typedef struct multi_head_info
{
    abup_uint32 multi_bin_offset;
    abup_uint32 multi_bin_address;
    abup_uint8  multi_bin_method;
    abup_uint8  multi_bin_resever[7];
} multi_head_info_patch;

/* Error code */
#define ABUP_FUDIFFNET_ERROR_NONE                     (0)
#define ABUP_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF (-100)

/* Updating related */
#define ABUP_FUDIFFNET_ERROR_UPDATE_ERROR_START (-600)
#define ABUP_FUDIFFNET_ERROR_UPDATE_ERROR_END   (-699)

abup_extern void       AbupProcedure(void);
abup_extern void       abup_patch_progress(abup_uint16 total, abup_uint16 current);
abup_extern abup_int32 abup_bl_write_block(abup_uint8 type, abup_uint8* src, abup_uint32 start, abup_uint32 size);
abup_extern abup_int32 abup_bl_read_block(abup_uint8 type, abup_uint8* dest, abup_uint32 start, abup_uint32 size);
abup_extern void       abup_bl_progress(abup_int32 percent);
abup_extern abup_uint8* abup_bl_get_working_buffer(void);
abup_extern abup_uint   abup_bl_get_working_buffer_len(void);
abup_extern abup_uint32 abup_bl_get_app_base(void);
abup_extern abup_uint32 abup_bl_get_diff_param_size(void);
abup_extern abup_uint32 abup_bl_get_delta_base(void);
#endif
