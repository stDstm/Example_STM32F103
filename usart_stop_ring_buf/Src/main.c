/* USER CODE BEGIN Header */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUF_OUT 256
// размер входного буфера в файле main.h
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint8_t buff_main[SIZE_BUF] = {0,};
volatile myuBUF_t index_r = 0;
volatile myuBUF_t index_w = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char ch = 65;
char *p;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		ch++;
		if(ch > 89) ch = 65;
		p = (char*)&ch;
		HAL_UART_Transmit(&huart2, (uint8_t*)p, 1, 1000);
	}
}

uint8_t GetByte()
{
	myuBUF_t new_dat = (myuBUF_t)index_w - (myuBUF_t)index_r; // кол-во новых байт в буфере

	if(new_dat > 0)
	{
		uint8_t sym = buff_main[index_r++ & BUFFER_MASK]; // прочитать символ из буфера и инкрементировать индекс
		return sym;
	}

	return 0;
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
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // прерывание при появлении данных в приёмном регистре - DR
  uint8_t read_buff[SIZE_BUF + 1] = {0,}; // + 1 для корректного вывода на печать

  // для вывода на печать
  char trans_str[BUF_OUT] = {0,};
  char dot_str[SIZE_BUF * 2 + 10] = {0,};
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(595); 

	  myuBUF_t new_dat = (myuBUF_t)index_w - (myuBUF_t)index_r; // кол-во новых байт в буфере

	  /*if(new_dat > 0)
	  {
		  for(myuBUF_t i = 0; i < 1; i++) // указать кол-во считываемых символов
		  {
			  read_buff[i] = GetByte();
		  }

		  snprintf(trans_str, SIZE_BUF + 12, "Buff_main: %s", buff_main);
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
		  HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 1000);

		  snprintf(trans_str, BUF_OUT - 1, "Read_buff: %s \nNew_dat: %d, index_r: %d, index_w: %d_END\n------------\n", read_buff, new_dat, (index_r & BUFFER_MASK), (index_w & BUFFER_MASK));
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
	  }
	  else
	  {
		  snprintf(trans_str, 30, "Not new_dat: %d_END\n", new_dat); // если в буфере ничего нового не появилось
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
	  }*/

	  //////////////////////////////// все что ниже, сделано для демонстрации, можно удалить /////////////////////////////////////////
	  if(new_dat > 0)
	  {
		  snprintf(trans_str, BUF_OUT - 1, "------------\nNew_dat: %d, index_r: %d, index_w: %d_END\n", new_dat, (index_r & BUFFER_MASK), (index_w & BUFFER_MASK));
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);

		  myuBUF_t ns = 0;
		  ns = (index_w & BUFFER_MASK);
		  ns = ((ns - 1) & BUFFER_MASK);

		  snprintf(trans_str, BUF_OUT - 1, "%s|%c| write\n", dot_str, buff_main[ns]);
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);

		  for(myuBUF_t i = 0; i < SIZE_BUF; i++)
		  {
			  if(i == SIZE_BUF - 1)
			  {
				  snprintf(trans_str, 5, "|%c|\n", buff_main[i]);
			  }
			  else
			  {
				  snprintf(trans_str, SIZE_BUF + 11, "|%c", buff_main[i]);
			  }

			  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
		  }

		  memset(dot_str, 0, SIZE_BUF * 2 + 10);

		  myuBUF_t s = 0;

		  for(myuBUF_t i = 0; i < (index_r & BUFFER_MASK); i++)
		  {
		  	dot_str[s] = ' ';
		  	s++;
		  	dot_str[s] = ' ';
		  	s++;
		  }

		  for(myuBUF_t i = 0; i < 1; i++)
		  {
		  		read_buff[i] = GetByte();
		  }

		  snprintf(trans_str, BUF_OUT - 1, "%s|%s| read\n", dot_str, read_buff);
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);

		  snprintf(trans_str, BUF_OUT - 1, "index_r: %d_END\n", (index_r & BUFFER_MASK));
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
	  }
	  else
	  {
		  snprintf(trans_str, 30, "Not new_dat: %d_END\n", new_dat); // если в буфере ничего нового не появилось
		  HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
	  }

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

  /**Initializes the CPU, AHB and APB busses clocks 
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
  /**Initializes the CPU, AHB and APB busses clocks 
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 720;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 700;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(led13_GPIO_Port, led13_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : led13_Pin */
  GPIO_InitStruct.Pin = led13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led13_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int _write(int32_t file, uint8_t *ptr, int32_t len) // instrumentation trace macrocell (ITM)
{
	int i;
	for(i = 0; i < len; i++)
		ITM_SendChar(*ptr++);

	return len;
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
	HAL_GPIO_WritePin(led13_GPIO_Port, led13_Pin, GPIO_PIN_RESET);
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
