// 4280 DistanceTable - Table-based distance calculation
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
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

static char dtab[16][16];

int mdist(int dx, int dy)
{
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;

	unsigned ux = dx, uy = dy;
	char s = 0;

	if ((ux | uy) & 0xf000) { ux >>= 8; uy >>= 8; s += 8; }
	if ((ux | uy) & 0xff00) { ux >>= 4; uy >>= 4; s += 4; }

	char bx = ux, by = uy;
	while ((bx | by) & 0xf0) { bx >>= 1; by >>= 1; s++; }

	return dtab[by][bx] << s;
}

int main(int argc, char *argv[])
{
	// Init lookup table
	for (char y = 0; y < 16; y++)
		for (char x = 0; x < 16; x++)
			dtab[y][x] = usqrt(x * x + y * y);

	textClear();
	spriteInit();

	unsigned char img = spriteExpand(SpriteImage, 0, 1);
	spriteSet(0, true, CenterX, CenterY, 0, 1);
	spriteSet(1, true, CenterX, CenterY, 0, 0);

	int sx = 0, sy = 0;
	for (;;)
	{
		char ch = keyboardGetCharAsync();
		if (ch == KEY_LEFT) sx--;
		if (ch == KEY_RIGHT) sx++;
		if (ch == KEY_UP) sy--;
		if (ch == KEY_DOWN) sy++;
		spriteMove(0, CenterX + sx, CenterY + sy);

		graphicsSetBorderC64Color(2);
		int d = mdist(sx, sy);
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
