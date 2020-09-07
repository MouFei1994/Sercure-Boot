/**
  ******************************************************************************
  * @file    sfu_fwimg_core.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the Firmware Images.
  *          This file contains the "core" functionalities of the image handling.
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

#define SFU_FWIMG_CORE_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "se_interface_bootloader.h"
#include "sfu_fwimg_regions.h"
#include "sfu_fwimg_services.h" /* to have definitions like SFU_IMG_InitStatusTypeDef
                                   (required by sfu_fwimg_internal.h) */
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_boot.h"


/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @defgroup SFU_IMG SFU Firmware Image
  * @brief Firmware Images Handling  (FWIMG)
  * @{
  */

/** @defgroup SFU_IMG_CORE SFU Firmware Image Core
  * @brief Core functionalities of the FW image handling (trailers, magic, swap...).
  * @{
  */


/** @defgroup SFU_IMG_CORE_Private_Defines Private Defines
  * @brief FWIMG Defines used only by sfu_fwimg_core
  * @{
  */

/**
  * @brief RAM chunk used for decryption / comparison / swap
  * it is the size of RAM buffer allocated in stack and used for decrypting/moving images.
  * some function allocates 2 buffer of this size in stack.
  * As image are encrypted by 128 bits blocks, this value is 16 bytes aligned.
  * it can be equal to SWAP_SIZE , or SWAP_SIZE = N  * CHUNK_SIZE
  */
#define VALID_SIZE (3*MAGIC_LENGTH)
#define CHUNK_SIZE_SIGN_VERIFICATION (1024U)  /*!< Signature verification chunk size*/

#define SFU_IMG_CHUNK_SIZE  (512U)


#define AES_BLOCK_SIZE (16U)  /*!< Size of an AES block to check padding needs for decrypting */

/**
  * @}
  */

/** @defgroup SFU_IMG_CORE_Private_Types Private Types
  * @brief FWIMG Types used only by sfu_fwimg_core
  * @{
  */

/**
  * @}
  */

/** @defgroup SFU_IMG_CORE_Private_Variable Private Variables
  * @brief FWIMG Module Variables used only by sfu_fwimg_core
  * @{
  */

/**
  * FW header (metadata) of the active FW in slot #0: raw format (access by bytes)
  */
static uint8_t fw_header_validated[FW_INFO_TOT_LEN] __attribute__((aligned(4)));
static uint8_t fw_tag_validated[SE_TAG_LEN];
#if (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
static  uint8_t decrypted_sector[FLASH_SECTOR_SIZE] __attribute__((aligned(4)));
#endif /* SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER */

/**
  * @}
  */

/** @defgroup SFU_IMG_CORE_Private_Functions Private Functions
  *  @brief Private functions used by fwimg_core internally.
  *  @note All these functions should be declared as static and should NOT have the SFU_IMG prefix.
  * @{
  */


/**
  * @brief  Memory compare with constant time execution.
  * @note   Objective is to avoid basic attacks based on time execution
  * @param  pAdd1 Address of the first buffer to compare
  * @param  pAdd2 Address of the second buffer to compare
  * @param  Size Size of the comparison
  * @retval SFU_ SUCCESS if equal, a SFU_error otherwise.
  */
static SFU_ErrorStatus MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size)
{
  uint8_t result = 0x00;
  uint32_t i;

  for (i = 0; i < Size; i++)
  {
    result |= pAdd1[i] ^ pAdd2[i];
  }

  if (result == 0x00)
  {
    return SFU_SUCCESS;
  }
  else
  {
    return SFU_ERROR;
  }
}


