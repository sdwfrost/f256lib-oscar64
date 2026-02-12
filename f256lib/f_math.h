/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef F256_MATH_H
#define F256_MATH_H
#ifndef WITHOUT_MATH


#include "f256lib.h"


int16_t  mathSignedDivision(int16_t a, int16_t b);
int16_t  mathSignedDivisionRemainder(int16_t a, int16_t b, int16_t *remainder);
int32_t  mathSignedMultiply(int16_t a, int16_t b);
uint32_t mathUnsignedAddition(uint32_t a, uint32_t b);
uint16_t mathUnsignedDivision(uint16_t a, uint16_t b);
uint16_t mathUnsignedDivisionRemainder(uint16_t a, uint16_t b, uint16_t *remainder);
uint32_t mathUnsignedMultiply(uint16_t a, uint16_t b);


#pragma compile("f_math.c")


#endif
#endif // F256_MATH_H
