// 2000 SIDMusicPlayback - Play a C major scale on SID
// New tutorial for F256K using f256lib
//
// Demonstrates the f256lib SID API by playing notes using voice 1.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();
	textPrint("SID MUSIC PLAYBACK\n\n");
	textPrint("PLAYING C MAJOR SCALE...\n");

	// Clear SID registers and set up instrument
	sidClearRegisters();
	sidSetInstrumentAllChannels(0);  // Triangle wave

	// C major scale: C4, D4, E4, F4, G4, A4, B4, C5
	// SID note indices (from sidLow/sidHigh tables):
	// C4=36, D4=38, E4=40, F4=41, G4=43, A4=45, B4=47, C5=48
	const byte notes[] = { 36, 38, 40, 41, 43, 45, 47, 48 };
	const byte ctrl = sidFetchCtrl(0);

	for (byte i = 0; i < 8; i++)
	{
		// Set frequency for voice 1 on SID chip 0
		POKE(SID1 + SID_VOICE1 + SID_LO_B, sidLow[notes[i]]);
		POKE(SID1 + SID_VOICE1 + SID_HI_B, sidHigh[notes[i]]);

		// Gate on
		sidNoteOnOrOff(SID1 + SID_VOICE1 + SID_CTRL, ctrl, true);

		textPrint("NOTE ");
		textPrintInt(notes[i]);
		textPutChar('\n');

		// Hold note for ~30 frames
		graphicsPause(30);

		// Gate off
		sidNoteOnOrOff(SID1 + SID_VOICE1 + SID_CTRL, ctrl, false);

		// Brief pause between notes
		graphicsPause(5);
	}

	textPrint("\nDONE.\n");
	sidShutAllVoices();

	return 0;
}
