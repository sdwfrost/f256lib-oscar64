#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "f256lib.h"
#include <string.h>

extern char tokens[4][21];
extern char ntokens;

enum Word
{
	VERB_MOVE,
	VERB_TAKE,
	VERB_DROP,
	VERB_USE,
	VERB_LOOK,

	NOUN_FLOWER,
	NOUN_INVENTORY,

	NOUN_NORTH,
	NOUN_SOUTH,
	NOUN_WEST,
	NOUN_EAST,
	NOUN_NONE,

	PREP_ON,
	PREP_WITH,
	PREP_IN,
	PREP_INTO,
	PREP_FROM,
};

#define NOUN_FIRST NOUN_FLOWER
#define PREP_FIRST PREP_ON

extern Word verb, nouns[2], prep;

void read_tokens(void);
bool parse_tokens(void);

#define ITF_CARRY 0x01

struct Item
{
	Word noun;
	const char *name;
	const char *description;
	char flags;
	Item *next;
};

struct Location
{
	const char *name;
	const char *description;
	Location *directions[4];
	Item *items;
};

enum Direction
{
	DIR_NORTH,
	DIR_SOUTH,
	DIR_WEST,
	DIR_EAST
};

extern Location *loc;
extern Item *inventory;

void move(Direction dir);
Item *item_find(Word w);
void item_pickup(Item *item);
void item_drop(Item *item);

#pragma compile("adventure.c")

#endif
