#ifndef SPRITE_UTIL_H
#define SPRITE_UTIL_H

// Sprite utility for porting C64 OscarTutorials to F256K f256lib.
//
// Provides:
// - C64 1-bit → 8bpp sprite expansion into far memory
// - C64 palette loading into graphics CLUT 0
// - Sprite position helper (TinyVicky +32 offset)
// - Software bounding-box collision detection

#include "f256lib.h"

// Sprite data address in system bus memory (above CPU space)
#define SPR_DATA_BASE    0x10000UL
#define SPR_IMG_SIZE     576       // 24x24 @ 8bpp
#define SPR_MAX_IMAGES   16

// TinyVicky sprite coordinates: visible area starts at (32,32)
#define SPR_OFFSET_X     32
#define SPR_OFFSET_Y     32

// Sprite state tracking
static int      _spr_x[8], _spr_y[8];
static byte     _spr_enabled;
static byte     _spr_image[8];
static byte     _spr_color[8];

// Load C64 16-color palette into Graphics CLUT 0
static void sprite_init_clut(void)
{
	byte i;
	for (i = 0; i < 16; i++)
		graphicsDefineColor(0, i, c64Palette[i].r, c64Palette[i].g, c64Palette[i].b);

	// Entry 0: transparent (TinyVicky treats pixel value 0 as transparent)
	// Set it to black with alpha=0 by writing directly
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_1);
	POKE(VKY_GR_CLUT_0 + 0, 0);
	POKE(VKY_GR_CLUT_0 + 1, 0);
	POKE(VKY_GR_CLUT_0 + 2, 0);
	POKE(VKY_GR_CLUT_0 + 3, 0);  // alpha = 0 → transparent
	POKE(MMU_IO_CTRL, mmu);
}

// Expand C64 1-bit sprite data (63 bytes, 24x21) to 8bpp (24x24, 576 bytes)
// in far memory at SPR_DATA_BASE + slot * SPR_IMG_SIZE.
// color: palette index for set bits (0 = transparent)
static byte sprite_expand_c64(const char *src, byte slot, byte color)
{
	uint32_t dest = SPR_DATA_BASE + (uint32_t)slot * SPR_IMG_SIZE;
	byte row, col;

	for (row = 0; row < 21; row++) {
		byte b0 = (byte)src[row * 3 + 0];
		byte b1 = (byte)src[row * 3 + 1];
		byte b2 = (byte)src[row * 3 + 2];

		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + col, (b0 & (0x80 >> col)) ? color : 0);
		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + 8 + col, (b1 & (0x80 >> col)) ? color : 0);
		for (col = 0; col < 8; col++)
			FAR_POKE(dest + row * 24 + 16 + col, (b2 & (0x80 >> col)) ? color : 0);
	}

	// Bottom 3 rows: transparent
	for (row = 21; row < 24; row++)
		for (col = 0; col < 24; col++)
			FAR_POKE(dest + row * 24 + col, 0);

	return slot;
}

// Initialize sprite system: enable sprite engine, load C64 CLUT
static void sprite_init(void)
{
	byte i;

	_spr_enabled = 0;
	for (i = 0; i < 8; i++) {
		_spr_x[i] = 0;
		_spr_y[i] = 0;
		_spr_image[i] = 0;
		_spr_color[i] = 0;
	}

	sprite_init_clut();

	// Enable sprite engine in master control
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	POKE(VKY_MSTR_CTRL_0, PEEK(VKY_MSTR_CTRL_0) | VKY_GRAPH | VKY_SPRITE);

	spriteReset();
}

// Configure and show a sprite
// sp: sprite number (0-7)
// show: visible flag
// xpos, ypos: screen coordinates (C64-style, 0,0 = top-left visible)
// image: expanded image slot number
// color: palette index (stored for collision color change)
static void sprite_set(byte sp, bool show, int xpos, int ypos,
                        byte image, byte color)
{
	sp &= 7;
	_spr_x[sp] = xpos;
	_spr_y[sp] = ypos;
	_spr_image[sp] = image;
	_spr_color[sp] = color;

	if (show)
		_spr_enabled |= (1 << sp);
	else
		_spr_enabled &= ~(1 << sp);

	uint32_t addr = SPR_DATA_BASE + (uint32_t)image * SPR_IMG_SIZE;
	spriteDefine(sp, addr, 24, 0, 0);
	spriteSetPosition(sp, (uint16_t)(xpos + SPR_OFFSET_X),
	                      (uint16_t)(ypos + SPR_OFFSET_Y));
	spriteSetVisible(sp, show);
}

// Move a sprite to a new position
static void sprite_move(byte sp, int xpos, int ypos)
{
	sp &= 7;
	_spr_x[sp] = xpos;
	_spr_y[sp] = ypos;
	spriteSetPosition(sp, (uint16_t)(xpos + SPR_OFFSET_X),
	                      (uint16_t)(ypos + SPR_OFFSET_Y));
}

// Change sprite image slot
static void sprite_image(byte sp, byte image)
{
	sp &= 7;
	_spr_image[sp] = image;
	uint32_t addr = SPR_DATA_BASE + (uint32_t)image * SPR_IMG_SIZE;
	spriteDefine(sp, addr, 24, 0, 0);
	spriteSetVisible(sp, (_spr_enabled & (1 << sp)) ? true : false);
}

// Change sprite color (re-expands pixel data in far memory)
static void sprite_recolor(byte sp, const char *src, byte new_color)
{
	sp &= 7;
	_spr_color[sp] = new_color;
	sprite_expand_c64(src, _spr_image[sp], new_color);
}

// Show or hide a sprite
static void sprite_show(byte sp, bool show)
{
	sp &= 7;
	if (show)
		_spr_enabled |= (1 << sp);
	else
		_spr_enabled &= ~(1 << sp);
	spriteSetVisible(sp, show);
}

// Software sprite-sprite collision detection (bounding-box)
// Returns 8-bit bitmask: bit N set if sprite N overlaps any other enabled sprite
static byte sprite_check_collisions(void)
{
	byte result = 0;
	byte i, j;

	for (i = 0; i < 8; i++) {
		if (!(_spr_enabled & (1 << i)))
			continue;
		for (j = i + 1; j < 8; j++) {
			if (!(_spr_enabled & (1 << j)))
				continue;

			int dx = _spr_x[i] - _spr_x[j];
			int dy = _spr_y[i] - _spr_y[j];
			if (dx < 0) dx = -dx;
			if (dy < 0) dy = -dy;

			if (dx < 24 && dy < 24) {
				result |= (1 << i);
				result |= (1 << j);
			}
		}
	}
	return result;
}

#endif // SPRITE_UTIL_H
