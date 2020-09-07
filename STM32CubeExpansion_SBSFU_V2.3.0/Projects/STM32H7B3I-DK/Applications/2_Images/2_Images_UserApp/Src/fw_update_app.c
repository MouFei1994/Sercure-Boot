/**
  ******************************************************************************
  * @file    fw_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage Firmware
  *          Update functionalities.
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
#include "se_def.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"
#include "stm32h7xx_hal.h"
#include "stm32h7b3i_discovery.h"
#include "fw_update_app.h"
#include "sfu_fwimg_regions.h"
#include "sfu_app_new_image.h"
#include "string.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */

/* Private defines -----------------------------------------------------------*/

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  FW_UPDATE Firmware Update Example
  * @{
  */
#if !defined(EXTERNAL_LOADER)
/** @defgroup  FW_UPDATE_Private_Variables Private Variables
  * @{
  */
static uint32_t m_uFileSizeYmodem = 0U;  /* !< Ymodem File size*/
static uint32_t m_uNbrBlocksYmodem = 0U; /* !< Ymodem Number of blocks*/
static uint32_t m_uPacketsReceived = 0U; /* !< Ymodem packets received*/

/**
  * @}
  */

/** @defgroup  FW_UPDATE_Private_Functions Private Functions
  * @{
  */
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea);
#endif /* !EXTERNAL_LOADER */

/**
  * @}
  */
/** @defgroup  FW_UPDATE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  FW_UPDATE_Control_Functions Control Functions
  * @{
   */

/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
#if defined(EXTERNAL_LOADER)
HAL_StatusTypeDef FW_UPDATE_Run(void)
{
  /* Print Firmware Update welcome message */
  printf("\r\n================ New Fw Download =========================\r\n\n");

  /* Standalone loader communication : execution requested */
  (*(uint32_t *)LOADER_COM_REGION_RAM_START) = STANDALONE_LOADER_BYPASS_REQ;

  NVIC_SystemReset();

  return HAL_OK;
}

#else
HAL_StatusTypeDef FW_UPDATE_Run(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  uint8_t  fw_header_slot1[SE_FW_HEADER_TOT_LEN];
  SFU_FwImageFlashTypeDef fw_image_dwl_area;

  /* Print Firmware Update welcome message */
  printf("\r\n================ New Fw Download =========================\r\n\n");

  /* Get Info about the download area */
  if (SFU_APP_GetDownloadAreaInfo(&fw_image_dwl_area) != HAL_ERROR)
  {
    /* Download new firmware image*/
    ret = FW_UPDATE_DownloadNewFirmware(&fw_image_dwl_area);

    if (HAL_OK == ret)
    {
      /* Read header in slot 1 */
      ret = FLASH_If_Read(fw_header_slot1, (void *) fw_image_dwl_area.DownloadAddr, SE_FW_HEADER_TOT_LEN);

      /* Ask for installation at next reset */
      (void)SFU_APP_InstallAtNextReset((uint8_t *) fw_header_slot1);

      /* System Reboot*/
      printf("  -- Image correctly downloaded - reboot\r\n\n");
      HAL_Delay(1000U);
      NVIC_SystemReset();
    }
  }

  if (ret != HAL_OK)
  {
    printf("  -- !!Operation failed!! \r\n\n");
  }
  return ret;
}
#endif /* EXTERNAL_LOADER */


/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  FW_UPDATE_Private_Functions
  * @{
  */

#if !defined(EXTERNAL_LOADER)
/**
  * @brief Download a new Firmware from the host.
  * @retval HAL status
  */
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  COM_StatusTypeDef e_result;
  uint32_t u_fw_size;
  YMODEM_CallbacksTypeDef ymodemCb = {Ymodem_HeaderPktRxCpltCallback, Ymodem_DataPktRxCpltCallback};

  printf("  -- Send Firmware \r\n\n");
  /* Refresh Watchdog */
  WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);

  /* Clear download area */
  printf("  -- -- Erasing download area ...\r\n\n");
  if ((ret = FLASH_If_Erase_Size((void *)(pFwImageDwlArea->DownloadAddr), pFwImageDwlArea->MaxSizeInBytes)) == HAL_OK)
  {
    /* Download binary */
    printf("  -- -- File> Transfer> YMODEM> Send ");

    /*Init of Ymodem*/
    Ymodem_Init();

    /*Receive through Ymodem*/
    e_result = Ymodem_Receive(&u_fw_size, pFwImageDwlArea->DownloadAddr, &ymodemCb);
    printf("\r\n\n");

    if ((e_result == COM_OK))
    {
      printf("  -- -- Programming Completed Successfully!\r\n\n");
      printf("  -- -- Bytes: %d\r\n\n", u_fw_size);
      ret = HAL_OK;
    }
    else if (e_result == COM_ABORT)
    {
      printf("  -- -- !!Aborted by user!!\r\n\n");
      ret = HAL_ERROR;
    }
    else
    {
      printf("  -- -- !!Error during file download!!\r\n\n");
      ret = HAL_ERROR;
    }
  }
  return ret;
}

/**
  * @}
  */

