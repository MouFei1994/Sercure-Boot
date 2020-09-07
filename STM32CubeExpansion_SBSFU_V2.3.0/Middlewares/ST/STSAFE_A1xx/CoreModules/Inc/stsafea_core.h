/**
  ******************************************************************************
  * @file    stsafea_core.h
  * @author  SMD/AME application teams
  * @version V3.0.0
  * @brief   Header file of STSAFE-A Middleware Core module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * STSAFE DRIVER SOFTWARE LICENSE AGREEMENT (SLA0088)
  *
  * BY INSTALLING, COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE
  * OR ANY PART THEREOF (AND THE RELATED DOCUMENTATION) FROM STMICROELECTRONICS
  * INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (STMICROELECTRONICS),
  * THE RECIPIENT, ON BEHALF OF HIMSELF OR HERSELF, OR ON BEHALF OF ANY ENTITY BY WHICH
  * SUCH RECIPIENT IS EMPLOYED AND/OR ENGAGED AGREES TO BE BOUND BY THIS SOFTWARE LICENSE
  * AGREEMENT.
  *
  * Under STMicroelectronics’ intellectual property rights, the redistribution,
  * reproduction and use in source and binary forms of the software or any part thereof,
  * with or without modification, are permitted provided that the following conditions
  * are met:
  * 1.  Redistribution of source code (modified or not) must retain any copyright notice,
  *     this list of conditions and the disclaimer set forth below as items 10 and 11.
  * 2.  Redistributions in binary form, except as embedded into a microcontroller or
  *     microprocessor device or a software update for such device, must reproduce any
  *     copyright notice provided with the binary code, this list of conditions, and the
  *     disclaimer set forth below as items 10 and 11, in documentation and/or other
  *     materials provided with the distribution.
  * 3.  Neither the name of STMicroelectronics nor the names of other contributors to this
  *     software may be used to endorse or promote products derived from this software or
  *     part thereof without specific written permission.
  * 4.  This software or any part thereof, including modifications and/or derivative works
  *     of this software, must be used and execute solely and exclusively in combination
  *     with a secure microcontroller device from STSAFE family manufactured by or for
  *     STMicroelectronics.
  * 5.  No use, reproduction or redistribution of this software partially or totally may be
  *     done in any manner that would subject this software to any Open Source Terms.
  *     “Open Source Terms” shall mean any open source license which requires as part of
  *     distribution of software that the source code of such software is distributed
  *     therewith or otherwise made available, or open source license that substantially
  *     complies with the Open Source definition specified at www.opensource.org and any
  *     other comparable open source license such as for example GNU General Public
  *     License(GPL), Eclipse Public License (EPL), Apache Software License, BSD license
  *     or MIT license.
  * 6.  STMicroelectronics has no obligation to provide any maintenance, support or
  *     updates for the software.
  * 7.  The software is and will remain the exclusive property of STMicroelectronics and
  *     its licensors. The recipient will not take any action that jeopardizes
  *     STMicroelectronics and its licensors' proprietary rights or acquire any rights
  *     in the software, except the limited rights specified hereunder.
  * 8.  The recipient shall comply with all applicable laws and regulations affecting the
  *     use of the software or any part thereof including any applicable export control
  *     law or regulation.
  * 9.  Redistribution and use of this software or any part thereof other than as  permitted
  *     under this license is void and will automatically terminate your rights under this
  *     license.
  * 10. THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY
  *     EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
  *     OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS, WHICH ARE DISCLAIMED TO THE FULLEST
  *     EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE
  *     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  *     THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  *     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  *     ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 11. EXCEPT AS EXPRESSLY PERMITTED HEREUNDER, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS
  *     OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF
  *     STMICROELECTRONICS OR ANY THIRD PARTY.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STSAFEA_CORE_H
#define STSAFEA_CORE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stsafeaxxx.h"
/** @addtogroup STSAFE_A1XX_CORE_MODULES
  * @{
  */
/** @addtogroup CORE
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CORE_Exported_Defines
  * @{
  */

/** @defgroup STSAFEA_DATA_LENGTH
  * @{
  */
