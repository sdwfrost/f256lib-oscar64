/*
 * screen.c - UI layout, menus, rendering for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "screen.h"
#include "strings.h"
#include "file.h"

#include <string.h>
#include <stdio.h>


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

// Communication buffer storage
static char comm_buffer[COMM_BUFFER_NUM_ROWS][COMM_BUFFER_NUM_COLS + 1];

// UI button data
typedef struct UI_Button
{
	byte	id_;
	byte	x1_;
	byte	y1_;
	byte	string_id_;
	bool	active_;
	bool	changed_;
	byte	key_;
} UI_Button;

static UI_Button ui_buttons[NUM_BUTTONS] = {
	// Device buttons
	{ BUTTON_ID_DEV_SD_CARD,   UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y,     ID_STR_DEV_SD,        false, true, '0' },
	{ BUTTON_ID_DEV_FLOPPY_1,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 1, ID_STR_DEV_FLOPPY_1,  false, true, '1' },
	{ BUTTON_ID_DEV_FLOPPY_2,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 2, ID_STR_DEV_FLOPPY_2,  false, true, '2' },
	{ BUTTON_ID_DEV_RAM,       UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 3, ID_STR_DEV_RAM,       true,  true, '8' },
	{ BUTTON_ID_DEV_FLASH,     UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 4, ID_STR_DEV_FLASH,     true,  true, '9' },
	{ BUTTON_ID_REFRESH,       UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 5, ID_STR_DEV_REFRESH_LISTING, true,  true, 'R' },
	{ BUTTON_ID_FORMAT,        UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_CMD_Y + 6, ID_STR_DEV_FORMAT,    false, true, '"' },
	// Directory buttons
	{ BUTTON_ID_MAKE_DIR,      UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DIR_CMD_Y,     ID_STR_DEV_MAKE_DIR,  true,  true, 'm' },
	{ BUTTON_ID_SORT_BY_TYPE,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DIR_CMD_Y + 1, ID_STR_DEV_SORT_BY_TYPE, true, true, 'T' },
	{ BUTTON_ID_SORT_BY_NAME,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DIR_CMD_Y + 2, ID_STR_DEV_SORT_BY_NAME, true, true, 'N' },
	{ BUTTON_ID_SORT_BY_SIZE,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DIR_CMD_Y + 3, ID_STR_DEV_SORT_BY_SIZE, true, true, 'S' },
	// File buttons
	{ BUTTON_ID_COPY,          UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y,     ID_STR_FILE_COPY_RIGHT, true, true, 'c' },
	{ BUTTON_ID_DELETE,        UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 1, ID_STR_FILE_DELETE,  true,  true, 'x' },
	{ BUTTON_ID_DUPLICATE,     UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 2, ID_STR_FILE_DUP,    true,  true, 'p' },
	{ BUTTON_ID_RENAME,        UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 3, ID_STR_FILE_RENAME, true,  true, 'r' },
	{ BUTTON_ID_TEXT_VIEW,     UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 4, ID_STR_FILE_TEXT_PREVIEW, true, true, 't' },
	{ BUTTON_ID_HEX_VIEW,     UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 5, ID_STR_FILE_HEX_PREVIEW, true, true, 'h' },
	{ BUTTON_ID_LOAD,         UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 6, ID_STR_FILE_LOAD,   true,  true, 'l' },
	// Bank buttons
	{ BUTTON_ID_BANK_FILL,    UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 7, ID_STR_BANK_FILL,   false, true, 'F' },
	{ BUTTON_ID_BANK_CLEAR,   UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 8, ID_STR_BANK_CLEAR,  false, true, 'z' },
	{ BUTTON_ID_BANK_FIND,    UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 9, ID_STR_BANK_FIND,   false, true, 'f' },
	{ BUTTON_ID_BANK_FIND_NEXT, UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_CMD_Y + 10, ID_STR_BANK_FIND_NEXT, false, true, 'g' },
	// App buttons
	{ BUTTON_ID_SET_CLOCK,     UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_CMD_Y,     ID_STR_APP_SET_CLOCK, true,  true, 'C' },
	{ BUTTON_ID_ABOUT,        UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_CMD_Y + 1, ID_STR_APP_ABOUT,    true,  true, 'a' },
	{ BUTTON_ID_EXIT_TO_BASIC, UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_CMD_Y + 2, ID_STR_APP_EXIT_TO_BASIC, true, true, 'b' },
	{ BUTTON_ID_EXIT_TO_DOS,  UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_CMD_Y + 3, ID_STR_APP_EXIT_TO_DOS, true, true, 'd' },
	{ BUTTON_ID_QUIT,         UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_CMD_Y + 4, ID_STR_APP_QUIT,     true,  true, 'q' },
};


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

void Screen_DrawUI(void)
{
	// Clear entire screen
	textFillBox(0, 0, 79, 59, ' ', APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);

	// Draw title bar area
	textFillBox(0, 0, 79, 2, ' ', APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);
	textDrawStringAt(1, 1, GetString(ID_STR_ABOUT_FMANAGER), APP_FOREGROUND_COLOR, APP_BACKGROUND_COLOR);

	// Draw left panel box
	textDrawBox(UI_LEFT_PANEL_BODY_X1, UI_VIEW_PANEL_BODY_Y1,
				UI_LEFT_PANEL_BODY_X2, UI_VIEW_PANEL_BODY_Y2,
				PANEL_ACCENT_COLOR, APP_BACKGROUND_COLOR);

	// Draw right panel box
	textDrawBox(UI_RIGHT_PANEL_BODY_X1, UI_VIEW_PANEL_BODY_Y1,
				UI_RIGHT_PANEL_BODY_X2, UI_VIEW_PANEL_BODY_Y2,
				PANEL_ACCENT_COLOR, APP_BACKGROUND_COLOR);

	// Draw comm buffer frame
	textDrawBox(0, COMM_BUFFER_FIRST_ROW - 1, 79, COMM_BUFFER_LAST_ROW + 1,
				BUFFER_ACCENT_COLOR, BUFFER_BACKGROUND_COLOR);

	// Clear comm buffer area
	textFillBox(1, COMM_BUFFER_FIRST_ROW, 78, COMM_BUFFER_LAST_ROW,
				' ', BUFFER_FOREGROUND_COLOR, BUFFER_BACKGROUND_COLOR);

	// Draw menu section headers
	textDrawStringAt(UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DEV_MENU_Y, GetString(ID_STR_MENU_DEVICE), MENU_FOREGROUND_COLOR, MENU_BACKGROUND_COLOR);
	textDrawStringAt(UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_DIR_MENU_Y, GetString(ID_STR_MENU_DIRECTORY), MENU_FOREGROUND_COLOR, MENU_BACKGROUND_COLOR);
	textDrawStringAt(UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_FILE_MENU_Y, GetString(ID_STR_MENU_FILE), MENU_FOREGROUND_COLOR, MENU_BACKGROUND_COLOR);
	textDrawStringAt(UI_MIDDLE_AREA_START_X, UI_MIDDLE_AREA_APP_MENU_Y, GetString(ID_STR_MENU_APP), MENU_FOREGROUND_COLOR, MENU_BACKGROUND_COLOR);
}


void Screen_Render(void)
{
	Screen_DrawUI();
	Screen_RenderMenu(PARAM_RENDER_ALL_MENU_ITEMS);
	Buffer_RefreshDisplay();
}


void Screen_SwapCopyDirectionIndicator(void)
{
	// Toggle between "Copy <-" and "Copy ->" for the copy button
	byte current_id = ui_buttons[BUTTON_ID_COPY].string_id_;

	if (current_id == ID_STR_FILE_COPY_RIGHT)
		ui_buttons[BUTTON_ID_COPY].string_id_ = ID_STR_FILE_COPY_LEFT;
	else
		ui_buttons[BUTTON_ID_COPY].string_id_ = ID_STR_FILE_COPY_RIGHT;

	ui_buttons[BUTTON_ID_COPY].changed_ = true;
}


void Screen_SetInitialMenuStates(byte num_disk_systems)
{
	// Enable/disable device buttons based on connected drives
	ui_buttons[BUTTON_ID_DEV_SD_CARD].active_ = (num_disk_systems >= 1);
	ui_buttons[BUTTON_ID_DEV_FLOPPY_1].active_ = (num_disk_systems >= 2);
	ui_buttons[BUTTON_ID_DEV_FLOPPY_2].active_ = (num_disk_systems >= 3);

	// RAM and Flash always available
	ui_buttons[BUTTON_ID_DEV_RAM].active_ = true;
	ui_buttons[BUTTON_ID_DEV_FLASH].active_ = true;

	// Mark all as changed so they get drawn
	{
		byte i;
		for (i = 0; i < NUM_BUTTONS; i++)
			ui_buttons[i].changed_ = true;
	}
}


byte Screen_GetValidUserInput(void)
{
	byte user_input;
	byte i;

	// Wait for keyboard input via kernel events
	while (1)
	{
		kernelNextEvent();
		if (kernelError)
		{
			kernelCall(Yield);
			continue;
		}

		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;

		user_input = kernelEventData.u.key.ascii;
		if (user_input == 0) continue;

		// Check special navigation keys
		if (user_input == MOVE_UP || user_input == MOVE_DOWN ||
			user_input == MOVE_LEFT || user_input == MOVE_RIGHT ||
			user_input == ACTION_SWAP_ACTIVE_PANEL)
			return user_input;

		// Check against enabled menu items
		for (i = 0; i < NUM_BUTTONS; i++)
		{
			if (ui_buttons[i].key_ == user_input)
			{
				if (ui_buttons[i].active_)
					return user_input;
				else
					return ACTION_INVALID_INPUT;
			}
		}

		// Unknown key - return it for app to handle
		return user_input;
	}
}


void Screen_UpdateMenuStates(UI_Menu_Enabler_Info* the_enabling_info)
{
	byte i;
	bool new_state;

	for (i = 0; i < NUM_BUTTONS; i++)
	{
		new_state = ui_buttons[i].active_;

		// File-specific buttons: disable if no disk panel
		if (i >= BUTTON_ID_COPY && i <= BUTTON_ID_LOAD)
		{
			new_state = the_enabling_info->for_disk_;
		}

		// Bank-specific buttons
		if (i >= BUTTON_ID_BANK_FILL && i <= BUTTON_ID_BANK_FIND_NEXT)
		{
			new_state = !the_enabling_info->for_disk_ && !the_enabling_info->for_flash_;
		}

		// Format: only for disk, not meatloaf
		if (i == BUTTON_ID_FORMAT)
		{
			new_state = the_enabling_info->for_disk_ && !the_enabling_info->is_meatloaf_;
		}

		// mkdir: only for disk
		if (i == BUTTON_ID_MAKE_DIR)
		{
			new_state = the_enabling_info->for_disk_ && !the_enabling_info->is_meatloaf_;
		}

		if (new_state != ui_buttons[i].active_)
		{
			ui_buttons[i].active_ = new_state;
			ui_buttons[i].changed_ = true;
		}
	}
}


void Screen_RenderMenu(bool sparse_render)
{
	byte i;
	byte color;

	for (i = 0; i < NUM_BUTTONS; i++)
	{
		if (sparse_render && !ui_buttons[i].changed_)
			continue;

		color = ui_buttons[i].active_ ? MENU_FOREGROUND_COLOR : MENU_INACTIVE_COLOR;
		textDrawStringAt(ui_buttons[i].x1_, ui_buttons[i].y1_,
						 GetString(ui_buttons[i].string_id_),
						 color, MENU_BACKGROUND_COLOR);

		ui_buttons[i].changed_ = false;
	}
}


void Screen_UpdateSortIcons(byte the_panel_x, void* the_sort_compare_function)
{
	// Clear all sort icons first
	byte header_y = UI_VIEW_PANEL_HEADER_Y;
	byte name_x = the_panel_x + UI_PANEL_FILENAME_OFFSET + 4;
	byte type_x = the_panel_x + UI_PANEL_FILETYPE_OFFSET + 4;
	byte size_x = type_x + UI_PANEL_FILESIZE_OFFSET;

	textSetCharAt(name_x, header_y, ' ');
	textSetCharAt(type_x, header_y, ' ');
	textSetCharAt(size_x, header_y, ' ');

	// Draw the sort icon for the active sort
	{
		bool (*fn_name)(void*, void*) = &File_CompareName;
		bool (*fn_type)(void*, void*) = &File_CompareFileTypeID;
		bool (*fn_size)(void*, void*) = &File_CompareSize;

		if (the_sort_compare_function == (void*)fn_name)
			textSetCharAt(name_x, header_y, CH_SORT_ICON);
		else if (the_sort_compare_function == (void*)fn_type)
			textSetCharAt(type_x, header_y, CH_SORT_ICON);
		else if (the_sort_compare_function == (void*)fn_size)
			textSetCharAt(size_x, header_y, CH_SORT_ICON);
	}
}


void Screen_UpdateMeatloafIcon(byte the_panel_x, bool meatloaf_mode)
{
	(void)the_panel_x;
	(void)meatloaf_mode;
	// Placeholder - meatloaf mode indicator
}


void Screen_ShowAppAboutInfo(void)
{
	Buffer_NewMessage(GetString(ID_STR_ABOUT_FMANAGER));
	Buffer_NewMessage(GetString(ID_STR_ABOUT_GIT));

	if (platformIsAnyK())
		Buffer_NewMessage(GetString(ID_STR_MACHINE_K));
	else
		Buffer_NewMessage(GetString(ID_STR_MACHINE_JR));
}


void Screen_DrawPanelHeader(byte panel_id, bool for_disk)
{
	byte x1;
	byte y = UI_VIEW_PANEL_HEADER_Y;

	if (panel_id == PANEL_ID_LEFT)
		x1 = UI_LEFT_PANEL_BODY_X1 + 1;
	else
		x1 = UI_RIGHT_PANEL_BODY_X1 + 1;

	// Clear header row
	textFillBox(x1, y, x1 + UI_PANEL_INNER_WIDTH - 1, y, ' ', LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);

	if (for_disk)
	{
		textDrawStringAt(x1 + UI_PANEL_FILENAME_OFFSET, y, GetString(ID_STR_LBL_FILENAME), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
		textDrawStringAt(x1 + UI_PANEL_FILETYPE_OFFSET, y, GetString(ID_STR_LBL_FILETYPE), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
		textDrawStringAt(x1 + UI_PANEL_FILETYPE_OFFSET + UI_PANEL_FILESIZE_OFFSET, y, GetString(ID_STR_LBL_FILESIZE), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
	}
	else
	{
		textDrawStringAt(x1 + UI_PANEL_FILENAME_OFFSET, y, GetString(ID_STR_LBL_FILENAME), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
		textDrawStringAt(x1 + UI_PANEL_BANK_NUM_OFFSET, y, GetString(ID_STR_LBL_BANK_NUM), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
		textDrawStringAt(x1 + UI_PANEL_BANK_NUM_OFFSET + UI_PANEL_BANK_ADDR_OFFSET, y, GetString(ID_STR_LBL_BANK_ADDRESS), LIST_HEADER_COLOR, APP_BACKGROUND_COLOR);
	}
}


char* Screen_GetStringFromUser(char* dialog_title, char* dialog_body, char* starter_string, byte max_len)
{
	static char input_buffer[256];
	byte i;
	byte cursor_pos;
	byte key;
	byte dialog_x1, dialog_y1, dialog_x2, dialog_y2;
	byte input_x, input_y;

	// Calculate dialog position (centered)
	dialog_x1 = (80 - APP_DIALOG_WIDTH) / 2;
	dialog_y1 = 20;
	dialog_x2 = dialog_x1 + APP_DIALOG_WIDTH - 1;
	dialog_y2 = dialog_y1 + APP_DIALOG_HEIGHT - 1;
	input_x = dialog_x1 + 2;
	input_y = dialog_y1 + 4;

	// Save screen area
	textCopyBoxToBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);

	// Draw dialog box
	textFillBox(dialog_x1, dialog_y1, dialog_x2, dialog_y2, ' ', DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);
	textDrawBox(dialog_x1, dialog_y1, dialog_x2, dialog_y2, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);

	// Draw title and body
	textDrawStringAt(dialog_x1 + 2, dialog_y1 + 1, dialog_title, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);
	textDrawStringAt(dialog_x1 + 2, dialog_y1 + 3, dialog_body, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);

	// Copy starter string
	generalStrlcpy(input_buffer, starter_string, max_len + 1);
	cursor_pos = (byte)generalStrnlen(input_buffer, max_len);

	// Draw initial input
	textDrawStringAt(input_x, input_y, input_buffer, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);

	// Input loop
	while (1)
	{
		textGotoXY(input_x + cursor_pos, input_y);
		platformEnableTextCursor(true);

		kernelNextEvent();
		if (kernelError) { kernelCall(Yield); continue; }
		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;

		key = kernelEventData.u.key.ascii;
		if (key == 0) continue;

		platformEnableTextCursor(false);

		if (key == ACTION_CONFIRM)
		{
			// Restore screen and return input
			textCopyBoxFromBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);
			return input_buffer;
		}
		else if (key == ACTION_CANCEL)
		{
			// Restore screen and return NULL
			textCopyBoxFromBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);
			return NULL;
		}
		else if (key == 0x14 && cursor_pos > 0) // Backspace
		{
			cursor_pos--;
			input_buffer[cursor_pos] = 0;
			textSetCharAt(input_x + cursor_pos, input_y, ' ');
		}
		else if (key >= 32 && key < 127 && cursor_pos < max_len)
		{
			input_buffer[cursor_pos] = key;
			cursor_pos++;
			input_buffer[cursor_pos] = 0;
			textDrawStringAt(input_x, input_y, input_buffer, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);
		}
	}
}


bool Screen_ShowUserTwoButtonDialog(char* dialog_title, byte dialog_body_string_id, byte positive_btn_label_string_id, byte negative_btn_label_string_id)
{
	byte key;
	byte dialog_x1, dialog_y1, dialog_x2, dialog_y2;
	byte btn_y;

	dialog_x1 = (80 - APP_DIALOG_WIDTH) / 2;
	dialog_y1 = 20;
	dialog_x2 = dialog_x1 + APP_DIALOG_WIDTH - 1;
	dialog_y2 = dialog_y1 + APP_DIALOG_HEIGHT - 1;
	btn_y = dialog_y1 + 5;

	// Save screen area
	textCopyBoxToBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);

	// Draw dialog
	textFillBox(dialog_x1, dialog_y1, dialog_x2, dialog_y2, ' ', DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);
	textDrawBox(dialog_x1, dialog_y1, dialog_x2, dialog_y2, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);

	textDrawStringAt(dialog_x1 + 2, dialog_y1 + 1, dialog_title, DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);
	textDrawStringAt(dialog_x1 + 2, dialog_y1 + 3, GetString(dialog_body_string_id), DIALOG_FOREGROUND_COLOR, DIALOG_BACKGROUND_COLOR);

	// Draw buttons
	textDrawStringAt(dialog_x1 + 4, btn_y, GetString(positive_btn_label_string_id), DIALOG_AFFIRM_COLOR, DIALOG_BACKGROUND_COLOR);
	textDrawStringAt(dialog_x1 + 20, btn_y, GetString(negative_btn_label_string_id), DIALOG_CANCEL_COLOR, DIALOG_BACKGROUND_COLOR);

	// Wait for Y/N response
	while (1)
	{
		kernelNextEvent();
		if (kernelError) { kernelCall(Yield); continue; }
		if (kernelEventData.type != kernelEvent(key.PRESSED)) continue;

		key = kernelEventData.u.key.ascii;
		if (key == 'y' || key == 'Y' || key == ACTION_CONFIRM)
		{
			textCopyBoxFromBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);
			return true;
		}
		if (key == 'n' || key == 'N' || key == ACTION_CANCEL)
		{
			textCopyBoxFromBuffer(dialog_x1, dialog_y1, dialog_x2, dialog_y2, temp_screen_buffer_char, temp_screen_buffer_attr);
			return false;
		}
	}
}


/*****************************************************************************/
/*                    Communication Buffer Functions                          */
/*****************************************************************************/

