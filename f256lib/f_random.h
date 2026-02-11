/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef RANDOM_H
#define RANDOM_H
#ifndef WITHOUT_RANDOM


#include "f256lib.h"


uint16_t randomRead(void);
void     randomReset(void);
void     randomSeed(uint16_t seed);


#pragma compile("f_random.c")


#endif
#endif // RANDOM_H