#define STSAFEA_LENGTH_SIZE                                 ( 2U )   /*!<Used length size is 2-bytes (uint16_t) */
#define STSAFEA_LENGTH_CMD_RESP_GET_SIGNATURE_SHA_256       ((uint16_t)(2U* (STSAFEA_LENGTH_SIZE + STSAFEA_XYRS_ECDSA_SHA256_LENGTH))) /*!< ECDSA_SHA256 GenerateSignature CMD Response len (equal to 68)*/
#define STSAFEA_LENGTH_CMD_RESP_GET_SIGNATURE_SHA_384       ((uint16_t)(2U* (STSAFEA_LENGTH_SIZE + STSAFEA_XYRS_ECDSA_SHA384_LENGTH))) /*!< ECDSA_SHA384 GenerateSignature CMD Response len (equal to 100)*/
#define STSAFEA_LENGTH_CMD_RESP_GENERATE_SIGNATURE_SHA_256  ((uint16_t)(2U* (STSAFEA_LENGTH_SIZE + STSAFEA_XYRS_ECDSA_SHA256_LENGTH))) /*!< ECDSA_SHA256 GenerateSignature CMD Response len (equal to  68)*/
#define STSAFEA_LENGTH_CMD_RESP_GENERATE_SIGNATURE_SHA_384  ((uint16_t)(2U* (STSAFEA_LENGTH_SIZE + STSAFEA_XYRS_ECDSA_SHA384_LENGTH))) /*!< ECDSA_SHA384 GenerateSignature CMD Response len (equal to 100)*/
/**
  * @}
  */


/** @defgroup STSAFEA_DECREMENT_CMD
  * @{
  */
/******************  Bits definition for DECREMENT command *******************/
#define STSAFEA_CMD_DECREMENT_HEADER_CHAC_POS             (4U)                                                      /*!< Change AC indicator, if '1' means change access request, if '0' no change. */
#define STSAFEA_CMD_DECREMENT_HEADER_CHAC_MSK             (0x1UL << STSAFEA_CMD_DECREMENT_HEADER_CHAC_POS)           /*!< 0x00010000 */
#define STSAFEA_CMD_DECREMENT_HEADER_CHACEN               STSAFEA_CMD_DECREMENT_HEADER_CHAC_MSK                      /*!< Change access request enabled */

#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_POS      (3U)  /* Zero based */                                     /*!< New value of update AC change right. Must be equal to '0' if change AC indicator is '0'.*/
#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_MSK      (0x1UL << STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_POS)    /*!< 0x00001000 */
#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_ON       STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_MSK               /*!< Update rigth enabled */
#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_OFF      (0x0UL & STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_MSK)     /*!< Update rigth disabled */

#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDAC_POS         (0U) /* Zero based */                                     /*!< New value of update AC. Must be stricter than current and equal to '000' if change AC indicator is '0'.*/
#define STSAFEA_CMD_DECREMENT_HEADER_NEWUPDAC_MSK         (0x7UL << STSAFEA_CMD_DECREMENT_HEADER_NEWUPDAC_POS)       /*!< 0x00000111 */
/**
  * @}
  */

/** @defgroup STSAFEA_UPDATE_CMD
  * @{
  */
/******************  Bits definition for UPDATE command ***********************/
#define STSAFEA_CMD_UPDATE_HEADER_ATOM_POS             (7U)                                                /*!< Atomicity mode, if '1' means the processing has to be atomic, if '0' it has not. Zero based */
#define STSAFEA_CMD_UPDATE_HEADER_ATOM_MSK             (0x1UL << STSAFEA_CMD_UPDATE_HEADER_ATOM_POS)       /*!< 0x10000000 */
#define STSAFEA_CMD_UPDATE_HEADER_ATOMEN               STSAFEA_CMD_UPDATE_HEADER_ATOM_MSK                  /*!< Atomicity mode enabled */

#define STSAFEA_CMD_UPDATE_HEADER_CHAC_POS             (4U)                                                /*!< Change AC indicator, if '1' means a change access request, if '0' no change. Zero based */
#define STSAFEA_CMD_UPDATE_HEADER_CHAC_MSK             (0x1UL << STSAFEA_CMD_UPDATE_HEADER_CHAC_POS)       /*!< 0x00010000 */
#define STSAFEA_CMD_UPDATE_HEADER_CHACEN               STSAFEA_CMD_UPDATE_HEADER_CHAC_MSK                  /*!< Change access request enabled */

#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_POS      (3U)                                                 /*!< New value of update AC change right. Must be equal to '0' if change AC indicator is '0'. Zero based */
#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_MSK      (0x1UL << STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_POS) /*!< 0x00001000 */
#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_ON       STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_MSK            /*!< Change access request enabled */
#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_OFF      (0x0UL & STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_MSK)  /*!< Change access request enabled */

