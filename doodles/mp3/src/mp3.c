// mp3 - Simple MP3 player for F256K2/Jr2
// Ported to oscar64 from F256KsimpleCdoodles
// NOTE: Requires VS1053b functions (openAllCODEC, boostVSClock) from muMidi/muVS1053b

#include "f256lib.h"
// f256lib provides: vs1053bBoostClock via f_vs1053b module
// For now, using f256lib MIDI module which may provide these

#define VS_SCI_CTRL   0xD700
#define    CTRL_Start   0x01
#define    CTRL_RWn     0x02
#define    CTRL_Busy    0x08

#define VS_SCI_ADDR  0xD701
#define VS_SCI_DATA  0xD702
#define VS_FIFO_STAT 0xD704
#define VS_FIFO_DATA 0xD707

#define CHUNK8K 0x2000
#define CHUNK4K 0x1000
#define CHUNK2K 0x0800
#define CHUNK1K 0x0400
#define CHUNK128B 0x80
#define CHUNK64B 0x40
#define CHUNK32B 0x20

// TODO: Replace EMBED with oscar64 #pragma section/#embed for binary assets:
// EMBED(backg, "../assets/rj.raw.pal", 0x10000);
// EMBED(palback, "../assets/rj.raw.bin", 0x10400);

void read8KChunk(void *, FILE *);
uint8_t openMP3File(const char *);
uint32_t totalsize = 0;

FILE *theMP3file;

uint8_t openMP3File(const char *name)
{
	uint32_t garbage;

	printf("Opening file: %s\n",name);
	theMP3file = fopen(name,"rb");
	if(theMP3file == NULL)
	{
		printf("Couldn't open the file: %s\n",name);
		return 1;
	}
	totalsize = 0;
	while(fread(&garbage,sizeof(uint8_t),128,theMP3file) == 128)
		totalsize += 128;

	rewind((FILE *)theMP3file);

	return 0;
}

void read8KChunk(void *buf, FILE *f)
{
	uint16_t i;
	for(i=0;i<64;i++)
		{
		fread((void *)((uint8_t *)buf+i*0x80), sizeof(uint8_t), 128, f);
		}
}

void backgroundSetup()
{
	uint16_t c=0;

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00101111);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
	POKE(VKY_LAYER_CTRL_0, 0b00000001);
	POKE(VKY_LAYER_CTRL_1, 0b00000010);
	POKE(0xD00D,0x00);
	POKE(0xD00E,0x00);
	POKE(0xD00F,0x00);

	POKE(MMU_IO_CTRL,1);
	for(c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(0x10000+c));
	}
	POKE(MMU_IO_CTRL,0);

	bitmapSetActive(0);
	bitmapSetAddress(0,0x10400);
	bitmapSetCLUT(0);
	bitmapSetVisible(0,true);
	bitmapSetVisible(1,false);
	bitmapSetVisible(2,false);
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	uint16_t i=0,j=0;
	uint32_t fileIndex=0;
	uint16_t bufferIndex=0;
	uint16_t readEntryBufferIndex = 0;
	uint16_t rawFIFOCount=0;
	uint16_t bytesToTopOff=0;
	uint16_t multipleOf64b = 0;

	static char buffer[CHUNK8K];

	// TODO: openAllCODEC() and vs1053bBoostClock() from f_vs1053b
	// openAllCODEC();
	// vs1053bBoostClock();
	backgroundSetup();

	openMP3File("mp3/ronald.mp3");
	read8KChunk((void *)buffer, theMP3file);
	fileIndex+=CHUNK8K;

	printf("Hit space to start playback on a F256K2 or a F256Jr2");
	kernelWaitKey();

	printf("\nPlayback launched for 'Try the Bass' from Ronald Jenkees, %lu bytes",totalsize);

	for(i=bufferIndex;i<bufferIndex+CHUNK2K;i++)
		{
		POKE(VS_FIFO_DATA, buffer[i]);
		}
	bufferIndex+=CHUNK2K;
	for(i=bufferIndex;i<bufferIndex+CHUNK2K;i++)
		{
		POKE(VS_FIFO_DATA, buffer[i]);
		}
	bufferIndex+=CHUNK2K;

	printf("\nStreaming mp3 data to the VS1053b onboard chip.");
	printf("\n\nBuffer in 64b chunks:");
	while(fileIndex<totalsize)
	{
		rawFIFOCount = PEEKW(VS_FIFO_STAT);
		bytesToTopOff = CHUNK2K - (rawFIFOCount&0x0FFF);
		multipleOf64b = bytesToTopOff>>6;
		textGotoXY(0,7);textPrintInt(32-multipleOf64b);textPrint(" ");

		for(i=0; i<multipleOf64b; i++)
		{
			for(j=0; j<CHUNK64B; j++)
			{
			POKE(VS_FIFO_DATA, buffer[bufferIndex+j]);
			}
			bufferIndex+=CHUNK64B;
			if(bufferIndex == CHUNK8K) bufferIndex = 0;

			fread((void *)(buffer+readEntryBufferIndex), sizeof(uint8_t), 64, theMP3file);
			readEntryBufferIndex+=CHUNK64B;
			if(readEntryBufferIndex== CHUNK8K) readEntryBufferIndex = 0;
		}
	}

	fclose(theMP3file);
	printf("\nPlayback ended. Press space to quit");
	kernelWaitKey();
	return 0;
}
