/*
 * nrf24l01.c
 *
 *  Created on: 1 авг. 2019 г.
 *      Author: dima
 */
#include "RF24.h"

#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC

#define HIGH 1
#define LOW  0

extern SPI_HandleTypeDef hspi1;

bool p_variant; /** False for RF24L01 and true for RF24L01P */
uint8_t payload_size = 0; /**< Fixed size of payloads */
bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
uint8_t pipe0_reading_address[5] = {0,}; /**< Last address set on pipe 0 for reading. */
uint8_t addr_width = 0; /**< The address width to use - 3,4 or 5 bytes. */
uint8_t txDelay = 0;


void DWT_Init(void)
{
    SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

void delay_us(uint32_t us) // DelayMicro
{
    uint32_t us_count_tic =  us * (SystemCoreClock / 1000000);
    DWT->CYCCNT = 0U; // обнуляем счётчик
    while(DWT->CYCCNT < us_count_tic);
}

void csn(uint8_t level)
{
	HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, level);
	delay_us(5);
}

void ce(uint8_t level)
{
	HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, level);
}

uint8_t read_register(uint8_t reg)
{
	uint8_t addr = R_REGISTER | (REGISTER_MASK & reg);
	uint8_t dt = 0;

	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi1, &addr, &dt, 1, 1000);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xff, &dt, 1, 1000);
	csn(HIGH);
	return dt;
}

uint8_t write_registerMy(uint8_t reg, const uint8_t* buf, uint8_t len)
{
	uint8_t status = 0;
	uint8_t addr = W_REGISTER | (REGISTER_MASK & reg);

	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi1, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)buf, len, 100);
	csn(HIGH);
	return status;
}

uint8_t write_register(uint8_t reg, uint8_t value)
{
	uint8_t status = 0;
	uint8_t addr = W_REGISTER | (REGISTER_MASK & reg);
	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi1, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
	csn(HIGH);
	return status;
}

uint8_t write_payload(const void* buf, uint8_t data_len, const uint8_t writeType)
{
	uint8_t status = 0;
	const uint8_t* current = (const uint8_t*)buf;
	uint8_t addr = writeType;

	data_len = rf24_min(data_len, payload_size);
	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi1, &addr, &status, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)current, data_len, 100);

	while(blank_len--)
	{
		uint8_t empt = 0;
		HAL_SPI_Transmit(&hspi1, &empt, 1, 100);
	}

	csn(HIGH);
	return status;
}

uint8_t read_payload(void* buf, uint8_t data_len)
{
	uint8_t status = 0;
	uint8_t* current = (uint8_t*)buf;

	if(data_len > payload_size)
	{
		data_len = payload_size;
	}

	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

	uint8_t addr = R_RX_PAYLOAD;
	csn(LOW);
	HAL_SPI_Transmit(&hspi1, &addr, 1, 100);
	HAL_SPI_Receive(&hspi1, (uint8_t*)current, data_len, 100);

	while(blank_len--)
	{
		uint8_t empt = 0;
		HAL_SPI_Receive(&hspi1, &empt, 1, 100);
	}

	csn(HIGH);
	return status;
}

uint8_t flush_rx(void)
{
	return spiTrans(FLUSH_RX);
}

uint8_t flush_tx(void)
{
	return spiTrans(FLUSH_TX);
}

uint8_t spiTrans(uint8_t cmd)
{
	uint8_t status = 0;
	csn(LOW);
	HAL_SPI_TransmitReceive(&hspi1, &cmd, &status, 1, 1000);
	csn(HIGH);
	return status;
}

uint8_t get_status(void)
{
	return spiTrans(NOP);
}

void setChannel(uint8_t channel)
{
	write_register(RF_CH, channel);
}

uint8_t getChannel()
{
	return read_register(RF_CH);
}

void setPayloadSize(uint8_t size)
{
	payload_size = rf24_min(size, 32);
}

uint8_t getPayloadSize(void)
{
	return payload_size;
}

