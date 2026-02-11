/*
 *	VS1053b MIDI test.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Uses f256lib's MIDI module.
 */

#include "f256lib.h"


void boostVSClock(void) {
	POKE(VS_SCI_ADDR, 0x03);
	POKEW(VS_SCI_DATA, 0x9800);
	POKE(VS_SCI_CTRL, 1);
	POKE(VS_SCI_CTRL, 0);
	while (PEEK(VS_SCI_CTRL) & 0x80);
}

int main(int argc, char *argv[]) {
	bool wv = true;

	boostVSClock();
	initVS1053MIDI();

	midiNoteOn(0, 0x4b, 0x3F, wv);
	kernelWaitKey();
	midiNoteOff(0, 0x4b, 0x3F, wv);
	midiNoteOn(0, 0x4d, 0x3F, wv);
	kernelWaitKey();
	midiNoteOff(0, 0x4d, 0x3F, wv);
	midiNoteOn(0, 0x4f, 0x3F, wv);
	kernelWaitKey();
	midiNoteOff(0, 0x4f, 0x3F, wv);
	midiNoteOn(0, 0x50, 0x3F, wv);
	kernelWaitKey();
	midiNoteOff(0, 0x50, 0x3F, wv);
	midiNoteOn(0, 0x52, 0x3F, wv);
	kernelWaitKey();
	midiNoteOff(0, 0x52, 0x3F, wv);

	return 0;
}
