/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef GRAPHICS_H
#define GRAPHICS_H
#ifndef WITHOUT_GRAPHICS


#include "f256lib.h"


extern const colorT c64Palette[16];

void graphicsDefineColor(byte clut, byte slot, byte r, byte g, byte b);
void graphicsPause(uint16_t frames);
void graphicsReset(void);
void graphicsSetBackgroundC64Color(byte c);
void graphicsSetBackgroundRGB(byte r, byte g, byte b);
void graphicsSetBorderC64Color(byte c);
void graphicsSetBorderRGB(byte r, byte g, byte b);
void graphicsSetLayerBitmap(byte layer, byte which);
void graphicsSetLayerTile(byte layer, byte which);
void graphicsWaitVerticalBlank(void);


#pragma compile("f_graphics.c")


#endif
#endif // GRAPHICS_H
