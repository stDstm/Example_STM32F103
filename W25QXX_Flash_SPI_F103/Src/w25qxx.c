
#include "w25qxx.h"

#if (INIT_DEBUG == 1)
#include "string.h"
#include "stdio.h"
char buf[64] = {0,};
extern UART_HandleTypeDef huart1;
#endif


#define W25QXX_DUMMY_BYTE        0xA5

w25qxx_t w25qxx;

#if (_W25QXX_USE_FREERTOS == 1)
#define	W25qxx_Delay(delay)		osDelay(delay)
#include "cmsis_os.h"
#else
#define	W25qxx_Delay(delay)		HAL_Delay(delay)
#endif



//###################################################################################################################
uint8_t	W25qxx_Spi(uint8_t	Data)
{
	uint8_t	ret;

	HAL_SPI_TransmitReceive(W25QXX_SPI_PTR, &Data, &ret, 1, 100); // spi2


	/*while(!(W25QXX_SPI->SR & SPI_SR_TXE));
	W25QXX_SPI->DR = Data;

	while(!(W25QXX_SPI->SR & SPI_SR_RXNE));
	ret = W25QXX_SPI->DR;*/

	//while((W25QXX_SPI->SR & SPI_SR_BSY));
	//__HAL_SPI_CLEAR_OVRFLAG(&hspi2);

	return ret;
}

//###################################################################################################################
uint32_t W25qxx_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_GET_JEDEC_ID);

	Temp0 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp2 = W25qxx_Spi(W25QXX_DUMMY_BYTE);

	W25QFLASH_CS_UNSELECT;

	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

//###################################################################################################################
/*void W25qxx_ReadUniqID(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_READ_UNIQUE_ID);

	for(uint8_t	i = 0; i < 4; i++)
		W25qxx_Spi(W25QXX_DUMMY_BYTE);

	for(uint8_t	i = 0; i < 8; i++)
		w25qxx.UniqID[i] = W25qxx_Spi(W25QXX_DUMMY_BYTE);

	W25QFLASH_CS_UNSELECT;
}*/

//###################################################################################################################
void W25qxx_WriteEnable(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_WRITE_ENABLE);
	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(1);
}

//###################################################################################################################
void W25qxx_WriteDisable(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_WRITE_DISABLE);
	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(1);
}

//###################################################################################################################
/*uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
	uint8_t	status=0;
	W25QFLASH_CS_SELECT;

	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_READ_STATUS_1);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_READ_STATUS_2);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi(W25_READ_STATUS_3);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister3 = status;
	}	

	W25QFLASH_CS_UNSELECT;

	return status;
}*/

//###################################################################################################################
/*void W25qxx_WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3, uint8_t Data)
{
	W25QFLASH_CS_SELECT;

	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_WRITE_STATUS_1);
		w25qxx.StatusRegister1 = Data;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_WRITE_STATUS_2);
		w25qxx.StatusRegister2 = Data;
	}
	else
	{
		W25qxx_Spi(W25_WRITE_STATUS_3);
		w25qxx.StatusRegister3 = Data;
	}

	W25qxx_Spi(Data);

	W25QFLASH_CS_UNSELECT;
}*/

//###################################################################################################################
void W25qxx_WaitForWriteEnd(void)
{
	W25qxx_Delay(1);
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_READ_STATUS_1);

	do{
		w25qxx.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(1);
	}
	while((w25qxx.StatusRegister1 & 0x01) == 0x01);

	W25QFLASH_CS_UNSELECT;
}