/**
  * @brief  Check raw header
  * @param  pFWinfoInput: pointer to raw header. This must match SE_FwRawHeaderTypeDef.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus VerifyFwRawHeaderTag(uint8_t *pFWInfoInput)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef se_status;

  if (SE_VerifyFwRawHeaderTag(&se_status, (SE_FwRawHeaderTypeDef *)pFWInfoInput) == SE_SUCCESS)
  {
    FLOW_STEP(uFlowCryptoValue, FLOW_STEP_AUTHENTICATE);
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;
}


/**
  * @brief Secure Engine Firmware TAG verification (FW in non contiguous area).
  *        It handles Firmware TAG verification of a complete buffer by calling
  *        SE_AuthenticateFW_Init, SE_AuthenticateFW_Append and SE_AuthenticateFW_Finish inside the firewall.
  * @note: AES_GCM tag: In order to verify the TAG of a buffer, the function will re-encrypt it
  *        and at the end compare the obtained TAG with the one provided as input
  *        in pSE_GMCInit parameter.
  * @note: SHA-256 tag: a hash of the firmware is performed and compared with the digest stored in the Firmware header.
  * @param pSE_Status: Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pSE_Metadata: Firmware metadata.
  * @param pSE_Payload: pointer to Payload Buffer descriptor.
  * @param SE_FwType: Type of Fw Image.
  *        This parameter can be SE_FW_IMAGE_COMPLETE or SE_FW_IMAGE_PARTIAL.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
static SFU_ErrorStatus VerifyTagScatter(SE_StatusTypeDef *pSeStatus, SE_FwRawHeaderTypeDef *pSE_Metadata,
                                        SE_Ex_PayloadDescTypeDef  *pSE_Payload, int32_t SE_FwType)
{
  SE_ErrorStatus se_ret_status = SE_ERROR;
  SFU_ErrorStatus sfu_ret_status = SFU_SUCCESS;
  /* Loop variables */
  uint32_t i = 0;
  uint32_t j = 0;
  /* Variables to handle the FW image chunks to be injected in the verification procedure and the result */
  int32_t fw_tag_len = 0;             /* length of the authentication tag to be verified */
  int32_t fw_verified_total_size = 0; /* number of bytes that have been processed during authentication check */
  int32_t fw_chunk_size;              /* size of a FW chunk to be verified */
  /* Authentication tag computed in this procedure (to be compared with the one stored in the FW metadata) */
  uint8_t fw_tag_output[SE_TAG_LEN] __attribute__((aligned(4)));
  /* FW chunk produced by the verification procedure if any   */
  uint8_t fw_chunk[CHUNK_SIZE_SIGN_VERIFICATION] __attribute__((aligned(4)));
  /* FW chunk provided as input to the verification procedure */
  uint8_t fw_image_chunk[CHUNK_SIZE_SIGN_VERIFICATION] __attribute__((aligned(4)));
  /* Variables to handle the FW image (this will be split in chunks) */
  int32_t payloadsize;
  uint8_t *ppayload;
  uint32_t scatter_nb;
  /* Variables to handle FW image size and tag */
  uint32_t fw_size;
  uint8_t *fw_tag;

  /* Check the pointers allocation */
  if ((pSeStatus == NULL) || (pSE_Metadata == NULL) || (pSE_Payload == NULL))
  {
    return SFU_ERROR;
  }
  if ((pSE_Payload->pPayload[0] == NULL) || ((pSE_Payload->pPayload[1] == NULL) && pSE_Payload->PayloadSize[1]))
  {
    return SFU_ERROR;
  }

  /* Check the parameters value and set fw_size and fw_tag to check */
  if (SE_FwType == SE_FW_IMAGE_COMPLETE)
  {
    fw_size = pSE_Metadata->FwSize;
    fw_tag = pSE_Metadata->FwTag;
  }
  else if (SE_FwType == SE_FW_IMAGE_PARTIAL)
  {
    fw_size = pSE_Metadata->PartialFwSize;
    fw_tag = pSE_Metadata->PartialFwTag;
  }
  else
  {
    return SFU_ERROR;
  }

  if ((pSE_Payload->PayloadSize[0] + pSE_Payload->PayloadSize[1]) != fw_size)
  {
    return SFU_ERROR;
  }

  /*  fix number of scatter block */
  if (pSE_Payload->PayloadSize[1])
  {
    scatter_nb = 2;
  }
  else
  {
    scatter_nb = 1;
  }


  /* Encryption process*/
  se_ret_status = SE_AuthenticateFW_Init(pSeStatus, pSE_Metadata, SE_FwType);

  /* check for initialization errors */
  if ((se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
  {
    for (j = 0; j < scatter_nb; j++)
    {
      payloadsize = pSE_Payload->PayloadSize[j];
      ppayload = pSE_Payload->pPayload[j];
      i = 0;
      fw_chunk_size = CHUNK_SIZE_SIGN_VERIFICATION;

      while ((i < (payloadsize / CHUNK_SIZE_SIGN_VERIFICATION)) && (*pSeStatus == SE_OK) &&
             (sfu_ret_status == SFU_SUCCESS))
      {

        sfu_ret_status = SFU_LL_FLASH_Read(fw_image_chunk, ppayload, fw_chunk_size) ;
        if (sfu_ret_status == SFU_SUCCESS)
        {
          se_ret_status = SE_AuthenticateFW_Append(pSeStatus, fw_image_chunk, fw_chunk_size,
                                                   fw_chunk, &fw_chunk_size);
        }
        else
        {
          *pSeStatus = SE_ERR_FLASH_READ;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;
        }
        ppayload += fw_chunk_size;
        fw_verified_total_size += fw_chunk_size;
        i++;
      }
      /* this the last path , size can be smaller */
      fw_chunk_size = (uint32_t)pSE_Payload->pPayload[j] + pSE_Payload->PayloadSize[j] - (uint32_t)ppayload;
      if ((fw_chunk_size) && (se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
      {
        sfu_ret_status = SFU_LL_FLASH_Read(fw_image_chunk, ppayload, fw_chunk_size) ;
        if (sfu_ret_status == SFU_SUCCESS)
        {

          se_ret_status = SE_AuthenticateFW_Append(pSeStatus, fw_image_chunk,
                                                   payloadsize - i * CHUNK_SIZE_SIGN_VERIFICATION, fw_chunk,
                                                   &fw_chunk_size);
        }
        else
        {
          *pSeStatus = SE_ERR_FLASH_READ;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;
        }
        fw_verified_total_size += fw_chunk_size;
      }
    }
  }

  if ((sfu_ret_status == SFU_SUCCESS) && (se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
  {
    if (fw_verified_total_size <= fw_size)
    {
      /* Do the Finalization, check the authentication TAG*/
      fw_tag_len = sizeof(fw_tag_output);
      se_ret_status =   SE_AuthenticateFW_Finish(pSeStatus, fw_tag_output, &fw_tag_len);

      if ((se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK) && (fw_tag_len == SE_TAG_LEN))
      {
        /* Firmware tag verification */
        if (MemoryCompare(fw_tag_output, fw_tag, SE_TAG_LEN) != SFU_SUCCESS)
        {
          *pSeStatus = SE_SIGNATURE_ERR;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;
          memset(fw_tag_validated, 0x00, SE_TAG_LEN);
        }
        else
        {
          FLOW_STEP(uFlowCryptoValue, FLOW_STEP_INTEGRITY);
          memcpy(fw_tag_validated, fw_tag, SE_TAG_LEN);
        }
      }
      else
      {
        sfu_ret_status = SFU_ERROR;
      }
    }
    else
    {
      sfu_ret_status = SFU_ERROR;
    }
  }
  else
  {
    sfu_ret_status = SFU_ERROR;
  }
  return sfu_ret_status;
}


/**
  * @brief Secure Engine Firmware TAG verification (FW in contiguous area).
  *        It handles Firmware TAG verification of a complete buffer by calling
  *        SE_AuthenticateFW_Init, SE_AuthenticateFW_Append and SE_AuthenticateFW_Finish inside the firewall.
  * @note: AES_GCM tag: In order to verify the TAG of a buffer, the function will re-encrypt it
  *        and at the end compare the obtained TAG with the one provided as input
  *        in pSE_GMCInit parameter.
  * @note: SHA-256 tag: a hash of the firmware is performed and compared with the digest stored in the Firmware header.
  * @param pSE_Status: Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pSE_Metadata: Firmware metadata.
  * @param pPayload: pointer to Payload Buffer.
  * @param SE_FwType: Type of Fw Image.
  *        This parameter can be SE_FW_IMAGE_COMPLETE or SE_FW_IMAGE_PARTIAL.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
static SFU_ErrorStatus VerifyTag(SE_StatusTypeDef *pSeStatus, SE_FwRawHeaderTypeDef *pSE_Metadata,
                                 uint8_t  *pPayload, int32_t SE_FwType)
{
  SE_Ex_PayloadDescTypeDef  pse_payload;
  uint32_t fw_size;
  uint32_t fw_offset;

  if (NULL == pSE_Metadata)
  {
    /* This should not happen */
    return SFU_ERROR;
  }

  /* Check SE_FwType parameter, and fix size and offset accordingly */
  if (SE_FwType == SE_FW_IMAGE_COMPLETE)
  {
    fw_size = pSE_Metadata->FwSize;
    fw_offset = 0;
  }
  else if (SE_FwType == SE_FW_IMAGE_PARTIAL)
  {
    fw_size = pSE_Metadata->PartialFwSize;
    fw_offset = pSE_Metadata->PartialFwOffset;
  }
  else
  {
    return SFU_ERROR;
  }

  pse_payload.pPayload[0] = pPayload + fw_offset;
  pse_payload.PayloadSize[0] = fw_size;
  pse_payload.pPayload[1] = NULL;
  pse_payload.PayloadSize[1] = 0;

  return  VerifyTagScatter(pSeStatus, pSE_Metadata, &pse_payload, SE_FwType);

}


/**
  * @brief Fill authenticated info in SE_FwImage.
  * @param SFU_APP_Status
  * @param pBuffer
  * @param BufferSize
  * @retval SFU_SUCCESS if successful, a SFU_ERROR otherwise.
  */
static SFU_ErrorStatus ParseFWInfo(SE_FwRawHeaderTypeDef *pFwHeader, uint8_t *pBuffer)
{
  /* Check the pointers allocation */
  if ((pFwHeader == NULL) || (pBuffer == NULL))
  {
    return SFU_ERROR;
  }
  memcpy(pFwHeader, pBuffer, sizeof(*pFwHeader));
  return SFU_SUCCESS;
}


/**
  * @brief  Check  that a file header is valid
  *         (Check if the header has a VALID tag)
  * @param  phdr  pointer to header to check
  * @retval SFU_ SUCCESS if valid, a SFU_ErrorStatus error otherwise.
  */
static SFU_ErrorStatus CheckHeaderValidated(uint8_t *phdr)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  if (memcmp(phdr + FW_INFO_TOT_LEN - FW_INFO_MAC_LEN, phdr + FW_INFO_TOT_LEN, MAGIC_LENGTH))
  {
    return  e_ret_status;
  }
  if (memcmp(phdr + FW_INFO_TOT_LEN - FW_INFO_MAC_LEN, phdr + FW_INFO_TOT_LEN + MAGIC_LENGTH, MAGIC_LENGTH))
  {
    return  e_ret_status;
  }
  return SFU_SUCCESS;
}



#if (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
/**
  * @brief Decrypt Image in slot #0
  * @ note Decrypt is done "in place" from slot 0 to slot 0
  * @param  pFwImageHeader
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
static SFU_ErrorStatus DecryptImageInSlot0(SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status;
  SE_ErrorStatus   se_ret_status;
  uint32_t NumberOfChunkPerSector = FLASH_SECTOR_SIZE / SFU_IMG_CHUNK_SIZE;
  SFU_FLASH_StatusTypeDef flash_if_status;
  /*  chunk size is the maximum , the 1st block can be smaller */
  /*  the chunk is static to avoid  large stack */
  uint8_t fw_encrypted_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(4)));
  uint32_t fw_dest_address_write = 0U;
  uint32_t fw_dest_erase_address = 0U;
  int32_t fw_decrypted_total_size = 0;
  int32_t size;
  int32_t fw_decrypted_chunk_size;
  int32_t fw_tag_len = 0;
  uint8_t fw_tag_output[SE_TAG_LEN];
  uint32_t pass_index = 0;
  uint32_t erase_index = 0;

  uint8_t *pfw_source_address;
  uint32_t decrypted_sector_index = 0;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  int32_t i;
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  if ((pFwImageHeader == NULL))
  {
    return e_ret_status;
  }

  /* Decryption process initialization*/
  se_ret_status = SE_Decrypt_Init(&e_se_status, pFwImageHeader, SE_FW_IMAGE_COMPLETE);
  if ((se_ret_status == SE_SUCCESS) && (e_se_status == SE_OK))
  {
    e_ret_status = SFU_SUCCESS;
    size = SFU_IMG_CHUNK_SIZE;

    /* Chunk Decryption loop*/
    while ((e_ret_status == SFU_SUCCESS) && (fw_decrypted_total_size < (pFwImageHeader->FwSize))
           && (e_se_status == SE_OK))
    {
      if (pass_index == 0)
      {
        /* Read header. No decryption */
        e_ret_status = SFU_LL_FLASH_Read(decrypted_sector, (uint8_t *)((uint32_t)SFU_IMG_SLOT_0_REGION_BEGIN),
                                         SFU_IMG_IMAGE_OFFSET);
        decrypted_sector_index += SFU_IMG_IMAGE_OFFSET;

        /* pointers initialization phase */
        pfw_source_address = (uint8_t *)((uint32_t)SFU_IMG_SLOT_0_REGION_BEGIN + SFU_IMG_IMAGE_OFFSET);
        fw_dest_erase_address = (uint32_t)SFU_IMG_SLOT_0_REGION_BEGIN;
        fw_dest_address_write = (uint32_t)(SFU_IMG_SLOT_0_REGION_BEGIN);    /* Single image ==> Decrypt in place */
        erase_index = NumberOfChunkPerSector;

        /*
         * Warning : SFU_IMG_CHUNK_SIZE should be equal to SFU_IMG_IMAGE_OFFSET
         * ==> first chunk is the header - no decryption needed : pass_index = 1
         */
        pass_index = 1;
      }

      /* Default chunk size can be troncated at end of file */
      fw_decrypted_chunk_size = SFU_IMG_CHUNK_SIZE;

      /* Last pass : n */
      if ((pFwImageHeader->FwSize - fw_decrypted_total_size) < fw_decrypted_chunk_size)
      {
        fw_decrypted_chunk_size = pFwImageHeader->FwSize - fw_decrypted_total_size;
      }
      /* All others pass */
      else
      {
        /* nothing */
      }

      size = fw_decrypted_chunk_size;

      /* Decrypt Append*/
      e_ret_status = SFU_LL_FLASH_Read(fw_encrypted_chunk, pfw_source_address, size);
      if (e_ret_status == SFU_ERROR)
      {
        break;
      }
      if (size != 0)
      {
        se_ret_status = SE_Decrypt_Append(&e_se_status, (uint8_t *)fw_encrypted_chunk, size,
                                          (uint8_t *)(decrypted_sector + decrypted_sector_index),
                                          &fw_decrypted_chunk_size);
      }
      else
      {
        e_ret_status = SFU_SUCCESS;
        fw_decrypted_chunk_size = 0;
      }
      if ((se_ret_status == SE_SUCCESS) && (e_se_status == SE_OK) && (fw_decrypted_chunk_size == size))
      {
        decrypted_sector_index += size;
        fw_decrypted_total_size += size;

        /* Erase Page */
        if (pass_index == erase_index - 1 || (fw_decrypted_total_size == pFwImageHeader->FwSize))
        {
          SFU_LL_SECU_IWDG_Refresh();
          e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, (void *)fw_dest_erase_address, FLASH_PAGE_SIZE - 1);
          erase_index += NumberOfChunkPerSector;
          fw_dest_erase_address += FLASH_PAGE_SIZE;

          /*
           * After erase, write the same sector with decrypted data
           * pass_index ==(NumberOfChunkPerSector-1) ==> first sector : wite header + beginning of FW
           * other sector : write FW
           */
          if (pass_index == (NumberOfChunkPerSector - 1))
          {
            /* Write Header and let free the remaining room until beginning of FW ...  */
            e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, (void *)fw_dest_address_write,  decrypted_sector,
                                              FW_INFO_TOT_LEN);
            e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, (void *)(fw_dest_address_write + SFU_IMG_IMAGE_OFFSET),
                                              (uint8_t *)(decrypted_sector + SFU_IMG_IMAGE_OFFSET), FLASH_PAGE_SIZE -
                                              SFU_IMG_IMAGE_OFFSET);

          }
          else
          {
            e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, (void *)fw_dest_address_write,  decrypted_sector,
                                              FLASH_PAGE_SIZE);
          }
          STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_WRITE_FAILED);

          if (e_ret_status == SFU_SUCCESS)
          {
            decrypted_sector_index = 0;
            memset(decrypted_sector, 0xff, sizeof(decrypted_sector));
            fw_dest_address_write  += (FLASH_PAGE_SIZE); /* Next sector */

          }
        }
        /* Update source pointer */
        pfw_source_address += size;
        pass_index += 1;
      }
    }
  }

