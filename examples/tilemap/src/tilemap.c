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


#define MAP_BRITANIA        0x10000  // Len:  0x1fc02
#define MAP_UNDERWORLD      0x2fc02  // Len:  0x1fc02
#define TILES_IMAGE_TOP     0x4f804  // Len:  0x20000
#define TILES_IMAGE_BOTTOM  0x5f804  // Halfway through TILES_IMAGE_TOP
#define TILES_CLUT          0x6f804  // Len:  0x40


// Embed tile data at far memory address 0x10000.
// tiles.bin is 391236 bytes, spanning 0x10000..0x6f844.
// Split into 64KB chunks due to 16-bit size_t limitation on 6502.

#pragma section( td0, 0)
#pragma region( td0, 0x10000, 0x20000, , , {td0} )
#pragma data(td0)
__export const char td_chunk0[] = {
	#embed 65536 0 "../tiles/tiles.bin"
};

#pragma section( td1, 0)
#pragma region( td1, 0x20000, 0x30000, , , {td1} )
#pragma data(td1)
__export const char td_chunk1[] = {
	#embed 65536 65536 "../tiles/tiles.bin"
};

#pragma section( td2, 0)
#pragma region( td2, 0x30000, 0x40000, , , {td2} )
#pragma data(td2)
__export const char td_chunk2[] = {
	#embed 65536 131072 "../tiles/tiles.bin"
};

#pragma section( td3, 0)
#pragma region( td3, 0x40000, 0x50000, , , {td3} )
#pragma data(td3)
__export const char td_chunk3[] = {
	#embed 65536 196608 "../tiles/tiles.bin"
};

#pragma section( td4, 0)
#pragma region( td4, 0x50000, 0x60000, , , {td4} )
#pragma data(td4)
__export const char td_chunk4[] = {
	#embed 65536 262144 "../tiles/tiles.bin"
};

#pragma section( td5, 0)
#pragma region( td5, 0x60000, 0x70000, , , {td5} )
#pragma data(td5)
__export const char td_chunk5[] = {
	#embed 63556 327680 "../tiles/tiles.bin"
};

#pragma data(data)


int main(int argc, char *argv[]) {
	byte     x;
	byte     r;
	byte     g;
	byte     b;
	uint32_t c;
	uint16_t xs = 75;
	uint16_t ys = 99;

	// Set up CLUT0.
	c = TILES_CLUT;
	for (x=0; x<16; x++) {
		b = FAR_PEEK(c++);
		g = FAR_PEEK(c++);
		r = FAR_PEEK(c++);
		c++;
		graphicsDefineColor(0, x, r, g, b);
	}

	graphicsSetLayerTile(0, 0);
	tileDefineTileSet(0, TILES_IMAGE_TOP, true);
	tileDefineTileSet(1, TILES_IMAGE_BOTTOM, true);
	tileDefineTileMap(0, MAP_BRITANIA, 16, 255, 255);  // Change to MAP_UNDERWORLD for more fun.
	tileSetVisible(0, true);

	tileSetScroll(0, 0, xs, 0, ys);

	while (true) {
		kernelEventData.type = 0;
		kernelCall(NextEvent);
		if (kernelEventData.type == kernelEvent(key.RELEASED)) {
			switch (kernelEventData.u.key.ascii) {
				case 'w':
				case 'W':
					ys--;
					break;
				case 'a':
				case 'A':
					xs--;
					break;
				case 's':
				case 'S':
					ys++;
					break;
				case 'd':
				case 'D':
					xs++;
					break;
			}
			tileSetScroll(0, 0, xs, 0, ys);
			textGotoXY(0, 0);
			textPrintInt(xs);
			textPrint(", ");
			textPrintInt(ys);
			textPrint("  ");
		}
	}

	return 0;
}