//###################################################################################################################
uint8_t W25qxx_Init(void)
{
	w25qxx.Lock = 1;
	while(HAL_GetTick() < 100)
	W25qxx_Delay(1);

	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(100);

	uint32_t id;

	#if (INIT_DEBUG == 1)
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Init Begin...\n", 14, 1000);
	#endif
	
	id = W25qxx_ReadID();

	#if (INIT_DEBUG == 1)
	snprintf(buf, 64, "ID:0x%lX\n", id);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);
	#endif

	switch(id & 0x0000FFFF)
	{
		case 0x401A:	// 	w25q512
			w25qxx.ID = W25Q512;
			w25qxx.BlockCount = 1024;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q512\n", 14, 1000);
			#endif
		break;

		case 0x4019:	// 	w25q256
			w25qxx.ID = W25Q256;
			w25qxx.BlockCount = 512;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q256\n", 14, 1000);
			#endif
		break;

		case 0x4018:	// 	w25q128
			w25qxx.ID = W25Q128;
			w25qxx.BlockCount = 256;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q128\n", 14, 1000);
			#endif
		break;

		case 0x4017:	//	w25q64
			w25qxx.ID = W25Q64;
			w25qxx.BlockCount = 128;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q64\n", 13, 1000);
			#endif
		break;

		case 0x4016:	//	w25q32
			w25qxx.ID = W25Q32;
			w25qxx.BlockCount = 64;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q32\n", 13, 1000);
			#endif
		break;

		case 0x4015:	//	w25q16
			w25qxx.ID = W25Q16;
			w25qxx.BlockCount = 32;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q16\n", 13, 1000);
			#endif
		break;

		case 0x4014:	//	w25q80
			w25qxx.ID = W25Q80;
			w25qxx.BlockCount = 16;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q80\n", 13, 1000);
			#endif
		break;

		case 0x4013:	//	w25q40
			w25qxx.ID = W25Q40;
			w25qxx.BlockCount = 8;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q40\n", 13, 1000);
			#endif
		break;

		case 0x4012:	//	w25q20
			w25qxx.ID = W25Q20;
			w25qxx.BlockCount = 4;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q20\n", 13, 1000);
			#endif
		break;

		case 0x4011:	//	w25q10
			w25qxx.ID = W25Q10;
			w25qxx.BlockCount = 2;
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Chip: w25q10\n", 13, 1000);
			#endif
		break;

		default:
			#if (INIT_DEBUG == 1)
			HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Unknown ID\n", 11, 1000);
			#endif

			w25qxx.Lock = 0;
			return 0;
	}


	w25qxx.PageSize = 256;
	w25qxx.SectorSize = 0x1000;
	w25qxx.SectorCount = w25qxx.BlockCount * 16;
	w25qxx.PageCount = (w25qxx.SectorCount * w25qxx.SectorSize) / w25qxx.PageSize;
	w25qxx.BlockSize = w25qxx.SectorSize * 16;
	w25qxx.CapacityInKiloByte = (w25qxx.SectorCount * w25qxx.SectorSize) / 1024;

	//W25qxx_ReadUniqID();
	//W25qxx_ReadStatusRegister(1);
	//W25qxx_ReadStatusRegister(2);
	//W25qxx_ReadStatusRegister(3);

	#if (INIT_DEBUG == 1)
	snprintf(buf, 64, "Page Size: %d Bytes\n", w25qxx.PageSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Page Count: %lu\n", w25qxx.PageCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Sector Size: %lu Bytes\n", w25qxx.SectorSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Sector Count: %lu\n", w25qxx.SectorCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Block Size: %lu Bytes\n", w25qxx.BlockSize);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Block Count: %lu\n", w25qxx.BlockCount);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	snprintf(buf, 64, "Capacity: %lu KBytes\n", w25qxx.CapacityInKiloByte);
	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buf, strlen(buf), 1000);

	HAL_UART_Transmit(DEBUG_UART, (uint8_t*)"Init Done\n", 10, 1000);
	#endif

	w25qxx.Lock = 0;
	return 1;
}	

//###################################################################################################################
void W25qxx_EraseChip(void)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_CHIP_ERASE);
	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(10);

	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_EraseSector(uint32_t SectorAddr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();
	SectorAddr = SectorAddr * w25qxx.SectorSize;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_SECTOR_ERASE);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((SectorAddr & 0xFF000000) >> 24);

	W25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
	W25qxx_Spi((SectorAddr & 0xFF00) >> 8);
	W25qxx_Spi(SectorAddr & 0xFF);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_EraseBlock(uint32_t BlockAddr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();

	BlockAddr = BlockAddr * w25qxx.SectorSize * 16;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_BLOCK_ERASE);

	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((BlockAddr & 0xFF000000) >> 24);

	W25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
	W25qxx_Spi((BlockAddr & 0xFF00) >> 8);
	W25qxx_Spi(BlockAddr & 0xFF);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

