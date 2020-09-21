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
 * @file        abup_hal.c
 *
 * @brief       Implement hardware abstraction layer
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "abup_hal.h"
#include "abup_hal_uart.h"

#if defined(ABUP_ONEOS)
#include "fal.h"
#else
#include "abup_hal_flash.h"
#endif

abup_char            abup_hal_buf[ABUP_BUF_MAX_LEN];
volatile abup_uint32 abup_tick = 0;

#ifdef ABUP_BOOTLOADER
#if ABUP_DEFAULT_SECTOR_SIZE > (0x1000)
ABUP_ALIGN(8) abup_uint8 abup_hal_flash_tmp[0x1000];
#else
ABUP_ALIGN(8) abup_uint8 abup_hal_flash_tmp[ABUP_DEFAULT_SECTOR_SIZE];
#endif
#else
abup_update_struct abup_hal_flash_tmp;
#endif

abup_update_struct* abup_hal_update = NULL;

abup_weak abup_char* abup_get_firmware_version(void)
{
    abup_debug_override_function();
    return (abup_char*)NULL;
}

abup_weak abup_uint32 Abup_GetTick(void)
{
    abup_debug_override_function();
    return abup_tick;
}

abup_weak abup_uint abup_timer_dec_offset(void)
{
    abup_debug_override_function();
    return 1;
}

abup_weak void Abup_IncTick(void)
{
    abup_debug_override_function();
}

abup_weak void Abup_Delay(abup_uint32 Delay)
{
    abup_uint32 tickstart = Abup_GetTick();
    abup_uint32 wait      = Delay;

    /* Add a period to guaranty minimum wait */
    if (wait < ABUP_MAX_DELAY)
    {
        wait += abup_timer_dec_offset();
    }

    while ((Abup_GetTick() - tickstart) < wait)
    {
    }
}

abup_char* abup_get_buf(void)
{
    return abup_hal_buf;
}

abup_uint abup_get_buf_len(void)
{
    return ABUP_BUF_MAX_LEN;
}

void abup_reset_buf(void)
{
    abup_memset(abup_hal_buf, 0, ABUP_BUF_MAX_LEN);
}

abup_weak abup_uint abup_hal_external_flash_get_blocksize(void)
{
    abup_debug_override_function();
    return ABUP_DEFAULT_SECTOR_SIZE;
}

abup_uint abup_hal_get_blocksize(void)
{
    abup_debug_override_function();
    return ABUP_DEFAULT_SECTOR_SIZE;
}

abup_weak abup_uint abup_hal_get_true_blocksize(abup_uint8 type)
{
    abup_debug_override_function();
    return abup_hal_get_blocksize();
}

abup_uint32 abup_hal_get_info_addr(void)
{
    return ABUP_UPDATE_ADDR + ABUP_UPDATE_SIZE - abup_hal_get_true_blocksize(ABUP_FILETYPE_PATCH);
}

abup_uint32 abup_hal_get_backup_addr(void)
{
    return ABUP_UPDATE_ADDR + ABUP_UPDATE_SIZE - 2 * abup_hal_get_true_blocksize(ABUP_FILETYPE_PATCH);
}

abup_uint32 abup_hal_get_app_addr(void)
{
    return ABUP_FLASH_APP_ADDR;
}

abup_uint32 abup_hal_get_delta_addr(void)
{
    return ABUP_UPDATE_ADDR;
}

abup_uint32 abup_hal_get_delta_size(void)
{
    return ABUP_UPDATE_SIZE - 2 * abup_hal_get_true_blocksize(ABUP_FILETYPE_PATCH);
}

abup_bool abup_hal_erase_sector(abup_uint8 type, abup_uint32 addr)
{
    abup_int abup_result = 0;
    abup_int blocksize   = abup_hal_get_true_blocksize(type);
    if ((addr % blocksize) == 0)
    {
        abup_result = abup_hal_flash_erase(type, addr, blocksize);
    }
    return (abup_result < 0) ? abup_false : abup_true;
}

#if defined(ABUP_WIFI_SSID_PWD)
abup_bool abup_hal_check_ssid_pwd(abup_char* ssid, abup_uint16 ssid_len, abup_char* pwd, abup_uint16 pwd_len)
{
    abup_update_struct* update = abup_hal_get_update();
    if (update)
    {
        if ((ssid_len > 2) && (ssid_len < sizeof(update->wifi_ssid)) && (pwd_len > 4) &&
            (pwd_len < sizeof(update->wifi_pwd)) && (!abup_str_is_same_char(ssid, ssid_len, 0)) &&
            (!abup_str_is_same_char(ssid, ssid_len, 0xFF)) && (!abup_str_is_same_char(pwd, pwd_len, 0)) &&
            (!abup_str_is_same_char(pwd, pwd_len, 0xFF)))
        {
            return abup_true;
        }
    }
    return abup_false;
}
#endif

