/**
  ******************************************************************************
  * @file    boot_hal.c
  * @author  MCD Application Team
  * @brief   This file contains  mcuboot stm32l5xx hardware specific implementation
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal.h"
#include "boot_hal_otfdec.h"
#include "boot_hal_imagevalid.h"
#include "mcuboot_config/mcuboot_config.h"
#include "uart_stdout.h"
#include "tfm_low_level_security.h"
#include "boot_record.h"
#include "target_cfg.h"
#include "cmsis.h"
#include "Driver_Flash.h"
#include "region_defs.h"
#ifdef MCUBOOT_EXT_LOADER
#include "bootutil/sha256.h"
#define BUTTON_PORT                       GPIOC
#define BUTTON_CLK_ENABLE                 __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_PIN                        GPIO_PIN_13
#endif /* MCUBOOT_EXT_LOADER */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;


/* otfdec magic is flash min write size: doubleword */
const uint32_t otfdec_magic[] =
{
  0x5a8b324f,
  0xc66da104,
};

/* otfdec key and magic flash layout */
#define OTFDEC_KEY_SIZE     BOOT_ENC_KEY_SIZE
#define OTFDEC_MAGIC_ARR_SZ (sizeof otfdec_magic / sizeof otfdec_magic[0])
#define OTFDEC_MAGIC_SZ     (sizeof otfdec_magic)

#define OTFDEC_KEY_ADDR     (FLASH_AREA_OTFDEC_OFFSET)
#define OTFDEC_MAGIC_ADDR   (FLASH_AREA_OTFDEC_OFFSET + OTFDEC_KEY_SIZE)

#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
/* Global variables to memorize images validation status */
#if (MCUBOOT_IMAGE_NUMBER == 1)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 2)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID};
#else
#error "MCUBOOT_IMAGE_NUMBER not supported"
#endif
uint8_t ImageValidIndex = 0;
uint8_t ImageValidEnable = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */

/*
#define ICACHE_MONITOR
*/
#if defined(ICACHE_MONITOR)
#define ICACHE_MONITOR_PRINT() printf("icache monitor - Hit: %x, Miss: %x\r\n", \
                            HAL_ICACHE_Monitor_GetHitValue(), HAL_ICACHE_Monitor_GetMissValue());
#else
#define ICACHE_MONITOR_PRINT()
#endif /* ICACHE_MONITOR */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup BOOT_HAL_Private_Functions  Private Functions
  * @{
  */
#ifdef MCUBOOT_EXT_LOADER
void execute_loader(void);
#endif /* MCUBOOT_EXT_LOADER */
void boot_clean_ns_ram_area(void);
__attribute__((naked)) void boot_jump_to_ns_image(uint32_t reset_handler_addr);
void icache_init(void);
/**
  * @}
  */
#ifdef MCUBOOT_EXT_LOADER
/**
  * @brief This function manage the jump to Local loader application.
  * @note
  * @retval void
  */
void boot_platform_noimage(void)
{
  /* unsecure all pin configuration ,non secure loader is initializing Octospi pin*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  GPIOA_S->SECCFGR = 0x0;
  GPIOB_S->SECCFGR = 0x0;
  GPIOC_S->SECCFGR = 0x0;
  GPIOD_S->SECCFGR = 0x0;
  GPIOE_S->SECCFGR = 0x0;
  GPIOF_S->SECCFGR = 0x0;
  GPIOG_S->SECCFGR = 0x0;
  GPIOH_S->SECCFGR = 0x0;
#if defined(MCUBOOT_PRIMARY_ONLY)
  /* loader code is set secure after control beeing set sucessfully */
  /* MPU allowing execution of this area is set after HDP activation */
  TFM_LL_SECU_SetLoaderCodeSecure();
#endif /* MCUBOOT_PRIMARY_ONLY */
  execute_loader();
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
/**
  * @brief This function activates the protection before executing local loader.
  * @note
  * @retval void
  */
void execute_loader(void)
{
#if defined(MCUBOOT_PRIMARY_ONLY)
  static struct arm_vector_table *vt = (struct arm_vector_table *)LOADER_S_CODE_START;

  /* Update run time protections for application execution */
  TFM_LL_SECU_UpdateLoaderRunTimeProtections();

  /* set the secure vector */
  SCB->VTOR = (uint32_t)LOADER_S_CODE_START;

  /*  change stack limit  */
  __set_MSPLIM(0);
  /* Restore the Main Stack Pointer Limit register's reset value
   * before passing execution to runtime firmware to make the
   * bootloader transparent to it.
   */
  __set_MSP(vt->msp);
  __DSB();
  __ISB();
  boot_jump_to_next_image(vt->reset);
#else
  static struct arm_vector_table *vt = (struct arm_vector_table *)LOADER_NS_CODE_START;
  /* Update run time protections for application execution */
  TFM_LL_SECU_UpdateLoaderRunTimeProtections();
  SCB_NS->VTOR = LOADER_NS_CODE_START;
  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)LOADER_NS_CODE_START));
  /* the function erase all internal SRAM , and unsecure all SRAM to adapt to any non secure loader mapping */
  boot_jump_to_ns_image(vt->reset);
