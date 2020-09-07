/*
*****************************************************************************
**

**  File        : mapping_fwimg.ld
**
**  Abstract    : Firmware images mapping definitions 
**                Common to SECoreBin, SBSFU and UserApp projects
**
**  Target      : STMicroelectronics STM32
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Ac6.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Ac6 permit registered System Workbench for MCU users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the System Workbench for MCU toolchain.
**
*****************************************************************************
*/

/* Slots Regions must be aligned on 4096 bytes (0x1000) */

/* slot 0 region (448 kbytes) */
__ICFEDIT_region_SLOT_0_start__ = 0x08010000;  
__ICFEDIT_region_SLOT_0_end__   = 0x0807FFFF;


