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
 * @file        abup_http.h
 *
 * @brief       The abup http header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_HTTP_H__
#define __ABUP_HTTP_H__

#include "abup_typedef.h"
#include "abup_hal_uart.h"

typedef struct
{
    abup_uint8  type[32];
    abup_uint16 content_len;
    abup_uint8* content;
} ABUP_ALIGN(1) abup_http_parameter;
abup_extern abup_int abup_http_content_len;

abup_extern void abup_fota_make_json_request(abup_int8 state, abup_char* ptr, abup_uint len);
abup_extern abup_int8* abup_make_http_data(abup_uint8 state);
abup_extern abup_bool  abup_parse_http_data(abup_char* data, abup_http_parameter* http_parameter, abup_uint16 count);
abup_extern abup_char* abup_is_http_data(abup_char* data, abup_uint16 len);
abup_extern abup_uint  abup_http_callback(abup_uint8 state, abup_char* data, abup_uint len);
void                   abup_http_get_new_version(void);
abup_char*             abup_get_http_server_host(void);
void                   abup_http_report_result(void);
abup_extern            abup_char*
                       abup_get_signptr(abup_char* mid, abup_char* productId, abup_char* productSecret, abup_uint32 utc_time);
abup_extern abup_uint8* abup_get_common_data(void);
abup_extern abup_uint   abup_get_common_data_len(void);
abup_extern abup_bool   abup_get_domain(abup_char* server, abup_char* domain, abup_int16 domain_len, abup_uint* port);

#endif
