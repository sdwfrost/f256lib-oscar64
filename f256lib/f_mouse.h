/*
 *	PS/2 Mouse support for F256 series.
 *	Adapted from mu0nlibs/mumouse for oscar64.
 */


#ifndef MOUSE_H
#define MOUSE_H
#ifndef WITHOUT_MOUSE


#include "f256lib.h"


void mouseInit(void);
void mouseShow(void);
void mouseHide(void);


// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

#define showMouse   mouseShow
#define hideMouse   mouseHide


#pragma compile("f_mouse.c")


#endif
#endif // MOUSE_H
