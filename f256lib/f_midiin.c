/*
 *	MIDI input parsing for F256 MIDI hardware.
 *	Adapted from mu0nlibs/muMIDIin for oscar64.
 */


#ifndef WITHOUT_MIDIIN


#include "f256lib.h"


bool midiInNoteColors[88] = {
	1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1,
	1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1,
	1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1,
	1,0,1,0,1, 1,0,1,0,1,0,1, 1
};


void midiInReset(midiInDataT *themidi) {
	themidi->recByte = 0x00;
	themidi->nextIsNote = 0x00;
	themidi->nextIsSpeed = 0x00;
	themidi->isHit = 0x00;
	themidi->lastCmd = 0x90;
	themidi->lastNote = 0x00;
	themidi->storedNote = 0x00;
}


void midiInProcess(midiInDataT *themidi) {
	uint16_t midiPending = 0;

	if (!(PEEK(MIDI_CTRL) & 0x02)) { // rx not empty
		midiPending = PEEKW(MIDI_RXD) & 0x0FFF;
		uint16_t i;
		for (i = 0; i < midiPending; i++) {
			themidi->recByte = PEEK(MIDI_FIFO);
			if (themidi->recByte == 0xfe) continue; // active sense, ignored

			if (themidi->nextIsSpeed) {
				// Third byte: velocity
				themidi->nextIsSpeed = false;
				dispatchNote(themidi->isHit, 0, themidi->storedNote, 0x7F, false, 0, false, 0);
				if (themidi->isHit == false) {
					// note ended
				}
				themidi->lastNote = themidi->storedNote;
			} else if (themidi->nextIsNote) {
				// Second byte: note number
				uint8_t noteColorIndex = themidi->recByte - 0x14;

				if (themidi->isHit) {
					graphicsDefineColor(0, noteColorIndex, 0xFF, 0x00, 0xFF);
				} else {
					uint8_t detectedColor = midiInNoteColors[noteColorIndex - 1] ? 0xFF : 0x00;
					graphicsDefineColor(0, noteColorIndex, detectedColor, detectedColor, detectedColor);
				}

				themidi->nextIsNote = false;
				themidi->storedNote = themidi->recByte;
				themidi->nextIsSpeed = true;
			} else {
				// First byte: command
				uint8_t cmd = themidi->recByte & 0xF0;
				if (cmd == 0x90) {
					themidi->nextIsNote = true;
					themidi->isHit = true;
					themidi->lastCmd = themidi->recByte;
				} else if (cmd == 0x80) {
					themidi->nextIsNote = true;
					themidi->isHit = false;
					themidi->lastCmd = themidi->recByte;
				} else if (themidi->recByte < 0x80) {
					// Running status: data byte without command prefix
					themidi->storedNote = themidi->recByte;
					themidi->nextIsNote = false;
					themidi->nextIsSpeed = true;
					uint8_t lastCmdType = themidi->lastCmd & 0xF0;
					if (lastCmdType == 0x90) {
						themidi->isHit = true;
					} else if (lastCmdType == 0x80) {
						themidi->isHit = true;
					}
				}
			}
		}
	}
}


#endif
