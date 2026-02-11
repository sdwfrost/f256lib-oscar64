#include "f256lib.h"

#define TEXT_SCROLL_COOKIE 0
#define TEXT_SCROLL_DELAY 2

struct timer_t scrollTextTimer;
const int16_t SIN[] = {
	   0,    1,    3,    4,    6,    7,    9,   10,
	  12,   14,   15,   17,   18,   20,   21,   23,
	  24,   25,   27,   28,   30,   31,   32,   34,
	  35,   36,   38,   39,   40,   41,   42,   44,
	  45,   46,   47,   48,   49,   50,   51,   52,
	  53,   54,   54,   55,   56,   57,   57,   58,
	  59,   59,   60,   60,   61,   61,   62,   62,
	  62,   63,   63,   63,   63,   63,   63,   63,
	  64,   63,   63,   63,   63,   63,   63,   63,
	  62,   62,   62,   61,   61,   60,   60,   59,
	  59,   58,   57,   57,   56,   55,   54,   54,
	  53,   52,   51,   50,   49,   48,   47,   46,
	  45,   44,   42,   41,   40,   39,   38,   36,
	  35,   34,   32,   31,   30,   28,   27,   25,
	  24,   23,   21,   20,   18,   17,   15,   14,
	  12,   10,    9,    7,    6,    4,    3,    1,
	   0,   -1,   -3,   -4,   -6,   -7,   -9,  -10,
	 -12,  -14,  -15,  -17,  -18,  -20,  -21,  -23,
	 -24,  -25,  -27,  -28,  -30,  -31,  -32,  -34,
	 -35,  -36,  -38,  -39,  -40,  -41,  -42,  -44,
	 -45,  -46,  -47,  -48,  -49,  -50,  -51,  -52,
	 -53,  -54,  -54,  -55,  -56,  -57,  -57,  -58,
	 -59,  -59,  -60,  -60,  -61,  -61,  -62,  -62,
	 -62,  -63,  -63,  -63,  -63,  -63,  -63,  -63,
	 -64,  -63,  -63,  -63,  -63,  -63,  -63,  -63,
	 -62,  -62,  -62,  -61,  -61,  -60,  -60,  -59,
	 -59,  -58,  -57,  -57,  -56,  -55,  -54,  -54,
	 -53,  -52,  -51,  -50,  -49,  -48,  -47,  -46,
	 -45,  -44,  -42,  -41,  -40,  -39,  -38,  -36,
	 -35,  -34,  -32,  -31,  -30,  -28,  -27,  -25,
	 -24,  -23,  -21,  -20,  -18,  -17,  -15,  -14,
	 -12,  -10,   -9,   -7,   -6,   -4,   -3,   -1,
};

const uint8_t scrollLUT[16] = {0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71,
                               0x71, 0x61, 0x51, 0x41, 0x31, 0x21, 0x11, 0x01};

uint8_t wholeMap[100][60];

void fullRandomText(void) {
	uint16_t rresult = 0;
	textGotoXY(0, 0);
	for (uint8_t j = 0; j < 60; j++) {
		textGotoXY(0, j);
		for (uint8_t i = 0; i < 100; i++) {
			rresult = HIGH_BYTE(randomRead());
			wholeMap[i][j] = (rresult >> 5) + 16;
		}
	}
}

void showText(int8_t offX, int8_t offY) {
	for (uint8_t j = 0; j < 59; j++) {
		textGotoXY(0, j);
		for (int8_t i = 10; i < 89; i++) {
			f256putchar(wholeMap[offX + i][j]);
		}
	}
}

void setup(void) {
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00000111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);
	POKE(VKY_LAYER_CTRL_0, 0b00010000);
	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	POKE(MMU_IO_CTRL, 0);

	POKE(0xD004, 0x01);
	POKE(0xD008, 16);
	POKE(0xD009, 0);
	bitmapSetVisible(0, false);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);
	scrollTextTimer.cookie = TEXT_SCROLL_COOKIE;
	scrollTextTimer.units = TIMER_FRAMES;
	scrollTextTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TEXT_SCROLL_DELAY;
	kernelSetTimer(&scrollTextTimer);
}

int main(int argc, char *argv[]) {
	uint8_t scroll = 0;

	setup();
	fullRandomText();

	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			switch (kernelEventData.u.timer.cookie) {
				case TEXT_SCROLL_COOKIE:
					scroll++;
					{
						int8_t bs = scroll % 8;
						bool dir = SIN[scroll] > 0 ? true : false;
						int8_t offX = (SIN[scroll] >> 3);
						POKE(0xD004, (0x20 + (dir ? (bs << 3) : (-bs << 3))) | 0x01);

						showText(offX, 10);
						scrollTextTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TEXT_SCROLL_DELAY;
						kernelSetTimer(&scrollTextTimer);
					}
					break;
			}
		}
	}
	return 0;
}
