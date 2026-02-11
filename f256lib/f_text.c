/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_TEXT


#include "f256lib.h"


colorT textColors[16] = {
	{ 0x00, 0x00, 0x00 },  // 0 Black
	{ 0xdd, 0x00, 0x33 },  // 1 Deep Red
	{ 0x00, 0x00, 0x99 },  // 2 Dark Blue
	{ 0xdd, 0x22, 0xdd },  // 3 Purple
	{ 0x00, 0x77, 0x22 },  // 4 Dark Green
	{ 0x55, 0x55, 0x55 },  // 5 Dark Gray
	{ 0x22, 0x22, 0xff },  // 6 Medium Blue
	{ 0x66, 0xaa, 0xff },  // 7 Light Blue
	{ 0x88, 0x55, 0x00 },  // 8 Brown
	{ 0xff, 0x66, 0x00 },  // 9 Orange
	{ 0xaa, 0xaa, 0xaa },  // A Light Gray
	{ 0xff, 0x99, 0x88 },  // B Pink
	{ 0x00, 0xdd, 0x00 },  // C Light Green
	{ 0xff, 0xff, 0x00 },  // D Yellow
	{ 0x55, 0xff, 0x99 },  // E Aquamarine
	{ 0xff, 0xff, 0xff }   // F White
};


static byte  _MAX_COL = 80;
static byte  _MAX_ROW = 30;
static byte  _row     = 0;
static byte  _col     = 0;
static byte  _fcolor  = 15;
static byte  _bcolor  = 0;
static byte  _ccolor  = 240;


void textClear(void) {
	byte           mmu   = PEEK(MMU_IO_CTRL);
	int16_t        i;
	int16_t        count = mathUnsignedMultiply(_MAX_COL, _MAX_ROW);
	volatile byte *vram  = (byte *)TEXT_MATRIX;

	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (i=0; i<count; i++) *vram++ = 32;

	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	vram = (byte *)TEXT_MATRIX;
	for (i=0; i<count; i++) *vram++ = _ccolor;

	POKE(MMU_IO_CTRL, mmu);

	textGotoXY(0, 0);
}


void textDefineBackgroundColor(byte slot, byte r, byte g, byte b) {
	byte *write;

	write = (byte *)VKY_TXT_BGLUT + mathUnsignedMultiply(slot, 4);
	*write++ = b;
	*write++ = g;
	*write++ = r;
	*write++ = 0xff;
}


void textDefineForegroundColor(byte slot, byte r, byte g, byte b) {
	byte *write;

	write = (byte *)VKY_TXT_FGLUT + mathUnsignedMultiply(slot, 4);
	*write++ = b;
	*write++ = g;
	*write++ = r;
	*write++ = 0xff;
}


void textEnableBackgroundColors(bool b) {
	POKE(VKY_MSTR_CTRL_1, (PEEK(VKY_MSTR_CTRL_1) & 0xef) | (b << 4));
}


void textGetXY(byte *x, byte *y) {
	*x = _col;
	*y = _row;
}


void textGotoXY(byte x, byte y) {
	_col = x;
	POKE(VKY_CRSR_X_L, _col);
	POKE(VKY_CRSR_X_H, 0);

	_row = y;
	POKE(VKY_CRSR_Y_L, _row);
	POKE(VKY_CRSR_Y_H, 0);
}


void textPrint(const char *message) {
	int16_t  x      = 0;
	int16_t  i      = 0;
	int16_t  j      = 0;
	int16_t  m      = 0;
	byte     mmu    = PEEK(MMU_IO_CTRL);
	volatile byte *vram = (byte *)mathUnsignedAddition(TEXT_MATRIX, mathUnsignedMultiply(_MAX_COL, _row));
	volatile byte *save = 0;

	while (message[x] != 0) {
		switch (message[x]) {
			default:
				POKE(MMU_IO_CTRL, MMU_IO_COLOR);
				vram[_col] = _ccolor;
				POKE(MMU_IO_CTRL, MMU_IO_TEXT);
				vram[_col] = message[x];
				_col++;
				if (_col != _MAX_COL) break;
				// Fall through.
			case 10:
			case 13:
				_col = 0;
				_row++;
				if (_row == _MAX_ROW) {
					vram = (byte *)TEXT_MATRIX;
					m = _MAX_COL * (_MAX_ROW - 1);
					POKE(MMU_IO_CTRL, MMU_IO_COLOR);
					for (j=0; j<2; j++) {
						for (i = 0; i < m; i++) {
							vram[i] = vram[i + _MAX_COL];
						}
						POKE(MMU_IO_CTRL, MMU_IO_TEXT);
					}
					vram += i;
					save = vram;
					POKE(MMU_IO_CTRL, MMU_IO_COLOR);
					for (i = 0; i < _MAX_COL; i++) *vram++ = _ccolor;
					POKE(MMU_IO_CTRL, MMU_IO_TEXT);
					vram = save;
					for (i = 0; i < _MAX_COL; i++) *vram++ = 32;
					_row--;
					vram = (byte *)mathUnsignedAddition(TEXT_MATRIX, (_MAX_ROW - 1));
				} else {
					vram += _MAX_COL;
				}
				break;
		}
		x++;
	}

	POKE(MMU_IO_CTRL, mmu);

	textGotoXY(_col, _row);
}


void textPrintInt(int32_t value){
	if (value < 0) {
		textPrint("-");
		value = -value;
	}
	textPrintUInt(value);
}


void textPrintUInt(uint32_t value){
	char c[2];

	if (value > 9) {
		if (value > 65535) {
			textPrintUInt(value / 10);
		} else {
			textPrintUInt(mathUnsignedDivision(value, 10));
		}
	}

	c[0] = '0' + (value % 10);
	c[1] = 0;
	textPrint(c);
}


