/**
  ******************************************************************************
  * @file    sfu_fwimg_services.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the Firmware Images.
  *          This file contains the services the bootloader (sfu_boot) can use to deal
  *          with the FW images handling.
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

#define SFU_FWIMG_SERVICES_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level_security_rss.h"
#include "sfu_low_level.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_mpu_isolation.h"        /* Secure Engine isolated thanks to MPU */



/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @addtogroup SFU_IMG SFU Firmware Image
  * @{
  */

/** @defgroup SFU_IMG_SERVICES SFU Firmware Image Services
  * @brief FW handling services the bootloader can call.
  * @{
  */

/** @defgroup SFU_IMG_Exported_Functions FW Images Handling Services
  * @brief Services the bootloader can call to handle the FW images.
  * @{
  */

/** @defgroup SFU_IMG_Initialization_Functions FW Images Handling Init. Functions
  * @brief FWIMG initialization functions.
  * @{
  */

/**
  * @brief FW Image Handling (FWIMG) initialization function.
  *        Checks the validity of the settings related to image handling (slots size and alignments...).
  * @note  The system initialization must have been performed before calling this function (flash driver ready to be
  *        used...etc...).
  *        Must be called first (and once) before calling the other Image handling services.
  * @param  None.
  * @retval SFU_IMG_InitStatusTypeDef SFU_IMG_INIT_OK if successful, an error code otherwise.
  */
SFU_IMG_InitStatusTypeDef SFU_IMG_InitImageHandling(void)
{
  /*
   * At the moment there is nothing more to do than initializing the fwimg_core part.
   */
  return (SFU_IMG_CoreInit());
}

/**
  * @brief FW Image Handling (FWIMG) de-init function.
  *        Terminates the images handling service.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ShutdownImageHandling(void)
{
  /*
   * At the moment there is nothing more to do than shutting down the fwimg_core part.
   */
  return (SFU_IMG_CoreDeInit());
}

/**
  * @}
  */

/** @defgroup SFU_IMG_Service_Functions FW Images Handling Services Functions
  * @brief FWIMG services functions.
  * @{
  */


#if (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
/** @defgroup SFU_IMG_Service_Functions_NewImg New image installation services
  * @brief New image installation functions.
  * @{
  */
/**
  * @brief Installs a new firmware, performs the post-installation checks and sets the metadata to tag this firmware as
  *        valid if the checks are successful.
  *        Cryptographic operations are used (if enabled): the firmware is decrypted and its authenticity is checked
  *        afterwards if the cryptographic scheme allows this (signature check for instance).
  *        The detailed errors are memorized in bootinfo area to be processed as critical errors if needed.
  *        This function modifies the FLASH content.
  *        If this procedure is interrupted during image preparation stage (e.g.: switch off) it cannot be resumed.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_TriggerImageInstallation(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /*
   * In this example, installing the new firmware image consists in decrypting the candidate firmware image.
   */
  e_ret_status = SFU_IMG_InstallNewVersion();

  if (SFU_SUCCESS != e_ret_status)
  {
    /* Erase downloaded FW in case of installation  error */
    (void) SFU_IMG_EraseDownloadedImg();
  }

  /* return the installation result */
  return (e_ret_status);
}

/**
  * @}
  */
#endif /* SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER */


/** @defgroup SFU_IMG_Service_Functions_CurrentImage Active Firmware services (current image)
  * @brief Active Firmware Image functions.
  * @{
  */

/**
  * @brief This function makes sure the current active firmware will not be considered as valid any more.
  *        This function alters the FLASH content.
  * @note It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *       running the procedure).
  * @param  None.
  * @retval SFU_SUCCESS if successful,SFU_ERROR error otherwise.
  */
SFU_ErrorStatus SFU_IMG_InvalidateCurrentFirmware(void)
{
  SFU_ErrorStatus e_ret_status;

  /* Reload Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  /* erase Slot0 except Header (under SECoreBin protection) for the anti-rollback check during next Fw update */
  e_ret_status = SFU_IMG_CleanUpSlot(SFU_IMG_SLOT_0_REGION_BEGIN, SFU_IMG_SLOT_0_REGION_SIZE, SFU_IMG_IMAGE_OFFSET);

  return (e_ret_status);
}

/**
  * @brief Verifies the validity of the active firmware image metadata.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  * @note This function populates the FWIMG module variable: fw_image_header_validated
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImgMetadata(void)
{
  /*
   * The active FW slot is slot #0.
   * If the metadata is valid then 'fw_image_header_validated' is filled with the metadata.
   */
  return (SFU_IMG_GetFWInfoMAC(&fw_image_header_validated, 0));
}

/**
  * @brief Verifies the validity of the active firmware image.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  *       Typically, SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate
  *       fw_image_header_validated.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImg(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status = SE_KO;

  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */
  e_ret_status = SFU_IMG_VerifyFwSignature(&e_se_status, &fw_image_header_validated, 0U, SE_FW_IMAGE_COMPLETE);
#if defined(SFU_VERBOSE_DEBUG_MODE)
  if (SFU_ERROR == e_ret_status)
  {
    /* We do not memorize any specific error, the FSM state is already providing the info */
    TRACE("\r\n=         SFU_IMG_VerifyActiveImg failure with se_status=%d!", e_se_status);
  }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  return (e_ret_status);
}

