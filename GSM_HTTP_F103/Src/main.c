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
#include "usart_ring.h"
#include "gprs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GSM_AVAIL_SIZE 128
#define DBG_AVAIL_SIZE 64
#define HTTP_BUFF_SIZE 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
volatile uint8_t flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/////////////////// удалить символы \r и \n из строки //////////////////////
static void clear_string(char *src)
{
	char *dst = NULL;
	if(!src) return;
	uint8_t i = 0;

	for(dst = src; *src; src++)
	{
		if(i < 2 && (*src == '\n' || *src == '\r'))
		{
			i++;
			continue;
		}
		else if(*src == '\n' || *src == '\r') *src = ' ';

		*dst++ = *src;
	}

	*dst = 0;
}

///////////////////// колбек таймера //////////////////////
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
	{
		flag = 1;
	}
}

//////////////// проверка и установка скорости 19200, нужна один раз /////////////////
void chek_speed(void)
{
  for(uint8_t i = 0; i < 7; i++)
  {
	  uint32_t sp = 0;

	  if(i == 0) sp = 2400;
	  else if(i == 1) sp = 4800;
	  else if(i == 2) sp = 9600;
	  else if(i == 3) sp = 19200;
	  else if(i == 4) sp = 38400;
	  else if(i == 5) sp = 57600;
	  else if(i == 6) sp = 115200;

	  huart1.Instance = USART1;
	  huart1.Init.BaudRate = sp;
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

	  char str[16] = {0,};
	  HAL_UART_Transmit(GSM, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
	  HAL_Delay(300);

	  if(gsm_available()) //если модуль что-то прислал
	  {
		  uint16_t i = 0;

		  while(gsm_available())
		  {
			  str[i++] = gsm_read();
			  if(i > 15) break;
			  HAL_Delay(2);
		  }

		  if(strstr(str, "OK") != NULL)
		  {
			  char buf[64] = {0,};
			  snprintf(buf, 64, "Uart modem was %lu, switched to 57600\n", huart1.Init.BaudRate);
			  HAL_UART_Transmit(DEBUG, (uint8_t*)buf, strlen(buf), 100);
			  HAL_UART_Transmit(GSM, (uint8_t*)"AT+IPR=57600\r\n", strlen("AT+IPR=57600\r\n"), 1000);
			  HAL_Delay(250);
			  MX_USART1_UART_Init();
			  break;
		  }
	  }
  }
}


void send_http(uint8_t a, uint8_t b)
{
	char buf[HTTP_BUFF_SIZE] = {0,}; // строка должна быть не больше 64 символов, если больше, то надо разбивать на части, и только последняя часть должна содержать символы \r\n
	//snprintf(buf, HTTP_BUFF_SIZE, "AT+HTTPPARA=\"URL\",\"http://some_site.ru/gsm.php?a=%d&b=%d\"\r\n", a, b); // адрес страницы, которую хотим запросить у сервера или отправить данные.
	snprintf(buf, HTTP_BUFF_SIZE, "AT+HTTPPARA=\"URL\",\"http://example.com/\"\r\n");


	uint8_t len = strlen(buf);
	uint8_t flag = 0;

	if(len < HTTP_BUFF_SIZE + 1)
	{
		HAL_UART_Transmit(DEBUG, (uint8_t*)"Send to gsm address\n", strlen("Send to gsm address\n"), 100); // debug
		HAL_UART_Transmit(GSM, (uint8_t*)buf, strlen(buf), 1000);
	}
	else
	{
		HAL_UART_Transmit(DEBUG, (uint8_t*)"Error, string > 64\n", strlen("Error, string > 64\n"), 100); // Error
		return;
	}

	HAL_Delay(100);
	memset(buf, 0, HTTP_BUFF_SIZE);

	if(gsm_available()) //если модуль что-то прислал
	{
		uint8_t i = 0;

		while(gsm_available())
		{
			buf[i++] = gsm_read();
			if(i > HTTP_BUFF_SIZE - 1) break;
			HAL_Delay(1);
		}

		if(strstr(buf, "OK") != NULL)
		{
			HAL_UART_Transmit(DEBUG, (uint8_t*)"Send to gsm AT+HTTPACTION=0\n", strlen("Send to gsm AT+HTTPACTION=0\n"), 100); // debug
			HAL_UART_Transmit(GSM, (uint8_t*)"AT+HTTPACTION=0\r\n", strlen("AT+HTTPACTION=0\r\n"), 1000); // делаем запрос (0 - GET, 1 - POST, 2 - HEAD)
		}
		else
		{
			HAL_UART_Transmit(DEBUG, (uint8_t*)"Error AT+HTTPPARA=URL\n", strlen("Error AT+HTTPPARA=URL\n"), 1000);
			HAL_UART_Transmit(DEBUG, (uint8_t*)buf, strlen(buf), 1000); // Error
			// как-то обрабатываем ошибку
			return;
		}

		///////////////////////////////// ожидание "ОК" после AT+HTTPACTION=0 /////////////////////////////////////
		memset(buf, 0, HTTP_BUFF_SIZE);

		for(uint16_t f = 0; f < 500; f++) // ожидание "ОК" после AT+HTTPACTION=0
		{
			if(gsm_available())
			{
				uint8_t k = 0;

				while(gsm_available())
				{
					buf[k++] = gsm_read();
					if(k > HTTP_BUFF_SIZE - 1) break;
					HAL_Delay(1);
				}


				if(strstr(buf, "OK") != NULL)
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)"AT+HTTPACTION OK, wait...\n", strlen("AT+HTTPACTION OK, wait...\n"), 100); // debug
					break; // for()
				}
				else
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)"Error AT+HTTPACTION\n", strlen("Error AT+HTTPACTION\n"), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)buf, strlen(buf), 1000); // Error
					// как-то обрабатываем ошибку
					return;
				}
			}

			if(f == 499)
			{
				HAL_UART_Transmit(DEBUG, (uint8_t*)"Error, not OK AT+HTTPACTION\n", strlen("Error, not OK AT+HTTPACTION\n"), 1000);
				return;
			}

			HAL_Delay(1);
		}

		/////////////////////////////// +HTTPACTION:0,200,x /////////////////////////////////////
		memset(buf, 0, HTTP_BUFF_SIZE);

		for(uint16_t f = 0; f < 10000; f++) // ожидание +HTTPACTION:0,200,x
		{
			if(gsm_available())
			{
				uint8_t k = 0;

				while(gsm_available())
				{
					buf[k++] = gsm_read();
					if(k > HTTP_BUFF_SIZE - 1) break;
					HAL_Delay(1);
				}


				if(strstr(buf, "+HTTPACTION:") != NULL)
				{
					if(strstr(buf, "200") != NULL)
					{
						HAL_UART_Transmit(DEBUG, (uint8_t*)"Server 200, send AT+HTTPREAD\n", strlen("Server 200, send AT+HTTPREAD\n"), 1000); // debug
						HAL_UART_Transmit(GSM, (uint8_t*)"AT+HTTPREAD\r\n", strlen("AT+HTTPREAD\r\n"), 1000); // команда на чтение полученых данных
						flag = 1;
						break; // for()
					}
					else
					{
						HAL_UART_Transmit(DEBUG, (uint8_t*)"Error, Server not 200\n", strlen("Error, Server not 200\n"), 1000);
						HAL_UART_Transmit(DEBUG, (uint8_t*)buf, strlen(buf), 1000); // Error
						// как-то обрабатываем ошибку
						return;
					}
				}
				else
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)"Error +HTTPACTION answer\n", strlen("Error +HTTPACTION answer\n"), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)buf, strlen(buf), 1000); // Error
					// как-то обрабатываем ошибку
					return;
				}
			}

			if(f == 9999)
			{
				HAL_UART_Transmit(DEBUG, (uint8_t*)"Error, not HTTPACTION\n", strlen("Error, not HTTPACTION\n"), 1000);
				return;
			}

			HAL_Delay(1);
		}

		///////////////////////////////// чтение ответа сервера /////////////////////////////////////
		if(flag)
		{
			flag = 0;
			HAL_Delay(500);
			char read_buf[GPRS_RX_BUFFER_SIZE] = {0,};

			while(gsm_available())
			{
				uint16_t i = 0;
				memset(read_buf, 0, GPRS_RX_BUFFER_SIZE);

				while(gsm_available())
				{
					read_buf[i++] = gsm_read();
					if(i > GPRS_RX_BUFFER_SIZE - 1) break;
				}

				HAL_UART_Transmit(DEBUG, (uint8_t*)"\n----------\nREAD_BUF:\n----------\n", strlen("\n----------\nREAD_BUF:\n----------\n"), 1000);
				HAL_UART_Transmit(DEBUG, (uint8_t*)read_buf, strlen(read_buf), 1000);

				// тут можно парсить ответ
			}
		}
	}
	else
	{
		HAL_UART_Transmit(DEBUG, (uint8_t*)"AT+HTTPPARA not reply\n", strlen("AT+HTTPPARA not reply\n"), 100); // Error
		// как-то обрабатываем ошибку
		return;
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
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(1000);
  HAL_GPIO_WritePin(ON_RELAY_GPIO_Port, ON_RELAY_Pin, GPIO_PIN_SET);
  HAL_Delay(6000); // задержка чтоб модем успел раздуплиться, если его включение происходит вместе с включением МК


  __HAL_UART_ENABLE_IT(GSM, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(DEBUG, UART_IT_RXNE);

  chek_speed(); // проверка и установка скорости 57600, нужна один раз

  ////////////////// настройка модема ///////////////////
  set_comand(ATCPAS);  // проверка статуса модема
  set_comand(ATCREG);  // проверка регистрации в сети - должен вернуть  +CREG: 0,1
  set_comand(ATCLIP1); // включить АОН
  set_comand(ATE);     // отключить «эхо»
  set_comand(ATS);     // поднимать трубку только "вручную"
  //set_comand(ATDDET);  // включить DTMF
  //set_comand(ATCCLKK); // установить дату/время
  set_comand(ATCMEE);  // включить расшифровку ошибок


  /*------------------- БЛОК SMS START --------------------*/
  /////////////////// настройки для работы с sms ////////////////
  //set_comand(ATCMGF);    // устанавливает текстовый режим смс-сообщения
  //set_comand(ATCPBS);    // открывает доступ к данным телефонной книги SIM-карты
  //set_comand(ATCSCS);    // кодировка текста - GSM
  //set_comand(ATCNMI);    // настройка вывода смс в консоль
  /*-------------------- БЛОК SMS END --------------------*/


  /*------------------- БЛОК INFA START --------------------*/
  //////////////////// различная инфа /////////////////////
  //set_comand(ATIPR);       // скорость usart'a модема
  //set_comand(ATI);         // название и версия модуля
  //set_comand(ATCGSN);      // считывание IMEI из EEPROM
  //set_comand(ATCSPN);      // оператор сети
  /*-------------------- БЛОК INFA END --------------------*/


  /*------------------- БЛОК HTTP START --------------------*/
  //////////////////// HTTP НАСТРОЙКА /////////////////////
  set_comand(ATCGATT1);      // включить GPRS сервис
  set_comand(ATCGATT);       // проверить подключен ли к GPRS - +CGATT: 1

  set_comand(ATSAPBR31CG);   // используем GPRS соединение
  set_comand(ATSAPBR31APN);  // APN
  set_comand(ATSAPBR31U);    // user
  set_comand(ATSAPBR31P);    // pass
  set_comand(ATSAPBR11);     // установить соединение

  //////////////////// HTTP ПРОВЕРКА //////////////////////
  set_comand(ATSAPBR21);     // посмотреть полученный IP адрес - +SAPBR: 1,1,"IP.ад.ре.с"
  set_comand(ATSAPBR41);     // посмотреть текущие настройки подключения

  //////////////////// HTTP ИНИЦИАЛИЗАЦИЯ //////////////////////
  set_comand(ATHTTPINIT);    // инициализация HTTP службы
  set_comand(ATHTTPPARA);    // установка CID параметра для http сессии
  /*-------------------- БЛОК HTTP END --------------------*/


  send_http(57, 69); // отправка данных серверу

  HAL_TIM_Base_Start_IT(&htim4);

  uint16_t count = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(flag == 1) // срабатывает раз в секунду выводить время
		{
			flag = 0;
			//get_date_time(); // будет раз в секунду выводить время
			count++;
		}

		if(count > 59) // каждые 60 секунд
		{
			count = 0;
			//send_http(57, 69);
		}


		if(gsm_available()) //если модуль что-то прислал
		{
			uint16_t i = 0;

			char buf[GSM_AVAIL_SIZE] = {0,};
			char str[GSM_AVAIL_SIZE] = {0,};
			HAL_Delay(50);

			while(gsm_available())
			{
				buf[i++] = gsm_read();
				if(i > GSM_AVAIL_SIZE - 1) break;
				HAL_Delay(1);
			}

			clear_string(buf); // очищаем строку от символов \r и \n

			if(strstr(buf, "RING") != NULL) // ЕСЛИ ЭТО ЗВОНОК
			{
				if(strstr(buf, "9823124561") != NULL) // если звонит нужный номер
				{
					// что-то делаем
					HAL_UART_Transmit(DEBUG, (uint8_t*)"My number\n", strlen("My number\n"), 1000);
					//incoming_call(); // можно принять звонок
					//HAL_NVIC_SystemReset(); // RESET
					disable_connection(); // сброс соединения
				}
			}
			else if(strstr(buf, "+SAPBR") != NULL)
			{
				if(strstr(buf, "DEACT") != NULL)
				{
					set_comand(ATSAPBR11); // установить соединение
				}
			}

			snprintf(str, GSM_AVAIL_SIZE, "%s\n", buf);
			HAL_UART_Transmit(DEBUG, (uint8_t*)str, strlen(str), 1000);
		}


		////////////////////////////////////// DEBUG ////////////////////////////////////////
		if(dbg_available()) //если послали в терминал какую-то команду, то она перенаправиться в модем
		{
			uint8_t i = 0;
			char dbg_str[DBG_AVAIL_SIZE] = {0,};
			char dbg_buf[DBG_AVAIL_SIZE] = {0,};

			while(dbg_available())
			{
				dbg_buf[i++] = dbg_read();
				if(i > DBG_AVAIL_SIZE - 1) break;
				HAL_Delay(1);
			}

			clear_string(dbg_buf);
			snprintf(dbg_str, DBG_AVAIL_SIZE, "%s\r\n", dbg_buf);
			HAL_UART_Transmit(GSM, (uint8_t*)dbg_str, strlen(dbg_str), 1000);
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
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  huart1.Init.BaudRate = 57600;
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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, ON_RELAY_Pin|ER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ON_RELAY_Pin ER_LED_Pin */
  GPIO_InitStruct.Pin = ON_RELAY_Pin|ER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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
