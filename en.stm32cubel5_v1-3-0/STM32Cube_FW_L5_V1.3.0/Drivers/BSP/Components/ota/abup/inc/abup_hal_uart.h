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
 * @file        abup_hal_uart.h
 *
 * @brief       The abup hal uart header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_HAL_UART__
#define __ABUP_HAL_UART__

#include "abup_stdlib.h"
#include "abup_at_device.h"


#ifdef ABUP_TIMERS
#include "abup_timer.h"
#endif

#ifdef ABUP_TIMERS
abup_extern AbupTimerHandle_t AbupAtpTimer;
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG NULL
#endif

abup_extern void abup_bl_main_printf(abup_char* data);

#ifdef ABUP_BL_PRINT_MAXLEN
#define abup_bl_debug_printf   abup_kprintf
#define abup_full_debug_printf abup_kprintf
#define abup_bl_full_printf    abup_kprintf
#define abup_bl_debug_printf   abup_kprintf
#define abup_bl_debug_print(TAG, CONTENT, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        abup_kprintf(CONTENT, ##__VA_ARGS__);                                                                          \
    } while (0);
#define peer_logout abup_kprintf
#else
#define abup_bl_debug_printf(...)
#define abup_full_debug_printf(...)
#define abup_bl_full_printf(...)
#define abup_bl_debug_printf(...)
#define abup_bl_debug_print(...)
#define peer_logout(...)
#endif

#ifdef ABUP_UARTTX_MAXLEN
abup_extern abup_char ATBuf[ABUP_UARTTX_MAXLEN];
#endif

#if (ABUP_APP_DEBUG == 1)
#define ABUP_USART_DEBUG_AT
#endif

#if !defined(ABUP_USART_DEBUG_BAUDRATE)
#define ABUP_USART_DEBUG_BAUDRATE 115200
#endif

#ifdef ABUP_UARTRX_MAXLEN
abup_extern abup_char AbupATRxBuf[ABUP_UARTRX_MAXLEN];
#endif

#ifdef ABUP_USART_DEBUG_AT
#define ABUP_UARTRX_DEBUGLEN 255
abup_extern abup_uint8  AbupDebugRxBuf[ABUP_UARTRX_DEBUGLEN];
abup_extern abup_uint16 AbupDebugRxBufLen;
abup_extern abup_char*  abup_get_AbupDebugRxBuf(void);
abup_extern abup_uint16 abup_get_AbupDebugRxBuf_len(void);
#endif

#ifdef ABUP_DEBUG_MODE
#define abup_debug_printf abup_kprintf
#define abup_debug_override_function()                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        static abup_bool first = abup_true;                                                                            \
        if (first)                                                                                                     \
        {                                                                                                              \
            ABUP_DEBUG(__func__, "function can be rewritten");                                                         \
            first = abup_false;                                                                                        \
        }                                                                                                              \
    } while (0)
#else
#define abup_debug_printf(...)
#define abup_debug_override_function(...)
#endif

#define ABUP_TAG "Abup"
#define ABUP_INFO(TAG, CONTENT, ...)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        abup_kprintf("[%s] " CONTENT "\r\n", TAG, ##__VA_ARGS__);                                                      \
    } while (0)
#define ABUP_DEBUG(TAG, CONTENT, ...)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        abup_debug_printf("\r\n[%s] " CONTENT "\r\n", TAG, ##__VA_ARGS__);                                             \
    } while (0)
abup_extern void     abup_kprintf(const char* fmt, ...);
abup_extern abup_int abup_at_send_cmd(const abup_char* fmt, ...);
abup_extern void     abup_at_send_data(abup_char* data, abup_int16 len);
abup_extern abup_char* abup_get_ATBuf(void);
abup_extern abup_char*  abup_get_AbupATRxBuf(void);
abup_extern abup_uint16 abup_get_ATBuf_len(void);
abup_extern abup_uint16 abup_get_AbupATRxBuf_len(void);

#endif
