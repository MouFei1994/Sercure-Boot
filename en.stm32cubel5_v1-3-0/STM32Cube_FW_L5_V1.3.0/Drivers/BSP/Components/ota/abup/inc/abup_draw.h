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
 * @file        abup_draw.h
 *
 * @brief       The abup draw header file
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-16   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __ABUP_DRAW_H__
#define __ABUP_DRAW_H__

#include "abup_typedef.h"

#if !defined(ABUP_BOOTLOADER) && defined(STM32F4_DISCO)
#include "abup_stdlib.h"
#define ABUP_LCD_BG_COLOR          GUI_LIGHTYELLOW    /* 0xb2c7ca */
#define ABUP_LCD_TXT_COLOR         GUI_RED
#define ABUP_LCD_TXT_DISABLE_COLOR GUI_GRAY
#include "GUI.h"
void AbupDisplayString(abup_int16 Line, const abup_char* content, ...);
void AbupDisplayClearLine(abup_int16 Line);
#elif defined(STM32L0_DISCO)
#include "stm32l0538_discovery_epd.h"
#define STM32L053C8_EPD_MAX_LINE         4
#define STM32L053C8_EPD_MAX_LINE_DISCHAR 40
#define AbupDisplayString(Line, CONTENT, ...)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        abup_memset(dis[Line], 0, sizeof(dis[Line]));                                                                  \
        abup_snprintf(dis[Line], sizeof(dis[Line]), CONTENT, ##__VA_ARGS__);                                           \
        AbupDrawString();                                                                                              \
    } while (0);
abup_extern abup_char dis[STM32L053C8_EPD_MAX_LINE][STM32L053C8_EPD_MAX_LINE_DISCHAR];
abup_extern void      AbupDrawString(void);
abup_extern void      AbupDisplayClearLine(abup_uint Line);
#else
#define AbupDisplayString(Line, CONTENT, ...)
#define AbupDisplayClearLine(Line)
#endif

#endif
