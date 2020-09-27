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
 * @file        aubp_hal.h
 *
 * @brief       The hal header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-13   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_HAL_H__
#define __ABUP_HAL_H__

#include "abup_stdlib.h"
#include "flash_map.h"

#if !defined(ABUP_BOOTLOADER)
#if defined(ABUP_ONEOS)
#else
#include "abup_at_device.h"
#endif
#include "abup_http.h"
#include "abup_coap.h"
#include "abup_md5.h"
#endif

#if defined(ABUP_AUTO_TEST_FOTA)
#include <stdlib.h>
#endif

#define ABUP_MAX_DELAY    (abup_uint32)0xFFFFFFFFU
#define ABUP_TOKEN_MAXLEN 16

#if (ABUP_HAL_PROTOCOL_MAX_LEN < 256)
#define ABUP_HAL_DATA_MAX_LEN 256
#else
#define ABUP_HAL_DATA_MAX_LEN ABUP_HAL_PROTOCOL_MAX_LEN
#endif

#define ABUP_DOWNLOAD_MD5_LEN 16
#if defined(ABUP_SLIM_RES)
#define ABUP_DOWNLOAD_HOST_MAX_LEN 32
#define ABUP_DOWNLOAD_URI_MAX_LEN  80
#else
#define ABUP_DOWNLOAD_HOST_MAX_LEN 64
#define ABUP_DOWNLOAD_URI_MAX_LEN  128
#endif

#define ABUP_AT_IP_MAXLEN 4

#if (ABUP_DOWNLOAD_HOST_MAX_LEN < 32)
#define ABUP_BUF_MAX_LEN 32
#else
#define ABUP_BUF_MAX_LEN ABUP_DOWNLOAD_HOST_MAX_LEN
#endif

#define ABUP_SN_VER_MAXLEN 4

#if defined(ABUP_SLIM_RES) && ((ABUP_KEY_VERSION == 2) || (ABUP_KEY_VERSION == 3))
#define ABUP_HASH_MAXLEN 4
#else
#define ABUP_HASH_MAXLEN 32
#endif

typedef enum
{
    ABUP_FLASH_OPERATION_READ,
    ABUP_FLASH_OPERATION_WRITE,
    ABUP_FLASH_OPERATION_ERASE,
    ABUP_FLASH_OPERATION_END
} ABUP_FLASH_OPERATION_TYPE;

#if defined(ABUP_AUTO_TEST_FOTA)
#define ABUP_AUTO_TEST_FOTA_COUNT   4
#define ABUP_AUTO_TEST_FOTA_VER_LEN 8

typedef enum
{
    ABUP_AUTO_TEST_MODULE_ERROR_TYPE_1,
    ABUP_AUTO_TEST_MODULE_ERROR_TYPE_END
} ABUP_AUTO_TEST_MODULE_ERROR_TYPE;

typedef enum
{
    ABUP_AUTO_TEST_NET_ERROR_TYPE_MD5_ERROR,
    ABUP_AUTO_TEST_NET_ERROR_TYPE_DELTA_WRITE_ERROR,
    ABUP_AUTO_TEST_NET_ERROR_TYPE_NO_RESPOND,
    ABUP_AUTO_TEST_NET_ERROR_TYPE_COMMUNICATION_FAILURE,
    ABUP_AUTO_TEST_NET_ERROR_TYPE_END = ABUP_AUTO_TEST_NET_ERROR_TYPE_COMMUNICATION_FAILURE + 16
} ABUP_AUTO_TEST_NET_ERROR_TYPE;

typedef struct
{
    abup_uint32 addr;
    abup_uint   size;
    abup_int    fail_count;
} ABUP_ALIGN(1) test_flash_struct;

typedef struct
{
    abup_char         ver[ABUP_AUTO_TEST_FOTA_VER_LEN];
    abup_int          cv_succ_count;
    abup_int          update_fail_count;
    abup_int          rpt_succ_count;
    abup_uint8        module_error[ABUP_AUTO_TEST_MODULE_ERROR_TYPE_END];
    abup_uint8        net_error[ABUP_AUTO_TEST_NET_ERROR_TYPE_END];
    test_flash_struct operation[ABUP_FLASH_OPERATION_END];
} ABUP_ALIGN(1) abup_test_struct;

typedef struct
{
    abup_uint8       mid[ABUP_MID_MAXLEN + 1];
    abup_uint8       deviceId[ABUP_DEVICEID_MAX_LEN + 1];
    abup_uint8       deviceSecret[ABUP_DEVICESECRET_MAX_LEN + 1];
    abup_int         auto_test_count;
    abup_int         auto_test_error_count;
    abup_int         auto_test_cv_succ_count;
    abup_int         auto_test_rpt_succ_count;
    abup_int         error_code;
    abup_char        current_ver[ABUP_AUTO_TEST_FOTA_VER_LEN];
    abup_test_struct auto_test[ABUP_AUTO_TEST_FOTA_COUNT];
} ABUP_ALIGN(1) abup_autotest_struct;
#endif

