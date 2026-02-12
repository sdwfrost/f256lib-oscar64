// 4240 CosinFloat - Float sin/cos circle motion
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
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

// Pre-computed: sintab[i] = round(64 * sin(i * PI / 128)) for i=0..255
static const signed char sintab[256] = {
	  0,   2,   3,   5,   6,   8,   9,  11,  12,  14,  16,  17,  19,  20,  22,  23,
	 25,  26,  28,  29,  31,  32,  33,  35,  36,  38,  39,  40,  42,  43,  44,  45,
	 47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  59,  60,  61,
	 61,  62,  62,  63,  63,  63,  64,  64,  64,  64,  64,  64,  64,  63,  63,  63,
	 62,  62,  61,  61,  60,  59,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,
	 49,  48,  47,  45,  44,  43,  42,  40,  39,  38,  36,  35,  33,  32,  31,  29,
	 28,  26,  25,  23,  22,  20,  19,  17,  16,  14,  12,  11,   9,   8,   6,   5,
	  3,   2,   0,  -2,  -3,  -5,  -6,  -8,  -9, -11, -12, -14, -16, -17, -19, -20,
	-22, -23, -25, -26, -28, -29, -31, -32, -33, -35, -36, -38, -39, -40, -42, -43,
	-44, -45, -47, -48, -49, -50, -51, -52, -53, -54, -55, -56, -57, -58, -59, -59,
	-60, -61, -61, -62, -62, -63, -63, -63, -64, -64, -64, -64, -64, -64, -64, -63,
	-63, -63, -62, -62, -61, -61, -60, -59, -59, -58, -57, -56, -55, -54, -53, -52,
	-51, -50, -49, -48, -47, -45, -44, -43, -42, -40, -39, -38, -36, -35, -33, -32,
	-31, -29, -28, -26, -25, -23, -22, -20, -19, -17, -16, -14, -12, -11,  -9,  -8,
	 -6,  -5,  -3,  -2,   0,   2,   3,   5,   6,   8,   9,  11,  12,  14,  16,  17,
	 19,  20,  22,  23,  25,  26,  28,  29,  31,  32,  33,  35,  36,  38,  39,  40
};

int main(int argc, char *argv[])
{
	textClear();
	sprite_init();

	unsigned char img = sprite_expand_c64(SpriteImage, 0, 1);
	sprite_set(0, true, CenterX, CenterY, 0, 1);
	sprite_set(1, true, CenterX, CenterY, 0, 0);

	char w = 0;
	for (;;)
	{
		graphicsSetBorderC64Color(2);
		int sx = sintab[((unsigned char)w + 64) & 0xff];
		int sy = sintab[(unsigned char)w];
		graphicsSetBorderC64Color(0);

		sprite_move(0, CenterX + sx, CenterY + sy);
		w++;

		graphicsWaitVerticalBlank();
	}

	return 0;
}
