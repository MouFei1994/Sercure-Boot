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
 * @file        abup_bl_flash.c
 *
 * @brief       Implement bootloader flash
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "abup_bl_flash.h"
#include "abup_hal.h"
#include "abup_typedef.h"
#include "peer_type.h"

#define HAL_FLASH_STATUS_OK          0
#define BL_FOTA_ERROR_LOADING_MARKER 0
#define BL_FOTA_ERROR_NONE           0
#define BL_FOTA_ERROR_ERASE_MARKER   0

void abup_printf_flashstatus(abup_int32 errcode)
{
    abup_bl_debug_print(0, "flashstatus:%d\n", errcode);
}

abup_uint32 abup_bl_flash_block_size(void)
{
    return ABUP_BLOCK_SIZE;
}

abup_bool AbupUseFlash(void)
{
    return abup_true;
}

abup_uint8 abup_wosun_get_type(abup_uint8 type)
{
    abup_uint8 abup_type = ABUP_FILETYPE_APP;
    switch (type)
    {
    case PEER_FILETYPE_ROM:
        abup_type = ABUP_FILETYPE_APP;
        break;
    case PEER_FILETYPE_FLASH:
        abup_type = ABUP_FILETYPE_PATCH;
        break;
    case PEER_FILETYPE_BACKUP:
        abup_type = ABUP_FILETYPE_BACKUP;
        break;
    }
    return abup_type;
}

