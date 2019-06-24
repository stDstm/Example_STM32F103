#include "nokia5110_LCD.h"

struct LCD_att lcd;
struct LCD_GPIO lcd_gpio;


/*----- GPIO Functions -----*/
/*
 * @brief Set functions for GPIO pins used
 * @param PORT: port of the pin used
 * @param PIN: pin of the pin used
 */
void LCD_setRST(GPIO_TypeDef* PORT, uint16_t PIN)
{
	lcd_gpio.RSTPORT = PORT;
	lcd_gpio.RSTPIN = PIN;
}

void LCD_setCE(GPIO_TypeDef* PORT, uint16_t PIN)
{
	lcd_gpio.CEPORT = PORT;
	lcd_gpio.CEPIN = PIN;
}

void LCD_setDC(GPIO_TypeDef* PORT, uint16_t PIN)
{
	lcd_gpio.DCPORT = PORT;
	lcd_gpio.DCPIN = PIN;
}
void LCD_setDIN(GPIO_TypeDef* PORT, uint16_t PIN)
{
	lcd_gpio.DINPORT = PORT;
	lcd_gpio.DINPIN = PIN;
}

void LCD_setCLK(GPIO_TypeDef* PORT, uint16_t PIN)
{
	lcd_gpio.CLKPORT = PORT;
	lcd_gpio.CLKPIN = PIN;
}

/*----- Library Functions -----*/

/*
 * @brief Send information to the LCD using configured GPIOs
 * @param val: value to be sent
 */
void LCD_send(uint8_t val)
{
  uint8_t i;

  for(i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(lcd_gpio.DINPORT, lcd_gpio.DINPIN, !!(val & (1 << (7 - i))));
    HAL_GPIO_WritePin(lcd_gpio.CLKPORT, lcd_gpio.CLKPIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(lcd_gpio.CLKPORT, lcd_gpio.CLKPIN, GPIO_PIN_RESET);
  }
}

/*
 * @brief Writes some data into the LCD
 * @param data: data to be written
 * @param mode: command or data
 */
void LCD_write(uint8_t data, uint8_t mode)
{
  if(mode == LCD_COMMAND)
  {
    HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_RESET);
    LCD_send(data);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_RESET);
    LCD_send(data);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_SET);
  }
}

/*
 * @brief Initialize the LCD using predetermined values
 */
void LCD_init()
{
  HAL_GPIO_WritePin(lcd_gpio.RSTPORT, lcd_gpio.RSTPIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(lcd_gpio.RSTPORT, lcd_gpio.RSTPIN, GPIO_PIN_SET);
  LCD_write(0x21, LCD_COMMAND); //LCD extended commands.
  LCD_write(0xB8, LCD_COMMAND); //set LCD Vop(Contrast).
  LCD_write(0x04, LCD_COMMAND); //set temp coefficent.
  LCD_write(0x14, LCD_COMMAND); //LCD bias mode 1:40.
  LCD_write(0x20, LCD_COMMAND); //LCD basic commands.
  LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND); //LCD normal.
  LCD_clrScr();
  lcd.inverttext = false;
}

/*
 * @brief Invert the color shown on the display
 * @param mode: true = inverted / false = normal
 */
void LCD_invert(bool mode)
{
  if(mode == true)
  {
    LCD_write(LCD_DISPLAY_INVERTED, LCD_COMMAND);
  }
  else
  {
    LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND);
  }
}

/*
 * @brief Invert the colour of any text sent to the display
 * @param mode: true = inverted / false = normal
 */
void LCD_invertText(bool mode)
{
  if(mode == true)
  {
    lcd.inverttext = true;
  }
  else
  {
    lcd.inverttext = false;
  }
}

/*
 * @brief Puts one char on the current position of LCD's cursor
 * @param c: char to be printed
 */
void LCD_putChar(char c)
{
  for(int i = 0; i < 6; i++)
  {
    if(lcd.inverttext != true)
      LCD_write(ASCII[c - 0x20][i], LCD_DATA);
    else
      LCD_write(~(ASCII[c - 0x20][i]), LCD_DATA);
  }
}

/*
 * @brief Print a string on the LCD
 * @param x: starting point on the x-axis (column)
 * @param y: starting point on the y-axis (line)
 */
void LCD_print(char *str, uint8_t x, uint8_t y)
{
  LCD_goXY(x, y);

  while(*str)
  {
    LCD_putChar(*str++);
  }
}

/*
 * @brief Clear the screen
 */
void LCD_clrScr()
{
  for(int i = 0; i < 504; i++)
  {
    LCD_write(0x00, LCD_DATA);
    lcd.buffer[i] = 0;
  }
}

/*
 * @brief Set LCD's cursor to position X,Y
 * @param x: position on the x-axis (column)
 * @param y: position on the y-axis (line)
 */
void LCD_goXY(uint8_t x, uint8_t y)
{
  LCD_write(0x80 | x, LCD_COMMAND); //Column.
  LCD_write(0x40 | y, LCD_COMMAND); //Row.
}

/*
 * @brief Updates the entire screen according to lcd.buffer
 */
