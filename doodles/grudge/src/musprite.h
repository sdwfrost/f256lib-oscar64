#ifndef MUSPRITE_H
#define MUSPRITE_H

/*
 * Sprite animation state management for the grudge doodle.
 * Ported from F256KsimpleCdoodles/grudge/src/musprite.h
 * Uses f256lib sprite functions (spriteDefine, spriteSetPosition, etc.)
 */

#include "f256lib.h"

#define SPR_BOY1_BASE    0x10400 //sprite 0
#define SPR_THNG_BASE    0x10800 //sprite 1
#define SPR_GLR1_BASE    0x10C00 //sprite 2
#define SPR_CATH_BASE    0x11000 //sprite 3
#define SPR_CAT_BASE     0x11400 //sprite 4

#define SPR_DASH_BASE    0x13000

#define SPR_COUNT 10
#define SPR_STATE_COUNT 6
#define SPR_STATE_IDLE_R 0
#define SPR_STATE_IDLE_L 1
#define SPR_STATE_WALK_R 2
#define SPR_STATE_WALK_L 3
#define SPR_STATE_DASH_R 4
#define SPR_STATE_DASH_L 5

#define SPR_SIZE  16
#define SPR_SIZE_SQUARED  0x100


typedef struct sprStatus
{
	uint16_t x,y; //position
	bool rightOrLeft; //last facing
	bool isDashing;
	uint32_t addr; //base address
	uint8_t frame; //frame into view
	uint16_t sx, sy; //speed
	struct timer_t timer; //animation timer
	uint8_t cookie; //cookie for timer
	uint8_t state; //which state: 0 idle, 1 walk right, 2 walk left, etc
	uint8_t *minIndexForState; //minimum index for given state
	uint8_t *maxIndexForState; //maximum index for given state
} sprStatus;

void mySpriteDefine(uint8_t, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t, bool, uint8_t);
void mySetSpriteAddr(uint8_t, uint32_t);

extern sprStatus spriteStatuses[]; //hold characters sprite information
extern uint8_t stateDelays[]; //delays between each frame for each state
extern uint8_t dashDelay; //delay between dash frames

#pragma compile("musprite.c")

#endif // MUSPRITE_H
