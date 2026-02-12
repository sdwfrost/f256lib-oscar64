// 4210 AtanTable - Table-based inverse tangent
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <string.h>

#define PI 3.14159265

static float fast_atan2(float y, float x)
{
	float abs_x = x < 0 ? -x : x;
	float abs_y = y < 0 ? -y : y;
	float min_val = abs_x < abs_y ? abs_x : abs_y;
	float max_val = abs_x > abs_y ? abs_x : abs_y;
	float a = max_val > 0 ? min_val / max_val : 0;
	float s = a * a;
	float r = ((-0.0464964749 * s + 0.15931422) * s - 0.327622764) * s * a + a;
	if (abs_y > abs_x) r = 1.57079637 - r;
	if (x < 0) r = 3.14159265 - r;
	if (y < 0) r = -r;
	return r;
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

static char atab[16][16];

unsigned uatan(unsigned dx, unsigned dy)
{
	if ((dx | dy) & 0xf000) { dx >>= 8; dy >>= 8; }
	if ((dx | dy) & 0xff00) { dx >>= 4; dy >>= 4; }
	char ux = dx, uy = dy;
	while ((ux | uy) & 0xf0) { ux >>= 1; uy >>= 1; }
	return atab[uy][ux];
}

int matan(int dx, int dy)
{
	if (dx < 0)
	{
		if (dy < 0) return 0x80 + uatan(-dx, -dy);
		else return 0x80 - uatan(-dx, dy);
	}
	else if (dy < 0) return 0x100 - uatan(dx, -dy);
	else return uatan(dx, dy);
}

int main(int argc, char *argv[])
{
	// Init lookup table
	for (char y = 0; y < 16; y++)
		for (char x = 0; x < 16; x++)
			atab[y][x] = (char)(fast_atan2(y, x) * 128 / PI + 0.5);

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
		int ai = matan(sx, sy);
		graphicsSetBorderC64Color(0);

		int af = (int)(fast_atan2(sy, sx) * 128 / PI) & 0xff;

		textGotoXY(0, 0);
		textPrintInt(sx);
		textPrint(", ");
		textPrintInt(sy);
		textPrint(", ");
		textPrintInt(ai);
		textPrint(" : ");
		textPrintInt(af);
		textPrint("    ");

		graphicsWaitVerticalBlank();
	}

	return 0;
}