void LCD_refreshScr()
{
  LCD_goXY(LCD_SETXADDR, LCD_SETYADDR);

  for(int i = 0; i < 6; i++)
  {
    for(int j = 0; j < LCD_WIDTH; j++)
    {
      LCD_write(lcd.buffer[(i * LCD_WIDTH) + j], LCD_DATA);
    }
  }
}

/*
 * @brief Updates a square of the screen according to given values
 * @param xmin: starting point on the x-axis
 * @param xmax: ending point on the x-axis
 * @param ymin: starting point on the y-axis
 * @param ymax: ending point on the y-axis
 */
void LCD_refreshArea(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax)
{
  for(int i = 0; i < 6; i++)
  {
    if(i * 8 > ymax)
    {
      break;
    }
    //LCD_goXY(xmin, i);
    LCD_write(LCD_SETYADDR | i, LCD_COMMAND);
    LCD_write(LCD_SETXADDR | xmin, LCD_COMMAND);

    for(int j = xmin; j <= xmax; j++)
    {
      LCD_write(lcd.buffer[(i * LCD_WIDTH) + j], LCD_DATA);
    }
  }
}

/*
 * @brief Sets a pixel on the screen
 */
void LCD_setPixel(uint8_t x, uint8_t y, bool pixel)
{
  if(x >= LCD_WIDTH)
    x = LCD_WIDTH - 1;
  if(y >= LCD_HEIGHT)
    y = LCD_HEIGHT - 1;
    
  if(pixel != false)
  {
    lcd.buffer[x + (y / 8) * LCD_WIDTH] |= 1 << (y % 8);
  }
  else
  {
    lcd.buffer[x + (y / 8) * LCD_WIDTH] &= ~(1 << (y % 8));
  }
}

/*
 * @brief Draws a horizontal line
 * @param x: starting point on the x-axis
 * @param y: starting point on the y-axis
 * @param l: length of the line
 */
void LCD_drawHLine(int x, int y, int l)
{
  int by, bi;

  if ((x>=0) && (x<LCD_WIDTH) && (y>=0) && (y<LCD_HEIGHT))
  {
    for (int cx=0; cx<l; cx++)
    {
      by=((y/8)*84)+x;
      bi=y % 8;
      lcd.buffer[by+cx] |= (1<<bi);
    }
  }
}

/*
 * @brief Draws a vertical line
 * @param x: starting point on the x-axis
 * @param y: starting point on the y-axis
 * @param l: length of the line
 */
void LCD_drawVLine(int x, int y, int l)
{

  if ((x>=0) && (x<84) && (y>=0) && (y<48))
  {
    for (int cy=0; cy<= l; cy++)
    {
      LCD_setPixel(x, y+cy, true);
    }
  }
}

/*
 * @brief abs function used in LCD_drawLine
 * @param x: any integer
 * @return absolute value of x
 */
int abs(int x)
{
	if(x < 0)
	{
		return x*-1;
	}
	return x;
}

/*
 * @brief Draws any line
 * @param x1: starting point on the x-axis
 * @param y1: starting point on the y-axis
 * @param x2: ending point on the x-axis
 * @param y2: ending point on the y-axis
 */
void LCD_drawLine(int x1, int y1, int x2, int y2)
{
  int tmp;
  double delta, tx, ty;
  
  if (((x2-x1)<0))
  {
    tmp=x1;
    x1=x2;
    x2=tmp;
    tmp=y1;
    y1=y2;
    y2=tmp;
  }

  if (((y2-y1)<0))
  {
    tmp=x1;
    x1=x2;
    x2=tmp;
    tmp=y1;
    y1=y2;
    y2=tmp;
  }
  
  if (y1==y2)
  {
    if (x1>x2)
    {
      tmp=x1;
      x1=x2;
      x2=tmp;
    }

    LCD_drawHLine(x1, y1, x2-x1);
  }
  else if (x1==x2)
  {
    if (y1>y2)
    {
      tmp=y1;
      y1=y2;
      y2=tmp;
    }

    LCD_drawHLine(x1, y1, y2-y1);
  }
  else if (abs(x2-x1)>abs(y2-y1))
  {
    delta=((double)(y2-y1)/(double)(x2-x1));
    ty=(double) y1;
    if (x1>x2)
    {
      for (int i=x1; i>=x2; i--)
      {
        LCD_setPixel(i, (int) (ty+0.5), true);
            ty=ty-delta;
      }
    }
    else
    {
      for (int i=x1; i<=x2; i++)
      {
        LCD_setPixel(i, (int) (ty+0.5), true);
        ty=ty+delta;
      }
    }
  }
  else
  {
    delta=((float) (x2-x1)/(float) (y2-y1));
    tx=(float) (x1);
        if (y1>y2)
        {
          for (int i=y2+1; i>y1; i--)
          {
            LCD_setPixel((int) (tx+0.5), i, true);
            tx=tx+delta;
          }
        }
        else
        {
          for (int i=y1; i<y2+1; i++)
          {
            LCD_setPixel((int) (tx+0.5), i, true);
            tx=tx+delta;
          }
        }
  }
}