//###################################################################################################################
uint32_t W25qxx_PageToSector(uint32_t PageAddress)
{
	return((PageAddress * w25qxx.PageSize) / w25qxx.SectorSize);
}

//###################################################################################################################
uint32_t W25qxx_PageToBlock(uint32_t PageAddress)
{
	return((PageAddress * w25qxx.PageSize) / w25qxx.BlockSize);
}

//###################################################################################################################
uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress)
{
	return((SectorAddress * w25qxx.SectorSize) / w25qxx.BlockSize);
}

//###################################################################################################################
uint32_t W25qxx_SectorToPage(uint32_t SectorAddress)
{
	return(SectorAddress * w25qxx.SectorSize) / w25qxx.PageSize;
}

//###################################################################################################################
uint32_t W25qxx_BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress * w25qxx.BlockSize) / w25qxx.PageSize;
}

//###################################################################################################################
uint8_t W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
	W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t size = 0;

	size = w25qxx.PageSize - OffsetInByte;
	WorkAddress = (OffsetInByte + Page_Address * w25qxx.PageSize);

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);

	W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
	W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
	W25qxx_Spi(WorkAddress & 0xFF);

	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, size, 100);

	W25QFLASH_CS_UNSELECT;

	for(uint16_t i = 0; i < size; i++)
	{
		if(pBuffer[i] != 0xFF)
		{
			w25qxx.Lock = 0;
			return 0;
		}
	}

	w25qxx.Lock = 0;
	return 1;
}

//##################################################################################################################
uint8_t W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
	W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t s_buf = 256;
	uint16_t size = 0;

	size = w25qxx.SectorSize - OffsetInByte;
	WorkAddress = (OffsetInByte + Sector_Address * w25qxx.SectorSize);

//char buf2[64] = {0,};
//snprintf(buf2, 64, "SIZE %d \n", size);
//HAL_UART_Transmit(&huart1, (uint8_t*)buf2, strlen(buf2), 100);

	uint16_t cikl = size / 256;
	uint16_t cikl2 = size % 256;
	uint16_t count_cikle = 0;

	if(size <= 256)
	{
		count_cikle = 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"1\n", 2, 100);
	}
	else if(cikl2 == 0)
	{
		count_cikle = cikl;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"2\n", 2, 100);
	}
	else
	{
		count_cikle = cikl + 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"3\n", 2, 100);
	}


	for(uint16_t i = 0; i < count_cikle; i++)
	{
		W25QFLASH_CS_SELECT;
		W25qxx_Spi(W25_FAST_READ);

		if(w25qxx.ID>=W25Q256)
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);

		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);

		W25qxx_Spi(0);

		if(size < 256) s_buf = size;

//snprintf(buf2, 64, "RECIV %d %d %d %lu\n", size, s_buf, i, WorkAddress);
//HAL_UART_Transmit(&huart1, (uint8_t*)buf2, strlen(buf2), 100);

		HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, s_buf, 100);

		W25QFLASH_CS_UNSELECT;

		for(uint16_t i = 0; i < s_buf; i++)
		{
			if(pBuffer[i] != 0xFF)
			{
				w25qxx.Lock = 0;
				return 0;
			}
		}

		size = size - 256;
		WorkAddress = WorkAddress + 256;
	}

	w25qxx.Lock = 0;
	return 1;
}

//###################################################################################################################
uint8_t W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
	W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t s_buf = 256;
	uint32_t size = 0;

	size = w25qxx.BlockSize - OffsetInByte;
	WorkAddress = (OffsetInByte + Block_Address * w25qxx.BlockSize);

//char buf2[64] = {0,};
//snprintf(buf2, 64, "SIZEB %lu \n", size);
//HAL_UART_Transmit(&huart1, (uint8_t*)buf2, strlen(buf2), 100);

	uint16_t cikl = size / 256;
	uint16_t cikl2 = size % 256;
	uint16_t count_cikle = 0;

	if(size <= 256)
	{
		count_cikle = 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"1\n", 2, 100);
	}
	else if(cikl2 == 0)
	{
		count_cikle = cikl;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"2\n", 2, 100);
	}
	else
	{
		count_cikle = cikl + 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"3\n", 2, 100);
	}


	for(uint16_t i = 0; i < count_cikle; i++)
	{
		W25QFLASH_CS_SELECT;
		W25qxx_Spi(W25_FAST_READ);

		if(w25qxx.ID>=W25Q256)
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);

		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);

		W25qxx_Spi(0);

		if(size < 256) s_buf = size;

