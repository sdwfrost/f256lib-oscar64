/*
 *	PSG (Programmable Sound Generator) support for F256.
 *	Adapted from mu0nlibs/mupsg for oscar64.
 */


#ifndef PSG_H
#define PSG_H
#ifndef WITHOUT_PSG


#include "f256lib.h"


#define PSG_SILENCE  0x0F


extern const byte psgLow[64];
extern const byte psgHigh[64];


void psgShut(void);
void psgNoteOn(byte chan, uint16_t addr, byte loByte, byte hiByte, byte velocity);
void psgNoteOff(byte chan, uint16_t addr);
void psgSetMono(void);
void psgSetStereo(void);


// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

#define setMonoPSG    psgSetMono
#define setStereoPSG  psgSetStereo


#pragma compile("f_psg.c")


#endif
#endif // PSG_H
