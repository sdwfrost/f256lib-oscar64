// 1000 BorderColor - Cycle border color
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();
	textPrint("BORDER COLOR CYCLING\nPRESS ANY KEY TO STOP\n");

	byte c = 0;
	while (!keyboardHit())
	{
		graphicsSetBorderC64Color(c & 15);
		graphicsPause(10);
		c++;
	}

	graphicsSetBorderC64Color(0);
	return 0;
}
