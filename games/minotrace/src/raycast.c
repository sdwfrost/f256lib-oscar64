#include "raycast.h"
#include "maze.h"
#include "display.h"

// Fixed-point 8.8 signed multiply: (a * b) >> 8
static int lmul8f8s(int a, int b)
{
	return (int)(((long)a * (long)b) >> 8);
}

// Table of 8.8 sine for 64 parts of a circle
const int sintab[64] = {
	0, 25, 50, 74, 98, 121, 142, 162, 181, 198, 213, 226, 237, 245, 251, 255, 256, 255, 251, 245, 237, 226, 213, 198, 181, 162, 142, 121, 98, 74, 50, 25, 0, -25, -50, -74, -98, -121, -142, -162, -181, -198, -213, -226, -237, -245, -251, -255, -256, -255, -251, -245, -237, -226, -213, -198, -181, -162, -142, -121, -98, -74, -50, -25
};

// Table of 8.8 cosine for 64 parts of a circle
const int costab[64] = {
	256, 255, 251, 245, 237, 226, 213, 198, 181, 162, 142, 121, 98, 74, 50, 25, 0, -25, -50, -74, -98, -121, -142, -162, -181, -198, -213, -226, -237, -245, -251, -255, -256, -255, -251, -245, -237, -226, -213, -198, -181, -162, -142, -121, -98, -74, -50, -25, 0, 25, 50, 74, 98, 121, 142, 162, 181, 198, 213, 226, 237, 245, 251, 255
};

// Table of scaled 8.8 sine for 64 parts of a circle for one column
const int dsintab[64] = {
	0, -1, -2, -4, -5, -6, -7, -8, -9, -10, -11, -11, -12, -12, -13, -13, -13, -13, -13, -12, -12, -11, -11, -10, -9, -8, -7, -6, -5, -4, -2, -1, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 12, 13, 13, 13, 13, 13, 12, 12, 11, 11, 10, 9, 8, 7, 6, 5, 4, 2, 1
};

// Table of scaled 8.8 cosine for 64 parts of a circle for one column
const int dcostab[64] = {
	13, 13, 13, 12, 12, 11, 11, 10, 9, 8, 7, 6, 5, 4, 2, 1, 0, -1, -2, -4, -5, -6, -7, -8, -9, -10, -11, -11, -12, -12, -13, -13, -13, -13, -13, -12, -12, -11, -11, -10, -9, -8, -7, -6, -5, -4, -2, -1, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 12, 13, 13
};

// Static table of quotients 4096 / i clamped at 255
static char inverse[4096] = {
	255,
#assign i 1
#repeat
	(4096 / i > 255) ? 255 : 4096 / i,
#assign i i + 1
#until i == 4096
#undef i
};

#pragma align(inverse, 256)

// Table of LSB and MSB of squares for fast 8 by 8 multiply
static char sqrtabl[256], sqrtabh[256];

#pragma align(sqrtabl, 256)
#pragma align(sqrtabh, 256)

// Brightness lookup table
char	blut[136];

// Result of ray cast for each column
static char col_h[41], col_x[41], col_y[41], col_d[41];

// Not used for double buffering on F256K, kept for compatibility
char sindex;

// Color lookup for maze block and direction
// Maps (field_type | direction) to F256K foreground color index
char clut[4 * 8] = {
	0, 0, 0, 0,          // MF_EMPTY
	1, 1, 1, 1,           // MF_EXIT (white)
	2, 1, 2, 1,           // MF_MINE (red/white flickering)
	0, 0, 0, 0,           // MF_DUMMY

	9, 2, 10, 2,          // MF_RED: brown/red/lt_red/red
	6, 6, 14, 6,          // MF_BLUE: blue/blue/lt_blue/blue
	4, 4, 3, 4,           // MF_PURPLE: purple/purple/cyan/purple
	12, 15, 1, 15,        // MF_WHITE: grey/lt_grey/white/lt_grey
};


