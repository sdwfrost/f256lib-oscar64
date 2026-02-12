// 1620 BigSpriteBlocking - Large sprite blocked by background
// New tutorial for F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <stdlib.h>

static const char spimage[63] = {
	0b11111111, 0b11111111, 0b11111111,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000001,
	0b11111111, 0b11111111, 0b11111111
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

bool blocked_h(int x, int y)
{
	return char_at_pix(x, y) >= 128 || char_at_pix(x + 8, y) >= 128 ||
	       char_at_pix(x + 16, y) >= 128 || char_at_pix(x + 23, y) >= 128;
}

bool blocked_v(int x, int y)
{
	return char_at_pix(x, y) >= 128 || char_at_pix(x, y + 8) >= 128 ||
	       char_at_pix(x, y + 16) >= 128 || char_at_pix(x, y + 20) >= 128;
}

int main(int argc, char *argv[])
{
	textClear();

	for (byte i = 0; i < 50; i++)
	{
		textGotoXY(rand() % 40, rand() % 30);
		textSetColor(LIGHT_BLUE, BLACK);
		textPutChar(160);
	}

	sprite_init();
	sprite_expand_c64(spimage, 0, 1);

	int spx = 140, spy = 100;
	sprite_set(0, true, spx, spy, 0, 1);

	for (;;)
	{
		char c = keyboardGetCharAsync();

		if (c == KEY_LEFT  && spx > 0 && !blocked_v(spx - 1, spy))
			spx--;
		if (c == KEY_RIGHT && spx < 296 && !blocked_v(spx + 24, spy))
			spx++;
		if (c == KEY_UP    && spy > 0 && !blocked_h(spx, spy - 1))
			spy--;
		if (c == KEY_DOWN  && spy < 219 && !blocked_h(spx, spy + 21))
			spy++;

		sprite_move(0, spx, spy);
		graphicsWaitVerticalBlank();
	}

	return 0;
}
