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
#include "nokia5110_LCD.h"
#include "string.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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
  /* USER CODE BEGIN 2 */

  // в Кубе настроить 5 пинов как Output, например PA3 - PA7 (можно переименовать их в соответствии с назначением)
  // и прописать в функции ниже - ПОРТ, ПИН. Подключать в соответствии с названиями контактов экрана.
  LCD_setRST(GPIOA, RST_Pin);
  LCD_setCE(GPIOA, CE_Pin);
  LCD_setDC(GPIOA, DC_Pin);
  LCD_setDIN(GPIOA, DIN_Pin);
  LCD_setCLK(GPIOA, CLK_Pin);

  LCD_init();

  uint16_t i = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // размер экрана 84x48 пикселей

	  LCD_clrScr(); // очистить экран

	  /////////////////////////////// вывод строк /////////////////////////////////
	  LCD_print("Hello", 0, 0);
	  HAL_Delay(200);
	  LCD_print("Hello", 6, 1);
	  HAL_Delay(200);
	  LCD_print("Hello", 12, 2);
	  HAL_Delay(200);
	  LCD_print("Hello", 18, 3);
	  HAL_Delay(200);
	  LCD_print("Hello", 24, 4);
	  HAL_Delay(200);
	  LCD_print("Hello", 30, 5);
	  HAL_Delay(200);

	  LCD_print("World", 0, 0);
	  HAL_Delay(200);
	  LCD_print("World", 6, 1);
	  HAL_Delay(200);
	  LCD_print("World", 12, 2);
	  HAL_Delay(200);
	  LCD_print("World", 18, 3);
	  HAL_Delay(200);
	  LCD_print("World", 24, 4);
	  HAL_Delay(200);
	  LCD_print("World", 30, 5);
	  HAL_Delay(500);

	  /////////////////////////////////////

	  char msg[6] = {0,};
	  snprintf(msg, 6, "%d", i++);
	  LCD_print(msg, 50, 0);

	  HAL_Delay(1000);
	  LCD_clrScr(); // очистить экран

	  /////////////////////////////////////

	  for(uint8_t i = 0; i < 6; i++)
	  {
		  LCD_print("istarik.ru stD", 0, i);
	  }

	  HAL_Delay(1000);
	  LCD_clrScr(); // очистить экран

	  ////////////////////////////////////

	  // далее показаны функции работающие через буфер - изображение копируется в буфер, а для вывода его на экран
	  // нужно воспользоваться одной из функций:
	  // LCD_refreshScr() - просто выводит на экран
	  // LCD_refreshArea(x, y, x, y) - выведет изображение в указанной области (остальная область останется не тронутой)

	  ///////////////////////////////////// линии ////////////////////////////////////////
	  LCD_drawHLine(20, 24, 44); // горизонтальная линия: слева, от верха, длина линии
	  LCD_refreshScr();

	  HAL_Delay(1000);

	  LCD_drawVLine(42, 4, 40); // вертикальная линия: слева, от верха, длина линии
	  LCD_refreshScr();

	  HAL_Delay(1000);

	  ///////////////////////////////////// произвольная линия ////////////////////////////////////////
	  LCD_drawLine(10, 10, 74, 38); // произвольная линия: начало - слева, от верха | конец - слева, от верха
	  LCD_refreshScr();

	  HAL_Delay(1000);

	  LCD_drawLine(10, 38, 74, 10); // произвольная линия: начало - слева, от верха | конец - слева, от верха
	  LCD_refreshScr();

	  HAL_Delay(1000);
	  LCD_clrScr(); // очистить экран

	  ////////////////////////////////// рисует один пиксель ///////////////////////////////////////
	  for(uint8_t i = 20; i < 64; i++)
	  {
		  LCD_setPixel(i, 24, true); // рисует один пиксель
		  LCD_refreshScr();
		  HAL_Delay(50);
	  }

	  HAL_Delay(500);

	  for(uint8_t i = 20; i < 64; i++)
	  {
		  LCD_setPixel(i, 24, false); // стирает один пиксель
		  LCD_refreshScr();
		  HAL_Delay(50);
	  }

	  HAL_Delay(500);


	  for(uint8_t i = 20; i < 64; i++)
	  {
		  LCD_setPixel(i, 24, true); // рисует один пиксель
		  LCD_setPixel(i, 25, true);
		  LCD_setPixel(i + 1, 24, true);
		  LCD_setPixel(i + 1, 25, true);
		  LCD_refreshScr();
		  HAL_Delay(100);

		  LCD_setPixel(i - 1, 24, false); // стирает один пиксель
		  LCD_setPixel(i - 1, 25, false);
		  LCD_setPixel(i - 2, 24, false);
		  LCD_setPixel(i - 2, 25, false);
		  LCD_refreshScr();
		  HAL_Delay(50);
	  }

	  HAL_Delay(100);

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
  HAL_GPIO_WritePin(GPIOA, CLK_Pin|DIN_Pin|DC_Pin|CE_Pin 
                          |RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CLK_Pin DIN_Pin DC_Pin CE_Pin 
                           RST_Pin */
  GPIO_InitStruct.Pin = CLK_Pin|DIN_Pin|DC_Pin|CE_Pin 
                          |RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
