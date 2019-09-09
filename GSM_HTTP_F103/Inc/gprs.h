/*
 * gprs.h
 *
 *  Created on: 18 авг. 2019 г.
 *      Author: dima
 */

#ifndef GSM_H_
#define GSM_H_

#include "string.h"
#include "stdio.h"

#define GSM &huart1
#define DEBUG &huart3

/////////////////////////////////////// ИНФОРМАЦИОННЫЕ ДЕФАЙНЫ /////////////////////////////////////////////
#define ATCPAS "AT+CPAS" // проверка статуса модема, ответ — +CPAS: 0 (0 — готов к работе, 2 — неизвестно, 3 — входящий звонок, 4 — в режиме соединения)
#define ATCREG "AT+CREG?" // регистрация в сети, интересен второй параметр в ответе, 0 - не зарегистрирован в сети, 1 - зерегистрированно, 2 - не зарегистрированно идет поиск сети, 3 - регистрация отклонена, 4 - неизвестно, 5 - зарегистрированно роуминг
#define ATI "ATI" // название и версия модуля
#define ATCGSN "AT+CGSN" // считывание IMEI из EEPROM
#define ATCSPN "AT+CSPN?" // оператор
#define ATIPR "AT+IPR?" // узнать скорость порта
#define ATCSQ "AT+CSQ" // уровень сигнала. Может быть в диапазоне 0..31, чем больше тем лучше. 99 значит отсутствие сигнала.


/////////////////////////////////////// ДЕФАЙНЫ НАСТРОЙКИ /////////////////////////////////////////////
#define ATCLIP1 "AT+CLIP=1" // включить АОН, 0 - отключить
#define ATE "ATE0" // отключить «эхо», 1 - включить
#define ATCCLKK "AT+CCLK=\"19/08/21,14:03:00+00\"" // установить дату/время - yy/mm/dd,hh:mm:ss+zz
#define ATIPRS "AT+IPR=57600" // установить скорость порта
#define ATS "ATS0=0" // поднимать трубку только "вручную", ATS0=2 - поднимать трубку после двух гудков, ATS0=3 - псле трёх и т.д.
#define ATGSMBUSY "AT+GSMBUSY=1" // запрет всех входящих звонков. 0 — разрешены.
#define ATDDET "AT+DDET=1" // включить DTMF, 0 - отключить.
#define ATCMGF "AT+CMGF=1" // устанавливает текстовый режим смс-сообщения
#define ATCPBS "AT+CPBS=\"SM\"" // открывает доступ к данным телефонной книги SIM-карты
#define ATCSCS "AT+CSCS=\"GSM\"" // кодировка текста - GSM
#define ATCNMI "AT+CNMI=1,2,2,1,0" // настройка вывода смс в консоль
#define ATCMEE "AT+CMEE=2" // включить полную расшифровку ошибок, "AT+CMEE=1" - ошибка обозначается цифровым кодом, "AT+CMEE=0" - отключить.

/////////////////////////////////////// ИНТЕРНЕТ /////////////////////////////////////////////
#define ATCGATT1 "AT+CGATT=1" // включить GPRS сервис (ждать)
#define ATCGATT0 "AT+CGATT=0" // отключить GPRS сервис (ждать)
#define ATCGATT "AT+CGATT?"  // проверить подключен ли к GPRS - +CGATT: 1    OK


/////////////////////////////////////// HTTP НАСТРОЙКА /////////////////////////////////////////////
#define ATSAPBR31CG "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"" // используем GPRS соединение (альтернатива CSD)
#define ATSAPBR31APN "AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\""
#define ATSAPBR31U "AT+SAPBR=3,1,\"USER\",\"mts\""
#define ATSAPBR31P "AT+SAPBR=3,1,\"PWD\",\"mts\""
#define ATSAPBR11 "AT+SAPBR=1,1" // установить соединение (ждать)
#define ATSAPBR01 "AT+SAPBR=0,1" // разорвать соединение (ждать)

/////////////////////////////////////// HTTP ПРОВЕРКА /////////////////////////////////////////////
#define ATSAPBR21 "AT+SAPBR=2,1" // посмотреть полученный IP адрес - +SAPBR: 1,1,"10.135.133.127"
#define ATSAPBR41 "AT+SAPBR=4,1" // посмотреть текущие настройки подключения
#define ATSAPBR51 "AT+SAPBR=5,1" // сохранить текущие настройки GPRS

/////////////////////////////////////// HTTP INIT /////////////////////////////////////////////
#define ATHTTPINIT "AT+HTTPINIT" // инициализация HTTP службы
#define ATHTTPPARA "AT+HTTPPARA=\"CID\",1" // установка CID параметра для http сессии



void replac_string(char *src);
void get_date_time(void);
void disable_connection(void);
void incoming_call(void);
void call(void);
void balance(void);
void set_comand(char *buff);

#endif /* GSM_H_ */
