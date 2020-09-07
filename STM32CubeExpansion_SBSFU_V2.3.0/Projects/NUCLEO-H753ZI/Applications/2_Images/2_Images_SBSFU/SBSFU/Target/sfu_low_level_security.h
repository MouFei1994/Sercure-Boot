/**
  ******************************************************************************
  * @file    sfu_low_level_security.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update security
  *          low level interface.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef SFU_LOW_LEVEL_SECURITY_H
#define SFU_LOW_LEVEL_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_fwimg_regions.h"
#include "sfu_def.h"


/** @addtogroup SFU Secure Secure Boot / Firmware Update
  * @{
  */

/** @addtogroup SFU_LOW_LEVEL
  * @{
  */

/** @defgroup SFU_LOW_LEVEL_SECURITY Security Low Level Interface
  * @{
  */

/** @defgroup SFU_SECURITY_Configuration Security Configuration
  * @{
  */


/** @defgroup SFU_CONFIG_BOOT_ADD Boot Addresses Configuration
  * @brief In the STM32H7x3, two different boot areas can be selected through the BOOT pin and the
  *        boot base address programmed in the BOOT_ADD0 and BOOT_ADD1 option bytes.
  * @note  To ensure a Root Of Trust, we must make sure that in product mode the device can
  *        boot only from one single boot address (unique entry point).
  * @{
  */
#define SFU_PROTECT_UNIQUE_BOOT_ADDRESS_MSB (0x0800U) /*!< MSB of the unique entry point of the device */
#define SFU_PROTECT_UNIQUE_BOOT_ADDRESS ((uint32_t)(SFU_PROTECT_UNIQUE_BOOT_ADDRESS_MSB<<16))
/*!< Address of the unique entry point of the device */

/**
  * @}
  */

/** @defgroup SFU_CONFIG_WRP_AREAS WRP Protected Areas Configuration
  * @{
  */
/*!< Bank 1, Area A  used to protect Vector Table */
#define SFU_PROTECT_WRP_AREA_1          (OB_WRPAREA_BANK1_AREAA)

/*!< First sector including the Vector Table: 0 based */
#define SFU_PROTECT_WRP_PAGE_START_1    ((uint32_t)((SFU_BOOT_BASE_ADDR - FLASH_BASE) / FLASH_SECTOR_SIZE))

/*!< Last sector: (code_size-1)/sector_size because the
     page indexes start from 0 */
#define SFU_PROTECT_WRP_PAGE_END_1      ((uint32_t)((SFU_AREA_ADDR_END - FLASH_BASE) / FLASH_SECTOR_SIZE))
/**
  * @}
  */

/** @defgroup SFU_CONFIG_PCROP_AREAS PCROP Protected Areas Configuration
  * @{
  */
#define SFU_PROTECT_PCROP_AREA          (FLASH_BANK_1)                            /*!< PCROP Area */
#define SFU_PROTECT_PCROP_ADDR_START    ((uint32_t)SFU_KEYS_AREA_ADDR_START)      /*!< PCROP Start Address (included) */
#define SFU_PROTECT_PCROP_ADDR_END      ((uint32_t)SFU_KEYS_AREA_ADDR_END)        /*!< PCROP End Address*/
/**
  * @}
  */


/** @defgroup SFU_CONFIG_MPU_REGIONS MPU Regions Configuration
  * @{
  */

/**
  * @brief The regions can overlap, and can be nested. The region 7 has the highest priority
  *    and the region 0 has the lowest one and this governs how overlapping the regions behave.
  *    The priorities are fixed, and cannot be changed.
  */



#define SFU_PROTECT_MPU_MAX_NB_SUBREG           (8U)

