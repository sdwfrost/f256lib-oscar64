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


// Sprite convenience layer
// ------------------------

// Sprite data address in system bus memory (above CPU space)
#define SPR_DATA_BASE    0x10000UL
#define SPR_IMG_SIZE     576       // 24x24 @ 8bpp
#define SPR_MAX_IMAGES   16

// TinyVicky sprite coordinates: visible area starts at (32,32)
#define SPR_OFFSET_X     32
#define SPR_OFFSET_Y     32

void spriteInitClut(void);
byte spriteExpand(const char *src, byte slot, byte color);
void spriteInit(void);
void spriteSet(byte sp, bool show, int xpos, int ypos, byte image, byte color);
void spriteMove(byte sp, int xpos, int ypos);
void spriteSetImage(byte sp, byte image);
void spriteRecolor(byte sp, const char *src, byte newColor);
void spriteShow(byte sp, bool show);
byte spriteCheckCollisions(void);


#pragma compile("f_sprite.c")


#endif
#endif // SPRITE_H
