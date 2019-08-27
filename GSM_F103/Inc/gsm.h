/*
 * gsm.h
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
#define ATCPAS "AT+CPAS" // проверка статуса модема, ответ — (+CPAS: 0), (0 — готов к работе, 2 — неизвестно, 3 — входящий звонок, 4 — в режиме соединения)
#define ATCREG "AT+CREG?" // регистрация в сети, интересен второй параметр в ответе - (+CREG: 0,1), 0 - не зарегистрирован в сети, 1 - зерегистрированно, 2 - не зарегистрированно идет поиск сети, 3 - регистрация отклонена, 4 - неизвестно, 5 - зарегистрированно роуминг
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

void replac_string(char *src);
void get_date_time(void);
void disable_connection(void);
void incoming_call(void);
void call(void);
void balance(void);
void set_comand(char *buff);

#endif /* GSM_H_ */
