// 2010 SIDMusicInterrupt - VBlank-synced SID melody
// New tutorial for F256K using f256lib
//
// Plays a repeating melody synced to frame timing.

#include "f256lib.h"

// Simple melody: note index, duration in frames (0 = end)
const byte melody[] = {
	36, 15,  // C4
	38, 15,  // D4
	40, 15,  // E4
	36, 15,  // C4
	36, 15,  // C4
	38, 15,  // D4
	40, 15,  // E4
	36, 15,  // C4
	40, 15,  // E4
	41, 15,  // F4
	43, 30,  // G4
	40, 15,  // E4
	41, 15,  // F4
	43, 30,  // G4
	0,  0,   // end marker
};

int main(int argc, char *argv[])
{
	textClear();
	textPrint("SID MELODY WITH VBLANK SYNC\n");
	textPrint("PRESS ANY KEY TO STOP\n\n");

	sidClearRegisters();
	sidSetInstrumentAllChannels(2);  // Pulse wave
	byte ctrl = sidFetchCtrl(2);

	while (!keyboardHit())
	{
		const byte *p = melody;
		while (*p != 0 && !keyboardHit())
		{
			byte note = *p++;
			byte dur = *p++;

			// Set frequency
			POKE(SID1 + SID_VOICE1 + SID_LO_B, sidLow[note]);
			POKE(SID1 + SID_VOICE1 + SID_HI_B, sidHigh[note]);

			// Gate on
			sidNoteOnOrOff(SID1 + SID_VOICE1 + SID_CTRL, ctrl, true);

			// Hold for duration frames
			for (byte f = 0; f < dur; f++)
				graphicsWaitVerticalBlank();

			// Gate off
			sidNoteOnOrOff(SID1 + SID_VOICE1 + SID_CTRL, ctrl, false);

			// Small gap
			for (byte f = 0; f < 2; f++)
				graphicsWaitVerticalBlank();
		}
	}

	sidShutAllVoices();
	textPrint("STOPPED.\n");

	return 0;
}