uint8_t NRF_Init(void)
{
	uint8_t setup = 0;
	p_variant = false;
	payload_size = 32;
	dynamic_payloads_enabled = false;
	addr_width = 5;
	pipe0_reading_address[0] = 0;

	ce(LOW);
	csn(HIGH);
	HAL_Delay(5);

	write_register(NRF_CONFIG, 0x0C); // Reset NRF_CONFIG and enable 16-bit CRC.
	setRetries(5, 15);
	setPALevel(RF24_PA_MAX); // Reset value is MAX

	if(setDataRate(RF24_250KBPS)) // check for connected module and if this is a p nRF24l01 variant
	{
		p_variant = true;
	}

	setup = read_register(RF_SETUP);
	setDataRate(RF24_1MBPS); // Then set the data rate to the slowest (and most reliable) speed supported by all hardware.

	// Disable dynamic payloads, to match dynamic_payloads_enabled setting - Reset value is 0
	toggle_features();
	write_register(FEATURE, 0);
	write_register(DYNPD, 0);
	dynamic_payloads_enabled = false;

	// Reset current status. Notice reset and flush is the last thing we do
	write_register(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
	setChannel(76);
	flush_rx();
	flush_tx();
	powerUp(); //Power up by default when begin() is called
	write_register(NRF_CONFIG, (read_register(NRF_CONFIG)) & ~(1 << PRIM_RX));
	return (setup != 0 && setup != 0xff);
}

bool isChipConnected()
{
	uint8_t setup = read_register(SETUP_AW);

	if(setup >= 1 && setup <= 3)
	{
		return true;
	}

	return false;
}

void startListening(void)
{
	powerUp();

	write_register(NRF_CONFIG, read_register(NRF_CONFIG) | (1 << PRIM_RX));
	write_register(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
	ce(HIGH);
	// Restore the pipe0 adddress, if exists
	if(pipe0_reading_address[0] > 0)
	{
		write_registerMy(RX_ADDR_P0, pipe0_reading_address, addr_width);
	}
	else
	{
		closeReadingPipe(0);
	}

	if(read_register(FEATURE) & (1 << EN_ACK_PAY))
	{
		flush_tx();
	}
}


static const uint8_t child_pipe_enable[] = {ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5};

void stopListening(void)
{
	ce(LOW);
	delay_us(txDelay);

	if(read_register(FEATURE) & (1 << EN_ACK_PAY))
	{
		delay_us(txDelay); //200
		flush_tx();
	}

	write_register(NRF_CONFIG, (read_register(NRF_CONFIG)) & ~(1 << PRIM_RX));
	write_register(EN_RXADDR, read_register(EN_RXADDR) | (1 << child_pipe_enable[0])); // Enable RX on pipe0
}

void powerDown(void)
{
	ce(LOW); // Guarantee CE is low on powerDown
	write_register(NRF_CONFIG, read_register(NRF_CONFIG) & ~(1 << PWR_UP));
}

//Power up now. Radio will not power down unless instructed by MCU for config changes etc.
void powerUp(void)
{
	uint8_t cfg = read_register(NRF_CONFIG);
	// if not powered up then power up and wait for the radio to initialize
	if(!(cfg & (1 << PWR_UP)))
	{
		write_register(NRF_CONFIG, cfg | (1 << PWR_UP));
		HAL_Delay(5);
	}
}


//Similar to the previous write, clears the interrupt flags
bool write(const void* buf, uint8_t len)
{
	startFastWrite(buf, len, 1, 1);

	while(!(get_status() & ((1 << TX_DS) | (1 << MAX_RT))))
	{}

	ce(LOW);
	uint8_t status = write_register(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

	if(status & (1 << MAX_RT))
	{
		flush_tx(); //Only going to be 1 packet int the FIFO at a time using this method, so just flush
		return 0;
	}

	//TX OK 1 or 0
	return 1;
}

void startFastWrite(const void* buf, uint8_t len, const bool multicast, bool startTx)
{
	write_payload(buf, len, multicast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD);

	if(startTx)
	{
		ce(HIGH);
	}
}

void maskIRQ(bool tx, bool fail, bool rx)
{
	uint8_t config = read_register(NRF_CONFIG);
	config &= ~(1 << MASK_MAX_RT | 1 << MASK_TX_DS | 1 << MASK_RX_DR); //clear the interrupt flags
	config |= fail << MASK_MAX_RT | tx << MASK_TX_DS | rx << MASK_RX_DR; // set the specified interrupt flags
	write_register(NRF_CONFIG, config);
}

uint8_t getDynamicPayloadSize(void)
{
	uint8_t result = 0, addr;
	csn(LOW);
	addr = R_RX_PL_WID;
	HAL_SPI_TransmitReceive(&hspi1, &addr, &result, 1, 1000);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xff, &result, 1, 1000);
	csn(HIGH);

	if(result > 32)
	{
		flush_rx();
		HAL_Delay(2);
		return 0;
	}

	return result;
}

bool availableMy(void)
{
	return available(NULL);
}

bool available(uint8_t* pipe_num)
{
	if(!(read_register(FIFO_STATUS) & (1 << RX_EMPTY)))
	{
		if(pipe_num) // If the caller wants the pipe number, include that
		{
			uint8_t status = get_status();
			*pipe_num = (status >> RX_P_NO) & 0x07;
		}

		return 1;
	}

	return 0;
}

void read(void* buf, uint8_t len)
{
	read_payload(buf, len);
	write_register(NRF_STATUS, (1 << RX_DR) | (1 << MAX_RT) | (1 << TX_DS));
}


uint8_t whatHappened()
{
	uint8_t status = write_register(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
	/*uint8_t tx_ok = status & (1 << TX_DS);
	uint8_t tx_fail = status & (1 << MAX_RT);
	uint8_t rx_ready = status & (1 << RX_DR);*/
	return status;
}

void openWritingPipe(uint64_t value)
{
	write_registerMy(RX_ADDR_P0, (uint8_t*)&value, addr_width);
	write_registerMy(TX_ADDR, (uint8_t*)&value, addr_width);
	write_register(RX_PW_P0, payload_size);
}


static const uint8_t child_pipe[] = {RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5};

static const uint8_t child_payload_size[] = {RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5};


void openReadingPipe(uint8_t child, uint64_t address)
{
	if(child == 0)
	{
		memcpy(pipe0_reading_address, &address, addr_width);
	}

	if(child <= 6)
	{
		// For pipes 2-5, only write the LSB
		if(child < 2)
		  write_registerMy(child_pipe[child], (const uint8_t*)&address, addr_width);
		else
		  write_registerMy(child_pipe[child], (const uint8_t*)&address, 1);

		write_register(child_payload_size[child], payload_size);
		write_register(EN_RXADDR, read_register(EN_RXADDR) | (1 << child_pipe_enable[child]));
	}
}

void setAddressWidth(uint8_t a_width)
{
	if(a_width -= 2)
	{
		write_register(SETUP_AW, a_width%4);
		addr_width = (a_width%4) + 2;
	}
	else
	{
        write_register(SETUP_AW, 0);
        addr_width = 2;
    }
}

void closeReadingPipe(uint8_t pipe)
{
	write_register(EN_RXADDR, read_register(EN_RXADDR) & ~(1 << child_pipe_enable[pipe]));
}

void toggle_features(void)
{
	uint8_t addr = ACTIVATE;
	csn(LOW);
	HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)0x73, 1, 1000);
	csn(HIGH);
}

void enableDynamicPayloads(void)
{
	write_register(FEATURE, read_register(FEATURE) | (1 << EN_DPL));
	write_register(DYNPD, read_register(DYNPD) | (1 << DPL_P5) | (1 << DPL_P4) | (1 << DPL_P3) | (1 << DPL_P2) | (1 << DPL_P1) | (1 << DPL_P0));
	dynamic_payloads_enabled = true;
}

void disableDynamicPayloads(void)
{
	write_register(FEATURE, 0);
	write_register(DYNPD, 0);
	dynamic_payloads_enabled = false;
}

void enableAckPayload(void)
{
	write_register(FEATURE, read_register(FEATURE) | (1 << EN_ACK_PAY) | (1 << EN_DPL));
	write_register(DYNPD, read_register(DYNPD) | (1 << DPL_P1) | (1 << DPL_P0));
	dynamic_payloads_enabled = true;
}

void enableDynamicAck(void)
{
    write_register(FEATURE, read_register(FEATURE) | (1 << EN_DYN_ACK));
}

void writeAckPayload(uint8_t pipe, const void* buf, uint8_t len)
{
	const uint8_t* current = (const uint8_t*)buf;
	uint8_t data_len = rf24_min(len, 32);
	uint8_t addr = W_ACK_PAYLOAD | (pipe & 0x07);
	csn(LOW);
	HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)current, data_len, 1000);
	csn(HIGH);
}