//snprintf(buf2, 64, "RECIVB %lu %d %d %lu\n", size, s_buf, i, WorkAddress);
//HAL_UART_Transmit(&huart1, (uint8_t*)buf2, strlen(buf2), 100);

		HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, s_buf, 100);

		W25QFLASH_CS_UNSELECT;

		for(uint16_t i = 0; i < s_buf; i++)
		{
			if(pBuffer[i] != 0xFF)
			{
				w25qxx.Lock = 0;
				return 0;
			}
		}

		size = size - 256;
		WorkAddress = WorkAddress + 256;
	}

	w25qxx.Lock = 0;
	return 1;
}

//###################################################################################################################
void W25qxx_WriteByte(uint8_t byte, uint32_t addr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_PAGE_PROGRAMM);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((addr & 0xFF000000) >> 24);

	W25qxx_Spi((addr & 0xFF0000) >> 16);
	W25qxx_Spi((addr & 0xFF00) >> 8);
	W25qxx_Spi(addr & 0xFF);

	W25qxx_Spi(byte);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	if(((NumByteToWrite_up_to_PageSize + OffsetInByte) > w25qxx.PageSize) || (NumByteToWrite_up_to_PageSize == 0))
		NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;

	if((OffsetInByte + NumByteToWrite_up_to_PageSize) > w25qxx.PageSize)
		NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;


	W25qxx_WaitForWriteEnd();

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_PAGE_PROGRAMM);

	Page_Address = (Page_Address * w25qxx.PageSize) + OffsetInByte;

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);

	W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Page_Address & 0xFF00) >> 8);
	W25qxx_Spi(Page_Address & 0xFF);

	HAL_SPI_Transmit(W25QXX_SPI_PTR, pBuffer, NumByteToWrite_up_to_PageSize, 100);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if((NumByteToWrite_up_to_SectorSize > w25qxx.SectorSize) || (NumByteToWrite_up_to_SectorSize == 0))
		NumByteToWrite_up_to_SectorSize = w25qxx.SectorSize;

	uint32_t StartPage;
	int32_t	BytesToWrite;
	uint32_t LocalOffset;

	if((OffsetInByte + NumByteToWrite_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToWrite = w25qxx.SectorSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;	

	StartPage = W25qxx_SectorToPage(Sector_Address) + (OffsetInByte / w25qxx.PageSize);
	LocalOffset = OffsetInByte % w25qxx.PageSize;

	do
	{		
		W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;

		BytesToWrite -= w25qxx.PageSize - LocalOffset;
		//pBuffer += w25qxx.PageSize;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToWrite > 0);
}

//###################################################################################################################
void W25qxx_WriteBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if((NumByteToWrite_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToWrite_up_to_BlockSize == 0))
		NumByteToWrite_up_to_BlockSize=w25qxx.BlockSize;

	uint32_t	StartPage;
	int32_t		BytesToWrite;
	uint32_t	LocalOffset;

	if((OffsetInByte+NumByteToWrite_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToWrite = w25qxx.BlockSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;	

	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/w25qxx.PageSize);

	LocalOffset = OffsetInByte%w25qxx.PageSize;	

	do
	{		
		W25qxx_WritePage(pBuffer,StartPage,LocalOffset,BytesToWrite);
		StartPage++;
		BytesToWrite -= w25qxx.PageSize - LocalOffset;
		//pBuffer += w25qxx.PageSize;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToWrite > 0);
}

//###################################################################################################################
void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);

	w25qxx.Lock=1;

	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_FAST_READ);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((Bytes_Address & 0xFF000000) >> 24);

	W25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Bytes_Address& 0xFF00) >> 8);
	W25qxx_Spi(Bytes_Address & 0xFF);
	W25qxx_Spi(0);

	*pBuffer = W25qxx_Spi(W25QXX_DUMMY_BYTE);

	W25QFLASH_CS_UNSELECT;

	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_ReadBytes(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while(w25qxx.Lock == 1)
	W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((ReadAddr & 0xFF000000) >> 24);

	W25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
	W25qxx_Spi((ReadAddr& 0xFF00) >> 8);
	W25qxx_Spi(ReadAddr & 0xFF);
	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, NumByteToRead, 2000);

	/*uint16_t i = 0;

	while(NumByteToRead > 0U)
	{
		//while(!(W25QXX_SPI->SR & SPI_SR_TXE));
		W25QXX_SPI->DR = 0;

		while(!(W25QXX_SPI->SR & SPI_SR_RXNE));
		pBuffer[i++] = W25QXX_SPI->DR;
		NumByteToRead--;
	}*/

	W25QFLASH_CS_UNSELECT;

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

