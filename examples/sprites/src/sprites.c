/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */


#include "f256lib.h"


#define SPRITE_COUNT          4
#define SPRITE_SIZE           32
#define SPRITE_WIDTH          5
#define SPRITE_HEIGHT         2

#define SPR_LEFT              0
#define SPR_LEFT_ADDR         0x10000

#define SPR_LEFT_FRONT        1
#define SPR_LEFT_FRONT_ADDR   0x12800

#define SPR_RIGHT_FRONT       2
#define SPR_RIGHT_FRONT_ADDR  0x15000

#define SPR_RIGHT             3
#define SPR_RIGHT_ADDR        0x17800

#define SPR_CLUT              0x1a000
#define SPR_CLUT_COLORS       32

#define TURN_SPEED            25


// Embed sprite data at far memory address 0x10000.
// sprites.bin is 41088 bytes, spanning 0x10000..0x1a080.
#pragma section( sprdata, 0)
#pragma region( sprdata, 0x10000, 0x1b000, , , {sprdata} )
#pragma data(sprdata)
__export const char embedded[] = {
	#embed "../generated/sprites.bin"
};
#pragma data(data)


static uint32_t spriteStartAddrs[SPRITE_COUNT] = { SPR_LEFT_ADDR, SPR_LEFT_FRONT_ADDR, SPR_RIGHT_FRONT_ADDR, SPR_RIGHT_ADDR };
static uint32_t spriteAddrs[SPRITE_COUNT][SPRITE_WIDTH][SPRITE_HEIGHT];
static byte     spriteIds[SPRITE_COUNT][SPRITE_WIDTH][SPRITE_HEIGHT];
static byte     anyJoy;


void drawHelicopter(void) {
	static byte     last  = SPR_RIGHT;        // Last frame used, can be anything other than "frame".
	static byte     frame = SPR_RIGHT_FRONT;
	static byte     lag   = 0;
	static uint16_t xSize = (SPRITE_WIDTH * SPRITE_SIZE);
	static uint16_t ySize = (SPRITE_HEIGHT * SPRITE_SIZE);
	static uint16_t xPos  = 96;   // (352 / 2) - (xSize / 2);
	static uint16_t yPos  = 104;  // (272 / 2) - (ySize / 2);
	byte i;
	byte j;

	// New frame?
	if (last != frame) {
		for (j=0; j<SPRITE_HEIGHT; j++) {
			for (i=0; i<SPRITE_WIDTH; i++) {
				spriteSetPosition(spriteIds[frame][i][j], mathUnsignedAddition(xPos, mathUnsignedMultiply(i, 32)), mathUnsignedAddition(yPos, mathUnsignedMultiply(j, 32)));
				spriteSetVisible(spriteIds[frame][i][j], true);
				spriteSetVisible(spriteIds[last][i][j], false);
			}
		}
		last = frame;
	}

	// Move sprite.
	if (anyJoy != 0) {

		if (anyJoy & JOY_UP) {
			if (yPos > 32) yPos--;
		}

		if (anyJoy & JOY_DOWN) {
			if (yPos + ySize < 271) yPos++;
		}

		if (anyJoy & JOY_LEFT) {
			if (xPos > 32) xPos--;
			if (frame > SPR_LEFT) {
				if (lag == 0) {
					lag = TURN_SPEED;
				} else {
					lag--;
					if (lag == 0) frame--;
				}
			}
		}

		if (anyJoy & JOY_RIGHT) {
			if (xPos + xSize < 351) xPos++;
			if (frame < SPR_RIGHT) {
				if (lag == 0) {
					lag = TURN_SPEED;
				} else {
					lag--;
					if (lag == 0) frame++;
				}
			}
		}

		// Update sprite positions.
		for (j=0; j<SPRITE_HEIGHT; j++) {
			for (i=0; i<SPRITE_WIDTH; i++) {
				spriteSetPosition(spriteIds[frame][i][j], mathUnsignedAddition(xPos, mathUnsignedMultiply(i, 32)), mathUnsignedAddition(yPos, mathUnsignedMultiply(j, 32)));
			}
		}

	}  // (anyJoy != 0)

}


