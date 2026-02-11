/*
 *	PS/2 Keyboard support for the F256K.
 *	Adapted from f256k.h helper library.
 */


#ifndef WITHOUT_KEYBOARD


#include "f256lib.h"


// PS/2 scan set 2 to ASCII lookup table (make codes only)
// Index = scancode, value = ASCII char (0 = no mapping)
const char keyboardScanToAscii[128] = {
	0,   0,   0,   0,   0,   0,   0,   0,   // 00-07
	0,   0,   0,   0,   0,   '\t',  '`', 0,   // 08-0F
	0,   0,   0,   0,   0,   'q', '1', 0,   // 10-17
	0,   0,   'z', 's', 'a', 'w', '2', 0,   // 18-1F
	0,   'c', 'x', 'd', 'e', '4', '3', 0,   // 20-27
	0,   ' ', 'v', 'f', 't', 'r', '5', 0,   // 28-2F
	0,   'n', 'b', 'h', 'g', 'y', '6', 0,   // 30-37
	0,   0,   'm', 'j', 'u', '7', '8', 0,   // 38-3F
	0,   ',', 'k', 'i', 'o', '0', '9', 0,   // 40-47
	0,   '.', '/', 'l', ';', 'p', '-', 0,   // 48-4F
	0,   0,  '\'', 0,   '[', '=', 0,   0,   // 50-57
	0,   0,  '\r', ']', 0,  '\\', 0,   0,   // 58-5F
	0,   0,   0,   0,   0,   0,   '\b',0,   // 60-67
	0,   '1', 0,   '4', '7', 0,   0,   0,   // 68-6F
	'0', '.', '2', '5', '6', '8', 27,  0,   // 70-77
	0,   '+', '3', '-', '*', '9', 0,   0    // 78-7F
};


const char keyboardScanToAsciiShift[128] = {
	0,   0,   0,   0,   0,   0,   0,   0,   // 00-07
	0,   0,   0,   0,   0,   0,   '~', 0,   // 08-0F
	0,   0,   0,   0,   0,   'Q', '!', 0,   // 10-17
	0,   0,   'Z', 'S', 'A', 'W', '@', 0,   // 18-1F
	0,   'C', 'X', 'D', 'E', '$', '#', 0,   // 20-27
	0,   ' ', 'V', 'F', 'T', 'R', '%', 0,   // 28-2F
	0,   'N', 'B', 'H', 'G', 'Y', '^', 0,   // 30-37
	0,   0,   'M', 'J', 'U', '&', '*', 0,   // 38-3F
	0,   '<', 'K', 'I', 'O', ')', '(', 0,   // 40-47
	0,   '>', '?', 'L', ':', 'P', '_', 0,   // 48-4F
	0,   0,  '"',  0,   '{', '+', 0,   0,   // 50-57
	0,   0,  '\r', '}', 0,   '|', 0,   0,   // 58-5F
	0,   0,   0,   0,   0,   0,   '\b',0,   // 60-67
	0,   0,   0,   0,   0,   0,   0,   0,   // 68-6F
	0,   0,   0,   0,   0,   0,   27,  0,   // 70-77
	0,   0,   0,   0,   0,   0,   0,   0    // 78-7F
};


char keyboardHit(void) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	char result = (PEEK(PS2_STAT) & PS2_KEMP) ? 0 : 1;
	POKE(MMU_IO_CTRL, mmu);
	return result;
}


byte keyboardGetScan(void) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	while (PEEK(PS2_STAT) & PS2_KEMP)
		;
	byte sc = PEEK(KBD_IN);
	POKE(MMU_IO_CTRL, mmu);
	return sc;
}


char keyboardGetChar(void) {
	static char shift_held = 0;
	static char extended = 0;

	for (;;) {
		byte sc = keyboardGetScan();

		// Extended key prefix
		if (sc == 0xE0) {
			extended = 1;
			continue;
		}

		// Break code prefix
		if (sc == 0xF0) {
			sc = keyboardGetScan();
			if (sc == 0x12 || sc == 0x59)
				shift_held = 0;
			extended = 0;
			continue;
		}

		// Shift press
		if (sc == 0x12 || sc == 0x59) {
			shift_held = 1;
			extended = 0;
			continue;
		}

		// Handle extended arrow keys
		if (extended) {
			extended = 0;
			switch (sc) {
			case 0x75: return KEY_UP;
			case 0x72: return KEY_DOWN;
			case 0x6B: return KEY_LEFT;
			case 0x74: return KEY_RIGHT;
			}
			continue;
		}

		// Normal key - look up ASCII
		if (sc < 128) {
			char c = shift_held ? keyboardScanToAsciiShift[sc] : keyboardScanToAscii[sc];
			if (c != 0)
				return c;
		}
	}
}


char keyboardGetCharAsync(void) {
	if (!keyboardHit())
		return 0;
	return keyboardGetChar();
}


#endif
