// 4290 DistanceCordic - CORDIC distance calculation
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <string.h>

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

int mdist(int dx, int dy)
{
	char ux = dx < 0 ? -dx : dx;
	signed char uy = dy;

	#pragma unroll(full)
	for (char i = 0; i < 4; i++)
	{
		char sx = ux >> i;
		signed char sy = uy >> i;

		if (uy > 0) { ux += sy; uy -= sx; }
		else { ux -= sy; uy += sx; }
	}

	return (ux + (ux >> 2) - (ux >> 6)) >> 1;
}

int main(int argc, char *argv[])
{
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
