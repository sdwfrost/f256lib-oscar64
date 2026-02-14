/*
 * terminal.c - Main application for F256 ANSI Terminal Emulator
 * Ported from F256-terminal CC65 version to oscar64/f256lib
 *
 * Original author: micahbly (Micah Bly)
 * Oscar64 port: 2024
 *
 * This is a single-file entry point that includes the app logic,
 * overlay configuration, and main event loop.
 *
 * The CC65 version used overlay segments loaded from disk.
 * In oscar64, overlay code is embedded in the PGZ and accessed
 * via MMU bank switching. For this port, the screen overlay code
 * is placed in far memory (physical bank 8) and trampolined.
 */


#include "f256lib.h"
#include "screen.h"
#include "serial.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define MAJOR_VERSION  0
#define MINOR_VERSION  3
#define UPDATE_VERSION 0

// Color scheme
#define APP_FOREGROUND_COLOR   ANSI_COLOR_BRIGHT_BLUE
#define APP_BACKGROUND_COLOR   ANSI_COLOR_BLACK

// Key action mapping
// In the CC65 version, Alt+key produced (key + 128). We use kernel events now.
// For simplicity, we define action keys by their raw kernel key codes.
// These can be refined based on actual F256 keyboard event codes.
#define CH_ALT_OFFSET          128

// Baud rate configurations
typedef struct {
	byte     key;
	uint16_t divisor;
	byte     msg_string_id;
	byte     lbl_string_id;
} baud_config_t;

static const baud_config_t baud_configs[10] = {
	{ 0, UART_BAUD_DIV_115200, ID_STR_MSG_SET_BAUD_115200, ID_STR_BAUD_115200 },
	{ 1, UART_BAUD_DIV_300,    ID_STR_MSG_SET_BAUD_300,    ID_STR_BAUD_300    },
	{ 2, UART_BAUD_DIV_1200,   ID_STR_MSG_SET_BAUD_1200,   ID_STR_BAUD_1200   },
	{ 3, UART_BAUD_DIV_2400,   ID_STR_MSG_SET_BAUD_2400,   ID_STR_BAUD_2400   },
	{ 4, UART_BAUD_DIV_3600,   ID_STR_MSG_SET_BAUD_3600,   ID_STR_BAUD_3600   },
	{ 5, UART_BAUD_DIV_4800,   ID_STR_MSG_SET_BAUD_4800,   ID_STR_BAUD_4800   },
	{ 6, UART_BAUD_DIV_9600,   ID_STR_MSG_SET_BAUD_9600,   ID_STR_BAUD_9600   },
	{ 7, UART_BAUD_DIV_19200,  ID_STR_MSG_SET_BAUD_19200,  ID_STR_BAUD_19200  },
	{ 8, UART_BAUD_DIV_38400,  ID_STR_MSG_SET_BAUD_38400,  ID_STR_BAUD_38400  },
	{ 9, UART_BAUD_DIV_57600,  ID_STR_MSG_SET_BAUD_57600,  ID_STR_BAUD_57600  },
};

