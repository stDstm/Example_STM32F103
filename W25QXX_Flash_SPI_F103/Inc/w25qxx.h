#ifndef _W25QXX_H
#define _W25QXX_H

/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
*/

#include "main.h"

extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart1;

#define DEBUG_UART               &huart1
#define W25QXX_SPI_PTR           &hspi2
//#define W25QXX_SPI               SPI2

#define W25QFLASH_CS_SELECT      HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET)
#define W25QFLASH_CS_UNSELECT    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET)


#define _W25QXX_USE_FREERTOS     0

#define INIT_DEBUG               1

#define W25_WRITE_DISABLE     0x04
#define W25_WRITE_ENABLE      0x06
#define W25_CHIP_ERASE        0xC7 //0x60
#define W25_SECTOR_ERASE      0x20
#define W25_BLOCK_ERASE       0xD8
#define W25_FAST_READ         0x0B
#define W25_PAGE_PROGRAMM     0x02
#define W25_GET_JEDEC_ID      0x9F
#define W25_READ_STATUS_1     0x05
#define W25_READ_STATUS_2     0x35
#define W25_READ_STATUS_3     0x15
#define W25_WRITE_STATUS_1    0x01
#define W25_WRITE_STATUS_2    0x31
#define W25_WRITE_STATUS_3    0x11
#define W25_READ_UNIQUE_ID    0x4B


typedef enum
{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
	
}W25QXX_ID_t;

typedef struct
{
	W25QXX_ID_t	ID;
	uint8_t		UniqID[8];
	uint16_t	PageSize;
	uint32_t	PageCount;
	uint32_t	SectorSize;
	uint32_t	SectorCount;
	uint32_t	BlockSize;
	uint32_t	BlockCount;
	uint32_t	CapacityInKiloByte;
	uint8_t		StatusRegister1;
	uint8_t		StatusRegister2;
	uint8_t		StatusRegister3;	
	uint8_t		Lock;
	
}w25qxx_t;

extern w25qxx_t	w25qxx;

//############################################################################
// in Page,Sector and block read/write functions, can put 0 to read maximum bytes 
//############################################################################

uint8_t	W25qxx_Init(void);

void W25qxx_EraseChip(void);
void W25qxx_EraseSector(uint32_t SectorAddr);
void W25qxx_EraseBlock(uint32_t BlockAddr);

uint32_t W25qxx_PageToSector(uint32_t PageAddress);
uint32_t W25qxx_PageToBlock(uint32_t PageAddress);
uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress);
uint32_t W25qxx_SectorToPage(uint32_t SectorAddress);
uint32_t W25qxx_BlockToPage(uint32_t BlockAddress);

uint8_t W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte);
uint8_t W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte);
uint8_t W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte);

void W25qxx_WriteByte(uint8_t byte, uint32_t addr);
void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
void W25qxx_WriteBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
void W25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_BlockSize);

uint8_t	W25qxx_Spi(uint8_t Data);

#endif

