/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_MATH


#include "f256lib.h"


int16_t mathSignedDivision(int16_t a, int16_t b) {
	byte    signA = 0;
	byte    signB = 0;
	int16_t r;

	if (a < 0) {
		signA = 1;
		a = -a;
	}
	if (b < 0) {
		signB = 1;
		b = -b;
	}

	POKEW(DIVU_NUM_L, a);
	POKEW(DIVU_DEN_L, b);
	r = PEEKW(QUOU_LL);

	if (signA + signB == 1) r = -r;

	return r;
}


int16_t mathSignedDivisionRemainder(int16_t a, int16_t b, int16_t *remainder) {
	byte    signA = 0;
	byte    signB = 0;
	int16_t r;

	if (a < 0) {
		signA = 1;
		a = -a;
	}
	if (b < 0) {
		signB = 1;
		b = -b;
	}

	POKEW(DIVU_NUM_L, a);
	POKEW(DIVU_DEN_L, b);
	r = PEEKW(QUOU_LL);
	*remainder = PEEKW(REMU_HL);

	if (signA + signB == 1) r = -r;

	return r;
}


int32_t mathSignedMultiply(int16_t a, int16_t b) {
	byte    signA = 0;
	byte    signB = 0;
	int32_t r;

	if (a < 0) {
		signA = 1;
		a = -a;
	}
	if (b < 0) {
		signB = 1;
		b = -b;
	}

	POKEW(MULU_A_L, a);
	POKEW(MULU_B_L, b);
	r = PEEKD(MULU_LL);

	if (signA + signB == 1) r = -r;

	return r;
}


uint32_t mathUnsignedAddition(uint32_t a, uint32_t b) {
	POKED(ADD_A_LL, a);
	POKED(ADD_B_LL, b);
	return PEEKD(ADD_R_LL);
}


uint16_t mathUnsignedDivision(uint16_t a, uint16_t b) {
	POKEW(DIVU_NUM_L, a);
	POKEW(DIVU_DEN_L, b);
	return PEEKW(QUOU_LL);
}


uint16_t mathUnsignedDivisionRemainder(uint16_t a, uint16_t b, uint16_t *remainder) {
	POKEW(DIVU_NUM_L, a);
	POKEW(DIVU_DEN_L, b);
	*remainder = PEEKW(REMU_HL);
	return PEEKW(QUOU_LL);
}


uint32_t mathUnsignedMultiply(uint16_t a, uint16_t b) {
	POKEW(MULU_A_L, a);
	POKEW(MULU_B_L, b);
	return PEEKD(MULU_LL);
}


#endif
