/*	Author: lab
 *  Partner(s) Name: Jaired Jawed
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// lights should turn on
// once every three seconds in order
// PB0, PB1, PB2, PB0, PB1, PB2, ...

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


enum States { Start, Wait, Reset, AfterAdd, Add, AfterSub, Sub} state;
unsigned char counter = 0;

void SM_Tick() {
	unsigned char button1 = ~PINA & 0x01;
	unsigned char button2 = ~PINA & 0x02;


	switch (state) {
		case Start:
			if (button1) {
				state = Add;
			}
			else if (button2) {
				state = Sub;
			}
			else {
				state = Wait;
			}
			break;
		case Wait:
			if (button1) {
				state = Add;
			}
			else if (button2) {
				state = Sub;
			}
			break;
		case Reset:
			state = Wait;
			break;
		case Add:
			state = AfterAdd;
			break;
		case AfterAdd:
			if (button2) {
				state = Reset;
			}
			else if (button1) {
				if (counter >= 0x0A) {
					counter = 0x00;
					state = Add;
				}
				counter += 1;
			}
			else if (!button1) {
				state = Wait;
			}
			break;
		case Sub:
			state = AfterSub;
			break;
		case AfterSub:
			if (button1) {
				state = Reset;
			}
			else if (button2) {
				if (counter >= 0x0A) {
					counter = 0x00;
					state = Sub;
				}
				counter += 1;
			}
			else if (!button2) {
				state = Wait;
			}
			break;
		default:
			state = Start;
			break;
	}

	switch (state) {
		case Start:
			PORTB = 0x07;
			break;
		case Wait:
			break;
		case Add:
			if (PORTB < 0x09) {
				PORTB = PORTB + 1;
			}
			break;
		case AfterAdd:
			break;
		case Sub:
			if (PORTB > 0x00) {
				PORTB = PORTB - 1;
				break;
			}
		case AfterSub:
			break;
		case Reset:
			PORTB = 0x00;
			break;
		default:
			PORTC = 0x07;
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();

	// Defaults to 7
	PORTB = 0x07;

    while (1) {
	SM_Tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
 
    return 1;
}
