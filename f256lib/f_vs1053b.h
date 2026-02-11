/*
 *	VS1053b codec support for F256.
 *	Adapted from mu0nlibs/muVS1053b for oscar64.
 */

#ifndef VS1053B_H
#define VS1053B_H
#ifndef WITHOUT_VS1053B

#include "f256lib.h"

void vs1053bInitPlugin(const uint16_t plugin[], uint16_t size);
void vs1053bBoostClock(void);
void vs1053bBoostBass(void);
void vs1053bInitBigPatch(void);
void vs1053bInitSpectrum(void);
void vs1053bInitRTMIDI(void);
uint16_t vs1053bCheckClock(void);
uint16_t vs1053bGetNbBands(void);
void vs1053bGetSAValues(uint16_t nbBands, uint16_t *values);

// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

#define initVS1053MIDI   vs1053bInitRTMIDI


#pragma compile("f_vs1053b.c")

#endif
#endif // VS1053B_H
