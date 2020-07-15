

#ifndef ILI9341_GFX_H
#define ILI9341_GFX_H

#include "main.h"
#include "fonts.h"

////////// DISPL ////////////
extern SPI_HandleTypeDef hspi1;
#define DISP_SPI_PTR     &hspi1
#define DISP_SPI         SPI1

////////////////////////////////////// настройка пинов /////////////////////////////////////
#define DISP_CS_SELECT      HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET)
#define DISP_CS_UNSELECT    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET)

#define DISP_DC_DATA        HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET)
#define DISP_DC_CMD         HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET)

#define DISP_RST_RESET      HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET)
#define DISP_RST_WORK       HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET)

///////////////// ширина высота ///////////////////
#define ILI9341_SCREEN_WIDTH 	320
#define ILI9341_SCREEN_HEIGHT   240

// различные цвета, создать нужный можно здесь https://trolsoft.ru/ru/articles/rgb565-color-picker
#define MYFON       0x2965 // #2F2F2F
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

/////////////////////////////////////////////////////////
#define SCREEN_VERTICAL_1		0
#define SCREEN_HORIZONTAL_1		1
#define SCREEN_VERTICAL_2		2
#define SCREEN_HORIZONTAL_2		3

/////////////////////////////////////////////////////////
void ILI9341_SPI_Init(void);
//void ILI9341_SPI_Send(unsigned char SPI_Data);
void ILI9341_Write_Command(uint8_t Command);
void ILI9341_Write_Data(uint8_t Data);
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);
void ILI9341_Reset(void);
void ILI9341_Set_Rotation(uint8_t Rotation);
void ILI9341_Init(void);
void ILI9341_Fill_Screen(uint16_t Colour);
void ILI9341_Draw_Colour(uint16_t Colour);
void ILI9341_Draw_Pixel(uint16_t X, uint16_t Y, uint16_t Colour);
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size);


void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour);
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour);
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour);
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour);
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour);
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour);
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour);
void ILI9341_Draw_Char(char character, uint16_t X, uint16_t Y, uint16_t colour, uint16_t size, uint16_t background_colour);
void ILI9341_Draw_Text(const char* text, uint16_t X, uint16_t Y, uint16_t colour, uint16_t size, uint16_t background_Colour);
void ILI9341_Draw_Filled_Rectangle_Size_Text(uint16_t X0, uint16_t Y0, uint16_t Size_X, uint16_t Size_Y, uint16_t Colour);

//USING CONVERTER: http://www.digole.com/tools/PicturetoC_Hex_converter.php
//65K colour (2Bytes / Pixel)
void ILI9341_Draw_Image(const char *image_array, uint16_t x_coordinat, uint16_t y_coordinat, uint16_t img_width, uint16_t img_height, uint32_t s_img);

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);

void ILI9341_Random_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

#endif















