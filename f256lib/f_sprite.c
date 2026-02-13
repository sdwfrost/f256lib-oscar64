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


// Sprite convenience layer
// ------------------------

static int  _spr_x[8], _spr_y[8];
static byte _spr_enabled;
static byte _spr_image[8];
static byte _spr_color[8];


void spriteInitClut(void) {
	byte i;
	for (i = 0; i < 16; i++)
		graphicsDefineColor(0, i, c64Palette[i].r, c64Palette[i].g, c64Palette[i].b);

	// Entry 0: transparent (TinyVicky treats pixel value 0 as transparent)
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_1);
	POKE(VKY_GR_CLUT_0 + 0, 0);
	POKE(VKY_GR_CLUT_0 + 1, 0);
	POKE(VKY_GR_CLUT_0 + 2, 0);
	POKE(VKY_GR_CLUT_0 + 3, 0);  // alpha = 0 -> transparent
	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


byte spriteExpand(const char *src, byte slot, byte color) {
	uint32_t dest = SPR_DATA_BASE + (uint32_t)slot * SPR_IMG_SIZE;
	byte row, col;

	for (row = 0; row < 21; row++) {
		byte b0 = (byte)src[row * 3 + 0];
		byte b1 = (byte)src[row * 3 + 1];
		byte b2 = (byte)src[row * 3 + 2];

		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + col, (b0 & (0x80 >> col)) ? color : 0);
		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + 8 + col, (b1 & (0x80 >> col)) ? color : 0);
		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + 16 + col, (b2 & (0x80 >> col)) ? color : 0);
	}

	// Bottom 3 rows: transparent
	for (row = 21; row < 24; row++)
		for (col = 0; col < 24; col++)
			FAR_POKE(dest + row * 24 + col, 0);

	return slot;
}


void spriteInit(void) {
	byte i;

	_spr_enabled = 0;
	for (i = 0; i < 8; i++) {
		_spr_x[i] = 0;
		_spr_y[i] = 0;
		_spr_image[i] = 0;
		_spr_color[i] = 0;
	}

	spriteInitClut();

	// Enable sprite engine in master control
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);
	POKE(VKY_MSTR_CTRL_0, PEEK(VKY_MSTR_CTRL_0) | VKY_GRAPH | VKY_SPRITE);

	spriteReset();
}


void spriteSet(byte sp, bool show, int xpos, int ypos,
                byte image, byte color) {
	sp &= 7;
	_spr_x[sp] = xpos;
	_spr_y[sp] = ypos;
	_spr_image[sp] = image;
	_spr_color[sp] = color;

	if (show)
		_spr_enabled |= (1 << sp);
	else
		_spr_enabled &= ~(1 << sp);

	uint32_t addr = SPR_DATA_BASE + (uint32_t)image * SPR_IMG_SIZE;
	spriteDefine(sp, addr, 24, 0, 0);
	spriteSetPosition(sp, (uint16_t)(xpos + SPR_OFFSET_X),
	                      (uint16_t)(ypos + SPR_OFFSET_Y));
	spriteSetVisible(sp, show);
}


void spriteMove(byte sp, int xpos, int ypos) {
	sp &= 7;
	_spr_x[sp] = xpos;
	_spr_y[sp] = ypos;
	spriteSetPosition(sp, (uint16_t)(xpos + SPR_OFFSET_X),
	                      (uint16_t)(ypos + SPR_OFFSET_Y));
}


void spriteSetImage(byte sp, byte image) {
	sp &= 7;
	_spr_image[sp] = image;
	uint32_t addr = SPR_DATA_BASE + (uint32_t)image * SPR_IMG_SIZE;
	spriteDefine(sp, addr, 24, 0, 0);
	spriteSetVisible(sp, (_spr_enabled & (1 << sp)) ? true : false);
}


void spriteRecolor(byte sp, const char *src, byte newColor) {
	sp &= 7;
	_spr_color[sp] = newColor;
	spriteExpand(src, _spr_image[sp], newColor);
}


void spriteShow(byte sp, bool show) {
	sp &= 7;
	if (show)
		_spr_enabled |= (1 << sp);
	else
		_spr_enabled &= ~(1 << sp);
	spriteSetVisible(sp, show);
}


byte spriteCheckCollisions(void) {
	byte result = 0;
	byte i, j;

	for (i = 0; i < 8; i++) {
		if (!(_spr_enabled & (1 << i)))
			continue;
		for (j = i + 1; j < 8; j++) {
			if (!(_spr_enabled & (1 << j)))
				continue;

			int dx = _spr_x[i] - _spr_x[j];
			int dy = _spr_y[i] - _spr_y[j];
			if (dx < 0) dx = -dx;
			if (dy < 0) dy = -dy;

			if (dx < 24 && dy < 24) {
				result |= (1 << i);
				result |= (1 << j);
			}
		}
	}
	return result;
}


#endif
