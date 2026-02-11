/*
 *	Hardware Timer 0 support for F256.
 *	Adapted from mu0nlibs/muTimer0Int for oscar64.
 */


#ifndef WITHOUT_TIMER0


#include "f256lib.h"


// Load a 24-bit comparison value into timer0
void timer0Load(uint32_t value) {
	POKE(TM0_CTRL, TM_CTRL_CLEAR);
	POKEA(TM0_CMP_L, value);
}


// Clear timer0 and restart in count-up mode with interrupt enabled
void timer0Reset(void) {
	POKE(TM0_CMP_CTRL, 0);
	POKE(TM0_CTRL, TM_CTRL_CLEAR);
	POKE(TM0_CTRL, TM_CTRL_INTEN | TM_CTRL_UP_DOWN | TM_CTRL_ENABLE);
}


// Set timer0 comparison value and start counting
void timer0Set(uint32_t value) {
	timer0Load(value);
	timer0Reset();
}


#endif