/**
  * @brief Region 0 - Enable the read/write operations for the 1st 128KBytes peripheral having no DMA, (RNG and HASH
  *                   excluded) in unprivileged mode.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_PERIPH_1_RGNV  MPU_REGION_NUMBER0
#define SFU_PROTECT_MPU_PERIPH_1_START PERIPH_BASE         /*!< Peripheral memory area */
#define SFU_PROTECT_MPU_PERIPH_1_SIZE  MPU_REGION_SIZE_128KB
#define SFU_PROTECT_MPU_PERIPH_1_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_PERIPH_1_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_PERIPH_1_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_PERIPH_1_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_PERIPH_1_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_PERIPH_1_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 1 - Enable the read/write operations for others peripheral as GPIO, RCC, Flash and backup register
  *                   (but DMA2D, MDMA, some DMA remains accessible) in unprivileged mode.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_PERIPH_2_RGNV  MPU_REGION_NUMBER1
#define SFU_PROTECT_MPU_PERIPH_2_START (0x50000000)
#define SFU_PROTECT_MPU_PERIPH_2_SIZE  MPU_REGION_SIZE_256MB
#define SFU_PROTECT_MPU_PERIPH_2_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_PERIPH_2_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_PERIPH_2_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_PERIPH_2_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_PERIPH_2_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_PERIPH_2_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 2 - Enable the read/write operations for full flash area in unprivileged mode.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_FLASHACC_RGNV  MPU_REGION_NUMBER2
#define SFU_PROTECT_MPU_FLASHACC_START D1_AXIFLASH_BASE    /*!< FLASH memory area */
#define SFU_PROTECT_MPU_FLASHACC_SIZE  MPU_REGION_SIZE_2MB
#define SFU_PROTECT_MPU_FLASHACC_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_FLASHACC_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_FLASHACC_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_FLASHACC_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_FLASHACC_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_FLASHACC_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 3 - Enable the execution for SB/SFU Full area (SBSFU + SE + Keys) in unprivileged mode.
  *                   Read only capability configured
  *                   Inner region inside the Region 2
  */
#define SFU_PROTECT_MPU_FLASHEXE_RGNV  MPU_REGION_NUMBER3
#define SFU_PROTECT_MPU_FLASHEXE_START D1_AXIFLASH_BASE    /*!< FLASH memory area */
#define SFU_PROTECT_MPU_FLASHEXE_SIZE  MPU_REGION_SIZE_128KB
#define SFU_PROTECT_MPU_FLASHEXE_SREG  0x00U               /*!< All subregions activated */
#define SFU_PROTECT_MPU_FLASHEXE_PERM  MPU_REGION_PRIV_RO_URO
#define SFU_PROTECT_MPU_FLASHEXE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define SFU_PROTECT_MPU_FLASHEXE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_FLASHEXE_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_FLASHEXE_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 4 - Enable the read/write operation in privileged mode for Header of Slot#0
  *                   Execution capability disabled
  *                   Inner region inside the Region 2
  */
#define SFU_PROTECT_MPU_HEADER_RGNV  MPU_REGION_NUMBER4
#define SFU_PROTECT_MPU_HEADER_START (uint32_t)SFU_IMG_SLOT_0_REGION_HEADER
#define SFU_PROTECT_MPU_HEADER_SIZE  MPU_REGION_SIZE_512B
#define SFU_PROTECT_MPU_HEADER_SREG  0x00U                 /*!< All subregions activated */
#define SFU_PROTECT_MPU_HEADER_PERM  MPU_REGION_PRIV_RW
#define SFU_PROTECT_MPU_HEADER_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_HEADER_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_HEADER_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_HEADER_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 5 - Enable the read/write operation in unprivileged mode for RAM area.
  *                   Execution capability disabled
  */
#define SFU_PROTECT_MPU_SRAMACC_RGNV  MPU_REGION_NUMBER5
#define SFU_PROTECT_MPU_SRAMACC_START D1_DTCMRAM_BASE      /*!< RAM memory area */
#define SFU_PROTECT_MPU_SRAMACC_SIZE  MPU_REGION_SIZE_512KB
#define SFU_PROTECT_MPU_SRAMACC_SREG  0x00U                /*!< All subregions activated */
#define SFU_PROTECT_MPU_SRAMACC_PERM  MPU_REGION_FULL_ACCESS
#define SFU_PROTECT_MPU_SRAMACC_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_SRAMACC_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_SRAMACC_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_SRAMACC_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 6 - Enable the read/write operation in privileged mode for Secure Engine RAM area.
  *                   Execution capability disabled
  *                   Inner region inside the Region 5
  *                   Address must be aligned on 4KB as size is 4KB
  */
