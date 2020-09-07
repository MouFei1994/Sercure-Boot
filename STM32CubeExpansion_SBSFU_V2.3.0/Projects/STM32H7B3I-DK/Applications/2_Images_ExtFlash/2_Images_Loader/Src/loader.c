/**
  ******************************************************************************
  * @file    sfu_loader.c
  * @author  MCD Application Team
  * @brief   Secure Firmware Update LOADER module.
  *          This file provides set of firmware functions to manage SFU local
  *          loader functionalities.
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
#include "main.h"
#include "loader.h"
#include "loader_low_level.h"
#include "loader_low_level_flash.h"
#include "se_def_metadata.h"
#include <string.h>
#include <stdlib.h>

/* Private defines -----------------------------------------------------------*/
#define SFUMAGIC "SFUM"                                      /*!< MAGIC tag in the header*/

#define LOADER_TIME_OUT            ((uint32_t )0x800U)       /*!< COM Transmit and Receive Timeout*/
#define LOADER_SERIAL_TIME_OUT     ((uint32_t )100U)         /*!< Serial PutByte and PutString Timeout*/

#define IS_CAP_LETTER(c)    (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            (((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))


/* Private variables ---------------------------------------------------------*/
static uint8_t  m_aPacketData[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_DATA_INDEX +
                              YMODEM_PACKET_TRAILER_SIZE] __attribute__((aligned(8))); /*!<Array used to store Packet */
static uint32_t m_uDwlAreaAddress = 0U;                          /*!< Address of to write in download area */
static uint32_t m_uDwlAreaStart = 0U;                            /*!< Address of download area */
static uint32_t m_uDwlAreaSize = 0U;                             /*!< Size of download area */
static uint32_t m_uFileSizeYmodem = 0U;                          /*!< Ymodem file size being received */
static uint32_t m_uNbrBlocksYmodem = 0U;                         /*!< Number of blocks being received via Ymodem*/
static uint32_t m_uPacketsReceived = 0U;                         /*!< Number of packets received via Ymodem*/

/* Private function prototypes -----------------------------------------------*/
static LOADER_StatusTypeDef LOADER_VerifyFwHeader(uint8_t *pBuffer);
static LOADER_StatusTypeDef LOADER_YMODEM_Receive(uint32_t *puSize);
static HAL_StatusTypeDef LOADER_YMODEM_RX_Packet(uint8_t *pData, uint32_t *puLength, uint32_t uTimeout);
static HAL_StatusTypeDef LOADER_Serial_PutByte(uint8_t uParam);
static uint32_t Str2Int(uint8_t *pInputStr, uint32_t *pIntNum);
static LOADER_StatusTypeDef LOADER_YMODEM_HeaderPktRxCpltCallback(uint32_t uFileSize);
static LOADER_StatusTypeDef LOADER_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize);


/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Initialize the SFU LOADER.
  * @param  None
  * @retval LOADER_OK if successful, LOADER_ERR otherwise.
  */
LOADER_StatusTypeDef LOADER_Init(void)
{
  /*
   * Sanity check to make sure that the local loader cannot read out of the buffer bounds
   * when doing a length alignment before writing in FLASH.
   */
  if (0U != (uint32_t)(YMODEM_PACKET_1K_SIZE % (uint32_t)sizeof(LOADER_LL_FLASH_write_t)))
  {
    /* The packet buffer (payload part) must be a multiple of the FLASH write length  */
    TRACE("\r\nPacket Payload size (%d) is not matching the FLASH constraints", YMODEM_PACKET_1K_SIZE);
    return LOADER_ERR;
  }

  return LOADER_OK;
}

/**
  * @brief  DeInitialize the SFU LOADER.
  * @param  None
  * @retval LOADER_OK, successful.
  */
LOADER_StatusTypeDef LOADER_DeInit(void)
{
  return LOADER_OK;
}

