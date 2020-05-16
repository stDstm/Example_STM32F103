Выводит в USART любые числа (8 бит, 16 бит, 32 бит) в двоичном виде.

Подключить в проект файл print_bits.h

Добавить инклюд...

```
#include "print_bits.h"
```

Пример работы:

```
char msg[64];

print_bits(msg, 34568, 16);

HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 1000);
```

Напечатает число 34568 в двоичном виде 1000011100001000

Первый аргумент - буфер в который запишутся данные, второй - число, которое нужно преобразовать, третий - размерность числа (8 бит, 16 бит, 32 бит).


[![Donate](https://istarik.ru/uploads/images/00/00/01/2020/04/12/ff1b11.png)](https://istarik.ru/don.html)
