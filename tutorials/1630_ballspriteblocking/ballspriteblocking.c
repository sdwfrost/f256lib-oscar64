// 1630 BallSpriteBlocking - Circular sprite with background blocking
// New tutorial for F256K using f256lib

#include "f256lib.h"
#include <stdlib.h>

const char SpriteImage[64] = {
	0b00000000, 0b11111000, 0b00000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00011111, 0b11111111, 0b11000000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b00011111, 0b11111111, 0b11000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00000000, 0b11111000, 0b00000000
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

bool spr_check_char(int sx, int sy, int x, int y)
{
	if (char_at_pix(x, y) >= 128) {
		x &= ~7; y &= ~7;
		signed char dx = x - sx;
		signed char dy = y - sy;
		if (dx < -3) dx += 7;
		if (dy < -3) dy += 7;
		if (dx * dx + dy * dy < 121)
			return true;
	}
	return false;
}

bool is_blocked(int x, int y)
{
	return
		spr_check_char(x+10, y+10, x, y) || spr_check_char(x+10, y+10, x+8, y) ||
		spr_check_char(x+10, y+10, x+16, y) || spr_check_char(x+10, y+10, x+20, y) ||
		spr_check_char(x+10, y+10, x, y+8) || spr_check_char(x+10, y+10, x+20, y+8) ||
		spr_check_char(x+10, y+10, x, y+16) || spr_check_char(x+10, y+10, x+20, y+16) ||
		spr_check_char(x+10, y+10, x, y+20) || spr_check_char(x+10, y+10, x+8, y+20) ||
		spr_check_char(x+10, y+10, x+16, y+20) || spr_check_char(x+10, y+10, x+20, y+20);
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

	spriteInit();
	spriteExpand(SpriteImage, 0, 1);

	int spx = 140, spy = 100;
	spriteSet(0, true, spx, spy, 0, 1);

	for (;;)
	{
		char c = keyboardGetCharAsync();

		if (c == KEY_LEFT  && spx > 0 && !is_blocked(spx - 1, spy))
			spx--;
		if (c == KEY_RIGHT && spx < 296 && !is_blocked(spx + 1, spy))
			spx++;
		if (c == KEY_UP    && spy > 0 && !is_blocked(spx, spy - 1))
			spy--;
		if (c == KEY_DOWN  && spy < 219 && !is_blocked(spx, spy + 1))
			spy++;

		spriteMove(0, spx, spy);
		graphicsWaitVerticalBlank();
	}

	return 0;
}
