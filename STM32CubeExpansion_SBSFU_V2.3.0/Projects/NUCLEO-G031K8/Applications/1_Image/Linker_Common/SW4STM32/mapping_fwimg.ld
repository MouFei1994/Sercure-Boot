/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32G031K8Tx Device with
**                64KByte FLASH, 8KByte RAM
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

/* Slot 0 region(32 kbytes) */
__ICFEDIT_region_SLOT_0_start__ = 0x0800A000;  
__ICFEDIT_region_SLOT_0_end__   = 0x0800FFFF;

