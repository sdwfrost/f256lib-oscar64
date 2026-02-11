/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef PLATFORM_H
#define PLATFORM_H
#ifndef WITHOUT_PLATFORM


#include "f256lib.h"


// Renamed from __putchar and getchar to avoid conflicts with
// oscar64's standard library.
void f256putchar(char c);
int  f256getchar(void);


// Platform detection (from mu0nlibs/muUtils)
bool platformIsAnyK(void);
bool platformIsK2(void);
bool platformHasCaseLCD(void);
bool platformIsWave2(void);


#pragma compile("f_platform.c")


#endif
#endif // PLATFORM_H