typedef struct
{
    abup_uint8  inited : 4;
    abup_uint8  serverNO : 4;
    abup_uint16 update_result;
    abup_uint8  mid[ABUP_MID_MAXLEN + 1];
    abup_uint8  code[32];
    abup_uint8  deviceId[ABUP_DEVICEID_MAX_LEN + 1];
    abup_uint8  deviceSecret[ABUP_DEVICESECRET_MAX_LEN + 1];
    abup_uint   index;
    abup_uint   index_max;
    abup_uint32 delta_id;
    abup_char   download_md5[ABUP_DOWNLOAD_MD5_LEN + 1];
    abup_uint   default_module;
#if defined(ABUP_WIFI_SSID_PWD)
    abup_char wifi_ssid[ABUP_WIFI_SSID_MAXLEN + 1];
    abup_char wifi_pwd[ABUP_WIFI_PWD_MAXLEN + 1];
#endif
#if defined(ABUP_AUTO_TEST_FOTA)
    abup_autotest_struct autotest;
#endif
} ABUP_ALIGN(1) abup_update_struct;

typedef struct
{
    abup_char download_host[ABUP_DOWNLOAD_HOST_MAX_LEN + 1];
#if !defined(ABUP_SLIM_RES)
    abup_char bkup_host[ABUP_DOWNLOAD_HOST_MAX_LEN + 1];
#endif
    abup_uint8  download_host_ip[ABUP_AT_IP_MAXLEN + 1];
    abup_char   download_url[ABUP_DOWNLOAD_URI_MAX_LEN + 1];
    abup_char   download_md5[ABUP_DOWNLOAD_MD5_LEN + 1];
    abup_uint32 download_delta_id;
    abup_uint32 download_delta_size;
    abup_uint   download_port;
} ABUP_ALIGN(1) download_uri_struct;

typedef enum
{
    ABUP_RESULT_FAIL = 0,
    ABUP_RESULT_CV_SUCC,
    ABUP_RESULT_CV_FAIL,
    ABUP_RESULT_RPT_SUCC,
    ABUP_RESULT_RPT_FAIL,
    ABUP_RESULT_PROGRESS,
    ABUP_RESULT_END,
} ABUP_RESULT;

typedef enum
{
    ABUP_HAL_FLAG_START,
    ABUP_HAL_FLAG_1   = 1,
    ABUP_HAL_FLAG_2   = 2,
    ABUP_HAL_FLAG_4   = 4,
    ABUP_HAL_FLAG_8   = 8,
    ABUP_HAL_FLAG_ALL = ABUP_HAL_FLAG_1 | ABUP_HAL_FLAG_2 | ABUP_HAL_FLAG_4 | ABUP_HAL_FLAG_8,
    /* do not add message id at here */
} ABUP_HAL_FLAG;

typedef struct
{
    abup_uint8 result;
    abup_uint  state;
} ABUP_ALIGN(1) abup_action_result_st;

typedef void (*abup_msg_cb)(void* ptr);
typedef void (*abup_state_result)(abup_uint8 state);
abup_extern abup_action_result_st abup_action_result;
abup_extern volatile abup_bool    abup_update_enable;
abup_extern volatile abup_uint32  abup_tick;

abup_extern abup_msg_cb abup_app_msg_cb;
abup_extern abup_msg_cb abup_atp_msg_cb;

abup_extern void Abup_Delay(abup_uint32 Delay);

#if defined(USE_FULL_LL_DRIVER) || defined(USE_HAL_DRIVER)
abup_extern void AbupGetChipUniqueID(abup_uint32* UID);
#endif

abup_extern void Abup_IncTick(void);

#ifndef ABUP_BOOTLOADER
abup_extern void Abup_AT_Uart_Recv(abup_uint8 ch);
#ifdef ABUP_USART_DEBUG_AT
abup_extern void Abup_Debug_Uart_Recv(abup_uint8 ch);
#endif
#endif

abup_extern abup_uint32 Abup_GetTick(void);
abup_extern void        abup_hal_parse_dns(abup_char* host, abup_uint8* ip);

abup_extern download_uri_struct* abup_get_download_atp_uri(void);
abup_extern void                 abup_hal_update_device(abup_uint8* mid,
                                                        abup_int16  mid_len,
                                                        abup_uint8* deviceId,
                                                        abup_int16  deviceIdLen,
                                                        abup_uint8* deviceSecret,
                                                        abup_int16  deviceSecretLen);
