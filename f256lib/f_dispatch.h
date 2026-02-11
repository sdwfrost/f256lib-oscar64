/*
 *	Multi-chip note dispatch for polyphonic playback.
 *	Routes notes to SID, PSG, OPL3, and MIDI chips.
 *	Adapted from mu0nlibs/mudispatch for oscar64.
 */


#ifndef DISPATCH_H
#define DISPATCH_H
#ifndef WITHOUT_DISPATCH


#include "f256lib.h"


typedef struct dispatchGlobalsS {
	bool wantVS1053;
	uint8_t sidInstChoice;
	uint8_t opl3InstChoice;
	uint8_t chipChoice;
	// SID
	sidInstrumentT *sidValues;
	// OPL3 channel-wide parameters
	uint8_t o_2_tvskf, o_1_tvskf;
	uint8_t o_2_kslvol, o_1_kslvol;
	uint8_t o_2_ad, o_1_ad;
	uint8_t o_2_sr, o_1_sr;
	uint8_t o_2_wav, o_1_wav;
	uint8_t o_chanfeed;
} dispatchGlobalsT;


extern uint8_t dispatchChipAct[];
extern uint8_t dispatchReservedSID[];
extern uint8_t dispatchReservedPSG[];
extern uint8_t dispatchReservedOPL3[];
extern dispatchGlobalsT *dispatchGlobals;


int8_t  dispatchFindFreeChannel(uint8_t *ptr, uint8_t howManyChans, uint8_t *reserved);
int8_t  dispatchLiberateChannel(uint8_t note, uint8_t *ptr, uint8_t howManyChans);
void    dispatchNote(bool isOn, uint8_t channel, uint8_t note, uint8_t speed,
                     bool wantAlt, uint8_t whichChip, bool isBeat, uint8_t beatChan);
void    dispatchResetGlobals(dispatchGlobalsT *gT);


#pragma compile("f_dispatch.c")


#endif
#endif // DISPATCH_H
