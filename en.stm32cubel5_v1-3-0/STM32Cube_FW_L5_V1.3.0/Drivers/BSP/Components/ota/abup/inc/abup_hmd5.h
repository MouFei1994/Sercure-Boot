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
 * @file        abup_hmd5.h
 *
 * @brief       The abup hmd5 header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_HMD5_H__
#define __ABUP_HMD5_H__

#define ABUP_HMD5_LEN 34

#include "abup_typedef.h"
#include "abup_md5.h"

#ifndef MD5_DIGESTSIZE
#define MD5_DIGESTSIZE 16
#endif

#ifndef MD5_BLOCKSIZE
#define MD5_BLOCKSIZE 64
#endif

typedef struct
{
    ABUP_MD5_CTX ictx;
    ABUP_MD5_CTX octx;
    abup_char    imd[MD5_DIGESTSIZE];
    abup_char    omd[MD5_DIGESTSIZE];
    abup_char    buf[MD5_BLOCKSIZE];
} ABUP_HMD5_CTX;

abup_extern abup_char* abup_hmd5_pid_psec_mid(abup_char* deviceId, abup_char* deviceSecret, abup_char* mid);
abup_extern abup_char* abup_hmd5_pid_psec_mid(abup_char* deviceId, abup_char* deviceSecret, abup_char* mid);

#endif