void getInput(void) {
	static byte oneJoy = 0;
	static byte twoJoy = 0;
	static byte keyJoy = 0;

	do {
		kernelCall(NextEvent);

		if (kernelEventData.type != 0) {
			textPrintInt(kernelEventData.type);
			textPrint(" ");
			textPrintInt(kernelEvent(key.PRESSED));
			textPrint(" ");
			textPrintInt(kernelEvent(key.RELEASED));
			textPrint("\n");
		}

		// Read real joysticks.
		if (kernelEventData.type == kernelEvent(GAME)) {
			oneJoy = kernelEventData.u.game.game0;
			twoJoy = kernelEventData.u.game.game1;
		}

		// Use keyboard as virtual joystick.
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.ascii) {
				case 'w':
				case 'W':
					keyJoy |= JOY_UP;
					break;
				case 'a':
				case 'A':
					keyJoy |= JOY_LEFT;
					break;
				case 's':
				case 'S':
					keyJoy |= JOY_DOWN;
					break;
				case 'd':
				case 'D':
					keyJoy |= JOY_RIGHT;
					break;
				case 'j':
				case 'J':
					keyJoy |= JOY_BUTTON_1;
					break;
				case 'k':
				case 'K':
					keyJoy |= JOY_BUTTON_2;
					break;
				case 'l':
				case 'L':
					keyJoy |= JOY_BUTTON_3;
					break;
			}
		}
		if (kernelEventData.type == kernelEvent(key.RELEASED)) {
			switch (kernelEventData.u.key.ascii) {
				case 'w':
				case 'W':
					keyJoy &= ~JOY_UP;
					break;
				case 'a':
				case 'A':
					keyJoy &= ~JOY_LEFT;
					break;
				case 's':
				case 'S':
					keyJoy &= ~JOY_DOWN;
					break;
				case 'd':
				case 'D':
					keyJoy &= ~JOY_RIGHT;
					break;
				case 'j':
				case 'J':
					keyJoy &= ~JOY_BUTTON_1;
					break;
				case 'k':
				case 'K':
					keyJoy &= ~JOY_BUTTON_2;
					break;
				case 'l':
				case 'L':
					keyJoy &= ~JOY_BUTTON_3;
					break;
			}
		}
	} while (kernelGetPending() > 0);

	// Merge inputs.  Yes, this allows dumb things like LEFT and RIGHT at the same time.
	anyJoy = oneJoy | twoJoy | keyJoy;
}


void setup(void) {
	byte     i;
	byte     j;
	byte     s;
	byte     r;
	byte     g;
	byte     b;
	uint32_t c;

	// Set up CLUT0.
	c = SPR_CLUT;
	for (i=0; i<SPR_CLUT_COLORS; i++) {
		b = FAR_PEEK(c++);
		g = FAR_PEEK(c++);
		r = FAR_PEEK(c++);
		c++;
		graphicsDefineColor(0, i, r, g, b);
	}

	// Sprite addresses and definitions.
	r = 0;
	for (s=0; s<SPRITE_COUNT; s++) {
		c = spriteStartAddrs[s];
		for (j=0; j<SPRITE_HEIGHT; j++) {
			for (i=0; i<SPRITE_WIDTH; i++) {
				spriteIds[s][i][j]   = r++;
				spriteAddrs[s][i][j] = c;
				c = mathUnsignedAddition(c, mathUnsignedMultiply(SPRITE_SIZE, SPRITE_SIZE));
				spriteDefine(spriteIds[s][i][j], spriteAddrs[s][i][j], SPRITE_SIZE, 0, 0);
				spriteSetVisible(spriteIds[s][i][j], false);
			}
		}
	}
}


int main(int argc, char *argv[]) {

	setup();

	while (true) {
		getInput();
		drawHelicopter();
	}

	return 0;
}
