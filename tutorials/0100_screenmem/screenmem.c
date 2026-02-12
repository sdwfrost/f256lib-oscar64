// 0100 ScreenMem - Direct screen memory access
// Ported from OscarTutorials to F256K using f256lib
//
// Fills screen with random characters via I/O page 2 (text matrix).

#include "f256lib.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
	volatile byte *vram = (volatile byte *)0xc000;

	// Write random characters to text matrix (I/O page 2)
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (unsigned int i = 0; i < 80 * 30; i++)
		vram[i] = (byte)(rand() & 0xFF);

	// Set all colors to white on black (I/O page 3)
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	for (unsigned int i = 0; i < 80 * 30; i++)
		vram[i] = 0xF0;  // white fg, black bg

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (true)
		;

	return 0;
}