#if (SFU_IMAGE_PROGRAMMING_TYPE == SFU_ENCRYPTED_IMAGE)
#if defined(SFU_VERBOSE_DEBUG_MODE)
  TRACE("\r\n\t  %d bytes of ciphertext decrypted.", fw_decrypted_total_size);
#endif /* SFU_VERBOSE_DEBUG_MODE */
#endif /* SFU_ENCRYPTED_IMAGE */

  if ((se_ret_status == SE_SUCCESS) && (e_ret_status == SFU_SUCCESS) && (e_se_status == SE_OK))
  {
    /* Do the Finalization, check the authentication TAG*/
    fw_tag_len = sizeof(fw_tag_output);
    se_ret_status = SE_Decrypt_Finish(&e_se_status, fw_tag_output, &fw_tag_len);
    if ((se_ret_status != SE_SUCCESS) || (e_se_status != SE_OK))
    {
      e_ret_status = SFU_ERROR;
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n\t  Decrypt fails at Finalization stage.");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    }
  }
  else
  {
    e_ret_status = SFU_ERROR;
  }
  return e_ret_status;
}
#endif /* SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER */


/**
  * @}
  */

/** @defgroup SFU_IMG_CORE_Exported_Functions Exported Functions
  * @brief Functions used by fwimg_services
  * @note All these functions are also listed in the Common services (High Level and Low Level Services).
  * @{
  */