abup_int abup_bl_flash_read(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{

    if (abup_hal_flash_read(abup_wosun_get_type(type), addr, buf, size) > -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

abup_int abup_bl_flash_write(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    if (abup_hal_flash_write(abup_wosun_get_type(type), addr, buf, size) > -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

abup_int abup_bl_flash_erase(abup_uint8 type, abup_uint32 addr, abup_uint size)
{
    if (abup_hal_flash_erase(abup_wosun_get_type(type), addr, size) > -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

abup_uint32 abup_bl_get_rom_base(void)
{
    return 0; /* NOR_BOOTING_NOR_FS_BASE_ADDRESS */
}

abup_int32 abup_bl_read_block(abup_uint8 type, abup_uint8* dest, abup_uint32 start, abup_uint32 size)
{
    long ret_val = -1; /* read error */

    abup_uint32 page_idx = 0, page_count = 0;
    abup_uint8* read_buf     = dest;
    abup_int32  read_address = start - abup_bl_get_rom_base(); /* custom_get_NORFLASH_Base() */

#ifdef ABUP_FOTA_ENABLE_DIFF_DEBUG
    /* abup_bl_debug_print(LOG_DEBUG, "abup_bl_read_block: %x(%d) =>
     * %x,read_addr=%x \r\n", start, size, dest,read_address); */
#endif

    page_count = size / NOR_PAGE_SIZE;
    for (page_idx = 0; page_idx < page_count; page_idx++)
    {
        ret_val = abup_bl_flash_read(type, read_address, read_buf, NOR_PAGE_SIZE);
        if (ret_val != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_printf_flashstatus(ret_val);
#endif
            return ret_val;
        }
        read_address += NOR_PAGE_SIZE;
        read_buf += NOR_PAGE_SIZE;
    }

    if (size % NOR_PAGE_SIZE != 0)
    {
        ret_val = abup_bl_flash_read(type, read_address, read_buf, size % NOR_PAGE_SIZE);
        if (ret_val != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_printf_flashstatus(ret_val);
#endif
            return ret_val;
        }
    }
    ret_val = size;
    return ret_val;
}

abup_int32 abup_bl_write_block(abup_uint8 type, abup_uint8* src, abup_uint32 start, abup_uint32 size)
{
    long ret_val = -1;

    abup_uint32 page_count = 0, current_page = 0;
    abup_uint32 address = start;
    abup_uint32 real_address;
    abup_int32  need_to_write = size;
    abup_int32  write_size    = size;

#if 1 /* def ABUP_FOTA_ENABLE_DIFF_DEBUG */
    abup_bl_debug_print(LOG_DEBUG, "abup_bl_write_block address = %x,size = %d \r\n", address, size);
#endif

    abup_int32  flash_status = HAL_FLASH_STATUS_OK;
    abup_uint8* buffer       = src;

#ifdef ABUP_TEST_POWEROFF
    abup_bool Breset = abup_get_reset_param(start, size);
#endif

    flash_status = abup_bl_erase_block(type, address);
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
#ifdef ABUP_FOTA_ENABLE_DIFF_DEBUG
        abup_printf_flashstatus(flash_status);
#endif
        return ret_val;
    }
    real_address = address - abup_bl_get_rom_base();
    page_count   = (abup_bl_flash_block_size() / NOR_PAGE_SIZE);
    while (write_size > 0)
    {
        if (current_page >= page_count)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_DEBUG
            abup_bl_debug_print(LOG_DEBUG, "Change block");
#endif
            current_page = 0;

            /* get block size */
            flash_status = abup_bl_erase_block(type, address);
            if (flash_status != HAL_FLASH_STATUS_OK)
            {

#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
                abup_printf_flashstatus(flash_status);
#endif
                return ret_val;
            }
            real_address = address - abup_bl_get_rom_base();
            page_count   = (abup_bl_flash_block_size() / NOR_PAGE_SIZE);
        }

        if (current_page == 0)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_DEBUG
            abup_bl_debug_print(LOG_DEBUG, "addr=%x\r\n", address);
#endif
        }

        real_address = address - abup_bl_get_rom_base();
#ifdef ABUP_TEST_POWEROFF
        if (Breset == abup_true)
        {
            if (current_page == abup_test_info.page_index && abup_test_info.reset_mode == ABUP_POWER_RESET_MODE0)
            {
                abup_bl_debug_print(LOG_DEBUG, "flash write =%d\r\n", current_page);
                abup_system_reboot();
            }
            else if (current_page == abup_test_info.page_index && abup_test_info.reset_mode == ABUP_POWER_RESET_MODE1)
            {
                flash_status = abup_bl_flash_write(real_address, (abup_uint8*)buffer, NOR_PAGE_SIZE >> 1);
                abup_bl_debug_print(LOG_DEBUG, "flash write =%d\r\n", current_page);
                abup_system_reboot();
            }
            else if (current_page == abup_test_info.page_index && abup_test_info.reset_mode == ABUP_POWER_RESET_MODE2)
            {
                flash_status = abup_bl_flash_write(real_address, (abup_uint8*)buffer, NOR_PAGE_SIZE);
                abup_bl_debug_print(LOG_DEBUG, "flash write =%d\r\n", current_page);
                abup_system_reboot();
            }
            else
            {
                flash_status = abup_bl_flash_write(real_address, (abup_uint8*)buffer, NOR_PAGE_SIZE);
                abup_bl_debug_print(LOG_DEBUG, "flash write =%d\r\n", current_page);
            }
        }
        else
        {
            flash_status = abup_bl_flash_write(real_address, (abup_uint8*)buffer, NOR_PAGE_SIZE);
            abup_bl_debug_print(LOG_DEBUG, "flash write =%d\r\n", current_page);
        }
#else
        flash_status = abup_bl_flash_write(type, real_address, (abup_uint8*)buffer, NOR_PAGE_SIZE);
#endif

        if (flash_status != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_printf_flashstatus(flash_status);
#endif
            return ret_val;
        }

        address += NOR_PAGE_SIZE;
        buffer += NOR_PAGE_SIZE;
        current_page++;
        write_size -= NOR_PAGE_SIZE;
    }

    ret_val = need_to_write - write_size;
    return ret_val;
}

abup_uint32 abup_bl_read_flash(abup_uint8 type, abup_uint32 offset, abup_uint8* dest, abup_uint32 size)
{
    abup_uint32 page_idx = 0, page_count = 0;
    abup_uint32 flash_status = HAL_FLASH_STATUS_OK;
    abup_int32  marker_addr  = offset; /* bl_custom_fota_size() - BL_FOTA_UPDATE_INFO_RESERVE_SIZE; */

    if ((offset + size) > abup_hal_get_delta_size())
    {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
#endif
        return BL_FOTA_ERROR_LOADING_MARKER;
    }

    marker_addr = marker_addr - abup_bl_get_rom_base();
#ifdef ABUP_FOTA_ENABLE_DIFF_DEBUG
#endif

    page_count = size / NOR_PAGE_SIZE;
    for (page_idx = 0; page_idx < page_count; page_idx++)
    {
        flash_status = abup_bl_flash_read(type, marker_addr, (abup_uint8*)dest, NOR_PAGE_SIZE);
        if (flash_status != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_printf_flashstatus(flash_status);
#endif
            return BL_FOTA_ERROR_LOADING_MARKER;
        }
        marker_addr += NOR_PAGE_SIZE;
        dest += NOR_PAGE_SIZE;
    }

    if (size % NOR_PAGE_SIZE != 0)
    {
        flash_status = abup_bl_flash_read(type, marker_addr, (abup_uint8*)dest, size % NOR_PAGE_SIZE);
        if (flash_status != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_printf_flashstatus(flash_status);
#endif
            return BL_FOTA_ERROR_LOADING_MARKER;
        }
    }

    return BL_FOTA_ERROR_NONE;
}

#ifdef BLOCK_SIZE_64
abup_uint8 abup_get_addr_info(abup_uint32 addr)
{
    abup_int64 addr_info = addr;
    if (addr_info >= RTOS_BASE && addr_info < RTOS_BASE + RTOS_LENGTH)
    {
        return 0; /* app */
    }
    else if (addr_info >= FOTA_RESERVED_BASE && addr_info < FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - ABUP_BLOCK_SIZE)
    {
        return 1; /* delta */
    }
    else if (addr_info >= FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - ABUP_BLOCK_SIZE &&
             addr_info < FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH)
    {
        return 2; /* backup */
    }
    else
    {
        return 3;
    }
}
#endif

abup_int32 abup_bl_erase_block(abup_uint8 type, abup_uint32 addr)
{
    abup_int32 flash_status = HAL_FLASH_STATUS_OK;
    abup_int32 marker_addr  = addr;

#ifdef BLOCK_SIZE_64
    {
        abup_uint8 i         = 0;
        abup_uint8 addr_type = abup_get_addr_info(addr);

        abup_bl_debug_print(LOG_DEBUG, "flash erase addr=%x\r\n", addr);
        for (i = 0; i < 16; i++)
        {
            if (abup_get_addr_info(addr + i * ABUP_TRUE_BLOCK_SIZE) != addr_type)
            {
                break;
            }
            flash_status = abup_erase_sector(marker_addr + (i * ABUP_TRUE_BLOCK_SIZE), ABUP_TRUE_BLOCK_SIZE);
            if (flash_status != HAL_FLASH_STATUS_OK)
            {
                abup_printf_flashstatus(flash_status);
                return BL_FOTA_ERROR_ERASE_MARKER;
            }
        }
    }
#else
    flash_status = abup_bl_flash_erase(type, marker_addr, ABUP_TRUE_BLOCK_SIZE);
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
        abup_printf_flashstatus(flash_status);
#endif
        return BL_FOTA_ERROR_ERASE_MARKER;
    }
#endif
    return BL_FOTA_ERROR_NONE;
}

abup_uint32 abup_bl_erase_delata(void)
{
    abup_uint32 flash_status = HAL_FLASH_STATUS_OK;
#if 1 /* only init first block */
    flash_status = abup_bl_erase_block(ABUP_FILETYPE_PATCH, 0);
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
        abup_printf_flashstatus(flash_status);
        return BL_FOTA_ERROR_ERASE_MARKER;
    }
#else
    abup_int32 erase_len = abup_hal_get_delta_size();
    while (erase_len > 0)
    {
        flash_status = abup_bl_erase_block(marker_addr);
        if (flash_status != HAL_FLASH_STATUS_OK)
        {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
            abup_bl_debug_print(LOG_DEBUG, "abup_erase_sector status = %d\r\n", flash_status);
#endif
            return BL_FOTA_ERROR_ERASE_MARKER;
        }
        erase_len -= abup_bl_flash_block_size();
        marker_addr += abup_bl_flash_block_size();
    }
#endif
    return BL_FOTA_ERROR_NONE;
}

void abup_bl_erase_backup_region(void)
{

    abup_int32 flash_status = HAL_FLASH_STATUS_OK;

    flash_status = abup_bl_erase_block(ABUP_FILETYPE_BACKUP, 0);
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
#ifdef ABUP_FOTA_ENABLE_DIFF_ERROR
        abup_printf_flashstatus(flash_status);
#endif
    }
}

abup_int32 abup_bl_write_backup_region(abup_uint8* data_ptr, abup_uint32 len)
{
    abup_int32 result;

    result = abup_bl_write_block(ABUP_FILETYPE_BACKUP, data_ptr, 0, len);
    return result;
}

abup_int32 abup_bl_read_backup_region(abup_uint8* data_ptr, abup_uint32 len)
{

    abup_int32 result;

    result = abup_bl_read_block(ABUP_FILETYPE_BACKUP, data_ptr, 0, len);
    return result;
}
