// 4260 CosinCordic - CORDIC sin/cos circle motion
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

static const int arortab[8] = {8192, 4836, 2555, 1297, 651, 326, 163, 81};

void cosincc(int w, signed char *si, signed char *co)
{
	int dx = 9945;
	int dy = 0;

	if (w > 16384 || w < -16384) { w ^= 0x8000; dx = -dx; }

	for (char i = 0; i < 8; i++)
	{
		int sx = dx >> i;
		int sy = dy >> i;

		if (w > 0) { dx += sy; dy -= sx; w -= arortab[i]; }
		else { dx -= sy; dy += sx; w += arortab[i]; }
	}

	*si = dy >> 8;
	*co = dx >> 8;
}

int main(int argc, char *argv[])
{
	textClear();
	spriteInit();

	unsigned char img = spriteExpand(SpriteImage, 0, 1);
	spriteSet(0, true, CenterX, CenterY, 0, 1);
	spriteSet(1, true, CenterX, CenterY, 0, 0);

	char w = 0;
	for (;;)
	{
		signed char sx, sy;

		graphicsSetBorderC64Color(2);
		cosincc(w << 8, &sx, &sy);
		graphicsSetBorderC64Color(0);

		spriteMove(0, CenterX + sx, CenterY + sy);
		w++;

		graphicsWaitVerticalBlank();
	}

	return 0;
}
