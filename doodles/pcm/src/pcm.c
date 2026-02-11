/*
 * PCM doodle.
 * Ported from F256KsimpleCdoodles for oscar64.
 */

#include "f256lib.h"

// TODO: Asset loading - original used EMBED(dash, "../assets/dash.wav", 0x10000)
// For oscar64, assets need to be loaded at runtime or via #embed pragma

int main(int argc, char *argv[]) {
	uint16_t i=0;
	//openAllCODEC(); //if the VS1053b is used, this might be necessary for some board revisions
	vs1053bBoostClock();
	vs1053bInitBigPatch();

	for(i=0;i<0xADDC;i++) POKE(VS_FIFO_DATA,FAR_PEEK(0x10000+i));

	return 0;
}
