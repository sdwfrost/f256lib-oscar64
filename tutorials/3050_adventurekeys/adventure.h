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
	VERB_OPEN,

	NOUN_FLOWER,
	NOUN_DOOR,
	NOUN_KEY,
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
#define ITF_OPEN  0x02

struct Item
{
	Word noun;
	const char *name;
	const char *description;
	char flags;
	bool open;
	Item *key;
	Item *next;
};

struct Location;

struct Passage
{
	Location *location;
	bool *condition;
};

struct Location
{
	const char *name;
	const char *description;
	Passage directions[4];
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
Item *item_object(void);
Item *item_with(void);
void item_pickup(Item *item);
void item_drop(Item *item);
void item_open(Item *item, Item *with);

#pragma compile("adventure.c")

#endif