/**
  * @brief  Download a new User Fw via Ymodem protocol.
  *         Writes firmware received in FLASH.
  * @param  pRxSize: Size of the downloaded image.
  * @retval LOADER_OK if successful, see LOADER_StatusTypeDef otherwise.
  */
LOADER_StatusTypeDef LOADER_DownloadNewUserFw(uint32_t *pRxSize)
{
  LOADER_StatusTypeDef e_ret_status = LOADER_OK;

  /* If the SecureBoot configured the IWDG, loader must reload IWDG counter with value defined in the reload register */
  WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);

  /* Transfer FW Image via YMODEM protocol */
  printf("\r\n\t  File> Transfer> YMODEM> Send ");

  /* Assign the download flash address to be used during the YMODEM process */
  m_uDwlAreaStart =  SLOT_DWL_REGION_START;
  m_uDwlAreaSize  =  SLOT_DWL_REGION_SIZE;

  /* Initialize CRC, needed to control data integrity */
  if (LOADER_LL_CRC_Init() == HAL_OK)
  {
    e_ret_status = LOADER_ERR_DOWNLOAD;
  }

  /* Receive the FW in RAM and write it in the Flash */
  e_ret_status = LOADER_YMODEM_Receive(pRxSize);
  if (e_ret_status == LOADER_OK)
  {
    /* Check received FW size */
    if (*pRxSize == 0U)
    {
      /* Download file size is not correct */
      e_ret_status = LOADER_ERR_DOWNLOAD;
    }
  }

  return e_ret_status;
}

/**
  * @brief  Write the FW header in the swap or write specific information in RAM to indicate an installation procedure
  *         should be started at next reboot
  * @param  pBuffer: pointer to header buffer in flash.
  * @retval LOADER_OK if successful, see LOADER_StatusTypeDef otherwise.
  */
LOADER_StatusTypeDef LOADER_InstallAtNextReset(uint8_t *pBuffer)
{
  LOADER_StatusTypeDef  e_ret_status = LOADER_OK;

  SE_FwRawHeaderTypeDef slot0_header;

  /* Standalone loader communication : FW installation requested trough SWAP area */
  (*(uint32_t *)LOADER_COM_REGION_RAM_START) = STANDALONE_LOADER_NO_REQ;          /* Download done */

  /* Read the header in flash */
  if (LOADER_LL_FLASH_Read((void *)&slot0_header, (void const *)pBuffer, sizeof(SE_FwRawHeaderTypeDef)) != HAL_OK)
  {
    e_ret_status = LOADER_ERR_FLASH_ACCESS;
  }

  /* Erase the swap region */
  if ((e_ret_status == LOADER_OK)
      && (LOADER_LL_FLASH_Erase_Size((void *) REGION_SWAP_START, IMAGE_OFFSET) != HAL_OK))
  {
    e_ret_status = LOADER_ERR_FLASH_ACCESS;
  }

  /* Store the header in the swap region */
  if ((e_ret_status == LOADER_OK)
      && (LOADER_LL_FLASH_Write((void *)REGION_SWAP_START, (void const *)&slot0_header, SE_FW_HEADER_TOT_LEN)
          != HAL_OK))
  {
    e_ret_status = LOADER_ERR_FLASH_ACCESS;
  }

  return e_ret_status;
}


/* Private Functions Definition ------------------------------------------------*/
/**
  * @brief  Verifies the Raw Fw Header received. It checks if the header is
  *         authentic and if the fields are ok with the device (e.g. size and version).
  * @param  pBuffer: pointer to header buffer.
  * @retval LOADER_OK if successful, see LOADER_StatusTypeDef otherwise.
  */
