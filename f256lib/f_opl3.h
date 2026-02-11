/*
 *	OPL3 (YMF262) FM synthesis support for F256.
 *	Adapted from mu0nlibs/muopl3 for oscar64.
 */


#ifndef OPL3_H
#define OPL3_H
#ifndef WITHOUT_OPL3


#include "f256lib.h"


typedef struct opl3InstrumentS {
	byte OP2_TVSKF, OP1_TVSKF;
	byte OP2_KSLVOL, OP1_KSLVOL;
	byte OP2_AD, OP1_AD;
	byte OP2_SR, OP1_SR;
	byte OP2_WAV, OP1_WAV;
	byte CHAN_FEED;
	byte CHAN_FRLO;
	byte CHAN_FNUM;
	byte KEYHIT;
	bool isHitRelease;
} opl3InstrumentT;


extern const uint16_t     opl3Fnums[12];
extern const byte         opl3InstrumentsSize;
extern opl3InstrumentT    opl3InstrumentDefs[18];
extern byte               opl3ChipVTPerc;
extern byte               opl3ChipPairs;
extern byte               opl3ChipEnable;
extern byte               opl3ChipNotesel;


void opl3Initialize(void);
void opl3InitializeDefs(void);
void opl3QuietAll(void);
void opl3Write(uint16_t address, byte value);
byte opl3Shadow(byte offset, byte value, byte hinb);
void opl3Note(byte channel, uint16_t fnum, byte block, bool onOrOff);
void opl3SetInstrument(opl3InstrumentT inst, byte channel);
void opl3SetInstrumentAllChannels(byte which, bool steamRoll);
void opl3SetFnum(byte val, byte which);
void opl3SetFrLo(byte val, byte which);
void opl3SetFeed(byte val, byte which);


#pragma compile("f_opl3.c")


#endif
#endif // OPL3_H