#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDAC_POS         (0U)                                                 /*!< New value of update AC. Must be stricter than current and equal to '000' if change AC indicator is '0'. Zero based */
#define STSAFEA_CMD_UPDATE_HEADER_NEWUPDAC_MSK         (0x7UL << STSAFEA_CMD_UPDATE_HEADER_NEWUPDAC_POS)    /*!< 0x00000111 */
/**
  * @}
  */

/** @defgroup STSAFEA_I2C_QUERY_CMD
  * @{
  */
/*******************  Bits definition for I2C parameters  *********************/
#define STSAFEA_I2C_MSK                       (0xFFFFU)                                       /*!< I2C param mask */
#define STSAFEA_I2C_ADDRESS_POS               (1U)                                            /*!< Address */
#define STSAFEA_I2C_ADDRESS_MSK               (0xFEU << STSAFEA_I2C_LPMODE_POS)                /*!< 0b1111111000000000 */

#define STSAFEA_I2C_LPMODE_POS                (1U)                                            /*!< Low-power mode */
#define STSAFEA_I2C_LPMODE_MSK                (0x3U << STSAFEA_I2C_LPMODE_POS)                 /*!< 0b0000000000000110 */

#define STSAFEA_I2C_LOCK_POS                  (0U)                                            /*!< Lock */
#define STSAFEA_I2C_LOCK_MSK                  (0x1U << STSAFEA_I2C_LOCK_POS)                   /*!< 0b0000000000000001 */
#define STSAFEA_I2C_LOCKEN                    STSAFEA_I2C_LOCK_MSK                             /*!< I2C Lock enabled */

#define STSAFEA_I2C_LPMODE_NONE               (0x0U << STSAFEA_I2C_LPMODE_POS)                 /*!< Low-power mode: none, 0b00000000 */
#define STSAFEA_I2C_LPMODE_STDBY              (0x2U << STSAFEA_I2C_LPMODE_POS)                 /*!< Low-power mode: standby, 0b00000100 */
#define STSAFEA_I2C_LPMODE_NOTAUTH            ((~((STSAFEA_I2C_LPMODE_NONE) | (STSAFEA_I2C_LPMODE_STDBY))) & STSAFEA_I2C_LPMODE_MSK)  /*!< Low-power mode: not authorized, 0b00000010 | 0b00000011  */

/**
  * @}
  */



/**
  * @}CORE_Exported_Defines
  */

/** @defgroup CORE_Exported_Macros
  * @{
  */
#define STSAFEA_GET_ECC_CURVE_OID(CURVE_ID)     (((CURVE_ID) == STSAFEA_NIST_P_256) ? STSAFEA_ECC_CURVE_OID_NIST_P_256 :                \
                                                 ((CURVE_ID) == STSAFEA_NIST_P_384) ? STSAFEA_ECC_CURVE_OID_NIST_P_384 :                \
                                                 ((CURVE_ID) == STSAFEA_BRAINPOOL_P_256) ? STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_256 :      \
                                                 ((CURVE_ID) == STSAFEA_BRAINPOOL_P_384) ? STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_384 : STSAFEA_ECC_CURVE_OID_NULL)

#define STSAFEA_GET_ECC_CURVE_OID_LEN(CURVE_ID) (((CURVE_ID) == STSAFEA_NIST_P_256) ? STSAFEA_ECC_CURVE_OID_NIST_P_256_LEN :             \
                                                 ((CURVE_ID) == STSAFEA_NIST_P_384) ? STSAFEA_ECC_CURVE_OID_NIST_P_384_LEN :             \
                                                 ((CURVE_ID) == STSAFEA_BRAINPOOL_P_256) ? STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_256_LEN :   \
                                                 ((CURVE_ID) == STSAFEA_BRAINPOOL_P_384) ? STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_384_LEN : 0U)

#define STSAFEA_GET_XYRS_LEN_FROM_CURVE(CURVE_ID) (((CURVE_ID) == STSAFEA_NIST_P_256) ? STSAFEA_XYRS_ECDSA_SHA256_LENGTH :                 \
                                                   ((CURVE_ID) == STSAFEA_NIST_P_384) ? STSAFEA_XYRS_ECDSA_SHA384_LENGTH :                 \
                                                   ((CURVE_ID) == STSAFEA_BRAINPOOL_P_256) ? STSAFEA_XYRS_ECDSA_SHA256_LENGTH :            \
                                                   ((CURVE_ID) == STSAFEA_BRAINPOOL_P_384) ? STSAFEA_XYRS_ECDSA_SHA384_LENGTH : 0U)

