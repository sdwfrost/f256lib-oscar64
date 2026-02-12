// 1200 ScrollUp - Scroll screen upward
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdlib.h>

static void scroll_up(void)
{
	volatile byte *vram = (volatile byte *)0xc000;
	unsigned int total = 80u * 30u;
	unsigned int copy_count = total - 80;

	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (unsigned int i = 0; i < copy_count; i++)
		vram[i] = vram[i + 80];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
}

static void fill_line(void)
{
	volatile byte *vram = (volatile byte *)0xc000;
	unsigned int offset = 80u * 29;

	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (byte i = 0; i < 80; i++)
		vram[offset + i] = (rand() & 1) + 'M';
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
}

int main(int argc, char *argv[])
{
	for (;;)
	{
		graphicsSetBorderC64Color(2);
		graphicsWaitVerticalBlank();

		graphicsSetBorderC64Color(0);
		scroll_up();

		graphicsSetBorderC64Color(1);
		fill_line();
	}

	return 0;
}
