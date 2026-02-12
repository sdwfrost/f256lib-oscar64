// 4230 AtanCordicByte - 8-bit CORDIC inverse tangent
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
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

static const char arortab[6] = {32, 19, 10, 5, 3, 1};

char matanu(char dx, signed char dy)
{
	signed char sum = 0;
	if (dx & 0x80) { dx >>= 1; dy >>= 1; }

	char sx = dx;
	signed char sy = dy;

	if (dy > 0) { dx += sy; dy -= sx; sum = arortab[0]; }
	else if (dy < 0) { dx -= sy; dy += sx; sum = -arortab[0]; }

	for (char i = 1; i < 6; i++)
	{
		char sx2 = dx >> i;
		signed char sy2 = dy >> i;

		if (dy > 0) { dx += sy2; dy -= sx2; sum += arortab[i]; }
		else if (dy < 0) { dx -= sy2; dy += sx2; sum -= arortab[i]; }
	}

	return sum;
}

char matan(int dx, signed char dy)
{
	if (dx < 0) return 128 - matanu(-dx, dy);
	else return matanu(dx, dy);
}

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
