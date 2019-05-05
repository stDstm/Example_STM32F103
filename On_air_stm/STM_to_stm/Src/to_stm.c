/*
 * to_stm.c
 *
 *  Created on: 2 мая 2019 г.
 *      Author: dima
 */
#include "main.h"
#include "usbd_cdc_if.h"
#include "to_stm.h"

extern UART_HandleTypeDef huart1;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern char res;


//////////////////////////// entr_bootloader /////////////////////////////////////
void entr_bootloader()
{
  on_off_boot(1); // подтягиваем BOOT_0 к плюсу
  HAL_Delay(500);
  on_reset(); // нажимаем ресет
  HAL_Delay(200);

  uint8_t array[1] = {0x7F};
  HAL_UART_Transmit(&huart1, (uint8_t*)array, 1, 1000); // первый запрос (для определения скорости)

  if(ack_byte() == 0)
  {
	  CDC_Transmit_FS((uint8_t*)"Bootloader - OK\r\n", strlen("Bootloader - OK\r\n"));
  }
  else
  {
	  CDC_Transmit_FS((uint8_t*)"Bootloader - ERROR\r\n", strlen("Bootloader - ERROR\r\n"));
  }
}

////////////////////////////// on_reset //////////////////////////////////////
void on_reset()
{
  HAL_GPIO_WritePin(GPIOA, RESET_PIN_Pin, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPIOA, RESET_PIN_Pin, GPIO_PIN_RESET);
}

////////////////////////////// on_off_boot ///////////////////////////////////
void on_off_boot(uint8_t state)
{
  HAL_GPIO_WritePin(GPIOA, BOOT_PIN_Pin, state);
}

////////////////////////////// on_off_boot ///////////////////////////////////
void boot_off_and_reset()
{
  on_off_boot(0);
  HAL_Delay(500);
  on_reset();
  CDC_Transmit_FS((uint8_t*)"Boot off and reset\r\n", strlen("Boot off and reset\r\n"));
}

////////////////////////////// erase_memory ////////////////////////////////////
uint8_t erase_memory()
{
  uint8_t cmd_array[2] = {0x43, 0xBC}; // команда на стирание

  if(send_cmd(cmd_array) == 0)
  {
    uint8_t cmd_array[2] = {0xFF, 0x00}; // код стирания (полное)

    if(send_cmd(cmd_array) == 0)
    {
      CDC_Transmit_FS((uint8_t*)"Erase Memory - OK\r\n", strlen("Erase Memory - OK\r\n"));
      return 0;
    }
    else CDC_Transmit_FS((uint8_t*)"Cmd cod Erase Memory - ERROR\r\n", strlen("Cmd cod Erase Memory - ERROR\r\n"));
  }
  else CDC_Transmit_FS((uint8_t*)"Cmd start Erase Memory - ERROR\r\n", strlen("Cmd start Erase Memory - ERROR\r\n"));

  return 1;
}

//////////////////////////////////////// Get ID //////////////////////////////////////////
void get_id()
{
    uint8_t cmd_array[2] = {0x02, 0xFD}; // код Get ID
    uint8_t id[5] = {0,};
    HAL_UART_Receive_IT(&huart1, (uint8_t*)id, 5);
    send_cmd(cmd_array);
    HAL_Delay(10);

    if(id[0] == 'y')
    {
		if(id[4] == 'y')
		{
			uint16_t stm_id = 0;
			stm_id = id[2];
			stm_id = (stm_id << 8) | id[3];
			char str[16] = {0,};
			snprintf(str, 16, "ID_chip: 0x%X\r\n", stm_id);
			CDC_Transmit_FS((uint8_t*)str, strlen(str));
		}
		else CDC_Transmit_FS((uint8_t*)"Not ID - ERROR\r\n", strlen("Not ID - ERROR\r\n"));
    }
    else CDC_Transmit_FS((uint8_t*)"Cmd Get ID - ERROR\r\n", strlen("Cmd Get ID - ERROR\r\n"));
}


////////////////////////////// send_cmd ////////////////////////////////////
uint8_t send_cmd(uint8_t *cmd_array)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)cmd_array, 2, 1000);
  if(ack_byte() == 0) return 0;
  else return 1;
}

uint8_t ack_byte()
{
	uint8_t ack_buff = 0;
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&ack_buff, 1);

	for(uint16_t i = 0; i < 500; i++)
	{
	  if(ack_buff == 'y') return 0;
	  HAL_Delay(1);
	}

	return 1;
}

/////////////////////////////// Go Запуск программы //////////////////////////////////
void go_prog()
{
  uint8_t cmd_array[2] = {0x21, 0xDE}; // код запуска программы

  if(send_cmd(cmd_array) == 0)
  {
    CDC_Transmit_FS((uint8_t*)"Cmd start programm - OK\r\n", strlen("Cmd start programm - OK\r\n"));

    uint8_t ret_adr = send_adress(WRITE_ADDR);

    if(ret_adr == 0)
    {
      CDC_Transmit_FS((uint8_t*)"Start programm - OK\r\n", strlen("Start programm - OK\r\n"));
    }
    else CDC_Transmit_FS((uint8_t*)"Address start programm - ERROR\r\n", strlen("Address start programm - ERROR\r\n"));

  }
  else CDC_Transmit_FS((uint8_t*)"Cmd start programm - ERROR\r\n", strlen("Cmd start programm - ERROR\r\n"));
}

///////////////////////////// send_adress ////////////////////////////////////
uint8_t send_adress(uint32_t addr)
{
  uint8_t buf[5] = {0,};
  buf[0] = addr >> 24;
  buf[1] = (addr >> 16) & 0xFF;
  buf[2] = (addr >> 8) & 0xFF;
  buf[3] = addr & 0xFF;
  buf[4] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];

  HAL_UART_Transmit(&huart1, (uint8_t*)buf, 5, 1000);

  if(ack_byte() == 0) return 0;
  else return 1;
}












