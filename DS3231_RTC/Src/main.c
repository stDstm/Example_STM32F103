/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "ds3231.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
volatile uint8_t flag_clear_alarm = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void I2C_Scan(I2C_HandleTypeDef *hi2c) // сканирует шину и показывает адреса устройств
{
    char info[] = "Scanning I2C bus...\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), 1000);

    for(uint16_t i = 0; i < 128; i++)
    {
        if(HAL_I2C_IsDeviceReady(hi2c, i << 1, 1, 100) == HAL_OK)
        {
        	char msg[64] = {0,};
            snprintf(msg, 64, "Device: 0x%02X\n", i);
            HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
        }
    }
}

void I2C_Error(char *er, uint32_t status) // ошибки i2c
{
	char str[64] = {0,};

	switch(status)
	{
		case HAL_ERROR:
			snprintf(str, 64, "%s - HAL_ERROR\n", er);
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_BUSY:
			snprintf(str, 64, "%s - HAL_BUSY\n", er);
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_TIMEOUT:
			snprintf(str, 64, "%s - HAL_TIMEOUT\n", er);
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		default:
		break;
	}

	uint32_t err = HAL_I2C_GetError(&hi2c1);

	switch(err)
	{
		case HAL_I2C_ERROR_NONE:
			snprintf(str, 64, "HAL_I2C_ERROR_NONE\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_BERR:
			snprintf(str, 64, "HAL_I2C_ERROR_BERR\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_ARLO:
			snprintf(str, 64, "HAL_I2C_ERROR_ARLO\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_AF:
			snprintf(str, 64, "HAL_I2C_ERROR_AF\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_OVR:
			snprintf(str, 64, "HAL_I2C_ERROR_OVR\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_DMA:
			snprintf(str, 64, "HAL_I2C_ERROR_DMA\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		case HAL_I2C_ERROR_TIMEOUT:
			snprintf(str, 64, "HAL_I2C_ERROR_TIMEOUT\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
		break;

		default:
		break;
	}

	while(1){}; // после вывода ошибки программа зацикливается
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == INT_ALARM_Pin)
	{
		//HAL_UART_Transmit(&huart1, (uint8_t*)"INT\n", 4, 1000);
		//flag_clear_alarm = 1;
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  I2C_Scan(&hi2c1); // НОМЕР I2C

  /////////////////////////////////////////////////////////////////////////////////////////////
  // устанока времени и даты ⇨ расскоментить ⇨ прошить ⇨ закомментить ⇨ прошить ⇨ пользоваться
  //setHour(22); // 0-23
  //setMinutes(23);
  //setSeconds(30);
  //setDate(24);
  //setMonth(5);
  //setYear(19);
  //setDay(1); // 1-7

  //Set_time_data();

  /////////////////////////////////////////////////////////////////////////////////////////////
  setOutput(OUTPUT_SQW); // разрешить вовод импульсов на пин SQW. OUTPUT_SQW - импульсы соответствующие функции setSQWRate(), OUTPUT_INT - прерывания от будильника

  setSQWRate(SQW_RATE_1); // 1Hz, SQW_RATE_1K - 1024Hz, SQW_RATE_4K - 4096Hz, SQW_RATE_8K - 8192Hz

  enable32KHz(0); // отключить вывод частоты на пин 32К, 1 - включить. (частота с кварца часов 32768)

  //setAlarm1(0, 0, 48, 20, EVERY_SECOND, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Read_time_data();

	  uint8_t h = readHours();
	  uint8_t m = readMinutes();
	  uint8_t s = readSeconds();

	  uint8_t date = readDate();
	  uint8_t month = readMonth();
	  uint8_t year = readYear();
	  uint8_t day = readDay();

	  char str[32] = {0,};
	  snprintf(str, 32, "Time %02d:%02d:%02d\n", h, m, s);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);

	  snprintf(str, 32, "Date %02d-%02d-20%02d day %d\n", date, month, year, day);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);

	  float t_ds = Read_temp_ds3213(); // температура внутри девайса обновляется каждые 64 секунды, можно запустить принудительное измерение, но я этого не стал прописывать в коде
	  snprintf(str, 32, "Temp %.2f\n", t_ds);
	  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);


	  HAL_Delay(1000);

	  if(flag_clear_alarm)
	  {
		  flag_clear_alarm = 0;
		  clearAlarm1();
		  armAlarm1(0);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  /** Initializes the CPU, AHB and APB buses clocks
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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

  /*Configure GPIO pin : INT_ALARM_Pin */
  GPIO_InitStruct.Pin = INT_ALARM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_ALARM_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

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
