/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "w25qxx.h"
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
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Start W25QXX\n", 13, 1000);

  ///////////////////////////////////// ИНИЦИАЛИЗАЦИЯ ФЛЕШКИ ////////////////////////////////
  W25qxx_Init();

  ///////////////////////////////////// ОЧИСТКА ВСЕЙ ФЛЕШКИ ////////////////////////////////
  //W25qxx_EraseChip();

  ///////////////////////////////////// ОЧИСТКА БЛОКА ////////////////////////////////
  W25qxx_EraseBlock(0); // 65536 байт

  ///////////////////////////////////// ОЧИСТКА СЕКТОРА ////////////////////////////////
  //W25qxx_EraseSector(0); // 4096 байт

  ///////////////////////////////////// ЗАПИСЬ ПОБАЙТНО ////////////////////////////////
  uint8_t b0 = 's';
  uint8_t b1 = 't';
  uint8_t b2 = 'D';

  W25qxx_WriteByte(b0, 25);
  W25qxx_WriteByte(b1, 26);
  W25qxx_WriteByte(b2, 27);

  ///////////////////////////////////// ЧТЕНИЕ ПОБАЙТНО ////////////////////////////////
  uint8_t buf[64] = {0,};

  W25qxx_ReadByte(&buf[0], 25);
  W25qxx_ReadByte(&buf[1], 26);
  W25qxx_ReadByte(&buf[2], 27);

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen((char*)buf), 100);
  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"\r\n", 2, 100);

  ///////////////////////////////////// ЗАПИСЬ СТРАНИЦЫ ////////////////////////////////
  uint8_t w_buf[] = "istarik.ru";

  W25qxx_WritePage(w_buf, 0, 28, 10);

  ///////////////////////////////////// ЧТЕНИЕ СТРАНИЦЫ ////////////////////////////////
  memset(buf, 0, 64);
  W25qxx_ReadPage(buf, 0, 28, 10);

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen((char*)buf), 100);
  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"\r\n", 2, 100);

  ///////////////////////////////////// ЗАПИСЬ СЕКТОРА ////////////////////////////////
  W25qxx_WriteSector(w_buf, 0, 1350, 10);

  ///////////////////////////////////// ЧТЕНИЕ СЕКТОРА ////////////////////////////////
  memset(buf, 0, 64);
  W25qxx_ReadSector(buf, 0, 1350, 10);

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen((char*)buf), 100);
  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"\r\n", 2, 100);

  ///////////////////////////////////// ЗАПИСЬ БЛОКА ////////////////////////////////
  W25qxx_WriteBlock(w_buf, 0, 9350, 10);

  ///////////////////////////////////// ЧТЕНИЕ БЛОКА ////////////////////////////////
  memset(buf, 0, 64);
  W25qxx_ReadBlock(buf, 0, 9350, 10);

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen((char*)buf), 100);
  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"\r\n", 2, 100);


  /////////////////////////////////// ЧТЕНИЕ ЛЮБОГО КОЛИЧЕСТВА БАЙТ /////////////////
  memset(buf, 0, 64);
  W25qxx_ReadBytes(buf, 9350, 10);

  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen((char*)buf), 100);
  HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"\r\n", 2, 100);


  /////////////////////////////////// СТЁРТА ЛИ СТРАНИЦА /////////////////////
  uint8_t clear = W25qxx_IsEmptyPage(0, 40);

  if(clear) HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Clear P\n", 8, 100);
  else HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Not clear P\n", 12, 100);

  /////////////////////////////////// СТЁРТ ЛИ СЕКТОР //////////////////////
  clear = W25qxx_IsEmptySector(0, 1360);

  if(clear) HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Clear S\n", 8, 100);
  else HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Not clear S\n", 12, 100);

  /////////////////////////////////// СТЁРТ ЛИ БЛОК ////////////////////////
  clear = W25qxx_IsEmptyBlock(0, 9360);

  if(clear) HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Clear B\n", 8, 100);
  else HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Not clear B\n", 12, 100);

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

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
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
