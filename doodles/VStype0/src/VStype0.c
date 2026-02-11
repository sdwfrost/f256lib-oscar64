// VStype0 - VS1053b Type 0 MIDI file playback test
// Ported to oscar64 from F256KsimpleCdoodles

#include "f256lib.h"

// VS1053b registers
#define VS_SCI_CTRL   0xD700
#define VS_SCI_ADDR   0xD701
#define VS_SCI_DATA   0xD702
#define VS_FIFO_STAT  0xD704
#define VS_FIFO_DATA  0xD707

// TODO: Replace EMBED with oscar64 #pragma section/#embed for binary asset:
// EMBED(human2, "../assets/human2.mid", 0x10000);
uint32_t fileSize = 33876;

// TODO: These functions come from muMidi.h / muVS1053b.h
// boostVSClock() - increase VS1053b clock speed
// initVS1053MIDI() - apply the real-time MIDI plugin

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	uint16_t i=0, j=0;
	uint16_t howManySoFar=0;
	uint8_t pass = 0;

	// TODO: Uncomment when f256lib MIDI module provides these:
	// boostVSClock();
	// initVS1053MIDI();

	// Check the first 4 bytes of the embedded MIDI file
	for(i=0;i<4;i++)
		printf("%02x",FAR_PEEK((uint32_t)0x10000+(uint32_t)i));

	printf("\n%04x bytes at start\n",PEEKW(VS_FIFO_STAT)&0x03FF);

	i=0;
	while(i<fileSize)
	{
		pass++;
		for(i=j;i<j+0x0800;i++)
			{
			if(howManySoFar + i > fileSize) POKE(VS_FIFO_DATA, 0x00);
			else POKE(VS_FIFO_DATA, FAR_PEEK((uint32_t)0x10000+(uint32_t)i));
			}
		j+=0x0800;
		howManySoFar+=0x0800;
		printf("%04x bytes after pass %d\n",PEEKW(VS_FIFO_STAT)&0x03FF,pass);
		while((PEEKW(VS_FIFO_STAT)& 0x8000) == 0);
	}

	while(true)
	{
	}
	return 0;
}
