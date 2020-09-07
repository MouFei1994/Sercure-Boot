/**
  ******************************************************************************
  * @file    sfu_low_level_flash_int.c
  * @author  MCD Application Team
  * @brief   SFU Flash Low Level Interface module
  *          This file provides set of firmware functions to manage SFU internal
  *          flash low level interface.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "se_interface_bootloader.h"
#include "string.h"


/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup  SFU_LOW_LEVEL
  * @{
  */

/** @defgroup SFU_LOW_LEVEL_FLASH Flash Low Level Interface
  * @{
  */

/** @defgroup SFU_FLASH_Private_Definition Private Definitions
  * @{
  */
#define NB_PAGE_SECTOR_PER_ERASE  2U    /*!< Nb page erased per erase */

/**
  * @}
  */
/** @defgroup SFU_FLASH_Private_Variables Private Variables
  * @{
  */
static __IO uint32_t DoubleECC_Error_Counter = 0U;


/**
  * @}
  */

/** @defgroup SFU_FLASH_Private_Functions Private Functions
  * @{
  */
static uint32_t SFU_LL_FLASH_INT_GetBankAddr(uint32_t bank);
static SFU_ErrorStatus SFU_LL_FLASH_INT_Clear_Error(void);

/**
  * @}
  */

/** @addtogroup SFU_FLASH_Exported_Functions
  * @{
  */

