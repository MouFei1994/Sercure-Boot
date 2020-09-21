/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        abup_at_device.h
 *
 * @brief       The abup at device header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __AT_DEVICE_H__
#define __AT_DEVICE_H__
#include "abup_config.h"

#ifndef ABUP_BOOTLOADER
#ifdef ABUP_MODULE_BC28_ENABLE
#include "abup_bc28.h"
#endif
#ifdef ABUP_MODULE_ESP07S_ENABLE
#include "abup_esp07s.h"
#endif
#ifdef ABUP_MODULE_ESP8266_ENABLE
#include "abup_esp8266.h"
#endif
#ifdef ABUP_MODULE_M5310_ENABLE
#include "abup_m5310.h"
#endif
#ifdef ABUP_MODULE_ME3630_ENABLE
#include "abup_me3630.h"
#endif
#ifdef ABUP_MODULE_SIM7020C_ENABLE
#include "abup_sim7020c.h"
#endif
#ifdef ABUP_MODULE_SIM7600_ENABLE
#include "abup_sim7600.h"
#endif
#ifdef ABUP_MODULE_SLM152_ENABLE
#include "abup_slm152.h"
#endif

#ifndef ABUP_DEFAULT_HTTP_PROTOCOL
#define ABUP_DEFAULT_HTTP_PROTOCOL ABUP_PROTOCOL_NONE
#endif
#ifndef ABUP_DEFAULT_COAP_PROTOCOL
#define ABUP_DEFAULT_COAP_PROTOCOL ABUP_PROTOCOL_NONE
#endif
#ifndef ABUP_DEFAULT_NETWORK_PROTOCOL
#define ABUP_DEFAULT_NETWORK_PROTOCOL (ABUP_DEFAULT_HTTP_PROTOCOL + ABUP_DEFAULT_COAP_PROTOCOL)
#endif

#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#define ABUP_UARTRX_MAXLEN ABUP_HTTP_MAX_LEN
#define ABUP_UARTTX_MAXLEN (448)
#elif (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP)
#if ((ABUP_COAP_MAX_LEN * 2 + 100) > ABUP_HTTP_MAX_LEN)
#define ABUP_UARTRX_MAXLEN (ABUP_COAP_MAX_LEN * 2 + 100)
#else
#define ABUP_UARTRX_MAXLEN ABUP_HTTP_MAX_LEN
#endif
#if (ABUP_UARTRX_MAXLEN > 512)
#define ABUP_UARTTX_MAXLEN ABUP_UARTRX_MAXLEN
#else
#define ABUP_UARTTX_MAXLEN (400)
#endif
#else
#define ABUP_UARTRX_MAXLEN (ABUP_COAP_MAX_LEN * 2 + 100)
#define ABUP_UARTTX_MAXLEN 512
#endif

#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#define ABUP_HAL_PROTOCOL_MAX_LEN (300 + ABUP_DATA_SEQ_MAX_LEN)
#elif (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP)
#if (ABUP_COAP_MAX_LEN > (300 + ABUP_DATA_SEQ_MAX_LEN))
#define ABUP_HAL_PROTOCOL_MAX_LEN (ABUP_COAP_MAX_LEN)
#else
#define ABUP_HAL_PROTOCOL_MAX_LEN (300 + ABUP_DATA_SEQ_MAX_LEN)
#endif
#else
#define ABUP_HAL_PROTOCOL_MAX_LEN (ABUP_COAP_MAX_LEN)
#endif

#else
#if defined(ABUP_DEBUG_MODE)
#ifndef ABUP_FOTA_ENABLE_DIFF_DEBUG
#define ABUP_FOTA_ENABLE_DIFF_DEBUG
#endif
#ifndef ABUP_FOTA_ENABLE_DIFF_ERROR
#define ABUP_FOTA_ENABLE_DIFF_ERROR
#endif
#ifndef ABUP_BL_PRINT_MAXLEN
#define ABUP_BL_PRINT_MAXLEN 512
#endif
#endif
#endif

#endif /* __AT_DEVICE_H__ */
