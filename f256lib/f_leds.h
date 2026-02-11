/*
 *	LED control for F256 series.
 *	Adapted from mu0nlibs/muleds for oscar64.
 */


#ifndef LEDS_H
#define LEDS_H
#ifndef WITHOUT_LEDS


#include "f256lib.h"


void ledsEnableManual(bool wantNet, bool wantLock, bool wantL1, bool wantL0, bool wantMedia, bool wantPower);


#pragma compile("f_leds.c")


#endif
#endif // LEDS_H
