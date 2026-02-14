/*
 * app.h - Main application definitions for the File Manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "f256lib.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define MAJOR_VERSION	1
#define MINOR_VERSION	2
#define UPDATE_VERSION	0

#define VERSION_NUM_X	0
#define VERSION_NUM_Y	24

// Panels
#define NUM_PANELS					2
#define PANEL_ID_LEFT				0
#define PANEL_ID_RIGHT				1

// Dialog dimensions
#define APP_DIALOG_WIDTH				42
#define APP_DIALOG_HEIGHT				7
#define APP_DIALOG_BUFF_SIZE			((APP_DIALOG_WIDTH+2) * (APP_DIALOG_HEIGHT+2))

// File naming limits
#define FILE_MAX_FILENAME_SIZE		32		// 31 chars + terminator
#define FILE_MAX_PATHNAME_SIZE		255
#define FILE_MAX_EXTENSION_SIZE		8
#define FILE_TYPE_MAX_SIZE_NAME		4
#define FILE_SIZE_MAX_SIZE			16
#define FILE_BYTES_PER_BLOCK_IEC	254
#define FILE_BYTES_PER_BLOCK		256

// Device configuration
#define DEVICE_LOWEST_DEVICE_NUM	0
#define DEVICE_HIGHEST_DEVICE_NUM	2
#define DEVICE_MAX_DEVICE_COUNT		(DEVICE_HIGHEST_DEVICE_NUM - DEVICE_LOWEST_DEVICE_NUM + 1)

// Memory viewer
#define MEM_DUMP_BYTES_PER_ROW		16
#define MAX_MEM_DUMP_LEN			(24 * MEM_DUMP_BYTES_PER_ROW)
#define MEM_DUMP_START_X_FOR_HEX	7
#define MEM_DUMP_START_X_FOR_CHAR	(MEM_DUMP_START_X_FOR_HEX + MEM_DUMP_BYTES_PER_ROW * 3)

#define MEM_TEXT_VIEW_BYTES_PER_ROW	80
#define MAX_TEXT_VIEW_ROWS_PER_PAGE	59
#define MAX_TEXT_VIEW_LEN			(MAX_TEXT_VIEW_ROWS_PER_PAGE * MEM_TEXT_VIEW_BYTES_PER_ROW)

// Temporary buffer in normal RAM (CC65 used fixed addresses below stack)
#define STORAGE_FILE_BUFFER_1_LEN	256


/*****************************************************************************/
/*                           App-wide color choices                          */
/*****************************************************************************/

#define APP_FOREGROUND_COLOR		LIGHT_BLUE
#define APP_BACKGROUND_COLOR		BLACK
#define APP_ACCENT_COLOR			DARK_BLUE
#define APP_SELECTED_FILE_COLOR		WHITE

#define BUFFER_FOREGROUND_COLOR		AQUAMARINE
#define BUFFER_BACKGROUND_COLOR		BLACK
#define BUFFER_ACCENT_COLOR			LIGHT_BLUE

#define PANEL_FOREGROUND_COLOR		DARK_GREEN
#define PANEL_BACKGROUND_COLOR		BLACK
#define PANEL_ACCENT_COLOR			LIGHT_BLUE

#define LIST_ACTIVE_COLOR			LIGHT_GREEN
#define LIST_INACTIVE_COLOR			DARK_GREEN

#define LIST_HEADER_COLOR			YELLOW

#define MENU_FOREGROUND_COLOR		AQUAMARINE
#define MENU_INACTIVE_COLOR			LIGHT_BLUE
#define MENU_BACKGROUND_COLOR		BLACK
#define MENU_ACCENT_COLOR			LIGHT_BLUE

#define DIALOG_FOREGROUND_COLOR		YELLOW
#define DIALOG_BACKGROUND_COLOR		DARK_BLUE
#define DIALOG_ACCENT_COLOR			LIGHT_BLUE
#define DIALOG_AFFIRM_COLOR			DARK_GREEN
#define DIALOG_CANCEL_COLOR			DEEP_RED

#define FILE_CONTENTS_FOREGROUND_COLOR	LIGHT_GREEN
#define FILE_CONTENTS_BACKGROUND_COLOR	BLACK
#define FILE_CONTENTS_ACCENT_COLOR		DARK_GREEN


/*****************************************************************************/
/*                                   Command Keys                            */
/*****************************************************************************/

#define ACTION_INVALID_INPUT		255

#define ACTION_CANCEL				0x1B	// ESC
#define ACTION_CONFIRM				0x0D	// ENTER

// Navigation
#define MOVE_UP						0x91	// cursor up
#define MOVE_RIGHT					0x1D	// cursor right
#define MOVE_DOWN					0x11	// cursor down
#define MOVE_LEFT					0x9D	// cursor left
#define ACTION_SWAP_ACTIVE_PANEL	0x09	// TAB

