// 0050 Salutation - Read a name and greet the user
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();

	textPrint("What is your name: ");
	char name[41];
	textReadLine(name, 40);

	textPrint("Hello ");
	textPrint(name);
	textPrint("!\n");

	return 0;
}
