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

enum States { Start, Light1, Light2, Light3 } state;

void SM_Tick() {
	switch (state) {
		case Start:
			state = Light1;
			break;
		case Light1:
			state = Light2;
			break;
		case Light2:
			state = Light3;
			break;
		case Light3:
			state = Light1;
			break;
		default:
			state = Start;

	}
		
	switch (state) {
		case Start:
			PORTB = 0x00;
			break;
		case Light1:
			PORTB = 0x01;
			break;
		case Light2:
			PORTB = 0x02;
			break;
		case Light3:
			PORTB = 0x04;
			break;
		default:
			PORTB = 0x00;
			break;
	}


}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF;
	PORTB = 0x00;
	TimerSet(1000);
	TimerOn();

	// initial state
	state = Start;
	PORTB = 0x00;

    while (1) {
	SM_Tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
 
    return 1;
}
