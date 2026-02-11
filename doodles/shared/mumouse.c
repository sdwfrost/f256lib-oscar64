/*
 *	Mouse support for F256 series.
 *	Ported from mu0nlibs/mumouse for oscar64.
 */

#include "f256lib.h"
#include "mumouse.h"


void prepMouse(void) {
	POKE(PS2_M_MODE_EN, 0x01);
	POKEW(PS2_M_X_LO, 0x100);
	POKEW(PS2_M_Y_LO, 0x100);
}
