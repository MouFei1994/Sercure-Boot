/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/ADC/ADC_AnalogWatchdog_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a ADC peripheral
  *          with ADC analog watchdog to monitor a channel and detect 
  *          when the corresponding conversion data is out of window thresholds.
  *          This example is based on the STM32L5xx ADC LL API;
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the 
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Definitions of ADC hardware constraints delays */
/* Note: Only ADC IP HW delays are defined in ADC LL driver driver,           */
/*       not timeout values:                                                  */
/*       Timeout values for ADC operations are dependent to device clock      */
/*       configuration (system clock versus ADC clock),                       */
/*       and therefore must be defined in user application.                   */
/*       Refer to @ref ADC_LL_EC_HW_DELAYS for description of ADC timeout     */
/*       values definition.                                                   */

  /* Timeout values for ADC operations. */
  /* (calibration, enable settling time, disable settling time, ...)          */
  /* Values defined to be higher than worst cases: low clock frequency,       */
  /* maximum prescalers.                                                      */
  #define ADC_CALIBRATION_TIMEOUT_MS       (   1U)
  #define ADC_ENABLE_TIMEOUT_MS            (   1U)
  #define ADC_DISABLE_TIMEOUT_MS           (   1U)
  #define ADC_STOP_CONVERSION_TIMEOUT_MS   (   1U)
  #define ADC_CONVERSION_TIMEOUT_MS        ( 500U)
  
  /* Delay between ADC end of calibration and ADC enable.                     */
  /* Delay estimation in CPU cycles: Case of ADC enable done                  */
  /* immediately after ADC calibration, ADC clock setting slow                */
  /* (LL_ADC_CLOCK_ASYNC_DIV32). Use a higher delay if ratio                  */
  /* (CPU clock / ADC clock) is above 32.                                     */
  #define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)
  

/* Definitions of environment analog values */
  /* Value of analog reference voltage (Vref+), connected to analog voltage   */
  /* supply Vdda (unit: mV).                                                  */
  #define VDDA_APPLI                       (3300UL)

/* Definitions of data related to this example */
  /* Definition of ADCx analog watchdog window thresholds */
  /* Value of ADC analog watchdog threshold high */
  #define ADC_AWD_THRESHOLD_HIGH           (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) / 2)
  /* Value of ADC analog watchdog threshold low  */
  #define ADC_AWD_THRESHOLD_LOW            (   0UL)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Variable to report status of ADC analog watchdog 1:                        */
/*  0: ADC conversion data into AWD window                                    */
/*  1: ADC conversion data out of AWD window                                  */
__IO   uint8_t ubAnalogWatchdog1Status = 0U; /* Variable set into analog watchdog 1 interruption callback */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

void     Activate_ADC(void);
void     LED_On(void);
void     LED_Off(void);
void     LED_Blinking(uint32_t Period);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  LL_PWR_DisableUCPDDeadBattery();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  
  /* Activate ADC */
  /* Perform ADC activation procedure to make it ready to convert. */
  Activate_ADC();
  
  /* Start ADC group regular conversion */
  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of this feature is conditioned to     */
  /*       ADC state:                                                         */
  /*       ADC must be enabled without conversion on going on group regular,  */
  /*       without ADC disable command on going.                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if ((LL_ADC_IsEnabled(ADC1) == 1)               &&
      (LL_ADC_IsDisableOngoing(ADC1) == 0)        &&
      (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
  {
    LL_ADC_REG_StartConversion(ADC1);
  }
  else
  {
    /* Error: ADC conversion start could not be performed */
    LED_Blinking(LED_BLINK_ERROR);
  }
 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Note: LED state depending on ADC analog watchdog 1 status              */
    /*       and status variable "ubAnalogWatchdog1Status"                    */
    /*       are set into ADC analog watchdog 1 IRQ handler,                  */
    /*       refer to function "AdcAnalogWatchdog1_Callback()".               */
    /*       After analog watchdog interruption, press on push button         */
    /*       to rearm ADC analog watchdog to be ready for another trig,       */
    /*       refer to function "UserButton_Callback()".                       */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  LL_RCC_MSI_Enable();

   /* Wait till MSI is ready */
  while(LL_RCC_MSI_IsReady() != 1)
  {
  }

  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  LL_RCC_MSI_SetCalibTrimming(0);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 55, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1�s transition state at intermediate medium speed clock based on DWT*/
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < 100);

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(110000000);

  LL_SetSystemCoreClock(110000000);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  /**ADC1 GPIO Configuration
  PC3   ------> ADC1_IN4
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_Init 1 */
  
  /* Enable GPIO Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  
  /* Configure GPIO in analog mode to be used as ADC input */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_ANALOG);
  
  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable ADC1 interruptions */
  NVIC_SetPriority(ADC1_2_IRQn, 0);
  NVIC_EnableIRQ(ADC1_2_IRQn);
  
  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

   /* Disable ADC deep power down (enabled by default after reset state) */
   LL_ADC_DisableDeepPowerDown(ADC1);
   /* Enable ADC internal voltage regulator */
   LL_ADC_EnableInternalRegulator(ADC1);
   /* Delay for ADC internal voltage regulator stabilization. */
   /* Compute number of CPU cycles to wait for, from delay in us. */
   /* Note: Variable divided by 2 to compensate partially */
   /* CPU processing cycles (depends on compilation optimization). */
   /* Note: If system core clock frequency is below 200kHz, wait time */
   /* is only a few CPU processing cycles. */
   uint32_t wait_loop_index;
   wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
   while(wait_loop_index != 0)
     {
   wait_loop_index--;
     }
  ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
  /** Configure Analog WatchDog 1
  */
  LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_ALL_CHANNELS_REG);
  LL_ADC_ConfigAnalogWDThresholds(ADC1, LL_ADC_AWD1, ADC_AWD_THRESHOLD_HIGH, ADC_AWD_THRESHOLD_LOW);
  LL_ADC_EnableIT_AWD1(ADC1);
  /** Configure Regular Channel
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_47CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SINGLE_ENDED);
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  /**/
  LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTC, LL_EXTI_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_DOWN);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Perform ADC activation procedure to make it ready to convert
  *         (ADC instance: ADC1).
  * @note   Operations:
  *         - ADC instance
  *           - Disable deep power down
  *           - Enable internal voltage regulator
  *           - Run ADC self calibration
  *           - Enable ADC
  *         - ADC group regular
  *           none: ADC conversion start-stop to be performed
  *                 after this function
  *         - ADC group injected
  *           none: ADC conversion start-stop to be performed
  *                 after this function
  * @param  None
  * @retval None
  */
