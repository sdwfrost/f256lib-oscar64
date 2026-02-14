/*
 * serial.c - ANSI terminal processing and UART buffer management
 * Ported from F256-terminal CC65 version to oscar64/f256lib
 */

#include "serial.h"
#include "screen.h"
#include "strings.h"
#include <string.h>
#include <stdlib.h>


// oscar64 doesn't provide strtok; simple implementation for ANSI parsing
static char *strtok_next = NULL;
static char *local_strtok(char *str, const char *delim) {
	char *start;
	if (str != NULL) strtok_next = str;
	if (strtok_next == NULL) return NULL;
	start = strtok_next;
	while (*strtok_next) {
		const char *d = delim;
		while (*d) {
			if (*strtok_next == *d) {
				*strtok_next = '\0';
				strtok_next++;
				return start;
			}
			d++;
		}
		strtok_next++;
	}
	strtok_next = NULL;
	return start;
}
#define strtok local_strtok


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define TERMINAL_DEFAULT_BACK_COLOR  ANSI_COLOR_BLACK
#define TERMINAL_DEFAULT_FORE_COLOR  ANSI_COLOR_WHITE

#define ANSI_MAX_SEQUENCE_LEN  128

// ANSI function codes (final byte of CSI sequence)
#define ANSI_FUNCTION_CUU      'A'  // Cursor Up
#define ANSI_FUNCTION_CUD      'B'  // Cursor Down
#define ANSI_FUNCTION_CUF      'C'  // Cursor Forward
#define ANSI_FUNCTION_CUB      'D'  // Cursor Back
#define ANSI_FUNCTION_CNL      'E'  // Cursor Next Line
#define ANSI_FUNCTION_CPL      'F'  // Cursor Previous Line
#define ANSI_FUNCTION_CHA      'G'  // Cursor Horizontal Absolute
#define ANSI_FUNCTION_CUP      'H'  // Cursor Position
#define ANSI_FUNCTION_ED       'J'  // Erase in Display
#define ANSI_FUNCTION_EL       'K'  // Erase in Line
#define ANSI_FUNCTION_SU       'S'  // Scroll Up
#define ANSI_FUNCTION_SD       'T'  // Scroll Down
#define ANSI_FUNCTION_HVP      'f'  // Horizontal Vertical Position
#define ANSI_FUNCTION_SGR      'm'  // Select Graphic Rendition
#define ANSI_FUNCTION_DSR      'n'  // Device Status Report
#define ANSI_FUNCTION_CLEAR    'U'  // Clear screen
#define ANSI_FUNCTION_SAVECUR  's'  // Save cursor position
#define ANSI_FUNCTION_RESTCUR  'u'  // Restore cursor position
#define ANSI_FUNCTION_HIDEMOUS 'h'  // Private: hide mouse
#define ANSI_FUNCTION_SHOWMOUS 'l'  // Private: show mouse

// ASCII control characters
#define CH_ESC       0x1B
#define CH_LF        0x0A
#define CH_CR        0x0D
#define CH_FF        0x0C
#define CH_BKSP      0x08
#define CH_SPACE     0x20
#define CH_LBRACKET  0x5B


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

static byte  ansi_sequence_storage[ANSI_MAX_SEQUENCE_LEN + 1];
static byte *ansi_sequence = ansi_sequence_storage;
static byte  ansi_phase = 0;
static bool  ansi_bold_mode = false;

static byte  serial_x;
static byte  serial_y;
static byte  serial_save_x;
static byte  serial_save_y;

static byte  serial_fg_color = TERMINAL_DEFAULT_FORE_COLOR;
static byte  serial_bg_color = TERMINAL_DEFAULT_BACK_COLOR;
static byte  serial_current_pref_color = ANSI_COLOR_BRIGHT_RED;

// Circular buffer for UART data
static byte     uart_in_buffer[UART_BUFFER_SIZE];
static uint16_t uart_write_idx;
static uint16_t uart_read_idx;

