/* Исходная библиотека взята тут - https://github.com/z3t0/Arduino-IRremote */


#include "main.h"

extern TIM_HandleTypeDef htim4;

// настройка таймера для приема - переполнение каждые 50 мкс (в данном случае системная частота 72Мгц)
#define MYPRESCALER 71 // получаем частоту 1МГц
#define MYPERIOD 49    // 50 мкс

// настройка таймера для отправки - указать системную частоту таймера
#define MYSYSCLOCK 72000000

// настройка пина для приёма recive_IR
#define RECIV_PIN (HAL_GPIO_ReadPin(recive_IR_GPIO_Port, recive_IR_Pin))

#define true 1
#define false 0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RAWBUF 256

#define STATE_IDLE      2
#define STATE_MARK      3
#define STATE_SPACE     4
#define STATE_STOP      5
#define STATE_OVERFLOW  6

// Due to sensor lag, when received, Marks  tend to be 100us too long and Spaces tend to be 100us too short
#define MARK_EXCESS 100

// microseconds per clock interrupt tick
#define USECPERTICK 50

// Upper and Lower percentage tolerances in measurements
#define TOLERANCE 25
#define LTOL (1.0 - (TOLERANCE / 100.))
#define UTOL (1.0 + (TOLERANCE / 100.))

// Minimum gap between IR transmissions
#define _GAP            5000
#define GAP_TICKS       (_GAP/USECPERTICK)

#define TICKS_LOW(us)   ((int)(((us)*LTOL / USECPERTICK)))
#define TICKS_HIGH(us)  ((int)(((us)*UTOL / USECPERTICK + 1)))

// IR detector output is active low
#define MARK   0
#define SPACE  1

#define PRONTO_ONCE        false
#define PRONTO_REPEAT      true
#define PRONTO_FALLBACK    true
#define PRONTO_NOFALLBACK  false

#define REPEAT 0xFFFFFFFF // Decoded value for NEC when a repeat code is received

typedef struct // The fields are ordered to reduce memory over caused by struct-padding
{ 
		uint8_t       rcvstate;        // State Machine state
		uint8_t       rawlen;          // counter of entries in rawbuf
		uint16_t      timer;           // State timer, counts 50uS ticks.
		uint16_t      rawbuf[RAWBUF];  // raw data
		uint8_t       overflow;        // Raw buffer overflow occurred
} irparams_t;

volatile irparams_t irparams;


typedef enum 
{
		UNUSED = -1,
		UNKNOWN = 0,
		RC5,
		RC6,
		NEC,
		SONY,
		PANASONIC,
		JVC,
		SAMSUNG,
		WHYNTER,
		AIWA_RC_T501,
		LG,
		SANYO,
		MITSUBISHI,
		DISH,
		SHARP,
		DENON,
		PRONTO,
} decode_type_t;


// Results returned from the decoder
typedef struct 
{
		decode_type_t decode_type; // UNKNOWN, NEC, SONY, RC5, ...
		uint16_t address; // Used by Panasonic & Sharp [16-bits]
		uint32_t value; // Decoded value [max 32-bits]
		int16_t bits; // Number of bits in decoded value
		volatile uint16_t *rawbuf; // Raw intervals in 50uS ticks
		int16_t rawlen; // Number of records in rawbuf
		int16_t overflow; // true iff IR raw code too long
} decode_results;

decode_results results;

// Mark & Space matching functions
int MATCH(int measured, int desired);
int MATCH_MARK(int measured_ticks, int desired_us);
int MATCH_SPACE(int measured_ticks, int desired_us);

void DWT_Init();

int16_t my_decode(decode_results *results);
long decodeHash(decode_results *results);
int compare(unsigned int oldval, unsigned int newval);
void my_enableIRIn();
void my_resume();

//////////////////////////////////// активированные протоколы ////////////////////////////////////////
#define DECODE_RC5           1 // чтоб отключить декодирование протокола RC5 нужно указать 0
#define SEND_RC5             1 // чтоб отключить отправку сигнала по протоколу RC5 нужно указать 0

#define DECODE_RC6           1
#define SEND_RC6             1

#define DECODE_NEC           1
#define SEND_NEC             1

#define DECODE_SONY          1
#define SEND_SONY            1

#define DECODE_PANASONIC     1
#define SEND_PANASONIC       1

#define DECODE_JVC           1
#define SEND_JVC             1

#define DECODE_SAMSUNG       1
#define SEND_SAMSUNG         1

#define DECODE_WHYNTER       1
#define SEND_WHYNTER         1