void Activate_ADC(void)
{
  __IO uint32_t wait_loop_index = 0U;
  #if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0U; /* Variable used for timeout management */
  #endif /* USE_TIMEOUT */
  
  /*## Operation on ADC hierarchical scope: ADC instance #####################*/
  
  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Disable ADC deep power down (enabled by default after reset state) */
    LL_ADC_DisableDeepPowerDown(ADC1);
    
    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);
    
    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }
    
    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    
    /* Poll for ADC effectively calibrated */
    #if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
    #endif /* USE_TIMEOUT */
    
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
    #if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {
        /* Time-out occurred. Set LED to blinking mode */
        LED_Blinking(LED_BLINK_ERROR);
        }
      }
    #endif /* USE_TIMEOUT */
    }
    
    /* Delay between ADC end of calibration and ADC enable.                   */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }
    
    /* Enable ADC */
    LL_ADC_Enable(ADC1);
    
    /* Poll for ADC ready to convert */
    #if (USE_TIMEOUT == 1)
    Timeout = ADC_ENABLE_TIMEOUT_MS;
    #endif /* USE_TIMEOUT */
    
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
    #if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {
        /* Time-out occurred. Set LED to blinking mode */
        LED_Blinking(LED_BLINK_ERROR);
        }
      }
    #endif /* USE_TIMEOUT */
    }
    
    /* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
    /*       status afterwards.                                               */
    /*       This flag should be cleared at ADC Deactivation, before a new    */
    /*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
  }
  
  /*## Operation on ADC hierarchical scope: ADC group regular ################*/
  /* Note: No operation on ADC group regular performed here.                  */
  /*       ADC group regular conversions to be performed after this function  */
  /*       using function:                                                    */
  /*       "LL_ADC_REG_StartConversion();"                                    */
  
  /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
  /* Note: No operation on ADC group injected performed here.                 */
  /*       ADC group injected conversions to be performed after this function */
  /*       using function:                                                    */
  /*       "LL_ADC_INJ_StartConversion();"                                    */
  
}

/**
  * @brief  Turn-on LED1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
}

/**
  * @brief  Turn-off LED1.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED1 off */
  LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
  
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    LL_mDelay(Period);
  }
}


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

/**
  * @brief  Function to manage IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Rearm ADC analog watchdog to be ready for another trig */

  /* Turn LED1 off */
  LED_Off();
  
  /* Reset status variable of ADC analog watchdog 1 */
  ubAnalogWatchdog1Status = 0;
  
  /* Clear flag ADC analog watchdog 1 */
  LL_ADC_ClearFlag_AWD1(ADC1);
  
  /* Enable ADC analog watchdog 1 interruption */
  LL_ADC_EnableIT_AWD1(ADC1);
}

/**
  * @brief  ADC analog watchdog 1 interruption callback
  * @note   This function is executed when the ADC conversion data is 
  *         out of analog watchdog 1 window thresholds.
  * @retval None
  */
void AdcAnalogWatchdog1_Callback()
{
  /* Disable ADC analog watchdog 1 interruption */
  LL_ADC_DisableIT_AWD1(ADC1);
  
  /* Update status variable of ADC analog watchdog 1 */
  ubAnalogWatchdog1Status = 1;
  
  /* Set LED depending on ADC analog watchdog status */
  /* - Turn-on if voltage is out of AWD window */
  LED_On();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

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
  /* USER CODE BEGIN 6 */

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
