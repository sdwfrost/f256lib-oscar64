/*
 *	Mouse support for F256 series.
 *	Ported from mu0nlibs/mumouse for oscar64.
 */

#ifndef MUMOUSE_H
#define MUMOUSE_H

#include "f256lib.h"


#define PS2_M_MODE_EN  0xD6E0
#define PS2_M_X_LO    0xD6E2
#define PS2_M_X_HI    0xD6E3
#define PS2_M_Y_LO    0xD6E4
#define PS2_M_Y_HI    0xD6E5


void prepMouse(void);


#pragma compile("mumouse.c")


#endif // MUMOUSE_H
