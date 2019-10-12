Кольцевой буфер УАРТА по типу ардуиновского - буфер заполняется и приём прекращается пока не будет вычитан хотя бы один символ.

----------------------------------------------------------

Подключить в проект файлы usart_ring.h и usart_ring.c

В usart_ring.h указать уарт и прописать размер входящего буфера:

```
#define MYUART huart1 // задефайнить USART

#define UART_RX_BUFFER_SIZE 128 // указать размер входного буфера
```

В файле stm32f1xx_it.c заинклюдить  usart_ring.h, добавить переменные, и код в обработчик прерывания:

```
/* USER CODE BEGIN Includes */
#include "usart_ring.h"
/* USER CODE END Includes */
```


```
/* USER CODE BEGIN PV */
extern volatile rx_buffer_index_t rx_buffer_head;
extern volatile rx_buffer_index_t rx_buffer_tail;
extern unsigned char rx_buffer[UART_RX_BUFFER_SIZE];
/* USER CODE END PV */
```


```
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	if((MYUART.Instance->SR & USART_SR_RXNE) != RESET)
	{
		uint8_t rbyte = (uint8_t)(MYUART.Instance->DR & (uint8_t)0x00FF); // читает байт из регистра
		rx_buffer_index_t i = (uint16_t)(rx_buffer_head + 1) % UART_RX_BUFFER_SIZE;

		if(i != rx_buffer_tail)
		{
			rx_buffer[rx_buffer_head] = rbyte;
			rx_buffer_head = i;
		}
	}

	return;
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
```

В main.c заинклюдить  usart_ring.h

```
/* USER CODE BEGIN Includes */
#include "usart_ring.h"
/* USER CODE END Includes */
```

Перед бесконечным циклом включить прерывания уарта:

```
/* USER CODE BEGIN 2 */
__HAL_UART_ENABLE_IT(&MYUART, UART_IT_RXNE); // включить прерывания usart'a
/* USER CODE END 2 */
```


Принимать в цикле:


```
while (1)
{
	  if(uart_available()) // есть ли что-то в уарте
	  {
		  char str[32] = {0,};
		  uint8_t i = 0;

		  while(uart_available())
		  {
			  str[i++] = uart_read(); // читаем
			  if(i > 32 - 1) break;
			  HAL_Delay(1); // пауза нужна или не нужна, в зависимости от задачи
		  }

		  HAL_UART_Transmit(&MYUART, (uint8_t*)str, strlen(str), 100);
	  }
}
```

Для обнуления (очистки) входного буфера есть функция...


```
/* USER CODE BEGIN 2 */
clear_uart_buff();
/* USER CODE END 2 */
```

---------------------------------------------------------------
В CubeMX настроить USART и включить прерывания.



 