/* Note: Consider to rework the following code in the next release.
   It generates a not-clear and not-readable return in the middle of functions that should be avoided */
#define STSAFEA_CHECK_SIZE(index, size)                   \
  {                                                         \
    if ((size + index ) > STSAFEA_BUFFER_DATA_PACKET_SIZE)  \
    {                                                       \
      return STSAFEA_BUFFER_LENGTH_EXCEEDED;                \
    }                                                       \
  }

/**
  * @}
  */
/** @defgroup CORE_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup CORE_Exported_Variables
  * @{
  */

/**
  * @}
  */


/** @addtogroup CORE_Exported_Functions
  * @{
  */

/** @addtogroup CORE_Exported_Functions_Group1 Initialization / Configuration APIs
  *  @brief    STSAFE-A Core APIs related to Initialization and configuration
  *
@verbatim
 ===============================================================================
              ##### Initialization/Configuration functions #####
 ===============================================================================
[..]
    (+) StSafeA_Init
    (+) StSafeA_GetVersion
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_Init(StSafeA_Handle_t *pStSafeA, uint8_t *pAllocatedRxTxBufferData);
int32_t StSafeA_GetVersion(void);
/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group2 General Purpose Commands APIs
  *  @brief    STSAFE-A APIs related to general purpose commands
  *
@verbatim
 ===============================================================================
              ##### General Purpose Commands #####
 ===============================================================================
[..]
    (+) Echo
    (+) Reset
    (+) Generate Random
    (+) Start Session
    (+) Hibernate
@endverbatim
  * @{
  */