static LOADER_StatusTypeDef LOADER_VerifyFwHeader(uint8_t *pBuffer)
{
  LOADER_StatusTypeDef  e_ret_status = LOADER_ERR_CMD_AUTH_FAILED;
  SE_FwRawHeaderTypeDef *p_header, slot0_header;
  uint32_t slot0_version = 0;

  /* Parse the header of SLOT 0 and retrieve the version installed */
  if (LOADER_LL_FLASH_Read((void *)&slot0_header, (void const *)REGION_SLOT_0_HEADER, SE_FW_HEADER_TOT_LEN) != HAL_OK)
  {
    return LOADER_ERR_CMD_AUTH_FAILED;
  }

  if (!memcmp(&(slot0_header.SFUMagic), SFUMAGIC, strlen(SFUMAGIC)))
  {
    slot0_version = ((int32_t)slot0_header.FwVersion);
  }

  /* Parse the received buffer and check the version to install */
  p_header = (SE_FwRawHeaderTypeDef *)pBuffer;

  /* Check if the received header packet is authentic : SFUM */
  if (!memcmp(&(p_header->SFUMagic), SFUMAGIC, strlen(SFUMAGIC)))
  {
    /*
     * It is not allowed to install a Firmware with a lower version than the active firmware.
     * But we authorize the re-installation of the current firmware version.
     */
    if ((((int32_t)p_header->FwVersion) >= slot0_version))
    {
      /* The installation is authorized */
      TRACE("\r\n          Anti-rollback: candidate version(%d) accepted | current version(%d) !", p_header->FwVersion,
            p_slot0_header->FwVersion);
      e_ret_status = LOADER_OK;
    }
    else
    {
      /* The installation is forbidden */
      TRACE("\r\n          Anti-rollback: candidate version(%d) rejected | current version(%d) !", p_header->FwVersion,
            p_slot0_header->FwVersion);
      e_ret_status = LOADER_ERR_OLD_FW_VERSION;
    }

    if (e_ret_status == LOADER_OK)
    {
      /*
       * Check length :
       * We do not check the length versus the trailer constraint because this check is already implemented in
       * FirmwareToInstall().
       * If the firmware is too big to have some room left for the trailer info then the installation will not be
       * triggered.
       * The interest is to avoid duplicating the checks (checking just in time).
       * This is also because in the case of a download handled by the UserApp we cannot rely on the checks done by the
       * UserApp before installing a FW.
       * The drawback is that the firmware can be downloaded in slot #1 though it is too big.
       *
       * Nevertheless, we can still detect if the FW is too big to be downloaded (cannot be written in slot #1).
       * This will avoid download issues (but the installation itself can still be rejected) or overflows.
       * The slot #1 must contain the HEADER and also the binary FW (encrypted).
       * But there is an offset of FW_OFFSET_IMAGE bytes to respect.
       */
      if ((p_header->PartialFwSize + (p_header->PartialFwOffset % SWAP_REGION_SIZE)) > (SLOT_DWL_REGION_SIZE -
          IMAGE_OFFSET))
      {
        /* The firmware cannot be written in slot #1 */
        e_ret_status = LOADER_ERR_FW_LENGTH;
      }
    }
  }
  return e_ret_status;
}

/**
  * @brief  Receive a file using the ymodem protocol.
  * @param  peCOMStatus: YMODEM_StatusTypeDef result of reception/programming.
  * @param  puSize: size of received file.
  * @retval HAL_OK if successful, HAL_ERROR otherwise
  */
