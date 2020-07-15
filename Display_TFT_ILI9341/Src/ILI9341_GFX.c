

#include "ILI9341_GFX.h"
//#include "w25qxx.h"
#include "stdlib.h"

#define swap(a, b) { int16_t t = a; a = b; b = t; }

volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH	 = ILI9341_SCREEN_WIDTH;

#define SIZE_RECIV 10000
#define START_WRITE 1024

volatile uint8_t spi1_tx_full_flag = 0;
volatile uint8_t spi1_tx_half_flag = 0;

volatile uint8_t spi2_rx_full_flag = 0;
volatile uint8_t spi2_rx_half_flag = 0;



/* Send command (char) to LCD */
void ILI9341_Write_Command(uint8_t Command)
{
	DISP_DC_CMD;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Command;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}

/* Send Data (char) to LCD */
void ILI9341_Write_Data(uint8_t Data)
{
	DISP_DC_DATA;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Data;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}

/* Set Address - Location block - to draw into */
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
	DISP_DC_CMD;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2A;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_DATA;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)(X1 >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)X1;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)(X2 >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)X2;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_CMD;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2B;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_DATA;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)(Y1 >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)Y1;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)(Y2 >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (uint8_t)Y2;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_CMD;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2C;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}

/*HARDWARE RESET*/
void ILI9341_Reset(void)
{
	DISP_RST_RESET;
	HAL_Delay(200);
	DISP_CS_SELECT;
	HAL_Delay(200);
	DISP_RST_WORK;
}

/*Ser rotation of the screen - changes x0 and y0*/
void ILI9341_Set_Rotation(uint8_t Rotation)
{
	DISP_DC_CMD;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x36;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;

	switch(Rotation)
	{
		case SCREEN_VERTICAL_1:
			ILI9341_Write_Data(0x40|0x08);
			LCD_WIDTH = 240;
			LCD_HEIGHT = 320;
			break;
		case SCREEN_HORIZONTAL_1:
			ILI9341_Write_Data(0x20|0x08);
			LCD_WIDTH  = 320;
			LCD_HEIGHT = 240;
			break;
		case SCREEN_VERTICAL_2:
			ILI9341_Write_Data(0x80|0x08);
			LCD_WIDTH  = 240;
			LCD_HEIGHT = 320;
			break;
		case SCREEN_HORIZONTAL_2:
			ILI9341_Write_Data(0x40|0x80|0x20|0x08);
			LCD_WIDTH  = 320;
			LCD_HEIGHT = 240;
			break;
		default:
			break;
	}
}


