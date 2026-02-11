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


/*
 * The original map data was swiped from http://grumdrig.com/u4map/ and then
 * converted to binary using makeBinMaps.js.  This program takes the binary
 * maps and turns them into F256 tilemaps.
 *
 * These Ultima V maps have a tileset stored in a 256x512 bitmap.  This is
 * 16x32 tiles of 16x16 pixels for a total of 512 tiles.  F256 tilesets are
 * limited to 256 tiles so we set this up as two tilesets.  Any tile index
 * over 255 is in set 2.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../../tools/shared/util.h"


int main(int argc, char *argv[]) {
	char     *filename;
	FILE     *in;
	FILE     *out;
	uint16_t  tile;
	int16_t   byte;
	uint16_t  x;
	uint16_t  y;

	if (argc != 2) {
		printf("Usage:  %s [mapfile]\n", argv[0]);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		printf("Unable to open %s!\n", argv[1]);
		return 2;
	}

	filename = utilReplaceExtension(argv[1], ".tiles");
	printf("Creating %s...\n", filename);
	out = fopen(filename, "wb");
	if (out == NULL) {
		printf("Unable to create %s!\n", filename);
		fclose(in);
		free(filename);
		return 3;
	}

	/*
	while ((byte = fgetc(in)) != EOF) {
		// We always use CLUT0, so there's no need to set it here.
		tile = ((byte > 255 ? 1 : 0) << 8) | (byte & 0x00ff);
		fwrite(&tile, sizeof(uint16_t), 1, out);
	}
	*/

	// Ultima maps are actually 256 high.  Skip last row.
	for (y=0; y<255; y++) {
		for (x=0; x<255; x++) {
			byte = fgetc(in);
			// We always use CLUT0, so there's no need to set it here.
			tile = ((byte > 255 ? 1 : 0) << 8) | (byte & 0x00ff);
			fwrite(&tile, sizeof(uint16_t), 1, out);
		}
		// Ultima maps are actually 256 wide.  Skip last column.
		fgetc(in);
	}

	fclose(out);
	fclose(in);
	free(filename);

	return 0;
}
