/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_TILE


#include "f256lib.h"


static byte _tileSize[3];


void tileDefineTileMap(byte t, uint32_t address, byte tileSize, uint16_t mapSizeX, uint16_t mapSizeY) {
	_tileSize[t] = tileSize;
	switch (t) {
		case 0:
			POKEA(VKY_TM0_ADDR_L, address);
			POKEW(VKY_TM0_SIZE_X, mapSizeX);
			POKEW(VKY_TM0_SIZE_Y, mapSizeY);
			break;
		case 1:
			POKEA(VKY_TM1_ADDR_L, address);
			POKEW(VKY_TM1_SIZE_X, mapSizeX);
			POKEW(VKY_TM1_SIZE_Y, mapSizeY);
			break;
		case 2:
			POKEA(VKY_TM2_ADDR_L, address);
			POKEW(VKY_TM2_SIZE_X, mapSizeX);
			POKEW(VKY_TM2_SIZE_Y, mapSizeY);
			break;
	}

	tileSetScroll(t, 0, 0, 0, 0);
}


void tileDefineTileSet(byte t, uint32_t address, bool square) {
	switch (t) {
		case 0:
			POKEA(VKY_TS0_ADDR_L, address);
			POKE(VKY_TS0_SQUARE, square << 3);
			break;
		case 1:
			POKEA(VKY_TS1_ADDR_L, address);
			POKE(VKY_TS1_SQUARE, square << 3);
			break;
		case 2:
			POKEA(VKY_TS2_ADDR_L, address);
			POKE(VKY_TS2_SQUARE, square << 3);
			break;
		case 3:
			POKEA(VKY_TS3_ADDR_L, address);
			POKE(VKY_TS3_SQUARE, square << 3);
			break;
		case 4:
			POKEA(VKY_TS4_ADDR_L, address);
			POKE(VKY_TS4_SQUARE, square << 3);
			break;
		case 5:
			POKEA(VKY_TS5_ADDR_L, address);
			POKE(VKY_TS5_SQUARE, square << 3);
			break;
		case 6:
			POKEA(VKY_TS6_ADDR_L, address);
			POKE(VKY_TS6_SQUARE, square << 3);
			break;
		case 7:
			POKEA(VKY_TS7_ADDR_L, address);
			POKE(VKY_TS7_SQUARE, square << 3);
			break;
	}
}


void tileSetScroll(byte t, byte xPixels, uint16_t xTiles, byte yPixels, uint16_t yTiles) {
	uint16_t scrollX = (xTiles << 4) + xPixels;
	uint16_t scrollY = (yTiles << 4) + yPixels;

	switch (t) {
		case 0:
			POKEW(VKY_TM0_POS_X_L, scrollX);
			POKEW(VKY_TM0_POS_Y_L, scrollY);
			break;
		case 1:
			POKEW(VKY_TM1_POS_X_L, scrollX);
			POKEW(VKY_TM1_POS_Y_L, scrollY);
			break;
		case 2:
			POKEW(VKY_TM2_POS_X_L, scrollX);
			POKEW(VKY_TM2_POS_Y_L, scrollY);
			break;
	}
}


void tileSetVisible(byte t, bool v) {
	switch (t) {
		case 0:
			POKE(VKY_TM0_CTRL, ((_tileSize[0] == 8 ? 1 : 0) << 4) | v);
			break;
		case 1:
			POKE(VKY_TM1_CTRL, ((_tileSize[1] == 8 ? 1 : 0) << 4) | v);
			break;
		case 2:
			POKE(VKY_TM2_CTRL, ((_tileSize[2] == 8 ? 1 : 0) << 4) | v);
			break;
	}
}


void tileReset(void) {
	_tileSize[0] = 8;
	_tileSize[1] = 8;
	_tileSize[2] = 8;
	tileSetVisible(0, false);
	tileSetVisible(1, false);
	tileSetVisible(2, false);
}


#endif
