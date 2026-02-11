/*
 * Music channel mapping for the mustart doodle.
 * Ported from F256KsimpleCdoodles/mustart/src/mumusicmap.h
 * Maps MIDI channels to specific sound chips (SID, PSG, OPL3, MIDI).
 */

#ifndef MUMUSICMAP_H
#define MUMUSICMAP_H

#include "f256lib.h"


void resetMap(void);

extern uint8_t chipXChannel[];

#pragma compile("mumusicmap.c")

#endif // MUMUSICMAP_H
