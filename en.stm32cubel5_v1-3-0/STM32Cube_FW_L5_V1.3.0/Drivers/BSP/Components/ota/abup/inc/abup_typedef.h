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
 * @file        abup_typedef.h
 *
 * @brief       The abup typedef header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_TYPEDEF_H__
#define __ABUP_TYPEDEF_H__

#include "abup_config.h"

#ifndef abup_extern
#ifdef __cplusplus
#define abup_extern extern "C"
#else
#define abup_extern extern
#endif
#endif /* abup_extern */

/* get data size by index */
#define ABUP_DATA_SEQ_MAX_LEN (1 << (ABUP_DEFAULT_SEGMENT_SIZE_INDEX + 4))
#define ABUP_HTTP_HEAD_LEN    640
#define ABUP_HTTP_MAX_LEN     (ABUP_DATA_SEQ_MAX_LEN + ABUP_HTTP_HEAD_LEN)
#define ABUP_COAP_MAX         (ABUP_DATA_SEQ_MAX_LEN + 32)
#define ABUP_COAP_MIN         256

#if (ABUP_COAP_MAX < ABUP_COAP_MIN)
#define ABUP_COAP_MAX_LEN ABUP_COAP_MIN
#else
#define ABUP_COAP_MAX_LEN ABUP_COAP_MAX
#endif

#if defined(__GNUC__)
#ifndef abup_weak
#define abup_weak __attribute__((weak))
#endif                            /* abup_weak */
#define ABUP_ASM           __asm  /* !< asm keyword for GNU Compiler */
#define ABUP_INLINE        inline /* !< inline keyword for GNU Compiler */
#define ABUP_STATIC_INLINE static inline
#define ABUP_ALIGN(x)      __attribute__((aligned(x)))
#elif defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCSTM8__)
#ifndef abup_weak
#define abup_weak __weak
#endif                            /* abup_weak */
#define ABUP_ASM           __asm  /* !< asm keyword for IAR Compiler */
#define ABUP_INLINE        inline /* !< inline keyword for IAR Compiler. Only available in High optimization mode! */
#define ABUP_STATIC_INLINE static inline
#define ABUP_ALIGN(x)      __attribute__((aligned(x)))
#elif defined(__CC_ARM)
#ifndef abup_weak
#define abup_weak __weak
#endif                              /* abup_weak */
#define ABUP_ASM           __asm    /* !< asm keyword for ARM Compiler */
#define ABUP_INLINE        __inline /* !< inline keyword for ARM Compiler */
#define ABUP_STATIC_INLINE static __inline
#define ABUP_ALIGN(x)      __align(x)
#else
#error "Alignment not supported for this compiler."
#endif

typedef enum
{
    ABUP_FILETYPE_APP,
    ABUP_FILETYPE_PATCH,
    ABUP_FILETYPE_BACKUP,
    ABUP_FILETYPE_PATCH_INFO,
    ABUP_FILETYPE_END
} ABUP_PEER_FILETYPE;

#define ABUPSW32(x)                                                                                                    \
    ((((abup_uint32)(x) & (abup_uint32)0x000000ff) << 24) | (((abup_uint32)(x) & (abup_uint32)0x0000ff00) << 8) |      \
     (((abup_uint32)(x) & (abup_uint32)0x00ff0000) >> 8) | (((abup_uint32)(x) & (abup_uint32)0xff000000) >> 24))

#define ABUP_UPDATE_SUCCESS   1
#define ABUP_UPDATE_AUTH_FAIL 98
#define ABUP_UPDATE_FAIL      99
#define ABUP_NO_UPDATE_RESULT 1000

#define ABUP_MID_MAXLEN           31
#define ABUP_DEVICEID_MAX_LEN     31
#define ABUP_DEVICESECRET_MAX_LEN 63
#define ABUP_CRC_MAXLEN           4
#define ABUP_VERSION_MAXLEN       4
#define ABUP_SN_DATETIME_MAXLEN   22

