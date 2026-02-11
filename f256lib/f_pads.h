/*
 *	NES/SNES game pad support for F256.
 *	Adapted from mu0nlibs/mupads for oscar64.
 */


#ifndef PADS_H
#define PADS_H
#ifndef WITHOUT_PADS


#include "f256lib.h"


void padsPollNES(void);
void padsPollSNES(void);
bool padsPollIsReady(void);
void padsPollWait(void);


// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

#define padPollDelayUntilReady  padsPollWait


#pragma compile("f_pads.c")


#endif
#endif // PADS_H
