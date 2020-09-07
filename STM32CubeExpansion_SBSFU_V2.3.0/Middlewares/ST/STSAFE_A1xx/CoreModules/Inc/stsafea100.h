/**
  ******************************************************************************
  * @file    stsafea100.h
  * @author  SMD/AME application teams
  * @version V3.0.0
  * @brief   STSAFE_A100 device specific include file.
  *          Specifies device specific defines, macros, types.
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

#ifndef STSAFEA100_H
#define STSAFEA100_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stsafea_conf.h"
#include "stsafea_types.h"

/** @addtogroup STSAFE_A1XX_DEVICE_DEF_MACRO_TYPES
  * @{
  */
/** @addtogroup DEVICE_STSAFE_A100
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/** @defgroup STSAFE_A100_Exported_Defines
  * @{
  */

/** @defgroup STSAFE_A100_Commands
  * @brief STSAFE Commands
  * @{
  */
#define STSAFEA_CMD_ECHO                      (0x00U)      /*!< Echo: Returns as a response the data that it received as command data. */
#define STSAFEA_CMD_RESET                     (0x01U)      /*!< Reset: Interrupts any on-going session. */
#define STSAFEA_CMD_GENERATE_RANDOM           (0x02U)      /*!< Generate Random: Returns the requested number of random bytes.*/
#define STSAFEA_CMD_START_SESSION             (0x03U)      /*!< Start Session: Starts a signature session. It must be used in combination with the Get Signature command. */
#define STSAFEA_CMD_DECREMENT                 (0x04U)      /*!< Decrement: Decrements the one-way counter in a counter zone. When the counter reaches zero, the command is refused.*/
#define STSAFEA_CMD_READ                      (0x05U)      /*!< Read: Used to read data from a data partition zone. This command can also be used to change the read access conditions of the zone to a more stricter value. */
#define STSAFEA_CMD_UPDATE                    (0x06U)      /*!< Update: Used to update data in a zone. This command can also be used to change the update access conditions of the zone to a more stricter value*/
#define STSAFEA_CMD_DELETE_KEY                (0x0CU)      /*!< Delete: Used to delete keys. Only Delete Password feature is supported through the available APIs */
#define STSAFEA_CMD_HIBERNATE                 (0x0DU)      /*!< Hibernate: Sets the product in very-low-power consumption mode. The device restart is equivalent to a restart after a reset or Power On Reset */
#define STSAFEA_CMD_WRAP_LOCAL_ENVELOPE       (0x0EU)      /*!< Wrap Local Envelope: This command is used to wrap data (typically working keys that are entirely managed by the local host) with a local key envelope using an AES key wrap algorithm.*/
#define STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE     (0x0FU)      /*!< Unwrap Local Envelope: This command is used to unwrap a local envelope with a local envelope key. */
#define STSAFEA_CMD_PUT_ATTRIBUTE             (0x10U)      /*!< Put Attribute: Used to put attributes in the STSAFE_A100 chips like Keys, a password, the host’s public key or I2C parameters. */
#define STSAFEA_CMD_GENERATE_KEY              (0x11U)      /*!< Generate Key: This command is used to generate key pairs (asymmetric cryptography) or local envelope keys (symmetric cryptography). */
#define STSAFEA_CMD_QUERY                     (0x14U)      /*!< Query: Used to check how the chip is configured.*/
#define STSAFEA_CMD_GET_SIGNATURE             (0x15U)      /*!< Get Signature: This command generates a digital signature over all commands and responses since the start of the signature session. */
#define STSAFEA_CMD_GENERATE_SIGNATURE        (0x16U)      /*!< Generate Signature: This command generates a digital signature over a message digest generated by the host. It is typically used in the IoT device authentication use case.*/
#define STSAFEA_CMD_VERIFY_SIGNATURE          (0x17U)      /*!< Verify Signature: This command serves message authentication and entity authentication purposes. */
#define STSAFEA_CMD_ESTABLISH_KEY             (0x18U)      /*!< Establish Key: This command can be used to establish a shared secret between two hosts by using asymmetric cryptography.*/
#define STSAFEA_CMD_VERIFY_PASSWORD           (0x1AU)      /*!< Verify Password: This command performs password verification and remembers the outcome for future authorization of Put Attribute commands. */
/**
  * @}
  */

/** @defgroup STSAFE_A100_Commands_Wait_Time
  * @brief STSAFEA Commands Waiting Time in ms between command and response
  * @{
  */