/*Initialize LCD display*/
void ILI9341_Init(void)
{
	DISP_RST_WORK; /*Enable LCD display*/
	//DISP_CS_SELECT; /* Initialize SPI */
	ILI9341_Reset();

	//SOFTWARE RESET
	ILI9341_Write_Command(0x01);
	HAL_Delay(1000);

	//POWER CONTROL A
	ILI9341_Write_Command(0xCB);
	ILI9341_Write_Data(0x39);
	ILI9341_Write_Data(0x2C);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x34);
	ILI9341_Write_Data(0x02);

	//POWER CONTROL B
	ILI9341_Write_Command(0xCF);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0xC1);
	ILI9341_Write_Data(0x30);

	//DRIVER TIMING CONTROL A
	ILI9341_Write_Command(0xE8);
	ILI9341_Write_Data(0x85);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x78);

	//DRIVER TIMING CONTROL B
	ILI9341_Write_Command(0xEA);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x00);

	//POWER ON SEQUENCE CONTROL
	ILI9341_Write_Command(0xED);
	ILI9341_Write_Data(0x64);
	ILI9341_Write_Data(0x03);
	ILI9341_Write_Data(0x12);
	ILI9341_Write_Data(0x81);

	//PUMP RATIO CONTROL
	ILI9341_Write_Command(0xF7);
	ILI9341_Write_Data(0x20);

	//POWER CONTROL,VRH[5:0]
	ILI9341_Write_Command(0xC0);
	ILI9341_Write_Data(0x23);

	//POWER CONTROL,SAP[2:0];BT[3:0]
	ILI9341_Write_Command(0xC1);
	ILI9341_Write_Data(0x10);

	//VCM CONTROL
	ILI9341_Write_Command(0xC5);
	ILI9341_Write_Data(0x3E);
	ILI9341_Write_Data(0x28);

	//VCM CONTROL 2
	ILI9341_Write_Command(0xC7);
	ILI9341_Write_Data(0x86);

	//MEMORY ACCESS CONTROL
	ILI9341_Write_Command(0x36);
	ILI9341_Write_Data(0x48);

	//PIXEL FORMAT
	ILI9341_Write_Command(0x3A);
	ILI9341_Write_Data(0x55);

	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	ILI9341_Write_Command(0xB1);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x18);

	//DISPLAY FUNCTION CONTROL
	ILI9341_Write_Command(0xB6);
	ILI9341_Write_Data(0x08);
	ILI9341_Write_Data(0x82);
	ILI9341_Write_Data(0x27);

	//3GAMMA FUNCTION DISABLE
	ILI9341_Write_Command(0xF2);
	ILI9341_Write_Data(0x00);

	//GAMMA CURVE SELECTED
	ILI9341_Write_Command(0x26);
	ILI9341_Write_Data(0x01);

	//POSITIVE GAMMA CORRECTION
	ILI9341_Write_Command(0xE0);
	ILI9341_Write_Data(0x0F);
	ILI9341_Write_Data(0x31);
	ILI9341_Write_Data(0x2B);
	ILI9341_Write_Data(0x0C);
	ILI9341_Write_Data(0x0E);
	ILI9341_Write_Data(0x08);
	ILI9341_Write_Data(0x4E);
	ILI9341_Write_Data(0xF1);
	ILI9341_Write_Data(0x37);
	ILI9341_Write_Data(0x07);
	ILI9341_Write_Data(0x10);
	ILI9341_Write_Data(0x03);
	ILI9341_Write_Data(0x0E);
	ILI9341_Write_Data(0x09);
	ILI9341_Write_Data(0x00);

	//NEGATIVE GAMMA CORRECTION
	ILI9341_Write_Command(0xE1);
	ILI9341_Write_Data(0x00);
	ILI9341_Write_Data(0x0E);
	ILI9341_Write_Data(0x14);
	ILI9341_Write_Data(0x03);
	ILI9341_Write_Data(0x11);
	ILI9341_Write_Data(0x07);
	ILI9341_Write_Data(0x31);
	ILI9341_Write_Data(0xC1);
	ILI9341_Write_Data(0x48);
	ILI9341_Write_Data(0x08);
	ILI9341_Write_Data(0x0F);
	ILI9341_Write_Data(0x0C);
	ILI9341_Write_Data(0x31);
	ILI9341_Write_Data(0x36);
	ILI9341_Write_Data(0x0F);

	//EXIT SLEEP
	ILI9341_Write_Command(0x11);
	HAL_Delay(120);

	//TURN ON DISPLAY
	ILI9341_Write_Command(0x29);

	//STARTING ROTATION
	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
}


