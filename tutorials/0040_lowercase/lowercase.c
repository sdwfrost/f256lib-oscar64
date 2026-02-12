// 0040 Lowercase - Display lowercase characters
// Ported from OscarTutorials to F256K using f256lib
//
// The F256K natively supports lowercase (no ROM switch needed).

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	textPrint("Hello World\n");
	textPrint("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	textPrint("abcdefghijklmnopqrstuvwxyz\n");

	return 0;
}