bool isAckPayloadAvailable(void)
{
	return !(read_register(FIFO_STATUS) & (1 << RX_EMPTY));
}

bool isPVariant(void)
{
	return p_variant;
}

void setAutoAck(bool enable)
{
	if(enable)
		write_register(EN_AA, 0x3F);
	else
		write_register(EN_AA, 0);
}

void setAutoAckPipe(uint8_t pipe, bool enable)
{
	if(pipe <= 6)
	{
		uint8_t en_aa = read_register(EN_AA);

		if(enable)
		{
			en_aa |= (1 << pipe);
		}
		else
		{
			en_aa &= ~(1 << pipe);
		}

		write_register(EN_AA, en_aa);
	}
}

void setPALevel(uint8_t level)
{
  uint8_t setup = read_register(RF_SETUP) & 0xF8;

  if(level > 3) // If invalid level, go to max PA
  {
	  level = (RF24_PA_MAX << 1) + 1;		// +1 to support the SI24R1 chip extra bit
  }
  else
  {
	  level = (level << 1) + 1;	 		// Else set level as requested
  }

  write_register(RF_SETUP, setup |= level);	// Write it to the chip
}

uint8_t getPALevel(void)
{
	return (read_register(RF_SETUP) & ((1 << RF_PWR_LOW) | (1 << RF_PWR_HIGH))) >> 1;
}

