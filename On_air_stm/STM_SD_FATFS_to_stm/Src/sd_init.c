/*
 * sd_init.c
 *
 *  Created on: 4 мая 2019 г.
 *      Author: dima
 */
//#include "main.h"
#include "stm32f3xx_hal.h"
//#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "fatfs.h"
#include "stdarg.h"
#include "string.h"


extern UART_HandleTypeDef huart1;
//extern USBD_HandleTypeDef hUsbDeviceFS;


void UART_Printf(const char* fmt, ...)
{
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    //HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff), HAL_MAX_DELAY);
    CDC_Transmit_FS((uint8_t*)buff, strlen(buff));
    va_end(args);
}


void init()
{
    FATFS fs;
    FRESULT res;
    UART_Printf("Ready!\r\n");

    // mount the default drive
    res = f_mount(&fs, "", 1);
    if(res != FR_OK)
    {
        UART_Printf("f_mount() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("f_mount() done!\r\n");

    uint32_t freeClust;
    FATFS* fs_ptr = &fs;
    res = f_getfree("", &freeClust, &fs_ptr); // Warning! This fills fs.n_fatent and fs.csize!

    if(res != FR_OK)
    {
        UART_Printf("f_getfree() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("f_getfree() done!\r\n");

    uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
    uint32_t freeBlocks = freeClust * fs.csize;

    UART_Printf("Total blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 2000);
    UART_Printf("Free blocks: %lu (%lu Mb)\r\n", freeBlocks, freeBlocks / 2000);

    DIR dir;
    res = f_opendir(&dir, "/");

    if(res != FR_OK)
    {
        UART_Printf("f_opendir() failed, res = %d\r\n", res);
        return;
    }

    FILINFO fileInfo;
    uint32_t totalFiles = 0;
    uint32_t totalDirs = 0;
    UART_Printf("--------\r\nRoot directory:\r\n");

    for(;;)
    {
        res = f_readdir(&dir, &fileInfo);
        if((res != FR_OK) || (fileInfo.fname[0] == '\0'))
        {
            break;
        }

        if(fileInfo.fattrib & AM_DIR)
        {
            UART_Printf("  DIR  %s\r\n", fileInfo.fname);
            totalDirs++;
        }
        else
        {
            UART_Printf("  FILE %s\r\n", fileInfo.fname);
            totalFiles++;
        }
    }

    UART_Printf("\r\nTotal: %lu dirs, %lu files\r\n--------\r\n", totalDirs, totalFiles);

    res = f_closedir(&dir);
    if(res != FR_OK)
    {
        UART_Printf("f_closedir() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Writing to log.txt...\r\n");

    char writeBuff[128];
    snprintf(writeBuff, sizeof(writeBuff), "Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 2000, freeBlocks, freeBlocks / 2000);

    FIL logFile;
    res = f_open(&logFile, "log.txt", FA_OPEN_ALWAYS | FA_WRITE);

    if(res != FR_OK)
    {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    unsigned int bytesToWrite = strlen(writeBuff);
    unsigned int bytesWritten;

    res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);

    if(res != FR_OK)
    {
        UART_Printf("f_write() failed, res = %d\r\n", res);
        return;
    }

    if(bytesWritten < bytesToWrite)
    {
        UART_Printf("WARNING! Disk is full, bytesToWrite = %lu, bytesWritten = %lu\r\n", bytesToWrite, bytesWritten);
    }

    res = f_close(&logFile);

    if(res != FR_OK)
    {
        UART_Printf("f_close() failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Reading file...\r\n");

    FIL msgFile;
    res = f_open(&msgFile, "test.txt", FA_READ);

    if(res != FR_OK)
    {
        UART_Printf("f_open() failed, res = %d\r\n", res);
        return;
    }

    char readBuff[128];
    unsigned int bytesRead;
    res = f_read(&msgFile, readBuff, sizeof(readBuff)-1, &bytesRead);

    if(res != FR_OK)
    {
        UART_Printf("f_read() failed, res = %d\r\n", res);
        return;
    }

    readBuff[bytesRead] = '\0';
    UART_Printf("%s\r\n", readBuff);
    HAL_Delay(10);

    res = f_close(&msgFile);

    if(res != FR_OK)
    {
        UART_Printf("f_close() failed, res = %d\r\n", res);
        return;
    }

    // Unmount
    res = f_mount(NULL, "", 0);

    if(res != FR_OK)
    {
        UART_Printf("Unmount failed, res = %d\r\n", res);
        return;
    }

    UART_Printf("Done!\r\n");
}