static void Buffer_DrawLine(byte row_idx, byte screen_row)
{
	textDrawStringAt(COMM_BUFFER_FIRST_COL, screen_row, comm_buffer[row_idx], BUFFER_FOREGROUND_COLOR, BUFFER_BACKGROUND_COLOR);
}


void Buffer_Initialize(void)
{
	byte i;
	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++)
	{
		memset(comm_buffer[i], ' ', COMM_BUFFER_NUM_COLS);
		comm_buffer[i][COMM_BUFFER_NUM_COLS] = 0;
	}
}


void Buffer_Clear(void)
{
	Buffer_Initialize();
	textFillBox(COMM_BUFFER_FIRST_COL, COMM_BUFFER_FIRST_ROW,
				COMM_BUFFER_LAST_COL, COMM_BUFFER_LAST_ROW,
				' ', BUFFER_FOREGROUND_COLOR, BUFFER_BACKGROUND_COLOR);
}


void Buffer_RefreshDisplay(void)
{
	byte i;
	for (i = 0; i < COMM_BUFFER_NUM_ROWS; i++)
		Buffer_DrawLine(i, COMM_BUFFER_FIRST_ROW + i);
}


void Buffer_NewMessage(const char *message)
{
	byte i;

	// Scroll buffer lines up
	for (i = 0; i < COMM_BUFFER_NUM_ROWS - 1; i++)
		memcpy(comm_buffer[i], comm_buffer[i + 1], COMM_BUFFER_NUM_COLS + 1);

	// Copy new message into bottom row, pad with spaces
	{
		byte last = COMM_BUFFER_NUM_ROWS - 1;
		int16_t msg_len = generalStrnlen(message, COMM_BUFFER_NUM_COLS);
		int16_t j;

		for (j = 0; j < msg_len && j < COMM_BUFFER_NUM_COLS; j++)
			comm_buffer[last][j] = message[j];
		for (; j < COMM_BUFFER_NUM_COLS; j++)
			comm_buffer[last][j] = ' ';
		comm_buffer[last][COMM_BUFFER_NUM_COLS] = 0;
	}

	Buffer_RefreshDisplay();
}
