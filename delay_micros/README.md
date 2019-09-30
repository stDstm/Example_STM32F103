Добавьте в проект файлы - delay_micros.c и delay_micros.h

В main.c добавить инклюд - #include "delay_micros.h"

В main() инициализировать счётчик - DWT_Init();

Пауза - delay_micros(20); - 20 микросекунд
