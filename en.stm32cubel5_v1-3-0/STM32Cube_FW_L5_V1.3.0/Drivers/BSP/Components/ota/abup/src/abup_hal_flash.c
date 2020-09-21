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
 * @file        abup_hal_flash.c
 *
 * @brief       Implement flash operator
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef ABUP_HAL_FLASH_C
#define ABUP_HAL_FLASH_C
#include "abup_hal_flash.h"
#include "abup_stdlib.h"

#if !defined(ABUP_ONEOS)
static uint32_t AbupGetPage(uint32_t Addr)
{
    abup_uint32 page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

static abup_uint32 AbupGetBank(uint32_t Addr)
{
    abup_uint32 bank = 0;
#if defined(STM32L432xx)
    bank = FLASH_BANK_1;
#else
#if defined(SYSCFG_MEMRMP_FB_MODE)
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
#endif /* SYSCFG_MEMRMP_FB_MODE */
    {
        /* Bank swap */
#if defined(FLASH_BANK_2)
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_2;
        }
        else
#endif /* FLASH_BANK_2 */
        {
            bank = FLASH_BANK_1;
        }
    }
#endif
    return bank;
}

abup_bool abup_flash_addr_valid(abup_uint32 addr, abup_uint size)
{
    abup_uint32 end = addr;
    if (size == 0)
    {
        end += ABUP_DEFAULT_SECTOR_SIZE;
    }
    else
    {
        end += size;
    }
    if ((ABUP_FLASH_BASE_END < addr)
#if (ABUP_FLASH_BASE_ADDR != 0)
        || (addr < ABUP_FLASH_BASE_ADDR)
#endif
        || (ABUP_FLASH_BASE_END < end))
    {
        return abup_false;
    }

    return abup_true;
}

abup_int abup_flash_read(abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_uint   i;
    abup_uint32 address = addr;

    if (!buf || size == 0)
    {
        return -1;
    }

    if (!abup_flash_addr_valid(addr, size))
    {
        return -1;
    }
    for (i = 0; i < size; i++, buf++, address++)
    {
        *buf = *(abup_uint8*)address;
    }

    return size;
}

#if defined(__GNUC__)
#elif defined(__ICCARM__) || defined(__ICCRX__)
#pragma location = "RAMCODE"
#elif defined(__CC_ARM)
#pragma arm section code = "RAMCODE"
#endif

abup_int abup_flash_write(abup_uint32 addr, abup_uint8* buf, abup_uint size)
{
    abup_uint   i, j;
    abup_uint32 address    = addr;
    abup_int8   result     = 0;
    abup_uint64 write_data = 0, temp_data = 0;

    if (!buf || size == 0)
    {
        return -1;
    }

    if (!abup_flash_addr_valid(addr, size))
    {
        return -1;
    }
    if (address % 8 != 0)
    {
        return -10;
    }

    HAL_FLASH_Unlock();
    /* Clear all error flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    if (size < 1)
    {
        return -1;
    }

    for (i = 0; i < size;)
    {
        if ((size - i) < 8)
        {
            for (j = 0; (size - i) > 0; i++, j++)
            {
                temp_data  = *buf;
                write_data = (write_data) | (temp_data << 8 * j);
                buf++;
            }
        }
        else
        {
            for (j = 0; j < 8; j++, i++)
            {
                temp_data  = *buf;
                write_data = (write_data) | (temp_data << 8 * j);
                buf++;
            }
        }

        /* write data */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, write_data) == HAL_OK)
        {
            /* Check the written value */
            if (*(abup_uint64*)address != write_data)
            {
                result = -1;
                goto __exit;
            }
        }
        else
        {
            result = -1;
            goto __exit;
        }

        temp_data  = 0;
        write_data = 0;

        address += 8;
    }

__exit:
    HAL_FLASH_Lock();
    if (result != 0)
    {
        return result;
    }

    return size;
}

abup_int abup_erase_sector(abup_uint32 addr, abup_uint size)
{
    abup_int8   result    = 0;
    abup_uint32 FirstPage = 0, NbOfPages = 0, BankNumber = 0;
    abup_uint32 address   = addr;
    abup_uint   PAGEError = 0;
    if (!abup_flash_addr_valid(addr, size))
    {
        return -1;
    }
    if (size == 0)
    {
        size = ABUP_DEFAULT_SECTOR_SIZE;
    }

    /* Variable used for Erase procedure */
    FLASH_EraseInitTypeDef EraseInitStruct;
    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Clear all error flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    /* Get the 1st page to erase */
    FirstPage = AbupGetPage(address);
    /* Get the number of pages to erase from 1st page */
    NbOfPages = AbupGetPage(address + size - 1) - FirstPage + 1;
    /* Get the bank */
    BankNumber = AbupGetBank(address);
    /* Fill EraseInit structure */
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks     = BankNumber;
    EraseInitStruct.Page      = FirstPage;
    EraseInitStruct.NbPages   = NbOfPages;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, (uint32_t*)&PAGEError) != HAL_OK)
    {
        result = -1;
        goto __exit;
    }

__exit:
    HAL_FLASH_Lock();

    if (result != 0)
    {
        return result;
    }
    return size;
}

#if defined(__GNUC__)
#elif defined(__ICCARM__) || defined(__ICCRX__)
#elif defined(__CC_ARM)
#pragma arm section
#endif
#endif
#endif
