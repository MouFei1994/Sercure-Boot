/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32H753ZITx Device with
**                2048KByte FLASH, 128KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
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

/* Slots Regions must be aligned on sector size */

/* Swap sector in sector 0 of Bank2 : 128 kbytes */  
__ICFEDIT_region_SWAP_start__   = 0x08100000;  
__ICFEDIT_region_SWAP_end__     = 0x0811FFFF;

/* Slot 0 in Sector 1 to 7 of Bank1; 7 x 128 kbytes */
__ICFEDIT_region_SLOT_0_start__ = 0x08020000;  
__ICFEDIT_bank1_secure_end__    = 0x080201FF;  /* Secure User Memory: end of the protected area */      
__ICFEDIT_region_SLOT_0_end__   = 0x080FFFFF;

/* Slot 1 in sector 1 to 7 of Bank2 : 7 * 128 kbytes */
__ICFEDIT_region_SLOT_1_start__ = 0x08120000;  
__ICFEDIT_region_SLOT_1_end__   = 0x081FFFFF;