//INTERNAL FUNCTION OF LIBRARY, USAGE NOT RECOMENDED, USE Draw_Pixel INSTEAD
/*Sends single pixel colour information to LCD*/
void ILI9341_Draw_Colour(uint16_t Colour)
{
	DISP_DC_DATA;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (Colour >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Colour;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}

//INTERNAL FUNCTION OF LIBRARY
/*Sends block colour information to LCD*/
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size)
{
	DISP_DC_DATA;
	DISP_CS_SELECT;

	while(Size > 0)
	{
		while(!(DISP_SPI->SR & SPI_SR_TXE));
		DISP_SPI->DR = (Colour >> 8);
		while(!(DISP_SPI->SR & SPI_SR_TXE));
		DISP_SPI->DR = Colour;
		Size--;
	}

	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// USER FUNCTION //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//FILL THE ENTIRE SCREEN WITH SELECTED COLOUR (either #define-d ones or custom 16bit)
void ILI9341_Fill_Screen(uint16_t Colour)
{
	ILI9341_Set_Address(0, 0, LCD_WIDTH, LCD_HEIGHT);
	ILI9341_Draw_Colour_Burst(Colour, LCD_WIDTH * LCD_HEIGHT);
}

//DRAW PIXEL AT XY POSITION WITH SELECTED COLOUR
void ILI9341_Draw_Pixel(uint16_t X, uint16_t Y, uint16_t Colour)
{
	if((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	//ADDRESS
	DISP_DC_CMD;
	DISP_CS_SELECT;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2A;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_DATA;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (X >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = X;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = ((X + 1) >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (X + 1);
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	//ADDRESS
	DISP_DC_CMD;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2B;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_DATA;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (Y >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Y;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = ((Y + 1) >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (Y + 1);
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	//ADDRESS
	DISP_DC_CMD;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = 0x2C;
	while(!(DISP_SPI->SR & SPI_SR_TXE));

	DISP_DC_DATA;
	//while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = (Colour >> 8);
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_SPI->DR = Colour;
	while(!(DISP_SPI->SR & SPI_SR_TXE));
	
	DISP_CS_UNSELECT;
}

//DRAW RECTANGLE OF SET SIZE AND HEIGTH AT X and Y POSITION WITH CUSTOM COLOUR
void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour)
{
	if((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	if((X + Width - 1) >= LCD_WIDTH)
	{
		Width = LCD_WIDTH - X;
	}

	if((Y + Height - 1) >= LCD_HEIGHT)
	{
		Height = LCD_HEIGHT - Y;
	}

	ILI9341_Set_Address(X, Y, X + Width - 1, Y + Height - 1);
	ILI9341_Draw_Colour_Burst(Colour, Height * Width);
}

//DRAW LINE FROM X,Y LOCATION to X+Width,Y LOCATION
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour)
{
	if((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	if((X + Width - 1) >= LCD_WIDTH)
	{
		Width = LCD_WIDTH - X;
	}

	ILI9341_Set_Address(X, Y, X + Width - 1, Y);
	ILI9341_Draw_Colour_Burst(Colour, Width);
}

//DRAW LINE FROM X,Y LOCATION to X,Y+Height LOCATION
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour)
{
	if((X >= LCD_WIDTH) || (Y >= LCD_HEIGHT)) return;

	if((Y + Height - 1) >= LCD_HEIGHT)
	{
		Height = LCD_HEIGHT - Y;
	}

	ILI9341_Set_Address(X, Y, X, Y + Height - 1);
	ILI9341_Draw_Colour_Burst(Colour, Height);
}

/*Draw hollow circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1);

    while (x >= y)
    {
        ILI9341_Draw_Pixel(X + x, Y + y, Colour);
        ILI9341_Draw_Pixel(X + y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - x, Y + y, Colour);
        ILI9341_Draw_Pixel(X - x, Y - y, Colour);
        ILI9341_Draw_Pixel(X - y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius << 1) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius;
	int y = 0;
	int xChange = 1 - (Radius << 1);
	int yChange = 0;
	int radiusError = 0;
	
	while (x >= y)
	{
		for (int i = X - x; i <= X + x; i++)
		{
			ILI9341_Draw_Pixel(i, Y + y,Colour);
			ILI9341_Draw_Pixel(i, Y - y,Colour);
		}

		for (int i = X - y; i <= X + y; i++)
		{
			ILI9341_Draw_Pixel(i, Y + x,Colour);
			ILI9341_Draw_Pixel(i, Y - x,Colour);
		}

		y++;
		radiusError += yChange;
		yChange += 2;

		if(((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

/*Draw a hollow rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	int32_t 	Calc_Negative = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;		
	}
	else
	{
		X_length = X0 - X1;		
	}

	ILI9341_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
	ILI9341_Draw_Horizontal_Line(X0, Y1, X_length, Colour);
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;		
	}

	ILI9341_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
	ILI9341_Draw_Vertical_Line(X1, Y0, Y_length, Colour);
	
	if((X_length > 0)||(Y_length > 0)) 
	{
		ILI9341_Draw_Pixel(X1, Y1, Colour);
	}
}

/*Draw a filled rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	int32_t 	Calc_Negative = 0;
	
	uint16_t X0_true = 0;
	uint16_t Y0_true = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;
		X0_true = X0;
	}
	else
	{
		X_length = X0 - X1;
		X0_true = X1;
	}
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;
		Y0_true = Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;
		Y0_true = Y1;	
	}
	
	ILI9341_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);	
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ILI9341_Random_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	// Bresenham's algorithm - thx wikpedia

	int16_t steep = abs(y2 - y1) > abs(x2 - x1);

	if(steep)
	{
		swap(x1, y1);
		swap(x2, y2);
	}

	if(x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	int16_t dx, dy;

	dx = x2 - x1;
	dy = abs(y2 - y1);

	int16_t err = dx / 2;
	int16_t ystep;

	if(y1 < y2)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for(; x1 <= x2; x1++)
	{
		if(steep)
		{
			ILI9341_Draw_Pixel(y1, x1, color);
		}
		else
		{
			ILI9341_Draw_Pixel(x1, y1, color);
		}

		err -= dy;

		if(err < 0)
		{
			y1 += ystep;
			err += dx;
		}
	}
}


void ILI9341_Draw_Image(const char *image_array, uint16_t x_coordinat, uint16_t y_coordinat, uint16_t img_width, uint16_t img_height, uint32_t s_img)
{
	ILI9341_Set_Address(x_coordinat, y_coordinat, img_width + x_coordinat - 1, img_height + y_coordinat - 1);

	DISP_DC_DATA;
	DISP_CS_SELECT;

	for(uint32_t i = 0; i < s_img; i++)
	{
		while(!(DISP_SPI->SR & SPI_SR_TXE));
		DISP_SPI->DR = image_array[i];
	}

	while(!(DISP_SPI->SR & SPI_SR_TXE));
	DISP_CS_UNSELECT;
}


// Вывод символа
static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ILI9341_Set_Address(x, y, x + font.width - 1, y + font.height - 1);

    for(i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];

        for(j = 0; j < font.width; j++)
        {
            if((b << j) & 0x8000)
            {
                ILI9341_Write_Data(color >> 8);
                ILI9341_Write_Data(color & 0xFF);
            }
            else
            {
                ILI9341_Write_Data(bgcolor >> 8);
                ILI9341_Write_Data(bgcolor & 0xFF);
            }
        }
    }
}

// Вывод строки
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    while(*str)
    {
        if(x + font.width >= ILI9341_SCREEN_WIDTH)
        {
            x = 0;
            y += font.height;

            if(y + font.height >= ILI9341_SCREEN_HEIGHT)
            {
                break;
            }

            if(*str == ' ')
            {
                str++;
                continue;
            }
        }

        ILI9341_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
}

///////////////////////////////////////// FROM SD ///////////////////////////////////////////////////
/*void ILI9341_Draw_Image_From_SD(char *name_file, uint16_t X, uint16_t Y, uint16_t xsize, uint16_t ysize)
{
	uint16_t s_buf = 512;

	uint8_t read_buff[512] = {0,};
	UINT bytes_read = 0;

	if(f_open(&myfile, name_file, FA_READ) != FR_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"OpenF_ER\n", 9, 1000);
	}

	int32_t size_file = f_size(&myfile);
 
	ILI9341_Set_Address(X, Y, X + xsize - 1, Y + ysize - 1);
	DISP_DC_DATA;
	DISP_CS_SELECT;

	while(size_file > 0)
	{
		if(size_file < 512) s_buf = size_file;
		f_read(&myfile, read_buff, s_buf, &bytes_read);
		HAL_SPI_Transmit_DMA(DISP_SPI_PTR, (uint8_t*)read_buff, s_buf);
		size_file = size_file - 512;
	}
	

	if(f_close(&myfile) != FR_OK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"CloseF_ER\n", 10, 1000);
	}

	while((HAL_SPI_GetState(DISP_SPI_PTR) != HAL_SPI_STATE_READY));
	
	DISP_CS_UNSELECT;
}*/

/////////////////////////////////////////// From_W25QFLASH ////////////////////////////////////////////////
/*void ILI9341_Draw_Image_From_W25QFLASH(uint32_t len, uint32_t adres, uint16_t X, uint16_t Y, uint16_t xsize, uint16_t ysize)
{
	uint8_t reciv[SIZE_RECIV] = {0,};
	int32_t len_img = len;
	int32_t offset = 0;

	spi1_tx_full_flag = 1;
	spi1_tx_half_flag = 1;
	spi2_rx_half_flag = 0;

	w25qxx.Lock = 1;

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(0x0B);

	if(w25qxx.ID >= W25Q256)
		W25qxx_Spi((adres & 0xFF000000) >> 24);

	W25qxx_Spi((adres & 0xFF0000) >> 16);
	W25qxx_Spi((adres & 0xFF00) >> 8);
	W25qxx_Spi(adres & 0xFF);
	W25qxx_Spi(0);

	ILI9341_Set_Address(X, Y, X + xsize - 1, Y + ysize - 1);
	DISP_DC_DATA;
	DISP_CS_SELECT;

	while(len_img > 0)
	{
		if(len_img < SIZE_RECIV)
		{
			while(spi1_tx_full_flag == 0){}
			spi1_tx_full_flag = 0;

			HAL_SPI_Receive_DMA(W25QXX_SPI_PTR, (uint8_t*)reciv, len_img);

			while(spi2_rx_full_flag == 0){}
			spi2_rx_full_flag = 0;

			spi1_tx_full_flag = 0;
			HAL_SPI_Transmit_DMA(DISP_SPI_PTR, (uint8_t*)reciv, len_img);
			while(spi1_tx_full_flag == 0){}

			break;
		}

		while(spi1_tx_half_flag == 0){}/////////////////
		spi1_tx_half_flag = 0;

		HAL_SPI_Receive_DMA(W25QXX_SPI_PTR, (uint8_t*)reciv, SIZE_RECIV);


		while(spi2_rx_half_flag == 0){}
		spi2_rx_half_flag = 0;
		spi1_tx_full_flag = 0;

		HAL_SPI_Transmit_DMA(DISP_SPI_PTR, (uint8_t*)reciv, SIZE_RECIV);

		offset = (offset + SIZE_RECIV);
		len_img = (len_img - SIZE_RECIV);
	}

	W25QFLASH_CS_UNSELECT;
	w25qxx.Lock = 0;
	DISP_CS_UNSELECT;
}*/



