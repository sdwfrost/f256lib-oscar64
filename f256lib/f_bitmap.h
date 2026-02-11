/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef BITMAP_H
#define BITMAP_H
#ifndef WITHOUT_BITMAP


#include "f256lib.h"


void bitmapClear(void);
void bitmapGetResolution(uint16_t *x, uint16_t *y);
void bitmapLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void bitmapPutPixel(uint16_t x, uint16_t y);
void bitmapReset(void);
void bitmapSetActive(byte p);
void bitmapSetAddress(byte p, uint32_t a);
void bitmapSetCLUT(byte clut);
void bitmapSetColor(byte c);
void bitmapSetVisible(byte p, bool v);


#pragma compile("f_bitmap.c")


#endif
#endif // BITMAP_H
