// 3010 AdventureParse - Token parsing with vocabulary
// Ported from OscarTutorials to F256K using f256lib

#include "adventure.h"

void execute(void)
{
	switch (verb)
	{
	case VERB_MOVE:
		switch (nouns[0])
		{
		case NOUN_NORTH: textPrint("MOVING NORTH\n"); break;
		case NOUN_WEST:  textPrint("MOVING WEST\n"); break;
		case NOUN_SOUTH: textPrint("MOVING SOUTH\n"); break;
		case NOUN_EAST:  textPrint("MOVING EAST\n"); break;
		default: textPrint("YOU CAN'T MOVE THERE\n");
		}
		break;
	case VERB_LOOK:
		textPrint("YOU SEE NOTHING SPECIAL\n");
		break;
	default:
		textPrint("I DON'T UNDERSTAND\n");
	}
}

int main(int argc, char *argv[])
{
	textClear();

	for (;;)
	{
		textSetColor(WHITE, BLACK);
		textPrint("ENTER COMMAND:\n");
		textSetColor(LIGHT_BLUE, BLACK);
		read_tokens();
		textSetColor(WHITE, BLACK);
		if (parse_tokens())
			execute();
	}

	return 0;
}
