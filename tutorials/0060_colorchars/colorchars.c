// 0060 ColorChars - Colored text output
// Ported from OscarTutorials to F256K using f256lib
//
// Displays text in all 16 colors from the text palette.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	for (byte i = 0; i < 16; i++)
	{
		textSetColor(i, BLACK);
		textPrint("COLOR ");
		textPrintInt(i);
		textPutChar('\n');
	}

	textSetColor(WHITE, BLACK);
	return 0;
}
