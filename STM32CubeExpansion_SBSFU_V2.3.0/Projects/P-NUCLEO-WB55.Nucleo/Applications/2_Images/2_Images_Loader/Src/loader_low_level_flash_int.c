/**
  ******************************************************************************
  * @file    loader_low_level_flash_int.c
  * @author  MCD Application Team
  * @brief   Flash Low Level Interface module
  *          This file provides set of firmware functions to manage internal flash
  *          low level interface.
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
#include "loader_low_level_flash.h"
#include <string.h>


/* Private defines -----------------------------------------------------------*/
#define NB_PAGE_SECTOR_PER_ERASE  2U    /*!< Nb page erased per erase */


/* Private variables ---------------------------------------------------------*/
static __IO uint32_t DoubleECC_Error_Counter = 0U;



/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef LOADER_LL_FLASH_INT_Clear_Error(void);


/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  This function initialize the internal flash interface if required
  * @param  none
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Init(void)
{

  return HAL_OK;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in user flash area
  * @param  pStart: pointer to  user flash area
  * @param  Length: number of bytes.
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Erase_Size(void *pStart, uint32_t Length)
{
  uint32_t page_error = 0U;
  uint32_t start = (uint32_t)pStart;
  FLASH_EraseInitTypeDef p_erase_init;
  HAL_StatusTypeDef e_ret_status = HAL_OK;
  uint32_t first_page = 0U;
  uint32_t nb_pages = 0U;
  uint32_t chunk_nb_pages;

  /* Initialize Flash - erase application code area*/
  e_ret_status = LOADER_LL_FLASH_INT_Clear_Error();

  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    e_ret_status = HAL_FLASH_Unlock();
    if (e_ret_status == HAL_OK)
    {
      first_page = LOADER_LL_FLASH_INT_GetPage(start);
      /* Get the number of pages to erase from 1st page */
      nb_pages = LOADER_LL_FLASH_INT_GetPage(start + Length - 1U) - first_page + 1U;
      p_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
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
          e_ret_status = HAL_ERROR;
        }

        /*
         * If the SecureBoot configured the IWDG, loader must reload IWDG counter with value defined in the reload
         * register
         */
        WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
      } while (nb_pages > 0);
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();

    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pDestination: Start address for target location. It has to be 8 bytes aligned.
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: Length of data buffer in bytes. It has to be 8 bytes aligned.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Write(void  *pDestination, const void *pSource, uint32_t Length)
{
  HAL_StatusTypeDef e_ret_status;
  uint32_t i = 0U;
  uint32_t pdata = (uint32_t)pSource;

  /* Initialize Flash - erase application code area*/
  e_ret_status = LOADER_LL_FLASH_INT_Clear_Error();

  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    e_ret_status = HAL_FLASH_Unlock();
    if (e_ret_status == HAL_OK)
    {
      /* DataLength must be a multiple of 64 bit */
      for (i = 0U; i < Length;  i += sizeof(LOADER_LL_FLASH_write_t))
      {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)pDestination, *((uint64_t *)(pdata + i)))
            == HAL_OK)
        {
          /* Check the written value */
          if (*(uint64_t *)pDestination != *(uint64_t *)(pdata + i))
          {
            /* Flash content doesn't match SRAM content */
            e_ret_status = HAL_ERROR;
            break;
          }
          else
          {
            /* Increment FLASH Destination address */
            pDestination = (uint8_t *)pDestination + sizeof(LOADER_LL_FLASH_write_t);
          }
        }
        else
        {
          /* Error occurred while writing data in Flash memory */
          e_ret_status = HAL_ERROR;
          break;
        }
      }
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) */
      HAL_FLASH_Lock();
    }
  }
  return e_ret_status;
}

/**
  * @brief  This function reads flash
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: Length in bytes of data buffer
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LOADER_LL_FLASH_INT_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;

  DoubleECC_Error_Counter = 0U;
  memcpy(pDestination, pSource, Length);
  if (DoubleECC_Error_Counter == 0U)
  {
    e_ret_status = HAL_OK;
  }
  DoubleECC_Error_Counter = 0U;
  return e_ret_status;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t LOADER_LL_FLASH_INT_GetPage(uint32_t Addr)
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




/* Private Functions Definition -------------------------------------------------*/

/**
  * @brief  Clear error flags raised during previous operation
  * @param  None.
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef LOADER_LL_FLASH_INT_Clear_Error(void)
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;

  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {

    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      e_ret_status = HAL_OK;
    }
  }

  return e_ret_status;

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
#endif /*  ( __ICCARM__ ) || ( __GNUC__ ) */

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
  uint32_t *p_sp;
  uint32_t lr;
  uint16_t opcode_msb;

  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_ECCD))
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ECCD);

    /*  memorize error to ignore the read value */
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
  }
  else
  {
    /* This exception occurs for another reason than flash double ECC errors */
    while (1);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
