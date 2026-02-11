/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_PLATFORM


#include "f256lib.h"


void f256putchar(char c) {
	static char s[2] = { 0, 0 };
	s[0] = c;
	textPrint(s);
}


int f256getchar(void) {
	while (1) {
		kernelNextEvent();
		if (kernelError) {
			kernelCall(Yield);
			continue;
		}

		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;
		if (kernelEventData.u.key.flags) continue;  // Meta key.

		return kernelEventData.u.key.ascii;
	}
}


bool platformIsAnyK(void) {
	uint8_t value = PEEK(VKY_MID) & 0x1F;
	return (value >= 0x10 && value <= 0x16);
}


bool platformIsK2(void) {
	uint8_t value = PEEK(VKY_MID) & 0x1F;
	return (value == 0x11);
}


bool platformHasCaseLCD(void) {
	return ((PEEK(0xDDC1) & 0x02) == 0);
}


bool platformIsWave2(void) {
	uint8_t mid = PEEK(VKY_MID) & 0x3F;
	return (mid == 0x22 || mid == 0x11);
}


#endif
