// 4270 DistanceSqrt - Integer square root distance
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <string.h>

static unsigned usqrt(unsigned n)
{
	unsigned x = n;
	unsigned y = 1;
	while (x > y)
	{
		x = (x + y) / 2;
		y = n / x;
	}
	return x;
}

static const char SpriteImage[64] = {
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xfc, 0x7e, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
};

static const int CenterX = 148;  // 160 - 12 (screen center - half sprite)
static const int CenterY = 108;  // 120 - 12

int main(int argc, char *argv[])
{
	textClear();
	sprite_init();

	unsigned char img = sprite_expand_c64(SpriteImage, 0, 1);
	sprite_set(0, true, CenterX, CenterY, 0, 1);
	sprite_set(1, true, CenterX, CenterY, 0, 0);

	int sx = 0, sy = 0;
	for (;;)
	{
		char ch = keyboardGetCharAsync();
		if (ch == KEY_LEFT) sx--;
		if (ch == KEY_RIGHT) sx++;
		if (ch == KEY_UP) sy--;
		if (ch == KEY_DOWN) sy++;
		sprite_move(0, CenterX + sx, CenterY + sy);

		graphicsSetBorderC64Color(2);
		int d = usqrt(sx * sx + sy * sy);
		graphicsSetBorderC64Color(0);

		textGotoXY(0, 0);
		textPrintInt(sx);
		textPrint(", ");
		textPrintInt(sy);
		textPrint(", ");
		textPrintInt(d);
		textPrint("    ");

		graphicsWaitVerticalBlank();
	}

	return 0;
}
