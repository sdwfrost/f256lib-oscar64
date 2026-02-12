// 0110 ColorMem - Direct color memory access
// Ported from OscarTutorials to F256K using f256lib
//
// Fills screen with character 'A' and cycles through colors.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	volatile byte *vram = (volatile byte *)0xc000;

	// Fill text matrix with 'A' (I/O page 2)
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (unsigned int i = 0; i < 80 * 30; i++)
		vram[i] = 'A';

	// Set each cell to a different color (I/O page 3)
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	for (unsigned int i = 0; i < 80 * 30; i++)
		vram[i] = (byte)(((i & 0x0F) << 4) | 0);  // cycling fg colors, black bg

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (true)
		;

	return 0;
}