StSafeA_ResponseCode_t StSafeA_Echo(
  StSafeA_Handle_t *pStSafeA,
  uint8_t *pInEchoData,
  uint16_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_Reset(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_GenerateRandom(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_RndSubject_t InRndSubject,
  uint8_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC);

StSafeA_ResponseCode_t StSafeA_StartSignatureSession(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint16_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC);

StSafeA_ResponseCode_t StSafeA_Hibernate(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InWakeUpMode,
  uint8_t InMAC);


/**
  * @}
  */


/** @addtogroup CORE_Exported_Functions_Group3 Data Partition Commands APIs
  *  @brief    STSAFE-A APIs related to data partition commands
  *
@verbatim
 ===============================================================================
              ##### Data Partition Commands #####
 ===============================================================================
[..]
    (+) Decrement
    (+) Read
    (+) Update
    (+) Increment
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_Decrement(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InChangeACIndicator,
  uint8_t InNewDecrementACRight,
  uint8_t InNewDecrementAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  uint32_t InAmount,
  StSafeA_LVBuffer_t *pInData,
  StSafeA_DecrementBuffer_t *pOutDecrement,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_Read(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InChangeACIndicator,
  uint8_t InNewReadACRight,
  uint8_t InNewReadAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  uint16_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_Update(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InAtomicity,
  uint8_t InChangeACIndicator,
  uint8_t InNewUpdateACRight,
  uint8_t InNewUpdateAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  StSafeA_LVBuffer_t *pInLVData,
  uint8_t InMAC);


/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group4 Companion Commands APIs
  *  @brief    STSAFE-A APIs related to companion commands
  *  @note     Companion commands are STSAFE-A110 related and don't apply to STSAFE-A100
  *
@verbatim
 ===============================================================================
              ##### Companion Commands  #####
 ===============================================================================
[..]
    (+) Cache Base Key
    (+) Generate C-MAC/R-MAC
    (+) Verify RMAC
@endverbatim
  * @{
  */

/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group5 Private and Public Key Commands APIs
  *  @brief    STSAFE-A APIs related to private and public key commands
  *
@verbatim
 ===============================================================================
              ##### Private and Public Key Commands  #####
 ===============================================================================
[..]
    (+) Get Signature
    (+) Generate Signature
    (+) Verify Message Signature
    (+) Verify Entity Signature
    (+) Establish Key
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_GetSignature(
  StSafeA_Handle_t *pStSafeA,
  const uint8_t *pInChallenge,
  uint16_t InChallengeSize,
  uint16_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_GenerateSignature(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  const uint8_t *pInDigest,
  StSafeA_HashTypes_t InDigestType,
  uint16_t RespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC,
  uint8_t InHostEncryption);


StSafeA_ResponseCode_t StSafeA_VerifyMessageSignature(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_CurveId_t InCurveId,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  const StSafeA_LVBuffer_t *pInRSignature,
  const StSafeA_LVBuffer_t *pInSSignature,
  const StSafeA_LVBuffer_t *pInDigest,
  StSafeA_VerifySignatureBuffer_t *pOutRespVerifySignature,
  uint8_t InMAC);



StSafeA_ResponseCode_t StSafeA_VerifyEntitySignature(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_LVBuffer_t *pInRSignature,
  const StSafeA_LVBuffer_t *pInSSignature,
  StSafeA_VerifySignatureBuffer_t *pOutRespVerifySignature,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_EstablishKey(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  uint16_t InSharedKeyLength,
  StSafeA_SharedSecretBuffer_t *pOutSharedSecret,
  uint8_t InMAC,
  uint8_t InHostEncryption);

/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group6 Administrative Commands APIs
  *  @brief    STSAFE-A APIs related to administrative commands
  *
@verbatim
 ===============================================================================
              ##### Administrative Commands  #####
 ===============================================================================
[..]
    (+) Query
    (+) Put Attribute
    (+) Generate Key
    (+) Delete
    (+) Wrap Local Envelope
    (+) Unwrap Local Envelope
    (+) Verify Password
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_ProductDataQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_ProductDataBuffer_t *pOutProductData,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_DataPartitionQuery(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InZoneMaxNum,
  StSafeA_DataPartitionBuffer_t *pOutDataPartition,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_I2cParameterQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_I2cParameterBuffer_t *pOutI2CParamData,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_LifeCycleStateQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_LifeCycleStateBuffer_t *pOutLifeCycleState,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_PublicKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_PublicKeySlotBuffer_t *pOutPubKeySlot,
  uint16_t InPubXYLen,
  StSafeA_LVBuffer_t *pOutPubX,
  StSafeA_LVBuffer_t *pOutPubY,
  StSafeA_LVBuffer_t *pOutCurveId,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_HostKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_HostKeySlotBuffer_t *pOutHostKeySlot,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_LocalEnvelopeKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_LocalEnvelopeKeyTableBuffer_t *pOutLocalEnvelopeKeyTable,
  StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t *pOutLlocalEnvelopeKeySlot0InformationRecord,
  StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t *pOutLlocalEnvelopeKeySlot1InformationRecord,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_CommandAuthorizationConfigurationQuery(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InCmdAuthRecordNum,
  StSafeA_CommandAuthorizationConfigurationBuffer_t *pOutCmdAuthConfig,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_PutAttribute(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InAttributeTag,
  const uint8_t *pInData,
  uint16_t InDataSize,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_PutPublicKeySlot(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_CurveId_t InCurveId,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_GenerateKeyPair(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint16_t InUseLimit,
  uint8_t InChangeAuthFlagsRight,
  uint8_t InAuthorizationFlags,
  StSafeA_CurveId_t InCurveId,
  uint8_t *pOutPointReprensentationId,
  uint16_t InPubXYLen,
  StSafeA_LVBuffer_t *pOutPubX,
  StSafeA_LVBuffer_t *pOutPubY,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_GenerateLocalEnvelopeKey(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t InKeyType,
  uint8_t *pInSeed,
  uint16_t InSeedSize,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_DeletePassword(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InMAC);


StSafeA_ResponseCode_t StSafeA_WrapLocalEnvelope(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t *pInData,
  uint16_t InDataSize,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC,
  uint8_t InHostEncryption);


StSafeA_ResponseCode_t StSafeA_UnwrapLocalEnvelope(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t *pInLocalEnvelope,
  uint16_t InLocalEnvelopeSize,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC,
  uint8_t InHostEncryption);


StSafeA_ResponseCode_t StSafeA_VerifyPassword(
  StSafeA_Handle_t *pStSafeA,
  const uint8_t *pInPassword,
  StSafeA_VerifyPasswordBuffer_t *pOutVerifyPassword,
  uint8_t InMAC);

/**
  * @}
  */


/** @addtogroup CORE_Exported_Functions_Group7 Additional Commands APIs
  *  @brief    STSAFE-A APIs other additional commands APIs
  *
@verbatim
 ===============================================================================
              ##### Additional Commands  #####
 ===============================================================================
[..]
    (+) RawCommand
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_RawCommand(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_TLVBuffer_t *pInRawCommand,
  uint16_t RespDataLen,
  StSafeA_TLVBuffer_t *pOutTLVResponse,
  uint32_t DelayMs,
  uint8_t InMAC);


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
#endif /* __cplusplus */

#endif /* STSAFEA_CORE_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
