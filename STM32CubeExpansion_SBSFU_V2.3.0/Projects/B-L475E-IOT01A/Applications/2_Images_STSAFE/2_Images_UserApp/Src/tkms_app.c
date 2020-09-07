/**
  ******************************************************************************
  * @file    tkms_app.c
  * @author  MCD Application Team
  * @brief   tKMS application examples module.
  *          This file provides examples of KMS API usages.
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


/* Private variables ---------------------------------------------------------*/
uint8_t clear_buffer[128] = "STM32 Key Management Services - Example buffer";

extern void KMS_Start_LocalUpdate(void);

static void tkms_app_print_menu(void)
{
  printf("\r\n======================= tKMS Examples Menu ===========================\r\n\n");
  printf("  TKMS - Test All (consider Import blob done)       --------------------- 0\r\n\n");
  printf("  TKMS - Tests AES-GCM Static key                   --------------------- 1\r\n\n");
  printf("  TKMS - Tests AES-CBC Static key                   --------------------- 2\r\n\n");
  printf("  TKMS - Import blob                                --------------------- 3\r\n\n");
  printf("  TKMS - Tests RSA Static key                       --------------------- 4\r\n\n");
  printf("  TKMS - Tests Derive key                           --------------------- 5\r\n\n");
  printf("  TKMS - Tests Ext. Token Sign Verify               --------------------- a\r\n\n");
  printf("  TKMS - Tests Ext. Token Key Gen + Diffie-Hellman  --------------------- b\r\n\n");
  printf("  Exit tKMS Examples Menu                           --------------------- x\r\n\n");
}

void tkms_app_menu(void)
{
  uint8_t key = 0;
  uint8_t exit = 0;
  CK_RV ret_status;
  uint8_t tests_executed;
  uint8_t tests_success;

  /* Initialize tKMS for subsequent usage */
  ret_status = C_Initialize(NULL);
  if (ret_status != CKR_OK)
  {
    printf("tKMS initialization failed\r\n");
    exit = 1;
  }
  else
  {
    tkms_app_print_menu();
  }

  while (exit == 0U)
  {
    key = 0U;

    INVOKE_SCHEDULE_NEEDS();

    /* Clean the user input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '0' :
          tests_executed = 0;
          tests_success = 0;
          ret_status = tkms_app_encrypt_decrypt_aes_gcm(sizeof(clear_buffer), clear_buffer);
          printf("AES GCM test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_encrypt_decrypt_aes_cbc(sizeof(clear_buffer), clear_buffer);
          printf("AES CBC test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_sign_verify_rsa(sizeof(clear_buffer), clear_buffer);
          printf("RSA test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_derive_key(sizeof(clear_buffer), clear_buffer);
          printf("Derive key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_ext_token_sign_verify_ecdsa();
          printf("Ext Token test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          ret_status = tkms_app_ext_token_generate_derive_key();
          printf("Ext Token test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tests_executed++;
          tests_success += (ret_status == CKR_OK) ? 1 : 0;
          INVOKE_SCHEDULE_NEEDS();
          if (tests_success == tests_executed)
          {
            printf("CUMULATIVE RESULT: ALL TESTS PASSED\r\n");
          }
          else
          {
            printf("CUMULATIVE RESULT: %d/%d success\r\n", tests_success, tests_executed);
          }
          tkms_app_print_menu();
          break;

        /* 1 = TKMS - Tests AES-GCM Static key */
        case '1' :
          ret_status = tkms_app_encrypt_decrypt_aes_gcm(sizeof(clear_buffer), clear_buffer);
          printf("AES GCM test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 2 = Tests AES-CBC Static key */
        case '2' :
          ret_status = tkms_app_encrypt_decrypt_aes_cbc(sizeof(clear_buffer), clear_buffer);
          printf("AES CBC test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 3 = Import Blob */
        case '3':
          KMS_Start_LocalUpdate();
          tkms_app_print_menu();
          break;
        /* 4 = Tests RSA Static key (requires blob import) */
        case '4' :
          ret_status = tkms_app_sign_verify_rsa(sizeof(clear_buffer), clear_buffer);
          printf("RSA test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* 5 = Tests Key derivation (requires blob import) */
        case '5':
          ret_status = tkms_app_derive_key(sizeof(clear_buffer), clear_buffer);
          printf("Derive key test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* a = Tests Ext Token - sign verify ECDSA*/
        case 'a' :
          ret_status = tkms_app_ext_token_sign_verify_ecdsa();
          printf("Ext Token test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        /* b = Tests Ext Token - key generation + Diffie-Hellman key derivation*/
        case 'b' :
          ret_status = tkms_app_ext_token_generate_derive_key();
          printf("Ext Token test %s\r\n", (ret_status == CKR_OK) ? "SUCCESSFULL" : "FAILED");
          tkms_app_print_menu();
          break;
        case 'x':
          exit = 1;
          break;

        default:
          printf("Invalid Number !\r");
          tkms_app_print_menu();
          break;
      }
    }
  }

  /* Finalize tKMS since no more in use */
  ret_status = C_Finalize(NULL);
  if (ret_status != CKR_OK)
  {
    printf("tKMS finalization failed\r\n");
  }
}

void tkms_buff2str(uint8_t *pbuff, uint8_t *pstr, uint32_t length)
{
  uint8_t ascii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  for (uint32_t i = 0; i < length; i += 1)
  {
    *pstr = ascii[*pbuff >> 4];
    pstr++;
    *pstr = ascii[*pbuff & 0x0FUL];
    pstr++;
    pbuff++;
  }
  *pstr = 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
