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
#include "string.h"
#include "stdio.h"
#include "addr_pages.h"
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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
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
  /* USER CODE BEGIN 2 */

  HAL_UART_Transmit(&huart1, (uint8_t*)"Start\n", 6, 100);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////// РАБОТА с ФЛЕШЕМ //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #define MYADDR ADDR_FLASH_PAGE_127

  static FLASH_EraseInitTypeDef EraseInitStruct; // структура для очистки флеша

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // постраничная очистка, FLASH_TYPEERASE_MASSERASE - очистка всего флеша
  EraseInitStruct.PageAddress = MYADDR; // адрес 127-ой страницы
  EraseInitStruct.NbPages = 1;                       // кол-во страниц для стирания
  //EraseInitStruct.Banks = FLASH_BANK_1; // FLASH_BANK_2 - банк №2, FLASH_BANK_BOTH - оба банка

  uint32_t page_error = 0; // переменная, в которую запишется адрес страницы при неудачном стирании

  char str[64] = {0,};

  ////////////////////////////// ОЧИСТКА ///////////////////////////////////
  HAL_FLASH_Unlock(); // разблокировать флеш

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK)
  {
	  uint32_t er = HAL_FLASH_GetError();
	  snprintf(str, 64, "ER %lu\n", er);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	  while(1){}
  }

  HAL_UART_Transmit(&huart1, (uint8_t*)"Erase OK\n", 9, 100);

  HAL_FLASH_Lock(); // заблокировать флеш
  ///////////////////////// КОНЕЦ ОЧИСТКИ //////////////////////////////

  // 0x00U - No error
  // 0x01U - Programming error
  // 0x02U - Write protection error
  // 0x04U - Option validity error

  ////////////////////////////// ЗАПИСЬ числа 16 bits ///////////////////////////////////
  HAL_FLASH_Unlock(); // разблокировать флеш

  uint32_t address = MYADDR; // запись в начало страницы 127
  uint16_t idata[] = {0x1941, 0x1945};    // массив из двух чисел для записи

  for(uint8_t i = 0; i < 2; i++)
  {
	  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, idata[i]) != HAL_OK)
	  {
		  uint32_t er = HAL_FLASH_GetError();
		  snprintf(str, 64, "ER %lu\n", er);
		  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
		  while(1){}
	  }

	  address = address + 2;
  }

  HAL_UART_Transmit(&huart1, (uint8_t*)"Write 16 bits OK\n", strlen("Write 16 bits OK\n"), 100);

  HAL_FLASH_Lock(); // заблокировать флеш
  /////////////////////// КОНЕЦ ЗАПИСИ числа 16 bits ///////////////////////////


  ////////////////////////////// ЧТЕНИЕ числа 16 bits ///////////////////////////////////
  address = MYADDR;

  for(uint16_t i = 0; i < 2; i++)
  {
	  uint16_t dig16 = *(uint16_t*)address; // читаем число по адресу

	  snprintf(str, 64, "READ_%d Dec: %d Hex: 0x%04X\n", i, dig16, dig16);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);

	  address = address + 2;
  }
  //////////////////////// КОНЕЦ ЧТЕНИЯ числа 16 bits ///////////////////////////////////


  ////////////////////////////// ЗАПИСЬ строки 8 bits ///////////////////////////////////
  HAL_FLASH_Unlock(); // разблокировать флеш

  address = MYADDR + 4; // смещаем адрес на 50 байт

  uint8_t data[] = " istarik.ru ";
  uint16_t data16 = 0, index0 = 0, index1 = 1;
  uint16_t len = strlen((char*)data);

  if(len % 2 != 0) // проверка кол-ва символов в массиве на чётность
  {
	  snprintf(str, 64, "ER uneven buff %d\n", len);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	  while(1){}
  }

  for(uint16_t i = 0; i < len / 2; i++)
  {
	  data16 = ((uint16_t)data[index1] << 8) | data[index0];
	  index0 = index0 + 2;
	  index1 = index1 + 2;

	  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data16) != HAL_OK)
	  {
		  uint32_t er = HAL_FLASH_GetError();
		  snprintf(str, 64, "ER %lu\n", er);
		  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
		  while(1){}
	  }

	  address = address + 2;
  }

  HAL_FLASH_Lock(); // заблокировать флеш

  HAL_UART_Transmit(&huart1, (uint8_t*)"Write 8 bits OK\n", strlen("Write 8 bits OK\n"), 100);
  /////////////////////// КОНЕЦ ЗАПИСИ строки 8 bits //////////////////////////


  ////////////////////////////// ЧТЕНИЕ строки 8 bits ///////////////////////////////////
  address = MYADDR + 4;

  char buf[64] = {0,};
  uint16_t index = 0;

  for(uint16_t i = 0; i < len / 2; i++)
  {
	  volatile uint32_t dig16 = *(uint32_t*)address;

	  buf[index++] = dig16 & 0xFF;
	  buf[index++] = (dig16 & 0xFF00) >> 8;

	  address = address + 2;
  }

  snprintf(str, 64, "Str: %s\n", buf);
  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
  //////////////////////// КОНЕЦ ЧТЕНИЯ строки 8 bits ///////////////////////////////////


  //////////////////////// ПРОЧИТАТЬ ПОЛЬЗОВАТЕЛЬСКИЕ БИТЫ - User Data  /////////////////////////////
  uint32_t user_bit = HAL_FLASHEx_OBGetUserData(OB_DATA_ADDRESS_DATA0);
  snprintf(str, 64, "User_bit_0: %lu\n", user_bit);
  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);

  user_bit = HAL_FLASHEx_OBGetUserData(OB_DATA_ADDRESS_DATA1);
  snprintf(str, 64, "User_bit_1: %lu\n", user_bit);
  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
  //////////////////////// КОНЕЦ ЧТЕНИЯ /////////////////////////////

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

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
