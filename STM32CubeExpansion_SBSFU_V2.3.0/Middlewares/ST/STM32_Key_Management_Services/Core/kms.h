/**
  ******************************************************************************
  * @file    kms.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module functionalities.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_H
#define KMS_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "pkcs11.h"
#include "kms_config.h"
#include "kms_blob_headers.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_OBJECTS Blob Objects Management
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @addtogroup KMS_OBJECTS_Exported_Types Exported Types
  * @{
  */
/**
  * @brief Blob import finalized
  */
#define KMS_IMPORT_BLOB_FINALIZED                (CKR_VENDOR_DEFINED+0UL)
/**
  * @brief Blob import failed: authentication error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR   (CKR_VENDOR_DEFINED+1UL)
/**
  * @brief Blob import failed: verification error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_VERIF_ERROR  (CKR_VENDOR_DEFINED+2UL)
/**
  * @brief Blob import failed: form error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_FORM_ERROR   (CKR_VENDOR_DEFINED+3UL)
/**
  * @brief Blob import failed: object  ID error
  */
#define KMS_IMPORT_BLOB_OBJECTID_ERROR           (CKR_VENDOR_DEFINED+4UL)
/**
  * @brief Blob import failed: NVM error
  */
#define KMS_IMPORT_BLOB_NVM_ERROR                (CKR_VENDOR_DEFINED+5UL)
/**
  * @brief Blob import finalized with NVM warning
  */
#define KMS_IMPORT_BLOB_NVM_WARNING              (CKR_VENDOR_DEFINED+6UL)
/**
  * @brief Blob import failed: session in use
  */
#define KMS_IMPORT_BLOB_SESSION_INUSE            (CKR_VENDOR_DEFINED+7UL)
/**
  * @brief Blob import intermediate status: no error
  */
#define KMS_IMPORT_BLOB_NO_ERROR                 (CKR_VENDOR_DEFINED+8UL)

/**
  * @brief KMS blob import returned state type definition
  */
typedef uint32_t kms_import_blob_state_t;
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup KMS_INIT Initialization And Session handling
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Constants Exported Constants
  * @{
  */
#define KMS_HANDLE_KEY_NOT_KNOWN        0xFFFFFFFFU     /*!< Unknow KMS key handle value */
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Types Exported Types
  * @{
  */
/**
  * @brief KMS session management structure definition
  */
typedef struct
{
  CK_SLOT_ID    slotID;                 /*!< Session Slot ID */
  CK_STATE      state;                  /*!< Session state */
  CK_FLAGS      flags;                  /*!< Session flags */
  CK_ULONG      ulDeviceError;          /*!< Session device error */
  CK_VOID_PTR   pApplication;           /*!< Session application-defined pointer */
  CK_NOTIFY     Notify;                 /*!< Session notification callback */
  CK_MECHANISM_TYPE   Mechanism;        /*!< Session mechanism */
  CK_OBJECT_HANDLE hKey;                /*!< Session key handle */
  CK_VOID_PTR   pKeyAllocBuffer;        /*!< Session key allocated buffer */
#ifdef KMS_EXT_TOKEN_ENABLED
  CK_SESSION_HANDLE hSession_ExtToken;  /*!< Session external token associated session handle */
#endif /* KMS_EXT_TOKEN_ENABLED */
} kms_session_desc_t;


/**
  * @brief Item reference in a serial blob
  */
typedef struct
{
  uint32_t id;        /*!< Item ID */
  uint32_t size;      /*!< Item Size */
  uint32_t data[1];   /*!< Item data */
} kms_ref_t;

/**
  * @}
  */


/* Exported variables --------------------------------------------------------*/

/** @addtogroup KMS_INIT_Exported_Variables Exported Variables
  * @{
  */
extern kms_session_desc_t    KMS_SessionList[KMS_NB_SESSIONS_MAX];
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Macros Exported Macros
  * @{
  */
/**
  * @brief KMS Session description assessor from external handle
  * @note  KMS session externally accessed with session handle starting from 1
  *        but internally, sessions are stored into a table indexed from 0
  */
#define KMS_GETSESSSION(HANDLE)  (KMS_SessionList[(HANDLE)-1UL])
/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Functions Exported Functions
  * @{
  */

/* kms_init */
#ifdef KMS_EXT_TOKEN_ENABLED
CK_RV  KMS_CallbackFunction_ForExtToken(CK_SESSION_HANDLE hSession,
                                        CK_NOTIFICATION event,
                                        CK_VOID_PTR pApplication);
CK_RV  KMS_OpenSession_RegisterExtToken(CK_SESSION_HANDLE hSession,
                                        CK_SESSION_HANDLE hSession_ExtToken);
#endif /* KMS_EXT_TOKEN_ENABLED */

CK_RV  KMS_Check_Session_Handle(CK_SESSION_HANDLE hSession) ;

#if defined(KMS_UNITARY_TEST)
CK_RV  KMS_UnitaryTest_GetAttribute(CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed);
CK_RV  KMS_UnitaryTest(CK_ULONG ulTestID, CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed);
#endif /* KMS_UNITARY_TEST */


/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */
CK_RV  KMS_Initialize(CK_VOID_PTR pInitArgs) ;
CK_RV  KMS_Finalize(CK_VOID_PTR pReserved);
CK_RV  KMS_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo);
CK_RV  KMS_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags,
                       CK_VOID_PTR pApplication, CK_NOTIFY Notify,
                       CK_SESSION_HANDLE_PTR phSession);
CK_RV  KMS_CloseSession(CK_SESSION_HANDLE hSession);



/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
