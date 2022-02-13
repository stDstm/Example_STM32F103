/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

#include "delay_micros.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//////////////////////////// RESET AX /////////////////////////////////
#define RESET_AX_HIGH    		(RESET_AX_GPIO_Port->BSRR = RESET_AX_Pin)
#define RESET_AX_LOW    		(RESET_AX_GPIO_Port->BSRR = (uint32_t)RESET_AX_Pin << 16U)

///////////////////////////////// A ///////////////////////////////////
#define STEP_A_HIGH     		(STEP_fscx_A_GPIO_Port->BSRR = STEP_fscx_A_Pin)
#define STEP_A_LOW      		(STEP_fscx_A_GPIO_Port->BSRR = (uint32_t)STEP_fscx_A_Pin << 16U)

#define DIR_RIGHT_A     		(DIR_CW_CCW_A_GPIO_Port->BSRR = DIR_CW_CCW_A_Pin)
#define DIR_LEFT_A     		    (DIR_CW_CCW_A_GPIO_Port->BSRR = (uint32_t)DIR_CW_CCW_A_Pin << 16U)

#define ROTATE                  (315 * 12) // полный оборот (315 градусов, 12 шагов на градус)
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
void step(int16_t step)
{
	while(step)
	{
		STEP_A_HIGH;
		delay_us(500);
		STEP_A_LOW;
		delay_us(2);
		step--;
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
  /* USER CODE BEGIN 2 */

  /////////////////////// DWT /////////////////////////////
  DWT_Init();

  /////////////////////// init_ax /////////////////////////
  RESET_AX_LOW;
  STEP_A_LOW;
  DIR_RIGHT_A;
  HAL_Delay(2);
  RESET_AX_HIGH;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
	  DIR_RIGHT_A;
	  step(ROTATE);

	  HAL_Delay(500);

	  DIR_LEFT_A;
	  step(ROTATE);

	  HAL_Delay(500);

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RESET_AX_Pin|DIR_CW_CCW_A_Pin|STEP_fscx_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RESET_AX_Pin DIR_CW_CCW_A_Pin STEP_fscx_A_Pin */
  GPIO_InitStruct.Pin = RESET_AX_Pin|DIR_CW_CCW_A_Pin|STEP_fscx_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//volatile uint8_t var0 = (cnt >> 24) & 0xff;
//volatile uint8_t var1 = (cnt >> 16) & 0xff;
//volatile uint8_t var2 = (cnt >> 8) & 0xff;
//volatile uint8_t var3 = cnt & 0xff;

//uint8_t *tmp = (uint8_t*)&cnt;

/*uint8_t var0 = *((uint8_t*)&cnt + 3);
uint8_t var1 = *((uint8_t*)&cnt + 2);
uint8_t var2 = *((uint8_t*)&cnt + 1);
uint8_t var3 = (uint8_t)cnt;

uint32_t cel = 0;

*((uint8_t*)&cel + 3) = var0;
*((uint8_t*)&cel + 2) = var1;
*((uint8_t*)&cel + 1) = var2;
*((uint8_t*)&cel) = var3;

snprintf(trans_str, 128, "Z %02X %02X %02X %02X C %08lX", var0, var1, var2, var3, cel);
trans_to_usart1(trans_str);*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

