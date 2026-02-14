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


// --- Extended text functions (ported from F256-terminal/F256-FileManager) ---

// Foenix character set line-drawing constants
#define CH_LINE_HORI      150  // horizontal line (W-E)
#define CH_LINE_VERT      130  // vertical line (N-S)
#define CH_CORNER_ES      160  // top-left corner (square)
#define CH_CORNER_WS      161  // top-right corner (square)
#define CH_CORNER_NE      162  // bottom-left corner (square)
#define CH_CORNER_WN      163  // bottom-right corner (square)
#define CH_CORNER_RND_ES  188  // top-left corner (rounded)
#define CH_CORNER_RND_WS  189  // top-right corner (rounded)
#define CH_CORNER_RND_NE  190  // bottom-left corner (rounded)
#define CH_CORNER_RND_WN  191  // bottom-right corner (rounded)
#define CH_INTERSECT_WNES 156  // four-way intersection
#define CH_TEE_NES        154  // T-piece: left wall
#define CH_TEE_WES        155  // T-piece: top wall
#define CH_TEE_WNE        157  // T-piece: bottom wall
#define CH_TEE_WNS        158  // T-piece: right wall

// Draw modes for line/fill functions
#define TEXT_DRAW_CHAR_ONLY     0
#define TEXT_DRAW_ATTR_ONLY     1
#define TEXT_DRAW_CHAR_AND_ATTR 2

// Copy direction for textCopyBox
#define TEXT_COPY_TO_SCREEN     true
#define TEXT_COPY_FROM_SCREEN   false

// Direct cell access (position-based, no cursor movement)
void textSetCharAt(byte x, byte y, char c);
void textSetAttrAt(byte x, byte y, byte attr);
void textSetCharAndAttrAt(byte x, byte y, char c, byte attr);
char textGetCharAt(byte x, byte y);

// Rectangle operations
void textFillBox(byte x1, byte y1, byte x2, byte y2, char c, byte fore, byte back);
void textFillBoxAttr(byte x1, byte y1, byte x2, byte y2, byte fore, byte back);
void textInvertBox(byte x1, byte y1, byte x2, byte y2);

// Line drawing (using Foenix line-draw characters)
void textDrawHLine(byte x, byte y, byte width, byte the_char, byte fore, byte back, byte draw_choice);
void textDrawVLine(byte x, byte y, byte height, byte the_char, byte fore, byte back, byte draw_choice);
void textDrawBox(byte x1, byte y1, byte x2, byte y2, byte fore, byte back);

// Positioned string drawing (no cursor movement, returns chars drawn)
byte textDrawStringAt(byte x, byte y, const char *s, byte fore, byte back);

// Memory copy operations (for screen save/restore, scrolling regions)
void textCopyBoxToBuffer(byte x1, byte y1, byte x2, byte y2, byte *char_buf, byte *attr_buf);
void textCopyBoxFromBuffer(byte x1, byte y1, byte x2, byte y2, const byte *char_buf, const byte *attr_buf);
void textScrollRowsUp(byte y1, byte y2);
void textScrollRowsDown(byte y1, byte y2);

// Font loading
void textLoadFont(const byte *data, uint16_t len, bool primary_slot);


#pragma compile("f_text.c")


#endif
#endif // TEXT_H
