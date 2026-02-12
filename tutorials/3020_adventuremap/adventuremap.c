// 3020 AdventureMap - Map navigation
// Ported from OscarTutorials to F256K using f256lib

#include "adventure.h"

extern Location loc_building;

Location loc_street = {
	"ON A STREET",
	"YOU ARE ON AN OPEN STREET, THERE IS A\n"
	"BUILDING TO THE NORTH",
	{&loc_building}
};

Location loc_building = {
	"IN A BUILDING",
	"YOU ARE IN AN OFFICE BUILDING, THERE IS\n"
	"A STAIRCASE TO THE WEST, AND AN EXIT TO\n"
	"THE SOUTH",
	{0, &loc_street}
};

void execute(void)
{
	switch (verb)
	{
	case VERB_MOVE:
		switch (nouns[0])
		{
		case NOUN_NORTH: move(DIR_NORTH); break;
		case NOUN_WEST:  move(DIR_WEST); break;
		case NOUN_SOUTH: move(DIR_SOUTH); break;
		case NOUN_EAST:  move(DIR_EAST); break;
		default: textPrint("YOU CAN'T MOVE THERE\n");
		}
		break;
	case VERB_LOOK:
		textPrint(loc->description);
		textPutChar('\n');
		break;
	}
}

int main(int argc, char *argv[])
{
	loc = &loc_street;

	for (;;)
	{
		textSetColor(WHITE, BLACK);
		textPrint("YOU ARE ");
		textPrint(loc->name);
		textPutChar('\n');
		textSetColor(LIGHT_BLUE, BLACK);
		read_tokens();
		textSetColor(WHITE, BLACK);
		if (parse_tokens())
			execute();
	}

	return 0;
}
