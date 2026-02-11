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


int main(int argc, char *argv[]) {
	uint16_t  x;
	uint16_t  y;
	uint16_t  x2;
	uint16_t  y2;
	uint16_t  mx;
	uint16_t  my;
	byte      l;
	byte      c = 0;

	(void)argc;
	(void)argv;

	for (l=0; l<TEXTCOLORS_COUNT; l++) {
		graphicsDefineColor(0, l, textColors[l].r, textColors[l].g, textColors[l].b);
	}

	bitmapSetColor(0);
	bitmapClear();
	bitmapSetVisible(0, true);
	bitmapGetResolution(&mx, &my);

	while (1) {
		bitmapSetColor(c++);
		if (c == TEXTCOLORS_COUNT) c = 0;

		x  = randomRead() % mx;
		y  = randomRead() % my;
		x2 = randomRead() % mx;
		y2 = randomRead() % my;

		bitmapLine(x, y, x2, y2);
	}

	return 0;
}
