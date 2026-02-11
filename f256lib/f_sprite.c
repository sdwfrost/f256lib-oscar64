/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_SPRITE


#include "f256lib.h"


#define OFF_SPR_ADL_L    1
#define OFF_SPR_POS_X_L  4
#define OFF_SPR_POS_Y_L  6


static byte _spriteCtl[64];


void spriteDefine(byte s, uint32_t address, byte size, byte CLUT, byte layer) {
	uint16_t sprite = VKY_SP0_CTRL + (s * 8);
	byte     sz;

	switch (size) {
		case 8:
			sz = 3;
			break;
		case 16:
			sz = 2;
			break;
		case 24:
			sz = 1;
			break;
		default:
			sz = 0;
			break;
	}

	_spriteCtl[s] = (sz << 5) | (layer << 3) | (CLUT << 1);
	POKE(sprite, _spriteCtl[s]);
	POKEA(sprite + OFF_SPR_ADL_L, address);
}


void spriteSetPosition(byte s, uint16_t x, uint16_t y) {
	uint16_t sprite = VKY_SP0_CTRL + (s * 8);

	POKEW(sprite + OFF_SPR_POS_X_L, x);
	POKEW(sprite + OFF_SPR_POS_Y_L, y);
}


void spriteSetVisible(byte s, bool v) {
	uint16_t sprite = VKY_SP0_CTRL + (s * 8);

	POKE(sprite, _spriteCtl[s] | (byte)v);
}


void spriteReset(void) {
	byte x;
	for (x=0; x<64; x++) spriteSetVisible(x, false);
}


#endif