static byte    current_baud_config = 5;  // Default: 4800 baud
static bool    clock_is_visible = true;


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static void App_Initialize(void);
static void App_MainLoop(void);
static void App_ChangeBaudRate(byte new_config_index);
static void App_DisplayTime(void);
static void App_EnterStealthTextUpdateMode(void);
static void App_ExitStealthTextUpdateMode(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


static void App_EnterStealthTextUpdateMode(void) {
	platformEnableTextCursor(false);
	Serial_ANSICursorSave();
}


static void App_ExitStealthTextUpdateMode(void) {
	Serial_ANSICursorRestore();
	platformEnableTextCursor(true);
}


static void App_ChangeBaudRate(byte new_config_index) {
	current_baud_config = new_config_index;
	Serial_SetBaud(baud_configs[current_baud_config].divisor);

	App_EnterStealthTextUpdateMode();
	Buffer_NewMessage(GetString(baud_configs[current_baud_config].msg_string_id));
	textDrawStringAt(TERM_BAUD_X1, TITLE_BAR_Y, GetString(baud_configs[current_baud_config].lbl_string_id), ANSI_COLOR_BRIGHT_BLUE, ANSI_COLOR_BLACK);
	App_ExitStealthTextUpdateMode();
}


static void App_DisplayTime(void) {
	rtcTimeT t;
	char     time_str[20];

	if (!clock_is_visible) return;

	rtcRead(&t);

	sprintf(time_str, "20%02X-%02X-%02X %02X:%02X", t.year, t.month, t.day, t.hour, t.min);

	App_EnterStealthTextUpdateMode();
	textDrawStringAt(TERM_DATE_X1, TITLE_BAR_Y, time_str, ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_BLACK);
	App_ExitStealthTextUpdateMode();
}


static void App_Initialize(void) {
	// 80x30 text mode (double height)
	textSetDouble(false, true);

	// Clear screen
	textFillBox(0, 0, 79, 29, ' ', APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);

	// Use primary font slot (CC65 version loaded custom ANSI font into secondary;
	// without that font data, we use the system default primary font)
	platformSwitchFont(true);

	// Initialize UART for serial comms (default 4800 baud)
	Serial_InitUART(baud_configs[current_baud_config].divisor);
	Serial_InitANSIColors();

	// Initialize and draw the comm buffer
	Buffer_Initialize();

	// Draw UI elements
	Screen_DrawUI();
	Screen_ShowAppAboutInfo();

	// Show initial baud rate
	App_ChangeBaudRate(current_baud_config);
}


static void App_MainLoop(void) {
	byte user_input;

	while (1) {
		// Enable cursor
		platformEnableTextCursor(true);

		// Clear terminal body area
		textFillBox(TERM_BODY_X1, TERM_BODY_Y1, TERM_BODY_X2, TERM_BODY_Y2, ' ', ANSI_COLOR_WHITE, ANSI_COLOR_BLACK);

		// Position cursor at top of terminal area
		textGotoXY(TERM_BODY_X1, TERM_BODY_Y1);

		// Inner loop: poll UART and keyboard
		while (1) {
			// Check for and buffer serial data
			Serial_ReadUART();
			Serial_ProcessAvailableData();

			// Check for keyboard input via kernel events
			kernelNextEvent();
			if (kernelError) {
				kernelCall(Yield);
				continue;
			}

			if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;

			user_input = kernelEventData.u.key.ascii;
			if (user_input == 0) continue;

			// Check for Alt+key (flags indicate modifier keys)
			if (kernelEventData.u.key.flags & 0x04) {
				// Alt key is held
				switch (user_input) {
					case 'c': case 'C':
						Serial_CycleForegroundColor();
						break;
					case 'r': case 'R':
						Serial_InitUART(baud_configs[current_baud_config].divisor);
						Buffer_NewMessage("UART reset.");
						break;
					case '1': App_ChangeBaudRate(1); break;  // 300
					case '2': App_ChangeBaudRate(2); break;  // 1200
					case '3': App_ChangeBaudRate(3); break;  // 2400
					case '4': App_ChangeBaudRate(4); break;  // 3600
					case '5': App_ChangeBaudRate(5); break;  // 4800
					case '6': App_ChangeBaudRate(6); break;  // 9600
					case '7': App_ChangeBaudRate(7); break;  // 19200
					case '8': App_ChangeBaudRate(8); break;  // 38400
					case '9': App_ChangeBaudRate(9); break;  // 57600
					case '0': App_ChangeBaudRate(0); break;  // 115200
					default: break;
				}
			} else {
				// Normal key: send over serial
				Serial_SendByte(user_input);
			}
		}
	}
}


/*****************************************************************************/
/*                                   Main                                    */
/*****************************************************************************/


int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// Set border to 0 (use all 80 cols and 30 rows)
	platformSetBorderSize(0, 0);

	// Initialize app
	App_Initialize();

	// Display time
	App_DisplayTime();

	// Enter main loop (never returns)
	App_MainLoop();

	return 0;
}
