/*
 *	Float math and sprite demo.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Note: EMBED for palette/sprite assets not yet converted.
 *	Note: fp_module.h dependency removed - uses f256lib math functions.
 */

#include "f256lib.h"

#define CENTERX 160
#define CENTERY 150
#define RADIUS 100.0
#define TOTALINDEX 60

#define PAL_BASE         0x10000
#define SPR_THNG_BASE    0x10800
#define SPR_SIZE  16
#define SPR_SIZE_SQUARED  0x100

// TODO: EMBED(palgrudge, "../assets/grudge.pal", 0x10000);
// TODO: EMBED(thing, "../assets/thing.bin", 0x10800);


const float sinTable[60] = {
	0.0, 0.104528, 0.207912, 0.309017, 0.406737, 0.5, 0.587785, 0.669131,
	0.743145, 0.809017, 0.866025, 0.913545, 0.951057, 0.978148, 0.994522,
	1.0, 0.994522, 0.978148, 0.951057, 0.913545, 0.866025, 0.809017,
	0.743145, 0.669131, 0.587785, 0.5, 0.406737, 0.309017, 0.207912,
	0.104528, 1.23e-16, -0.104530, -0.207910, -0.309020, -0.406740,
	-0.5, -0.587790, -0.669130, -0.743140, -0.809020, -0.866030,
	-0.913550, -0.951060, -0.978150, -0.994520, -1.0, -0.994520,
	-0.978150, -0.951060, -0.913550, -0.866030, -0.809020, -0.743140,
	-0.669130, -0.587790, -0.5, -0.406740, -0.309020, -0.207910,
	-0.104530
};

int main(int argc, char *argv[]) {

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00101111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);

	POKE(MMU_IO_CTRL, 1);
	for (uint16_t c = 0; c < 1023; c++) {
		POKE(VKY_GR_CLUT_0 + c, FAR_PEEK(PAL_BASE + c));
	}
	POKE(MMU_IO_CTRL, 0);

	spriteDefine(0, SPR_THNG_BASE, SPR_SIZE, 0, 0);
	spriteSetVisible(0, true);
	spriteSetPosition(0, CENTERX, CENTERY);

	float fx, fy;
	uint8_t mode = 0;

	uint8_t indexS = 0;
	uint8_t indexC = 15;

	int16_t x = 10, y = 0;

	textGotoXY(0, 0);
	textPrint("Regular C float products                   ");
	POKE(0xD600, 0x98);

	while (true) {
		switch (mode) {
			case 0:
				fx = sinTable[indexC++] * RADIUS;
				fy = sinTable[indexS++] * RADIUS;
				if (indexC == TOTALINDEX) indexC = 0;
				if (indexS == TOTALINDEX) indexS = 0;

				x = (uint16_t)((int16_t)CENTERX + (int16_t)fx);
				y = (uint16_t)((int16_t)CENTERY + (int16_t)fy);
				break;
			case 1:
				fx = sinTable[indexC++] * RADIUS;
				fy = sinTable[indexS++] * RADIUS;
				if (indexC == TOTALINDEX) indexC = 0;
				if (indexS == TOTALINDEX) indexS = 0;

				x = CENTERX + (int16_t)(fx);
				y = CENTERY + (int16_t)(fy);
				break;
		}
		POKE(0xD600, indexS & 0x1F);
		POKE(0xD600, (indexS & 0xF0) >> 4);
		spriteSetPosition(0, x, y);
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (mode) {
				case 0:
					mode = 1;
					textGotoXY(0, 0);
					textPrint("2x core float block                        ");
					break;
				case 1:
					mode = 0;
					textGotoXY(0, 0);
					textPrint("Regular C float products                   ");
					break;
			}
		}
	}

	return 0;
}