#define DECODE_AIWA_RC_T501  1
#define SEND_AIWA_RC_T501    1

#define DECODE_LG            1
#define SEND_LG              1

#define DECODE_SANYO         1
#define SEND_SANYO           0 // NOT WRITTEN

#define DECODE_MITSUBISHI    1
#define SEND_MITSUBISHI      0 // NOT WRITTEN

#define DECODE_DISH          0 // NOT WRITTEN
#define SEND_DISH            1

#define DECODE_SHARP         0 // NOT WRITTEN
#define SEND_SHARP           1

#define DECODE_DENON         1
#define SEND_DENON           1

#define DECODE_PRONTO        0 // This function doe not logically make sense
#define SEND_PRONTO          1

///////////////////////////////////////////////////////////////////////////////////////////////
#if (DECODE_RC5 || DECODE_RC6)
	// This helper function is shared by RC5 and RC6
	int getRClevel(decode_results *results, int *offset, int *used, int t1);
#endif

#if DECODE_RC5
	uint8_t decodeRC5(decode_results *results);
#endif

#if DECODE_RC6
	uint8_t decodeRC6(decode_results *results);
#endif
//......................................................................
#if DECODE_NEC
	uint8_t decodeNEC(decode_results *results);
#endif
//......................................................................
#if DECODE_SONY
	uint8_t decodeSony(decode_results *results);
#endif
//......................................................................
#if DECODE_PANASONIC
	uint8_t decodePanasonic(decode_results *results);
#endif
//......................................................................
#if DECODE_JVC
	uint8_t decodeJVC(decode_results *results);
#endif
//......................................................................
#if DECODE_SAMSUNG
	uint8_t decodeSAMSUNG(decode_results *results);
#endif
//......................................................................
#if DECODE_WHYNTER
	uint8_t decodeWhynter(decode_results *results);
#endif
//......................................................................
#if DECODE_AIWA_RC_T501
	uint8_t decodeAiwaRCT501(decode_results *results);
#endif
//......................................................................
#if DECODE_LG
	uint8_t  decodeLG(decode_results *results);
#endif
//......................................................................
#if DECODE_SANYO
	uint8_t decodeSanyo(decode_results *results);
#endif
//......................................................................
#if DECODE_MITSUBISHI
	uint8_t decodeMitsubishi(decode_results *results);
#endif
//......................................................................
#if DECODE_DISH
	uint8_t decodeDish(decode_results *results); // NOT WRITTEN
#endif
//......................................................................
#if DECODE_SHARP
	uint8_t decodeSharp(decode_results *results); // NOT WRITTEN
#endif
//......................................................................
#if DECODE_DENON
	uint8_t decodeDenon(decode_results *results);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
void custom_delay_usec(unsigned long uSecs);
void enableIROut(uint8_t khz) ;
void mark(unsigned int usec);
void space(unsigned int usec);
void sendRaw(unsigned int buf[], unsigned int len, uint8_t hz);

///////////////////////////////////////////////////////////////////////////////////////////////
#if SEND_RC5
	void sendRC5(unsigned long data, int nbits);
#endif

#if SEND_RC6
	void sendRC6(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_NEC
	void sendNEC(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_SONY
	void sendSony(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_PANASONIC
	void sendPanasonic(unsigned int address, unsigned long data);
#endif
//......................................................................
#if SEND_JVC
	// JVC does NOT repeat by sending a separate code (like NEC does).
	// The JVC protocol repeats by skipping the header.
	// To send a JVC repeat signal, send the original code value
	//   and set 'repeat' to true
	void sendJVC(unsigned long data, int nbits, uint8_t repeat);
#endif
//......................................................................
#if SEND_SAMSUNG
	void sendSAMSUNG(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_WHYNTER
	void sendWhynter(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_AIWA_RC_T501
	void sendAiwaRCT501(int code);
#endif
//......................................................................
#if SEND_LG
	void sendLG(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_SANYO
	void sendSanyo(); // NOT WRITTEN
#endif
//......................................................................
#if SEND_MISUBISHI
	void sendMitsubishi(); // NOT WRITTEN
#endif
//......................................................................
#if SEND_DISH
	void sendDISH(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_SHARP
	void sendSharpRaw(unsigned long data, int nbits);
	void sendSharp(unsigned int address, unsigned int command);
#endif
//......................................................................
#if SEND_DENON
	void sendDenon(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_PRONTO
	void sendPronto(char* code, uint8_t repeat, uint8_t fallback);
#endif





