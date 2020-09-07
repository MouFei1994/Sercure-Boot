/**
  ******************************************************************************
  * @file    sfu_low_level_security_rss.c
  * @author  MCD Application Team
  * @brief   SFU Security RSS Low Level Interface module
  *          This file provides set of firmware functions to manage SFU security
  *          RSS low level interface.
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
#include "sfu_low_level_security_rss.h"
#include "sfu_trace.h"
#include "sfu_boot.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */


#ifdef  SFU_SECURE_USER_PROTECT_ENABLE


/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup  SFU_LOW_LEVEL
  * @{
  */

/** @defgroup SFU_SECURITY_RSS_Constants RSS Constants
  * @{
  */
#define SFU_LL_RSS_BANK1_SEC_USER_MEM_START ((uint32_t) BANK1_SECURE_START) /*!< Bank1 Secure User memory start @ */
#define SFU_LL_RSS_BANK1_SEC_USER_MEM_END   ((uint32_t) BANK1_SECURE_END)   /*!< Bank1 Secure User memory end @   */
/**
  * The size of a secure user memory area can be set from 512 bytes to full bank with a granularity of 256 bytes.
  * So, the size of a secure user memory area is always a multiple of 256 bytes.
  */
#define SFU_LL_RSS_BANK1_SEC_USER_MEM_SIZE (BANK1_SECURE_END - BANK1_SECURE_START + 1)

/**
  * The start and end addresses are defined as offsets from the Bank base address (0x0800 0000 for Bank 1, 0x0810 0000
  * for Bank 2).
  * The offsets are 12-bit values.
  * So the last 8 bits of the address are not stored as anyway the secure user memory area granularity is 256 bytes.
  * The end address is: FLASH_BANK1_BASE + offset SEC_AREA_END1<<8 but the protected area size is a multiple of 256
  * bytes so + 0x000000FF to be considered.
  */

/*!< Bank1 Secure User memory start @ read from Option Bytes (12-bit offset) */
#define SFU_LL_RSS_BANK1_SEC_USER_MEM_START_FROM_OB ((uint32_t)((SFU_LL_RSS_BANK1_SEC_USER_MEM_START>>8)<<8))
/*!< Bank1 Secure User memory end @ read from Option Bytes (12-bit offset) */
#define SFU_LL_RSS_BANK1_SEC_USER_MEM_END_FROM_OB ((uint32_t)((SFU_LL_RSS_BANK1_SEC_USER_MEM_END>>8)<<8))
/**
  * @}
  */


/** @defgroup SFU_SECURITY_RSS_Private_Variables RSS Private Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup SFU_SECURITY_RSS_Private_Functions RSS Private Functions
  * @{
  */

/**
  * @}
  */

/** @defgroup SFU_SECURITY_RSS_Exported_Functions RSS Exported Functions
  * @{
  */
SFU_ErrorStatus SFU_LL_RSS_CheckProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Test if Secure Bit is set.
   * Secure access mode on the device is configured through the SECURITY option bit in FLASH_OPTSR_CUR register.
   */
  if ((psFlashOptionBytes->USERConfig & FLASH_OPTSR_SECURITY) != RESET)
  {
    if ((psFlashOptionBytes->SecureAreaStartAddr == SFU_LL_RSS_BANK1_SEC_USER_MEM_START_FROM_OB) &&
        (psFlashOptionBytes->SecureAreaEndAddr == SFU_LL_RSS_BANK1_SEC_USER_MEM_END_FROM_OB))
    {
      /*
       * In this example we use one secure area in bank 1, covering:
       * - the SB_SFU code
       * - the header of the slot #0
       */
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n= [SBOOT] Secure User memory Area settings: [%x-%x]", psFlashOptionBytes->SecureAreaStartAddr,
            ((uint32_t)(psFlashOptionBytes->SecureAreaEndAddr | 0x000000FF)));
#endif /* SFU_VERBOSE_DEBUG_MODE */
      e_ret_status = SFU_SUCCESS;
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SEC_MEM, FLOW_CTRL_SEC_MEM);
    }
    else
    {
      /* else the settings are incorrect: ERROR */
      TRACE("\r\n= [SBOOT] Incorrect Secure User memory Area settings - s:%x e:%x !",
            psFlashOptionBytes->SecureAreaStartAddr,     psFlashOptionBytes->SecureAreaEndAddr);
    }
  }
  else
  {
    /*
     * Else: ERROR
     *    Secure Bit Not Set.
     *    If this bit is not set then Secure User Software cannot run.
     *    In this example this means that SB_SFU would be bypassed.
     */
    TRACE("\r\n= [SBOOT] SECURITY bit not set !");
  }

  return e_ret_status;
}


