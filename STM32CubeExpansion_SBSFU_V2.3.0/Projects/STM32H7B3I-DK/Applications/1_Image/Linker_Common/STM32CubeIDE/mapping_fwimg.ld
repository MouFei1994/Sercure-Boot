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

/* Slot 0 (1984 kbytes) */
__ICFEDIT_region_SLOT_0_start__ = 0x08010000;  
__ICFEDIT_bank1_secure_end__    = 0x080101FF;  /* Secure User Memory: end of the protected area */      
__ICFEDIT_region_SLOT_0_end__   = 0x081FFFFF;


