/*
 * gsm.c
 *
 *  Created on: 18 авг. 2019 г.
 *      Author: dima
 */
#include <gprs.h>
#include "main.h"
#include "usart_ring.h"

#define SEND_STR_SIZE 128

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

///////////////////////// Функция для замены смволов \r и \n на пробелы ////////////////////////////
void replac_string(char *src)
{
	if(!src) return;

	for(;*src; src++)
	{
		if(*src == '\n' || *src == '\r') *src = ' ';
	}
}

///////////////////////// Функция для отправки настроечных команд, в цикле лучше не использовать ////////////////////////////
void set_comand(char *buff)
{
	uint8_t count_err = 0;
	char str[SEND_STR_SIZE] = {0,};
	snprintf(str, SEND_STR_SIZE, "%s\r\n", buff);
	HAL_UART_Transmit(GSM, (uint8_t*)str, strlen(str), 1000);
	HAL_Delay(200);

	memset(str, 0, SEND_STR_SIZE);

	for(uint8_t i = 0; i < 30; i++)
	{
		if(gsm_available()) //если модуль что-то прислал
		{
			uint16_t i = 0;

			while(gsm_available())
			{
				str[i++] = gsm_read();
				if(i > SEND_STR_SIZE - 1) break;
				HAL_Delay(1);
			}

			replac_string(str);

			char *p = NULL;

			if((p = strstr(str, "+CPAS:")) != NULL)
			{
				if(strstr(str, "0") == NULL)
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)p, strlen(p), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)"\n+CPAS not ready, must be '0'\n", strlen("\n+CPAS not ready, must be '0'\n"), 1000);

					while(1) // мигаем 5 секунд и ресетим плату
					{
						count_err++;
						HAL_GPIO_TogglePin(ER_LED_GPIO_Port, ER_LED_Pin);
						HAL_Delay(100);
						//if(count_err > 49) HAL_NVIC_SystemReset();
					}
				}
			}
			else if((p = strstr(str, "+CREG:")) != NULL)
			{
				if(strstr(str, "0,1") == NULL)
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)p, strlen(p), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)"\n+CREG not ready, must be '0,1'\n", strlen("\n+CREG not ready, must be '0,1'\n"), 1000);

					while(1) // мигаем 5 секунд и ресетим плату
					{
						count_err++;
						HAL_GPIO_TogglePin(ER_LED_GPIO_Port, ER_LED_Pin);
						HAL_Delay(100);
						//if(count_err > 49) HAL_NVIC_SystemReset();
					}
				}
			}
			else if((p = strstr(str, "+CGATT:")) != NULL)
			{
				if(strstr(str, "1") == NULL)
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)p, strlen(p), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)"\n+CGATT not ready, must be '1'\n", strlen("\n+CGATT not ready, must be '1'\n"), 1000);

					while(1) // мигаем 5 секунд и ресетим плату
					{
						count_err++;
						HAL_GPIO_TogglePin(ER_LED_GPIO_Port, ER_LED_Pin);
						HAL_Delay(100);
						//if(count_err > 49) HAL_NVIC_SystemReset();
					}
				}
			}
			else if(((p = strstr(str, "+SAPBR:")) != NULL) && (strstr(str, "CONTYPE") == NULL))
			{
				if(strstr(str, "0.0.0.0") != NULL)
				{
					HAL_UART_Transmit(DEBUG, (uint8_t*)p, strlen(p), 1000);
					HAL_UART_Transmit(DEBUG, (uint8_t*)"\nNot IP\n", strlen("\nNot IP\n"), 1000);

					while(1) // мигаем 5 секунд и ресетим плату
					{
						count_err++;
						HAL_GPIO_TogglePin(ER_LED_GPIO_Port, ER_LED_Pin);
						HAL_Delay(100);
						//if(count_err > 49) HAL_NVIC_SystemReset();
					}
				}
			}

			p = 0;

			char dbg_str[SEND_STR_SIZE + 32] = {0,};
			snprintf(dbg_str, SEND_STR_SIZE + 32, "Set %s %s\n", buff, str);
			HAL_UART_Transmit(DEBUG, (uint8_t*)dbg_str, strlen(dbg_str), 1000);

			return;
		}

		HAL_Delay(500);

	} // END for()

	HAL_UART_Transmit(DEBUG, (uint8_t*)"Not reply ", strlen("Not reply "), 1000);
	HAL_UART_Transmit(DEBUG, (uint8_t*)buff, strlen(buff), 1000);

	while(1) // мигаем 5 секунд и ресетим плату
	{
		count_err++;
		HAL_GPIO_TogglePin(ER_LED_GPIO_Port, ER_LED_Pin);
		HAL_Delay(100);
		//if(count_err > 49) HAL_NVIC_SystemReset();
	}
}

/////////////////////// Функции для использования в цикле //////////////////////////
void balance(void)
{
	char ATD[] = "ATD#100#;\r\n"; // баланс (бабло)
	HAL_UART_Transmit(GSM, (uint8_t*)ATD, strlen(ATD), 1000);
}

void disable_connection(void)
{
	char ATH[] = "ATH\r\n"; // сбросить все соединения
	HAL_UART_Transmit(GSM, (uint8_t*)ATH, strlen(ATH), 1000);
}

void call(void)
{
	char ATD[] = "ATD+79819555551;\r\n"; // позвонить
	HAL_UART_Transmit(GSM, (uint8_t*)ATD, strlen(ATD), 1000);
}

void incoming_call(void)
{
	char ATA[] = "ATA\r\n"; // принять звонок
	HAL_UART_Transmit(GSM, (uint8_t*)ATA, strlen(ATA), 1000);
}

void get_date_time(void)
{
	char ATCCLK[] = "AT+CCLK?\r\n"; // узнать дату/время
	HAL_UART_Transmit(GSM, (uint8_t*)ATCCLK, strlen(ATCCLK), 1000);
}







