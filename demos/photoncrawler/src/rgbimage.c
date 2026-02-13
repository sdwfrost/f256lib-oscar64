#include "f256lib.h"
#include "rgbimage.h"

RGBA rgbblock[8][4];
RGBA rgbpalette[4];  // unused but kept for API compatibility

void rgbimg_begin(void)
{
	// Set up 6x6x6 RGB cube palette (216 colors) in CLUT 0
	byte idx = 0;
	for (byte r = 0; r < 6; r++)
		for (byte g = 0; g < 6; g++)
			for (byte b = 0; b < 6; b++)
				graphicsDefineColor(0, idx++, r * 51, g * 51, b * 51);

	// Set up bitmap display
	bitmapSetCLUT(0);
	bitmapSetColor(0);
	bitmapClear();
	bitmapSetVisible(0, true);
	graphicsSetBackgroundRGB(0, 0, 0);
	graphicsSetBorderRGB(0, 0, 0);
}

void rgbimg_end(void)
{
	bitmapSetVisible(0, false);
}

void rgbimg_putblock(char x, char y)
{
	// Each block: 4 traced pixels wide, 8 tall
	// Each traced pixel maps to 2 screen pixels horizontally
	for (char row = 0; row < 8; row++)
	{
		for (char col = 0; col < 4; col++)
		{
			RGBA c = rgbblock[row][col];
			byte idx = (byte)((unsigned)c.r / 43) * 36
			         + (byte)((unsigned)c.g / 43) * 6
			         + (byte)((unsigned)c.b / 43);
			uint16_t sx = (uint16_t)x * 8 + (uint16_t)col * 2;
			uint16_t sy = (uint16_t)y * 8 + row;
			bitmapSetColor(idx);
			bitmapPutPixel(sx, sy);
			bitmapPutPixel(sx + 1, sy);
		}
	}
}

void rgbimg_noiseblock(void)
{
	// Light noise for anti-banding (simplified from C64 version)
	// With 216 colors (vs C64's 4 per block), much less noise needed
}

void rgbimg_buildpal(void)
{
	// no-op: 216-color CLUT needs no per-block palette selection
}

void rgbimg_mapblock(void)
{
	// no-op: no dithering needed with 216 colors
}

char rgbimg_mapcolor(RGBA c)
{
	return (byte)((unsigned)c.r / 43) * 36
	     + (byte)((unsigned)c.g / 43) * 6
	     + (byte)((unsigned)c.b / 43);
}
