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


enum States { Start, Light1, Light2First, Light2Second, Light3, PauseLight1, PauseLight2First, PauseLight2Second, PauseLight3 } state;

void SM_Tick() {
	unsigned char button = ~PINA & 0x01;

	switch (state) {
		case Start:
			state = Light1;
			break;
		case Light1:
			if (button) {
				state = PauseLight1;
			}
			else {
				state = Light2First;
			}
			break;
		case Light2First:
			if (button) {
				state = PauseLight2First;
			}
			else {
				state = Light3;
			}
			break;
		case Light3:
			if (button) {
				state = PauseLight3;
			}
			else {
				state = Light2Second;
			}
			break;
		case Light2Second:
			if (button) {
				state = PauseLight2Second;
			}
			else {
				state = Light1;
			}
			break;
		case PauseLight1:
			if (!button) {
				state = Light2First; 
			}
			break;
		case PauseLight2First:
			if (!button) {
				state = Light3;
			}
			break;
		case PauseLight2Second:
			if (!button) {
				state = Light1;
			}
			break;
		case PauseLight3:
			if (!button) {
				state = Light1;
			}
			break;
		default:
			state = Start;
			break;
		// what was before, during, after???
		// if state = PB1;
		// if lastState = PB0
		// 	state = PB2
		// else if lastState = PB2
		// 	state = PB0;
		//
		// if state = pb2
		// 	state = pb1
		// if state = pb0
		// state = pb1
	}

	switch (state) {
		case PauseLight1:
		case Light1:
			PORTB = 0x01;
			break;
		case PauseLight2First:
		case PauseLight2Second:
		case Light2First:
		case Light2Second:
			PORTB = 0x02;
			break;
		case PauseLight3:
		case Light3:
			PORTB = 0x04;
			break;
		case Start:
		default:
			PORTB = 0x00;
			break;
			
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(3000);
	TimerOn();

	// initial state
	PORTB = 0x00;
//unsigned char button = 0x00;
    while (1) {
	SM_Tick();
	//
	//button = ~PINA & 0x01;
	//if (button) {
	//	PORTB = 0x01;
	//}
	//else {
	//	PORTB = 0x00;
	//}
	while (!TimerFlag);
	TimerFlag = 0;
    }
 
    return 1;
}
