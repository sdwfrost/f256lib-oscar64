#include "adventure.h"

char tokens[4][21];
char ntokens;
Word verb, nouns[2], prep;

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
				tokens[ntokens][i++] = line[pos];
			pos++;
		}
		if (i > 0)
		{
			tokens[ntokens][i] = 0;
			ntokens++;
		}
	}
}

static inline bool isverb(Word w) { return w < NOUN_FIRST; }
static inline bool isnoun(Word w) { return w >= NOUN_FIRST && w < PREP_FIRST; }

struct Vocab
{
	Word word;
	const char *text;
	Word verb;
};

const Vocab vocabulary[] = {
	{VERB_MOVE, "MOVE"},
	{VERB_TAKE, "TAKE"},
	{VERB_DROP, "DROP"},
	{VERB_USE,  "USE"},
	{VERB_LOOK, "LOOK"}, {VERB_LOOK, "L"},
	{VERB_OPEN, "OPEN"},

	{NOUN_FLOWER, "FLOWER", VERB_USE},
	{NOUN_DOOR, "DOOR", VERB_OPEN},
	{NOUN_KEY, "KEY", VERB_USE},
	{NOUN_INVENTORY, "INVENTORY", VERB_LOOK}, {NOUN_INVENTORY, "I", VERB_LOOK},

	{NOUN_WEST,  "WEST",  VERB_MOVE}, {NOUN_WEST, "W", VERB_MOVE},
	{NOUN_EAST,  "EAST",  VERB_MOVE}, {NOUN_EAST, "E", VERB_MOVE},
	{NOUN_NORTH, "NORTH", VERB_MOVE}, {NOUN_NORTH, "N", VERB_MOVE},
	{NOUN_SOUTH, "SOUTH", VERB_MOVE}, {NOUN_SOUTH, "S", VERB_MOVE},

	{PREP_ON,   "ON"},
	{PREP_WITH, "WITH"},
	{PREP_IN,   "IN"},
	{PREP_FROM, "FROM"},
	{PREP_INTO, "INTO"}
};

static const int VOCSIZE = sizeof(vocabulary) / sizeof(Vocab);

int vocab_find(const char *token)
{
	for (int i = 0; i < VOCSIZE; i++)
		if (!strcmp(vocabulary[i].text, token))
			return i;

	textPrint("UNKNOWN WORD <");
	textPrint(token);
	textPrint(">\n");
	return -1;
}

bool parse_tokens(void)
{
	verb = NOUN_NONE;
	prep = NOUN_NONE;
	nouns[0] = NOUN_NONE;
	nouns[1] = NOUN_NONE;

	for (int i = 0; i < ntokens; i++)
	{
		int k = vocab_find(tokens[i]);
		if (k < 0) return false;

		if (isverb(vocabulary[k].word))
		{
			if (verb != NOUN_NONE)
			{
				textPrint("TOO MANY VERBS\n");
				return false;
			}
			verb = vocabulary[k].word;
		}
		else if (isnoun(vocabulary[k].word))
		{
			if (nouns[1] != NOUN_NONE)
			{
				textPrint("TOO MANY NOUNS\n");
				return false;
			}
			if (nouns[0] != NOUN_NONE)
				nouns[1] = vocabulary[k].word;
			else
			{
				if (verb == NOUN_NONE)
					verb = vocabulary[k].verb;
				nouns[0] = vocabulary[k].word;
			}
		}
		else
			prep = vocabulary[k].word;
	}

	return verb != NOUN_NONE;
}

Location *loc;
Item *inventory;

void move(Direction dir)
{
	if (loc->directions[dir].location)
	{
		if (loc->directions[dir].condition && !*(loc->directions[dir].condition))
			textPrint("THE WAY IS BLOCKED\n");
		else
			loc = loc->directions[dir].location;
	}
	else
		textPrint("YOU CAN'T MOVE THERE\n");
}

Item *item_find(Word w)
{
	Item *i = loc->items;
	while (i)
	{
		if (i->noun == w) return i;
		i = i->next;
	}
	i = inventory;
	while (i)
	{
		if (i->noun == w) return i;
		i = i->next;
	}
	return 0;
}

Item *item_object(void) { return item_find(nouns[0]); }

Item *item_with(void)
{
	if (nouns[1] != NOUN_NONE)
	{
		Item *i = inventory;
		while (i)
		{
			if (i->noun == nouns[1]) return i;
			i = i->next;
		}
		textPrint("YOU DON'T HAVE IT\n");
	}
	return 0;
}

bool item_remove(Item **items, Item *item)
{
	Item *i = *items;
	if (i == item)
	{
		*items = i->next;
		return true;
	}
	while (i->next)
	{
		if (i->next == item)
		{
			i->next = item->next;
			return true;
		}
		i = i->next;
	}
	return false;
}

void item_pickup(Item *item)
{
	if (!item)
		textPrint("CAN'T FIND IT\n");
	else if (!(item->flags & ITF_CARRY))
		textPrint("YOU CANNOT PICK THIS UP\n");
	else if (item_remove(&(loc->items), item))
	{
		item->next = inventory;
		inventory = item;
	}
	else
		textPrint("IT IS NOT HERE\n");
}

void item_drop(Item *item)
{
	if (!item)
		textPrint("CAN'T FIND IT\n");
	else if (item_remove(&inventory, item))
	{
		item->next = loc->items;
		loc->items = item;
	}
	else
		textPrint("YOU DON'T HAVE THIS\n");
}

void item_open(Item *item, Item *with)
{
	if (!item)
		textPrint("CAN'T FIND IT\n");
	else if (item->flags & ITF_OPEN)
	{
		if (item->key && item->key != with)
			textPrint("IT APPEARS TO BE LOCKED\n");
		else
			item->open = true;
	}
	else
		textPrint("YOU CAN'T OPEN IT\n");
}
