// 3000 AdventureTokens - Text tokenization
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <string.h>

char tokens[4][21];
char ntokens;

void read_tokens(void)
{
	static char line[81];
	ntokens = 0;
	while (ntokens == 0)
	{
		textPrint("> ");
		textReadLine(line, 80);

		int i = 0;
		int pos = 0;
		int len = strlen(line);
		while (pos < len && ntokens < 4)
		{
			if (line[pos] == ' ')
			{
				if (i != 0)
				{
					tokens[ntokens][i] = 0;
					ntokens++;
					i = 0;
				}
			}
			else if (i < 20)
			{
				tokens[ntokens][i++] = line[pos];
			}
			pos++;
		}
		if (i > 0)
		{
			tokens[ntokens][i] = 0;
			ntokens++;
		}
	}
}

int main(int argc, char *argv[])
{
	textClear();

	for (;;)
	{
		read_tokens();

		textSetColor(YELLOW, BLACK);
		for (char i = 0; i < ntokens; i++)
		{
			textPrint(tokens[i]);
			textPutChar('\n');
		}
		textSetColor(WHITE, BLACK);
	}

	return 0;
}
