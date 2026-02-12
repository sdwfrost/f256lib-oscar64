// 0100 ScreenMem - Direct screen memory access
// Ported from OscarTutorials to F256K using f256lib
//
// Fills screen memory with all 256 characters.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	volatile byte *vram = (volatile byte *)TEXT_MATRIX;

	// Fill screen memory with all 256 characters
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (int i = 0; i < 256; i++)
		vram[i] = i;

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (true)
		;

	return 0;
}
