/*
 *	Common utility functions for doodle examples.
 *	Ported from mu0nlibs/muUtils for oscar64.
 */

#ifndef MUUTILS_H
#define MUUTILS_H

#include "f256lib.h"


#define textColorGreen  0x04
#define textColorOrange 0x09
#define textColorRed    0x91
#define textColorBlue   0x07
#define textColorGray   0x05

#define TIMER_FRAMES  0
#define TIMER_SECONDS 1


bool     setTimer(const struct timer_t *timer);
uint8_t  getTimerAbsolute(uint8_t units);
void     lilpause(uint8_t timedelay);
void     hitspace(void);
bool     isAnyK(void);
bool     isK2(void);
bool     hasCaseLCD(void);
bool     isWave2(void);
void     wipeBitmapBackground(uint8_t r, uint8_t g, uint8_t b);


#pragma compile("muUtils.c")


#endif // MUUTILS_H
