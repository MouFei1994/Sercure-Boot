/**
  ******************************************************************************
  * @file    sfu_interface_crypto_scheme.c
  * @author  MCD Application Team
  * @brief   Secure Engine Interface module.
  *          This file provides set of firmware functions to manage SE Interface
  *          functionalities. These services are used by the bootloader.
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
#include "main.h"           /* se_interface_bootloader.c is compiled in SBSFU project using main.h from this project */
#include "se_low_level.h"   /* This file is part of SE_CoreBin and adapts the Secure Engine (and its interface) to the 
                               STM32 board specificities */
#include "se_interface_common.h"
#include "pkcs11.h"
#include "tkms.h"
#include "se_interface_kms.h"
#include "sfu_interface_crypto_scheme.h"
#include <string.h>
#include "kms_platf_objects_interface.h"

/* Private variables ---------------------------------------------------------*/
CK_SESSION_HANDLE session;
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
static uint8_t *gcm_tag_header;
#endif /* SECBOOT_CRYPTO_SCHEME */

/* Private defines -----------------------------------------------------------*/
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
#define GCM_TAGBITS_SIZE          128
#define GCM_IVLEN_BITS            16
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief Secure Engine Encrypt Init function.
  *        It is a wrapper of Encrypt_Init function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pxSE_Metadata Metadata that will be used to fill the Crypto Init structure.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Encrypt_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata, int32_t SE_FwType)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  uint8_t *gcm_iv =  pxSE_Metadata->Nonce;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    GCM_IVLEN_BITS,
    NULL,
    0,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* The bootloader does not need the encrypt service in this crypto scheme: reject this request later on */
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 ||SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256 */

  /* Check the pointers allocation */
  if ((peSE_Status == NULL) || (pxSE_Metadata == NULL))
  {
    return SE_ERROR;
  }

  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Initialize encryption session  */
  rv = C_EncryptInit(session, &cktest_aes_gcm_mechanism, KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256) )
  /* The bootloader does not need the encrypt service in this crypto scheme: reject this request */
  se_status = SE_ERROR;
  *peSE_Status = SE_KO;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}


