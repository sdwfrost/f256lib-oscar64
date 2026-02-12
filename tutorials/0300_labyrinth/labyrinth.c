// 0300 Labyrinth - Simple maze game
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdlib.h>

// Maze map (80x30 grid, '#' = wall, ' ' = empty, 'X' = exit)
static const byte MAZE_W = 40;
static const byte MAZE_H = 20;

void draw_maze(void)
{
	volatile byte *text = (volatile byte *)0xc000;
	volatile byte *color = (volatile byte *)0xc000;

	// Draw walls
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for (byte y = 0; y < MAZE_H; y++)
		for (byte x = 0; x < MAZE_W; x++) {
			unsigned int pos = (unsigned int)y * 80 + x;
			if (x == 0 || x == MAZE_W-1 || y == 0 || y == MAZE_H-1)
				text[pos] = '#';
			else if ((x % 4 == 0) && (y % 2 == 0))
				text[pos] = '#';
			else
				text[pos] = ' ';
		}
	// Exit
	text[(unsigned int)(MAZE_H-1) * 80 + MAZE_W-2] = 'X';

	// Color the walls
	POKE(MMU_IO_CTRL, MMU_IO_COLOR);
	for (byte y = 0; y < MAZE_H; y++)
		for (byte x = 0; x < MAZE_W; x++) {
			unsigned int pos = (unsigned int)y * 80 + x;
			color[pos] = 0xF0;  // white on black
		}
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
}

byte get_char(byte x, byte y)
{
	volatile byte *text = (volatile byte *)0xc000;
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	byte c = text[(unsigned int)y * 80 + x];
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);
	return c;
}

int main(int argc, char *argv[])
{
	textClear();
	draw_maze();

	byte px = 1, py = 1;

	textGotoXY(0, MAZE_H + 1);
	textPrint("NAVIGATE TO X. ARROWS TO MOVE.");

	for (;;)
	{
		textGotoXY(px, py);
		textSetColor(YELLOW, BLACK);
		textPutChar('@');

		char c = keyboardGetChar();

		textGotoXY(px, py);
		textSetColor(WHITE, BLACK);
		textPutChar(' ');

		byte nx = px, ny = py;
		switch (c) {
		case KEY_UP:    if (ny > 0) ny--; break;
		case KEY_DOWN:  ny++; break;
		case KEY_LEFT:  if (nx > 0) nx--; break;
		case KEY_RIGHT: nx++; break;
		}

		byte tc = get_char(nx, ny);
		if (tc != '#') {
			px = nx;
			py = ny;
		}
		if (tc == 'X') {
			textGotoXY(0, MAZE_H + 3);
			textPrint("YOU WIN!");
			return 0;
		}
	}

	return 0;
}
