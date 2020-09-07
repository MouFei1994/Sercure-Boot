/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32H7B3LIHxQ Device with
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
*****************************************************************************
*/

/* Slots Regions must be aligned on sector size */

/* Swap sector : 128 kbytes */  
__ICFEDIT_region_SWAP_start__   = 0x90800000;  
__ICFEDIT_region_SWAP_end__     = 0x9081FFFF;

/* Slot 0 : 4 MBytes */
__ICFEDIT_region_SLOT_0_header__ = 0x08020000;  
__ICFEDIT_bank1_secure_end__    = 0x080201FF;  /* Secure User Memory: end of the protected area */      
__ICFEDIT_region_SLOT_0_start__ = 0x90000000;  
__ICFEDIT_region_SLOT_0_end__   = 0x903FFFFF;

/* Slot 1 : 4 MBytes */
__ICFEDIT_region_SLOT_1_start__ = 0x90400000;  
__ICFEDIT_region_SLOT_1_end__   = 0x907FFFFF;

