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
 * @file        abup_bl_main.c
 *
 * @brief       Implement bootloader main function
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "abup_hal_uart.h"
#include "abup_bl_main.h"
#include "abup_patch.h"
#include "abup_draw.h"
#include "abup_stdlib.h"
#include "abup_hal.h"
#if !defined(ABUP_8BIT)
#include "abup_bl_flash.h"
#include "peer_type.h"
#endif

abup_char              progress[128];
abup_extern abup_int   abup_fotapatch_procedure(void);
abup_extern abup_bool  abup_IsPackgeFound(void);
abup_extern void       abup_get_package_number(abup_uint8* delta_num);
abup_extern abup_int32 abup_get_package_info(multi_head_info_patch* info, abup_uint8 curr_delta);
ABUP_ALIGN(8) abup_int8 abup_malloc_buf[ABUP_WORKING_BUFFER_LEN];

abup_uint8            abup_delta_num  = 0;
abup_uint8            abup_curr_delta = 1;
multi_head_info_patch PackageInfo     = {0};

abup_extern abup_int  fotapatch_procedure(void);
abup_extern abup_int  frame_memory_finalize(void);
abup_extern abup_bool abup_init_mem_pool(abup_uint8* buffer, abup_uint32 len);

void abup_patch_progress_ext(abup_bool PrePatch, abup_uint16 total, abup_uint16 current)
{
    static abup_int curr = -1;
#if defined(STemWin) || defined(STM32L0_DISCO)
    static abup_int percent = 0;
#endif
    if (curr != current)
    {
        curr = current;
        abup_memset(progress, 0, sizeof(progress));
        if (PrePatch)
        {
            abup_memcpy(progress, "PrePatch:", 9);
        }
        abup_itoa(curr * 100 / total,
                  progress + abup_strlen(progress),
                  sizeof(progress) - abup_strlen(progress),
                  10,
                  abup_false);
        abup_memcpy(progress + abup_strlen(progress), "%", 1);
        abup_memcpy(progress + abup_strlen(progress), "\r\n", 2);
        abup_bl_main_printf(progress);
#if defined(STemWin) || defined(STM32L0_DISCO)
        percent = current * 100 / total;
        if (PrePatch)
        {
            abup_memcpy(progress, "PrePatch:", 9);
            AbupDisplayString(2, "PrePatch:%d%%", percent);
        }
        else
        {
            AbupDisplayString(2, "Progress:%d%%", percent);
        }
#endif
    }
}

void abup_patch_progress(abup_uint16 total, abup_uint16 current)
{
    abup_patch_progress_ext(abup_false, total, current);
}

/**
 ***********************************************************************************************************************
 * @brief           This function will begin lusun difference algorithm restoration and upgrade
 * @param[in]       void
 *
 * @retval          void
 ***********************************************************************************************************************
 */
void AbupLusunProcedure(void)
{
    abup_int16          status = ABUP_FUDIFFNET_ERROR_NONE;
    abup_update_struct* update = abup_hal_init_update();
    if (update == NULL)
    {
        abup_bl_main_printf("Failed to read parameter\r\n");
        AbupDisplayString(1, "Failed to read parameter");
    }
    if (update->update_result != 100)
    {
        abup_bl_main_printf("NO PACKAGE\r\n");
        AbupDisplayString(1, "NO PACKAGE");
        return;
    }

    if (status == ABUP_FUDIFFNET_ERROR_NONE)
    {
        status = abup_fotapatch_procedure();
    }

    if (status == ABUP_FUDIFFNET_ERROR_NONE)
    {
        abup_bl_main_printf("Update succ\r\n");
        AbupDisplayString(1, "Update succ");
    }
    else
    {
        abup_bl_main_printf("Update fail\r\n");
        AbupDisplayString(1, "Update fail");
    }

    if (status == ABUP_UPDATE_AUTH_FAIL)
    {
        update->update_result = status;
        abup_hal_set_update(update);
    }
}

#if !defined(ABUP_8BIT)
void peer_watchdog_restart(void)
{
#if defined(WIN32) || defined(LINUX)
    peer_logout("WacthDogRestart");
#else
    abup_bl_debug_print(NULL, "WacthDogRestart");
#endif
}
void abup_stream_init_decoder(STREAM_INF_FUN s_stream_fun[2], abup_int* stream_type)
{
#if (ABUP_FOTA_ALGORITHM == 1)
    peer_logout("WOSUN_BZ_ENABLE");
    *stream_type += STREAM_TYPE_BZ;
    s_stream_fun[0].init_fun = stream_init_bz_decoder;
    s_stream_fun[0].read_fun = stream_bz_read;
    s_stream_fun[0].end_fun  = stream_bz_end;
#elif (ABUP_FOTA_ALGORITHM == 2)
    peer_logout("WOSUN_LZMA_ENABLE");
    *stream_type += STREAM_TYPE_LZMA;
    s_stream_fun[1].init_fun = stream_init_lzma_decoder;
    s_stream_fun[1].read_fun = stream_lzma_read;
    s_stream_fun[1].end_fun  = stream_lzma_end;
#endif
}
abup_bool abup_get_pre_patch_flag(void)
{
    return abup_true;
}