abup_int abup_hal_get_key_version(void)
{
    return ABUP_KEY_VERSION;
}

abup_uint8 abup_hal_get_serverNO(void)
{
    abup_update_struct* update = abup_hal_get_update();
    if (update)
        return update->serverNO;
    else
        return (abup_uint8)-1;
}

abup_weak abup_uint8* abup_hal_get_mid(void)
{
    abup_update_struct* update = abup_hal_get_update();
    if (update)
        return update->mid;
    else
        return NULL;
}

abup_uint16 abup_hal_get_update_result(void)
{
    abup_update_struct* update = abup_hal_get_update();
    if (update)
        return update->update_result;
    else
        return 0;
}

void abup_hal_set_update_result(abup_uint16 result)
{
    abup_update_struct* update = abup_hal_get_update();
    if (update)
    {
        update->update_result = result;
        abup_hal_set_update(update);
    }
}

abup_update_struct* abup_hal_get_update(void)
{
    return (abup_update_struct*)abup_hal_update;
}

abup_bool abup_hal_get_inited(void)
{
    if (abup_hal_update == NULL)
    {
        return abup_false;
    }
    else
    {
        return abup_true;
    }
}

abup_update_struct* abup_hal_init_update(void)
{
    if (abup_hal_update == NULL)
    {
        abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
#ifdef ABUP_BOOTLOADER
        abup_hal_update = (abup_update_struct*)abup_hal_flash_tmp;
#else
        abup_hal_update = (abup_update_struct*)&abup_hal_flash_tmp;
#endif
        abup_memset(abup_hal_update, 0, abup_hal_update_size);
        abup_hal_flash_read(ABUP_FILETYPE_PATCH_INFO, 0, (abup_uint8*)abup_hal_update, abup_hal_update_size);
        if (abup_hal_update->inited != 0x8)
        {
            abup_hal_flash_erase(ABUP_FILETYPE_PATCH_INFO, 0, abup_hal_update_size);
            abup_memset(abup_hal_update, 0, abup_hal_update_size);
            abup_hal_update->inited = 0x8;
            /* abup_hal_update->serverNO = 0x2; test server */
            abup_hal_flash_write(ABUP_FILETYPE_PATCH_INFO, 0, (abup_uint8*)abup_hal_update, abup_hal_update_size);
            abup_memset(abup_hal_update, 0, abup_hal_update_size);
            abup_hal_flash_read(ABUP_FILETYPE_PATCH_INFO, 0, (abup_uint8*)abup_hal_update, abup_hal_update_size);
        }
    }
    return (abup_update_struct*)abup_hal_update;
}

abup_update_struct* abup_hal_rst_fota(void)
{
    abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
    abup_hal_flash_erase(ABUP_FILETYPE_PATCH_INFO, 0, abup_hal_update_size);
    abup_hal_update = NULL;
    return abup_hal_init_update();
}

abup_bool abup_hal_set_update(abup_update_struct* abup_update)
{
    abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
    if (abup_hal_erase_sector(ABUP_FILETYPE_PATCH_INFO, 0))
    {
        if (abup_hal_flash_write(ABUP_FILETYPE_PATCH_INFO, 0, (abup_uint8*)abup_update, abup_hal_update_size) ==
            abup_hal_update_size)
        {
            return abup_true;
        }
    }
    return abup_false;
}

abup_weak void AbupHalDeltaWrite(abup_bool inited, abup_bool succ)
{
    abup_debug_override_function();
}

abup_bool abup_hal_write_delta(abup_uint16 index, abup_uint8* data, abup_uint16 len)
{
    if (abup_hal_erase_sector(ABUP_FILETYPE_PATCH, index * ABUP_DATA_SEQ_MAX_LEN))
    {
        if (abup_hal_flash_write(ABUP_FILETYPE_PATCH, index * ABUP_DATA_SEQ_MAX_LEN, data, len) == len)
        {
            AbupHalDeltaWrite(abup_hal_get_inited(), abup_true);
            return abup_true;
        }
    }
    AbupHalDeltaWrite(abup_hal_get_inited(), abup_false);
    return abup_false;
}

