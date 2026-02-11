// vgmplayer2 - VGM player with OPL3, MIDI-in, LCD support
// Ported to oscar64 from F256KsimpleCdoodles
// NOTE: This is the most complex doodle - requires many mu0nlib modules

#include "f256lib.h"
// TODO: Port these mu0nlib modules to shared:
// - muopl3.h (opl3_initialize, opl3_write, opl3_quietAll, opl3_set4OPS, etc.)
// - muvgmplay.h (playback, load_VGM_file, checkVGMHeader, etc.)
// - f_filepicker (filePickModal_far, getTheFile_far, initFPR, fpr_set_currentPath)
// - muTimer0Int.h (timer0 interrupt routines)
// - mulcd.h (displayImage for K2 LCD)
// - f_midiin (midiInDataT, midiInReset, midiInProcess)
// - muMidi.h (VS1053b MIDI functions)
// - mudispatch.h (dispatch/channel management)
// - textUI.h (textUI, instructions, axes_info, show_inst, show_all_inst, etc.)

#include <stdio.h>
#include <stdlib.h>

#define LCDBIN 0x40000

// TODO: Replace EMBED with oscar64 #pragma section/#embed for binary assets:
// EMBED(pianopal, "../assets/piano.pal", 0x10000);
// EMBED(keys, "../assets/piano.raw", 0x10400);
// EMBED(lcdimg, "../assets/opl3snoop.bin", 0x40000);

void setup(void);

void setup()
{
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00001111);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
	POKE(VKY_LAYER_CTRL_0, 0b00010000);
	POKE(0xD00D,0x00);
	POKE(0xD00E,0x00);
	POKE(0xD00F,0x00);

	POKE(MMU_IO_CTRL,1);
	for(uint16_t c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(0x10000+c));
	}
	POKE(MMU_IO_CTRL,0);

	bitmapSetAddress(0,0x10400);
	bitmapSetVisible(0,false);
	// TODO: initFPR();

	POKE(MMU_IO_CTRL,0);
	// TODO: midiInReset(&gMID);
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// TODO: This program requires many mu0nlib modules:
	// muopl3, muvgmplay, muFilePicker, muTimer0Int, mulcd,
	// muMIDIin, muMidi, mudispatch, textUI
	//
	// The setup() and display code is ported above.
	// Once the shared modules are ported, the full main loop
	// (file picking, VGM playback, instrument snooping, LCD display)
	// can be enabled.

	setup();

	printf("vgmplayer2 requires many mu0nlib modules:\n");
	printf("muopl3, muvgmplay, muFilePicker, muTimer0Int,\n");
	printf("mulcd, muMIDIin, muMidi, mudispatch, textUI\n");
	printf("These shared modules are not yet ported to oscar64.\n");
	kernelWaitKey();

	return 0;
}
