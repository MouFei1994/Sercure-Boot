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

/* Secure User Memory protection starts from here (and goes up to the end of the FW header in slot #0) */
__ICFEDIT_bank1_secure_start__             = 0x08000000;  /* Secure User Memory: beginning of the protected area */
VECTOR_SIZE = 0x400;

/* SE Code region protected by MPU isolation */
__ICFEDIT_SE_Code_region_ROM_start__       = 0x08000000 + VECTOR_SIZE;
__ICFEDIT_SE_CallGate_region_ROM_start__   = __ICFEDIT_SE_Code_region_ROM_start__ + 0x4;
__ICFEDIT_SE_CallGate_region_ROM_end__     = __ICFEDIT_SE_Code_region_ROM_start__ + 0x1FF;

/* SE key region protected by MPU isolation */
__ICFEDIT_SE_Key_region_ROM_start__        = __ICFEDIT_SE_CallGate_region_ROM_end__ + 0x1;
__ICFEDIT_SE_Key_region_ROM_end__          = __ICFEDIT_SE_Key_region_ROM_start__ + 0x2FF;  /* H7: The minimum PCROP area that can be set is 16 Flash words, that is 512 bytes. */

/* SE Startup */
__ICFEDIT_SE_Startup_region_ROM_start__    = __ICFEDIT_SE_Key_region_ROM_end__ + 0x1;
__ICFEDIT_SE_Code_nokey_region_ROM_start__ = __ICFEDIT_SE_Startup_region_ROM_start__ + 0x100;
/* Aligned SE End at the end of the 1st 24Kbytes of flash, MPU protection isolation constraints */
__ICFEDIT_SE_Code_region_ROM_end__         = 0x08005FFF;

/* SE IF ROM: used to locate Secure Engine interface code out of MPU isolation     */
/* 
 * We must be out of Region 2 to allow unprivileged access 
 * With the current settings Region 2 starts from __ICFEDIT_bank1_secure_start__ and covers 24kB of code (0x6000)
 */
__ICFEDIT_SE_IF_region_ROM_start__         = __ICFEDIT_bank1_secure_start__ + 0x6000; 
__ICFEDIT_SE_IF_region_ROM_end__           = __ICFEDIT_SE_IF_region_ROM_start__ + 0x8FF;

/* SBSFU Code region */
__ICFEDIT_SB_region_ROM_start__            = __ICFEDIT_SE_IF_region_ROM_end__ + 0x1;
__ICFEDIT_SB_region_ROM_end__              = __ICFEDIT_SB_region_ROM_start__ + 0xAFFF;

/* LOADER code region : alignment on 0x400 (VECTOR_SIZE) for LOADER vector table */
__ICFEDIT_LOADER_region_ROM_start__        = (((__ICFEDIT_SB_region_ROM_end__ / VECTOR_SIZE) + 1) * VECTOR_SIZE);
/* Aligned LOADER end at the end of the 1st 128Kbytes of FLASH, MPU protection isolation constraints */
__ICFEDIT_LOADER_region_ROM_end__          = 0x0801FFFF;

SE_Entry_Secure_ROM_Region_Length = __ICFEDIT_SE_CallGate_region_ROM_end__ - __ICFEDIT_SE_CallGate_region_ROM_start__ + 1;
SE_Key_region_ROM_Length          = __ICFEDIT_SE_Key_region_ROM_end__ - __ICFEDIT_SE_Key_region_ROM_start__ + 1;
SE_Startup_region_ROM_Length      = __ICFEDIT_SE_Code_nokey_region_ROM_start__ - __ICFEDIT_SE_Startup_region_ROM_start__ ;
SE_ROM_region_Length              = __ICFEDIT_SE_Code_region_ROM_end__ - __ICFEDIT_SE_Code_nokey_region_ROM_start__ + 1;
SE_IF_region_ROM_Length           = __ICFEDIT_SE_IF_region_ROM_end__ - __ICFEDIT_SE_IF_region_ROM_start__ + 1;
SB_ROM_region_Length              = __ICFEDIT_SB_region_ROM_end__ - __ICFEDIT_SB_region_ROM_start__ + 1;
LOADER_ROM_region_Length          = __ICFEDIT_LOADER_region_ROM_end__ - __ICFEDIT_LOADER_region_ROM_start__ + 1;

