/*
 * dmx_sender.c
 *
 *  Created on: 04.04.2012
 *      Author: timon
 */
#include <avr/io.h>
#include <avr/interrupt.h>

// Baud rate calculations
#define BAUD 115200UL                          // baud rate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)  // round
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))    // real baud rate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)     // error in promille, 1000 = no error
#if ((BAUD_ERROR<986) || (BAUD_ERROR>1010))
#  error Systematic error in baud rate > 1% which is too much!
#endif

#include <util/setbaud.h>

#define MAX_ADDRESS 513
volatile uint8_t values[MAX_ADDRESS];

#define BREAK_LENGTH 20
#define MARK_AFTER_BREAK_LENGTH 220
#define SINGLE_BIT_LENGTH 249
#define DOUBLE_BIT_LENGTH 239
volatile uint8_t dmxTxState;
volatile uint8_t currentValue;
volatile uint16_t valueIndex;
volatile uint8_t valueToSet;
volatile uint8_t timerCountToSet;
volatile uint8_t blackOut;
volatile uint16_t maxValueIndex;

// Timer interrupt
ISR(TIMER0_OVF_vect) {
	TCNT0 = timerCountToSet;
	PORTD = valueToSet;

	if (dmxTxState > 0x80) {
		// 8 Datenbits
		if ((currentValue & blackOut) == 0x00) {
			valueToSet = PORTD & 0xFB;
		} else {
			valueToSet = PORTD | 0x04;
		}
		currentValue = currentValue >> 1;
		timerCountToSet = SINGLE_BIT_LENGTH;
		dmxTxState--;
	} else if (dmxTxState == 0x80) {
		// 2 Stopbits 1
		valueToSet = PORTD | 0x04;
		timerCountToSet = DOUBLE_BIT_LENGTH;

		valueIndex--;
		if (valueIndex == 0x00) {
			valueIndex = maxValueIndex; //MAX_ADDRESS;
			dmxTxState = 0x40;
		} else {
			dmxTxState = 0x10;
		}
	} else if (dmxTxState == 0x10) {
		// 1 Startbit 0
		valueToSet = PORTD & 0xF3;
		timerCountToSet = SINGLE_BIT_LENGTH;
		dmxTxState = 0x88;

		currentValue = values[maxValueIndex - valueIndex]; // MAX_ADDRESS - valueIndex];
	} else if (dmxTxState == 0x40) {
		// Break 0
		valueToSet = PORTD & 0xFB;
		timerCountToSet = BREAK_LENGTH;
		dmxTxState = 0x20;
	} else {
		// Mark after break 1
		timerCountToSet = MARK_AFTER_BREAK_LENGTH;
		if (dmxTxState == 0x20) {
			valueToSet = PORTD | 0x0C;
			dmxTxState = 0x10;
		}
	}
}

void init_uart(void) {
	//UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE); //enable UART RX, UART TX and interrupt for RX
	UCSRB = (1 << RXEN) | (1 << TXEN); //enable UART RX and UART TX
	UCSRC |= (1 << URSEL) | (3 << UCSZ0); // Asynchronous 8N1

	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;

	/*
	 UBRRH = UBRR_VAL >> 8;
	 UBRRL = UBRR_VAL & 0xFF;

	 UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // Asynchron 8N1
	 UCSRB |= (1<<RXEN);                        // UART RX einschalten
	 */
}

void init_timer(void) {
	TIMSK = TIMSK | (1 << TOIE0); //Enable timer overflow interrupt
	TCCR0 = TCCR0 | (1 << CS01); //Start timer with presscaler = 8
	//Set timer counter initial value*/
	TCNT0 = 0x00;
}

#define FLASH_UPDATE 0x02
#define RESET 0x22
#define RESET_RESPONSE_OK 0xA2
#define RESET_RESPONSE_FAIL 0xA3
#define VERSION_REQUEST 0x24
#define VERSION_REQUEST_RESPONSE_START 0xA4
#define VERSION_REQUEST_RESPONSE_END 0x00
#define NO_OPERATION 0x26
#define NO_OPERATION_RESPONSE 0xA6
#define SET_USER_MEMORY_VALUE_LOW_BIT 0x28
#define SET_USER_MEMORY_VALUE_HIGH_BIT 0x29
#define READ_USER_MEMORY_VALUE_LOW_BIT 0x2A
#define READ_USER_MEMORY_VALUE_HIGH_BIT 0x2B
#define READ_USER_MEMORY_VALUE_LOW_BIT_RESPONSE 0xAA
#define READ_USER_MEMORY_VALUE_HIGH_BIT_RESPONSE 0xAB

