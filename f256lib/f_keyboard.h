/*
 *	Keyboard support for the F256 via f256-microkernel events.
 *	Adapted from f256k.h helper library.
 */


#ifndef KEYBOARD_H
#define KEYBOARD_H
#ifndef WITHOUT_KEYBOARD


#include "f256lib.h"


#define KEY_UP     0x10
#define KEY_DOWN   0x0E
#define KEY_LEFT   0x02
#define KEY_RIGHT  0x06


char     keyboardHit(void);
byte     keyboardGetScan(void);
char     keyboardGetChar(void);
char     keyboardGetCharAsync(void);


#pragma compile("f_keyboard.c")


#endif
#endif // KEYBOARD_H
