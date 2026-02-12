// 0070 Guessing - Number guessing game
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
	textClear();

	char check;

	do {
		int dice = (randomRead() % 6) + 1;
		int guess;

		do {
			textPrint("\nInput your guess from 1 to 6: ");
		} while (!textReadInt(&guess) || guess < 1 || guess > 6);

		if (guess == dice)
			textPrint("\nYou guessed right!\n");
		else {
			textPrint("\nSorry, wrong number, the dice rolled ");
			textPrintInt(dice);
			textPutChar('\n');
		}

		textPrint("\n\nTry again (y/n): ");
		do {
			check = keyboardGetChar();
		} while (check != 'y' && check != 'n');
		textPutChar(check);
		textPutChar('\n');

	} while (check == 'y');

	return 0;
}
