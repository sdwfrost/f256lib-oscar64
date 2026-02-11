/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_RANDOM


#include "f256lib.h"


uint16_t randomRead(void) {
	uint16_t result;

	POKE(VKY_RND_CTRL, 1);  // Enable.
	result = PEEKW(VKY_RNDL);

	return result;
}


void randomReset(void) {
	struct rtc_time_t clock;

	// Seed the random number generator from the clock.
	kernelArgs->u.common.buf = &clock;
	kernelArgs->u.common.buflen = sizeof(struct rtc_time_t);
	kernelCall(Clock.GetTime);
	randomSeed(mathUnsignedMultiply(clock.seconds, clock.centis));
}


void randomSeed(uint16_t seed) {
	POKEW(VKY_SEEDL, seed);
	POKE(VKY_RND_CTRL, 3);  // Enable, load seed.
}


#endif
