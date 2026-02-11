/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef TEXT_H
#define TEXT_H
#ifndef WITHOUT_TEXT


#include "f256lib.h"


// Apple IIgs Colors, because that's how we roll.
typedef enum textColorsE {
	BLACK = 0,
	DEEP_RED,
	DARK_BLUE,
	PURPLE,
	DARK_GREEN,
	DARK_GRAY,
	MEDIUM_BLUE,
	LIGHT_BLUE,
	BROWN,
	ORANGE,
	LIGHT_GRAY,
	PINK,
	LIGHT_GREEN,
	YELLOW,
	AQUAMARINE,
	WHITE,
	TEXTCOLORS_COUNT
} textColorsT;


extern colorT textColors[16];


void textClear(void);
void textDefineBackgroundColor(byte slot, byte r, byte g, byte b);
void textDefineForegroundColor(byte slot, byte r, byte g, byte b);
void textEnableBackgroundColors(bool b);
void textGetXY(byte *x, byte *y);
void textGotoXY(byte x, byte y);
void textPrint(const char *message);
void textPrintInt(int32_t value);
void textPrintUInt(uint32_t value);
void textScrollUp(void);
void textPutChar(char c);
void textPrintHex(uint32_t value, byte digits);
void textPrintFloat(float val, byte decimals);
#ifndef WITHOUT_KEYBOARD
void textReadLine(char *buf, byte maxlen);
char textReadInt(int *result);
#endif
void textReset(void);
void textSetColor(byte f, byte b);
void textSetCursor(byte c);
void textSetDouble(bool x, bool y);


#pragma compile("f_text.c")


#endif
#endif // TEXT_H
