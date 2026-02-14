#include "display.h"
#include "gamemusic.h"
#include "input.h"
#include "raycast.h"
#include "f256lib.h"
#include <string.h>
#include <stdlib.h>

// RAM screen buffers for the 3D viewport
char screen_chars[SCREEN_SIZE];
char screen_colors[SCREEN_SIZE];

// Timer state
bool         time_running;
signed char  time_digits[5];
unsigned     time_count;

// Level colors
char display_wall_color;
char display_floor_color;

// C64-compatible palette (indices 0-15)
static const colorT c64_palette[16] = {
	{ 0x00, 0x00, 0x00 },  //  0 black
	{ 0xFF, 0xFF, 0xFF },  //  1 white
	{ 0x88, 0x39, 0x32 },  //  2 red
	{ 0x67, 0xB6, 0xBD },  //  3 cyan
	{ 0x8B, 0x3F, 0x96 },  //  4 purple
	{ 0x55, 0xA0, 0x49 },  //  5 green
	{ 0x40, 0x31, 0x8D },  //  6 blue
	{ 0xBF, 0xCE, 0x72 },  //  7 yellow
	{ 0x8B, 0x54, 0x29 },  //  8 orange
	{ 0x57, 0x42, 0x00 },  //  9 brown
	{ 0xB8, 0x69, 0x62 },  // 10 light red
	{ 0x50, 0x50, 0x50 },  // 11 dark grey
	{ 0x78, 0x78, 0x78 },  // 12 grey
	{ 0x94, 0xE0, 0x89 },  // 13 light green
	{ 0x78, 0x69, 0xC4 },  // 14 light blue
	{ 0x9F, 0x9F, 0x9F },  // 15 light grey
};

// Embedded ASCII font bitmap for big text (chars 32-90)
// Each character is 8 bytes (8x8 pixels, MSB first)
static const char bigfont_data[] = {
	// Space (32)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// ! (33)
	0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00,
	// " (34)
	0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// # (35)
	0x6C, 0xFE, 0x6C, 0x6C, 0xFE, 0x6C, 0x00, 0x00,
	// $ (36)
	0x18, 0x7E, 0x58, 0x7E, 0x1A, 0x7E, 0x18, 0x00,
	// % (37)
	0x62, 0x64, 0x08, 0x10, 0x20, 0x4C, 0x8C, 0x00,
	// & (38)
	0x30, 0x48, 0x30, 0x56, 0x88, 0x88, 0x76, 0x00,
	// ' (39)
	0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	// ( (40)
	0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00,
	// ) (41)
	0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00,
	// * (42)
	0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00,
	// + (43)
	0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00,
	// , (44)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30,
	// - (45)
	0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00,
	// . (46)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,
	// / (47)
	0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00,
	// 0 (48)
	0x7C, 0xC6, 0xCE, 0xD6, 0xE6, 0xC6, 0x7C, 0x00,
	// 1 (49)
	0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,
	// 2 (50)
	0x7C, 0xC6, 0x06, 0x3C, 0x60, 0xC0, 0xFE, 0x00,
	// 3 (51)
	0x7C, 0xC6, 0x06, 0x3C, 0x06, 0xC6, 0x7C, 0x00,
	// 4 (52)
	0x0C, 0x1C, 0x3C, 0x6C, 0xFE, 0x0C, 0x0C, 0x00,
	// 5 (53)
	0xFE, 0xC0, 0xFC, 0x06, 0x06, 0xC6, 0x7C, 0x00,
	// 6 (54)
	0x7C, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0x7C, 0x00,
	// 7 (55)
	0xFE, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,
	// 8 (56)
	0x7C, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0x7C, 0x00,
	// 9 (57)
	0x7C, 0xC6, 0xC6, 0x7E, 0x06, 0x06, 0x7C, 0x00,
	// : (58)
	0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00,
	// ; (59)
	0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, 0x00,
	// < (60)
	0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00,
	// = (61)
	0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00,
	// > (62)
	0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00,
	// ? (63)
	0x7C, 0xC6, 0x06, 0x1C, 0x18, 0x00, 0x18, 0x00,
	// @ (64)
	0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x7C, 0x00,
	// A-Z (65-90)
	0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00,
	0xFC, 0xC6, 0xFC, 0xC6, 0xC6, 0xC6, 0xFC, 0x00,
	0x7C, 0xC6, 0xC0, 0xC0, 0xC0, 0xC6, 0x7C, 0x00,
	0xF8, 0xCC, 0xC6, 0xC6, 0xC6, 0xCC, 0xF8, 0x00,
	0xFE, 0xC0, 0xF8, 0xC0, 0xC0, 0xC0, 0xFE, 0x00,
	0xFE, 0xC0, 0xF8, 0xC0, 0xC0, 0xC0, 0xC0, 0x00,
	0x7C, 0xC6, 0xC0, 0xCE, 0xC6, 0xC6, 0x7E, 0x00,
	0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0x00,
	0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00,
	0x06, 0x06, 0x06, 0x06, 0xC6, 0xC6, 0x7C, 0x00,
	0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0x00,
	0xC6, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0x00,
	0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00,
	0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00,
	0xFC, 0xC6, 0xC6, 0xFC, 0xC0, 0xC0, 0xC0, 0x00,
	0x7C, 0xC6, 0xC6, 0xC6, 0xD6, 0xCC, 0x76, 0x00,
	0xFC, 0xC6, 0xC6, 0xFC, 0xD8, 0xCC, 0xC6, 0x00,
	0x7C, 0xC6, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00,
	0xFE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
	0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00,
	0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00,
	0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00,
	0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0xC6, 0x00,
	0xC6, 0xC6, 0x6C, 0x38, 0x18, 0x18, 0x18, 0x00,
	0xFE, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xFE, 0x00,
};

