/**
  ******************************************************************************
  * @file    se_low_level.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE low level
  *          interface functionalities.
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
#include "se_low_level.h"
#if defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __CC_ARM */
#include "se_exception.h"
#include "string.h"
#include "se_key.h"               /* required to access the keys when not provided as input parameter (metadata 
                                     authentication) */
#include "se_crypto_common.h"

/** @addtogroup SE Secure Engine
  * @{
  */
/** @defgroup  SE_HARDWARE SE Hardware Interface
  * @{
  */

/** @defgroup SE_HARDWARE_Private_Variables Private Variables
  * @{
  */
static CRC_HandleTypeDef    CrcHandle;                  /*!< SE Crc Handle*/

static __IO uint32_t SE_DoubleECC_Error_Counter = 0U;

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Private_Functions Private Functions
  * @{
  */
static uint32_t SE_LL_GetBank(uint32_t Address);

static uint32_t SE_LL_GetSector(uint32_t Address);
/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_Variables Exported Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup SE_HARDWARE_Exported_CRC_Functions CRC Exported Functions
  * @{
  */

/**
  * @brief  Set CRC configuration and call HAL CRC initialization function.
  * @param  None.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise
  */
SE_ErrorStatus SE_LL_CRC_Config(void)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  CrcHandle.Instance = CRC;
  /* The input data are not inverted */
  CrcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;

  /* The output data are not inverted */
  CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

  /* The Default polynomial is used */
  CrcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  /* The default init value is used */
  CrcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  /* The input data are 32-bit long words */
  CrcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
  /* CRC Init*/
  if (HAL_CRC_Init(&CrcHandle) == HAL_OK)
  {
    e_ret_status = SE_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  Wrapper to HAL CRC initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_Init(void)
{
  /* CRC Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();

  return SE_LL_CRC_Config();
}

/**
  * @brief  Wrapper to HAL CRC de-initilization function.
  * @param  None
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_CRC_DeInit(void)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;

  if (HAL_CRC_DeInit(&CrcHandle) == HAL_OK)
  {
    /* Initialization OK */
    e_ret_status = SE_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  Wrapper to HAL CRC Calculate function.
  * @param  pBuffer: pointer to data buffer.
  * @param  uBufferLength: buffer length in 32-bits word.
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
uint32_t SE_LL_CRC_Calculate(uint32_t pBuffer[], uint32_t uBufferLength)
{
  return HAL_CRC_Calculate(&CrcHandle, pBuffer, uBufferLength);
}

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_FLASH_Functions FLASH Exported Functions
  * @{
  */

/**
  * @brief  This function does an erase of nb sectors in user flash area
  * @param  pStart: pointer to  user flash area
  * @param  Length: number of bytes.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Erase(void *pStart, uint32_t Length)
{
  uint32_t sector_error = 0U;
  FLASH_EraseInitTypeDef p_erase_init;
  SE_ErrorStatus e_ret_status = SE_SUCCESS;

  /* Unlock the Flash to enable the flash control register access *************/
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    /* Fill EraseInit structure*/
    p_erase_init.TypeErase     = FLASH_TYPEERASE_SECTORS;
    /* Do nothing if FLASH_CR_PSIZE not define */
    p_erase_init.Banks         = SE_LL_GetBank((uint32_t) pStart);
    p_erase_init.Sector        = SE_LL_GetSector((uint32_t) pStart);
    p_erase_init.NbSectors     = SE_LL_GetSector(((uint32_t) pStart) + Length - 1U) - p_erase_init.Sector + 1U;
    if (HAL_FLASHEx_Erase(&p_erase_init, &sector_error) != HAL_OK)
    {
      e_ret_status = SE_ERROR;
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    (void)HAL_FLASH_Lock();
  }
  else
  {
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Write in Flash  protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  pSource pointer to input buffer
  * @param  Length number of bytes to be written
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_LL_FLASH_Write(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret = SE_SUCCESS;
  uint32_t i;
  uint32_t pdata = (uint32_t)pSource;
  uint32_t areabegin = (uint32_t)pDestination;

  /* Test if access is in this range : SLOT 0 header */
  if (Length == 0U)
  {
    return SE_ERROR;
  }

  if ((areabegin < SFU_IMG_SLOT_0_REGION_HEADER_VALUE) ||
      ((areabegin + Length) > (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    return SE_ERROR;
  }

  /* Unlock the Flash to enable the flash control register access *************/
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    for (i = 0U; i < Length; i += 16U)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (areabegin + i), (uint64_t)(pdata + i)) != HAL_OK)
      {
        ret = SE_ERROR;
        break;
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) */
    (void)HAL_FLASH_Lock();
  }
  else
  {
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Read in Flash protected area
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to read
  * @param  Length: Length in bytes of data buffer
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  uint32_t areabegin = (uint32_t)pSource;
  SE_ErrorStatus e_ret_status = SE_ERROR;

  /* Test if access is in this range : SLOT 0 header or SWAP area */
  if (Length == 0U)
  {
    return e_ret_status;
  }

  if ((areabegin < SFU_IMG_SLOT_0_REGION_HEADER_VALUE) ||
      ((areabegin + Length) > (SFU_IMG_SLOT_0_REGION_HEADER_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    return e_ret_status;
  }

  SE_DoubleECC_Error_Counter = 0U;
  (void)memcpy(pDestination, pSource, Length);
  if (SE_DoubleECC_Error_Counter == 0U)
  {
    e_ret_status = SE_SUCCESS;
  }
  SE_DoubleECC_Error_Counter = 0U;
  return e_ret_status;
}

/**
  * @}
  */

/** @defgroup SE_HARDWARE_Exported_FLASH_EXT_Functions External FLASH Exported Functions
  * @{
  */

/**
  * @brief Initilisation of external flash On The Fly DECryption (OTFDEC)
  * @param pxSE_Metadata: Firmware metadata.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_FLASH_EXT_Decrypt_Init(SE_FwRawHeaderTypeDef *pxSE_Metadata)
{
  SE_ErrorStatus e_ret_status = SE_SUCCESS;
  OTFDEC_HandleTypeDef OtfdecHandle;
  OTFDEC_RegionConfigTypeDef OTFDEC_Config = {0};
  uint8_t key[16];
  uint8_t key_swapped[16];
  uint32_t i;


  /* Enable OTFDEC1 clock */
  __HAL_RCC_OTFDEC1_CLK_ENABLE();

  /* Init Region1 Of OTFDEC1 */
  OtfdecHandle.Instance = OTFDEC1;
  if ((e_ret_status == SE_SUCCESS) && (HAL_OTFDEC_Init(&OtfdecHandle) != HAL_OK))
  {
    e_ret_status = SE_ERROR;
  }

  /* Set OTFDEC Mode */
  if ((e_ret_status == SE_SUCCESS)
      && (HAL_OTFDEC_RegionSetMode(&OtfdecHandle, OTFDEC_REGION1, OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES)
          != HAL_OK))
  {
    e_ret_status = SE_ERROR;
  }

  /* Read the Symmetric Key */
  SE_ReadKey(key);

  /* Swap the key :
     - Reakey function : Key[0:127] : byte0[8:0] || byte1[8:0]  ...  || byte16[8:0]
     - OTFDEC : AES_KEY[127:0]= RxKEYR3[31:0] || RxKEYR2[31:0] || RxKEYR1[31:0] || RxKEYR0[31:0]
   */
  for (i = 0; i < 16; i++)
  {
    key_swapped[i] = key[15 - i];
  }

  /* Set OTFDEC Key */
  if ((e_ret_status == SE_SUCCESS)
      && (HAL_OTFDEC_RegionSetKey(&OtfdecHandle, OTFDEC_REGION1, (uint32_t *)key_swapped) != HAL_OK))
  {
    e_ret_status = SE_ERROR;
  }

  /* Activate OTFDEC decryption :
     ===========================
     The initialization vector AES_IV[127:0]= RxNONCER[63:0] || 0b0000 0000 0000 0000 || RxCFGR[31:16] || 0b00
                                              || (Region ID) || ReadAddress[31:4] (address modulo 128-bit)

     In this example, we assume :
     - Region ID = 0 (Region1)
     - Version = 0
  */
  OTFDEC_Config.Nonce[0]     = pxSE_Metadata->Nonce[4] << 24 | pxSE_Metadata->Nonce[5] << 16 |
                               pxSE_Metadata->Nonce[6] << 8  | pxSE_Metadata->Nonce[7];
  OTFDEC_Config.Nonce[1]     = pxSE_Metadata->Nonce[0] << 24 | pxSE_Metadata->Nonce[1] << 16 |
                               pxSE_Metadata->Nonce[2] << 8  | pxSE_Metadata->Nonce[3];
  OTFDEC_Config.StartAddress = SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET;
  OTFDEC_Config.EndAddress   = SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_SLOT_0_REGION_SIZE;
  OTFDEC_Config.Version      = 0x0;
  if ((e_ret_status == SE_SUCCESS)
      && (HAL_OTFDEC_RegionConfig(&OtfdecHandle, OTFDEC_REGION1,  &OTFDEC_Config, OTFDEC_REG_CONFIGR_LOCK_ENABLE)
          != HAL_OK))
  {
    e_ret_status = SE_ERROR;
  }

  return e_ret_status;
}

/**
  * @}
  */

/**
  * @brief Check if an array is inside the RAM of the product
  * @param Addr : address  of array
  * @param Length : legnth of array in byte
  */
SE_ErrorStatus SE_LL_Buffer_in_ram(void *pBuff, uint32_t Length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + Length - 1U;
  if ((Length != 0U) && ((addr_end  <= SB_REGION_RAM_END) && (addr_start >= (SE_REGION_RAM_END + 1))))
  {
    e_ret_status = SE_SUCCESS;
  }
  else
  {
    e_ret_status = SE_ERROR;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  return e_ret_status;
}

/**
  * @brief function checking if a buffer is in sbsfu ram.
  * @param pBuff: Secure Engine protected function ID.
  * @param length: length of buffer in bytes
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_Buffer_in_SBSFU_ram(const void *pBuff, uint32_t length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + length - 1U;
  if ((length != 0U) && ((addr_end  <= SB_REGION_RAM_END) && (addr_start >= SB_REGION_RAM_START)))
  {
    e_ret_status = SE_SUCCESS;
  }
  else
  {
    e_ret_status = SE_ERROR;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  return e_ret_status;
}

/**
  * @brief function checking if a buffer is PARTLY in se ram.
  * @param pBuff: Secure Engine protected function ID.
  * @param length: length of buffer in bytes
  * @retval SE_ErrorStatus SE_SUCCESS for buffer in se ram, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_LL_Buffer_part_of_SE_ram(const void *pBuff, uint32_t length)
{
  SE_ErrorStatus e_ret_status;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + length - 1U;
  if ((length != 0U) && (!(((addr_start < SE_REGION_RAM_START) && (addr_end < SE_REGION_RAM_START)) ||
                           ((addr_start > SE_REGION_RAM_END) && (addr_end > SE_REGION_RAM_END)))))
  {
    e_ret_status = SE_SUCCESS;

    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  else
  {
    e_ret_status = SE_ERROR;
  }
  return e_ret_status;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SE_HARDWARE_Private_Functions
  * @{
  */

/**
  * @brief  Gets the sector of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The sector of a given address
  */
static uint32_t SE_LL_GetSector(uint32_t Address)
{
  uint32_t sector = 0U;

  if (Address < (FLASH_BASE + (FLASH_BANK_SIZE)))
  {
    /* Bank 1 */
    sector = (Address - FLASH_BASE) / FLASH_SECTOR_SIZE;
  }
  else
  {
    /* Bank 2 */
    sector = (Address - (FLASH_BASE + (FLASH_BANK_SIZE))) / FLASH_SECTOR_SIZE;
  }

  return sector;
}

static uint32_t SE_LL_GetBank(uint32_t Address)
{
  uint32_t bank;

  if (READ_BIT(FLASH->OPTCCR, FLASH_OPTCR_SWAP_BANK) == 0U)
  {
    /* No Bank swap */
    if (Address < (FLASH_BASE + (FLASH_BANK_SIZE)))
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
    if (Address < (FLASH_BASE + (FLASH_BANK_SIZE)))
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
  * @brief  NMI Handler redirection to SE NMI handler
  * @param  None.
  * @retval None.
  */
void NMI_Handler(void)
{
  SE_NMI_ExceptionHandler();
}

/**
  * @brief  Bus Fault Handler present for handling Double ECC Bus Fault interrupt
  * @param  None.
  * @retvat void
  */
void BusFault_Handler(void)
{
  uint32_t *p_sp;
  uint32_t lr;
  uint16_t opcode_msb;

  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_DBECCE_BANK1RR) || __HAL_FLASH_GET_FLAG(FLASH_FLAG_DBECCE_BANK2RR))
  {
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_DBECCE_BANK1RR);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_DBECCE_BANK2RR);

    /* Memorize error to ignore the read value */
    SE_DoubleECC_Error_Counter++;

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
    SE_NMI_ExceptionHandler();
  }
}

/**
  * @brief  Cleanup SE CORE
  * The fonction is called  during SE_LOCK_RESTRICT_SERVICES.
  *
  */
void  SE_LL_CORE_Cleanup(void)
{
#if defined(__ICCARM__)
#pragma section = ".bss"
#pragma section = ".data"
  uint8_t *begin = __section_begin(".bss");
  uint8_t *end = __section_end(".bss");
  (void)memset(begin, 0x0, (size_t)(end - begin));
  begin = __section_begin(".data");
  end = __section_end(".data");
  (void)memset(begin, 0x0, (size_t)(end - begin));
#elif defined (__CC_ARM)
  __arm_clean_data();
#elif defined(__GNUC__)
  __gcc_clean_data();
#endif /* __ICCARM__ */
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