static LOADER_StatusTypeDef LOADER_YMODEM_Receive(uint32_t *puSize)
{
  LOADER_StatusTypeDef e_ret_status = LOADER_OK;
  uint32_t i = 0U;
  uint32_t packet_length = 0U;
  uint32_t session_done = 0U;
  uint32_t file_done = 0U;
  uint32_t errors = 0U;
  uint32_t session_begin = 0U;
  uint32_t ramsource = 0U;
  uint32_t filesize = 0U;
  uint32_t packets_received = 0;
  uint8_t *file_ptr;
  uint8_t file_size[YMODEM_FILE_SIZE_LENGTH + 1U];
  uint8_t tmp = 0U;



  while ((session_done == 0U) && (e_ret_status == LOADER_OK))
  {
    packets_received = 0U;
    file_done = 0U;
    while ((file_done == 0U) && (e_ret_status == LOADER_OK))
    {
      switch (LOADER_YMODEM_RX_Packet(m_aPacketData, &packet_length, YMODEM_DOWNLOAD_TIMEOUT))
      {
        case HAL_OK:
          errors = 0U;
          switch (packet_length)
          {
            case 3U:
              /* Startup sequence */
              break;
            case 2U:
              /* Abort by sender */
              LOADER_Serial_PutByte(YMODEM_ACK);
              e_ret_status = LOADER_ERR_ABORT;
              break;
            case 0U:
              /* End of transmission */
              LOADER_Serial_PutByte(YMODEM_ACK);
              *puSize = filesize;
              file_done = 1U;
              break;
            default:
              /* Normal packet */
              if (m_aPacketData[YMODEM_PACKET_NUMBER_INDEX] != (packets_received & 0xff))
              {
                /* No NACK sent for a better synchro with remote : packet will be repeated */
              }
              else
              {
                if (packets_received == 0U)
                {
                  /* File name packet */
                  if (m_aPacketData[YMODEM_PACKET_DATA_INDEX] != 0U)
                  {
                    /* File name parsing */
                    i = 0U;
                    file_ptr = m_aPacketData + YMODEM_PACKET_DATA_INDEX;
                    while ((*file_ptr != 0U) && (i < YMODEM_FILE_NAME_LENGTH))
                    {
                      file_ptr++;
                    }

                    /* File size extraction */
                    i = 0U;
                    file_ptr ++;
                    while ((*file_ptr != ' ') && (i < YMODEM_FILE_SIZE_LENGTH))
                    {
                      file_size[i++] = *file_ptr++;
                    }
                    file_size[i++] = '\0';
                    Str2Int(file_size, &filesize);

                    /* Header packet received callback call*/
                    e_ret_status = LOADER_YMODEM_HeaderPktRxCpltCallback((uint32_t) filesize);
                    if (e_ret_status == LOADER_OK)
                    {
                      /* Continue reception */
                      LOADER_Serial_PutByte(YMODEM_ACK);
                      LOADER_LL_UART_Flush();
                      LOADER_Serial_PutByte(YMODEM_CRC16);
                    }
                    else
                    {
                      /* End session */
                      tmp = YMODEM_CA;
                      LOADER_LL_UART_Transmit(&tmp, 1U, YMODEM_NAK_TIMEOUT);
                      LOADER_LL_UART_Transmit(&tmp, 1U, YMODEM_NAK_TIMEOUT);
                    }
                  }
                  /* File header packet is empty, end session */
                  else
                  {
                    LOADER_Serial_PutByte(YMODEM_ACK);
                    file_done = 1U;
                    session_done = 1U;
                    break;
                  }
                }
                else /* Data packet */
                {
                  ramsource = (uint32_t) & m_aPacketData[YMODEM_PACKET_DATA_INDEX];

                  /* Data packet received callback call*/
                  e_ret_status = LOADER_YMODEM_DataPktRxCpltCallback((uint8_t *) ramsource, (uint32_t) packet_length);
                  if (e_ret_status == LOADER_OK)
                  {
                    /* Continue reception */
                    LOADER_Serial_PutByte(YMODEM_ACK);
                  }
                  else /* An error occurred while writing to Flash memory / Checking header */
                  {
                    /* End session */
                    LOADER_Serial_PutByte(YMODEM_CA);
                    LOADER_Serial_PutByte(YMODEM_CA);
                  }
                }
                packets_received ++;
                session_begin = 1;
              }
              break;
          }
          break;
        case HAL_BUSY: /* Abort actually */
          LOADER_Serial_PutByte(YMODEM_CA);
          LOADER_Serial_PutByte(YMODEM_CA);
          e_ret_status = LOADER_ERR_ABORT;
          break;
        default:
          if (session_begin > 0U)
          {
            errors ++;
          }
          if (errors > YMODEM_MAX_ERRORS)
          {
            /* Abort communication */
            LOADER_Serial_PutByte(YMODEM_CA);
            LOADER_Serial_PutByte(YMODEM_CA);
            e_ret_status = LOADER_ERR_COM;
          }
          else
          {
            LOADER_Serial_PutByte(YMODEM_CRC16);          /* Ask for a packet */
            printf("\b.");                                /* Replace C char by . on display console */
            BSP_LED_Toggle(LOADER_STATUS_LED);
          }
          break;
      }
    }
  }
  /* Make sure the status LED is turned off */
  BSP_LED_Off(LOADER_STATUS_LED);

  return e_ret_status;
}

