/*
 * OPL3 more doodle.
 * Ported from F256KsimpleCdoodles for oscar64.
 * Note: original file was named opl3.c in the opl3more directory.
 */

#include "f256lib.h"
// TODO: muopl3.h not yet ported to oscar64 - needs OPL3 module
// Original used: #include "../src/muopl3.h"

void play_sweep() {
    uint8_t channel = 0, i,j; // Channel 0
	for(j=0;j<8;j++) //across all octaves, 'blocks 0 to 7'
		{
		for(i=0;i<12;i++) //across all 12 tones of an octave
			{
			opl3Note(channel, opl3Fnums[i], j, true);
			kernelPause(2);
			opl3Note(channel, opl3Fnums[i], j, false);
			}
		}
	printf("-=- ALERT! -=-\n");
}

int main(int argc, char *argv[]) {
	opl3Initialize();
	while(true) play_sweep();
	return 0;
}