#endif /* defined(MCUBOOT_PRIMARY_ONLY) */

  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while (1);
#endif
}

/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __GNUC__ */

/*!
 * \brief Chain-loading the next image in the boot sequence.
 *
 * This function calls the Reset_Handler of the next image in the boot sequence,
 * usually it is the secure firmware. Before passing the execution to next image
 * there is conditional rule to remove the secrets from the memory. This must be
 * done if the following conditions are satisfied:
 *  - Memory is shared between SW components at different stages of the trusted
 *    boot process.
 *  - There are secrets in the memory: KDF parameter, symmetric key,
 *    manufacturer sensitive code/data, etc.
 */
__attribute__((naked)) void boot_jump_to_ns_image(uint32_t reset_handler_addr)
{
  __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                 \n"
#endif /* !defined(__ICCARM__) */
    "mov     r7, r0                  \n"
    "bl      boot_clean_ns_ram_area  \n" /* Clean all RAM and unsecure all before jump */
    "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
    "mov     r1, r0                  \n" /* except R7 */
    "mov     r2, r0                  \n"
    "mov     r3, r0                  \n"
    "mov     r4, r0                  \n"
    "mov     r5, r0                  \n"
    "mov     r6, r0                  \n"
    "mov     r8, r0                  \n"
    "mov     r9, r0                  \n"
    "mov     r10, r0                 \n"
    "mov     r11, r0                 \n"
    "mov     r12, r0                 \n"
    "mov     lr,  r0                 \n"
    "bic.w   r7, r7, #1              \n"
    "blxns   r7                      \n" /* Jump to non secure Reset_handler */
  );
}
/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#endif /* MCUBOOT_EXT_LOADER */

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */
/**
  * @brief This function manage the jump to secure application.
  * @note
  * @retval void
  */
void jumper(struct arm_vector_table *vector)
{
  static struct arm_vector_table *vt;
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
  uint32_t image_index;

  /* Check again if images have been validated, to resist to basic hw attacks */
  for (image_index = 0; image_index < MCUBOOT_IMAGE_NUMBER; image_index++)
  {
    if (ImageValidStatus[image_index] != IMAGE_VALID)
    {
      BOOT_LOG_ERR("Error while double controlling images validation");
      Error_Handler();
    }
  }
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */


  ICACHE_MONITOR_PRINT()

#ifdef TFM_ICACHE_ENABLE
  /* Invalidate ICache before jumping to application */
  if (HAL_ICACHE_Invalidate() != HAL_OK)
  {
    Error_Handler();
  }
#endif /* ICACHE_ENABLED */

  /* Update run time protections for application execution */
  TFM_LL_SECU_UpdateRunTimeProtections();

  /* set the secure vector */
  SCB->VTOR = (uint32_t)vector;

  vt = (struct arm_vector_table *)vector;

  /* Double the update of run time protections, to resist to basic hardware attacks */
  TFM_LL_SECU_UpdateRunTimeProtections();

  /*  change stack limit  */
  __set_MSPLIM(0);
  /* Restore the Main Stack Pointer Limit register's reset value
   * before passing execution to runtime firmware to make the
   * bootloader transparent to it.
   */
  __set_MSP(vt->msp);
  __DSB();
  __ISB();
  boot_jump_to_next_image(vt->reset);

  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while (1);
#endif
}