/**
  * @brief  Receive a packet from sender
  * @param  pData: pointer to received data.
  * @param  puLength
  *     0: end of transmission
  *     2: abort by sender
  *    >0: packet length
  * @param  uTimeOut: receive timeout (ms).
  * @retval HAL_OK: normally return
  *         HAL_BUSY: abort by user
  */
static HAL_StatusTypeDef LOADER_YMODEM_RX_Packet(uint8_t *pData, uint32_t *puLength, uint32_t uTimeout)
{
  uint32_t crc;
  uint32_t packet_size = 0U;
  HAL_StatusTypeDef e_ret_status;
  HAL_StatusTypeDef e_rx_status;
  uint8_t char1;

  *puLength = 0U;

  /* If the SecureBoot configured the IWDG, loader must reload IWDG counter with value defined in the reload register */
  WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);

  e_rx_status = LOADER_LL_UART_Receive(&char1, 1, uTimeout);

  if (e_rx_status == HAL_OK)
  {
    e_ret_status = HAL_OK;

    switch (char1)
    {
      case YMODEM_SOH:
        packet_size = YMODEM_PACKET_SIZE;
        break;
      case YMODEM_STX:
        packet_size = YMODEM_PACKET_1K_SIZE;
        break;
      case YMODEM_EOT:
        break;
      case YMODEM_CA:
        if ((LOADER_LL_UART_Receive(&char1, 1U, uTimeout) == HAL_OK) && (char1 == YMODEM_CA))
        {
          packet_size = 2U;
        }
        else
        {
          e_ret_status = HAL_ERROR;
        }
        break;
      case YMODEM_ABORT1:
      case YMODEM_ABORT2:
        e_ret_status = HAL_BUSY;
        break;
      case YMODEM_RB:
        LOADER_LL_UART_Receive(&char1, 1U, uTimeout);          /* Ymodem starup sequence : rb ==> 0x72 + 0x62 + 0x0D */
        LOADER_LL_UART_Receive(&char1, 1U, uTimeout);
        packet_size = 3U;
        break;
      default:
        e_ret_status = HAL_ERROR;
        break;
    }
    *pData = char1;

    if (packet_size >= YMODEM_PACKET_SIZE)
    {

      e_rx_status = LOADER_LL_UART_Receive(&pData[YMODEM_PACKET_NUMBER_INDEX], packet_size +
                                           YMODEM_PACKET_OVERHEAD_SIZE, uTimeout);

      /* Simple packet sanity check */
      if (e_rx_status == HAL_OK)
      {
        e_ret_status = HAL_OK;

        if (pData[YMODEM_PACKET_NUMBER_INDEX] != ((pData[YMODEM_PACKET_CNUMBER_INDEX])
                                                  ^ YMODEM_NEGATIVE_BYTE))
        {
          packet_size = 0U;
          e_ret_status = HAL_ERROR;
        }
        else
        {
          /* Check packet CRC*/
          crc = pData[ packet_size + YMODEM_PACKET_DATA_INDEX ] << 8U;
          crc += pData[ packet_size + YMODEM_PACKET_DATA_INDEX + 1U ];

          if (LOADER_LL_CRC_Calculate((uint32_t *)&pData[YMODEM_PACKET_DATA_INDEX], packet_size) != crc)
          {
            packet_size = 0U;
            e_ret_status = HAL_ERROR;
          }
        }
      }
      else
      {
        e_ret_status = HAL_ERROR;
      }
    }
  }
  else
  {
    e_ret_status = HAL_ERROR;
  }

  *puLength = packet_size;
  return e_ret_status;
}

