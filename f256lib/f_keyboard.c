/*
 *	Keyboard support for the F256 via kernel events.
 */


#ifndef WITHOUT_KEYBOARD


#include "f256lib.h"


// One-character lookahead buffer for keyboardHit()/keyboardGetChar() pairing
static char _kbd_buf;
static byte _kbd_has_buf;


// Map a kernel key event to a character.  Returns 0 if not a usable keypress.
static char _kbd_map_event(void) {
	if (kernelEventData.type != kernelEvent(key.PRESSED))
		return 0;

	char c = kernelEventData.u.key.ascii;
	if (c) {
		// For printable keys, skip if modifier flags are set
		if (kernelEventData.u.key.flags)
			return 0;
		return c;
	}

	// Arrow keys have no ASCII — map from raw scancode
	// (checked regardless of flags since arrows are extended PS/2 keys)
	switch (kernelEventData.u.key.raw) {
	case 0x75: return KEY_UP;
	case 0x72: return KEY_DOWN;
	case 0x6B: return KEY_LEFT;
	case 0x74: return KEY_RIGHT;
	}
	return 0;
}


char keyboardHit(void) {
	if (_kbd_has_buf) return 1;

	kernelNextEvent();
	if (kernelError) return 0;

	char c = _kbd_map_event();
	if (c) {
		_kbd_buf = c;
		_kbd_has_buf = 1;
		return 1;
	}
	return 0;
}


byte keyboardGetScan(void) {
	while (1) {
		kernelNextEvent();
		if (kernelError) {
			kernelCall(Yield);
			continue;
		}
		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;
		return kernelEventData.u.key.raw;
	}
}


char keyboardGetChar(void) {
	if (_kbd_has_buf) {
		_kbd_has_buf = 0;
		return _kbd_buf;
	}
	while (1) {
		kernelNextEvent();
		if (kernelError) {
			kernelCall(Yield);
			continue;
		}
		char c = _kbd_map_event();
		if (c) return c;
	}
}


char keyboardGetCharAsync(void) {
	if (_kbd_has_buf) {
		_kbd_has_buf = 0;
		return _kbd_buf;
	}
	kernelNextEvent();
	if (kernelError) return 0;
	return _kbd_map_event();
}


#endif