/**
  * @brief  Apply Secure User Memory protection.
  * @param  psFlashOptionBytes: pointer to the Option Bytes structure.
  * @param  pbIsProtectionToBeApplied: Output parameter to be set as "TRUE" if
  *         this OptByte has to be modified and immediately reloaded.
  * @retval If Protection is not set, RSS procedure is called with a System Reset
  */
SFU_ErrorStatus SFU_LL_RSS_SetProtectionSecUser(FLASH_OBProgramInitTypeDef *psFlashOptionBytes,
                                                SFU_BoolTypeDef *pbIsProtectionToBeApplied)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_LL_RSS_SecureArea_t aSecureAreas[SFU_LL_RSS_NB_SECURE_AREAS];

  /* Check if Secure User Memory is already configured */
  if (SFU_LL_RSS_CheckProtectionSecUser(psFlashOptionBytes) == SFU_SUCCESS)
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    /* Apply the proper configuration if we are in development mode */
#if defined(SECBOOT_OB_DEV_MODE)
    /* Set the security bit if needed */
    /* ------------------------------ */
    if ((psFlashOptionBytes->USERConfig & FLASH_OPTSR_SECURITY) == RESET)
    {
      psFlashOptionBytes->Banks = FLASH_BANK_1;
      psFlashOptionBytes->OptionType = OPTIONBYTE_USER;
      psFlashOptionBytes->USERType = OB_USER_SECURITY;
      psFlashOptionBytes->USERConfig = OB_SECURITY_ENABLE;

      if (HAL_FLASHEx_OBProgram(psFlashOptionBytes) == HAL_OK)
      {
        TRACE("\r\n\t Security bit set.");
        /*
          * We need to reload the OBs before configuring the secure user memory area,
          * otherwise the configuration is skipped because Secure Mode is not entered at reset (not booting in RSS).
          *
          * Note: updating pbIsProtectionToBeApplied and e_ret_status would be useless here.
          */
        HAL_FLASH_OB_Launch();
      }
      else
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n\t Cannot set the security bit - ERROR");
#endif /* SFU_VERBOSE_DEBUG_MODE */
        return (SFU_ERROR);
      }
    }
    /* else only the secure user memory area needs to be set */

    /* Set the secure user memory area */
    /* ------------------------------- */
    aSecureAreas[0].sizeInBytes = SFU_LL_RSS_BANK1_SEC_USER_MEM_SIZE;
    aSecureAreas[0].startAddress = SFU_LL_RSS_BANK1_SEC_USER_MEM_START;
    aSecureAreas[0].removeDuringBankErase = 1;
    /* Only 1 secure area is used */
    aSecureAreas[1].sizeInBytes = 0;
    aSecureAreas[1].startAddress = (uint32_t) NULL;
    aSecureAreas[1].removeDuringBankErase = 1;

    TRACE("\r\n= [SBOOT] SecArea.size: %d SecArea.addr:%x - RESET", aSecureAreas[0].sizeInBytes,
          aSecureAreas[0].startAddress);

    /* no need to set pbIsProtectionToBeApplied and e_ret_status because the next function triggers a RESET */
    SFU_LL_RSS->resetAndInitializeSecureAreas(1, aSecureAreas);
    /* System reset triggered by previous function */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SEC_MEM, FLOW_CTRL_SEC_MEM);
#else
    TRACE("\r\n= [SBOOT] System Security Configuration failed: Secure User Memory is incorrect. STOP!");
    SFU_EXCPT_Security_Error();
#endif /* SECBOOT_OB_DEV_MODE */
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

#endif /* SFU_SECURE_USER_PROTECT_ENABLE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
