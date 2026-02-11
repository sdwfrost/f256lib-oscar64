/*
 *	NES/SNES game pad support for F256.
 *	Adapted from mu0nlibs/mupads for oscar64.
 */


#ifndef WITHOUT_PADS


#include "f256lib.h"


void padsPollNES(void) {
	POKE(PAD_CTRL, PAD_TRIG | PAD_MODE_NES);
	POKE(PAD_CTRL, PAD_MODE_NES);
}


void padsPollSNES(void) {
	POKE(PAD_CTRL, PAD_TRIG | PAD_MODE_SNES);
	POKE(PAD_CTRL, PAD_MODE_SNES);
}


bool padsPollIsReady(void) {
	return (PEEK(PAD_STAT) & PAD_DONE);
}


void padsPollWait(void) {
	while ((PEEK(PAD_STAT) & PAD_DONE) == 0) {
		__asm { nop }
	}
}


#endif
