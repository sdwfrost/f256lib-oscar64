/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_GRAPHICS


#include "f256lib.h"


const colorT c64Palette[16] = {
	{ 0x00, 0x00, 0x00 },  //  0 black
	{ 0xFF, 0xFF, 0xFF },  //  1 white
	{ 0x88, 0x39, 0x32 },  //  2 red
	{ 0x67, 0xB6, 0xBD },  //  3 cyan
	{ 0x8B, 0x3F, 0x96 },  //  4 purple
	{ 0x55, 0xA0, 0x49 },  //  5 green
	{ 0x40, 0x31, 0x8D },  //  6 blue
	{ 0xBF, 0xCE, 0x72 },  //  7 yellow
	{ 0x8B, 0x54, 0x29 },  //  8 orange
	{ 0x57, 0x42, 0x00 },  //  9 brown
	{ 0xB8, 0x69, 0x62 },  // 10 light red
	{ 0x50, 0x50, 0x50 },  // 11 dark grey
	{ 0x78, 0x78, 0x78 },  // 12 grey
	{ 0x94, 0xE0, 0x89 },  // 13 light green
	{ 0x78, 0x69, 0xC4 },  // 14 light blue
	{ 0x9F, 0x9F, 0x9F },  // 15 light grey
};


void graphicsDefineColor(byte clut, byte slot, byte r, byte g, byte b) {
	byte      mmu = PEEK(MMU_IO_CTRL);
	byte     *write;
	uint16_t  gclut;

	switch (clut) {
		case 0:
			gclut = VKY_GR_CLUT_0;
			break;
		case 1:
			gclut = VKY_GR_CLUT_1;
			break;
		case 2:
			gclut = VKY_GR_CLUT_2;
			break;
		default:
			gclut = VKY_GR_CLUT_3;
			break;
	}

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_1);

	write = (byte *)gclut + (slot * 4);
	*write++ = b;
	*write++ = g;
	*write++ = r;
	*write++ = 0xff;

	POKE(MMU_IO_CTRL, mmu);
}


void graphicsReset(void) {
	int16_t  x;
	byte     y;

	for (y=0; y<4; y++) {
		for (x=0; x<256; x++) {
			graphicsDefineColor(y, x, x, x, x);
		}
	}

	graphicsSetLayerBitmap(0, 0);
	graphicsSetLayerBitmap(1, 1);
	graphicsSetLayerBitmap(2, 2);
}


void graphicsSetLayerBitmap(byte layer, byte which) {
	switch (layer) {
		case 0:
			POKE(VKY_LAYER_CTRL_0, (PEEK(VKY_LAYER_CTRL_0) & 0xf0) | which);
			break;
		case 1:
			POKE(VKY_LAYER_CTRL_0, (PEEK(VKY_LAYER_CTRL_0) & 0x0f) | (which << 4));
			break;
		case 2:
			POKE(VKY_LAYER_CTRL_1, which);
			break;
	}
}


void graphicsSetLayerTile(byte layer, byte which) {
	graphicsSetLayerBitmap(layer, which + 4);
}


void graphicsPause(uint16_t frames) {
	uint16_t i;
	for (i = 0; i < frames; i++)
		graphicsWaitVerticalBlank();
}


void graphicsSetBackgroundC64Color(byte c) {
	const colorT *p = &c64Palette[c & 15];
	graphicsSetBackgroundRGB(p->r, p->g, p->b);
}


void graphicsSetBackgroundRGB(byte r, byte g, byte b) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	POKE(VKY_BKG_COL_R, r);
	POKE(VKY_BKG_COL_G, g);
	POKE(VKY_BKG_COL_B, b);
	POKE(MMU_IO_CTRL, mmu);
}


void graphicsSetBorderC64Color(byte c) {
	const colorT *p = &c64Palette[c & 15];
	graphicsSetBorderRGB(p->r, p->g, p->b);
}


void graphicsSetBorderRGB(byte r, byte g, byte b) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	POKE(VKY_BRDR_COL_R, r);
	POKE(VKY_BRDR_COL_G, g);
	POKE(VKY_BRDR_COL_B, b);
	POKE(MMU_IO_CTRL, mmu);
}


void graphicsWaitVerticalBlank(void) {
	while (PEEKW(RAST_ROW_L) != 482)
		;
}


#endif
