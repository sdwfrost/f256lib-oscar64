// 1600 SpriteBackCollision - Sprite vs background collision
// New tutorial for F256K using f256lib
//
// Move a sprite with arrow keys. Detects collision with background chars.

#include "f256lib.h"
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

// Read character from screen VRAM at text column/row
byte char_at(byte cx, byte cy)
{
	volatile byte *vram = (volatile byte *)0xc000;
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	byte c = vram[(unsigned int)cy * 80 + cx];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	return c;
}

// Get character at pixel position
byte char_at_pix(int x, int y)
{
	if (x < 0 || y < 0) return 32;
	return char_at((byte)(x >> 3), (byte)(y >> 3));
}

int main(int argc, char *argv[])
{
	textClear();

	// Place random obstacles
	for (byte i = 0; i < 100; i++)
	{
		byte cx = rand() % 40;
		byte cy = rand() % 30;
		textGotoXY(cx, cy);
		textSetColor(LIGHT_BLUE, BLACK);
		textPutChar(160);  // filled block char
	}

	spriteInit();
	spriteExpand(spimage, 0, 1);  // white

	int spx = 160, spy = 120;
	spriteSet(0, true, spx, spy, 0, 1);

	for (;;)
	{
		char c = keyboardGetCharAsync();
		if (c == KEY_LEFT  && spx > 0)   spx--;
		if (c == KEY_RIGHT && spx < 312) spx++;
		if (c == KEY_UP    && spy > 0)   spy--;
		if (c == KEY_DOWN  && spy < 232) spy++;

		spriteMove(0, spx, spy);

		// Check four corners of 8x8 sprite area
		byte tl = char_at_pix(spx, spy);
		byte tr = char_at_pix(spx + 7, spy);
		byte bl = char_at_pix(spx, spy + 7);
		byte br = char_at_pix(spx + 7, spy + 7);

		byte collided = (tl | tr | bl | br) >= 128;
		spriteRecolor(0, spimage, collided ? 2 : 1);  // red on collision, white otherwise

		graphicsWaitVerticalBlank();
	}

	return 0;
}
