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

	// Check ASCII field for printable keys (flags < 0 means no ASCII available)
	char c = kernelEventData.u.key.ascii;
	if (c && !kernelEventData.u.key.flags)
		return c;

	// Map f256-microkernel raw key codes for non-ASCII or flagged keys.
	// These values are defined by the microkernel (hardware/keys.asm),
	// not raw PS/2 scancodes.
	switch (kernelEventData.u.key.raw) {
	case 0xB6: return KEY_UP;
	case 0xB7: return KEY_DOWN;
	case 0xB8: return KEY_LEFT;
	case 0xB9: return KEY_RIGHT;
	case 0x94: return 13;   // ENTER
	case 0x95: return 27;   // ESC
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
