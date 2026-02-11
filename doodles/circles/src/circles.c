#include "f256lib.h"

#define WIDTH 320
#define HEIGHT 240


void drawFilledCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint8_t col) {
	bitmapSetColor(col);
	for (int16_t y = -radius; y <= (int16_t)radius; y++) {
		int16_t yy = (int16_t)y0 + y;
		if (yy < 0 || yy >= HEIGHT) continue;
		bitmapLine(x0 - radius, yy, x0 + radius, yy);
	}
}

int main(int argc, char *argv[]) {
	uint16_t x, y, r, c;

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00001111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);
	POKE(VKY_LAYER_CTRL_0, 0b00010000);
	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	bitmapSetActive(0);
	bitmapSetCLUT(0);
	bitmapSetColor(4);
	bitmapSetVisible(0, true);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);

	bitmapSetColor(0);
	for (uint16_t xi = 0; xi < 320; xi++) {
		for (uint8_t yi = 0; yi < 240; yi++) {
			bitmapPutPixel(xi, yi);
		}
	}
	printf("go");
	while (true) {
		x = randomRead();
		r = randomRead();
		drawFilledCircle(x >> 8, x & 0x00FF, r >> 10, (uint8_t)(r & 0x00FF));
	}

	return 0;
}