/**
  * @brief  This function initialize the internal flash interface if required
  * @param  none
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Init(void)
{
  return SFU_SUCCESS;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in user flash area
  * @param  pFlashStatus: SFU_FLASH Status pointer
  * @param  pStart: flash address to be erased
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Erase_Size(SFU_FLASH_StatusTypeDef *pFlashStatus, void *pStart, uint32_t Length)
{
  uint32_t page_error = 0U;
  uint32_t start = (uint32_t)pStart;
  FLASH_EraseInitTypeDef p_erase_init;
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint32_t first_page = 0U;
  uint32_t nb_pages = 0U;
  uint32_t chunk_nb_pages;
  uint32_t erase_command = 0U;
  uint32_t bank_number = 0U;

  /* Check the pointers allocation */
  if (pFlashStatus == NULL)
  {
    return SFU_ERROR;
  }

  *pFlashStatus = SFU_FLASH_SUCCESS;

  /* Clear error flags raised during previous operation */
  e_ret_status = SFU_LL_FLASH_INT_Clear_Error();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      /* Get the 1st page to erase */
      do
      {
        first_page = SFU_LL_FLASH_INT_GetPage(start);
        bank_number = SFU_LL_FLASH_INT_GetBank(start);
        if (SFU_LL_FLASH_INT_GetBank(start + Length - 1U) == bank_number)
        {
          /* Get the number of pages to erase from 1st page */
          nb_pages = SFU_LL_FLASH_INT_GetPage(start + Length - 1U) - first_page + 1U;
          /* Get the bank */

          /* Fill EraseInit structure*/
          p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
          p_erase_init.Banks       = bank_number;

          /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
          do
          {
            chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
            p_erase_init.Page = first_page;
            p_erase_init.NbPages = chunk_nb_pages;
            first_page += chunk_nb_pages;
            nb_pages -= chunk_nb_pages;
            if (HAL_FLASHEx_Erase(&p_erase_init, &page_error) != HAL_OK)
            {
              e_ret_status = SFU_ERROR;
              *pFlashStatus = SFU_FLASH_ERR_ERASE;
            }
            SFU_LL_SECU_IWDG_Refresh(); /* calling this function which checks the compiler switch */
          } while (nb_pages > 0);
          erase_command = 1U;
        }
        else
        {
          uint32_t startbank2 = SFU_LL_FLASH_INT_GetBankAddr(FLASH_BANK_2);
          nb_pages = SFU_LL_FLASH_INT_GetPage(startbank2 - 1U) - first_page + 1U;
          p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
          p_erase_init.Banks       = bank_number;
          Length = Length  - (startbank2 - start);
          start = startbank2;

          /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
          do
          {
            chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
            p_erase_init.Page = first_page;
            p_erase_init.NbPages = chunk_nb_pages;
            first_page += chunk_nb_pages;
            nb_pages -= chunk_nb_pages;
            if (HAL_FLASHEx_Erase(&p_erase_init, &page_error) != HAL_OK)
            {
              e_ret_status = SFU_ERROR;
              *pFlashStatus = SFU_FLASH_ERR_ERASE;
            }
            SFU_LL_SECU_IWDG_Refresh(); /* calling this function which checks the compiler switch */
          } while (nb_pages > 0);
        }
      } while (erase_command == 0);
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();

    }
    else
    {
      *pFlashStatus = SFU_FLASH_ERR_HAL;
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pFlashStatus: FLASH_StatusTypeDef
  * @param  pDestination: Start address for target location. It has to be 8 bytes aligned.
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: Length of data buffer in bytes. It has to be 8 bytes aligned.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Write(SFU_FLASH_StatusTypeDef *pFlashStatus, void  *pDestination, const void *pSource,
                                       uint32_t Length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t i = 0U;
  uint32_t pdata = (uint32_t)pSource;

  /* Check the pointers allocation */
  if ((pFlashStatus == NULL) || (pSource == NULL))
  {
    return SFU_ERROR;
  }
  /* Test if access is in this range : SLOT 0 header */
  if ((Length != 0) && ((uint32_t)pDestination >= SFU_IMG_SLOT_0_REGION_HEADER_VALUE) &&
      ((((uint32_t)pDestination + Length - 1)) < (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET))
     )
  {
    /* SE Access */
    SE_StatusTypeDef se_status;
    SE_ErrorStatus se_ret_status = SE_SFU_IMG_Write(&se_status, pDestination, pSource, Length);
    if (se_ret_status == SE_SUCCESS)
    {
      e_ret_status = SFU_SUCCESS;
      *pFlashStatus = SFU_FLASH_SUCCESS;
    }
    else
    {
      e_ret_status = SFU_ERROR;
      *pFlashStatus = SFU_FLASH_ERROR;
    }
  }
  else
  {
    *pFlashStatus = SFU_FLASH_ERROR;

    /* Clear error flags raised during previous operation */
    e_ret_status = SFU_LL_FLASH_INT_Clear_Error();

    if (e_ret_status == SFU_SUCCESS)
    {
      /* Unlock the Flash to enable the flash control register access *************/
      if (HAL_FLASH_Unlock() != HAL_OK)
      {
        *pFlashStatus = SFU_FLASH_ERR_HAL;

      }
      else
      {
        /* DataLength must be a multiple of 64 bit */
        for (i = 0U; i < Length;  i += sizeof(SFU_LL_FLASH_write_t))
        {
          *pFlashStatus = SFU_FLASH_ERROR;

          /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
          be done by word */
          if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)pDestination, *((uint64_t *)(pdata + i)))
              == HAL_OK)
          {
            /* Check the written value */
            if (*(uint64_t *)pDestination != *(uint64_t *)(pdata + i))
            {
              /* Flash content doesn't match SRAM content */
              *pFlashStatus = SFU_FLASH_ERR_WRITINGCTRL;
              e_ret_status = SFU_ERROR;
              break;
            }
            else
            {
              /* Increment FLASH Destination address */
              pDestination = (uint8_t *)pDestination + sizeof(SFU_LL_FLASH_write_t);
              e_ret_status = SFU_SUCCESS;
              *pFlashStatus = SFU_FLASH_SUCCESS;
            }
          }
          else
          {
            /* Error occurred while writing data in Flash memory */
            *pFlashStatus = SFU_FLASH_ERR_WRITING;
            e_ret_status = SFU_ERROR;
            break;
          }
        }
        /* Lock the Flash to disable the flash control register access (recommended
        to protect the FLASH memory against possible unwanted operation) */
        HAL_FLASH_Lock();
      }
    }
  }
  return e_ret_status;
}

/**
  * @brief  This function reads flash
  * @param  pDestination: Start address for target location
  * @param  pSource: flash address to read
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_INT_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Test if access is in this range : SLOT 0 header */
  if (((uint32_t)pSource >= SFU_IMG_SLOT_0_REGION_HEADER_VALUE) &&
      ((((uint32_t)pSource + Length - 1)) < (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET))
     )
  {
    /* SE Access */
    SE_StatusTypeDef se_status;
    DoubleECC_Error_Counter = 0U;
    SE_ErrorStatus se_ret_status = SE_SFU_IMG_Read(&se_status, pDestination, pSource, Length);
    if ((se_ret_status == SE_SUCCESS) && (DoubleECC_Error_Counter == 0))
    {
      e_ret_status = SFU_SUCCESS;
    }
    DoubleECC_Error_Counter = 0U;
  }
  else
  {
    DoubleECC_Error_Counter = 0U;
    memcpy(pDestination, pSource, Length);
    if (DoubleECC_Error_Counter == 0U)
    {
      e_ret_status = SFU_SUCCESS;
    }
    DoubleECC_Error_Counter = 0U;
  }
  return e_ret_status;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: flash address
  * @retval The page of a given address
  */