abup_extern abup_int abup_hal_flash_read(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size);
abup_extern abup_int abup_hal_flash_write(abup_uint8 type, abup_uint32 addr, abup_uint8* buf, abup_uint size);
abup_extern abup_int abup_hal_flash_erase(abup_uint8 type, abup_uint32 addr, abup_uint size);
abup_uint32          abup_hal_get_addr(abup_uint8 type, abup_uint32 addr);
abup_uint            abup_hal_get_blocksize(void);
abup_uint            abup_hal_get_true_blocksize(abup_uint8 type);
abup_uint32          abup_hal_get_info_addr(void);
abup_uint32          abup_hal_get_backup_addr(void);
abup_uint32          abup_hal_get_app_addr(void);
abup_uint32          abup_hal_get_delta_addr(void);
abup_uint32          abup_hal_get_delta_size(void);
abup_extern abup_uint8* abup_get_hal_data(void);
abup_extern void        abup_reset_hal_data(void);
abup_extern abup_uint   abup_get_hal_data_len(void);
abup_extern abup_bool   abup_hal_write_delta(abup_uint16 index, abup_uint8* data, abup_uint16 len);
abup_extern abup_bool   abup_hal_erase_sector(abup_uint8 type, abup_uint32 addr);
abup_extern abup_bool   abup_hal_get_inited(void);
abup_extern void        AbupHalDeltaWrite(abup_bool inited, abup_bool succ);

abup_extern void       Abup_UartRecved(void);
abup_extern void       abup_hal_app_msg(void* ptr);
abup_extern void       abup_hal_start(abup_msg_cb app_msg_cb, abup_msg_cb atp_msg_cb, abup_state_result state_result);
abup_extern abup_bool  abup_hal_started(void);
abup_extern void       abup_hal_stop(void);
void                   abup_display_update(abup_bool enable);
abup_extern abup_uint  abup_get_download_index(void);
abup_extern abup_uint  abup_get_download_index_max(void);
abup_extern abup_uint  abup_md5_calc_result;
abup_extern abup_uint  download_index;
abup_extern abup_uint  download_index_max;
abup_extern abup_uint8 abup_current_token[ABUP_TOKEN_MAXLEN];
abup_extern abup_uint8 abup_current_token_len;
abup_extern abup_int8  abup_get_conn_try_count(void);
abup_extern void       abup_set_conn_try_count(abup_int8);
abup_extern abup_uint8* abup_get_str_ip(abup_uint8* ip);
abup_extern abup_update_struct* abup_hal_init_update(void);
abup_extern abup_update_struct* abup_hal_rst_fota(void);
abup_extern abup_update_struct* abup_hal_get_update(void);
abup_extern abup_bool           abup_hal_set_update(abup_update_struct* abup_update);

#if defined(ABUP_BOOTLOADER)
#else
#if defined(ABUP_WIFI_SSID_PWD)
abup_bool   abup_hal_check_ssid_pwd(abup_char* ssid, abup_uint16 ssid_len, abup_char* pwd, abup_uint16 pwd_len);
#endif
abup_bool   abup_hal_get_state(void);
abup_char*  abup_get_server_host(void);
abup_bool   abup_hal_para_url(abup_char* url, abup_uint16 url_len);
abup_uint*  abup_get_host_port(void);
abup_bool   abup_hal_para_http(abup_uint8* data, abup_http_parameter* tmp, abup_uint8 tmplen);
abup_uint   abup_MD5Calc_result(void);
abup_int    abup_MD5Calc(abup_uint buflen, abup_char* md5out);
abup_int8   abup_hal_get_current_state(void);
abup_uint8* abup_hal_get_download_host_ip(void);
abup_uint32 abup_hal_get_download_delta_id(void);
abup_uint32 abup_hal_get_download_delta_size(void);
abup_char*  abup_hal_get_download_url(void);
abup_char*  abup_hal_get_download_host(void);
abup_uint32 abup_hal_get_delta_id(void);
abup_uint8* abup_hal_get_deviceId(void);
abup_uint8* abup_hal_get_deviceSecret(void);
#endif

abup_int    abup_hal_get_key_version(void);
abup_bool   abup_hal_check_key_version(void);
abup_bool   abup_hal_server_support_key(void);
abup_uint8  abup_hal_get_serverNO(void);
abup_uint8* abup_hal_get_mid(void);
abup_uint16 abup_hal_get_update_result(void);
void        abup_hal_set_update_result(abup_uint16 result);
void        abup_hal_upgrade_success(void);
void        abup_hal_upgrade_fail(void);

#endif