#define STSAFEA_MS_WAIT_TIME_CMD_ECHO                     (  5U)
#define STSAFEA_MS_WAIT_TIME_CMD_RESET                    ( 30U)
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_RANDOM          ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_START_SESSION            ( 25U)
#define STSAFEA_MS_WAIT_TIME_CMD_DECREMENT                ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_READ                     (  5U)
#define STSAFEA_MS_WAIT_TIME_CMD_UPDATE                   (  5U)
#define STSAFEA_MS_WAIT_TIME_CMD_HIBERNATE                ( 50U)
#define STSAFEA_MS_WAIT_TIME_CMD_WRAP_LOCAL_ENVELOPE      (200U)
#define STSAFEA_MS_WAIT_TIME_CMD_UNWRAP_LOCAL_ENVELOPE    (400U)
#define STSAFEA_MS_WAIT_TIME_CMD_PUT_ATTRIBUTE            ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_KEY             ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_DELETE_KEY               ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_QUERY                    (  5U)
#define STSAFEA_MS_WAIT_TIME_CMD_GET_SIGNATURE            (450U)
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_SIGNATURE_256   ( 80U)
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_SIGNATURE_384   (140U)
#define STSAFEA_MS_WAIT_TIME_CMD_VERIFY_MSG_SIGNATURE     (145U)
#define STSAFEA_MS_WAIT_TIME_CMD_VERIFY_ENTITY_SIGNATURE  ( 15U)
#define STSAFEA_MS_WAIT_TIME_CMD_ESTABLISH_KEY            (200U)
#define STSAFEA_MS_WAIT_TIME_CMD_VERIFY_PASSWORD          ( 15U)

/**
  * @}STSAFE_A100_Commands_Wait_Time
  */

/** @defgroup STSAFE_A100_Commands_Tag
  * @brief STSAFEA Commands Tag
  * @{
  */
#define STSAFEA_TAG_ADMIN_BASE_KEY_SLOT                 (0x01U)
#define STSAFEA_TAG_SERVICE_BASE_KEY_SLOT               (0x03U)
#define STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE            (0x07U)
#define STSAFEA_TAG_LIFE_CYCLE_STATE                    (0x08U)
#define STSAFEA_TAG_PASSWORD_SLOT                       (0x09U)
#define STSAFEA_TAG_I2C_PARAMETER                       (0x0AU)
#define STSAFEA_TAG_PUBLIC_KEY_SLOT                     (0x0BU)
#define STSAFEA_TAG_LTC_PARAM                           (0x0DU)
#define STSAFEA_TAG_PRODUCT_DATA                        (0x11U)
#define STSAFEA_TAG_DATA_PARTITION_CONFIGURATION        (0x12U)
#define STSAFEA_TAG_PRIVATE_KEY_SLOT                    (0x13U)
#define STSAFEA_TAG_PRIVATE_KEY_TABLE                   (0x14U)
#define STSAFEA_TAG_HOST_KEY_SLOT                       (0x17U)
#define STSAFEA_TAG_TKM_CAPA                            (0x20U)
#define STSAFEA_TAG_TKM_ID                              (0x21U)
#define STSAFEA_TAG_ADMIN_BASE_KEY_SLOT_LIMITS          (0x22U)
#define STSAFEA_TAG_COMMAND_AUTHORIZATION_CONFIGURATION (0x24U)
#define STSAFEA_TAG_SERVICE_BASE_KEY_SLOT_LIMITS        (0x25U)
#define STSAFEA_TAG_APPTKMID                            (0x26U)
/**
  * @}STSAFE_A100_Commands_Tag
  */

/**
  * @}STSAFE_A100_Exported_Defines
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup STSAFE_A100_Exported_Macros
  * @{
  */
#define IS_STSAFEA_TAG(TAG) (((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT)                 || \
                             ((TAG) == STSAFEA_TAG_SERVICE_BASE_KEY_SLOT)               || \
                             ((TAG) == STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE)            || \
                             ((TAG) == STSAFEA_TAG_LIFE_CYCLE_STATE)                    || \
                             ((TAG) == STSAFEA_TAG_PASSWORD_SLOT)                       || \
                             ((TAG) == STSAFEA_TAG_I2C_PARAMETER)                       || \
                             ((TAG) == STSAFEA_TAG_PUBLIC_KEY_SLOT)                     || \
                             ((TAG) == STSAFEA_TAG_LTC_PARAM)                           || \
                             ((TAG) == STSAFEA_TAG_PRODUCT_DATA)                        || \
                             ((TAG) == STSAFEA_TAG_DATA_PARTITION_CONFIGURATION)        || \
                             ((TAG) == STSAFEA_TAG_PRIVATE_KEY_SLOT)                    || \
                             ((TAG) == STSAFEA_TAG_PRIVATE_KEY_TABLE)                   || \
                             ((TAG) == STSAFEA_TAG_HOST_KEY_SLOT)                       || \
                             ((TAG) == STSAFEA_TAG_TKM_CAPA)                            || \
                             ((TAG) == STSAFEA_TAG_TKM_ID)                              || \
                             ((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT_LIMITS)          || \
                             ((TAG) == STSAFEA_TAG_COMMAND_AUTHORIZATION_CONFIGURATION) || \
                             ((TAG) == STSAFEA_TAG_SERVICE_BASE_KEY_SLOT_LIMITS)        || \
                             ((TAG) == STSAFEA_TAG_APPTKMID))
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

#endif /* STSAFEA100_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
