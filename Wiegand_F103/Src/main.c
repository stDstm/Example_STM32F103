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
#include "wiegand.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define D0        D0_Pin
#define D1        D1_Pin
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
volatile uint8_t wig_flag_inrt = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(wig_flag_inrt && GPIO_Pin == D0)
	{
		ReadD0();
	}
	else if(wig_flag_inrt && GPIO_Pin == D1)
	{
		ReadD1();
	}
}
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

  /////// раскомментить для складывания цифр в массив ///////
  /*uint8_t wig_dig[12] = {0,};
  memset(wig_dig, '\0', 12);
  uint8_t wdig = 0;
  uint32_t wcode = 0;
  int16_t wtype = 0;*/

  /* USER CODE END 2 */
 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(wig_available())
		{
			wig_flag_inrt = 0;
			uint32_t wcode = getCode();
			int16_t wtype = getWiegandType();
			wig_flag_inrt = 1;

			char str[64] = {0,};
			snprintf(str, 64, "HEX=0x%lX DEC=%lu, Protokol Wiegand-%d\n", wcode, wcode, wtype);
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		}



	  	/*if(wig_available())
	  	{
	  		wig_flag_inrt = 0;
	  		wcode = getCode();
	  		wtype = getWiegandType();
	  		wig_flag_inrt = 1;

	  		char str[64] = {0,};
	  		snprintf(str, 64, "HEX=0x%lX DEC=%lu, Protokol Wiegand-%d\n", wcode, wcode, wtype);
	  		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen((char*)str), 1000);

	  		if(wtype == 4)
	  		{
	  			if(wcode == 0x1B) // * сброс кнопок
	  			{
	  				HAL_UART_Transmit(&huart1, (uint8_t*)"Reset\n", strlen("Reset\n"), 1000);

	  				wdig = 0;
	  				memset(wig_dig, '\0', 12);
	  			}
	  			else if(wcode == 0x0D) // # Enter
	  			{
	  				HAL_UART_Transmit(&huart1, (uint8_t*)"Enter\n", strlen("Enter\n"), 1000);

	  				snprintf(str, 64, "Digits %s\n", wig_dig);
	  				HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen((char*)str), 1000);

	  				wdig = 0;
	  				memset(wig_dig, '\0', 12);
	  			}
	  			else
	  			{
	  				HAL_UART_Transmit(&huart1, (uint8_t*)"Reciv dig\n", strlen("Reciv dig\n"), 1000);

	  				sprintf((char*)&wig_dig[wdig++], "%d", (uint8_t)wcode);

	  				if(wdig > 11)
	  				{
	  					HAL_UART_Transmit(&huart1, (uint8_t*)"Buff overflow\n", strlen("Buff overflow\n"), 1000);

	  					wdig = 0;
	  					memset(wig_dig, '\0', 12);
	  				}
	  			}
	  		}
	  		else if(wtype > 4)
	  		{
	  			HAL_UART_Transmit(&huart1, (uint8_t*)"Cart\n", strlen("Cart\n"), 1000);

	  			snprintf(str, 64, "ID cart HEX=0x%lX DEC=%lu\n", wcode, wcode);
	  			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen((char*)str), 1000);

	  			wdig = 0;
	  			memset(wig_dig, '\0', 12);
	  		}
	  	}*/


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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : D0_Pin D1_Pin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
