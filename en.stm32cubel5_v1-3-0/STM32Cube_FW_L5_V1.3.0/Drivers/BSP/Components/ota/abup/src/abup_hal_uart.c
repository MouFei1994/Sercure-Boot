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
 * @file        abup_hal_uart.c
 *
 * @brief       Implement uart driver
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#include "abup_typedef.h"
#include "abup_stdlib.h"
#include "abup_hal_uart.h"
#include <mcuboot_config/mcuboot_logging.h>

#if defined(ABUP_ONEOS)
abup_char uart_debug = 1;
abup_char uart_at    = 2;
#elif defined(ABUP_RTT_LOG)
abup_char uart_debug = 1;
#endif

#ifdef ABUP_UARTTX_MAXLEN
#if defined(ABUP_TASK_SCHEDULER)
ABUP_ALIGN(1) abup_char abup_ts_buf[ABUP_UARTTX_MAXLEN] = {0};
#endif
ABUP_ALIGN(1) abup_char ATBuf[ABUP_UARTTX_MAXLEN] = {0};
#endif

#ifdef ABUP_UARTRX_MAXLEN
ABUP_ALIGN(1) abup_char AbupATRxBuf[ABUP_UARTRX_MAXLEN] = {0};
#endif

#if defined(ABUP_BL_PRINT_MAXLEN)
abup_char AbupPrintBuf[ABUP_BL_PRINT_MAXLEN] = {0};
#endif

#ifdef ABUP_USART_DEBUG_AT
ABUP_ALIGN(1) abup_uint8 AbupDebugRxBuf[ABUP_UARTRX_DEBUGLEN] = {0};
abup_uint16 AbupDebugRxBufLen                                 = 0;
abup_char*  abup_get_AbupDebugRxBuf(void)
{
    return (abup_char*)AbupDebugRxBuf;
}
abup_uint16 abup_get_AbupDebugRxBuf_len(void)
{
    return ABUP_UARTRX_DEBUGLEN;
}
#endif

abup_weak void abup_printf(abup_char* data, abup_int len)
{
    MCUBOOT_LOG_INF("%s\r\n",data);
}

abup_weak void abup_moudule_printf(abup_char* data, abup_int len)
{
}

abup_weak void abup_com_send_wait(abup_char* data)
{
}

#ifdef ABUP_UARTTX_MAXLEN
abup_char* abup_get_ATBuf(void)
{
    abup_com_send_wait(ATBuf);
    return (abup_char*)ATBuf;
}

abup_uint16 abup_get_ATBuf_len(void)
{
    return ABUP_UARTTX_MAXLEN;
}
#endif

#ifdef ABUP_UARTRX_MAXLEN
abup_char* abup_get_AbupATRxBuf(void)
{
    return (abup_char*)AbupATRxBuf;
}

abup_uint16 abup_get_AbupATRxBuf_len(void)
{
    return ABUP_UARTRX_MAXLEN;
}
#endif

void abup_kprintf(const char* fmt, ...)
{
#ifdef ABUP_DEBUG_MODE
    abup_int n;
    va_list  args;
#ifdef ABUP_UARTTX_MAXLEN
#if defined(ABUP_TASK_SCHEDULER)
    abup_char* buf = abup_ts_buf;
#else
    abup_char* buf = abup_get_ATBuf();
#endif
    abup_int len = abup_get_ATBuf_len();
#elif defined(ABUP_BL_PRINT_MAXLEN)
    abup_char* buf = AbupPrintBuf;
    abup_int   len = ABUP_BL_PRINT_MAXLEN;
#else
    abup_char  data;
    abup_char* buf = &data;
    abup_int   len = 1;
#endif

    va_start(args, fmt);
    n = abup_vsnprintf(buf, len, fmt, args);
    va_end(args);
    abup_printf(buf, n > len ? len : n);
#elif defined(ABUP_UARTTX_MAXLEN)
    abup_int   n;
    va_list    args;
    abup_char* buf = abup_get_ATBuf();
    abup_int   len = abup_get_ATBuf_len();
    va_start(args, fmt);
    n = abup_vsnprintf(buf, len, fmt, args);
    va_end(args);
    abup_printf(buf, n > len ? len : n);
#endif
}

void abup_bl_main_printf(abup_char* data)
{
    abup_printf(data, abup_strlen(data));
}

abup_int abup_at_send_cmd(const abup_char* fmt, ...)
{
#ifdef ABUP_UARTTX_MAXLEN
    abup_int n;
    va_list  args;
#if defined(ABUP_TASK_SCHEDULER)
    abup_char* buf = abup_ts_buf;
#else
    abup_char* buf = abup_get_ATBuf();
#endif
    abup_int len = abup_get_ATBuf_len();

    va_start(args, fmt);
    n = abup_vsnprintf(buf, len, fmt, args);
    va_end(args);
    abup_at_send_data(buf, n);
    return n;
#else
    return 0;
#endif
}

void abup_at_send_data(abup_char* data, abup_int16 len)
{
    if (len > 0)
    {
        abup_moudule_printf(data, len);
#ifdef ABUP_DEBUG_MODE
        abup_printf(data, len);
#endif
    }
#ifdef ABUP_TIMERS
    AbupReStartTimer(AbupAtpTimer);
#endif
}