/* Continue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_NoHdp_Code")))

/* Ensure local variables of this function are not placed in RAM but in
 * registers only, even in case of low compile optimization configuration
 * at project side.
 * - IAR: No need to force compile optimization level.
 * - KEIL: Force high compile optimization on per-file basis.
 * - STM32CUBEIDE: Force high compile optimization on function only.
 *  */
#if !defined(__ARMCC_VERSION)
__attribute__((optimize("Os")))
#endif /* !__ARMCC_VERSION */
#endif /* __GNUC__ */

/**
  * @brief This function is called to clear all RAM area before jumping in
  * in Secure application .
  * @note
  * @retval void
  */
void boot_clear_bl2_ram_area(void)
{
  __IO uint32_t *pt = (uint32_t *)BL2_DATA_START;
  uint32_t index;

  for (index = 0; index < (BL2_DATA_SIZE / 4); index++)
  {
    pt[index] = 0;
  }
}

/* Cotinue to place code in a specific section */
#if defined(__GNUC__)
__attribute__((section(".BL2_NoHdp_Code")))
/* Ensure local variables of this function are not placed in RAM but in
 * registers only, even in case of low compile optimization configuration
 * at project side.
 * - IAR: No need to force compile optimization level.
 * - KEIL: Force high compile optimization on per-file basis.
 * - STM32CUBEIDE: Force high compile optimization on function only.
 *  */
#if !defined(__ARMCC_VERSION)
__attribute__((optimize("Os")))
#endif /* !__ARMCC_VERSION */
#endif /* __GNUC__ */

/**
  * @brief This function is called to clear all RAM area before jumping in
  * in Secure application .
  * @note
  * @retval void
  */