#if defined(ABUP_WIFI_SSID_PWD)
#define ABUP_WIFI_SSID_MAXLEN 32
#define ABUP_WIFI_PWD_MAXLEN  32
#endif

typedef char               abup_char;
typedef unsigned short     abup_wchar;
typedef unsigned char      abup_uint8;
typedef signed char        abup_int8;
typedef unsigned short int abup_uint16;
typedef signed short int   abup_int16;
typedef unsigned int       abup_uint;
typedef signed int         abup_int;
typedef unsigned long      abup_ulong;
typedef signed long        abup_long;
typedef unsigned long long abup_ull;
typedef signed long long   abup_ll;

#ifdef ABUP_8BIT
typedef unsigned long      abup_uint32;
typedef signed long        abup_int32;
typedef unsigned long long abup_uint64;
typedef signed long long   abup_int64;
#else
typedef unsigned int       abup_uint32;
typedef signed int         abup_int32;
typedef unsigned long long abup_uint64;
typedef signed long long   abup_int64;
#endif

typedef unsigned int abup_size_t;
typedef signed int   abup_intptr_t;
typedef unsigned int abup_uintptr_t;

typedef enum
{
    STATE_INIT = 1,
    STATE_CV, /* Check Version */
    STATE_DL, /* Download*/
    STATE_RD, /* Report download result*/
    STATE_UG,
    STATE_RU, /* Report upgrade result */
    STATE_KY,
    STATE_RG, /* Register */
    STATE_END
} FotaState;

#define E_ABUP_SUCCESS             0
#define E_ABUP_FAILURE             -1
#define E_ABUP_NOMEMORY            -2
#define E_ABUP_NOTEXIST            -3
#define E_ABUP_WOULDBLOCK          -4
#define E_ABUP_SIM_NOTEXIST        -5
#define E_ABUP_MID_ERROR           -6
#define E_ABUP_NETWORK_ERROR       -7
#define E_ABUP_DOWNLOAD_FAIL       -8
#define E_ABUP_LAST_VERSION        -9
#define E_ABUP_TOKEN_WORNG         -10
#define E_ABUP_PROJECT_WORONG      -11
#define E_ABUP_PARAM_WORONG        -12
#define E_ABUP_NET_CONNECTING      -13
#define E_ABUP_DOWNLOAD_CONNECTING -14

typedef enum
{
    ABUP_DIFF_PATCH,
    ABUP_FULL_PATCH,
    ABUP_PATCH_END
} ABUP_PATCH_METHOD;

typedef abup_uint8 abup_bool;
#define abup_true  ((abup_bool)1)
#define abup_false ((abup_bool)0)

abup_char* abup_get_manufacturer(void);
abup_char* abup_get_model_number(void);
abup_char* abup_get_product_id(void);
abup_char* abup_get_product_sec(void);
abup_char* abup_get_device_type(void);
abup_char* abup_get_platform(void);
abup_char* abup_get_sdk_version(void);
abup_char* abup_get_apk_version(void);
abup_char* abup_get_firmware_version(void);
abup_char* abup_get_hw_version(void);
abup_char* abup_get_sw_version(void);
abup_char* abup_get_network_type(void);
abup_uint  abup_get_try_timer(void);
abup_int8  abup_get_try_count(void);
abup_int32 abup_get_utc_time(void);
abup_int32 abup_get_downStart_time(void);
abup_int32 abup_get_downEnd_time(void);
abup_uint  abup_get_default_module(void);
abup_bool  abup_set_default_module(abup_uint moudule);
abup_uint8 abup_get_default_protocol(void);
abup_uint8 abup_get_default_segment_size(void);
abup_uint  abup_get_data_max_len(void);
abup_char* abup_get_buf(void);
void       abup_reset_buf(void);
abup_uint  abup_get_buf_len(void);

#endif