uint32_t SFU_LL_FLASH_INT_GetPage(uint32_t Addr)
{
  uint32_t page = 0U;

  if (Addr < (FLASH_BASE + (FLASH_BANK_SIZE)))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + (FLASH_BANK_SIZE))) / FLASH_PAGE_SIZE;
  }

  return page;
}


/**
  * @brief  Gets the bank of a given address
  * @param  Addr: flash address
  * @retval The bank of a given address
  */
uint32_t SFU_LL_FLASH_INT_GetBank(uint32_t Addr)
{
  uint32_t bank = 0U;
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0U)
  {
    /* No Bank swap */
    if (Addr < (FLASH_BASE + (FLASH_BANK_SIZE)))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
    /* Bank swap */
    if (Addr < (FLASH_BASE + (FLASH_BANK_SIZE)))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }

  return bank;
}


/**
  * @}
  */

/** @defgroup SFU_FLASH_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Clear error flags raised during previous operation
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus SFU_LL_FLASH_INT_Clear_Error(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {

    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      e_ret_status = SFU_SUCCESS;
    }
  }

  return e_ret_status;

}

/**
  * @brief  Gets the address of a bank
  * @param  Bank: Bank ID
  * @retval Address of the bank
  */
static uint32_t SFU_LL_FLASH_INT_GetBankAddr(uint32_t Bank)
{
  if (Bank == FLASH_BANK_2)
  {
    return  FLASH_BASE + (FLASH_BANK_SIZE);
  }
  else
  {
    return FLASH_BASE;
  }
}

/**
  * @brief  Get Link Register value (LR)
  * @param  None.
  * @retval LR Register value
  */
__attribute__((always_inline)) __STATIC_INLINE uint32_t __get_LR(void)
{
  register uint32_t result;

#if defined ( __ICCARM__ ) || ( __GNUC__ )
  __ASM volatile("MOV %0, LR" : "=r"(result));
#elif defined ( __CC_ARM )
  __ASM volatile("MOV result, __return_address()");
#endif

  return result;
}

/*
   As this handler code relies on stack pointer position to manipulate the PC return value, it is important
   not to use extra registers (on top of scratch registers), because it would change the stack pointer
   position. Then compiler optimizations are customized to ensure that.
*/
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#elif defined ( __GNUC__ )
__attribute__((optimize("O1")))
#endif /* __ICCARM__ */
/**
  * @brief  NMI Handler present for handling Double ECC NMI interrupt
  * @param  None.
  * @retvat void
  */
void NMI_Handler(void)
{
  if (ECC_Fault_Handler() != SFU_SUCCESS)
  {
    while (1U);
  }
}

/**
  * @brief  Bus Fault Handler present for handling Double ECC Bus Fault interrupt
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if ECC fault managed, SFU_ERROR otherwise.
  */
SFU_ErrorStatus ECC_Fault_Handler(void)
{
  uint32_t *p_sp;
  uint32_t lr;
  uint16_t opcode_msb;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_ECCD))
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ECCD);

    /* Memorize error to ignore the read value */
    DoubleECC_Error_Counter++;

    lr = __get_LR();

    /* Check EXC_RETURN value in LR to know which SP was used prior entering exception */
    if (((lr) & (0xFU)) == 0xDU)
    {
      /* interrupted code was using Process Stack Pointer */
      p_sp = (uint32_t *)__get_PSP();
    }
    else
    {
      /* interrupted code was using Main Stack Pointer */
      p_sp = (uint32_t *)__get_MSP();
    }

    /* Test caller mode T bit from CPSR in stack */
    if ((*(p_sp + 7U) & (1U << xPSR_T_Pos)) != 0U)
    {
      /* Thumb  mode.
         Test PC in stack.
         If bits [15:11] of the halfword being decoded take any of the following values,
         the halfword is the first halfword of a 32-bit instruction: 0b11101, 0b11110, 0b11111.
         Otherwise, the halfword is a 16-bit instruction.
      */
      opcode_msb = (*(uint16_t *)(*(p_sp + 6) & 0xFFFFFFFEU) & 0xF800U);
      if ((opcode_msb == 0xE800U) || (opcode_msb == 0xF000U) || (opcode_msb == 0xF800U))
      {
        /* execute next instruction PC +4  */
        *(p_sp + 6U) += 4U;
      }
      else
      {
        /* execute next instruction PC +2  */
        *(p_sp + 6U) += 2U;
      }
    }
    else
    {
      /* ARM mode execute next instruction PC +4 */
      *(p_sp + 6U) += 4U;
    }
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;
}

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