void  boot_clean_ns_ram_area(void)
{
#ifdef TFM_ERROR_HANDLER_NON_SECURE
  __IO uint32_t *pt = (uint32_t *)(SRAM1_BASE + 256);
#else /*  TFM_ERROR_HANDLER_NON_SECURE */
  __IO uint32_t *pt = (uint32_t *)SRAM1_BASE;
#endif /* TFM_ERROR_HANDLER_NON_SECURE */
  uint32_t index;
  /* clean all SRAM1 */
  for (index = 0; index < (_SRAM1_SIZE_MAX / 4); index++)
  {
    pt[index] = 0;
  }
  /* unsecure all SRAM1 */
  for (index = 0; index < 24 ; index++)
  {
    /* assume loader is only in SRAM1 */
    GTZC_MPCBB1_S->VCTR[index] = 0;
  }
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

/**
  * @brief This function is called to recopy sw measurement
  * @note as secure user application does not implement
   *initial attestation , the recopy is not done.
  * @retval boot_status_err_t
  */
enum boot_status_err_t boot_save_boot_status(uint8_t sw_module,
                                             const struct image_header *hdr,
                                             const struct flash_area *fap)
{
  return  BOOT_STATUS_OK;
}


/**
  * @brief This function configures and enables the ICache.
  * @note
  * @retval execution_status
  */
void icache_init(void)
{
#ifdef ICACHE_MONITOR
  if (HAL_ICACHE_Monitor_Reset(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Monitor_Start(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
  {
    Error_Handler();
  }
#endif /* ICACHE_MONITOR */
  ICACHE_MONITOR_PRINT()

  /* Enable ICache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief This function configures the source of the time base:
  *        The time source is configured to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function overwrites the one declared as __weak in HAL.
  *       In this implementation, nothing is done.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /* load 1Hz for timeout 1 second */
  uint32_t ticks = SystemCoreClock ;
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return HAL_OK;
}
/**
  * @brief Provide a tick value in millisecond.
  * @note This function overwrites the one declared as __weak in HAL.
  *       In this implementation, time is counted without using SysTick timer interrupts.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
  static uint32_t m_uTick = 0U;
  static uint32_t t1 = 0U, tdelta = 0U;
  uint32_t t2;

  t2 =  SysTick->VAL;

  if (t2 <= t1)
  {
    tdelta += t1 - t2;
  }
  else
  {
    tdelta += t1 + SysTick->LOAD - t2;
  }

  if (tdelta > (SystemCoreClock / (1000U)))
  {
    tdelta = 0U;
    m_uTick ++;
  }

  t1 = t2;
  return m_uTick;
}

/* exported variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/**
  * @brief  Platform init
  * @param  None
  * @retval status
  */
int32_t boot_platform_init(void)
{
#ifdef MCUBOOT_EXT_LOADER
  GPIO_InitTypeDef GPIO_Init;
#endif /* MCUBOOT_EXT_LOADER */
  /* STM32L5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 110 MHz */
  SystemClock_Config();

#ifdef TFM_DEV_MODE
  /* Init for log */
  stdio_init();
#endif /*  TFM_DEV_MODE */

#ifdef TFM_ICACHE_ENABLE
  /* Configure and enable ICache */
  icache_init();
#endif /* ICACHE_ENABLED */

  /* Apply Run time Protection */
  TFM_LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  TFM_LL_SECU_CheckStaticProtections();

  /* enable SRAM2 retention low power mode */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableSRAM2ContentRetention();

  /* Double protections apply / check to resist to basic hardware attacks */
  /* Apply Run time Protection */
  TFM_LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  TFM_LL_SECU_CheckStaticProtections();

  if (FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    BOOT_LOG_ERR("Error while initializing Flash Interface");
    Error_Handler();
  }



#ifdef MCUBOOT_EXT_LOADER
  /* configure Button pin */
  BUTTON_CLK_ENABLE;
  GPIO_Init.Pin       = BUTTON_PIN;
  GPIO_Init.Mode      = 0;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Alternate = 0;
  HAL_GPIO_Init(BUTTON_PORT, &GPIO_Init);
  /* read pin value */
  if (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_SET)
  {
    boot_platform_noimage();
  }
#endif /* MCUBOOT_EXT_LOADER */
  return 0;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 110000000
  *            HCLK(Hz)                       = 110000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 55
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 5
  *            Voltage range                  = 0
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 0 for frequency above 80 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_NoHdp_Code"
#else
__attribute__((section(".BL2_NoHdp_Code")))
#endif /* __ICCARM__ */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
#ifdef TFM_ERROR_HANDLER_NON_SECURE
#define WHILE_1_OPCODE 0xe7fe
  typedef void (*nsfptr_t)(void) __attribute__((cmse_nonsecure_call));
  nsfptr_t nsfptr = (nsfptr_t)(SRAM1_BASE_NS + 1);
  __IO uint16_t *pt = (uint16_t *)SRAM1_BASE_NS;
  /*  copy while(1) instruction */
  *pt = WHILE_1_OPCODE;
  /* Flush and refill pipeline  */
  __DSB();
  __ISB();
  /*  call non secure while(1) */
  nsfptr();
#else /*  TFM_ERROR_HANDLER_NON_SECURE */
  /* it is customizeable */
  /* an infinite loop,  and a reset for single fault injection */
  static __IO int once = 1;
  while (once);
  NVIC_SystemReset();
#endif  /*  TFM_ERROR_HANDLER_NON_SECURE */
}

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef TFM_DEV_MODE
  printf("assertion \" %s \" failed: file %s %d\n", expr, file, line);
#endif /*  TFM_DEV_MODE  */
  Error_Handler();
}
#endif  /*  __ARMCC_VERSION */
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  Error_Handler();
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