#if defined(ABUP_ONEOS)
fal_part_t* abup_hal_get_device(abup_uint8 type)
{
    abup_char* device = NULL;
    if (type == ABUP_FILETYPE_APP)
    {
        device = ABUP_ONEOS_APP;
    }
    else if ((type == ABUP_FILETYPE_BACKUP) || (type == ABUP_FILETYPE_PATCH) || (type == ABUP_FILETYPE_PATCH_INFO))
    {
        device = ABUP_ONEOS_FLASH;
    }
    if (device != NULL)
    {
        return fal_part_find(device);
    }
    else
    {
        return NULL;
    }
}

abup_uint32 abup_hal_get_device_addr(abup_uint8 type, abup_uint32 addr)
{
    abup_uint32 address = 0;
    if (type == ABUP_FILETYPE_APP)
    {
        address = addr;
    }
    else if (type == ABUP_FILETYPE_BACKUP)
    {
        address = ABUP_UPDATE_SIZE - 2 * abup_hal_get_true_blocksize(type) + addr;
    }
    else if (type == ABUP_FILETYPE_PATCH)
    {
        address = addr;
    }
    else if (type == ABUP_FILETYPE_PATCH_INFO)
    {
        address = ABUP_UPDATE_SIZE - abup_hal_get_true_blocksize(type) + addr;
    }
    return address;
}

#else
abup_uint32 abup_hal_get_addr(abup_uint8 type, abup_uint32 addr)
{
    abup_uint32 address = 0;
    if (type == ABUP_FILETYPE_APP)
    {
        address = abup_hal_get_app_addr() + addr;
    }
    else if (type == ABUP_FILETYPE_BACKUP)
    {
        address = abup_hal_get_backup_addr() + addr;
    }
    else if (type == ABUP_FILETYPE_PATCH)
    {
        address = abup_hal_get_delta_addr() + addr;
    }
    else if (type == ABUP_FILETYPE_PATCH_INFO)
    {
        address = abup_hal_get_info_addr() + addr;
    }
    return address;
}

abup_weak abup_int AbupHalFlashRead(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_debug_override_function();
    return abup_flash_read(abup_hal_get_addr(type, addr), buf, size);
}

abup_weak abup_int AbupHalFlashWrite(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_debug_override_function();
    return abup_flash_write(abup_hal_get_addr(type, addr), buf, size);
}

abup_weak abup_int AbupHalFlashWriteErase(abup_uint8 type, abup_uint32 addr, abup_uint size)
{
    abup_debug_override_function();
    return abup_erase_sector(abup_hal_get_addr(type, addr), size);
}
#endif

abup_weak void AbupHalFlashOperation(abup_bool                 inited,
                                     abup_uint8                type,
                                     abup_uint32               addr,
                                     abup_uint                 size,
                                     abup_int                  result,
                                     ABUP_FLASH_OPERATION_TYPE optype)
{
    abup_debug_override_function();
}

abup_int abup_hal_flash_read(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_int result = 0;
#if defined(ABUP_ONEOS)
    result = fal_part_read(abup_hal_get_device(type), abup_hal_get_device_addr(type, addr), buf, size);
#else
    result = AbupHalFlashRead(type, addr, buf, size);
#endif
    AbupHalFlashOperation(abup_hal_get_inited(), type, addr, size, result, ABUP_FLASH_OPERATION_READ);
    return result;
}

abup_int abup_hal_flash_write(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_int result = 0;
#if defined(ABUP_ONEOS)
    result = fal_part_write(abup_hal_get_device(type), abup_hal_get_device_addr(type, addr), buf, size);
#else
    result = AbupHalFlashWrite(type, addr, buf, size);
#endif
    AbupHalFlashOperation(abup_hal_get_inited(), type, addr, size, result, ABUP_FLASH_OPERATION_WRITE);
    return result;
}

abup_int abup_hal_flash_erase(abup_uint8 type, abup_uint32 addr, abup_uint size)
{
    abup_int result = 0;
    if (size == 0)
    {
        size = abup_hal_get_true_blocksize(type);
    }
#if defined(ABUP_ONEOS)
    result = fal_part_erase(abup_hal_get_device(type), abup_hal_get_device_addr(type, addr), size);
#else
    result = AbupHalFlashWriteErase(type, addr, size);
#endif
    AbupHalFlashOperation(abup_hal_get_inited(), type, addr, size, result, ABUP_FLASH_OPERATION_ERASE);
    return result;
}
