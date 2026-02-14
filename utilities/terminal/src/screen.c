/*
 * screen.c - UI layout, status bar, communication buffer, dialogs
 * Ported from F256-terminal CC65 version to oscar64/f256lib
 */

#include "screen.h"
#include "serial.h"
#include "strings.h"
#include <string.h>


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

// Communication buffer storage
static char comm_buffer[COMM_BUFFER_NUM_ROWS][COMM_BUFFER_NUM_COLS + 1];
static byte comm_buffer_row_idx = 0;

// Title bar glyph data (inverse-space based)
static const byte titlebar_glyphs[80] = {
	0x07, 0xB9, 32, 'f', '/', 't', 'e', 'r', 'm', 32, 'F', '2', '5', '6', 32, 0xB8,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0xB9, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xB8, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xB9, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xB8, 0x07,
};


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


static void Buffer_DrawLine(byte row_idx, byte screen_row) {
	textDrawStringAt(COMM_BUFFER_FIRST_COL, screen_row, comm_buffer[row_idx], ANSI_COLOR_CYAN, ANSI_COLOR_BLACK);
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


void Screen_DrawTitleBar(void) {
	byte     mmu        = PEEK(MMU_IO_CTRL);
	uint16_t row_offset = (uint16_t)TITLE_BAR_Y * SCREEN_NUM_COLS;
	byte     i;

	// Write titlebar characters
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_TEXT);
	for (i = 0; i < 80; i++) {
		POKE(TEXT_MATRIX + row_offset + i, titlebar_glyphs[i]);
	}

	// Write titlebar attributes (bright blue on black)
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_COLOR);
	for (i = 0; i < 80; i++) {
		POKE(TEXT_MATRIX + row_offset + i, (ANSI_COLOR_BRIGHT_BLUE << 4) | ANSI_COLOR_BLACK);
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void Screen_DrawUI(void) {
	// Draw title bar
	Screen_DrawTitleBar();

	// Draw communication buffer frame
	textDrawBox(0, COMM_BUFFER_FIRST_ROW - 1, 79, COMM_BUFFER_LAST_ROW + 1, ANSI_COLOR_BRIGHT_BLUE, ANSI_COLOR_BLACK);

	// Clear the comm buffer area
	textFillBox(1, COMM_BUFFER_FIRST_ROW, 78, COMM_BUFFER_LAST_ROW, ' ', ANSI_COLOR_CYAN, ANSI_COLOR_BLACK);
}


void Screen_ShowAppAboutInfo(void) {
	// Draw a simple about box in the comm buffer area
	Buffer_NewMessage(GetString(ID_STR_ABOUT_FTERM));
	Buffer_NewMessage(GetString(ID_STR_ABOUT_GIT));

	if (platformIsAnyK()) {
		Buffer_NewMessage(GetString(ID_STR_MACHINE_K));
	} else {
		Buffer_NewMessage(GetString(ID_STR_MACHINE_JR));
	}
}


void Buffer_Initialize(void) {
	byte i;
	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++) {
		memset(comm_buffer[i], ' ', COMM_BUFFER_NUM_COLS);
		comm_buffer[i][COMM_BUFFER_NUM_COLS] = 0;
	}
	comm_buffer_row_idx = 0;
}


void Buffer_Clear(void) {
	Buffer_Initialize();
	textFillBox(COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW,
	            COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW,
	            ' ', ANSI_COLOR_CYAN, ANSI_COLOR_BLACK);
}


void Buffer_RefreshDisplay(void) {
	byte i;
	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++) {
		Buffer_DrawLine(i, COMM_BUFFER_FIRST_ROW + i);
	}
}


void Buffer_NewMessage(const char *message) {
	byte i;

	// Scroll buffer lines up
	for (i = 0; i < COMM_BUFFER_NUM_ROWS - 1; i++) {
		memcpy(comm_buffer[i], comm_buffer[i + 1], COMM_BUFFER_NUM_COLS + 1);
	}

	// Copy new message into bottom row, pad with spaces
	{
		byte last = COMM_BUFFER_NUM_ROWS - 1;
		int16_t msg_len = generalStrnlen(message, COMM_BUFFER_NUM_COLS);
		int16_t j;

		for (j = 0; j < msg_len && j < COMM_BUFFER_NUM_COLS; j++) {
			comm_buffer[last][j] = message[j];
		}
		for (; j < COMM_BUFFER_NUM_COLS; j++) {
			comm_buffer[last][j] = ' ';
		}
		comm_buffer[last][COMM_BUFFER_NUM_COLS] = 0;
	}

	// Refresh display
	Buffer_RefreshDisplay();
}
