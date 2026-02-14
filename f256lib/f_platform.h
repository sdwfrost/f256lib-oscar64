/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef PLATFORM_H
#define PLATFORM_H
#ifndef WITHOUT_PLATFORM


#include "f256lib.h"


// Renamed from __putchar and getchar to avoid conflicts with
// oscar64's standard library.
void f256putchar(char c);
int  f256getchar(void);


// Platform detection (from mu0nlibs/muUtils)
bool platformIsAnyK(void);
bool platformIsK2(void);
bool platformHasCaseLCD(void);
bool platformIsWave2(void);

// Video mode control
// Enables/disables individual graphics layers on VKY_MSTR_CTRL_0
void platformSetGraphicMode(bool sprites, bool bitmaps, bool tiles, bool textOverlay, bool text);

// Border control: set width (left+right) and height (top+bottom) in pixels
void platformSetBorderSize(byte width, byte height);

// Switch between primary and secondary font slot
void platformSwitchFont(bool usePrimary);

// Enable or disable the hardware text cursor
void platformEnableTextCursor(bool enable);

// Return the raw model ID byte from VKY_MID
byte platformGetModelId(void);


#pragma compile("f_platform.c")


#endif
#endif // PLATFORM_H
