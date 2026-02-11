/*
 *	Mandelbrot fractal.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Note: EMBED for palette not yet implemented - uses default CLUT.
 */

#include "f256lib.h"

#define PAL_BASE    0x10000


void setup(void) {
	uint16_t c;

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00001111);
	POKE(VKY_MSTR_CTRL_1, 0b00010100);
	POKE(VKY_LAYER_CTRL_0, 0b00010000);
	POKE(VKY_LAYER_CTRL_1, 0b00000010);
	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	POKE(MMU_IO_CTRL, 1);
	for (c = 0; c < 1023; c++) {
		POKE(VKY_GR_CLUT_0 + c, FAR_PEEK(PAL_BASE + c));
	}

	POKE(MMU_IO_CTRL, 0x00);

	bitmapSetActive(0);
	bitmapSetCLUT(0);

	bitmapSetVisible(0, true);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);
}

void dopixel(uint16_t x, uint16_t y, uint8_t c) {
	bitmapSetColor(c);
	bitmapPutPixel(x, y);
}

int main(int argc, char *argv[]) {
	uint16_t x, y;
	float x0, y0, xf, yf, x2, y2, result;
	uint8_t iter;

	setup();

	for (y = 0; y < 240; y++) {
		for (x = 0; x < 320; x++) {
			result = 0.0;
			iter = 0;
			xf = 0.0;
			yf = 0.0;
			x2 = 0.0;
			y2 = 0.0;
			x0 = -2.0 + (float)x / 106.666;
			y0 = -1.0 + (float)y / 120.0;
			while (iter < 20) {
				x2 = xf * xf;
				y2 = yf * yf;
				result = x2 + y2;
				if (result > 4.0) break;
				xf = x2 - y2 + x0;
				yf = 2.0 * xf * yf + y0;
				iter++;
			}

			dopixel(x, y, 0xff - iter);
		}
	}
	while (true);
	return 0;
}