// Compass direction names
static const char * const compass_names[8] = {
	"  E", " NE", "  N", " NW", "  W", " SW", "  S", " SE"
};


// Build the custom font for wall rendering
static void display_font_build(void)
{
	unsigned char font_data[18 * 8];

	// Char 0: empty (ceiling)
	for (char i = 0; i < 8; i++)
		font_data[0 * 8 + i] = 0x00;

	// Char 1: solid wall
	for (char i = 0; i < 8; i++)
		font_data[1 * 8 + i] = 0xFF;

	// Chars 2-8: top edge sub-positions 1-7
	// Wall grows from bottom of char upward
	for (char s = 1; s <= 7; s++)
	{
		for (char i = 0; i < 8; i++)
		{
			if (i >= (8 - s))
				font_data[(1 + s) * 8 + i] = 0xFF;  // Wall
			else
				font_data[(1 + s) * 8 + i] = 0x00;  // Ceiling
		}
	}

	// Chars 9-15: bottom edge sub-positions 1-7
	// Wall shrinks from top, floor appears from bottom
	for (char s = 1; s <= 7; s++)
	{
		for (char i = 0; i < 8; i++)
		{
			if (i < s)
				font_data[(8 + s) * 8 + i] = 0xFF;  // Wall
			else
				font_data[(8 + s) * 8 + i] = (i & 1) ? 0x55 : 0xAA;  // Floor checker
		}
	}

	// Char 16: floor (checkered)
	for (char i = 0; i < 8; i++)
		font_data[16 * 8 + i] = (i & 1) ? 0x55 : 0xAA;

	// Char 17: full block (for big text)
	for (char i = 0; i < 8; i++)
		font_data[17 * 8 + i] = 0xFF;

	// Load the custom chars into the primary font slot at char positions 0-17
	// We need to write directly since textLoadFont replaces the whole font
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	volatile byte *font_dest = (volatile byte *)0xC000;
	for (int i = 0; i < 18 * 8; i++)
		font_dest[i] = font_data[i];

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void display_init(void)
{
	// Set 40-column mode (double X pixels), keep 30 rows (double Y)
	textSetDouble(true, true);

	// Disable hardware cursor
	textSetCursor(0);

	// Set border to black
	graphicsSetBorderRGB(0, 0, 0);
	graphicsSetBackgroundRGB(0, 0, 0);

	// Define C64-compatible color palette for text foreground and background
	for (char i = 0; i < 16; i++)
	{
		textDefineForegroundColor(i, c64_palette[i].r, c64_palette[i].g, c64_palette[i].b);
		textDefineBackgroundColor(i, c64_palette[i].r, c64_palette[i].g, c64_palette[i].b);
	}

	// Build custom wall font
	display_font_build();

	// Clear screen
	textClear();

	// Initialize screen buffers
	memset(screen_chars, FONT_EMPTY, SCREEN_SIZE);
	memset(screen_colors, 0x00, SCREEN_SIZE);

	// Initialize SID volume
	POKE(SID1 + SID_FM_VC, 0x0F);
}


void display_game(void)
{
	// Clear everything
	textClear();

	// Clear screen buffers
	memset(screen_chars, FONT_EMPTY, SCREEN_SIZE);
	memset(screen_colors, 0x00, SCREEN_SIZE);

	// Set up HUD text on rows 25-29
	// Row 26: compass and timer labels
	textDrawStringAt(0, 26, "DIR:", WHITE, BLACK);
	textDrawStringAt(20, 26, "TIME:", WHITE, BLACK);

	// Row 28: level info
	textDrawStringAt(0, 28, "MINOTRACE", YELLOW, BLACK);
}


static void display_copy_to_vram(void)
{
	byte mmu = PEEK(MMU_IO_CTRL);

	// Copy chars to VRAM (I/O page 2 = text matrix)
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_TEXT);
	{
		volatile byte *vram = (volatile byte *)TEXT_MATRIX;
		const char *src = screen_chars;
		for (char y = 0; y < SCREEN_ROWS; y++)
		{
			for (char x = 0; x < SCREEN_COLS; x++)
				vram[x] = src[x];
			vram += SCREEN_COLS;
			src += SCREEN_COLS;
		}
	}

	// Copy colors to VRAM (I/O page 3 = color matrix)
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_COLOR);
	{
		volatile byte *vram = (volatile byte *)TEXT_MATRIX;
		const char *src = screen_colors;
		for (char y = 0; y < SCREEN_ROWS; y++)
		{
			for (char x = 0; x < SCREEN_COLS; x++)
				vram[x] = src[x];
			vram += SCREEN_COLS;
			src += SCREEN_COLS;
		}
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void display_flip(void)
{
	graphicsWaitVerticalBlank();
	display_copy_to_vram();
}


void display_reset(void)
{
	memset(screen_chars, FONT_EMPTY, SCREEN_SIZE);
	memset(screen_colors, 0x00, SCREEN_SIZE);
}


void time_dec(void)
{
	if (time_count > 0)
	{
		// Check if we've counted down this second's sub-frames
		if (time_digits[4] == 0 && time_digits[3] == 0)
			time_count--;

		if (time_count > 0)
		{
			// Sub-frame counter (frame-based)
			if (--time_digits[4] >= 0)
				return;
			time_digits[4] = 9;

			if (--time_digits[3] >= 0)
				return;
			time_digits[3] = 4;  // ~50fps (PAL-like timing for F256K at 60Hz, close enough)

			// Seconds
			if (--time_digits[2] >= 0)
				return;
			time_digits[2] = 9;

			if (--time_digits[1] >= 0)
				return;
			time_digits[1] = 5;

			// Minutes
			if (--time_digits[0] >= 0)
				return;
			time_digits[0] = 9;
		}
	}
}


void time_init(unsigned seconds)
{
	time_count = seconds + 1;

	time_digits[3] = time_digits[4] = 0;
	time_digits[2] = seconds % 10; seconds /= 10;
	time_digits[1] = seconds %  6; seconds /= 6;
	time_digits[0] = seconds;
}


void compass_draw(char w)
{
	// Map 64-step direction to 8 compass points
	char dir = ((w + 4) >> 3) & 7;
	textDrawStringAt(4, 26, compass_names[dir], AQUAMARINE, BLACK);
}


void time_draw(void)
{
	char buf[8];
	buf[0] = '0' + time_digits[0];
	buf[1] = ':';
	buf[2] = '0' + time_digits[1];
	buf[3] = '0' + time_digits[2];
	buf[4] = '.';
	buf[5] = '0' + time_digits[3];
	buf[6] = '0' + time_digits[4];
	buf[7] = 0;
	textDrawStringAt(25, 26, buf, YELLOW, BLACK);
}


void display_put_bigtext(char x, char y, const char * text, BlockChar bc)
{
	char c;
	while ((c = *text++) != 0)
	{
		// Convert to uppercase if lowercase
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';

		// Get font data pointer
		const char *sp;
		if (c >= 32 && c <= 90)
			sp = bigfont_data + (c - 32) * 8;
		else
			sp = bigfont_data;  // Default to space

		// Render 8x8 bitmap into screen buffer
		for (char iy = 0; iy < 8; iy++)
		{
			char row = y + iy;
			if (row >= SCREEN_ROWS)
				break;

			char b = sp[iy];
			for (char ix = 0; ix < 8; ix++)
			{
				char col = x + ix;
				if (col >= SCREEN_COLS)
					break;

				if (b & 0x80)
				{
					int idx = (int)row * SCREEN_COLS + col;
					switch (bc)
					{
						case BC_WHITE:
							screen_chars[idx] = FONT_BLOCK;
							screen_colors[idx] = 0xF0;  // White fg, black bg
							break;
						case BC_BLACK:
							screen_chars[idx] = FONT_EMPTY;
							screen_colors[idx] = 0x00;
							break;
						case BC_BOX_RED:
							screen_chars[idx] = FONT_BLOCK;
							screen_colors[idx] = 0x20;  // Red fg, black bg
							break;
						case BC_BOX_BLACK:
							screen_chars[idx] = FONT_BLOCK;
							screen_colors[idx] = 0xB0;  // Dark grey fg, black bg
							break;
					}
				}
				b <<= 1;
			}
		}
		x += 8;
	}
}


void display_scroll_left(void)
{
	for (char y = 0; y < SCREEN_ROWS; y++)
	{
		int row_offset = (int)y * SCREEN_COLS;
		for (char x = 0; x < SCREEN_COLS - 1; x++)
		{
			screen_chars[row_offset + x] = screen_chars[row_offset + x + 1];
			screen_colors[row_offset + x] = screen_colors[row_offset + x + 1];
		}
		screen_chars[row_offset + SCREEN_COLS - 1] = FONT_EMPTY;
		screen_colors[row_offset + SCREEN_COLS - 1] = 0x00;
	}
}


void display_scroll_right(void)
{
	for (char y = 0; y < SCREEN_ROWS; y++)
	{
		int row_offset = (int)y * SCREEN_COLS;
		for (signed char x = SCREEN_COLS - 2; x >= 0; x--)
		{
			screen_chars[row_offset + x + 1] = screen_chars[row_offset + x];
			screen_colors[row_offset + x + 1] = screen_colors[row_offset + x];
		}
		screen_chars[row_offset] = FONT_EMPTY;
		screen_colors[row_offset] = 0x00;
	}
}


// Five-star distance table (same as C64 original)
static const char display_five_table[] = {
	190, 188, 186, 185, 183, 181, 180, 179, 178, 178, 178, 178, 168, 153, 140, 127, 115, 103, 92, 82, 72, 82, 92, 103, 115, 127, 140, 153, 168, 178, 178, 178, 178, 179, 180, 181, 183, 185, 186, 188,
	178, 176, 174, 172, 170, 168, 167, 165, 164, 163, 163, 163, 163, 149, 135, 122, 109, 97, 86, 76, 66, 76, 86, 97, 109, 122, 135, 149, 163, 163, 163, 163, 164, 165, 167, 168, 170, 172, 174, 176,
	166, 164, 161, 159, 157, 155, 154, 152, 150, 149, 148, 148, 148, 144, 130, 117, 104, 92, 80, 70, 60, 70, 80, 92, 104, 117, 130, 144, 148, 148, 148, 149, 150, 152, 154, 155, 157, 159, 161, 164,
	154, 152, 149, 147, 145, 143, 141, 139, 137, 136, 135, 134, 133, 133, 126, 112, 98, 86, 75, 64, 54, 64, 75, 86, 98, 112, 126, 133, 133, 134, 135, 136, 137, 139, 141, 143, 145, 147, 149, 152,
	143, 140, 138, 135, 133, 131, 128, 126, 124, 122, 121, 120, 119, 118, 119, 107, 93, 81, 69, 58, 48, 58, 69, 81, 93, 107, 119, 118, 119, 120, 121, 122, 124, 126, 128, 131, 133, 135, 138, 140,
	132, 129, 126, 124, 121, 119, 116, 114, 112, 110, 108, 106, 105, 104, 104, 103, 88, 75, 63, 52, 42, 52, 63, 75, 88, 103, 104, 104, 105, 106, 108, 110, 112, 114, 116, 119, 121, 124, 126, 129,
	130, 121, 115, 112, 110, 107, 105, 102, 100, 97, 95, 93, 91, 90, 89, 89, 84, 70, 57, 46, 36, 46, 57, 70, 84, 89, 89, 90, 91, 93, 95, 97, 100, 102, 105, 107, 110, 112, 115, 121,
	137, 129, 120, 111, 102, 96, 93, 91, 88, 85, 83, 81, 79, 77, 75, 74, 74, 65, 52, 40, 30, 40, 52, 65, 74, 74, 75, 77, 79, 81, 83, 85, 88, 91, 93, 96, 102, 111, 120, 129,
	145, 136, 127, 119, 110, 101, 92, 84, 77, 74, 71, 69, 66, 64, 62, 60, 59, 59, 47, 34, 24, 34, 47, 59, 59, 60, 62, 64, 66, 69, 71, 74, 77, 84, 92, 101, 110, 119, 127, 136,
	153, 144, 135, 127, 118, 109, 100, 91, 82, 74, 65, 58, 55, 52, 50, 48, 46, 45, 42, 29, 18, 29, 42, 45, 46, 48, 50, 52, 55, 58, 65, 74, 82, 91, 100, 109, 118, 127, 135, 144,
	162, 153, 144, 135, 126, 117, 108, 99, 90, 81, 73, 64, 55, 46, 38, 36, 33, 31, 30, 23, 12, 23, 30, 31, 33, 36, 38, 46, 55, 64, 73, 81, 90, 99, 108, 117, 126, 135, 144, 153,
	171, 162, 153, 144, 135, 126, 117, 108, 99, 90, 81, 72, 63, 54, 45, 36, 27, 19, 17, 15, 6, 15, 17, 19, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162,
	180, 171, 162, 153, 144, 135, 126, 117, 108, 99, 90, 81, 72, 63, 54, 45, 36, 27, 18, 9, 0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171,
	190, 181, 172, 163, 154, 145, 136, 127, 118, 109, 100, 91, 82, 73, 64, 56, 47, 38, 24, 11, 12, 11, 24, 38, 47, 56, 64, 73, 82, 91, 100, 109, 118, 127, 136, 145, 154, 163, 172, 181,
	200, 191, 182, 173, 164, 155, 146, 138, 129, 120, 111, 102, 94, 85, 75, 61, 47, 34, 21, 17, 24, 17, 21, 34, 47, 61, 75, 85, 94, 102, 111, 120, 129, 138, 146, 155, 164, 173, 182, 191,
	211, 202, 193, 184, 175, 167, 158, 149, 140, 132, 123, 113, 99, 85, 71, 57, 44, 32, 23, 28, 36, 28, 23, 32, 44, 57, 71, 85, 99, 113, 123, 132, 140, 149, 158, 167, 175, 184, 193, 202,
	222, 213, 204, 196, 187, 178, 170, 161, 151, 137, 122, 108, 95, 81, 68, 55, 42, 31, 34, 40, 48, 40, 34, 31, 42, 55, 68, 81, 95, 108, 122, 137, 151, 161, 170, 178, 187, 196, 204, 213,
	234, 225, 216, 208, 199, 189, 174, 160, 146, 132, 118, 105, 91, 78, 65, 53, 41, 40, 45, 52, 60, 52, 45, 40, 41, 53, 65, 78, 91, 105, 118, 132, 146, 160, 174, 189, 199, 208, 216, 225,
	246, 238, 226, 212, 198, 184, 170, 156, 142, 128, 115, 102, 89, 76, 64, 52, 46, 51, 56, 63, 72, 63, 56, 51, 46, 52, 64, 76, 89, 102, 115, 128, 142, 156, 170, 184, 198, 212, 226, 238,
	250, 235, 221, 207, 193, 179, 165, 152, 138, 125, 112, 99, 86, 74, 62, 52, 57, 62, 68, 75, 84, 75, 68, 62, 57, 52, 62, 74, 86, 99, 112, 125, 138, 152, 165, 179, 193, 207, 221, 235,
	245, 231, 217, 203, 189, 175, 162, 149, 135, 122, 110, 97, 85, 73, 61, 63, 68, 73, 80, 87, 96, 87, 80, 73, 68, 63, 61, 73, 85, 97, 110, 122, 135, 149, 162, 175, 189, 203, 217, 231,
	240, 227, 213, 199, 186, 172, 159, 146, 133, 120, 108, 95, 84, 72, 69, 74, 79, 85, 91, 99, 108, 99, 91, 85, 79, 74, 69, 72, 84, 95, 108, 120, 133, 146, 159, 172, 186, 199, 213, 227,
	236, 223, 209, 196, 182, 169, 156, 143, 131, 118, 106, 94, 83, 75, 80, 85, 90, 96, 103, 111, 120, 111, 103, 96, 90, 85, 80, 75, 83, 94, 106, 118, 131, 143, 156, 169, 182, 196, 209, 223,
	233, 219, 206, 193, 180, 167, 154, 141, 129, 117, 105, 93, 82, 86, 91, 96, 101, 108, 115, 123, 132, 123, 115, 108, 101, 96, 91, 86, 82, 93, 105, 117, 129, 141, 154, 167, 180, 193, 206, 219,
	229, 216, 203, 190, 177, 164, 152, 139, 127, 115, 104, 92, 92, 97, 102, 107, 113, 119, 127, 135, 144, 135, 127, 119, 113, 107, 102, 97, 92, 92, 104, 115, 127, 139, 152, 164, 177, 190, 203, 216
};


void display_five_star(char t)
{
	for (int i = 0; i < SCREEN_SIZE; i++)
	{
		if (display_five_table[i] < t)
		{
			screen_chars[i] = FONT_BLOCK;
			screen_colors[i] = 0xF0;  // White
		}
		else
		{
			screen_chars[i] = FONT_EMPTY;
			screen_colors[i] = 0x00;
		}
	}
}


void display_explosion(void)
{
	// Velocity and position of explosion particles
	signed char	dx[128], dy[128];
	char		xp[128], yp[128];

	// Initialize particles
	for (char i = 0; i < 128; i++)
	{
		char s = i & 63;
		xp[i] = (rand() & 7) + 16;
		yp[i] = (rand() & 7) + 10;
		dx[i] = sintab[s] >> 5;
		dy[i] = (costab[s] >> 4) - 8;
	}

	// Loop 20 times over all particles
	for (int j = 0; j < 20; j++)
	{
		for (char i = 0; i < 128; i++)
		{
			if (yp[i] < 25)
			{
				// Erase particle
				int idx = (int)yp[i] * SCREEN_COLS + xp[i];
				if (idx >= 0 && idx < SCREEN_SIZE)
				{
					screen_chars[idx] = FONT_EMPTY;
					screen_colors[idx] = 0x00;
				}

				// Move
				int nx = xp[i] + (dx[i] >> 2);
				int ny = yp[i] + (dy[i] >> 2);

				if (ny < 0 || ny >= 25 || nx < 0 || nx >= 40)
					yp[i] = 255;
				else
				{
					xp[i] = nx;
					yp[i] = ny;

					// Draw particle
					idx = ny * SCREEN_COLS + nx;
					if (idx >= 0 && idx < SCREEN_SIZE)
					{
						screen_chars[idx] = FONT_BLOCK;
						screen_colors[idx] = 0xF0;  // White
					}

					dx[i] += (dx[i] >> 2);
					dy[i] += (dy[i] >> 2);
					dy[i] += 2;  // Gravity
				}
			}
		}
	}
}


// Title screen - text-based replacement for C64 bitmap
void display_title(void)
{
	// Clear screen
	textClear();
	memset(screen_chars, FONT_EMPTY, SCREEN_SIZE);
	memset(screen_colors, 0x00, SCREEN_SIZE);

	// Draw "MINOTRACE" in big text centered
	display_put_bigtext(0, 2, "MINO", BC_WHITE);
	display_put_bigtext(0, 10, "TRACE", BC_WHITE);

	// Copy to VRAM
	display_flip();

	// Draw credits in HUD area
	textDrawStringAt(2, 26, "DESIGN: DR.MORTAL WOMBAT", LIGHT_GRAY, BLACK);
	textDrawStringAt(6, 27, "MUSIC: CRISPS", LIGHT_GRAY, BLACK);
	textDrawStringAt(2, 28, "F256K PORT: COMMUNITY", LIGHT_GRAY, BLACK);
	textDrawStringAt(4, 29, "PRESS BUTTON TO START", YELLOW, BLACK);

	// Wait for button press
	bool down = true;
	for (;;)
	{
		joy_poll_input();
		if (!joyb)
			down = false;
		if (joyb && !down)
			break;
		graphicsWaitVerticalBlank();
	}
}


void display_completed(void)
{
	textClear();
	memset(screen_chars, FONT_EMPTY, SCREEN_SIZE);
	memset(screen_colors, 0x00, SCREEN_SIZE);

	display_put_bigtext(0, 2, "WELL", BC_WHITE);
	display_put_bigtext(0, 10, "DONE!", BC_WHITE);

	display_flip();

	textDrawStringAt(1, 26, "CONGRATULATIONS!", YELLOW, BLACK);
	textDrawStringAt(1, 27, "ALL MAZES COMPLETED!", LIGHT_GREEN, BLACK);
	textDrawStringAt(4, 29, "PRESS BUTTON", YELLOW, BLACK);

	bool down = true;
	for (;;)
	{
		joy_poll_input();
		if (!joyb)
			down = false;
		if (joyb && !down)
			break;
		graphicsWaitVerticalBlank();
	}
}
