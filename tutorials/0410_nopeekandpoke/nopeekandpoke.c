// 0410 NoPeekAndPoke - Screen access without direct PEEK/POKE
// Ported from OscarTutorials to F256K using f256lib
//
// Same result as 0400 but using text library functions.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	textSetColor(YELLOW, BLACK);
	textGotoXY(0, 0);
	textPrint("HELLO");

	textSetColor(WHITE, BLACK);
	textGotoXY(0, 2);
	textPrint("Written using text library!\n");
	textPrint("No direct PEEK/POKE needed.\n");

	return 0;
}