// ANSI standard text color LUT (BGRA format, 16 colors x 4 bytes)
static const byte ansi_text_color_lut[64] = {
	0x00, 0x00, 0x00, 0x00,  // 0: Black
	0x00, 0x00, 0xAA, 0x00,  // 1: Red
	0x00, 0xAA, 0x00, 0x00,  // 2: Green
	0x00, 0x55, 0xAA, 0x00,  // 3: Yellow/Brown
	0xAA, 0x00, 0x00, 0x00,  // 4: Blue
	0xAA, 0x00, 0xAA, 0x00,  // 5: Magenta
	0xAA, 0xAA, 0x00, 0x00,  // 6: Cyan
	0xAA, 0xAA, 0xAA, 0x00,  // 7: White
	0x55, 0x55, 0x55, 0x00,  // 8: Bright Black (Gray)
	0x55, 0x55, 0xFF, 0x00,  // 9: Bright Red
	0x55, 0xFF, 0x55, 0x00,  // 10: Bright Green
	0x55, 0xFF, 0xFF, 0x00,  // 11: Bright Yellow
	0xFF, 0x55, 0x55, 0x00,  // 12: Bright Blue
	0xFF, 0x55, 0xFF, 0x00,  // 13: Bright Magenta
	0xFF, 0xFF, 0x55, 0x00,  // 14: Bright Cyan
	0xFF, 0xFF, 0xFF, 0x00,  // 15: Bright White
};

// Temp string buffers
static char string_buff1[256];


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static void Serial_ProcessByte(byte the_byte);
static void Serial_PrintByte(byte the_byte);
static void Serial_ProcessANSI(void);
static void Serial_ANSICursorUp(byte count);
static void Serial_ANSICursorDown(byte count);
static void Serial_ANSICursorLeft(byte count);
static void Serial_ANSICursorRight(byte count);
static void Serial_ANSICursorNextLine(byte count);
static void Serial_ANSICursorPreviousLine(byte count);
static void Serial_ANSIScrollUp(bool scroll_page);
static void Serial_ANSIScrollDown(bool scroll_page);
static void Serial_ANSICursorSetXPos(byte count);
static void Serial_ANSICursorSetXYPos(void);
static void Serial_ANSICursorMoveToXY(void);
static void Serial_ANSIClear(void);
static void Serial_ANSIEraseInDisplay(byte count);
static void Serial_ANSIEraseInLine(byte count);
static void Serial_ANSISendDSR(byte count);
static void Serial_ANSIHandleSGR(byte the_len);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