//###################################################################################################################
void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	if((NumByteToRead_up_to_PageSize>w25qxx.PageSize) || (NumByteToRead_up_to_PageSize==0))
		NumByteToRead_up_to_PageSize=w25qxx.PageSize;

	if((OffsetInByte+NumByteToRead_up_to_PageSize) > w25qxx.PageSize)
		NumByteToRead_up_to_PageSize = w25qxx.PageSize - OffsetInByte;

	Page_Address = Page_Address * w25qxx.PageSize + OffsetInByte;
	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);

	W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Page_Address& 0xFF00) >> 8);
	W25qxx_Spi(Page_Address & 0xFF);

	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, NumByteToRead_up_to_PageSize, 100);

	/*uint16_t i = 0;

	while(NumByteToRead_up_to_PageSize > 0U)
	{
		while(!(W25QXX_SPI->SR & SPI_SR_TXE));
		W25QXX_SPI->DR = 0;

		while(!(W25QXX_SPI->SR & SPI_SR_RXNE));
		pBuffer[i++] = W25QXX_SPI->DR;
		NumByteToRead_up_to_PageSize--;
	}*/

	W25QFLASH_CS_UNSELECT;

	W25qxx_Delay(1);
	w25qxx.Lock=0;
}

//###################################################################################################################
void W25qxx_ReadSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_SectorSize)
{	
	if((NumByteToRead_up_to_SectorSize>w25qxx.SectorSize) || (NumByteToRead_up_to_SectorSize==0))
		NumByteToRead_up_to_SectorSize=w25qxx.SectorSize;

	uint32_t StartPage;
	int32_t	BytesToRead;
	uint32_t LocalOffset;

	if((OffsetInByte + NumByteToRead_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToRead = w25qxx.SectorSize - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;	

	StartPage = W25qxx_SectorToPage(Sector_Address) + (OffsetInByte / w25qxx.PageSize);

	LocalOffset = OffsetInByte % w25qxx.PageSize;

	do
	{		
		W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= w25qxx.PageSize-LocalOffset;
		//pBuffer+=w25qxx.PageSize;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToRead > 0);
}

//###################################################################################################################
void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t	NumByteToRead_up_to_BlockSize)
{
	if((NumByteToRead_up_to_BlockSize > w25qxx.BlockSize) || (NumByteToRead_up_to_BlockSize == 0))
		NumByteToRead_up_to_BlockSize = w25qxx.BlockSize;

	uint32_t StartPage;
	int32_t	BytesToRead;
	uint32_t LocalOffset;

	if((OffsetInByte+NumByteToRead_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToRead = w25qxx.BlockSize-OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_BlockSize;

	StartPage = W25qxx_BlockToPage(Block_Address) + (OffsetInByte / w25qxx.PageSize);

	LocalOffset = OffsetInByte%w25qxx.PageSize;	

	do
	{		
		W25qxx_ReadPage(pBuffer,StartPage,LocalOffset,BytesToRead);
		StartPage++;
		BytesToRead-=w25qxx.PageSize-LocalOffset;
		//pBuffer+=w25qxx.PageSize;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset=0;
	}
	while(BytesToRead > 0);
}
//###################################################################################################################









