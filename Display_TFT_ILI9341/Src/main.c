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

#include "ILI9341_GFX.h"
#include "fonts.h"
#include "img.h"
#include "xpt2046_touch.h"
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  //HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // этот таймер используется у меня для подсветки экрана
  HAL_UART_Transmit(&huart1, (uint8_t*)"Start\n", 6, 1000);

  __HAL_SPI_ENABLE(DISP_SPI_PTR);

  DISP_CS_UNSELECT;
  TOUCH_CS_UNSELECT; // это нужно только если есть тач

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Init(); // инициализация дисплея

  /////////////////////////// далее демонстрируются различные пользовательские функции ////////////////////////////
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2); // установка ориентации экрана (варианты в файле ILI9341_GFX.h)

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Fill_Screen(MYFON); // заливка всего экрана цветом (цвета в файле ILI9341_GFX.h)

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /* вывод строк разными шрифтами (шрифты определены в файле fonts.h, а массивы шрифтов в файле fonts.c)
  первый и второй аргумент это начало координат (справа, сверху), четвёртый аргумент шрифт
  два последних аргумента это цвет шрифта и цвет фона шрифта */

  ILI9341_WriteString(10, 10, "Hello World", Font_7x10, WHITE, MYFON); // можно передавать непосредственно текст
  ILI9341_WriteString(20, 30, "Hello World", Font_11x18, WHITE, MYFON);
  ILI9341_WriteString(30, 60, "Hello World", Font_16x26, BLUE, DARKGREEN);

  char txt_buf[] = "Hello World";
  ILI9341_WriteString(40, 96, txt_buf, Font_16x26, RED, GREEN); // можно передавать массив

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Pixel(100, 100, WHITE); // рисует пиксель (координаты и цвет)
  HAL_Delay(300);
  ILI9341_Draw_Pixel(102, 100, MAROON);
  HAL_Delay(300);
  ILI9341_Draw_Pixel(100, 102, BLUE);
  HAL_Delay(300);
  ILI9341_Draw_Pixel(102, 102, RED);
  HAL_Delay(300);

  for(uint8_t i = 0; i < 100; i++)
  {
	  ILI9341_Draw_Pixel(i, 20, WHITE);
	  HAL_Delay(10);
  }

  for(uint8_t i = 0; i < 100; i++)
  {
	  ILI9341_Draw_Pixel(40, i, BLUE);
	  HAL_Delay(10);
  }

  for(uint8_t i = 0; i < 100; i++)
  {
	  ILI9341_Draw_Pixel(i, i, RED);
	  HAL_Delay(10);
  }

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Rectangle(10, 10, 50, 70, WHITE); // рисует закрашеный прямоугольник (первые два аргумента это начальные координаты, а следующие два это ширина и высота)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Horizontal_Line(10, 10, 200, WHITE); // рисует горизонтальную линию (первые два аргумента это начальные координаты, а третий длина)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Vertical_Line(10, 10, 200, WHITE); // рисует вертикальную линию (первые два аргумента это начальные координаты, а третий длина)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Hollow_Circle(100, 100, 50, WHITE); // рисует прозрачный круг (первые два аргумента это координаты центра, а третий радиус)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Filled_Circle(150, 100, 40, WHITE); // рисует закрашеный круг (первые два аргумента это координаты центра, а третий радиус)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Hollow_Rectangle_Coord(10, 10, 50, 70, WHITE); // рисует прозрачный прямоугольник (первые два аргумента это начальные координаты, а вторые два конечные)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ILI9341_Draw_Filled_Rectangle_Coord(20, 20, 70, 60, WHITE); // рисует закрашеный прямоугольник (первые два аргумента это начальные координаты, а вторые два конечные)

  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);

  ////////////////////////////////// ВЫВОД КАРТИНКИ ИЗ ВНУТРЕННЕЙ ПАМЯТИ //////////////////////////////////////////
  uint16_t size_img = sizeof(img_logo); // размер картинки в байтах (картинка лежит в файле img.h)

  ILI9341_Draw_Image(img_logo, 30, 30, 80, 80, size_img); // подробности см. статье

  HAL_Delay(1000);

  //////////////////////////////////// смена ориентации экрана ///////////////////////////////////////

  ILI9341_Fill_Screen(MYFON);
  ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
  ILI9341_Draw_Image(img_logo, 30, 30, 80, 80, size_img);
  ILI9341_WriteString(30, 120, "SCREEN_VERTICAL_1", Font_11x18, WHITE, MYFON);
  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
  ILI9341_Draw_Image(img_logo, 30, 30, 80, 80, size_img);
  ILI9341_WriteString(30, 120, "SCREEN_HORIZONTAL_1", Font_11x18, WHITE, MYFON);
  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);
  ILI9341_Set_Rotation(SCREEN_VERTICAL_2);
  ILI9341_Draw_Image(img_logo, 30, 30, 80, 80, size_img);
  ILI9341_WriteString(30, 120, "SCREEN_VERTICAL_2", Font_11x18, WHITE, MYFON);
  HAL_Delay(1000);

  ILI9341_Fill_Screen(MYFON);
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
  ILI9341_Draw_Image(img_logo, 30, 30, 80, 80, size_img);
  ILI9341_WriteString(30, 120, "SCREEN_HORIZONTAL_2", Font_11x18, WHITE, MYFON);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  char buf[64] = {0,};

  uint8_t flag_press = 1;
  uint32_t time_press = 0;

  uint8_t flag_hold = 1;
  uint32_t timme_hold = 0;

  uint16_t x = 0;
  uint16_t y = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
{
  if(flag_press && HAL_GPIO_ReadPin(IRQ_GPIO_Port, IRQ_Pin) == GPIO_PIN_RESET) // если нажат тачскрин
  {
	  x = 0;
	  y = 0;

	  TOUCH_CS_UNSELECT;
	  DISP_CS_UNSELECT;

	  HAL_SPI_DeInit(DISP_SPI_PTR);
	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	  HAL_SPI_Init(DISP_SPI_PTR);

	  if(ILI9341_TouchGetCoordinates(&x, &y))
	  {
		  flag_press = 0;

		  //////// вывод координат в уарт для отладки ////////
		  snprintf(buf, 64, "X = %d, Y = %d\n", x, y);
		  HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 100);
		  buf[strlen(buf) - 1] = '\0';
		  //////////////////////////////////////////////////////
	  }

	  HAL_SPI_DeInit(DISP_SPI_PTR);
	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	  HAL_SPI_Init(DISP_SPI_PTR);

	  __HAL_SPI_ENABLE(DISP_SPI_PTR);
	  DISP_CS_SELECT;

	  //////// вывод координат на экран для отладки ////////
	  ILI9341_Fill_Screen(MYFON);
	  ILI9341_WriteString(10, 120, buf, Font_11x18, WHITE, MYFON);
	  //////////////////////////////////////////////////////

	  if(x > 250 && x < 285 && y > 65 && y < 96) // если нажатие происходит в области этих координат
	  {
		  // что-то делаем
	  }
	  else if(x > 120 && x < 160 && y > 50 && y < 90) // если нажатие происходит в области этих координат
	  {
		  // что-то делаем
	  }
	  ///////////////// если нажатие происходит c удержанием кнопки ////////////////
	  else if(x > 5 && x < 90 && y > 160 && y < 230 && flag_hold) // первая кнопка
	  {
		  flag_hold = 0;
		  timme_hold = HAL_GetTick();
		  // здесь ничего не делаем
	  }
	  else if(x > 100 && x < 200 && y > 160 && y < 230 && flag_hold) // вторая кнопка
	  {
		  flag_hold = 0;
		  timme_hold = HAL_GetTick();
		  // здесь ничего не делаем
	  }


	  ///////////////////////////
	  time_press = HAL_GetTick();
  }

  if(!flag_press && (HAL_GetTick() - time_press) > 200) // задержка до следующего нажатия
  {
	  flag_press = 1;
  }


  //////////////////////////// удержание кнопки //////////////////////////////
  if(!flag_hold && HAL_GPIO_ReadPin(IRQ_GPIO_Port, IRQ_Pin) != GPIO_PIN_RESET)
  {
	  flag_hold = 1;
  }

  if(!flag_hold && (HAL_GetTick() - timme_hold) > 2000) // 2 sek удержание кнопки
  {
	  if(HAL_GPIO_ReadPin(IRQ_GPIO_Port, IRQ_Pin) == GPIO_PIN_RESET)
	  {
		  if(x > 5 && x < 90 && y > 160 && y < 230) // первая кнопка
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)"LONG PRESS_1\n", 13, 100); // отладка
			  ILI9341_WriteString(10, 150, "LONG PRESS_1", Font_11x18, WHITE, MYFON); // отладка
			  // что-то делаем
		  }
		  else if(x > 100 && x < 200 && y > 160 && y < 230) // вторая кнопка
		  {
			  HAL_UART_Transmit(&huart1, (uint8_t*)"LONG PRESS_2\n", 13, 100); // отладка
			  ILI9341_WriteString(10, 150, "LONG PRESS_2", Font_11x18, WHITE, MYFON); // отладка
			  // что-то делаем
		  }
	  }

	  flag_hold = 1;
  }
  /////////////////////////////////////////////////////////////////////////////


// далее какой-то ваш код, который не должен сильно тормозить цикл

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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 71;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

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
  HAL_GPIO_WritePin(GPIOB, TOUCH_CS_Pin|TFT_CS_Pin|TFT_RST_Pin|TFT_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TOUCH_CS_Pin TFT_CS_Pin TFT_RST_Pin TFT_DC_Pin */
  GPIO_InitStruct.Pin = TOUCH_CS_Pin|TFT_CS_Pin|TFT_RST_Pin|TFT_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IRQ_Pin */
  GPIO_InitStruct.Pin = IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IRQ_GPIO_Port, &GPIO_InitStruct);

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