abup_bool abup_get_pre_check_flag(void)
{
    return abup_true;
}

abup_uint32 abup_bl_get_diff_param_size(void)
{
    return ABUP_DIFF_PARAM_SIZE;
}

abup_uint32 abup_bl_get_app_base(void)
{
    return 0;
}

abup_uint32 abup_bl_get_delta_base(void)
{
    return PackageInfo.multi_bin_offset;
}

void WacthDogRestart()
{
}

void abup_wait_sec(abup_int32 sec)
{
}

abup_bool abup_IsPartPackge(void)
{
    abup_uint8 data[5];
    abup_memset(data, 0, sizeof(data));
    abup_bl_read_backup_region(data, sizeof(data));
    abup_bl_debug_print(LOG_DEBUG, "%c%c%c%c%c\r\n", data[0], data[1], data[2], data[3], data[4]);
    if (data[0] == 'A' && data[1] == 'D' && data[2] == 'U' && data[3] == 'P' && data[4] == 'S')
    {
        abup_bl_debug_print(LOG_DEBUG, "abup_IsPartPackge: YES!\r\n");
        return abup_true;
    }
    abup_bl_debug_print(LOG_DEBUG, "abup_IsPartPackge: NO!\r\n");
    return abup_false;
}

/**
 ***********************************************************************************************************************
 * @brief           This function will begin wosun difference algorithm restoration and upgrade
 * @param[in]       void
 *
 * @retval          void
 ***********************************************************************************************************************
 */
void AbupWosunProcedure(void)
{
    abup_int16 status = ABUP_FUDIFFNET_ERROR_NONE;
    abup_bl_debug_print(LOG_DEBUG, "AbupWosunProcedure  4.0\r\n");

    abup_update_struct* update = abup_hal_init_update();
    if (update == NULL)
    {
        abup_bl_main_printf("Failed to read parameter\r\n");
        AbupDisplayString(1, "Failed to read parameter");
    }
    if ((update->update_result != 100) || !abup_IsPackgeFound())
    {
        abup_bl_main_printf("NO PACKAGE\r\n");
        AbupDisplayString(1, "NO PACKAGE");
        return;
    }

    /* Init memory buffer */

    if (NULL == abup_bl_get_working_buffer())
    {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
        abup_bl_debug_print(LOG_DEBUG, "Can not alloc UPI working buffer\n");
#endif
        status = ABUP_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF;
    }

    peer_setCheckOldbin(TRUE);
    if (status == ABUP_FUDIFFNET_ERROR_NONE)
    {
        abup_init_mem_pool(abup_bl_get_working_buffer(), abup_bl_get_working_buffer_len());
        status = abup_get_package_info(&PackageInfo, abup_curr_delta);
        if (status == ABUP_FUDIFFNET_ERROR_NONE)
        {
            status = fotapatch_procedure();
        }
        frame_memory_finalize();
    }
    if (status == ABUP_FUDIFFNET_ERROR_NONE)
    {
        abup_bl_main_printf("Update succ\r\n");
        AbupDisplayString(1, "Update succ");
    }
    else
    {
        abup_bl_main_printf("Update fail\r\n");
        AbupDisplayString(1, "Update fail");
    }

    if (status == ABUP_UPDATE_AUTH_FAIL)
    {
        update->update_result = status;
        abup_hal_set_update(update);
    }

#ifdef ABUP_TEST_POWEROFF
    abup_dump_test_info();
#endif
}
#endif

abup_uint32 abup_bl_get_curr_write_address(void)
{
    return 0;
}

abup_uint8* abup_bl_get_working_buffer(void)
{
    return (abup_uint8*)abup_malloc_buf; /* (RAM_BASE+1024*500);//(abup_malloc_buf); */
}

abup_uint abup_bl_get_working_buffer_len(void)
{
#if (ABUP_FOTA_ALGORITHM == 0)
    static abup_uint working_buffer_len = 0;
    if (working_buffer_len == 0)
    {
        working_buffer_len = ABUP_WORKING_BUFFER_LEN;
        if (abup_hal_get_blocksize() < working_buffer_len)
        {
            working_buffer_len = abup_hal_get_blocksize();
        }
    }
    return working_buffer_len;
#else
    return ABUP_WORKING_BUFFER_LEN;
#endif
}

abup_bool abup_bl_is_limit_len(void)
{
#if (ABUP_WORKING_BUFFER_LEN == ABUP_DEFAULT_SECTOR_SIZE)
    return abup_true;
#else
    if (abup_hal_get_blocksize() == abup_bl_get_working_buffer_len())
    {
        return abup_true;
    }
    else
    {
        return abup_false;
    }
#endif
}

void AbupProcedure(void)
{
#if (ABUP_FOTA_ALGORITHM == 0)
    AbupLusunProcedure();
#else
    AbupWosunProcedure();
#endif
}