void rcast_init_tables(void)
{
	// Init table of squares
	for (unsigned i = 0; i < 256; i++)
	{
		unsigned s = i * i;
		sqrtabl[i] = s & 0xff;
		sqrtabh[i] = s >> 8;
	}

	// Brightness lookup table
	for (int i = 0; i < 136; i++)
	{
		int t = 72 / (i + 8);
		if (t > 6) t = 6;
		blut[i] = 16 * (6 - t);
	}
}


// Square of an unsigned byte using table lookup
static inline unsigned square(char c)
{
	return (unsigned)sqrtabl[(unsigned char)c] | ((unsigned)sqrtabh[(unsigned char)c] << 8);
}

// Multiply two eight bit numbers using binomials
static inline unsigned mul88(char a, char b)
{
	unsigned s = (unsigned char)a + (unsigned char)b;
	if (s >= 256)
	{
		s &= 0xff;
		return ((square(s) - square(a) - square(b)) >> 1) + (s << 8);
	}
	else
		return (square(s) - square(a) - square(b)) >> 1;
}

// Calculate height of a column with given distance r and scale d
static inline char colheight(unsigned d, unsigned r)
{
	if (r >= 4096)
		return 0;
	else
	{
		unsigned h = mul88(d, inverse[r]) >> 4;
		if (h >= 256)
			return 255;
		else
			return h;
	}
}


// Draw a single column into the screen buffer
// col: column (0-39)
// height: wall height in pixels (0-135)
// fg_color: foreground color index (0-15)
static void drawColumn(char col, char height, char fg_color)
{
	char hchars = height >> 3;          // Wall height in full chars (0-16)
	char hfrac_bot = height & 7;        // Sub-char bottom edge (0-7)
	char hfrac_top = (height >> 1) & 7; // Sub-char top edge (0-7)

	// Vertical positions (same as C64 fastcode)
	char yl = 8 - (hchars >> 1);   // First filled char row
	char yh = 8 + hchars;          // First row after wall

	// Color attributes
	char wall_attr = (fg_color << 4) | 0x00;   // Wall fg, black bg
	char ceil_attr = 0x00;                       // Black on black
	char floor_attr = (display_floor_color << 4) | 0x00;  // Floor color, black bg
	char edge_top_attr = (fg_color << 4) | 0x00;  // Wall fg, black bg (for top edge)
	char edge_bot_attr = (fg_color << 4) | display_floor_color;  // Wall fg, floor bg (for bottom edge)

	// Draw ceiling (rows 0 to yl-2)
	for (char y = 0; y < yl - 1 && y < 25; y++)
	{
		screen_chars[(int)y * 40 + col] = FONT_EMPTY;
		screen_colors[(int)y * 40 + col] = ceil_attr;
	}

	// Draw top edge char (row yl-1) if visible
	if (yl > 0 && yl <= 25)
	{
		char top_char;
		if (hfrac_top == 0)
			top_char = FONT_SOLID;  // Full wall char
		else
			top_char = FONT_TOP_1 + hfrac_top - 1;  // Top edge sub-position

		screen_chars[(int)(yl - 1) * 40 + col] = top_char;
		screen_colors[(int)(yl - 1) * 40 + col] = edge_top_attr;
	}

	// Draw inner wall chars (rows yl to yh-1)
	for (char y = yl; y < yh && y < 25; y++)
	{
		screen_chars[(int)y * 40 + col] = FONT_SOLID;
		screen_colors[(int)y * 40 + col] = wall_attr;
	}

	// Draw bottom edge char (row yh) if visible
	if (yh < 25)
	{
		char bot_char;
		if (hfrac_bot == 0)
			bot_char = FONT_FLOOR;  // Just floor
		else
			bot_char = FONT_BOT_1 + hfrac_bot - 1;  // Bottom edge sub-position

		screen_chars[(int)yh * 40 + col] = bot_char;
		screen_colors[(int)yh * 40 + col] = edge_bot_attr;
	}

	// Draw floor (rows yh+1 to 24)
	for (char y = yh + 1; y < 25; y++)
	{
		screen_chars[(int)y * 40 + col] = FONT_FLOOR;
		screen_colors[(int)y * 40 + col] = floor_attr;
	}
}


