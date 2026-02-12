/*
 *	PS/2 Keyboard support for the F256K.
 *	Adapted from f256k.h helper library.
 */


#ifndef KEYBOARD_H
#define KEYBOARD_H
#ifndef WITHOUT_KEYBOARD


#include "f256lib.h"


#define KEY_UP     0x80
#define KEY_DOWN   0x81
#define KEY_LEFT   0x82
#define KEY_RIGHT  0x83


char     keyboardHit(void);
byte     keyboardGetScan(void);
char     keyboardGetChar(void);
char     keyboardGetCharAsync(void);


#pragma compile("f_keyboard.c")


#endif
#endif // KEYBOARD_H
