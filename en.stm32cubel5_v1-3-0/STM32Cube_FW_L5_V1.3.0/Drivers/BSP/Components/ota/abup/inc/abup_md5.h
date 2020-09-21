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
 * @file        abup_md5.h
 *
 * @brief       The abup md5 header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_MD5_H__
#define __ABUP_MD5_H__

#include "abup_stdlib.h"

#ifndef MD5_BLOCKSIZE
#define MD5_BLOCKSIZE 64
#endif

typedef struct
{
    abup_uint32 count[2];
    abup_uint32 state[4];
    abup_uint8  buffer[MD5_BLOCKSIZE];
} ABUP_MD5_CTX;

typedef struct
{
    abup_uint8  content1;
    abup_uint8  content2;
    abup_uint32 content3;
} ABUP_MD5_DATA;

#define F(x, y, z)        (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z)        (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z)        ((x) ^ (y) ^ (z))
#define I(x, y, z)        ((y) ^ ((x) | ~(z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, ac)                                                                                       \
    {                                                                                                                  \
        (a) += F((b), (c), (d)) + (x) + (ac);                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                   \
        (a) += (b);                                                                                                    \
    }

#define GG(a, b, c, d, x, s, ac)                                                                                       \
    {                                                                                                                  \
        (a) += G((b), (c), (d)) + (x) + (ac);                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                   \
        (a) += (b);                                                                                                    \
    }

#define HH(a, b, c, d, x, s, ac)                                                                                       \
    {                                                                                                                  \
        (a) += H((b), (c), (d)) + (x) + (ac);                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                   \
        (a) += (b);                                                                                                    \
    }

#define II(a, b, c, d, x, s, ac)                                                                                       \
    {                                                                                                                  \
        (a) += I((b), (c), (d)) + (x) + (ac);                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                   \
        (a) += (b);                                                                                                    \
    }

abup_extern void AbupMD5Init(ABUP_MD5_CTX* context);
abup_extern void AbupMD5Update(ABUP_MD5_CTX* context, abup_uint8* input, abup_uint32 inputlen);
abup_extern void AbupMD5Final(ABUP_MD5_CTX* context, abup_uint8 digest[16]);
abup_extern void AbupMD5Transform(abup_uint32 state[4], abup_uint8 block[64]);
abup_extern void AbupMD5Encode(abup_uint8* output, abup_uint32* input, abup_uint32 len);
abup_extern void AbupMD5Decode(abup_uint32* output, abup_uint8* input, abup_uint32 len);

#endif