void rcast_draw_screen(void)
{
	for (char x = 0; x < 40; x++)
	{
		char w = col_h[x];
		if (w > 135)
			w = 135;

		// Get foreground color from lookup table
		char fg = clut[col_d[x]];

		drawColumn(x, w, fg);
	}
}


// Cast a single ray with unsigned fractionals
static inline void dcast(char sx, char ix, char iy, unsigned irx, unsigned iry, signed char dix, signed char diy, unsigned idx, unsigned idy)
{
	const char *bp = maze_grid + 256 * iy;
	char udx = idx >> 2, udy = idy >> 2;
	signed char id = (int)(mul88(irx, udy) - mul88(iry, udx)) >> 8;

	for(;;)
	{
		while (id < 0)
		{
			ix += dix;
			if (bp[ix])
			{
				col_x[sx] = ix;
				col_y[sx] = (char)((unsigned)bp >> 8);
				col_d[sx] = dix < 0 ? bp[ix] | 0 : bp[ix] | 2;
				col_h[sx] = colheight(udx, irx);
				return;
			}
			irx += 256;
			id += udy;
		}

		while (id >= 0)
		{
			bp += 256 * diy;
			if (bp[ix])
			{
				col_x[sx] = ix;
				col_y[sx] = (char)((unsigned)bp >> 8);
				col_d[sx] = diy < 0 ? bp[ix] | 1 : bp[ix] | 3;
				col_h[sx] = colheight(udy, iry);
				return;
			}
			iry += 256;
			id -= udx;
		}
	}
}


// Cast a single ray with signed fractionals
static inline void icast(char sx, int ipx, int ipy, int idx, int idy)
{
	char ix = ipx >> 8, iy = ipy >> 8;
	unsigned irx = ipx & 255;
	unsigned iry = ipy & 255;

	if (idx < 0)
	{
		if (idy < 0)
			dcast(sx, ix, iy, irx, iry, -1, -1, -idx, -idy);
		else
			dcast(sx, ix, iy, irx, iry ^ 0xff, -1, 1, -idx, idy);
	}
	else
	{
		if (idy < 0)
			dcast(sx, ix, iy, irx ^ 0xff, iry, 1, -1, idx, -idy);
		else
			dcast(sx, ix, iy, irx ^ 0xff, iry ^ 0xff, 1, 1, idx, idy);
	}
}


void rcast_cast_rays(int ipx, int ipy, int idx, int idy, int iddx, int iddy)
{
	// Let mine flicker
	clut[MF_MINE + 0] ^= 0x02;
	clut[MF_MINE + 1] ^= 0x02;
	clut[MF_MINE + 2] ^= 0x02;
	clut[MF_MINE + 3] ^= 0x02;

	// Cast left most ray
	icast(0, ipx, ipy, idx, idy);

	// Step through all columns in a step size of two
	for (int i = 0; i < 39; i += 2)
	{
		icast(i + 2, ipx, ipy, idx + 2 * iddx, idy + 2 * iddy);

		if (col_x[i] == col_x[i + 2] && col_y[i] == col_y[i + 2] && col_d[i] == col_d[i + 2])
		{
			col_x[i + 1] = col_x[i];
			col_y[i + 1] = col_y[i];
			col_d[i + 1] = col_d[i];
			col_h[i + 1] = (col_h[i] + col_h[i + 2]) >> 1;
		}
		else
		{
			icast(i + 1, ipx, ipy, idx + iddx, idy + iddy);
		}

		idx += 2 * iddx;
		idy += 2 * iddy;
	}
}