#define SET_TX_START_CODE 0x42
#define SET_TX_START_CODE_RESPONSE 0xC2
#define TURN_TX_ON 0x44
#define TURN_TX_OFF 0x46
#define SET_DMX_CHANNEL_VALUE_LOW_BIT 0x48
#define SET_DMX_CHANNEL_VALUE_HIGH_BIT 0x49
#define TURN_ON_BLACKOUT 0x4A
#define TURN_ON_BLACKOUT_RESPONSE 0xCA
#define TURN_OFF_BLACKOUT 0x4C
#define TURN_OFF_BLACKOUT_RESPONSE 0xCC
#define SET_THE_LAST_TX_CODE_LOW_BIT 0x4E
#define SET_THE_LAST_TX_CODE_HIGH_BIT 0x4F
#define SET_THE_LAST_TX_CODE_RESPONSE 0xCE
#define CHECK_TX_STATUS 0x50
#define CHECK_TX_STATUS_RESPONSE_TX_ON 0xC4
#define CHECK_TX_STATUS_RESPONSE_TX_OFF 0xC6
#define READ_TX_CHANNEL_VALUE_LOW_BIT 0x52
#define READ_TX_CHANNEL_VALUE_HIGH_BIT 0x53
#define READ_TX_CHANNEL_VALUE_LOW_BIT_RESPONSE 0xD2
#define READ_TX_CHANNEL_VALUE_HIGH_BIT_RESPONSE 0xD3
uint8_t command;
uint8_t commandData1;
uint8_t commandData2;
uint8_t lastCommand;

void reset(void) {
	PORTD = 0x0c;
	dmxTxState = 0x00;
	currentValue = 0x00;
	maxValueIndex = MAX_ADDRESS;

	blackOut = 0x01;

	command = 0x00;
	commandData1 = 0x00;
	commandData2 = 0x00;
	lastCommand = 0x00;

	uint16_t i;
	for (i = 1; i < MAX_ADDRESS; i++) {
		values[i] = 0x00;
	}

}

uint8_t uart_getc(void) {
	while (!(UCSRA & (1 << RXC)))
		// warten bis Zeichen verfuegbar
		;
	return UDR; // Zeichen aus UDR an Aufrufer zurueckgeben
}

void uart_putc(uint8_t c) {
	while (!(UCSRA & (1 << UDRE))) /* warten bis Senden moeglich */
	{
	}
	UDR = c; /* sende Zeichen */
}

int main(void) {

#if defined __AVR_ATmega8__
	// Setup ATmega8 device
	DDRD = 0x0C;// PD2 and PD3 as output;

	init_timer();
	init_uart();
#else
#	error This MCU is not supported
#endif

	reset();

	dmxTxState = 0x10;
	maxValueIndex = 513;

	//values[2] = 0xFF;
	//values[0x1F0] = 0xFF;


	sei();
	while (1) {
		//values[2] = values[2]-1;

		if ((UCSRA & (1 << RXC))) {
			command = uart_getc();
			if (command == SET_TX_START_CODE) {
				commandData1 = uart_getc();
				values[0] = commandData1;
				uart_putc(SET_TX_START_CODE_RESPONSE);
			} else if (command == NO_OPERATION) {
				uart_putc(NO_OPERATION_RESPONSE);
			} else if (command == SET_DMX_CHANNEL_VALUE_LOW_BIT) {
				commandData1 = uart_getc();
				commandData2 = uart_getc();
				values[commandData1 + 0x001] = commandData2;
			} else if (command == SET_DMX_CHANNEL_VALUE_HIGH_BIT) {
				commandData1 = uart_getc();
				commandData2 = uart_getc();
				values[commandData1 + 0x101] = commandData2;
			} else if (command == VERSION_REQUEST) {
				uart_putc(VERSION_REQUEST_RESPONSE_START);
				uart_putc(0x14);
				uart_putc('T');
				uart_putc('S');
				uart_putc(VERSION_REQUEST_RESPONSE_END);
			} else if (command == RESET) {
				if (lastCommand == NO_OPERATION) {
					reset();
					uart_putc(RESET_RESPONSE_OK);
				} else {
					uart_putc(RESET_RESPONSE_FAIL);
				}
			} else if (command == TURN_ON_BLACKOUT) {
				blackOut = 0x00;
				uart_putc(TURN_ON_BLACKOUT_RESPONSE);
			} else if (command == TURN_OFF_BLACKOUT) {
				blackOut = 0x01;
				uart_putc(TURN_OFF_BLACKOUT_RESPONSE);
			} else if (command == TURN_TX_ON) {
				dmxTxState = 0x10;
			} else if (command == TURN_TX_OFF) {
				dmxTxState = 0x00;
			} else if (command == SET_THE_LAST_TX_CODE_LOW_BIT) {
				commandData1 = uart_getc();
				maxValueIndex = commandData1 + 0x001;
				uart_putc(SET_THE_LAST_TX_CODE_RESPONSE);
			} else if (command == SET_THE_LAST_TX_CODE_HIGH_BIT) {
				commandData1 = uart_getc();
				maxValueIndex = commandData1 + 0x101;
				uart_putc(SET_THE_LAST_TX_CODE_RESPONSE);
			}
			lastCommand = command;
		}

	}
}

