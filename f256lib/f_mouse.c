/*
 *	PS/2 Mouse support for F256 series.
 *	Adapted from mu0nlibs/mumouse for oscar64.
 */


#ifndef WITHOUT_MOUSE


#include "f256lib.h"


void mouseInit(void) {
	POKE(PS2_M_MODE_EN, 0x01);
	POKEW(PS2_M_X_LO, 0x100);
	POKEW(PS2_M_Y_LO, 0x100);
}


void mouseShow(void) {
	POKE(PS2_M_MODE_EN, 0x01);
}


void mouseHide(void) {
	POKE(PS2_M_MODE_EN, 0x00);
}


#endif
