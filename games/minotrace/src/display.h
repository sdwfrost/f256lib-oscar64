#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

// Screen dimensions
#define SCREEN_COLS   40
#define SCREEN_ROWS   25
#define SCREEN_SIZE   (SCREEN_COLS * SCREEN_ROWS)
#define HUD_ROW       25
#define TOTAL_ROWS    30

// RAM screen buffers for the 3D viewport (rows 0-24)
extern char screen_chars[SCREEN_SIZE];
extern char screen_colors[SCREEN_SIZE];

// Custom font character indices
#define FONT_EMPTY     0     // Ceiling (all zeros)
#define FONT_SOLID     1     // Solid wall (all 0xFF)
#define FONT_TOP_1     2     // Top edge sub-position 1 (wall at bottom row)
#define FONT_TOP_7     8     // Top edge sub-position 7 (wall at rows 1-7)
#define FONT_BOT_1     9     // Bottom edge sub-position 1 (wall at top row)
#define FONT_BOT_7     15    // Bottom edge sub-position 7 (wall at rows 0-6)
#define FONT_FLOOR     16    // Checkered floor pattern
#define FONT_BLOCK     17    // Full block (for big text highlight)

// BlockChar values for big text overlay
enum BlockChar
{
	BC_BLACK     = 0,
	BC_WHITE     = 1,
	BC_BOX_BLACK = 2,
	BC_BOX_RED   = 3
};

// Timing
extern bool         time_running;
extern unsigned     time_count;
extern signed char  time_digits[5];

// Level colors set by maze_build
extern char display_wall_color;
extern char display_floor_color;

// Basic display init
void display_init(void);

// Prepare display for game play
void display_game(void);

// Display the title screen
void display_title(void);

// Display the game completed screen
void display_completed(void);

// Flip: wait for VBlank and copy screen buffers to VRAM
void display_flip(void);

// Reset screen buffers
void display_reset(void);

// Set the compass direction in HUD
void compass_draw(char w);

// Draw the current countdown timer in HUD
void time_draw(void);

// Init the countdown timer to the given duration
void time_init(unsigned seconds);

// Decrement countdown timer (called each frame)
void time_dec(void);

// Draw big text on top of the 3D screen
void display_put_bigtext(char x, char y, const char * text, BlockChar c);

// Scroll the screen buffer one char to the left
void display_scroll_left(void);

// Scroll the screen buffer one char to the right
void display_scroll_right(void);

// Display five-star shutter transition
void display_five_star(char t);

// Display mine explosion particles
void display_explosion(void);

#pragma compile("display.c")

#endif
