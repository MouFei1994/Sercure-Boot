/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *use this file except in compliance with the License. You may obtain a copy of
 *the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *License for the specific language governing permissions and limitations under
 *the License.
 *
 * @file        peer_type.h
 *
 * @brief       The peer type header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __PEER_TYPE_H__
#define __PEER_TYPE_H__

#include "abup_typedef.h"
typedef abup_uint   ML_U32;
typedef abup_int    ML_S32;
typedef abup_uint16 ML_U16;
typedef abup_int16  ML_S16;
typedef abup_uint8  ML_U8;
typedef abup_char   ML_S8;
typedef abup_bool   ML_BOOL;

#define MLMAX(a, b) (((a) > (b)) ? (a) : (b))
#define MLMIN(a, b) (((a) < (b)) ? (a) : (b))

#define FALSE abup_false
#define TRUE  abup_true

#define ML_ALIGNSIZE(x, n) (((x) + (n)-1) & (~((n)-1)))
#define ML_ALIGNSIZE8(x)   ML_ALIGNSIZE(x, 8)
#define ML_ISALIGN(x, n)   (((ML_U32)(x) & (ML_U32)((n)-1)) == 0)

#define ML_FILE_FLAG_INVALID 0
#define ML_FILE_FLAG_NORMAL  1
#define ML_FILE_FLAG_PUSH    2

typedef struct peer_file_stru_tag
{
    ML_U32 fd;
    ML_U16 type;
    ML_U16 flag;
    ML_S8* fname;
    ML_U32 param;
    ML_U32 base_position;
    ML_U32 position;
    ML_U32 length;
    ML_U32 errorcode;
} ML_PEER_FILE_STRU;

typedef ML_PEER_FILE_STRU* ML_PEER_FILE;

#define PEER_FILETYPE_ROM    1
#define PEER_FILETYPE_FLASH  2
#define PEER_FILETYPE_BACKUP 3

#define PATCH_TYPE_ZHUSUN 1
#define PATCH_TYPE_WOSUN  2

#define STREAM_TYPE_NONE 0
#define STREAM_TYPE_BZ   1
#define STREAM_TYPE_LZMA 2

typedef void* (*stream_init_decoder_ptr)(ML_PEER_FILE f);
typedef abup_int (*stream_inf_stream_read_ptr)(void* stream, ML_U8* buf, abup_int len);
typedef void (*stream_inf_stream_end_ptr)(void* stream);

typedef struct
{
    stream_init_decoder_ptr    init_fun;
    stream_inf_stream_read_ptr read_fun;
    stream_inf_stream_end_ptr  end_fun;
} STREAM_INF_FUN;

abup_extern void*    stream_init_bz_decoder(ML_PEER_FILE fp);
abup_extern abup_int stream_bz_read(void* stream, ML_U8* buf, abup_int len);
abup_extern void     stream_bz_end(void* strm);
abup_extern void*    stream_init_lzma_decoder(ML_PEER_FILE fp);
abup_extern abup_int stream_lzma_read(void* stream, ML_U8* buf, abup_int len);
abup_extern void     stream_lzma_end(void* strm);
abup_extern void     peer_setPrePatch(ML_BOOL flag);
abup_extern void     peer_setCheckOldbin(ML_BOOL flag);

#ifdef LINUX
#define ML_INLINE __inline__
#else
#define ML_INLINE inline
#endif

#define restrict

#if defined(PEER_HAVING_MEM_TEST)
#include "peer_mem.h"
#endif

#if defined(WIN32)
#include "windows.h"
#include <assert.h>
#include <io.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#elif defined(LINUX)
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#else
#include "abup_bl_flash.h"
#include "abup_bl_main.h"
#include "abup_stdlib.h"
#include "abup_typedef.h"
#include "stdarg.h"
#endif

#if !defined(WIN32)
typedef struct _SYSTEMTIME
{
    ML_U16 wYear;
    ML_U16 wMonth;
    ML_U16 wDayOfWeek;
    ML_U16 wDay;
    ML_U16 wHour;
    ML_U16 wMinute;
    ML_U16 wSecond;
    ML_U16 wMilliseconds;
} SYSTEMTIME;
#endif

#define ML_SIZE_MAX 0xffffffff

/* Refine the return value according to platform needs */
/* When RETCODE <0, it is forbidden to delete differential packets */
/* When RETCODE >=0, the differential packet can be deleted */
typedef enum RETCODE_FOTA_ENUM
{
    RETCODE_FOTA_WRITE_ERR            = -2,
    RETCODE_FOTA_UPDATE_ERR           = -1,
    RETCODE_FOTA_OK                   = 0,
    RETCODE_FOTA_PROCESS_ERR          = 1,
    RETCODE_FOTA_PROCESS_ERRSYSTEM    = 2,
    RETCODE_FOTA_PROCESS_ERRFORMAT    = 3,
    RETCODE_FOTA_PROCESS_ERRMEM       = 4,
    RETCODE_FOTA_PROCESS_ERRHEADER    = 5,
    RETCODE_FOTA_PROCESS_ERRDATA      = 6,
    RETCODE_FOTA_PROCESS_ERRFILESPACE = 7,
    RETCODE_FOTA_PROCESS_PATCHED      = 8,
    RETCODE_FOTA_PROCESS_ERRPATCHFILE = 9
} RETCODE_FOTA;

#endif    /* __PEER_TYPE_H__ */