#define SFU_PROTECT_MPU_SRAM_SE_RGNV  MPU_REGION_NUMBER6
#define SFU_PROTECT_MPU_SRAM_SE_START SFU_SENG_RAM_ADDR_START  /*!< SE RAM memory area */
#define SFU_PROTECT_MPU_SRAM_SE_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_SRAM_SE_SREG  0x00U                /*!< All subregions activated */
#define SFU_PROTECT_MPU_SRAM_SE_PERM  MPU_REGION_PRIV_RW
#define SFU_PROTECT_MPU_SRAM_SE_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_SRAM_SE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_SRAM_SE_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_SRAM_SE_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 7 - Enable the execution for Secure Engine flash area in privileged mode.
  *                   Read only capability configured
  *                   Inner region inside the Region 3
  */
#define SFU_PROTECT_MPU_EXEC_SE_RGNV  MPU_REGION_NUMBER7
#define SFU_PROTECT_MPU_EXEC_SE_START D1_AXIFLASH_BASE    /*!< FLASH memory area */
#define SFU_PROTECT_MPU_EXEC_SE_SIZE  MPU_REGION_SIZE_32KB
#define SFU_PROTECT_MPU_EXEC_SE_SREG  0x00U                /*!< All subregions activated */
#define SFU_PROTECT_MPU_EXEC_SE_PERM  MPU_REGION_PRIV_RO
#define SFU_PROTECT_MPU_EXEC_SE_EXECV MPU_INSTRUCTION_ACCESS_ENABLE
#define SFU_PROTECT_MPU_EXEC_SE_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_EXEC_SE_B     MPU_ACCESS_NOT_BUFFERABLE
#define SFU_PROTECT_MPU_EXEC_SE_C     MPU_ACCESS_CACHEABLE

/**
  * @brief Region 8 - Disable all read/write operations for DMA2D peripheral.
  *                   Execution capability disabled
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_DMA2D_RGNV  MPU_REGION_NUMBER8
#define SFU_PROTECT_MPU_DMA2D_START DMA2D_BASE             /*!< DMA2D peripheral memory area */
#define SFU_PROTECT_MPU_DMA2D_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_DMA2D_SREG  0x00U                  /*!< All subregions activated */
#define SFU_PROTECT_MPU_DMA2D_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_DMA2D_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_DMA2D_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_DMA2D_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_DMA2D_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 9 - Disable all read/write operations for MDMA peripheral.
  *                   Execution capability disabled
  *                   Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_MDMA_RGNV  MPU_REGION_NUMBER9
#define SFU_PROTECT_MPU_MDMA_START MDMA_BASE               /*!< MDMA peripheral memory area */
#define SFU_PROTECT_MPU_MDMA_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_MDMA_SREG  0x00U                   /*!< All subregions activated */
#define SFU_PROTECT_MPU_MDMA_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_MDMA_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_MDMA_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_MDMA_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_MDMA_C     MPU_ACCESS_NOT_CACHEABLE

/**
  * @brief Region 10 - Disable all read/write operations for BDMA peripheral.
  *                    Execution capability disabled
  *                    Inner region inside the Region 0
  */
#define SFU_PROTECT_MPU_BDMA_RGNV  MPU_REGION_NUMBER10
#define SFU_PROTECT_MPU_BDMA_START BDMA_BASE               /*!< BDMA peripheral memory area */
#define SFU_PROTECT_MPU_BDMA_SIZE  MPU_REGION_SIZE_4KB
#define SFU_PROTECT_MPU_BDMA_SREG  0x00U                   /*!< All subregions activated */
#define SFU_PROTECT_MPU_BDMA_PERM  MPU_REGION_NO_ACCESS
#define SFU_PROTECT_MPU_BDMA_EXECV MPU_INSTRUCTION_ACCESS_DISABLE
#define SFU_PROTECT_MPU_BDMA_TEXV  MPU_TEX_LEVEL0
#define SFU_PROTECT_MPU_BDMA_B     MPU_ACCESS_BUFFERABLE
#define SFU_PROTECT_MPU_BDMA_C     MPU_ACCESS_NOT_CACHEABLE



