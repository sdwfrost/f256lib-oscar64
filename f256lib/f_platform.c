/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_PLATFORM


#include "f256lib.h"


void f256putchar(char c) {
	static char s[2] = { 0, 0 };
	s[0] = c;
	textPrint(s);
}


int f256getchar(void) {
	while (1) {
		kernelNextEvent();
		if (kernelError) {
			kernelCall(Yield);
			continue;
		}

		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;
		if (kernelEventData.u.key.flags) continue;  // Meta key.

		return kernelEventData.u.key.ascii;
	}
}


bool platformIsAnyK(void) {
	uint8_t value = PEEK(VKY_MID) & 0x1F;
	return (value >= 0x10 && value <= 0x16);
}


bool platformIsK2(void) {
	uint8_t value = PEEK(VKY_MID) & 0x1F;
	return (value == 0x11);
}


bool platformHasCaseLCD(void) {
	return ((PEEK(0xDDC1) & 0x02) == 0);
}


bool platformIsWave2(void) {
	uint8_t mid = PEEK(VKY_MID) & 0x3F;
	return (mid == 0x22 || mid == 0x11);
}


void platformSetGraphicMode(bool sprites, bool bitmaps, bool tiles, bool textOverlay, bool text) {
	// VKY_MSTR_CTRL_0 bit layout:
	//   bit 0: text mode
	//   bit 1: text overlay (on graphics)
	//   bit 2: graphics mode (bitmap)
	//   bit 3: graphics mode (tile)
	//   bit 4: sprite engine
	//   bit 5: gamma correction
	byte val = PEEK(VKY_MSTR_CTRL_0) & 0xE0;  // preserve bits 5-7

	if (text)        val |= 0x01;
	if (textOverlay) val |= 0x02;
	if (bitmaps)     val |= 0x04;
	if (tiles)       val |= 0x08;
	if (sprites)     val |= 0x10;

	POKE(VKY_MSTR_CTRL_0, val);
}


void platformSetBorderSize(byte width, byte height) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	if (width == 0 && height == 0) {
		// Disable border
		POKE(VKY_BRDR_CTRL, 0x00);
	} else {
		// Enable border and set size
		POKE(VKY_BRDR_CTRL, 0x01);
		POKE(VKY_BRDR_HORI, width);
		POKE(VKY_BRDR_VERT, height);
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void platformSwitchFont(bool usePrimary) {
	// VKY_MSTR_CTRL_1 bit 0: 0=primary font, 1=secondary font
	byte val = PEEK(VKY_MSTR_CTRL_1);
	if (usePrimary) {
		val &= ~0x01;
	} else {
		val |= 0x01;
	}
	POKE(VKY_MSTR_CTRL_1, val);
}


void platformEnableTextCursor(bool enable) {
	if (enable) {
		POKE(VKY_CRSR_CTRL, 0x03);  // Enable cursor, flash rate
	} else {
		POKE(VKY_CRSR_CTRL, 0x00);  // Disable cursor
	}
}


byte platformGetModelId(void) {
	return PEEK(VKY_MID);
}


#endif