/**
  * @brief  Transmit a byte to the COM Port.
  * @param  uParam: The byte to be sent.
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef LOADER_Serial_PutByte(uint8_t uParam)
{
  return LOADER_LL_UART_Transmit(&uParam, 1U, LOADER_SERIAL_TIME_OUT);
}

/**
  * @brief  Convert a string to an integer.
  * @param  pInputStr: The string to be converted.
  * @param  pIntNum: The integer value.
  * @retval 1: Correct
  *         0: Error
  */
static uint32_t Str2Int(uint8_t *pInputStr, uint32_t *pIntNum)
{
  uint32_t i = 0U;
  uint32_t res = 0U;
  uint32_t val = 0U;

  if ((pInputStr[0] == '0') && ((pInputStr[1] == 'x') || (pInputStr[1] == 'X')))
  {
    i = 2U;
    while ((i < 11U) && (pInputStr[i] != '\0'))
    {
      if (ISVALIDHEX(pInputStr[i]))
      {
        val = (val << 4U) + CONVERTHEX(pInputStr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }

    /* valid result */
    if (pInputStr[i] == '\0')
    {
      *pIntNum = val;
      res = 1U;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ((i < 11U) && (res != 1U))
    {
      if (pInputStr[i] == '\0')
      {
        *pIntNum = val;
        /* return 1 */
        res = 1U;
      }
      else if (((pInputStr[i] == 'k') || (pInputStr[i] == 'K')) && (i > 0U))
      {
        val = val << 10U;
        *pIntNum = val;
        res = 1U;
      }
      else if (((pInputStr[i] == 'm') || (pInputStr[i] == 'M')) && (i > 0U))
      {
        val = val << 20U;
        *pIntNum = val;
        res = 1U;
      }
      else if (ISVALIDDEC(pInputStr[i]))
      {
        val = val * 10U + CONVERTDEC(pInputStr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }
  }

  return res;
}


/* Callback Functions Definition ------------------------------------------------*/

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize: Dimension of the file that will be received.
  * @retval LOADER_OK, successful.
  */
static LOADER_StatusTypeDef LOADER_YMODEM_HeaderPktRxCpltCallback(uint32_t uFileSize)
{
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /*Compute the number of 1K blocks */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (YMODEM_PACKET_1K_SIZE - 1U)) / YMODEM_PACKET_1K_SIZE;

  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000U);

  return LOADER_OK;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData: Pointer to the buffer.
  * @param  uSize: Packet dimension.
  * @retval LOADER_OK if successful, see LOADER_StatusTypeDef otherwise.
  */
static LOADER_StatusTypeDef LOADER_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize)
{
  static uint8_t fw_header[sizeof(SE_FwRawHeaderTypeDef)];
  /* Size of downloaded Image initialized with first packet (header) and checked along download process */
  static uint32_t m_uDwlImgSize = 0U;

  LOADER_StatusTypeDef e_ret_status = LOADER_OK;
  uint32_t uOldSize;
  uint32_t uLength;

  /*Increase the number of received packets*/
  m_uPacketsReceived++;

  /* Last packet : size of data to write could be different than YMODEM_PACKET_1K_SIZE */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    /*Extracting actual payload from last packet*/
    if (0 == (m_uFileSizeYmodem % YMODEM_PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      uSize = YMODEM_PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      uSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / YMODEM_PACKET_1K_SIZE) * YMODEM_PACKET_1K_SIZE);
    }
  }

  /* First packet : Contains the FW header (SE_FW_HEADER_TOT_LEN bytes length) which is not encrypted  */
  if (m_uPacketsReceived == 1)
  {

    m_uDwlAreaAddress = m_uDwlAreaStart;
    memcpy(fw_header, pData, sizeof(SE_FwRawHeaderTypeDef));

    /* Verify header */
    e_ret_status = LOADER_VerifyFwHeader(pData);
    if (e_ret_status == LOADER_OK)
    {
      /*
       * Downloaded Image size : Header size + gap for image alignment to (UpdateFwOffset % sector size) + Partial
       * Image size
       */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwSize +
                      (((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwOffset % SWAP_REGION_SIZE) + IMAGE_OFFSET;
    }

    /* Clear Download application area (including TRAILERS area) */
    if ((e_ret_status == LOADER_OK)
        && (LOADER_LL_FLASH_Erase_Size((void *) m_uDwlAreaAddress, SLOT_DWL_REGION_SIZE) != HAL_OK))
    {
      e_ret_status = LOADER_ERR_FLASH_ACCESS;
    }
  }

  /* This packet : contains end of FW header */
  if ((m_uDwlAreaAddress < (m_uDwlAreaStart + IMAGE_OFFSET)) &&
      ((m_uDwlAreaAddress + uSize) >= (m_uDwlAreaStart + IMAGE_OFFSET)))
  {
    /* Write the FW header part (IMAGE_OFFSET % YMODEM_PACKET_1K_SIZE bytes length) */
    uLength = IMAGE_OFFSET % YMODEM_PACKET_1K_SIZE;
    if (uLength == 0)
    {
      uLength = YMODEM_PACKET_1K_SIZE;
    }
    if (LOADER_LL_FLASH_Write((void *)m_uDwlAreaAddress, pData, uLength) == HAL_OK)
    {
      /* Shift the DWL area pointer, to align image with (PartialFwOffset % sector size) in DWL area */
      m_uDwlAreaAddress += uLength + ((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwOffset % SWAP_REGION_SIZE;

      /* Update remaining packet size to write */
      uSize -= uLength;

      /* Update pData pointer to received packet data */
      pData += uLength;
    }
    else
    {
      e_ret_status = LOADER_ERR_FLASH_ACCESS;
    }
  }

  if (e_ret_status == LOADER_OK)
  {
    /* Check size to avoid writing beyond DWL image size */
    if ((m_uDwlAreaAddress + uSize) > (m_uDwlAreaStart + m_uDwlImgSize))
    {
      e_ret_status = LOADER_ERR_FW_LENGTH;
    }

    /*
     * Set dimension to the appropriate length for FLASH programming.
     * Example: 64-bit length for L4.
     */
    if ((uSize % (uint32_t)sizeof(LOADER_LL_FLASH_write_t)) != 0)
    {
      /*
       * By construction, the length of the buffer (fw_decrypted_chunk or pData) must be a multiple of sizeof(write_t)
       * to avoid reading out of the buffer
       */
      uOldSize = uSize;
      uSize = uSize + ((uint32_t)sizeof(LOADER_LL_FLASH_write_t) - (uSize % (uint32_t)sizeof(LOADER_LL_FLASH_write_t)));
      /*
       * In case of alignment with LOADER_LL_FLASH_write_t structure
       * Initialization of the additional data to the "erasing" pattern
       */
      while (uOldSize < uSize)
      {
        pData[uOldSize] = 0xFF;
        uOldSize++;
      }
    }

    /* Check size to avoid writing beyond DWL area */
    if ((m_uDwlAreaAddress + uSize) > (m_uDwlAreaStart + m_uDwlAreaSize))
    {
      e_ret_status = LOADER_ERR_FW_LENGTH;
    }
  }

  /* Write Data in Flash */
  if (e_ret_status == LOADER_OK)
  {
    if (LOADER_LL_FLASH_Write((void *)m_uDwlAreaAddress, pData, uSize) == HAL_OK)
    {
      m_uDwlAreaAddress += (uSize);
    }
    else
    {
      e_ret_status = LOADER_ERR_FLASH_ACCESS;
    }
  }

  /* Last packet : reset m_uPacketsReceived */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    m_uPacketsReceived = 0U;
  }

  /* Reset data counters in case of error */
  if (e_ret_status != LOADER_OK)
  {
    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }

  return e_ret_status;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