/**
  * @brief Secure Engine Encrypt Append function.
  *        It is a wrapper of Encrypt_Append function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pInputBuffer pointer to Input Buffer.
  * @param uInputSize Input Size (bytes).
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Encrypt_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                  uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  CK_RV rv = CKR_OK;
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointers allocation */
  if ((pInputBuffer == NULL) || (pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Buffer encryption */
  rv = C_EncryptUpdate(session, (CK_BYTE *)pInputBuffer, InputSize, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* The bootloader does not need this service in this crypto scheme: reject this request */
  se_status = SE_ERROR;
  *peSE_Status = SE_KO;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}


/**
  * @brief Secure Engine Encrypt Finish function.
  *        It is a wrapper of Encrypt_Finish function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Encrypt_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* End-up encryption session */
  rv = C_EncryptFinal(session, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* The bootloader does not need the encrypt service in this crypto scheme: reject this request */
  se_status = SE_ERROR;
  *peSE_Status = SE_KO;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Close the session */
  rv = C_CloseSession(session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;
}

/**
  * @brief Secure Engine Decrypt Init function.
  *        It is a wrapper of Decrypt_Init function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pxSE_Metadata Metadata that will be used to fill the Crypto Init structure.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata, int32_t SE_FwType)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  uint8_t *gcm_iv = pxSE_Metadata->Nonce;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    GCM_IVLEN_BITS,
    NULL,
    0,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

  gcm_tag_header = pxSE_Metadata->FwTag;

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  uint8_t *iv_cbc = pxSE_Metadata->InitVector;

  CK_MECHANISM cktest_aes_cbc_mechanism =
  {
    CKM_AES_CBC,
    iv_cbc,
    SE_IV_LEN
  };
#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)
  /*
   * In this crypto scheme the Firmware is not encrypted.
   * The Decrypt operation is called anyhow before installing the firmware.
   * Indeed, it allows moving the Firmware image blocks in FLASH.
   * These moves are mandatory to create the appropriate mapping in FLASH
   * allowing the swap procedure to run without using the swap area at each and every move.
   *
   * See in SB_SFU project: @ref SFU_IMG_PrepareCandidateImageForInstall.
   */
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointer allocation */
  if (pxSE_Metadata == NULL)
  {
    return SE_ERROR;
  }

  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Initialize the decryption session */
  rv = C_DecryptInit(session, &cktest_aes_gcm_mechanism, KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)
  /* Initialize the decryption session */
  rv = C_DecryptInit(session, &cktest_aes_cbc_mechanism, KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)
  /* Nothing to do as we won't decrypt anything */
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Decrypt Append function.
  *        It is a wrapper of Decrypt_Append function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pInputBuffer pointer to Input Buffer.
  * @param uInputSize Input Size (bytes).
  * @param pOutputBuffe pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                  uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;
#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  CK_RV rv = CKR_OK;
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  /* DecryptImageInSlot1() always starts by calling the Decrypt service with a 0 byte buffer */
  if (0 == InputSize)
  {
    /* Nothing to do but we must return a success for the decrypt operation to continue */
    return (SE_SUCCESS);
  }

  /* Check the pointers allocation */
  if ((pInputBuffer == NULL) || (pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  /* Buffer decryption */
  rv = C_DecryptUpdate(session, (CK_BYTE *)pInputBuffer, InputSize, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)
  /*
   * The firmware is not encrypted.
   * The only thing we need to do is to recopy the input buffer in the output buffer
   */
  (void)memcpy(pOutputBuffer, pInputBuffer, (uint32_t)InputSize);
  *pOutputSize = InputSize;
  se_status = SE_SUCCESS;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  return se_status;
}

/**
  * @brief Secure Engine Decrypt Finish function.
  *        It is a wrapper of Decrypt_Finish function included in the protected area.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_Decrypt_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  *peSE_Status = SE_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  memcpy(pOutputBuffer, gcm_tag_header, 16);

  /* End-up decryption session */
  rv = C_DecryptFinal(session, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)
  /* Nothing to do */
  se_status = SE_SUCCESS;
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256 */

  /* Close the session */
  rv = C_CloseSession(session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Init function.
  *        It is a wrapper of AuthenticateFW_Init function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pxSE_Metadata Metadata that will be used to fill the Crypto Init structure.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Init(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxSE_Metadata,
                                       int32_t SE_FwType)
{
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  uint8_t *gcm_iv =  pxSE_Metadata->Nonce;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    GCM_IVLEN_BITS,
    NULL,
    0,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  CK_MECHANISM cktest_sha_256_mechanism =
  {
    CKM_SHA256,
    NULL,
    0
  };
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointer allocation */
  if (pxSE_Metadata == NULL)
  {
    return SE_ERROR;
  }

  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Initialize encryption session  */
  rv = C_EncryptInit(session, &cktest_aes_gcm_mechanism, KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* Digest initialization */
  rv = C_DigestInit(session, &cktest_sha_256_mechanism);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Append function.
  *        It is a wrapper of AuthenticateFW_Append function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pInputBuffer pointer to Input Buffer.
  * @param uInputSize Input Size (bytes).
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Append(SE_StatusTypeDef *peSE_Status, const uint8_t *pInputBuffer, int32_t InputSize,
                                         uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pInputBuffer == NULL) || (pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Buffer encryption */
  rv = C_EncryptUpdate(session, (CK_BYTE *)pInputBuffer, InputSize, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* Digest calculation */
  rv = C_DigestUpdate(session, (CK_BYTE_PTR)pInputBuffer, InputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  return se_status;
}

/**
  * @brief Secure Engine Firmware Authentication Finish function.
  *        It is a wrapper of AuthenticateFW_Finish function included in the Firewall.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pOutputBuffer pointer to Output Buffer.
  * @param puOutputSize pointer to Output Size (bytes).
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_AuthenticateFW_Finish(SE_StatusTypeDef *peSE_Status, uint8_t *pOutputBuffer, int32_t *pOutputSize)
{
  SE_ErrorStatus se_status = SE_SUCCESS;
  CK_RV rv = CKR_OK;

  /* Check the pointers allocation */
  if ((pOutputBuffer == NULL) || (pOutputSize == NULL))
  {
    return SE_ERROR;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* End-up encryption session */
  rv = C_EncryptFinal(session, pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
  /* End-up Digest calculation */
  rv = C_DigestFinal(session, (CK_BYTE_PTR)pOutputBuffer, (CK_ULONG_PTR)pOutputSize);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Close the session */
  rv = C_CloseSession(session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;
}

/**
  * @brief Secure Engine Verify Raw Fw Header Tag.
  *        It verifies the signature of a raw header file.
  * @param peSE_Status Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param pFwRawHeader pointer to RawHeader Buffer.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SFU_VerifyFwRawHeaderTag(SE_StatusTypeDef *peSE_Status, SE_FwRawHeaderTypeDef *pxFwRawHeader)
{

  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  CK_RV rv = CKR_OK;
  SE_ErrorStatus se_status = SE_SUCCESS;
  *peSE_Status = SE_OK;

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  int32_t fw_raw_header_output_length;
  uint8_t *gcm_iv = pxFwRawHeader->Nonce;
  SE_FwRawHeaderTypeDef *gcm_header = pxFwRawHeader;

  CK_GCM_PARAMS kms_test_gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    SE_NONCE_LEN,
    GCM_IVLEN_BITS,
    (CK_BYTE_PTR)gcm_header,
    ((int32_t)SE_FW_HEADER_TOT_LEN) - SE_TAG_LEN,
    GCM_TAGBITS_SIZE
  };

  CK_MECHANISM cktest_aes_gcm_mechanism =
  {
    CKM_AES_GCM,
    (CK_VOID_PTR *) &kms_test_gcm_params,
    sizeof(kms_test_gcm_params)
  };

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))

  const uint8_t *pPayload;                        /* Metadata payload */
  int32_t payloadSize;                            /* Metadata length to be considered for hash */
  uint8_t *pSignature = pxFwRawHeader->HeaderMAC; /* Signature ECDSA */
  CK_MECHANISM cktest_ecdsa_256_mechanism =
  {
    CKM_ECDSA_SHA256,
    NULL,
    0
  };
#endif /* SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM */

  /* Check the pointers allocation */
  if (NULL == pxFwRawHeader)
  {
    *peSE_Status = SE_KO;
    return SE_ERROR;
  }

#if ((SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256))
  gcm_tag_header = pxFwRawHeader->HeaderMAC;
#endif /* SECBOOT_CRYPTO_SCHEME */

  /* Initialize the KMS service */
  C_Initialize(NULL) ;

  /* Start by opening a session */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  /* Initialize the decryption session */
  rv = C_DecryptInit(session, &cktest_aes_gcm_mechanism, KMS_SBSFU_KEY_AES128_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up decryption session  : control if the tag is correct */
  rv = C_DecryptFinal(session, gcm_tag_header, (CK_ULONG_PTR)&fw_raw_header_output_length);

  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256) || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))

  pPayload = (const uint8_t *)pxFwRawHeader;
  payloadSize = SE_FW_HEADER_TOT_LEN - SE_MAC_LEN;

  /* Initialize the verification process */
  rv = C_VerifyInit(session, &cktest_ecdsa_256_mechanism, KMS_SBSFU_KEY_ECDSA_OBJECT_HANDLE);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up verification process : control if the signature is correct */
  rv = C_Verify(session, (CK_BYTE_PTR)pPayload, payloadSize, (CK_BYTE_PTR)pSignature, SE_MAC_LEN);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_CRYPTO_SCHEME */

  /* Close the session */
  rv = C_CloseSession(session);
  if (rv != CKR_OK)
  {
    se_status = SE_ERROR;
    *peSE_Status = SE_KO;
  }

  /* End-up the KMS service */
  C_Finalize(NULL) ;

  return se_status;

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