void textScrollUp(void) {
	volatile byte *vram = (byte *)TEXT_MATRIX;
	uint16_t       total = (uint16_t)_MAX_COL * (uint16_t)_MAX_ROW;
	uint16_t       copy_count = total - _MAX_COL;
	uint16_t       i;

	// Scroll text matrix
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (i = 0; i < copy_count; i++)
		vram[i] = vram[i + _MAX_COL];
	for (i = copy_count; i < total; i++)
		vram[i] = 32;

	// Scroll color matrix
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	vram = (byte *)TEXT_MATRIX;
	for (i = 0; i < copy_count; i++)
		vram[i] = vram[i + _MAX_COL];
	for (i = copy_count; i < total; i++)
		vram[i] = _ccolor;

	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
}


static void textAdvanceCursor(void) {
	_col++;
	if (_col >= _MAX_COL) {
		_col = 0;
		_row++;
		if (_row >= _MAX_ROW) {
			textScrollUp();
			_row = _MAX_ROW - 1;
		}
	}
}


static void textNewline(void) {
	_col = 0;
	_row++;
	if (_row >= _MAX_ROW) {
		textScrollUp();
		_row = _MAX_ROW - 1;
	}
}


void textPutChar(char c) {
	if (c == 10 || c == 13) {
		textNewline();
	} else {
		volatile byte *vram = (byte *)TEXT_MATRIX;
		uint16_t       pos = (uint16_t)_row * (uint16_t)_MAX_COL + _col;

		POKE(MMU_IO_CTRL, MMU_IO_COLOR);
		vram[pos] = _ccolor;
		POKE(MMU_IO_CTRL, MMU_IO_TEXT);
		vram[pos] = (byte)c;
		POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

		textAdvanceCursor();
	}
	// Update hardware cursor
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	POKE(VKY_CRSR_X_L, _col);
	POKE(VKY_CRSR_X_H, 0);
	POKE(VKY_CRSR_Y_L, _row);
	POKE(VKY_CRSR_Y_H, 0);
	POKE(MMU_IO_CTRL, mmu);
}


void textPrintHex(uint32_t value, byte digits) {
	byte i;
	for (i = digits; i > 0; i--) {
		byte nibble = (byte)((value >> ((i - 1) * 4)) & 0x0f);
		if (nibble < 10)
			textPutChar('0' + nibble);
		else
			textPutChar('a' + nibble - 10);
	}
}


void textPrintFloat(float val, byte decimals) {
	if (val < 0.0) {
		textPutChar('-');
		val = -val;
	}

	uint32_t ipart = (uint32_t)val;
	textPrintUInt(ipart);

	if (decimals > 0) {
		textPutChar('.');

		float frac = val - (float)ipart;
		byte  d;
		for (d = 0; d < decimals; d++) {
			frac *= 10.0;
			byte digit = (byte)frac;
			textPutChar('0' + digit);
			frac -= (float)digit;
		}
	}
}


#ifndef WITHOUT_KEYBOARD
void textReadLine(char *buf, byte maxlen) {
	byte i = 0;
	for (;;) {
		char ch = keyboardGetChar();
		if (ch == '\r' || ch == '\n') {
			buf[i] = 0;
			textPutChar('\n');
			return;
		} else if (ch == '\b') {
			if (i > 0) {
				i--;
				_col--;
				textPutChar(' ');
				_col--;
				byte mmu = PEEK(MMU_IO_CTRL);
				POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
				POKE(VKY_CRSR_X_L, _col);
				POKE(MMU_IO_CTRL, mmu);
			}
		} else if (ch >= ' ' && i < maxlen) {
			if (ch >= 'a' && ch <= 'z')
				ch = ch - 'a' + 'A';
			buf[i++] = ch;
			textPutChar(ch);
		}
	}
}


char textReadInt(int *result) {
	char buf[12];
	textReadLine(buf, 11);

	int  val = 0;
	char neg = 0;
	char i = 0;

	if (buf[0] == '-') { neg = 1; i = 1; }
	if (buf[i] == 0) return 0;

	while (buf[i]) {
		if (buf[i] < '0' || buf[i] > '9') return 0;
		val = val * 10 + (buf[i] - '0');
		i++;
	}

	*result = neg ? -val : val;
	return 1;
}
#endif


void textReset(void) {
	byte  x;

	_fcolor  = 15;
	_bcolor  = 0;
	_ccolor  = 240;

	textSetDouble(false, true);
	textSetCursor(0);

	for (x=0; x<TEXTCOLORS_COUNT; x++) {
		textDefineForegroundColor(x, textColors[x].r, textColors[x].g, textColors[x].b);
		textDefineBackgroundColor(x, textColors[x].r, textColors[x].g, textColors[x].b);
	}

	textClear();
}


void textSetColor(byte f, byte b) {
	_fcolor = f;
	_bcolor = b;
	_ccolor = (f << 4) + b;
}


void textSetCursor(byte c) {
	if (c == 0) {
		POKE(VKY_CRSR_CTRL, 0);
	} else {
		POKE(VKY_CRSR_CTRL, 3);
		POKE(VKY_CRSR_CHAR, c);
	}
}


void textSetDouble(bool x, bool y) {
	POKE(VKY_MSTR_CTRL_1, (PEEK(VKY_MSTR_CTRL_1) & 0xf9) | (x << 1) | (y << 2));

	_MAX_COL = x ? 40 : 80;
	_MAX_ROW = y ? 30 : 60;
}


#endif