/**
  * @brief  Initialize the SFU APP.
  * @param  None
  * @note   Not used in Alpha version -
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_IMG_InitStatusTypeDef SFU_IMG_CoreInit(void)
{
  SFU_IMG_InitStatusTypeDef e_ret_status = SFU_IMG_INIT_OK;

  /*
   * When there is no valid FW in slot 0, the fw_header_validated array is filled with 0s.
   * When installing a first FW (after local download) this means that WRITE_TRAILER_MAGIC will write a SWAP magic
   * starting with 0s.
   * This causes an issue when calling CLEAN_TRAILER_MAGIC (because of this we added an erase that generated
   * side-effects).
   * To avoid all these problems we can initialize fw_header_validated with a non-0  value.
   */
  memset(fw_header_validated, 0xFE, sizeof(fw_header_validated));


  /*
   * Sanity check: let's make sure all slots are properly aligned with regards to flash constraints
   */
  if (!IS_ALIGNED(SFU_IMG_SLOT_0_REGION_BEGIN_VALUE))
  {
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
    TRACE("\r\n= [FWIMG] slot 0 (%x) is not properly aligned: please tune your settings",
          SFU_IMG_SLOT_0_REGION_BEGIN_VALUE);
  } /* else slot 0 is properly aligned */


  /*
   * Sanity check: let's make sure the MAGIC patterns used by the internal algorithms match the FLASH constraints.
   */
  if (0 != (uint32_t)(MAGIC_LENGTH % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
    TRACE("\r\n= [FWIMG] magic size (%d) is not matching the FLASH constraints", MAGIC_LENGTH);
  } /* else the MAGIC patterns size is fine with regards to FLASH constraints */

  /*
   * Sanity check: let's make sure the Firmware Header Length is fine with regards to FLASH constraints
   */
  if (0 != (uint32_t)(SE_FW_HEADER_TOT_LEN % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    /* The code writing the FW header in FLASH requires the FW Header length to match the FLASH constraints */
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
    TRACE("\r\n= [FWIMG] FW Header size (%d) is not matching the FLASH constraints", SE_FW_HEADER_TOT_LEN);
  } /* else the FW Header Length is fine with regards to FLASH constraints */

  /*
   * Sanity check: let's make sure the slot 0 header does not overlap SB code area protected by WRP)
   */
  if ((SFU_LL_FLASH_GetSector(SFU_IMG_SLOT_0_REGION_HEADER_VALUE)) <= SFU_PROTECT_WRP_PAGE_END_1)
  {
    TRACE("\r\n= [FWIMG] SLOT 0 overlaps SBSFU code area protected by WRP\r\n");
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
  }



  /* FWIMG core initialization completed */
  return e_ret_status;
}


/**
  * @brief  DeInitialize the SFU APP.
  * @param  None
  * @note   Not used in Alpha version.
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_CoreDeInit(void)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  /*
   *  To Be Reviewed after Alpha.
  */
  return e_ret_status;
}


