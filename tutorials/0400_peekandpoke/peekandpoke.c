// 0400 PeekAndPoke - Direct memory access with PEEK/POKE
// Ported from OscarTutorials to F256K using f256lib
//
// Demonstrates writing to screen via direct VRAM access.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	volatile byte *vram = (volatile byte *)0xc000;

	// Write "HELLO" directly to screen memory (I/O page 2)
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	vram[0] = 'H';
	vram[1] = 'E';
	vram[2] = 'L';
	vram[3] = 'L';
	vram[4] = 'O';

	// Color them yellow on black (I/O page 3)
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	for (byte i = 0; i < 5; i++)
		vram[i] = (YELLOW << 4) | BLACK;

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Read back and display via textPrint
	textGotoXY(0, 2);
	textPrint("Written directly to VRAM!\n");

	// Show the PEEK value
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	byte ch = vram[0];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	textPrint("PEEK of first char: ");
	textPrintHex(ch, 2);
	textPrint(" = '");
	textPutChar(ch);
	textPrint("'\n");

	return 0;
}
