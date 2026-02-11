/*
 *	LED control for F256 series.
 *	Adapted from mu0nlibs/muleds for oscar64.
 */


#ifndef WITHOUT_LEDS


#include "f256lib.h"


void ledsEnableManual(bool wantNet, bool wantLock, bool wantL1, bool wantL0, bool wantMedia, bool wantPower) {
	byte val = 0;
	if (wantPower) val |= VKY_SYS_PWR_LED;
	if (wantMedia) val |= VKY_SYS_SD_LED;
	if (wantL0)    val |= VKY_SYS_L0;
	if (wantL1)    val |= VKY_SYS_L1;
	if (wantLock)  val |= VKY_SYS_LCK;
	if (wantNet)   val |= VKY_SYS_NET;
	POKE(VKY_SYS0, val);
}


#endif