/**
  * @brief  Check that the FW in slot #0 has been tagged as valid by the bootloader.
  *         This function populates the FWIMG module variable: 'fw_header_validated'
  * @param  None.
  * @note It is up to the caller to make sure that slot#0 contains a valid firmware first.
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus  SFU_IMG_CheckSlot0FwValid()
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint8_t fw_header_slot[FW_INFO_TOT_LEN + VALID_SIZE];

  /*
   * It is up to the caller to make sure that slot#0 contains a valid firmware before calling this function.
   * Extract the header.
   */
  e_ret_status = SFU_LL_FLASH_Read(fw_header_slot, (uint8_t *) SFU_IMG_SLOT_0_REGION_HEADER, sizeof(fw_header_slot));

  STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Check the header is tagged as VALID */
    e_ret_status = CheckHeaderValidated(fw_header_slot);
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Populating the FWIMG module variable with the header */
    e_ret_status = SFU_LL_FLASH_Read(fw_header_validated, (uint8_t *) SFU_IMG_SLOT_0_REGION_BEGIN,
                                     sizeof(fw_header_validated));
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
  }

  return (e_ret_status);
}


/**
  * @brief  Verify Image Header in the slot given as a parameter
  * @param  pFwImageHeader pointer to a structure to handle the header info (filled by this function)
  * @param  SlotNumber slot #0 = active Firmware , slot #1 = downloaded image or backed-up image, slot #2 = swap region
  * @note   Not used in Alpha version -
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_GetFWInfoMAC(SE_FwRawHeaderTypeDef *pFwImageHeader, uint32_t SlotNumber)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint8_t *pbuffer;
  uint8_t  buffer[FW_INFO_TOT_LEN];
  if ((pFwImageHeader == NULL) || (SlotNumber > (SFU_SLOTS - 1)))
  {
    return SFU_ERROR;
  }
  pbuffer = (uint8_t *) SlotHeaderAddress[SlotNumber];

  /* use api read to detect possible ECC error */
  e_ret_status = SFU_LL_FLASH_Read(buffer, pbuffer, sizeof(buffer));
  if (e_ret_status == SFU_SUCCESS)
  {

    e_ret_status = VerifyFwRawHeaderTag(buffer);

    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = ParseFWInfo(pFwImageHeader, buffer);
    }
  }
  /*  cleaning */
  memset(buffer, 0, FW_INFO_TOT_LEN);
  return e_ret_status;
}


