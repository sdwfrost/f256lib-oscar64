#ifndef OPL3EXT_H
#define OPL3EXT_H

/*
 * Extended OPL3 instrument structure and stage-write functions
 * for the opl3tweak doodle.
 * Ported from F256KsimpleCdoodles/opl3tweak/src/muopl3.h
 */

#include "f256lib.h"

typedef struct opl3InstrumentExt {
	uint8_t VT_DEPTH;
	uint8_t OP2_TVSKF, OP1_TVSKF;
	uint8_t OP2_KSLVOL, OP1_KSLVOL;
	uint8_t OP2_AD, OP1_AD;
	uint8_t OP2_SR, OP1_SR;
	uint8_t OP2_WAV, OP1_WAV;
	uint8_t CHAN_FEED;
} opl3InstrumentExtT;

void opl3ExtStageOne(void);
void opl3ExtStageTwo(uint8_t chan);

extern opl3InstrumentExtT opl3ExtInst;

#pragma compile("opl3Ext.c")

#endif // OPL3EXT_H