static void Serial_ANSICursorUp(byte count) {
	while (serial_y > TERM_BODY_Y1 && count > 0) {
		serial_y--;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorDown(byte count) {
	while (serial_y < TERM_BODY_Y2 && count > 0) {
		serial_y++;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorLeft(byte count) {
	while (serial_x > 0 && count > 0) {
		serial_x--;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorRight(byte count) {
	while (serial_x < TERM_BODY_X2 && count > 0) {
		serial_x++;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorNextLine(byte count) {
	serial_x = TERM_BODY_X1;
	while (serial_y < TERM_BODY_Y2 && count > 0) {
		textScrollRowsUp(TERM_BODY_Y1 + 1, TERM_BODY_Y2);
		textFillBox(TERM_BODY_X1, TERM_BODY_Y2, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
		serial_y++;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorPreviousLine(byte count) {
	serial_x = 0;
	while (serial_y > TERM_BODY_Y1 && count > 0) {
		serial_y--;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSIScrollUp(bool scroll_page) {
	byte count = scroll_page ? TERM_BODY_HEIGHT : 1;
	serial_x = TERM_BODY_X1;
	while (serial_y < TERM_BODY_Y2 && count > 0) {
		textScrollRowsUp(TERM_BODY_Y1 + 1, TERM_BODY_Y2);
		textFillBox(TERM_BODY_X1, TERM_BODY_Y2, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
		serial_y++;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSIScrollDown(bool scroll_page) {
	byte count = scroll_page ? TERM_BODY_HEIGHT : 1;
	serial_x = TERM_BODY_X1;
	while (serial_y > TERM_BODY_Y1 && count > 0) {
		serial_y--;
		count--;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorSetXPos(byte count) {
	if (count == 0) return;
	--count;  // ANSI is 1-based

	if (count <= TERM_BODY_X2)
		serial_x = count;
	else
		serial_x = TERM_BODY_X2;

	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorSetXYPos(void) {
	byte   the_x, the_y;
	char  *this_token;
	char  *splitter;

	splitter = strchr((char *)ansi_sequence, ';');

	if (splitter == NULL) {
		the_y = 1;
		this_token = strtok((char *)ansi_sequence, "H");
		the_x = atoi(this_token);
	} else {
		this_token = strtok((char *)ansi_sequence, ";");
		the_y = atoi(this_token);
		this_token = strtok(NULL, ";");
		the_x = atoi(this_token);
	}

	if (the_x == 0) the_x = 1;
	if (the_y == 0) the_y = 1;

	the_y--;
	the_x--;
	the_y += TERM_BODY_Y1;

	serial_y = (the_y < TERM_BODY_Y2) ? the_y : TERM_BODY_Y2;
	serial_x = (the_x < TERM_BODY_X2) ? the_x : TERM_BODY_X2;

	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSICursorMoveToXY(void) {
	byte   the_x, the_y;
	char  *this_token;
	char  *splitter;

	splitter = strchr((char *)ansi_sequence, ';');

	if (splitter == NULL) {
		the_y = 1;
		this_token = strtok((char *)ansi_sequence, "f");
		the_x = atoi(this_token);
	} else {
		this_token = strtok((char *)ansi_sequence, ";");
		the_y = atoi(this_token);
		this_token = strtok(NULL, ";");
		the_x = atoi(this_token);
	}

	if (the_x == 0) the_x = 1;
	if (the_y == 0) the_y = 1;

	the_y--;
	the_x--;
	the_y += TERM_BODY_Y1;

	if (the_y <= TERM_BODY_Y2) {
		serial_y = the_y;
	} else {
		byte diff = the_y - serial_y;
		while (serial_y < TERM_BODY_Y2 && diff > TERM_BODY_Y1) {
			textScrollRowsUp(TERM_BODY_Y1 + 1, TERM_BODY_Y2);
			textFillBox(TERM_BODY_X1, TERM_BODY_Y2, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
			serial_y++;
			diff--;
		}
	}

	serial_x = (the_x <= TERM_BODY_X2) ? the_x : TERM_BODY_X2;
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSIClear(void) {
	serial_fg_color = TERMINAL_DEFAULT_FORE_COLOR;
	serial_bg_color = TERMINAL_DEFAULT_BACK_COLOR;
	textFillBox(TERM_BODY_X1, TERM_BODY_Y1, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
	serial_x = TERM_BODY_X1;
	serial_y = TERM_BODY_Y1;
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSIEraseInDisplay(byte count) {
	switch (count) {
		case 0:
			textFillBox(TERM_BODY_X1, serial_y, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
			serial_x = TERM_BODY_X1;
			break;
		case 1:
			textFillBox(TERM_BODY_X1, TERM_BODY_Y1, TERM_BODY_X2, serial_y, CH_SPACE, serial_fg_color, serial_bg_color);
			serial_x = TERM_BODY_X1;
			break;
		case 2:
		case 3:
			textFillBox(TERM_BODY_X1, TERM_BODY_Y1, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
			serial_x = TERM_BODY_X1;
			serial_y = TERM_BODY_Y1;
			break;
		default:
			return;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSIEraseInLine(byte count) {
	switch (count) {
		case 0:
			textFillBox(serial_x + 1, serial_y, TERM_BODY_X2, serial_y, CH_SPACE, serial_fg_color, serial_bg_color);
			break;
		case 1:
			if (serial_x > TERM_BODY_X1)
				textFillBox(TERM_BODY_X1, serial_y, serial_x - 1, serial_y, CH_SPACE, serial_fg_color, serial_bg_color);
			break;
		case 2:
			textFillBox(TERM_BODY_X1, serial_y, TERM_BODY_X2, serial_y, CH_SPACE, serial_fg_color, serial_bg_color);
			break;
		default:
			return;
	}
	textGotoXY(serial_x, serial_y);
}


static void Serial_ANSISendDSR(byte count) {
	uint16_t len;

	if (count == 6) {
		sprintf(string_buff1, "%c[%d;%dR", CH_ESC, (serial_y + 1) - TERM_BODY_Y1, serial_x + 1);
	} else {
		sprintf(string_buff1, "%c[%02d;%02dR", CH_ESC, TERM_BODY_HEIGHT, TERM_BODY_WIDTH);
	}

	len = strlen(string_buff1);
	Serial_SendData((const byte *)string_buff1, len);
}


static void Serial_ANSIHandleSGR(byte the_len) {
	byte    temp;
	int16_t this_color_code;
	char   *this_token;
	char   *splitter;

	(void)the_len;

	splitter = strchr((char *)ansi_sequence, ';');

	if (splitter == NULL) {
		this_token = strtok((char *)ansi_sequence, "m");
	} else {
		this_token = strtok((char *)ansi_sequence, ";");
	}

	while (this_token != NULL) {
		this_color_code = atoi(this_token);

		if (this_token[0] == '0' && strlen(this_token) == 1) {
			// Reset
			serial_fg_color = TERMINAL_DEFAULT_FORE_COLOR;
			serial_bg_color = TERMINAL_DEFAULT_BACK_COLOR;
			ansi_bold_mode = false;
		} else if (this_color_code == 1) {
			// Bold
			ansi_bold_mode = true;
			if (serial_fg_color < 8) serial_fg_color += 8;
		} else if (this_color_code == 2) {
			// Dim
			ansi_bold_mode = false;
		} else if (this_color_code == 3 || this_color_code == 7) {
			// Italic/Inverse: swap fg and bg
			temp = serial_fg_color;
			serial_fg_color = serial_bg_color;
			serial_bg_color = temp;
		} else if (this_color_code == 8) {
			// Invisible
			serial_fg_color = serial_bg_color;
		} else if (this_color_code > 99) {
			// Bright background (100-107)
			serial_bg_color = (this_color_code - 100) + 8;
		} else if (this_color_code > 89) {
			// Bright foreground (90-97)
			ansi_bold_mode = true;
			serial_fg_color = (this_color_code - 90) + 8;
		} else if (this_color_code > 39) {
			// Background color (40-47)
			serial_bg_color = this_color_code - 40;
		} else if (this_color_code > 29) {
			// Foreground color (30-37)
			serial_fg_color = this_color_code - 30;
			if (ansi_bold_mode) serial_fg_color += 8;
		}

		this_token = strtok(NULL, ";");
	}
}


static void Serial_ProcessByte(byte the_byte) {
	if (the_byte == 0) return;

	if (ansi_phase == 0 && the_byte != CH_ESC) {
		Serial_PrintByte(the_byte);
	} else {
		if (ansi_phase == 0 && the_byte == CH_ESC) {
			ansi_phase = 1;
			ansi_sequence = ansi_sequence_storage;
		} else if (ansi_phase == 1) {
			if (the_byte == CH_LBRACKET) {
				ansi_phase = 2;
			} else {
				ansi_phase = 0;
				Serial_PrintByte(the_byte);
			}
		} else {
			// Collecting ANSI sequence
			if ((the_byte > '@' && the_byte < '[') || (the_byte > '`' && the_byte < '{')) {
				// Terminator character (alpha)
				*ansi_sequence++ = the_byte;
				*ansi_sequence = 0;
				ansi_phase = 0;
				Serial_ProcessANSI();
				ansi_sequence = ansi_sequence_storage;
			} else {
				*ansi_sequence++ = the_byte;
			}
		}
	}
}


static void Serial_PrintByte(byte the_byte) {
	bool update_cursor = true;

	textGotoXY(serial_x, serial_y);

	if (the_byte == CH_CR) {
		serial_x = TERM_BODY_X1;
	} else if (the_byte == CH_LF || the_byte == CH_FF) {
		if (serial_y >= TERM_BODY_Y2) {
			textScrollRowsUp(TERM_BODY_Y1 + 1, TERM_BODY_Y2);
			textFillBox(TERM_BODY_X1, TERM_BODY_Y2, TERM_BODY_X2, TERM_BODY_Y2, CH_SPACE, serial_fg_color, serial_bg_color);
		} else {
			serial_y++;
		}
	} else if (the_byte == CH_BKSP && serial_x > TERM_BODY_X1) {
		serial_x--;
	} else {
		textSetCharAndAttrAt(serial_x, serial_y, the_byte, (serial_fg_color << 4) | serial_bg_color);
		serial_x++;
		update_cursor = false;

		if (serial_x > TERM_BODY_X2) {
			serial_x = TERM_BODY_X2;
		}
	}

	if (update_cursor) {
		textGotoXY(serial_x, serial_y);
	}
}


static void Serial_ProcessANSI(void) {
	byte the_len;
	byte ansi_function;
	byte the_count;

	ansi_sequence = ansi_sequence_storage;
	the_len = generalStrnlen((const char *)ansi_sequence, ANSI_MAX_SEQUENCE_LEN + 1);
	ansi_function = ansi_sequence[the_len - 1];
	the_len--;
	the_count = atoi((char *)ansi_sequence);

	switch (ansi_function) {
		case ANSI_FUNCTION_CUU:  Serial_ANSICursorUp(the_count); break;
		case ANSI_FUNCTION_SU:   Serial_ANSIScrollUp(false); break;
		case ANSI_FUNCTION_CUD:  Serial_ANSICursorDown(the_count); break;
		case ANSI_FUNCTION_SD:   Serial_ANSIScrollDown(false); break;
		case ANSI_FUNCTION_CUF:  Serial_ANSICursorRight(the_count); break;
		case ANSI_FUNCTION_CUB:  Serial_ANSICursorLeft(the_count); break;
		case ANSI_FUNCTION_CNL:  Serial_ANSICursorNextLine(the_count); break;
		case ANSI_FUNCTION_CPL:  Serial_ANSICursorPreviousLine(the_count); break;
		case ANSI_FUNCTION_CHA:  Serial_ANSICursorSetXPos(the_count); break;
		case ANSI_FUNCTION_CUP:  Serial_ANSICursorSetXYPos(); break;
		case ANSI_FUNCTION_SAVECUR:  Serial_ANSICursorSave(); break;
		case ANSI_FUNCTION_RESTCUR:  Serial_ANSICursorRestore(); break;
		case ANSI_FUNCTION_CLEAR: Serial_ANSIClear(); break;
		case ANSI_FUNCTION_ED:   Serial_ANSIEraseInDisplay(the_count); break;
		case ANSI_FUNCTION_EL:   Serial_ANSIEraseInLine(the_count); break;
		case ANSI_FUNCTION_HVP:  Serial_ANSICursorMoveToXY(); break;
		case ANSI_FUNCTION_SGR:  Serial_ANSIHandleSGR(the_len); break;
		case ANSI_FUNCTION_DSR:  Serial_ANSISendDSR(the_count); break;
		case ANSI_FUNCTION_HIDEMOUS:
		case ANSI_FUNCTION_SHOWMOUS:
			break;  // Ignored private sequences
		default:
			break;
	}
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


void Serial_InitANSIColors(void) {
	byte     mmu = PEEK(MMU_IO_CTRL);
	uint16_t i;

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Set foreground color LUT
	for (i = 0; i < 64; i++) {
		POKE(VKY_TXT_FGLUT + i, ansi_text_color_lut[i]);
	}

	// Set background color LUT
	for (i = 0; i < 64; i++) {
		POKE(VKY_TXT_BGLUT + i, ansi_text_color_lut[i]);
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void Serial_InitUART(uint16_t baud_divisor) {
	uartInit(baud_divisor);

	serial_x = TERM_BODY_X1;
	serial_y = TERM_BODY_Y1;

	uart_write_idx = 0;
	uart_read_idx = 0;
}


void Serial_SetBaud(uint16_t baud_divisor) {
	uartSetBaud(baud_divisor);
}


bool Serial_SendByte(byte the_byte) {
	uartSendByte(the_byte);
	return true;
}


byte Serial_SendData(const byte *buffer, uint16_t len) {
	uint16_t i;
	for (i = 0; i < len; i++) {
		uartSendByte(buffer[i]);
	}
	return (byte)i;
}


bool Serial_ReadUART(void) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (PEEK(UART_LSR) & UART_LSR_DATA_READY) {
		uart_in_buffer[uart_write_idx] = PEEK(UART_RXD);
		uart_write_idx = (uart_write_idx + 1) & UART_BUFFER_MASK;

		// Overflow check: if write catches up to read, advance read
		if (uart_write_idx == uart_read_idx) {
			uart_read_idx = (uart_read_idx + 1) & UART_BUFFER_MASK;
		}
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
	return true;
}


bool Serial_ProcessAvailableData(void) {
	if (uart_read_idx == uart_write_idx) {
		return false;
	}

	while (uart_read_idx != uart_write_idx) {
		Serial_ProcessByte(uart_in_buffer[uart_read_idx]);
		uart_read_idx = (uart_read_idx + 1) & UART_BUFFER_MASK;
	}

	return true;
}


void Serial_FlushInBuffer(void) {
	uart_read_idx = 0;
	uart_write_idx = 0;
}


void Serial_ANSICursorSave(void) {
	serial_save_x = serial_x;
	serial_save_y = serial_y;
}


void Serial_ANSICursorRestore(void) {
	serial_x = serial_save_x;
	serial_y = serial_save_y;
	textGotoXY(serial_x, serial_y);
}


void Serial_CycleForegroundColor(void) {
	serial_current_pref_color++;
	if (serial_current_pref_color > 15) {
		serial_current_pref_color = 1;  // Skip black-on-black
	}
	textFillBoxAttr(TERM_BODY_X1, TERM_BODY_Y1, TERM_BODY_X2, TERM_BODY_Y2, serial_current_pref_color, ANSI_COLOR_BLACK);
	serial_fg_color = serial_current_pref_color;
}
