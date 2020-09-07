/**
  ******************************************************************************
  * @file    tkms_app_derive_key.c
  * @author  MCD Application Team
  * @brief   tKMS application examples module.
  *          This file provides examples of KMS API usage to derivate a key
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

/* Private defines -----------------------------------------------------------*/

/* Private structures -- -----------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t a_string[1024] = {0};
static uint8_t encrypted_message[256] __attribute__((aligned(8))) = {0};
static uint8_t decrypted_message[256] __attribute__((aligned(8))) = {0};

static CK_ULONG DeriveKey_template_class = CKO_SECRET_KEY;
static CK_ULONG DeriveKey_template_destroyable = CK_TRUE;
static CK_ULONG DeriveKey_template_encrypt = CK_TRUE;
static CK_ULONG DeriveKey_template_decrypt = CK_TRUE;
static CK_ULONG DeriveKey_template_extract = CK_TRUE;
static CK_ATTRIBUTE DeriveKey_template[] =
{
  {CKA_CLASS, (CK_VOID_PTR) &DeriveKey_template_class,       sizeof(CK_ULONG)},
  {CKA_DESTROYABLE, (CK_VOID_PTR) &DeriveKey_template_destroyable, sizeof(CK_ULONG)},
  {CKA_ENCRYPT, (CK_VOID_PTR) &DeriveKey_template_encrypt,     sizeof(CK_ULONG)},
  {CKA_DECRYPT, (CK_VOID_PTR) &DeriveKey_template_decrypt,     sizeof(CK_ULONG)},
  {CKA_EXTRACTABLE, (CK_VOID_PTR) &DeriveKey_template_extract,     sizeof(CK_ULONG)}
};

static CK_ATTRIBUTE GetKey_template[] = {{CKA_VALUE, (CK_VOID_PTR) &DeriveKey_template_class, 32}};


/*
 * Key derivation
 * -----------------------------------------------------------------------------
 * This example is showing how to use key derivation to create
 * a specific session key.
 * -----------------------------------------------------------------------------
 * Key derivation shared information between emitter and receiver
 * - Secret derivation Key: stored in KMS under
 *                         KMS_KEY_AES128_DERIVABLE_OBJECT_HANDLE object handle
 * - Session key variation: transmitted or calculated by both emitter & receiver
 */
CK_RV tkms_app_derive_key(uint32_t length, uint8_t *pClearMessage)
{
  CK_RV rv;
  CK_SESSION_HANDLE session;
  CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
  /* Key derivation */
  uint8_t session_variant[32] = "My session variation 0122004578";
  CK_MECHANISM      mech = {CKM_AES_ECB_ENCRYPT_DATA, session_variant, 32};
  CK_OBJECT_HANDLE  derivedKeyHdle;
  uint8_t derivedKey[32] = {0};
  /* AES encryption / decryption */
  uint32_t encrypted_length = 0;
  uint32_t decrypted_length = 0;
  char cbc_iv[12] = ">CBC VECTOR";
  CK_MECHANISM aes_cbc_mechanism = { CKM_AES_CBC, (CK_VOID_PTR)cbc_iv, sizeof(cbc_iv) };
  uint8_t tag[16] = {0};
  uint32_t tag_lenth = 0;

  /* Open session with KMS */
  rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

  /* Derive key with pass phrase */
  if (rv == CKR_OK)
  {
    rv = C_DeriveKey(session, &(mech), (CK_OBJECT_HANDLE)KMS_KEY_AES128_DERIVABLE_OBJECT_HANDLE,
                     &DeriveKey_template[0], sizeof(DeriveKey_template) / sizeof(CK_ATTRIBUTE), &derivedKeyHdle);
  }

  /* Get derived key to display */
  if (rv == CKR_OK)
  {
    GetKey_template[0].pValue = derivedKey;
    rv = C_GetAttributeValue(session, derivedKeyHdle, &(GetKey_template[0]), sizeof(GetKey_template) /
                             sizeof(CK_ATTRIBUTE));
  }
  if (rv == CKR_OK)
  {
    printf("--- Derivating key --------------------------------------------\r\n");
    printf("--- AES ECB\r\n");
    printf("--- Pass phrase   [%s]\r\n", session_variant);
    tkms_buff2str(derivedKey, a_string, 32);
    printf("--- Derived key   [%s]\r\n", a_string);
  }

  /* Encryption --------------------------------------------------------------*/
  printf("--- Encrypting --------------------------------------------------\r\n");
  printf("--- AES CBC\r\n");
  printf("--- IV      [%s]\r\n", cbc_iv);
  printf("--- Message [%s]\r\n", pClearMessage);
  tkms_buff2str(pClearMessage, a_string, length);
  printf("---         [0x%s]\r\n", a_string);
  printf("--- Length  [%d]\r\n", length);

  /* Configure session to encrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_EncryptInit(session, &aes_cbc_mechanism, derivedKeyHdle);
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

  /* Configure session to decrypt message in AES GCM with settings included into the mechanism */
  if (rv == CKR_OK)
  {
    rv = C_DecryptInit(session, &aes_cbc_mechanism, derivedKeyHdle);
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

  /* Remove key once no more needed */
  if (rv == CKR_OK)
  {
    rv = C_DestroyObject(session, derivedKeyHdle);
  }

  /* Verify key is no more usable */
  if (rv == CKR_OK)
  {
    if (C_EncryptInit(session, &aes_cbc_mechanism, derivedKeyHdle) == CKR_OK)
    {
      rv = CKR_FUNCTION_FAILED;
    }
  }

  /* Close sessions */
  (void)C_CloseSession(session);

  return rv;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
