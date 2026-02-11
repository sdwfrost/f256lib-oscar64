/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef SPRITE_H
#define SPRITE_H
#ifndef WITHOUT_SPRITE


#include "f256lib.h"


void spriteDefine(byte s, uint32_t address, byte size, byte CLUT, byte layer);
void spriteSetPosition(byte s, uint16_t x, uint16_t y);
void spriteSetVisible(byte s, bool v);
void spriteReset(void);


#pragma compile("f_sprite.c")


#endif
#endif // SPRITE_H
