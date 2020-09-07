/**
  ******************************************************************************
  * @file    se_crypto_config.h
  * @author  MCD Application Team
  *          This file provides a template to configure the crypto scheme used
  *          for the services used by the bootloader (crypto operations for the
  *          Firmware and Metadata).
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SE_CRYPTO_CONFIG_H
#define SE_CRYPTO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @addtogroup SE_CRYPTO SE Crypto
  * @{
  */

/** @defgroup SE_CRYPTO_CONFIG SE Crypto scheme configuration
  * @brief Choose the cryptographic scheme you want the bootloader to use.
  *        These settings apply to the services used by the bootloader only, not to the services called by the User
  *        Application.
  * @{
  */

/**
  * SE_CoreBin instantiates the crypto scheme selected thanks to "SECBOOT_CRYPTO_SCHEME".
  * "SECBOOT_CRYPTO_SCHEME" is composed as indicated below:
  *        SECBOOT_authenticationalgo_encryptionalgo_MACalgo
  *
  * Due to FLASH size constraint, "SECBOOT_CRYPTO_SCHEME" is limited to the following value :
  *
  * SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM: Authentication, Integrity, Confidentiality are ensured.
  *                           Authenticate Firmware Metadata using AES128-GCM tag.
  *                           Decrypt Firmware image using AES128-GCM algo.
  *                           Firmware Image Integrity using AES128-GCM tag.
  */
#define SECBOOT_CRYPTO_SCHEME SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM /*!< Selected Crypto Scheme */

#define SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM     (3U) /*!< symmetric crypto                              */

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

#ifdef __cplusplus
}
#endif

#endif /* SE_CRYPTO_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