/**
  * @brief  Verify image signature of binary contiguous in flash
  * @param  pSeStatus pointer giving the SE status result
  * @param  pFwImageHeader pointer to fw header
  * @param  SlotNumber flash slot to check
  * @param SE_FwType: Type of Fw Image.
  *        This parameter can be SE_FW_IMAGE_COMPLETE or SE_FW_IMAGE_PARTIAL.
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_VerifyFwSignature(SE_StatusTypeDef  *pSeStatus, SE_FwRawHeaderTypeDef *pFwImageHeader,
                                          uint32_t SlotNumber, int32_t SE_FwType)
{
  uint8_t *pbuffer;
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  /*  put it OK, to discriminate error in SFU FWIMG parts */
  *pSeStatus = SE_OK;

  /* Check the parameters value */
  if ((pFwImageHeader == NULL) || (SlotNumber > (SFU_SLOTS - 1)))
  {
    return SFU_ERROR;
  }
  if ((SE_FwType != SE_FW_IMAGE_PARTIAL) && (SE_FwType != SE_FW_IMAGE_COMPLETE))
  {
    return SFU_ERROR;
  }

  pbuffer = (uint8_t *)(SlotBeginAddress[SlotNumber] + SFU_IMG_IMAGE_OFFSET);


  /* Signature Verification */
  e_ret_status = VerifyTag(pSeStatus, pFwImageHeader, (uint8_t *) pbuffer, SE_FwType);

  return e_ret_status;
}


