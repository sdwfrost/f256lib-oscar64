// 0200 CursorMove - Move cursor with arrow keys
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();
	textPrint("USE ARROW KEYS TO MOVE, Q TO QUIT\n");

	byte x = 40, y = 15;
	textGotoXY(x, y);
	textPutChar('*');

	for (;;)
	{
		char c = keyboardGetChar();

		// Erase old position
		textGotoXY(x, y);
		textPutChar(' ');

		switch (c) {
		case KEY_UP:    if (y > 0) y--; break;
		case KEY_DOWN:  if (y < 29) y++; break;
		case KEY_LEFT:  if (x > 0) x--; break;
		case KEY_RIGHT: if (x < 79) x++; break;
		case 'q': return 0;
		}

		// Draw new position
		textGotoXY(x, y);
		textPutChar('*');
	}

	return 0;
}
