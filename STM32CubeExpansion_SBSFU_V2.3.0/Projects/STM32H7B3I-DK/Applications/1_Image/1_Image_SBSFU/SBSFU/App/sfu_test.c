/**
  ******************************************************************************
  * @file    sfu_test.c
  * @author  MCD Application Team
  * @brief   Test Protections module.
  *          This file provides set of firmware functions to manage Test Protections
  *          functionalities.
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

/* Includes ------------------------------------------------------------------*/
#include "sfu_test.h"
#include "main.h"
#include "sfu_trace.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level.h"
#include "string.h" /* required for strncmp */

#if defined(SFU_TEST_PROTECTION)

/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @defgroup  SFU_TEST SFU Test
  * @brief This file provides the functions to test the protection(MPU, FWALL, PCROP, WRP)
  * @{
  */

#define TEST_ISOLATION 1

/** @defgroup SFU_TEST_Imported_Variables Imported Variables
  * @{
  */
extern RTC_HandleTypeDef RtcHandle;

/**
  * @}
  */

/** @defgroup  SFU_TEST_Private_Variable Private Variables
  * @{
  */


/* Automatic tests : list of tests (Address, Message, Operation */
/* Flash programming by 128 bits */
#define SHIFT_FLASH_WRITE (16U-1U)

ProtectionTest_t aProtectTests[] =
{
#ifdef SFU_MPU_PROTECT_ENABLE
  {SFU_RAM_BASE, "MPU SRAM1 start address", TEST_EXECUTE},
  {SFU_RAM_END - 3U, "MPU SRAM1 end address", TEST_EXECUTE},             /* -3 : previous 32 bits for execute test */
  {PERIPH_BASE, "Peripheral base address", TEST_EXECUTE},
  {((uint32_t)0x5FFFFFFF - 3U), "Peripheral end adress", TEST_EXECUTE},    /* -3 : previous 32 bits for execute test */
  /* READ_FLASH is possible only in privileged mode */
  {INTVECT_START, "Init. Vector", TEST_READ_FLASH},
  {REGION_SLOT_0_START, "MPU code slot 0 begin address", TEST_EXECUTE},
  {REGION_SLOT_0_END - 3U, "MPU code slot 0 end address", TEST_EXECUTE},   /* -3 : alignment for 32bits execute test */
#endif /* SFU_MPU_PROTECT_ENABLE */
#ifdef SFU_WRP_PROTECT_ENABLE
  {INTVECT_START, "WRP start address", TEST_WRITE_FLASH},
  {SFU_AREA_ADDR_END - SHIFT_FLASH_WRITE, "WRP end address", TEST_WRITE_FLASH},  /* -7 : alignment for 64bits writing
                                                                                   test */
  {SB_REGION_ROM_START, "WRP internal (SB) address", TEST_ERASE_FLASH},
#endif /* SFU_WRP_PROTECT_ENABLE */
#ifdef SFU_PCROP_PROTECT_ENABLE
  {SE_KEY_REGION_ROM_START, "PCROP start address", TEST_WRITE_FLASH},
  {SE_KEY_REGION_ROM_END - SHIFT_FLASH_WRITE, "PCROP end address", TEST_WRITE_FLASH}, /* -7 : alignment for 64bits
                                                                                         writing test */
  {SE_KEY_REGION_ROM_START, "PCROP start address", TEST_READ_FLASH},
  {SE_KEY_REGION_ROM_END, "PCROP end address", TEST_READ_FLASH},
#endif /* SFU_PCROP_PROTECT_ENABLE */
#if defined(TEST_ISOLATION)
  {SE_CODE_REGION_ROM_START, "Isolated code start address", TEST_WRITE_FLASH},        /* WRP protection applies  */
  {SE_CODE_REGION_ROM_END - SHIFT_FLASH_WRITE, "Isolated code end address", TEST_WRITE_FLASH},/* -7 : alignment for
                                                                         64bits writing test + WRP protection applies */
  {SE_CODE_REGION_ROM_START, "Isolated code start address", TEST_READ_FLASH},
  {SE_KEY_REGION_ROM_START - 1U, "Isol KeyConst start addr", TEST_READ_FLASH},
  {SE_KEY_REGION_ROM_END - 1U, "Isol KeyConst end addr", TEST_READ_FLASH},
  {SE_CODE_REGION_ROM_END, "Isolated code end address", TEST_READ_FLASH},
  {SE_IF_REGION_ROM_START - 1U, "Isolated MPU region last address", TEST_READ_FLASH}, /* last protected address as the
                                                                           isolated region is bigger than the SE code */
  {SE_STARTUP_REGION_ROM_START + 1U, "Isolated startup address", TEST_EXECUTE}, /* +1 : contains the SE startup code
                                                                    that initializes all the variables in the binary. */
  {SE_CODE_REGION_ROM_START, "Isolated code start address", TEST_EXECUTE},
  {SE_CODE_REGION_ROM_END - 3U, "Isolated code end address", TEST_EXECUTE}, /* -3 : alignment for 32bits execute test */
  {SE_REGION_RAM_START, "Isolated Vdata address", TEST_WRITE_RAM},
  {SE_REGION_RAM_END - 3U, "Isolated Vdata address", TEST_WRITE_RAM},     /* -3 : alignment for 32bits writing test */
  {SE_REGION_RAM_START, "Isolated Vdata address", TEST_READ_RAM},
  {SE_REGION_RAM_END, "Isolated Vdata address", TEST_READ_RAM},
  {SE_REGION_RAM_START, "Isolated Vdata address", TEST_EXECUTE},
  {SE_REGION_RAM_END - 3U, "Isolated Vdata address", TEST_EXECUTE},       /* -3 : alignment for 32bits writing test */
  /*
   * The H7/G0/G4 does not protect the slot 0 header with isolation.
   * So the slot 0 header is not protected against SB_SFU.
   * But, it is protected from the user application thanks to the secure user memory.
   */
  /*
   * On H7, with RSS V2.0, SWAP bit is always disable when secure memmory set.
   * We do not protect the symmetric @ of the bank 1 protected regions.
   * ==> tests removed.
   */
#endif /* TEST_ISOLATION  */
#ifdef SFU_MPU_PROTECT_ENABLE
  /* Testing the MPU protection of the OBs */
  /*
   * OBs are not mapped to any memory address and can be accessed only through the Flash register interface.
   * No testing required.
   */
#endif /* SFU_MPU_PROTECT_ENABLE  */
  /*  Check DMA access not possible  */
  /*  Check peripheral bus master access not possible */
  {(uint32_t)(&DMA1_Stream0->CR), "DMA1 stream 0 CR", TEST_WRITE_RAM},
  {(uint32_t)(&DMA1_Stream7->FCR), "DMA1 stream 7 FCR", TEST_WRITE_RAM},
  {(uint32_t)(&DMA2_Stream0->CR), "DMA2 stream 0 CR", TEST_WRITE_RAM},
  {(uint32_t)(&DMA2_Stream7->FCR), "DMA2 stream 7 FCR", TEST_WRITE_RAM},
  {(uint32_t)(&MDMA_Channel0->CISR), "MDMA chan10 CISR ", TEST_WRITE_RAM},
  {(uint32_t)(&MDMA_Channel15->CMDR), "MDMA chan115 CMDR", TEST_WRITE_RAM},
  {(uint32_t)(&BDMA_Channel0->CCR), "BDMA chan10 CCR", TEST_WRITE_RAM},
  {(uint32_t)(&BDMA_Channel7->CM1AR), "BDMA chan17 CM1AR", TEST_WRITE_RAM},
  {(uint32_t)(&USB1_OTG_HS->GOTGCTL), "USB1_OTG_HS GOTGCTL", TEST_WRITE_RAM},
  {(uint32_t)(&USB1_OTG_HS->DIEPTXF[0x0E]), "USB1_OTG_HS DIEPTXF", TEST_WRITE_RAM},
  {(uint32_t)(&DMA2D->CR), "DMA2D CR", TEST_WRITE_RAM},
  {(uint32_t)(&DMA2D->BGCLUT[255]), "DMA2D BGCLUT", TEST_WRITE_RAM},
  /*  Check ITCM RAM not executeable , not readeable */
  {CD_ITCMRAM_BASE, "0x0 ITCM RAM", TEST_EXECUTE},
  {CD_ITCMRAM_BASE, "0x0 ITCM RAM", TEST_READ_RAM},
  {0x00000000, "Execution successful", TEST_END},
};


/* Automatic test : list of operation */
uint8_t aTestOperation[][20] =
{
  "write 32 bytes",
  "read 1 byte",
  "read 1 byte",
  "erase 512 bytes",
  "write 4 bytes",
  "read 1 byte",
  "execute",
  "execute",
  "end"
};

/**
  * @}
  */
/** @defgroup  SFU_TEST_Private_Functions Private Functions
  * @{
  */
static void SFU_TEST_Protection(void);

/**
  * @}
  */

/** @addtogroup  SFU_TEST_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Automatic test of PCROP/WRP/FWALL/MPU protections
  * @param  None.
  * @retval None.
  */
static void SFU_TEST_Protection(void)
{
  uint32_t  offset = 0;
  uint32_t test_idx;
  uint32_t flashErrCode = 0;
  const uint8_t pattern[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0
                             };
  uint32_t page_error = 0U;
  FLASH_EraseInitTypeDef p_erase_init;
  TestStatus status = TEST_IN_PROGRESS;
  __IO uint8_t tmp;
  void (*func)(void);

  /*
    * Now checking the security
    * We will try to access protected @ and run protected code.
    * Each time a a flash error will be raised or a reset will be generated by IP protection.
    * At next boot we read the backup register to check the next @ until all addresses have been verified.
    * WatchDog should be disabled : while(1) loop in case HardFault when trying to execute code @ address protected by
    * FWALL/Code Isolation
    */

  do
  {
    /* slow down execution */
    HAL_Delay(100);

    /* Increment test number for next execution */
    test_idx = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, (test_idx + 1));
    offset = aProtectTests[test_idx].type == TEST_READ_FLASH_BANK2 ? (FLASH_BANK_SIZE) : 0;
    printf("\r\n= [TEST] %s @ %s %08x", aTestOperation[aProtectTests[test_idx].type], aProtectTests[test_idx].msg,
           aProtectTests[test_idx].address + offset);
    switch (aProtectTests[test_idx].type)
    {
      /* Trying to write 64bits in FLASH : WRP flag is set for WRP or PCROP protected area
         or reset generated if under FWALL or MPU protection */
      case TEST_WRITE_FLASH :
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, aProtectTests[test_idx].address, (uint64_t) pattern);
        HAL_FLASH_Lock();
        flashErrCode = HAL_FLASH_GetError();
        printf("\r\nflash error code: %x", flashErrCode);
        if ((flashErrCode & HAL_FLASH_ERROR_WRP) == 0U)
        {
          status = TEST_ERROR;
        }

        /*
         * OBs protection test: MPU generates a MEMORY FAULT and a RESET
         * So the test is FAILED if we reach this line
         */
        if (0 == strncmp("OBs @", (const char *)aProtectTests[test_idx].msg, 5))
        {
          status = TEST_ERROR;
        }
        /* else not an OB protection test so probably not an error */
        break;

        /* Trying to read in FLASH : RDP set in case of PCROP protected area
           or reset generated if under FWALL or MPU protection */
      case TEST_READ_FLASH_BANK2:
      case TEST_READ_FLASH :
        tmp = *(uint8_t *)(aProtectTests[test_idx].address + offset);
        HAL_Delay(1);                                                 /* ensure Flag is set */
        if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_RDPERR_BANK1 | FLASH_FLAG_RDPERR_BANK2) == 0)
        {
          printf(" value : %d", tmp);
          status = TEST_ERROR;
        }
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_RDPERR_BANK1 | FLASH_FLAG_RDPERR_BANK2);
        /*
         * When Isolation activated : RESET should be generated
         * So the test is FAILED if we reach this line
         */
        if (0 == strncmp("Isolated", (const char *)aProtectTests[test_idx].msg, 8))
        {
          status = TEST_ERROR;
        }

        break;

      /* Trying to erase 512 bytes in FLASH : WRP flag set for WRP or PCROP protected area
         or reset generated if under FWALL or MPU protection */
      case TEST_ERASE_FLASH :
        HAL_FLASH_Unlock();
        p_erase_init.Banks       = SFU_LL_FLASH_INT_GetBank(aProtectTests[test_idx].address);
        p_erase_init.TypeErase    = FLASH_TYPEERASE_SECTORS;
        /* Do nothing if FLASH_CR_PSIZE not define */
        p_erase_init.Sector       = SFU_LL_FLASH_GetSector(aProtectTests[test_idx].address);
        p_erase_init.NbSectors    = 1;
        HAL_FLASHEx_Erase(&p_erase_init, &page_error);
        HAL_FLASH_Lock();
        if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_WRP) == 0U)
        {
          status = TEST_ERROR;
        }
        break;

      /* Trying to write in RAM : reset generated if under FWALL or MPU protection */
      case TEST_WRITE_RAM :
        *(uint32_t *)aProtectTests[test_idx].address = 0x00000000;
        status = TEST_ERROR;
        break;

      /* Trying to read in RAM : reset generated if under FWALL or MPU protection */
      case TEST_READ_RAM :
        tmp = *(uint8_t *)aProtectTests[test_idx].address;
        printf(" value : %d ", tmp);
        status = TEST_ERROR;
        break;

        /* Tryning to execute code : reset generated if under FWALL or MPU protection */
      case TEST_EXECUTE_BANK2:
      case TEST_EXECUTE :
        func = (void(*)(void))(aProtectTests[test_idx].address + offset);
        func();
        status = TEST_ERROR;
        break;

      /* End of execution */
      case TEST_END :
        status = TEST_COMPLETED;
        break;

      default :
        break;
    }
  } while (status == TEST_IN_PROGRESS) ;

  if (status == TEST_ERROR)
  {
    SFU_TEST_Error();
  }
  else if (status == TEST_COMPLETED)
  {
    /* reset for next run (need a RESET of the platform to restart a session) */
    printf("\b\b\b\b\b\b\b\b");
    printf("\r\nYou can RESET the platform to restart the tests.\r\n");
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_DONE);              /* no more test in progress */
  }
}

/**
  * @}
  */

/** @defgroup  SFU_TEST_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initialization of backup register and start test
  * @param  None.
  * @retval None.
  */
void SFU_TEST_Init(void)
{
  /* Use register DR0 to know which if the test sequence is in progress or done */
  /* Use register DR1 to know which test to run after a reset */

  if (MAGIC_TEST_DONE == HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0))      /* Test done */
  {
    return;
  }
  else if (MAGIC_TEST_INIT != HAL_RTCEx_BKUPRead(&RtcHandle,
                                                 RTC_BKP_DR0)) /* Test not done nor in progress: Init magic (DR0) and
                                                                  reset test index (DR1) */
  {
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_INIT);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x0000);
  }

  /* Start test */
  SFU_TEST_Protection();
}


/**
  * @brief  Reset backup register to be able to execute a new test sequence
  * @param  None.
  * @retval None.
  */
void SFU_TEST_Reset(void)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_INIT);
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x0000);
}


/**
  * @brief  Error detected during test sequence
  * @param  None.
  * @retval None.
  */

void SFU_TEST_Error(void)
{
  printf(" ===> KO!!!!!");
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, MAGIC_TEST_DONE);
}


/**
  * @}
  */

#endif /* SFU_TEST_PROTECTION */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
