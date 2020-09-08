/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
HASH_HandleTypeDef hhash;
__ALIGN_BEGIN static const uint8_t pKeyHASH[261] __ALIGN_END = {
                            0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,0x20,0x70,
                            0x72,0x6f,0x63,0x65,0x73,0x73,0x6f,0x72,0x20,0x69,
                            0x73,0x20,0x61,0x20,0x66,0x75,0x6c,0x6c,0x79,0x20,
                            0x63,0x6f,0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                            0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,0x74,0x61,
                            0x74,0x69,0x6f,0x6e,0x20,0x6f,0x66,0x20,0x74,0x68,
                            0x65,0x20,0x73,0x65,0x63,0x75,0x72,0x65,0x20,0x68,
                            0x61,0x73,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                            0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,0x2d,0x31,
                            0x29,0x2c,0x20,0x74,0x68,0x65,0x20,0x4d,0x44,0x35,
                            0x20,0x28,0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x2d,
                            0x64,0x69,0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                            0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,0x29,0x20,
                            0x68,0x61,0x73,0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,
                            0x69,0x74,0x68,0x6d,0x20,0x61,0x6e,0x64,0x20,0x74,
                            0x68,0x65,0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                            0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,0x68,0x20,
                            0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x20,0x61,0x75,
                            0x74,0x68,0x65,0x6e,0x74,0x69,0x63,0x61,0x74,0x69,
                            0x6f,0x6e,0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                            0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x73,
                            0x75,0x69,0x74,0x61,0x62,0x6c,0x65,0x20,0x66,0x6f,
                            0x72,0x20,0x61,0x20,0x76,0x61,0x72,0x69,0x65,0x74,
                            0x79,0x20,0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                            0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,0x2a,0x2a,
                            0x2a,0x20,0x53,0x54,0x4d,0x33,0x32,0x20,0x2a,0x2a,
                            0x2a};

/* USER CODE BEGIN PV */
__ALIGN_BEGIN const uint8_t aInput[261] __ALIGN_END =
                        {0x54,0x68,0x65,0x20,0x68,0x61,0x73,0x68,
                         0x20,0x70,0x72,0x6f,0x63,0x65,0x73,0x73,
                         0x6f,0x72,0x20,0x69,0x73,0x20,0x61,0x20,
                         0x66,0x75,0x6c,0x6c,0x79,0x20,0x63,0x6f,
                         0x6d,0x70,0x6c,0x69,0x61,0x6e,0x74,0x20,
                         0x69,0x6d,0x70,0x6c,0x65,0x6d,0x65,0x6e,
                         0x74,0x61,0x74,0x69,0x6f,0x6e,0x20,0x6f,
                         0x66,0x20,0x74,0x68,0x65,0x20,0x73,0x65,
                         0x63,0x75,0x72,0x65,0x20,0x68,0x61,0x73,
                         0x68,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,
                         0x74,0x68,0x6d,0x20,0x28,0x53,0x48,0x41,
                         0x2d,0x31,0x29,0x2c,0x20,0x74,0x68,0x65,
                         0x20,0x4d,0x44,0x35,0x20,0x28,0x6d,0x65,
                         0x73,0x73,0x61,0x67,0x65,0x2d,0x64,0x69,
                         0x67,0x65,0x73,0x74,0x20,0x61,0x6c,0x67,
                         0x6f,0x72,0x69,0x74,0x68,0x6d,0x20,0x35,
                         0x29,0x20,0x68,0x61,0x73,0x68,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x61,0x6e,0x64,0x20,0x74,0x68,0x65,
                         0x20,0x48,0x4d,0x41,0x43,0x20,0x28,0x6b,
                         0x65,0x79,0x65,0x64,0x2d,0x68,0x61,0x73,
                         0x68,0x20,0x6d,0x65,0x73,0x73,0x61,0x67,
                         0x65,0x20,0x61,0x75,0x74,0x68,0x65,0x6e,
                         0x74,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,
                         0x20,0x63,0x6f,0x64,0x65,0x29,0x20,0x61,
                         0x6c,0x67,0x6f,0x72,0x69,0x74,0x68,0x6d,
                         0x20,0x73,0x75,0x69,0x74,0x61,0x62,0x6c,
                         0x65,0x20,0x66,0x6f,0x72,0x20,0x61,0x20,
                         0x76,0x61,0x72,0x69,0x65,0x74,0x79,0x20,
                         0x6f,0x66,0x20,0x61,0x70,0x70,0x6c,0x69,
                         0x63,0x61,0x74,0x69,0x6f,0x6e,0x73,0x2e,
                         0x2a,0x2a,0x2a,0x20,0x53,0x54,0x4d,0x33,
                         0x32,0x20,0x2a,0x2a,0x2a};

__ALIGN_BEGIN static uint8_t aSHA1Digest[20] __ALIGN_END;

__ALIGN_BEGIN static uint8_t aExpectSHA1Digest[20] __ALIGN_END = {0x2b, 0x47, 0xef, 0x34, 0xf6, 0x66, 0x58, 0xbe,
                                                                  0x3b, 0x2a, 0xc6, 0xec, 0x58, 0x85, 0xf2, 0x1e,
                                                                  0xd5, 0x9f, 0xdd, 0xad};

__ALIGN_BEGIN static uint8_t aMD5Digest[16] __ALIGN_END;

__ALIGN_BEGIN static uint8_t aExpectMD5Digest[16] __ALIGN_END = {0x87, 0x7e, 0xbb, 0xbf, 0xc8, 0xa9, 0xc0, 0x85,
                                                                 0x78, 0xa2, 0xc3, 0x31, 0xd6, 0x7e, 0xc7, 0x0e};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_HASH_Init(void);
/* USER CODE BEGIN PFP */

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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LEDs */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_HASH_Init();
  /* USER CODE BEGIN 2 */
  /****************************************************************************/
  /*************************** HMAC-SHA1 **************************************/
  /****************************************************************************/
  /* Compute HMAC-SHA1 */
  if (HAL_HMAC_SHA1_Start(&hhash, (uint8_t *)aInput, 261, aSHA1Digest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }
  /* Compare computed digest with expected one */
  if (memcmp(aSHA1Digest, aExpectSHA1Digest, sizeof(aExpectSHA1Digest) / sizeof(aExpectSHA1Digest[0])) != 0)
  {
    BSP_LED_On(LED_RED);
  }
  else
  {
    BSP_LED_On(LED_GREEN);
  }

  /****************************************************************************/
  /*************************** HMAC-MD5 ***************************************/
  /****************************************************************************/
  if(HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  hhash.Init.DataType = HASH_DATATYPE_8B;
  hhash.Init.KeySize = 261;
  hhash.Init.pKey = (uint8_t *)pKeyHASH;
  
  if(HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compute HMAC-MD5 */
  if (HAL_HMAC_MD5_Start(&hhash, (uint8_t *)aInput, 261, aMD5Digest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }
  /* Compare computed digest with expected one */
  if (memcmp(aMD5Digest, aExpectMD5Digest, sizeof(aExpectMD5Digest) / sizeof(aExpectMD5Digest[0])) != 0)
  {
    Error_Handler();
  }
  else
  {
    BSP_LED_On(LED_BLUE);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Init.DataType = HASH_DATATYPE_8B;
  hhash.Init.KeySize = 261;
  hhash.Init.pKey = (uint8_t *)pKeyHASH;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Turn LED_RED on */
  BSP_LED_On(LED_RED);
  while (1)
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
  while (1)
  {
  } 
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
