// 0110 ColorMem - Direct color memory access
// Ported from OscarTutorials to F256K using f256lib
//
// Fills screen with spaces and sets vertical colour bars via color memory.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	volatile byte *vram = (volatile byte *)TEXT_MATRIX;
	unsigned int i;
	byte x;

	// Enable per-cell background colours
	textEnableBackgroundColors(true);

	// Fill screen with spaces
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (i = 0; i < 80 * 30; i++)
		vram[i] = ' ';

	// Vertical colour bars (fg and bg set to same colour = solid cell)
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	x = 0;
	for (i = 0; i < 80 * 30; i++)
	{
		byte color = x / 5;
		vram[i] = (color << 4) | color;
		x++;
		if (x == 80) x = 0;
	}

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (true)
		;

	return 0;
}
