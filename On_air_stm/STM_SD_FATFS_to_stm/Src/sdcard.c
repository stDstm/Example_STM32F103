/*
 * sdcard.c
 *
 *  Created on: 2 мая 2019 г.
 *      Author: dima
 */

#include "sdcard.h"


static void SDCARD_Select()
{
    HAL_GPIO_WritePin(SDCARD_CS_GPIO_Port, SDCARD_CS_Pin, GPIO_PIN_RESET);
}

void SDCARD_Unselect()
{
    HAL_GPIO_WritePin(SDCARD_CS_GPIO_Port, SDCARD_CS_Pin, GPIO_PIN_SET);
}

/*
R1: 0abcdefg
     ||||||`- 1th bit (g): card is in idle state
     |||||`-- 2th bit (f): erase sequence cleared
     ||||`--- 3th bit (e): illigal command detected
     |||`---- 4th bit (d): crc check error
     ||`----- 5th bit (c): error in the sequence of erase commands
     |`------ 6th bit (b): misaligned addres used in command
     `------- 7th bit (a): command argument outside allowed range
             (8th bit is always zero)
*/
static uint8_t SDCARD_ReadR1()
{
    uint8_t r1;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    for(;;)
    {
        HAL_SPI_TransmitReceive(&SDCARD_SPI_PORT, &tx, &r1, sizeof(r1), HAL_MAX_DELAY);
        if((r1 & 0x80) == 0) // 8th bit alwyas zero, r1 recevied
            break;
    }
    return r1;
}

// data token for CMD9, CMD17, CMD18 and CMD24 are the same
#define DATA_TOKEN_CMD9  0xFE
#define DATA_TOKEN_CMD17 0xFE
#define DATA_TOKEN_CMD18 0xFE
#define DATA_TOKEN_CMD24 0xFE
#define DATA_TOKEN_CMD25 0xFC

static int SDCARD_WaitDataToken(uint8_t token)
{
    uint8_t fb;
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    for(;;)
    {
        HAL_SPI_TransmitReceive(&SDCARD_SPI_PORT, &tx, &fb, sizeof(fb), HAL_MAX_DELAY);
        if(fb == token)
            break;

        if(fb != 0xFF)
            return -1;
    }
    return 0;
}

static int SDCARD_ReadBytes(uint8_t* buff, size_t buff_size)
{
    // make sure FF is transmitted during receive
    uint8_t tx = 0xFF;
    while(buff_size > 0)
    {
        HAL_SPI_TransmitReceive(&SDCARD_SPI_PORT, &tx, buff, 1, HAL_MAX_DELAY);
        buff++;
        buff_size--;
    }

    return 0;
}

static int SDCARD_WaitNotBusy()
{
    uint8_t busy;
    do {
        if(SDCARD_ReadBytes(&busy, sizeof(busy)) < 0)
        {
            return -1;
        }
    } while(busy != 0xFF);

    return 0;
}

