Работа с датчиком температуры DS18B20

В CubeMX настроить любой пин как GPIO_Output, например PB5


GPIO Output Level - Low

GPIO mode - Output Open Drain

GPIO Pull-up/Pull-down - No pull-up and no pull-down

Maximum output speed - High

-------------------------------------------------------
В файле ds18b20.c прописать этот пин...

#define MY_PORT GPIOB
#define MY_PIN  GPIO_PIN_5

-------------------------------------------------------


![](https://github.com/stDstm/Example_STM32F103/blob/master/ds18b20/cubemx2.png)

 
