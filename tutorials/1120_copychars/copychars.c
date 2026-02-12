// 1120 CopyChars - Copy font and add custom characters
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

static const byte Smiley[] = {
	0b01111100,
	0b11111110,
	0b10010010,
	0b11111110,
	0b10000010,
	0b11000110,
	0b01111100,
	0b00000000,

	0b01111100,
	0b11111110,
	0b10010010,
	0b11111110,
	0b11000110,
	0b10111010,
	0b01111100,
	0b00000000,
};

int main(int argc, char *argv[])
{
	volatile byte *mem = (volatile byte *)0xc000;

	// Copy custom chars into font at position 128 (I/O page 1)
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_1);
	for (byte i = 0; i < 16; i++)
		mem[128u * 8 + i] = Smiley[i];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	unsigned int total = 80u * 30u;

	// Fill screen with cycling chars 0-255 (I/O page 2)
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (unsigned int i = 0; i < total; i++)
		mem[i] = (byte)i;

	// Color all cells yellow on black (I/O page 3)
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	for (unsigned int i = 0; i < total; i++)
		mem[i] = (YELLOW << 4) | BLACK;

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	graphicsSetBackgroundC64Color(0);
	graphicsSetBorderC64Color(0);

	while (true)
		;

	return 0;
}