/** @defgroup FW_UPDATE_Callback_Functions Callback Functions
  * @{
  */

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize Dimension of the file that will be received (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_HeaderPktRxCpltCallback(uint32_t uFileSize)
{
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /* compute the number of 1K blocks */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (PACKET_1K_SIZE - 1U)) / PACKET_1K_SIZE;

  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000U);

  return HAL_OK;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData Pointer to the buffer.
  * @param  uSize Packet dimension (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_DataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize)
{
  /* Start address of downloaded area : initialized with first packet (header) */
  static uint32_t m_uDwlImgStart = 0U;

  /* End address of downloaded Image : initialized with first packet (header) and checked along download process */
  static uint32_t m_uDwlImgEnd = 0U;

  /* Current destination address for data packet : initialized with first packet (header), incremented at each
     flash write */
  static uint32_t m_uDwlImgCurrent = 0U;

  HAL_StatusTypeDef e_ret_status = HAL_OK;
  uint32_t uLength;
  uint32_t uOldSize;
  SE_FwRawHeaderTypeDef fw_header_dwl;

  m_uPacketsReceived++;

  /*Increase the number of received packets*/
  if (m_uPacketsReceived == m_uNbrBlocksYmodem) /*Last Packet*/
  {
    /*Extracting actual payload from last packet*/
    if (0 == (m_uFileSizeYmodem % PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      uSize = PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      uSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / PACKET_1K_SIZE) * PACKET_1K_SIZE);
    }
  }

  /* First packet : Contains header information: PartialFwSize and PartialFwOffset information */
  if (m_uPacketsReceived == 1)
  {
    /* Start of Download area */
    m_uDwlImgStart = uFlashDestination;

    /* Initialize Current destination address for data packet */
    m_uDwlImgCurrent = uFlashDestination;

    /* End of Image to be downloaded */
    m_uDwlImgEnd = uFlashDestination + ((SE_FwRawHeaderTypeDef *)pData)->PartialFwSize
                   + (((SE_FwRawHeaderTypeDef *)pData)->PartialFwOffset % SFU_IMG_SWAP_REGION_SIZE)
                   + SFU_IMG_IMAGE_OFFSET;
  }

  /* This packet : contains end of FW header */
  if ((m_uDwlImgCurrent < (m_uDwlImgStart + SFU_IMG_IMAGE_OFFSET)) &&
      ((m_uDwlImgCurrent + uSize) >= (m_uDwlImgStart + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Write the FW header part (SFU_IMG_IMAGE_OFFSET % PACKET_1K_SIZE bytes length) */
    uLength = SFU_IMG_IMAGE_OFFSET % PACKET_1K_SIZE;
    if (uLength == 0)
    {
      uLength = PACKET_1K_SIZE;
    }
    if (FLASH_If_Write((void *)m_uDwlImgCurrent, pData, uLength) == HAL_OK)
    {
      /*
       * Read header from dwl area : in some configuration header can be transmitted with 2 YMODEM packets
       * ==> pData contains only the last part of the header and cannot be used to retrieve PartialFwOffset
       */
      e_ret_status = FLASH_If_Read((uint8_t *)&fw_header_dwl, (void *) m_uDwlImgStart, SE_FW_HEADER_TOT_LEN);

      /* Shift the DWL area pointer, to align image with (PartialFwOffset % sector size) in DWL area */
      m_uDwlImgCurrent += uLength + fw_header_dwl.PartialFwOffset % SFU_IMG_SWAP_REGION_SIZE;

      /* Update remaining packet size to write */
      uSize -= uLength;

      /* Update pData pointer to received packet data */
      pData += uLength;
    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }

  /*Adjust dimension to unitary flash programming length */
  if (uSize % FLASH_IF_MIN_WRITE_LEN != 0U)
  {
    uOldSize = uSize;
    uSize += (FLASH_IF_MIN_WRITE_LEN - (uSize % FLASH_IF_MIN_WRITE_LEN));
    m_uDwlImgEnd += (FLASH_IF_MIN_WRITE_LEN - (uSize % FLASH_IF_MIN_WRITE_LEN));
    while (uOldSize < uSize)
    {
      pData[uOldSize] = 0xFF;
      uOldSize++;
    }
  }

  /* Write Data in Flash - size has to be 256-bit aligned */

  /* Write in flash only if not beyond allowed area */
  if (((m_uDwlImgCurrent + uSize) <= m_uDwlImgEnd) && (e_ret_status == HAL_OK))
  {
    if (FLASH_If_Write((void *)m_uDwlImgCurrent, pData, uSize) == HAL_OK)
    {
      e_ret_status = HAL_OK;

      /* Move Current destination address for next packet */
      m_uDwlImgCurrent += uSize;
    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }
  else
  {
    e_ret_status = HAL_ERROR;
  }

  /* Last packet : reset m_uPacketsReceived */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    m_uPacketsReceived = 0U;
  }

  /* Reset data counters in case of error */
  if (e_ret_status == HAL_ERROR)
  {

    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }
  return e_ret_status;
}
#endif /* !(EXTERNAL_LOADER) */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
