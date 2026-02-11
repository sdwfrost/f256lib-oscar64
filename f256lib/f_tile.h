/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef TILE_H
#define TILE_H
#ifndef WITHOUT_TILE


#include "f256lib.h"


void tileDefineTileMap(byte t, uint32_t address, byte tileSize, uint16_t mapSizeX, uint16_t mapSizeY);
void tileDefineTileSet(byte t, uint32_t address, bool square);
void tileSetScroll(byte t, byte xPixels, uint16_t xTiles, byte yPixels, uint16_t yTiles);
void tileSetVisible(byte t, bool v);
void tileReset(void);


#pragma compile("f_tile.c")


#endif
#endif // TILE_H