// File and memory bank actions
#define ACTION_SELECT				0x0D
#define ACTION_DELETE				0x14	// BKSP
#define ACTION_DELETE_ALT			'x'
#define ACTION_COPY					'c'
#define ACTION_DUPLICATE			'p'
#define ACTION_VIEW_AS_HEX			'h'
#define ACTION_VIEW_AS_TEXT			't'
#define ACTION_RENAME				'r'
#define ACTION_LOAD					'l'
#define ACTION_FILL_MEMORY			'F'
#define ACTION_CLEAR_MEMORY			'z'
#define ACTION_SEARCH_MEMORY		'f'
#define ACTION_SEARCH_MEMORY_NEXT	'g'

// Folder actions
#define ACTION_NEW_FOLDER			'm'
#define ACTION_SORT_BY_NAME			'N'
#define ACTION_SORT_BY_SIZE			'S'
#define ACTION_SORT_BY_TYPE			'T'
#define ACTION_REFRESH_PANEL		'R'
#define ACTION_LOAD_MEATLOAF_URL	'M'

// Device actions
#define ACTION_SWITCH_TO_SD			'0'
#define ACTION_SWITCH_TO_FLOPPY_1	'1'
#define ACTION_SWITCH_TO_FLOPPY_2	'2'
#define ACTION_SWITCH_TO_RAM		'8'
#define ACTION_SWITCH_TO_FLASH		'9'
#define ACTION_FORMAT_DISK			'"'

// App actions
#define ACTION_SET_TIME				'C'
#define ACTION_ABOUT				'a'
#define ACTION_EXIT_TO_BASIC		'b'
#define ACTION_EXIT_TO_DOS			'd'
#define ACTION_QUIT					'q'


/*****************************************************************************/
/*                                 Error Codes                               */
/*****************************************************************************/

#define ERROR_NO_ERROR					0
#define ERROR_NO_FILES_IN_FILE_LIST		101
#define ERROR_PANEL_WAS_NULL			102
#define ERROR_COULD_NOT_OPEN_DIR		107
#define ERROR_NO_CONNECTED_DRIVES_FOUND	110
#define ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ	125
#define ERROR_COULD_NOT_CREATE_OR_RESET_MEMSYS_OBJ	132
#define ERROR_COULD_NOT_INIT_SYSTEM		133


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

#define LIST_SCOPE_ALL				0
#define LIST_SCOPE_SELECTED			1
#define LIST_SCOPE_NOT_SELECTED		2

typedef enum device_number
{
	DEVICE_SD_CARD			= 0,
	DEVICE_FLOPPY_1			= 1,
	DEVICE_FLOPPY_2			= 2,
	DEVICE_MAX_DISK_DEVICE	= 3,
	DEVICE_MIN_MEMORY_DEVICE = 7,
	DEVICE_RAM				= 8,
	DEVICE_FLASH			= 9,
} device_number;


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

// Date/time for file entries
typedef struct DateTime {
	byte year;
	byte month;
	byte day;
	byte hour;
	byte min;
	byte sec;
} DateTime;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Progress bar
void App_ShowProgressBar(void);
void App_HideProgressBar(void);
void App_UpdateProgressBar(byte progress_bar_total);

// Display time
void App_DisplayTime(void);

// Error exit
void App_Exit(byte the_error_number);

// Filename storage - in oscar64, filenames are stored in a heap-allocated
// array instead of extended memory banks
char* App_GetFilename(byte panel_id, byte file_id);
void  App_SetFilename(byte panel_id, byte file_id, const char* the_filename);

// Communication buffer (forward declare)
void Buffer_NewMessage(const char *message);
void Buffer_Initialize(void);
void Buffer_Clear(void);


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

// Shared string buffers
extern char  global_string_buff1[256];
extern char  global_temp_path_1[FILE_MAX_PATHNAME_SIZE + 1];
extern char  global_temp_path_2[FILE_MAX_PATHNAME_SIZE + 1];

// Temporary screen save buffers for dialogs
extern byte  temp_screen_buffer_char[APP_DIALOG_BUFF_SIZE];
extern byte  temp_screen_buffer_attr[APP_DIALOG_BUFF_SIZE];

// Connected devices
extern int8_t  global_connected_device[DEVICE_MAX_DEVICE_COUNT];

// Flags
extern bool  global_clock_is_visible;
extern bool  global_find_next_enabled;

// Search phrase
#define MAX_SEARCH_PHRASE_LEN  32
extern char    global_search_phrase[MAX_SEARCH_PHRASE_LEN + 1];
extern char    global_search_phrase_human_readable[MAX_SEARCH_PHRASE_LEN + 1];
extern byte    global_search_phrase_len;


#endif /* FILE_MANAGER_H_ */