int SDCARD_Init()
{
    /*
    Step 1.
    Set DI and CS high and apply 74 or more clock pulses to SCLK. Without this
    step under certain circumstances SD-card will not work. For instance, when
    multiple SPI devices are sharing the same bus (i.e. MISO, MOSI, CS).
    */
    SDCARD_Unselect();

    uint8_t high = 0xFF;
    for(int i = 0; i < 10; i++) // 80 clock pulses
    {
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, &high, sizeof(high), HAL_MAX_DELAY);
    }

    SDCARD_Select();

    /*
    Step 2.

    Send CMD0 (GO_IDLE_STATE): Reset the SD card.
    */
    if(SDCARD_WaitNotBusy() < 0)
    {
        SDCARD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SDCARD_ReadR1() != 0x01)
    {
        SDCARD_Unselect();
        return -1;
    }

    /*
    Step 3.
    After the card enters idle state with a CMD0, send a CMD8 with argument of
    0x000001AA and correct CRC prior to initialization process. If the CMD8 is
    rejected with illigal command error (0x05), the card is SDC version 1 or
    MMC version 3. If accepted, R7 response (R1(0x01) + 32-bit return value)
    will be returned. The lower 12 bits in the return value 0x1AA means that
    the card is SDC version 2 and it can work at voltage range of 2.7 to 3.6
    volts. If not the case, the card should be rejected.
    */
    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x08 /* CMD8 */, 0x00, 0x00, 0x01, 0xAA /* ARG */, (0x43 << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SDCARD_ReadR1() != 0x01)
    {
        SDCARD_Unselect();
        return -2; // not an SDHC/SDXC card (i.e. SDSC, not supported)
    }

    {
        uint8_t resp[4];
        if(SDCARD_ReadBytes(resp, sizeof(resp)) < 0)
        {
            SDCARD_Unselect();
            return -3;
        }

        if(((resp[2] & 0x01) != 1) || (resp[3] != 0xAA))
        {
            SDCARD_Unselect();
            return -4;
        }
    }

    /*
    Step 4.
    And then initiate initialization with ACMD41 with HCS flag (bit 30).
    */
    for(;;)
    {
        if(SDCARD_WaitNotBusy() < 0) // keep this!
        {
            SDCARD_Unselect();
            return -1;
        }

        {
            static const uint8_t cmd[] =
                { 0x40 | 0x37 /* CMD55 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
        }

        if(SDCARD_ReadR1() != 0x01)
        {
            SDCARD_Unselect();
            return -5;
        }

        if(SDCARD_WaitNotBusy() < 0) // keep this!
        {
            SDCARD_Unselect();
            return -1;
        }

        {
            static const uint8_t cmd[] =
                { 0x40 | 0x29 /* ACMD41 */, 0x40, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
            HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
        }

        uint8_t r1 = SDCARD_ReadR1();

        if(r1 == 0x00)
        {
            break;
        }

        if(r1 != 0x01)
        {
            SDCARD_Unselect();
            return -6;
        }
    }

    /*
    Step 5.
    After the initialization completed, read OCR register with CMD58 and check
    CCS flag (bit 30). When it is set, the card is a high-capacity card known
    as SDHC/SDXC.
    */
    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    {
        static const uint8_t cmd[] =
            { 0x40 | 0x3A /* CMD58 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -7;
    }

    {
        uint8_t resp[4];
        if(SDCARD_ReadBytes(resp, sizeof(resp)) < 0)
        {
            SDCARD_Unselect();
            return -8;
        }

        if((resp[0] & 0xC0) != 0xC0)
        {
            SDCARD_Unselect();
            return -9;
        }
    }

    SDCARD_Unselect();
    return 0;
}


int SDCARD_GetBlocksNumber(uint32_t* num)
{
    uint8_t csd[16];
    uint8_t crc[2];

    SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0)  // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    /* CMD9 (SEND_CSD) command */
    {
        static const uint8_t cmd[] =
            { 0x40 | 0x09 /* CMD9 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD9) < 0)
    {
        SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_ReadBytes(csd, sizeof(csd)) < 0)
    {
        SDCARD_Unselect();
        return -4;
    }

    if(SDCARD_ReadBytes(crc, sizeof(crc)) < 0)
    {
        SDCARD_Unselect();
        return -5;
    }

    SDCARD_Unselect();

    // first byte is VVxxxxxxxx where VV is csd.version
    if((csd[0] & 0xC0) != 0x40) // csd.version != 1
        return -6;

    uint32_t tmp = csd[7] & 0x3F; // two bits are reserved
    tmp = (tmp << 8) | csd[8];
    tmp = (tmp << 8) | csd[9];
    tmp = (tmp + 1) << 10;
    *num = tmp;

    return 0;
}

int SDCARD_ReadSingleBlock(uint32_t blockNum, uint8_t* buff)
{
    uint8_t crc[2];

    SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    /* CMD17 (SEND_SINGLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x11 /* CMD17 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };

    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD17) < 0)
    {
        SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_ReadBytes(buff, 512) < 0)
    {
        SDCARD_Unselect();
        return -4;
    }

    if(SDCARD_ReadBytes(crc, 2) < 0)
    {
        SDCARD_Unselect();
        return -5;
    }

    SDCARD_Unselect();
    return 0;
}


int SDCARD_WriteSingleBlock(uint32_t blockNum, const uint8_t* buff)
{
    SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    /* CMD24 (WRITE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x18 /* CMD24 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };

    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    uint8_t dataToken = DATA_TOKEN_CMD24;
    uint8_t crc[2] = { 0xFF, 0xFF };
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, &dataToken, sizeof(dataToken), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)buff, 512, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, crc, sizeof(crc), HAL_MAX_DELAY);

    /*
        dataResp:
        xxx0abc1
            010 - Data accepted
            101 - Data rejected due to CRC error
            110 - Data rejected due to write error
    */
    uint8_t dataResp;
    SDCARD_ReadBytes(&dataResp, sizeof(dataResp));

    if((dataResp & 0x1F) != 0x05) // data rejected
    {
        SDCARD_Unselect();
        return -3;
    }

    if(SDCARD_WaitNotBusy() < 0)
    {
        SDCARD_Unselect();
        return -4;
    }

    SDCARD_Unselect();
    return 0;
}

int SDCARD_ReadBegin(uint32_t blockNum)
{
    SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    /* CMD18 (READ_MULTIPLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x12 /* CMD18 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };

    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    SDCARD_Unselect();
    return 0;
}

int SDCARD_ReadData(uint8_t* buff)
{
    uint8_t crc[2];
    SDCARD_Select();

    if(SDCARD_WaitDataToken(DATA_TOKEN_CMD18) < 0)
    {
        SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_ReadBytes(buff, 512) < 0)
    {
        SDCARD_Unselect();
        return -2;
    }

    if(SDCARD_ReadBytes(crc, 2) < 0)
    {
        SDCARD_Unselect();
        return -3;
    }

    SDCARD_Unselect();
    return 0;

}

int SDCARD_ReadEnd() {
    SDCARD_Select();

    /* CMD12 (STOP_TRANSMISSION) */
    {
        static const uint8_t cmd[] = { 0x40 | 0x0C /* CMD12 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 };
        HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);
    }

    /*
    The received byte immediataly following CMD12 is a stuff byte, it should be
    discarded before receive the response of the CMD12
    */
    uint8_t stuffByte;
    if(SDCARD_ReadBytes(&stuffByte, sizeof(stuffByte)) < 0)
    {
        SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    SDCARD_Unselect();
    return 0;
}


int SDCARD_WriteBegin(uint32_t blockNum)
{
    SDCARD_Select();

    if(SDCARD_WaitNotBusy() < 0) // keep this!
    {
        SDCARD_Unselect();
        return -1;
    }

    /* CMD25 (WRITE_MULTIPLE_BLOCK) command */
    uint8_t cmd[] = {
        0x40 | 0x19 /* CMD25 */,
        (blockNum >> 24) & 0xFF, /* ARG */
        (blockNum >> 16) & 0xFF,
        (blockNum >> 8) & 0xFF,
        blockNum & 0xFF,
        (0x7F << 1) | 1 /* CRC7 + end bit */
    };

    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)cmd, sizeof(cmd), HAL_MAX_DELAY);

    if(SDCARD_ReadR1() != 0x00)
    {
        SDCARD_Unselect();
        return -2;
    }

    SDCARD_Unselect();
    return 0;
}

int SDCARD_WriteData(const uint8_t* buff)
{
    SDCARD_Select();

    uint8_t dataToken = DATA_TOKEN_CMD25;
    uint8_t crc[2] = { 0xFF, 0xFF };
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, &dataToken, sizeof(dataToken), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, (uint8_t*)buff, 512, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, crc, sizeof(crc), HAL_MAX_DELAY);

    /*
        dataResp:
        xxx0abc1
            010 - Data accepted
            101 - Data rejected due to CRC error
            110 - Data rejected due to write error
    */
    uint8_t dataResp;
    SDCARD_ReadBytes(&dataResp, sizeof(dataResp));

    if((dataResp & 0x1F) != 0x05) // data rejected
    {
        SDCARD_Unselect();
        return -1;
    }

    if(SDCARD_WaitNotBusy() < 0)
    {
        SDCARD_Unselect();
        return -2;
    }

    SDCARD_Unselect();
    return 0;
}

int SDCARD_WriteEnd()
{
    SDCARD_Select();

    uint8_t stopTran = 0xFD; // stop transaction token for CMD25
    HAL_SPI_Transmit(&SDCARD_SPI_PORT, &stopTran, sizeof(stopTran), HAL_MAX_DELAY);

    // skip one byte before readyng "busy"
    // this is required by the spec and is necessary for some real SD-cards!
    uint8_t skipByte;
    SDCARD_ReadBytes(&skipByte, sizeof(skipByte));

    if(SDCARD_WaitNotBusy() < 0)
    {
        SDCARD_Unselect();
        return -1;
    }

    SDCARD_Unselect();
    return 0;
}



