// 1610 SpriteBackBlocking - Background blocks sprite movement
// New tutorial for F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <stdlib.h>

static const char spimage[63] = {
	0b11111111, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b11111111, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
};

byte char_at_pix(int x, int y)
{
	if (x < 0 || y < 0) return 32;
	volatile byte *vram = (volatile byte *)0xc000;
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	byte c = vram[(unsigned int)(y >> 3) * 80 + (x >> 3)];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	return c;
}

bool is_blocked(int x, int y)
{
	return char_at_pix(x, y) >= 128;
}

int main(int argc, char *argv[])
{
	textClear();

	for (byte i = 0; i < 100; i++)
	{
		textGotoXY(rand() % 40, rand() % 30);
		textSetColor(LIGHT_BLUE, BLACK);
		textPutChar(160);
	}

	sprite_init();
	sprite_expand_c64(spimage, 0, 1);

	int spx = 160, spy = 120;
	sprite_set(0, true, spx, spy, 0, 1);

	for (;;)
	{
		char c = keyboardGetCharAsync();

		if (c == KEY_LEFT  && spx > 0 && !is_blocked(spx - 1, spy) && !is_blocked(spx - 1, spy + 7))
			spx--;
		if (c == KEY_RIGHT && spx < 312 && !is_blocked(spx + 8, spy) && !is_blocked(spx + 8, spy + 7))
			spx++;
		if (c == KEY_UP    && spy > 0 && !is_blocked(spx, spy - 1) && !is_blocked(spx + 7, spy - 1))
			spy--;
		if (c == KEY_DOWN  && spy < 232 && !is_blocked(spx, spy + 8) && !is_blocked(spx + 7, spy + 8))
			spy++;

		sprite_move(0, spx, spy);
		graphicsWaitVerticalBlank();
	}

	return 0;
}