/* RAM section */
/* SE RAM1 region protected by firewall */
/* SE stack is placed 1st in RAM, stack overflow does not write on other RAM area */
__ICFEDIT_SE_region_RAM_start__     = 0x20000000;
__ICFEDIT_SE_region_RAM_stack_top__ = 0x20000400;
__ICFEDIT_SE_region_RAM_end__       = 0x20000FFF;

/* LOADER communication region when bypass mode activated */
__ICFEDIT_LOADER_COM_region_RAM_start__  = __ICFEDIT_SE_region_RAM_end__ + 1;
__ICFEDIT_LOADER_COM_region_RAM_end__    = __ICFEDIT_LOADER_COM_region_RAM_start__ + 0x0F;

/* LOADER RAM1 region */
__ICFEDIT_LOADER_region_RAM_start__ = __ICFEDIT_LOADER_COM_region_RAM_end__ + 1;
__ICFEDIT_LOADER_region_RAM_end__   = __ICFEDIT_LOADER_region_RAM_start__ + 0x1FFF;

/* SBSFU RAM1 region */
__ICFEDIT_SB_region_RAM_start__     = __ICFEDIT_LOADER_region_RAM_end__ + 0x1;
__ICFEDIT_SB_region_RAM_end__       = 0x2001FFFF;

SE_RAM_region_Length = __ICFEDIT_SE_region_RAM_end__ - __ICFEDIT_SE_region_RAM_stack_top__ + 1;
LOADER_RAM_region_Length = __ICFEDIT_LOADER_region_RAM_end__ - __ICFEDIT_LOADER_region_RAM_start__ + 1;
SB_RAM_region_Length = __ICFEDIT_SB_region_RAM_end__ - __ICFEDIT_SB_region_RAM_start__ + 1;

MEMORY
{
 SE_Entry_Secure_ROM_Region (rx)     : ORIGIN = __ICFEDIT_SE_CallGate_region_ROM_start__, LENGTH = SE_Entry_Secure_ROM_Region_Length
 SE_Key_region_ROM (rx)              : ORIGIN = __ICFEDIT_SE_Key_region_ROM_start__, LENGTH = SE_Key_region_ROM_Length
 SE_Startup_region_ROM (rx)          : ORIGIN = __ICFEDIT_SE_Startup_region_ROM_start__, LENGTH = SE_Startup_region_ROM_Length
 SE_ROM_region (rx)                  : ORIGIN = __ICFEDIT_SE_Code_nokey_region_ROM_start__, LENGTH = SE_ROM_region_Length
 SE_IF_region_ROM (rx)               : ORIGIN = __ICFEDIT_SE_IF_region_ROM_start__, LENGTH = SE_IF_region_ROM_Length
 SB_ROM_region (rx)                  : ORIGIN = __ICFEDIT_SB_region_ROM_start__, LENGTH = SB_ROM_region_Length
 LOADER_ROM_region (rx)              : ORIGIN = __ICFEDIT_LOADER_region_ROM_start__, LENGTH = LOADER_ROM_region_Length
 SE_RAM_region (xrw)               : ORIGIN = __ICFEDIT_SE_region_RAM_stack_top__, LENGTH = SE_RAM_region_Length
 LOADER_RAM_region (xrw)           : ORIGIN = __ICFEDIT_LOADER_region_RAM_start__, LENGTH = LOADER_RAM_region_Length
 SB_RAM_region (xrw)               : ORIGIN = __ICFEDIT_SB_region_RAM_start__, LENGTH = SB_RAM_region_Length
}

