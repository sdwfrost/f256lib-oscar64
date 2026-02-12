// 0020 Diagonals - Diagonal text pattern
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	for (byte y = 0; y < 25; y++)
	{
		textGotoXY(y, y);
		textPrint("HELLO WORLD");
	}

	return 0;
}
