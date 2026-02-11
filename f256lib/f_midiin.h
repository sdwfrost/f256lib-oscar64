/*
 *	MIDI input parsing for F256 MIDI hardware.
 *	Adapted from mu0nlibs/muMIDIin for oscar64.
 */


#ifndef MIDIIN_H
#define MIDIIN_H
#ifndef WITHOUT_MIDIIN


#include "f256lib.h"


typedef struct midiInDataS {
	uint8_t recByte;       // last received MIDI-in byte
	uint8_t nextIsNote;    // 1 = awaiting MIDI note byte
	uint8_t nextIsSpeed;   // 1 = awaiting velocity byte
	uint8_t isHit;         // 1 = note-on event, 0 = note-off event
	uint8_t lastCmd;       // last command for running status
	uint8_t lastNote;      // last MIDI note value
	uint8_t storedNote;    // stored note for running status
} midiInDataT;


extern bool midiInNoteColors[];


void midiInReset(midiInDataT *themidi);
void midiInProcess(midiInDataT *themidi);


#pragma compile("f_midiin.c")


#endif
#endif // MIDIIN_H
