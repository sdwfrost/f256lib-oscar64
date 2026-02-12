// 4200 AtanFloat - Floating-point atan2
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include "sprite_util.h"
#include <string.h>
#include <stdio.h>

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
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0xfc, 0x7e, 0x00,
	0x00, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
};

// Center of screen in sprite coordinates
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
		// Move with arrow keys
		char ch = keyboardGetCharAsync();
		if (ch == KEY_LEFT) sx--;
		if (ch == KEY_RIGHT) sx++;
		if (ch == KEY_UP) sy--;
		if (ch == KEY_DOWN) sy++;
		sprite_move(0, CenterX + sx, CenterY + sy);

		// Calculate atan2, show timing with border color
		graphicsSetBorderC64Color(2);
		float a = fast_atan2(sy, sx);
		graphicsSetBorderC64Color(0);

		// Print result
		textGotoXY(0, 0);
		textPrintInt(sx);
		textPrint(", ");
		textPrintInt(sy);
		textPrint(", ");
		char buf[20];
		sprintf(buf, "%.3", a);
		textPrint(buf);
		textPrint("    ");

		graphicsWaitVerticalBlank();
	}

	return 0;
}
