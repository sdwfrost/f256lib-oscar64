// 0030 KeyWait - Wait for keypress
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	textPrint("PRESS A KEY\n");

	char c = keyboardGetChar();

	textPrint("YOU PRESSED: ");
	textPutChar(c);
	textPutChar('\n');

	return 0;
}
