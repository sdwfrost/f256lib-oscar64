// 1100 CharRom - Read and display character ROM data
// Ported from OscarTutorials to F256K using f256lib
//
// Reads font memory (I/O page 1) and prints each char's bit pattern.

#include "f256lib.h"

void expand8(byte c, char *s)
{
	for (byte i = 0; i < 8; i++)
	{
		s[i] = (c & 0x80) ? '#' : '.';
		c <<= 1;
	}
	s[8] = 0;
}

int main(int argc, char *argv[])
{
	volatile byte *font = (volatile byte *)0xc000;

	for (unsigned int i = 0; i < 2048; i++)
	{
		char exs[9];

		// Read font byte (I/O page 1)
		POKE(MMU_IO_CTRL, MMU_IO_PAGE_1);
		byte c = font[i];
		POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

		expand8(c, exs);

		textPrintHex(i, 3);
		textPrint(" : ");
		textPrintHex(c, 2);
		textPrint(" - ");
		textPrint(exs);
		textPutChar('\n');
	}

	return 0;
}
