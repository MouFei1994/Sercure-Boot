/**
  ******************************************************************************
  * @file    mapping_fwimg.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for firmware images mapping
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright(c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAPPING_FWIMG_H
#define MAPPING_FWIMG_H

/* Slots Regions must be aligned on 4096 bytes (0x1000) */

/* swap region (4 kbytes) */
#define REGION_SWAP_START                   0x08029000
#define REGION_SWAP_END                     0x08029FFF

/* slot 0 region (36 kbytes) */
#define REGION_SLOT_0_START                 0x08020000
#define REGION_SLOT_0_END                   0x08028FFF

/* slot 1 region (36 kbytes) */
#define REGION_SLOT_1_START                 0x0800F000
#define REGION_SLOT_1_END                   0x08017FFF

#endif
