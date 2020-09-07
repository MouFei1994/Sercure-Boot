/**
  ******************************************************************************
  * @file    loader.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update local
  *          loader.
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
#ifndef LOADER_H
#define LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  LOADER_OK                            = 0x00U,
  LOADER_ERR                           = 0x01U,
  LOADER_ERR_COM                       = 0x02U,
  LOADER_ERR_CMD_AUTH_FAILED           = 0x03U,
  LOADER_ERR_FW_LENGTH                 = 0x04U,
  LOADER_ERR_OLD_FW_VERSION            = 0x05U,
  LOADER_ERR_DOWNLOAD                  = 0x06U,
  LOADER_ERR_FLASH_ACCESS              = 0x07U,
  LOADER_ERR_ABORT                     = 0x08U
} LOADER_StatusTypeDef;

/* Exported constants --------------------------------------------------------*/

/* YMODEM protocol definitions */
#define YMODEM_PACKET_HEADER_SIZE      ((uint32_t)3U)    /*!<Header Size*/
#define YMODEM_PACKET_DATA_INDEX       ((uint32_t)4U)    /*!<Data Index*/
#define YMODEM_PACKET_START_INDEX      ((uint32_t)1U)    /*!<Start Index*/
#define YMODEM_PACKET_NUMBER_INDEX     ((uint32_t)2U)    /*!<Packet Number Index*/
#define YMODEM_PACKET_CNUMBER_INDEX    ((uint32_t)3U)    /*!<Cnumber Index*/
#define YMODEM_PACKET_TRAILER_SIZE     ((uint32_t)2U)    /*!<Trailer Size*/
#define YMODEM_PACKET_OVERHEAD_SIZE    (YMODEM_PACKET_HEADER_SIZE + YMODEM_PACKET_TRAILER_SIZE - 1U) /*!<Overhead Size*/
#define YMODEM_PACKET_SIZE             ((uint32_t)128U)  /*!<Packet Size*/
#define YMODEM_PACKET_1K_SIZE          ((uint32_t)1024U) /*!<Packet 1K Size*/

#define YMODEM_FILE_NAME_LENGTH        ((uint32_t)64U)   /*!< File name length*/
#define YMODEM_FILE_SIZE_LENGTH        ((uint32_t)16U)   /*!< File size length*/

#define YMODEM_SOH                     ((uint8_t)0x01U)  /*!< Start of 128-byte data packet */
#define YMODEM_STX                     ((uint8_t)0x02U)  /*!< Start of 1024-byte data packet */
#define YMODEM_EOT                     ((uint8_t)0x04U)  /*!< End of transmission */
#define YMODEM_ACK                     ((uint8_t)0x06U)  /*!< Acknowledge */
#define YMODEM_NAK                     ((uint8_t)0x15U)  /*!< Negative acknowledge */
#define YMODEM_CA                      ((uint8_t)0x18U)  /*!< Two of these in succession aborts transfer */
#define YMODEM_CRC16                   ((uint8_t)0x43U)  /*!< 'C' == 0x43, request 16-bit CRC */
#define YMODEM_RB                      ((uint8_t)0x72U)  /*!< Startup sequence */
#define YMODEM_NEGATIVE_BYTE           ((uint8_t)0xFFU)  /*!< Negative Byte*/

#define YMODEM_ABORT1                  ((uint8_t)0x41U)  /*!< 'A' == 0x41, abort by user */
#define YMODEM_ABORT2                  ((uint8_t)0x61U)  /*!< 'a' == 0x61, abort by user */

#define YMODEM_NAK_TIMEOUT             ((uint32_t)0x100000U)  /*!< NAK Timeout*/
#define YMODEM_DOWNLOAD_TIMEOUT        ((uint32_t)3000U) /*!< Retry delay, has to be smaller than IWDG */
#define YMODEM_MAX_ERRORS              ((uint32_t)3U)    /*!< Maximum number of retry*/


/* Comunication with SBSFU : same defintion in SBSFU part */
#define STANDALONE_LOADER_NO_REQ         0x00000000U     /*!< No request */
#define STANDALONE_LOADER_BYPASS_REQ     0xABCDEF01U     /*!< By pass mode request */
#define STANDALONE_LOADER_INSTALL_REQ    0x01234567U     /*!< User App installation request */

/* Exported functions ------------------------------------------------------- */
LOADER_StatusTypeDef LOADER_Init(void);
LOADER_StatusTypeDef LOADER_DeInit(void);
LOADER_StatusTypeDef LOADER_DownloadNewUserFw(uint32_t *pRxSize);
LOADER_StatusTypeDef LOADER_InstallAtNextReset(uint8_t *pBuffer);


#ifdef __cplusplus
}
#endif

#endif /* LOADER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

