/**
  ******************************************************************************
  * @file    tkms_app_encrypt_decrypt.c
  * @author  MCD Application Team
  * @brief   KMS application examples module.
  *          This file provides examples of KMS API usage to encrypt & decrypt
  *          messages.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "kms.h"
#include "tkms.h"
#include "se_interface_kms.h"
#include "tkms_app.h"
#include "com.h"
#include "common.h"
#include "kms_platf_objects_interface.h"

/* Private defines ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_string[1024] = {0};
static uint8_t encrypted_message[256] __attribute__((aligned(8))) = {0};
static uint8_t decrypted_message[256] __attribute__((aligned(8))) = {0};

/*
 * AES GCM 128 Bits
 * -----------------------------------------------------------------------------
 * This example is showing how to use AES GCM encryption/decryption to
 * authenticate and protect a message.
 * -----------------------------------------------------------------------------
 * Shared information between emitter and receiver
 * - Secret encryption/decryption Key: stored in KMS under
 *                                     KMS_KEY_AES128_OBJECT_HANDLE object handle
 * - Encrypted message: transmitted from emitter to sender
 * - Initialization Vector: transmitted from emitter to sender
 * - Message Header: transmitted from emitter to sender
 * - Authentication Tag: transmitted from emitter to sender
 *                      (calculated during encryption, verified during decryption)
 */
CK_RV tkms_app_encrypt_decrypt_aes_gcm(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t encrypted_length = 0;
  uint32_t decrypted_length = 0;
  uint8_t tag[16] = {0};  /* 128 bits tag size */
  uint32_t tag_lenth = 0;

  /* AES GCM Configuration variables */
  uint8_t gcm_iv[12] = ">GCM VECTOR";
  uint8_t gcm_header[16] = ">AES GCM Header";
  CK_GCM_PARAMS gcm_params =
  {
    (CK_BYTE *)gcm_iv,
    sizeof(gcm_iv),
    16,
    (CK_BYTE_PTR)gcm_header,
    sizeof(gcm_header),
    128   /* Tag length in Bits */
  };
  CK_MECHANISM aes_gcm_mechanism = { CKM_AES_GCM, (CK_VOID_PTR *) &gcm_params, sizeof(gcm_params) };

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Encryption --------------------------------------------------------------*/
  printf("--- Encrypting --------------------------------------------------\r\n");
  printf("--- AES GCM\r\n");
  printf("--- IV      [%s]\r\n", gcm_iv);
  printf("--- Header  [%s]\r\n", gcm_header);
  printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  printf("---         [0x%s]\r\n", a_string);
  printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_gcm_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES128_OBJECT_HANDLE);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    rv = C_EncryptUpdate(session, (CK_BYTE_PTR)pClearMessage, length,
                         encrypted_message, (CK_ULONG_PTR)&encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    rv = C_EncryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }

  printf("--- Encrypted ---------------------------------------------------\r\n");
  tkms_buff2str(encrypted_message, a_string, encrypted_length);
  printf("--- Message [0x%s]\r\n", a_string);
  tkms_buff2str(tag, a_string, tag_lenth);
  printf("--- Tag     [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_gcm_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES128_OBJECT_HANDLE);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    rv = C_DecryptUpdate(session, (CK_BYTE_PTR)encrypted_message, encrypted_length,
                         decrypted_message, (CK_ULONG_PTR)&decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    rv = C_DecryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }
  printf("--- Decrypted ---------------------------------------------------\r\n");
  printf("--- Message [%s]\r\n", decrypted_message);
  tkms_buff2str(decrypted_message, a_string, decrypted_length);
  printf("---         [0x%s]\r\n", a_string);

  if (rv == CKR_OK)
  {
    printf(">>> Decrypted message authenticated\r\n");
  }
  else
  {
    printf("XXX Decrypted message not authenticated\r\n");
  }

  /* Compare decrypted message with clear one */
  if (rv == CKR_OK)
  {
    if (memcmp(pClearMessage, decrypted_message, length))
    {
      printf("XXX Decrypted message differs\r\n");
      rv = CKR_FUNCTION_FAILED;
    }
    else
    {
      printf(">>> Decrypted message is same\r\n");
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/*
 * AES CBC 256 Bits
 * -----------------------------------------------------------------------------
 * This example is showing how to use AES CBC encryption/decryption to
 * protect a message.
 * -----------------------------------------------------------------------------
 * CBC shared information between emitter and receiver
 * - Secret encryption/decryption Key: stored in KMS under
 *                                     KMS_KEY_AESC256_OBJECT_HANDLE object handle
 * - Encrypted message: transmitted from emitter to sender
 * - Initialization Vector: transmitted from emitter to sender
 */
CK_RV tkms_app_encrypt_decrypt_aes_cbc(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  uint32_t encrypted_length = 0;
  uint32_t decrypted_length = 0;
  uint8_t tag[16] = {0};
  uint32_t tag_lenth = 0;

  /* AES CBC Configuration variables */
  char cbc_iv[12] = ">CBC VECTOR";
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)cbc_iv, sizeof(cbc_iv) };

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Encryption --------------------------------------------------------------*/
  printf("--- Encrypting --------------------------------------------------\r\n");
  printf("--- AES CBC\r\n");
  printf("--- IV      [%s]\r\n", cbc_iv);
  printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  printf("---         [0x%s]\r\n", a_string);
  printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Encrypt clear message */
  if (rv == CKR_OK)
  {
    rv = C_EncryptUpdate(session, (CK_BYTE_PTR)pClearMessage, length,
                         encrypted_message, (CK_ULONG_PTR)&encrypted_length);
  }

  /* Finalize message encryption */
  if (rv == CKR_OK)
  {
    rv = C_EncryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }

  printf("--- Encrypted ---------------------------------------------------\r\n");
  tkms_buff2str(encrypted_message, a_string, encrypted_length);
  printf("--- Message [0x%s]\r\n", a_string);

  /* Decryption --------------------------------------------------------------*/

  /* Configure session to decrypt message in AES CBC with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_cbc_mechanism, (CK_OBJECT_HANDLE)KMS_KEY_AES256_OBJECT_HANDLE);
  }

  /* Decrypt encrypted message */
  if (rv == CKR_OK)
  {
    rv = C_DecryptUpdate(session, (CK_BYTE_PTR)encrypted_message, encrypted_length,
                         decrypted_message, (CK_ULONG_PTR)&decrypted_length);
  }

  /* Finalize message decryption */
  if (rv == CKR_OK)
  {
    rv = C_DecryptFinal(session, &tag[0], (CK_ULONG_PTR)&tag_lenth);
  }
  printf("--- Decrypted ---------------------------------------------------\r\n");
  printf("--- Message [%s]\r\n", decrypted_message);
  tkms_buff2str(decrypted_message, a_string, decrypted_length);
  printf("---         [0x%s]\r\n", a_string);

  /* Compare decrypted message with clear one */
  if (rv == CKR_OK)
  {
    if (memcmp(pClearMessage, decrypted_message, length))
    {
      printf("XXX Decrypted message differs\r\n");
      rv = CKR_FUNCTION_FAILED;
    }
    else
    {
      printf(">>> Decrypted message is same\r\n");
    }
  }

  /* Close session with KMS */
  (void)C_CloseSession(session);

  return rv;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