/**
  * @brief  Write a valid header in slot #0
  * @param  pHeader Address of the header to be installed in slot #0
  * @retval SFU_ SUCCESS if valid, a SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_WriteHeaderValidated(uint8_t *pHeader)
{
  /*  get header from counter area  */
  SFU_ErrorStatus e_ret_status;
  SFU_FLASH_StatusTypeDef flash_if_status;
  /* The VALID magic is made of: 3*active FW header */
  uint8_t  FWInfoInput[FW_INFO_TOT_LEN + VALID_SIZE];
  uint8_t *pDestFWInfoInput;
  uint8_t *pSrcFWInfoInput;
  uint32_t i;
  uint32_t j;

  e_ret_status =  SFU_LL_FLASH_Read(FWInfoInput, pHeader,  FW_INFO_TOT_LEN);
  STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
  /*  compute Validated Header */
  if (e_ret_status == SFU_SUCCESS)
  {
    for (i = 0U, pDestFWInfoInput = FWInfoInput + FW_INFO_TOT_LEN; i < 3U; i++)
    {
      for (j = 0U, pSrcFWInfoInput = FWInfoInput + FW_INFO_TOT_LEN - FW_INFO_MAC_LEN; j < MAGIC_LENGTH; j++)
      {
        *pDestFWInfoInput = *pSrcFWInfoInput;
        pDestFWInfoInput++;
        pSrcFWInfoInput++;
      }
    }
    /*  write in flash  */
    e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, SFU_IMG_SLOT_0_REGION_HEADER + FW_INFO_TOT_LEN, FWInfoInput +
                                      FW_INFO_TOT_LEN, VALID_SIZE);
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_WRITE_FAILED);
  }
  return e_ret_status;
}

/**
  * @brief Verifies the validity of a slot.
  * @note  Control if there is no additional code beyond the firmware image (malicious SW).
  * @param pSlotBegin Start address of a slot.
  * @param uSlotSize Size of a slot.
  * @param uFwSize Size of the firmware image.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifySlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uFwSize)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  SFU_LL_FLASH_write_t pattern1;
  SFU_LL_FLASH_write_t pattern2;
  SFU_LL_FLASH_write_t value;
  uint32_t pdata;

  /* Set dimension to the appropriate length for FLASH programming.
   * Example: 64-bit length for L4.
   */
  if ((uFwSize % (uint32_t)sizeof(SFU_LL_FLASH_write_t)) != 0)
  {
    uFwSize = uFwSize + ((uint32_t)sizeof(SFU_LL_FLASH_write_t) - (uFwSize % (uint32_t)sizeof(SFU_LL_FLASH_write_t)));
  }

  /* Clean-up pattern initialization */
  memset((void *)&pattern1, 0xFFU, sizeof(SFU_LL_FLASH_write_t));
  memset((void *)&pattern2, 0x00U, sizeof(SFU_LL_FLASH_write_t));

  /* Clean-up Check operation */
  for (pdata = (uint32_t)pSlotBegin + SFU_IMG_IMAGE_OFFSET + uFwSize;
       (pdata < ((uint32_t)pSlotBegin + uSlotSize)) && (e_ret_status == SFU_SUCCESS);
       pdata += sizeof(SFU_LL_FLASH_write_t))
  {
    e_ret_status = SFU_LL_FLASH_Read(&value, (void *) pdata, sizeof(SFU_LL_FLASH_write_t));
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);

    if (e_ret_status == SFU_SUCCESS)
    {
      /* Check is already clean */
      if ((memcmp((void *) &value, (void *) &pattern1, sizeof(SFU_LL_FLASH_write_t)) != 0U) &&
          (memcmp((void *) &value, (void *) &pattern2, sizeof(SFU_LL_FLASH_write_t)) != 0U))
      {
        e_ret_status = SFU_ERROR;
      }
    }

    /* Reload watchdoag every 64Kbytes */
    if (pdata % 0x10000U == 0U)
    {
      SFU_LL_SECU_IWDG_Refresh();
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function clean-up the flash
  * @note   Not designed to clean-up area inside secure engine isolation.
  * @param  pSlotBegin Start address of a slot.
  * @param  uSlotSize Size of a slot.
  * @param  uOffset Offset in the slot to start cleanup, until end of the slot.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_CleanUpSlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uOffset)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  uint32_t pdata;
  SFU_LL_FLASH_write_t pattern;
  SFU_LL_FLASH_write_t value;
  SFU_FLASH_StatusTypeDef flash_if_info;

  /* Clean-up pattern initialization */
  memset((void *)&pattern, 0x00U, sizeof(SFU_LL_FLASH_write_t));

  /* Cleaning operation */
  for (pdata = (uint32_t) pSlotBegin + uOffset; pdata < ((uint32_t) pSlotBegin + uSlotSize);
       pdata += sizeof(SFU_LL_FLASH_write_t))
  {
    e_ret_status =  SFU_LL_FLASH_Read(&value, (void *) pdata, sizeof(SFU_LL_FLASH_write_t));

    if (e_ret_status == SFU_SUCCESS)
    {
      /* If read succeed, then the pattern write is performed only if pattern is not already present */
      if (memcmp((void *) &value, (void *) &pattern, sizeof(SFU_LL_FLASH_write_t)) != 0U)
      {
        e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, (void *) pdata, (void *) &pattern,
                                          sizeof(SFU_LL_FLASH_write_t));
      }
    }
    else
    {
      /* If read fail, then the write is performed unconditionally */
      e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, (void *) pdata, (void *) &pattern,
                                        sizeof(SFU_LL_FLASH_write_t));
    }

    /* Reload Watchdog every 4Kbytes */
    if ((pdata % 0x1000U) == 0U)
    {
      SFU_LL_SECU_IWDG_Refresh();
    }
  }
  return e_ret_status;
}