bool setDataRate(rf24_datarate_e speed)
{
	bool result = false;
	uint8_t setup = read_register(RF_SETUP);
	setup &= ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));
	txDelay = 85;

	if(speed == RF24_250KBPS)
	{
		setup |= (1 << RF_DR_LOW);
		txDelay = 155;
	}
	else
	{
		if(speed == RF24_2MBPS)
		{
			setup |= (1 << RF_DR_HIGH);
			txDelay = 65;
		}
	}

	write_register(RF_SETUP, setup);
	uint8_t ggg = read_register(RF_SETUP);

	if(ggg == setup)
	{
		result = true;
	}

	return result;
}

rf24_datarate_e getDataRate(void)
{
	rf24_datarate_e result ;
	uint8_t dr = read_register(RF_SETUP) & ((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));

	// switch uses RAM (evil!)
	// Order matters in our case below
	if(dr == (1 << RF_DR_LOW))
	{
		result = RF24_250KBPS;
	}
	else if(dr == (1 << RF_DR_HIGH))
	{
		result = RF24_2MBPS;
	}
	else
	{
		result = RF24_1MBPS;
	}

	return result;
}

void setCRCLength(rf24_crclength_e length)
{
	uint8_t config = read_register(NRF_CONFIG) & ~((1 << CRCO) | (1 << EN_CRC));

	if(length == RF24_CRC_DISABLED)
	{
		// Do nothing, we turned it off above.
	}
	else if(length == RF24_CRC_8)
	{
		config |= (1 << EN_CRC);
	}
	else
	{
		config |= (1 << EN_CRC);
		config |= (1 << CRCO);
	}

	write_register(NRF_CONFIG, config);
}

rf24_crclength_e getCRCLength(void)
{
	rf24_crclength_e result = RF24_CRC_DISABLED;

	uint8_t config = read_register(NRF_CONFIG) & ((1 << CRCO) | (1 << EN_CRC));
	uint8_t AA = read_register(EN_AA);

	if(config & (1 << EN_CRC) || AA)
	{
		if(config & (1 << CRCO))
		  result = RF24_CRC_16;
		else
		  result = RF24_CRC_8;
	}

	return result;
}

void disableCRC(void)
{
	uint8_t disable = read_register(NRF_CONFIG) & ~(1 << EN_CRC);
	write_register(NRF_CONFIG, disable);
}

void setRetries(uint8_t delay, uint8_t count)
{
	write_register(SETUP_RETR, (delay&0xf)<<ARD | (count&0xf)<<ARC);
}