/**
  * @brief Verifies if no malicious code beyond Fw Image in Slot #0.
  * @note SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate fw_image_header_validated.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveSlot(void)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_0_REGION_BEGIN, SFU_IMG_SLOT_0_REGION_SIZE,
                             fw_image_header_validated.FwSize));
}

/**
  * @brief Verifies if no malicious code in Slot #0.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyEmptyActiveSlot(void)
{
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_0_REGION_BEGIN, SFU_IMG_SLOT_0_REGION_SIZE, 0U));
}

/**
  * @brief  Control slot#0 FW tag
  * @note   This control will be done twice for security reasons (first control done in VerifyTagScatter)
  * @param  None
  * @retval SFU_SUCCESS if successful,SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ControlActiveImgTag(void)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */
  return (SFU_IMG_ControlFwTag(fw_image_header_validated.FwTag));
}

/**
  * @brief Launches the user application.
  *        The caller must be prepared to never get the hand back after calling this function.
  *        If a problem occurs, it must be memorized in the bootinfo area.
  *        If the caller gets the hand back then this situation must be handled as a critical error.
  * @note It is up to the caller to make sure the conditions to call this primitive are met
  *       (typically: no security check performed before launching the firmware).
  * @note This function only handles the "system" aspects.
  *       It is up to the caller to manage any security related action (enable ITs, disengage MPU, clean RAM...).
  *       Nevertheless, cleaning-up the stack and heap used by SB_SFU is part of the system actions handled by this
  *       function (as this needs to be done just before jumping into the user application).
  * @param  None.
  * @retval SFU_ErrorStatus Does not return if successful, returns SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_LaunchActiveImg(void)
{
  /* Destroy the Volatile data and CSTACK in SRAM used by Secure Boot in order to prevent any access to sensitive data
     from the UserApp.
     If MPU isolation is used the Secure Engine CStack context switch is available, these data are the data
     outside the isolation, so not so sensitive, but still in this way we increase the level of security.
  */
  SFU_LL_SB_SRAM_Erase();

  /* this function does not return */
  SFU_MPU_SysCall((uint32_t)SB_SYSCALL_LAUNCH_APP, SFU_IMG_SLOT_0_REGION_BEGIN + SFU_IMG_IMAGE_OFFSET);

  /* The point below should NOT be reached */
  return (SFU_ERROR);
}

/**
  * @brief Get the version of the active FW (present in slot #0)
  * @note It is up to the caller to make sure the slot #0 contains a valid active FW.
  * @note In the current implementation the header is checked (authentication) and no version is returned if this check
  *       fails.
  * @param  None.
  * @retval the FW version if it succeeds (coded on uint16_t), -1 otherwise
  */
int32_t SFU_IMG_GetActiveFwVersion(void)
{
  SE_FwRawHeaderTypeDef fw_image_header;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  int32_t version = -1;

  /*  check the header in slot #0 */
  e_ret_status = SFU_IMG_GetFWInfoMAC(&fw_image_header, 0U);
  if (e_ret_status == SFU_SUCCESS)
  {
    /* retrieve the version from the header without any further check */
    version = (int32_t)fw_image_header.FwVersion;
  }

  return (version);
}

/**
  * @brief Indicates if a valid active firmware image is installed.
  *        It performs the same checks as SFU_IMG_VerifyActiveImgMetadata and SFU_IMG_VerifyActiveImg
  *        but it also verifies if the Firmware image is internally tagged as valid.
  * @note This function modifies the FWIMG variables 'fw_image_header_validated' and 'fw_header_validated'
  * @param  None.
  * @retval SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_HasValidActiveFirmware(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  /* check the installed header (metadata) */
  e_ret_status = SFU_IMG_VerifyActiveImgMetadata();

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* check the installed FW image itself */
    e_ret_status = SFU_IMG_VerifyActiveImg();
  }

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* check that this FW has been tagged as valid by the bootloader  */
    e_ret_status = SFU_IMG_CheckSlot0FwValid();
  }

  return e_ret_status;
}

/**
  * @brief  Validate the active FW image in slot #0 by installing the header and the VALID tags
  * @param  address of the header to be installed in slot #0
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_Validation(uint8_t *pHeader)
{
  return (SFU_IMG_WriteHeaderValidated(pHeader));
}

/**
  * @brief  Erase downloaded firmware in case of decryption/authentication/integrity error
  * @param  None
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_EraseDownloadedImg(void)
{
  SFU_FLASH_StatusTypeDef flash_if_status;
  SFU_ErrorStatus e_ret_status_dwl = SFU_SUCCESS;

  e_ret_status_dwl = SFU_LL_FLASH_Erase_Size(&flash_if_status, SFU_IMG_SLOT_DWL_REGION_BEGIN,
                                             SFU_IMG_SLOT_DWL_REGION_SIZE);
  STATUS_FWIMG(e_ret_status_dwl == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

  return ((e_ret_status_dwl != SFU_SUCCESS) ?  SFU_ERROR : SFU_SUCCESS);
}

/**
  * @brief Verifies if the slot #1 is erased.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyDownloadSlot(void)
{
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_DWL_REGION_BEGIN, SFU_IMG_SLOT_DWL_REGION_SIZE, 0));
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

/**
  * @}
  */

#undef SFU_FWIMG_SERVICES_C

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