#if (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
/**
  * @brief  Install the new version
  * @retval SFU_SUCCESS if successful,SFU_ERROR error otherwise.
  */
SFU_ErrorStatus SFU_IMG_InstallNewVersion(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status;
  SE_FwRawHeaderTypeDef fw_image_header;

  /*
   * check and populate SLOT0 header
   */
  e_ret_status = SFU_IMG_GetFWInfoMAC(&fw_image_header, 0U);
  if (e_ret_status != SFU_SUCCESS)
  {
    /*  Decrypt "in place" in slot 0 */
    (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_HEADER_AUTH_FAILED);
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [FWIMG] Decryption failure!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    return e_ret_status;
  }

  /*
   * Control if there is no additional code beyond the firmware image (malicious SW)
   */
  e_ret_status = SFU_IMG_VerifySlot(SFU_IMG_SLOT_DWL_REGION_BEGIN, SFU_IMG_SLOT_DWL_REGION_SIZE,
                                    fw_image_header.FwSize);
  if (e_ret_status != SFU_SUCCESS)
  {
    (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_FLASH_ERROR);
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [FWIMG] Additional code detected beyond FW image!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    return e_ret_status;
  }

  /*
   * Decrypt "in place" the FW image in Slot0
   */
  e_ret_status = DecryptImageInSlot0(&fw_image_header);
  if (e_ret_status != SFU_SUCCESS)
  {
    (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_DECRYPT_FAILURE);
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [FWIMG] Decryption failure!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    return e_ret_status;
  }

  /*
   * Verify signature after decrypt
   */
  e_ret_status = SFU_IMG_VerifyFwSignature(&e_se_status, &fw_image_header, 0U, SE_FW_IMAGE_COMPLETE);
  if (e_ret_status != SFU_SUCCESS)
  {
    (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_SIGNATURE_FAILURE);
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [FWIMG] The decrypted image is incorrect!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    return e_ret_status;
  }

  /*
   * validate immediately the new active FW
   */
  e_ret_status = SFU_IMG_Validation(SFU_IMG_SLOT_0_REGION_HEADER);
  if (e_ret_status != SFU_SUCCESS)
  {
    (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_FLASH_ERROR);
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\r\n= [FWIMG] Header validation failure!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    return e_ret_status;
  }

  /* Return the result of this preparation */
  return (e_ret_status);
}
#endif /* SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER */

/**
  * @brief  Control FW tag
  * @note   This control will be done twice for security reasons (first control done in VerifyTagScatter)
  * @param  pTag Tag of slot#0 firmware
  * @retval SFU_SUCCESS if successful,SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ControlFwTag(uint8_t *pTag)
{

  if (MemoryCompare(fw_tag_validated, pTag, SE_TAG_LEN) != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }
  else
  {
    FLOW_STEP(uFlowCryptoValue, FLOW_STEP_INTEGRITY);
    return SFU_SUCCESS;
  }
}

/**
  * @brief  Get size of the trailer
  * @note   This area mapped at the end of a slot#1 is not available for the firmware image
  * @param  None
  * @retval Size of the trailer.
  */
uint32_t SFU_IMG_GetTrailerSize(void)
{
  return 0U;
}

/**
  * @brief  Check candidate image version is allowed.
  * @param  CurrentVersion Version of currently installed image if any
  * @param  CandidateVersion Version of candidate image
  * @retval SFU_SUCCESS if candidate image version is allowed, SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_CheckFwVersion(int32_t CurrentVersion, int32_t CandidateVersion)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /*
    * It is not allowed to install a Firmware with a lower version than the active firmware.
    * But we authorize the re-installation of the current firmware version.
    * We also check that the candidate version is at least the min. allowed version for this device.
    */

  if ((CandidateVersion >= CurrentVersion) && (CandidateVersion >= SFU_FW_VERSION_START_NUM))
  {
    /* Candidate version is allowed */
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

/**
  * @}
  */

#undef SFU_FWIMG_CORE_C

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
