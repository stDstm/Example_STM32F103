/*
 * to_stm.h
 *
 *  Created on: 2 мая 2019 г.
 *      Author: dima
 */

#ifndef TO_STM_H_
#define TO_STM_H_

#define WRITE_ADDR 0x08000000

void USB_reset(void);
void entr_bootloader();
void on_reset();
void on_off_boot(GPIO_PinState i);
void boot_off_and_reset();
uint8_t ack_byte();
uint8_t erase_memory();
uint8_t send_cmd(uint8_t *cmd_array);
void go_prog();
uint8_t send_adress(uint32_t addr);
void get_id();
void write_memory();


#endif /* TO_STM_H_ */
