/*
 *	LCD display support for F256K2 case display.
 *	Adapted from mu0nlibs/mulcd for oscar64.
 */


#ifndef WITHOUT_LCD


#include "f256lib.h"


// Display a 240x280 image centered on the LCD screen.
// Assumes a R5G6B5 bitmap file converted to 2-byte-per-pixel raw binary,
// stored at the given far memory address.
// boost: 1=normal, 2=double pixels, 4=quad pixels
void lcdDisplayImage(uint32_t addr, uint8_t boost) {
	uint32_t index = 0;
	uint8_t innerLoop = 240 / boost;
	uint8_t skip = 2 * boost;
	bool doubleSpeed = false, quadSpeed = false;
	uint16_t curPixel = 0x0000;
	uint16_t j;

	if (boost == 2) doubleSpeed = true;
	if (boost == 4) {
		doubleSpeed = true;
		quadSpeed = true;
	}

	POKE(LCD_CMD_CMD, LCD_WIN_X);
	POKE(LCD_CMD_DTA, 0);     // xstart high
	POKE(LCD_CMD_DTA, 0);     // xstart low
	POKE(LCD_CMD_DTA, 0);     // xend high
	POKE(LCD_CMD_DTA, 239);   // xend low

	POKE(LCD_CMD_CMD, LCD_WIN_Y);
	POKE(LCD_CMD_DTA, 0);     // ystart high
	POKE(LCD_CMD_DTA, 20);    // ystart low
	POKE(LCD_CMD_DTA, 0x01);  // yend high
	POKE(LCD_CMD_DTA, 0x3F);  // yend low

	POKE(LCD_CMD_CMD, LCD_WRI);

	for (j = 0; j < 280; j++) {
		uint8_t i;
		for (i = 0; i < innerLoop; i++) {
			curPixel = FAR_PEEKW(addr + index);
			POKEW(LCD_PIX_LO, curPixel);
			if (doubleSpeed) POKEW(LCD_PIX_LO, curPixel);
			if (quadSpeed) {
				POKEW(LCD_PIX_LO, curPixel);
				POKEW(LCD_PIX_LO, curPixel);
			}
			index += skip;
		}
	}
}


#endif