/**
  * @}
  */

/** @defgroup SFU_CONFIG_TAMPER Tamper Configuration
  * @{
  */
#define TAMPER_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define RTC_TAMPER_ID                      RTC_TAMPER_3
#define RTC_TAMPER_ID_INTERRUPT            RTC_TAMPER3_INTERRUPT

/**
  * @}
  */

/** @defgroup SFU_CONFIG_DBG Debug Port Configuration
  * @{
  */
#define SFU_DBG_PORT            GPIOA
#define SFU_DBG_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define SFU_DBG_SWDIO_PIN       GPIO_PIN_13
#define SFU_DBG_SWCLK_PIN       GPIO_PIN_14


/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup SFU_SECURITY_Exported_Constants_BOOL SFU Bool definition
  * @{
  */
typedef enum
{
  SFU_FALSE = 0U,
  SFU_TRUE = !SFU_FALSE
} SFU_BoolTypeDef;

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Constants_Wakeup FU WAKEUP ID Type definition
  * @{
  */
typedef enum
{
  SFU_RESET_UNKNOWN = 0x00U,
  SFU_RESET_WDG_RESET,
  SFU_RESET_LOW_POWER,
  SFU_RESET_HW_RESET,
  SFU_RESET_BOR_RESET,
  SFU_RESET_SW_RESET,
  SFU_RESET_OB_LOADER,
} SFU_RESET_IdTypeDef;

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Constants_Protections FU SECURITY Protections_Constants
  * @{
  */
#define SFU_PROTECTIONS_NONE                 ((uint32_t)0x00000000U)   /*!< Protection configuration unchanged */
#define SFU_STATIC_PROTECTION_RDP            ((uint32_t)0x00000001U)   /*!< RDP protection level 1 is applied */
#define SFU_STATIC_PROTECTION_WRP            ((uint32_t)0x00000002U)   /*!< Constants section in Flash. Needed as
                                                                            separate section to support PCRoP */
#define SFU_STATIC_PROTECTION_PCROP          ((uint32_t)0x00000004U)   /*!< SFU App section in Flash */
#define SFU_STATIC_PROTECTION_LOCKED         ((uint32_t)0x00000008U)   /*!< RDP Level2 is applied. The device is Locked!
                                                                            Std Protections cannot be
                                                                            added/removed/modified */
#define SFU_STATIC_PROTECTION_BFB2           ((uint32_t)0x00000010U)   /*!< BFB2 is disabled. The device shall always
                                                                            boot in bank1! */

#define SFU_RUNTIME_PROTECTION_MPU           ((uint32_t)0x00000100U)   /*!< Shared Info section in Flash */
#define SFU_RUNTIME_PROTECTION_IWDG          ((uint32_t)0x00000400U)   /*!< Independent Watchdog */
#define SFU_RUNTIME_PROTECTION_DAP           ((uint32_t)0x00000800U)   /*!< Debug Access Port control */
#define SFU_RUNTIME_PROTECTION_DMA           ((uint32_t)0x00001000U)   /*!< DMA protection, disable DMAs */
#define SFU_RUNTIME_PROTECTION_ANTI_TAMPER   ((uint32_t)0x00002000U)   /*!< Anti-Tampering protections */
#define SFU_RUNTIME_PROTECTION_CLOCK_MONITOR ((uint32_t)0x00004000U)   /*!< Activate a clock monitoring */
#define SFU_RUNTIME_PROTECTION_TEMP_MONITOR  ((uint32_t)0x00008000U)   /*!< Activate a Temperature monitoring */

#define SFU_STATIC_PROTECTION_ALL           (SFU_STATIC_PROTECTION_RDP   | SFU_STATIC_PROTECTION_WRP   | \
                                             SFU_STATIC_PROTECTION_PCROP | SFU_STATIC_PROTECTION_LOCKED)
/*!< All the static protections */

#define SFU_RUNTIME_PROTECTION_ALL          (SFU_RUNTIME_PROTECTION_MPU  | SFU_RUNTIME_PROTECTION_FWALL | \
                                             SFU_RUNTIME_PROTECTION_IWDG | SFU_RUNTIME_PROTECTION_DAP   | \
                                             SFU_RUNTIME_PROTECTION_DMA  | SFU_RUNTIME_PROTECTION_ANTI_TAMPER  | \
                                             SFU_RUNTIME_PROTECTION_CLOCK_MONITOR | SFU_RUNTIME_PROTECTION_TEMP_MONITOR)
/*!< All the run-time protections */

#define SFU_INITIAL_CONFIGURATION           (0x00)     /*!< Initial configuration */
#define SFU_SECOND_CONFIGURATION            (0x01)     /*!< Second configuration */
#define SFU_THIRD_CONFIGURATION             (0x02)     /*!< Third configuration */
/**
  * @}
  */

/**
  * @}
  */


/** @defgroup SFU_SECURITY_Exported_Types Exported Types
  * @{
  */

/**
  * @brief  SFU_MPU Type SFU MPU Type
  */
typedef struct
{
  uint8_t                Number;            /*!< Specifies the number of the region to protect. This parameter can be a
                                                 value of CORTEX_MPU_Region_Number */
  uint32_t               BaseAddress;       /*!< Specifies the base address of the region to protect. */
  uint8_t                Size;              /*!< Specifies the size of the region to protect. */
  uint8_t                AccessPermission;  /*!< Specifies the region access permission type. This parameter can be a
                                                 value of CORTEX_MPU_Region_Permission_Attributes */
  uint8_t                DisableExec;       /*!< Specifies the instruction access status. This parameter can be a value
                                                 of  CORTEX_MPU_Instruction_Access */
  uint8_t                SubRegionDisable;  /*!< Specifies the sub region field (region is divided in 8 slices) when bit
                                                 is 1 region sub region is disabled */
  uint8_t                Tex;               /*!< Specifies the tex value  */
  uint8_t                Cacheable;         /*!< Specifies the cacheable value  */
  uint8_t                Bufferable;        /*!< Specifies the cacheable value  */
} SFU_MPU_InitTypeDef;

typedef uint32_t      SFU_ProtectionTypeDef;  /*!<   SFU HAL IF Protection Type Def*/

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Macros Exported Macros
  * @{
  */

/** @defgroup SFU_SECURITY_Exported_Macros_Exceptions Exceptions Management
  * @brief Definitions used in order to have the same clean approach in the SFU_BOOT to manage exceptions
  * @{
  */

#define SFU_CALLBACK_ANTITAMPER(void) HAL_RTCEx_Tamper3EventCallback(RTC_HandleTypeDef *hrtc)
/*!<SFU Redirect of RTC Tamper Event Callback*/
#define SFU_CALLBACK_MEMORYFAULT(void) MemManage_Handler(void)  /*!<SFU Redirect of Mem Manage Callback*/
#define SFU_CALLBACK_HARDFAULT(void) HardFault_Handler(void)    /*!<SFU Redirect of Hard Fault Callback*/

/**
  * @}
  */

/** @defgroup SFU_SECURITY_Exported_Functions Exported Functions
  * @{
  */
SFU_ErrorStatus    SFU_LL_SECU_IWDG_Refresh(void);
SFU_ErrorStatus    SFU_LL_SECU_CheckApplyStaticProtections(void);
SFU_ErrorStatus    SFU_LL_SECU_CheckApplyRuntimeProtections(uint8_t uStep);
void               SFU_LL_SECU_GetResetSources(SFU_RESET_IdTypeDef *peResetpSourceId);
void               SFU_LL_SECU_ClearResetSources(void);
#ifdef SFU_MPU_PROTECT_ENABLE
SFU_ErrorStatus    SFU_LL_SECU_SetProtectionMPU(void);
#endif /*SFU_MPU_PROTECT_ENABLE*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_SECURITY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
