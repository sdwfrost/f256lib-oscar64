/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_BITMAP


#include "f256lib.h"


static uint16_t _MAX_X;
static uint16_t _MAX_Y;
static uint32_t _PAGE_SIZE;
static uint32_t _BITMAP_BASE[3];
static byte     _BITMAP_CLUT[3];
static byte     _color;
static byte     _active;


// Replaced GCC statement expression with a do/while macro.
// Must only be used as a statement (not an expression).
#define bitmapPutPixelIOSet(px, py) do { \
	uint32_t _bpp_address = _BITMAP_BASE[_active] + mathUnsignedAddition(mathUnsignedMultiply((py), _MAX_X), (int32_t)(px)); \
	byte     _bpp_block   = _bpp_address / EIGHTK; \
	_bpp_address &= 0x1FFF; \
	POKE(SWAP_SLOT, _bpp_block); \
	POKE(SWAP_ADDR + _bpp_address, _color); \
} while(0)


void bitmapClear(void) {
#ifdef BOOM
	dmaFill(_BITMAP_BASE[_active], _PAGE_SIZE, _color);
#else
	byte     block = _BITMAP_BASE[_active] / EIGHTK;
	byte     x;
	uint16_t c;
	volatile byte *mem = (byte *)SWAP_ADDR;

	SWAP_IO_SETUP();

	for (x=0; x<9; x++) {
		POKE(SWAP_SLOT, block++);
		for (c=0; c<EIGHTK; c++) mem[c] = _color;
	}
	POKE(SWAP_SLOT, block);
	for (c=0; c<5120; c++) mem[c] = _color;

	SWAP_RESTORE_SLOT();
	SWAP_IO_SHUTDOWN();
#endif
}


void bitmapGetResolution(uint16_t *x, uint16_t *y) {
	*x = _MAX_X;
	*y = _MAX_Y;
}


void bitmapLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	uint16_t x;
	uint16_t y;
	int16_t  dx;
	int16_t  dy;
	int16_t  incX;
	int16_t  incY;
	int16_t  balance;

	SWAP_IO_SETUP();

	if (x2 >= x1) {
		dx = x2 - x1;
		incX = 1;
	} else {
		dx = x1 - x2;
		incX = -1;
	}

	if (y2 >= y1) {
		dy = y2 - y1;
		incY = 1;
	} else {
		dy = y1 - y2;
		incY = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy) {
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;
		while (x != x2) {
			bitmapPutPixelIOSet(x, y);
			if (balance >= 0) {
				y += incY;
				balance -= dx;
			}
			balance += dy;
			x += incX;
		}
		bitmapPutPixelIOSet(x, y);
	} else {
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;
		while (y != y2) {
			bitmapPutPixelIOSet(x, y);
			if (balance >= 0) {
				x += incX;
				balance -= dy;
			}
			balance += dx;
			y += incY;
		}
		bitmapPutPixelIOSet(x, y);
	}

	SWAP_RESTORE_SLOT();
	SWAP_IO_SHUTDOWN();
}


void bitmapPutPixel(uint16_t x, uint16_t y) {
	SWAP_IO_SETUP();
	bitmapPutPixelIOSet(x, y);
	SWAP_RESTORE_SLOT();
	SWAP_IO_SHUTDOWN();
}


void bitmapReset(void) {
	uint32_t realSize;
	uint32_t pageBlocks;

	_MAX_X       = 320;
	_MAX_Y       = 240;
	_PAGE_SIZE   = mathUnsignedMultiply(_MAX_X, _MAX_Y);
	_active      = 0;
	_color       = 255;

	pageBlocks   = _PAGE_SIZE / EIGHTK;
	if (mathUnsignedMultiply(pageBlocks, EIGHTK) != _PAGE_SIZE) {
		pageBlocks++;
	}
	realSize = mathUnsignedMultiply(pageBlocks, EIGHTK);

	_BITMAP_BASE[0] = 0x080000 - realSize;
	_BITMAP_BASE[1] = _BITMAP_BASE[0] - realSize;
	_BITMAP_BASE[2] = _BITMAP_BASE[1] - realSize;

	_BITMAP_CLUT[0] = 0;
	_BITMAP_CLUT[1] = 0;
	_BITMAP_CLUT[2] = 0;

	POKEA(VKY_BM0_ADDR_L, _BITMAP_BASE[0]);
	POKEA(VKY_BM1_ADDR_L, _BITMAP_BASE[1]);
	POKEA(VKY_BM2_ADDR_L, _BITMAP_BASE[2]);

	bitmapSetVisible(0, false);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);
}


void bitmapSetActive(byte p) {
	_active = p;
}


void bitmapSetAddress(byte p, uint32_t a) {
	_BITMAP_BASE[p] = a;
	switch (p) {
		case 0:
			POKEA(VKY_BM0_ADDR_L, a);
			break;
		case 1:
			POKEA(VKY_BM1_ADDR_L, a);
			break;
		case 2:
			POKEA(VKY_BM2_ADDR_L, a);
			break;
	}
}


void bitmapSetCLUT(byte clut) {
	_BITMAP_CLUT[_active] = clut << 1;

	switch (_active) {
		case 0:
			POKE(VKY_BM0_CTRL, (PEEK(VKY_BM0_CTRL) & 0xf9) | _BITMAP_CLUT[_active]);
			break;
		case 1:
			POKE(VKY_BM1_CTRL, (PEEK(VKY_BM1_CTRL) & 0xf9) | _BITMAP_CLUT[_active]);
			break;
		case 2:
			POKE(VKY_BM2_CTRL, (PEEK(VKY_BM2_CTRL) & 0xf9) | _BITMAP_CLUT[_active]);
			break;
	}
}


void bitmapSetColor(byte c) {
	_color = c;
}


void bitmapSetVisible(byte p, bool v) {
	switch (p) {
		case 0:
			POKE(VKY_BM0_CTRL, v ? 1 | _BITMAP_CLUT[p] : 0);
			break;
		case 1:
			POKE(VKY_BM1_CTRL, v ? 1 | _BITMAP_CLUT[p] : 0);
			break;
		case 2:
			POKE(VKY_BM2_CTRL, v ? 1 | _BITMAP_CLUT[p] : 0);
			break;
	}
}


#endif
