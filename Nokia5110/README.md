 
Подходит для любых МК.

В Кубе настроить 5 пинов как Output, например PA3 - PA7 (можно переименовать их в соответствии с назначением) и прописать в функции ниже - ПОРТ, ПИН. Подключать в соответствии с названиями контактов экрана.

LCD_setRST(GPIOA, RST_Pin);
LCD_setCE(GPIOA, CE_Pin);
LCD_setDC(GPIOA, DC_Pin);
LCD_setDIN(GPIOA, DIN_Pin);
LCD_setCLK(GPIOA, CLK_Pin); 

Добавить в проект файлы nokia5110_LCD.h, nokia5110_LCD.с и font.h

В main.c заинклюдить nokia5110_LCD.h

Всё остальное прокомментировано в коде.

  
Источник...  
--------------------
Author: Caio Rodrigo  
Github: https://github.com/Zeldax64?tab=repositories

https://github.com/Zeldax64/Nokia-LCD5110-HAL
--------------------
 
