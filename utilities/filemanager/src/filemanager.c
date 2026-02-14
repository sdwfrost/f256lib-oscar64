/*
 * filemanager.c - Main entry point for the F256 File Manager
 * Ported from F256-FileManager CC65 version
 *
 * A dual-panel file commander for the F256K, supporting:
 * - SD card and IEC floppy drives
 * - RAM and Flash bank browsing
 * - File copy, delete, rename, view (hex/text)
 * - Directory creation and navigation
 * - Memory bank fill, clear, and search
 *
 * Code overlay configuration:
 *   Overlay 1 (bank 8): screen.c     - UI drawing, menus, dialogs
 *   Overlay 2 (bank 9): list_panel.c - Panel controller, file operations
 *   Overlay 3 (bank 10): overlay_em.c - Hex/text viewer, memory search
 */

#include "app.h"
#include "bank.h"
#include "file.h"
#include "folder.h"
#include "list.h"
#include "list_panel.h"
#include "memsys.h"
#include "overlay_em.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// Overlay memory configuration
// ---------------------------------------------------------------------------

#define OVERLAY_MMU_REG  MMU_MEM_BANK_5

#define OVL_SCREEN_BLOCK     8   // Physical 0x10000
#define OVL_PANEL_BLOCK      9   // Physical 0x12000
#define OVL_EM_BLOCK        10   // Physical 0x14000

// Overlay 1: screen.c - stored at physical 0x10000, runs at 0xA000
#pragma section( ovl_screen_code, 0 )
#pragma region( ovl_screen, 0x10000, 0x12000, , 1, { ovl_screen_code }, 0xA000 )

// Overlay 2: list_panel.c - stored at physical 0x12000, runs at 0xA000
#pragma section( ovl_panel_code, 0 )
#pragma region( ovl_panel, 0x12000, 0x14000, , 2, { ovl_panel_code }, 0xA000 )

// Overlay 3: overlay_em.c - stored at physical 0x14000, runs at 0xA000
#pragma section( ovl_em_code, 0 )
#pragma region( ovl_em, 0x14000, 0x16000, , 3, { ovl_em_code }, 0xA000 )


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define CH_PROGRESS_BAR_FULL		CH_CHECKERBOARD
#define PROGRESS_BAR_Y				(COMM_BUFFER_FIRST_ROW - 4)
#define PROGRESS_BAR_START_X		UI_MIDDLE_AREA_START_X
#define PROGRESS_BAR_WIDTH			10
#define PROGRESS_BAR_DIVISOR		8
#define COLOR_PROGRESS_BAR			AQUAMARINE


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

static UI_Menu_Enabler_Info		app_menu_enabler;
static byte						app_active_panel_id;
static byte						app_connected_drive_count;

static byte app_progress_bar_char[8] = {
	' ',
	CH_PROGRESS_BAR_CHECKER_CH1,
	CH_PROGRESS_BAR_CHECKER_CH1 + 1,
	CH_PROGRESS_BAR_CHECKER_CH1 + 2,
	CH_PROGRESS_BAR_CHECKER_CH1 + 3,
	CH_PROGRESS_BAR_CHECKER_CH1 + 4,
	CH_PROGRESS_BAR_CHECKER_CH1 + 5,
	CH_PROGRESS_BAR_CHECKER_CH1 + 6,
};


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

// Shared string buffers
char	global_string_buff1[256];
char	global_temp_path_1[FILE_MAX_PATHNAME_SIZE + 1];
char	global_temp_path_2[FILE_MAX_PATHNAME_SIZE + 1];

// Temporary screen save buffers for dialogs
byte	temp_screen_buffer_char[APP_DIALOG_BUFF_SIZE];
byte	temp_screen_buffer_attr[APP_DIALOG_BUFF_SIZE];

// Connected devices
int8_t	global_connected_device[DEVICE_MAX_DEVICE_COUNT];

// Flags
bool	global_clock_is_visible;
bool	global_find_next_enabled = false;

// Search phrase
char	global_search_phrase[MAX_SEARCH_PHRASE_LEN + 1];
char	global_search_phrase_human_readable[MAX_SEARCH_PHRASE_LEN + 1];
byte	global_search_phrase_len;

// Panel instances
static WB2KViewPanel	app_file_panel[NUM_PANELS];

// Filename storage - heap-allocated arrays
// Two panels, each with up to 256 filenames of FILE_MAX_FILENAME_SIZE bytes
static char*	filename_storage[NUM_PANELS] = { NULL, NULL };
#define FILENAME_MAX_FILES	256


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static void App_SwapActivePanel(void);
static int8_t App_ScanDevices(void);
static void App_Initialize(void);
static void App_InitializePanelForDisk(byte panel_id);
static void App_InitializePanelForMemory(byte panel_id, bool for_flash);
static byte App_MainLoop(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


static void App_SwapActivePanel(void)
{
	Panel_ToggleActiveState(&app_file_panel[PANEL_ID_LEFT]);
	Panel_ToggleActiveState(&app_file_panel[PANEL_ID_RIGHT]);
	++app_active_panel_id;
	app_active_panel_id = app_active_panel_id % 2;
}


static int8_t App_ScanDevices(void)
{
	byte	drive_num = 0;
	byte	device;
	char	drive_path[4];
	char*	dir_handle;

	app_connected_drive_count = 0;

	for (device = DEVICE_LOWEST_DEVICE_NUM; device <= DEVICE_HIGHEST_DEVICE_NUM; device++)
	{
		sprintf(drive_path, "%u:", device);

		// Try to open the directory to see if device is present
		dir_handle = fileOpenDir(drive_path);
		if (dir_handle != NULL)
		{
			global_connected_device[drive_num] = device;
			++drive_num;
			fileCloseDir(dir_handle);
		}
	}

	return drive_num;
}


static void App_Initialize(void)
{
	Buffer_Clear();

	// Show about info
	Screen_ShowAppAboutInfo();

	// Scan which devices are connected
	Buffer_NewMessage(GetString(ID_STR_MSG_SCANNING));
	app_connected_drive_count = App_ScanDevices();

	sprintf(global_string_buff1, GetString(ID_STR_MSG_SHOW_DRIVE_COUNT), app_connected_drive_count);
	Buffer_NewMessage(global_string_buff1);

	// Tell Screen which disk systems are available
	Screen_SetInitialMenuStates(app_connected_drive_count);

	// Left panel always starts as active
	app_active_panel_id = PANEL_ID_LEFT;

	if (app_connected_drive_count < 1)
	{
		Buffer_NewMessage(GetString(ID_STR_MSG_NO_DRIVES_AVAILABLE));
		App_InitializePanelForMemory(PANEL_ID_LEFT, false);
		App_InitializePanelForMemory(PANEL_ID_RIGHT, true);
	}
	else
	{
		App_InitializePanelForDisk(PANEL_ID_LEFT);

		if (app_connected_drive_count > 1)
			App_InitializePanelForDisk(PANEL_ID_RIGHT);
		else
			App_InitializePanelForMemory(PANEL_ID_RIGHT, false);
	}

	app_file_panel[PANEL_ID_LEFT].active_ = true;
	app_file_panel[PANEL_ID_RIGHT].active_ = false;
}


static void App_InitializePanelForDisk(byte panel_id)
{
	byte	the_drive_index = panel_id;
	byte	panel_x_offset;
	char	drive_path[4];

	sprintf(drive_path, "%u:", global_connected_device[the_drive_index]);

	if ((app_file_panel[panel_id].root_folder_ = Folder_NewOrReset(app_file_panel[panel_id].root_folder_, global_connected_device[the_drive_index], drive_path)) == NULL)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
		App_Exit(ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ);
	}

	panel_x_offset = UI_RIGHT_PANEL_X_DELTA * panel_id;

	Panel_Initialize(
		panel_id,
		&app_file_panel[panel_id],
		PARAM_INITIALIZE_FOR_DISK,
		(UI_LEFT_PANEL_BODY_X1 + panel_x_offset + 1), (UI_VIEW_PANEL_BODY_Y1 + 2),
		(UI_VIEW_PANEL_BODY_WIDTH - 2), (UI_VIEW_PANEL_BODY_HEIGHT - 3)
	);

	Panel_SetCurrentDevice(&app_file_panel[panel_id], global_connected_device[the_drive_index]);
}


static void App_InitializePanelForMemory(byte panel_id, bool for_flash)
{
	byte panel_x_offset;

	if ((app_file_panel[panel_id].memory_system_ = MemSys_NewOrReset(app_file_panel[panel_id].memory_system_, for_flash)) == NULL)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
		App_Exit(ERROR_COULD_NOT_CREATE_OR_RESET_MEMSYS_OBJ);
	}

	panel_x_offset = UI_RIGHT_PANEL_X_DELTA * panel_id;

	Panel_Initialize(
		panel_id,
		&app_file_panel[panel_id],
		PARAM_INITIALIZE_FOR_MEMORY,
		(UI_LEFT_PANEL_BODY_X1 + panel_x_offset + 1), (UI_VIEW_PANEL_BODY_Y1 + 2),
		(UI_VIEW_PANEL_BODY_WIDTH - 2), (UI_VIEW_PANEL_BODY_HEIGHT - 3)
	);
}


static byte App_MainLoop(void)
{
	byte			user_input;
	byte			new_device_num;
	bool			exit_main_loop = false;
	bool			success;
	WB2KViewPanel*	the_panel;

	while (exit_main_loop == false)
	{
		the_panel = &app_file_panel[app_active_panel_id];

		do
		{
			// Collect info for menu enabling/disabling
			if (the_panel->for_disk_ == true)
			{
				app_menu_enabler.file_type_ = Folder_GetCurrentFileType(the_panel->root_folder_);
				app_menu_enabler.for_flash_ = false;
				app_menu_enabler.is_kup_ = false;
				app_menu_enabler.is_meatloaf_ = the_panel->root_folder_->is_meatloaf_;
			}
			else
			{
				app_menu_enabler.file_type_ = 0;
				app_menu_enabler.for_flash_ = the_panel->memory_system_->is_flash_;
				app_menu_enabler.is_kup_ = MemSys_GetCurrentRowKUPState(the_panel->memory_system_);
				app_menu_enabler.is_meatloaf_ = false;
			}

			app_menu_enabler.for_disk_ = the_panel->for_disk_;
			app_menu_enabler.other_panel_for_disk_ = app_file_panel[(app_active_panel_id + 1) % 2].for_disk_;

			// Safe access to other panel's properties
			if (app_file_panel[(app_active_panel_id + 1) % 2].memory_system_ != NULL)
				app_menu_enabler.other_panel_for_flash_ = app_file_panel[(app_active_panel_id + 1) % 2].memory_system_->is_flash_;
			else
				app_menu_enabler.other_panel_for_flash_ = false;

			if (app_file_panel[(app_active_panel_id + 1) % 2].root_folder_ != NULL)
				app_menu_enabler.other_panel_is_meatloaf_ = app_file_panel[(app_active_panel_id + 1) % 2].root_folder_->is_meatloaf_;
			else
				app_menu_enabler.other_panel_is_meatloaf_ = false;

			Screen_UpdateMenuStates(&app_menu_enabler);
			Screen_RenderMenu(PARAM_ONLY_RENDER_CHANGED_ITEMS);
			user_input = Screen_GetValidUserInput();

			switch (user_input)
			{
				case ACTION_INVALID_INPUT:
					break;

				case ACTION_SORT_BY_NAME:
					the_panel->sort_compare_function_ = (void*)&File_CompareName;
					Panel_SortAndDisplay(the_panel);
					break;

				case ACTION_SORT_BY_SIZE:
					the_panel->sort_compare_function_ = (void*)&File_CompareSize;
					Panel_SortAndDisplay(the_panel);
					break;

				case ACTION_SORT_BY_TYPE:
					the_panel->sort_compare_function_ = (void*)&File_CompareFileTypeID;
					Panel_SortAndDisplay(the_panel);
					break;

				case ACTION_VIEW_AS_HEX:
					global_clock_is_visible = false;
					success = Panel_ViewCurrentFile(the_panel, PARAM_VIEW_AS_HEX);
					Screen_Render();
					Screen_RenderMenu(PARAM_RENDER_ALL_MENU_ITEMS);
					Panel_RenderContents(&app_file_panel[PANEL_ID_LEFT]);
					Panel_RenderContents(&app_file_panel[PANEL_ID_RIGHT]);
					break;

				case ACTION_VIEW_AS_TEXT:
					global_clock_is_visible = false;
					success = Panel_ViewCurrentFile(the_panel, PARAM_VIEW_AS_TEXT);
					Screen_Render();
					Screen_RenderMenu(PARAM_RENDER_ALL_MENU_ITEMS);
					Panel_RenderContents(&app_file_panel[PANEL_ID_LEFT]);
					Panel_RenderContents(&app_file_panel[PANEL_ID_RIGHT]);
					break;

				case ACTION_COPY:
					success = Panel_CopyCurrentFile(the_panel, &app_file_panel[(app_active_panel_id + 1) % 2]);
					break;

				case ACTION_DUPLICATE:
					success = Panel_CopyCurrentFile(the_panel, the_panel);
					break;

				case ACTION_RENAME:
					success = Panel_RenameCurrentFile(the_panel);
					break;

				case ACTION_DELETE:
				case ACTION_DELETE_ALT:
					success = Panel_DeleteCurrentFile(the_panel);
					break;

				case ACTION_SELECT:
				case ACTION_LOAD:
					success = Panel_OpenCurrentFileOrFolder(the_panel);
					break;

				case ACTION_FILL_MEMORY:
					success = Panel_FillCurrentBank(the_panel);
					break;

				case ACTION_CLEAR_MEMORY:
					success = Panel_ClearCurrentBank(the_panel);
					break;

				case ACTION_SEARCH_MEMORY:
					success = Panel_SearchCurrentBank(the_panel);
					break;

				case ACTION_SEARCH_MEMORY_NEXT:
					success = global_find_next_enabled = EM_SearchMemory(0, 0, 0, PARAM_START_AFTER_LAST_HIT);
					break;

				case ACTION_SWAP_ACTIVE_PANEL:
					App_SwapActivePanel();
					Screen_SwapCopyDirectionIndicator();
					the_panel = &app_file_panel[app_active_panel_id];
					break;

				case ACTION_SWITCH_TO_SD:
				case ACTION_SWITCH_TO_FLOPPY_1:
				case ACTION_SWITCH_TO_FLOPPY_2:
				case ACTION_SWITCH_TO_RAM:
				case ACTION_SWITCH_TO_FLASH:
					new_device_num = user_input - '0';
					success = Panel_SwitchDevice(the_panel, new_device_num);
					break;

				case ACTION_REFRESH_PANEL:
					Panel_Refresh(the_panel);
					break;

				case ACTION_FORMAT_DISK:
					success = Panel_FormatDrive(the_panel);
					if (success)
						Panel_Refresh(the_panel);
					break;

				case ACTION_LOAD_MEATLOAF_URL:
					success = Panel_OpenMeatloafURL(the_panel);
					break;

				case ACTION_NEW_FOLDER:
					success = Panel_MakeDir(the_panel);
					break;

				case ACTION_SET_TIME:
				{
					char* time_str;
					generalStrlcpy(global_string_buff1, GetString(ID_STR_DLG_SET_CLOCK_TITLE), APP_DIALOG_WIDTH);
					global_temp_path_2[0] = 0;

					time_str = Screen_GetStringFromUser(
						global_string_buff1,
						(char*)GetString(ID_STR_DLG_SET_CLOCK_BODY),
						global_temp_path_2,
						14
					);

					if (time_str != NULL)
					{
						// TODO: parse time string and set RTC via rtcWrite()
						App_DisplayTime();
					}
					break;
				}

				case ACTION_ABOUT:
					Screen_ShowAppAboutInfo();
					break;

				case MOVE_UP:
					success = Panel_SelectPrevFile(the_panel);
					break;

				case MOVE_DOWN:
					success = Panel_SelectNextFile(the_panel);
					break;

				case MOVE_LEFT:
					if (app_active_panel_id == PANEL_ID_RIGHT)
					{
						App_SwapActivePanel();
						Screen_SwapCopyDirectionIndicator();
						the_panel = &app_file_panel[app_active_panel_id];
					}
					break;

				case MOVE_RIGHT:
					if (app_active_panel_id == PANEL_ID_LEFT)
					{
						App_SwapActivePanel();
						Screen_SwapCopyDirectionIndicator();
						the_panel = &app_file_panel[app_active_panel_id];
					}
					break;

				case ACTION_EXIT_TO_BASIC:
					kernelCall(RunNamed);
					success = false;
					break;

				case ACTION_EXIT_TO_DOS:
					kernelCall(RunNamed);
					success = false;
					break;

				case ACTION_QUIT:
					if (Screen_ShowUserTwoButtonDialog(
						(char*)GetString(ID_STR_DLG_ARE_YOU_SURE),
						ID_STR_DLG_QUIT_CONFIRM,
						ID_STR_DLG_YES,
						ID_STR_DLG_NO) == true)
					{
						exit_main_loop = true;
						continue;
					}
					break;

				default:
					user_input = ACTION_INVALID_INPUT;
					break;
			}
		} while (user_input == ACTION_INVALID_INPUT);
	}

	return 0;
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


void App_ShowProgressBar(void)
{
	textDrawHLine(UI_MIDDLE_AREA_START_X, PROGRESS_BAR_Y - 1, UI_MIDDLE_AREA_WIDTH, 196, MENU_ACCENT_COLOR, APP_BACKGROUND_COLOR, TEXT_DRAW_CHAR_AND_ATTR);
	textFillBox(UI_MIDDLE_AREA_START_X, PROGRESS_BAR_Y, UI_MIDDLE_AREA_START_X + UI_MIDDLE_AREA_WIDTH - 1, PROGRESS_BAR_Y, ' ', MENU_ACCENT_COLOR, APP_BACKGROUND_COLOR);
	textDrawHLine(UI_MIDDLE_AREA_START_X, PROGRESS_BAR_Y + 1, UI_MIDDLE_AREA_WIDTH, 196, MENU_ACCENT_COLOR, APP_BACKGROUND_COLOR, TEXT_DRAW_CHAR_AND_ATTR);
}


void App_HideProgressBar(void)
{
	textFillBox(UI_MIDDLE_AREA_START_X, PROGRESS_BAR_Y - 1, UI_MIDDLE_AREA_START_X + UI_MIDDLE_AREA_WIDTH - 1, PROGRESS_BAR_Y + 1, ' ', MENU_ACCENT_COLOR, APP_BACKGROUND_COLOR);
}


void App_UpdateProgressBar(byte progress_bar_total)
{
	byte	i;
	byte	full_blocks;
	byte	the_char_code;
	byte	progress_bar_char_index;

	if (progress_bar_total > 100)
		progress_bar_total = 100;

	full_blocks = progress_bar_total / PROGRESS_BAR_DIVISOR;
	progress_bar_char_index = progress_bar_total % PROGRESS_BAR_DIVISOR;

	for (i = 0; i < PROGRESS_BAR_WIDTH; i++)
	{
		if (i < full_blocks)
			the_char_code = CH_PROGRESS_BAR_FULL;
		else if (i == full_blocks && progress_bar_char_index > 0)
			the_char_code = app_progress_bar_char[progress_bar_char_index];
		else
			the_char_code = ' ';

		textSetCharAndAttrAt(PROGRESS_BAR_START_X + i, PROGRESS_BAR_Y, the_char_code, (COLOR_PROGRESS_BAR << 4) | APP_BACKGROUND_COLOR);
	}
}


void App_DisplayTime(void)
{
	byte old_rtc_control;

	if (global_clock_is_visible != true)
		return;

	// Read RTC registers (need I/O page for register access)
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	old_rtc_control = PEEK(RTC_CTRL);
	POKE(RTC_CTRL, old_rtc_control | 0x08);  // Stop RTC updates

	sprintf(global_string_buff1, "20%02X-%02X-%02X %02X:%02X",
		PEEK(RTC_YEAR), PEEK(RTC_MONTH), PEEK(RTC_DAY),
		PEEK(RTC_HOURS), PEEK(RTC_MINS));

	POKE(RTC_CTRL, old_rtc_control);

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_TEXT);

	textDrawStringAt(64, 3, global_string_buff1, YELLOW, BLACK);
}


void App_Exit(byte the_error_number)
{
	if (the_error_number != ERROR_NO_ERROR)
	{
		sprintf(global_string_buff1, GetString(ID_STR_MSG_FATAL_ERROR), the_error_number);
		Screen_ShowUserTwoButtonDialog(
			global_string_buff1,
			ID_STR_MSG_FATAL_ERROR_BODY,
			ID_STR_DLG_OK,
			ID_STR_DLG_OK
		);
	}

	// Free filename storage
	if (filename_storage[0] != NULL) { free(filename_storage[0]); filename_storage[0] = NULL; }
	if (filename_storage[1] != NULL) { free(filename_storage[1]); filename_storage[1] = NULL; }

	// Reset vector jump (exit to kernel)
	POKE(0xD6A2, 0xDE);
	POKE(0xD6A3, 0xAD);
	POKE(0xD6A0, 0xF0);
	POKE(0xD6A0, 0x00);
	// The above triggers a system reset
	while (1) {}  // Should not reach here
}


char* App_GetFilename(byte panel_id, byte file_id)
{
	if (panel_id >= NUM_PANELS) return "";
	if (filename_storage[panel_id] == NULL) return "";

	return &filename_storage[panel_id][(uint16_t)file_id * FILE_MAX_FILENAME_SIZE];
}


void App_SetFilename(byte panel_id, byte file_id, const char* the_filename)
{
	if (panel_id >= NUM_PANELS) return;

	// Allocate storage on first use
	if (filename_storage[panel_id] == NULL)
	{
		filename_storage[panel_id] = (char*)malloc((uint16_t)FILENAME_MAX_FILES * FILE_MAX_FILENAME_SIZE);
		if (filename_storage[panel_id] == NULL) return;
		memset(filename_storage[panel_id], 0, (uint16_t)FILENAME_MAX_FILES * FILE_MAX_FILENAME_SIZE);
	}

	generalStrlcpy(
		&filename_storage[panel_id][(uint16_t)file_id * FILE_MAX_FILENAME_SIZE],
		the_filename,
		FILE_MAX_FILENAME_SIZE
	);
}


/*****************************************************************************/
/*                         Communication Buffer                              */
/*****************************************************************************/

// These are defined in screen.c but declared in app.h for global access.
// The actual implementations are in screen.c - we just need the forward
// declarations in app.h so other modules can call Buffer_NewMessage etc.
// (screen.c provides the implementations)


/*****************************************************************************/
/*                            Main Entry Point                               */
/*****************************************************************************/

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	// Set border to 0 for full 80x60 text
	platformSetBorderSize(0, 0);

	// Initialize the communication buffer
	Buffer_Initialize();

	// Clear screen and draw main UI
	Screen_Render();

	// Initialize panels (scan devices, set up folders/memsys)
	App_Initialize();

	// Refresh both panels to show initial content
	Panel_Refresh(&app_file_panel[PANEL_ID_LEFT]);
	Panel_Refresh(&app_file_panel[PANEL_ID_RIGHT]);

	// Show the clock
	global_clock_is_visible = true;
	App_DisplayTime();

	// Enter main event loop
	App_MainLoop();

	// Exit
	App_Exit(ERROR_NO_ERROR);

	return 0;
}


// ---------------------------------------------------------------------------
// Overlay code sections
// ---------------------------------------------------------------------------
// Each overlay .c file is #included with its public function names
// redirected to FAR_ prefixed versions via #define.  After the include,
// trampolines in main memory swap the MMU bank and call the FAR_ function.

// ===== Overlay 1: screen.c (bank 8) =====
#define Screen_Render FAR_Screen_Render
#define Screen_DrawUI FAR_Screen_DrawUI
#define Screen_SwapCopyDirectionIndicator FAR_Screen_SwapCopyDirectionIndicator
#define Screen_SetInitialMenuStates FAR_Screen_SetInitialMenuStates
#define Screen_GetValidUserInput FAR_Screen_GetValidUserInput
#define Screen_UpdateMenuStates FAR_Screen_UpdateMenuStates
#define Screen_RenderMenu FAR_Screen_RenderMenu
#define Screen_UpdateSortIcons FAR_Screen_UpdateSortIcons
#define Screen_UpdateMeatloafIcon FAR_Screen_UpdateMeatloafIcon
#define Screen_ShowAppAboutInfo FAR_Screen_ShowAppAboutInfo
#define Screen_DrawPanelHeader FAR_Screen_DrawPanelHeader
#define Screen_GetStringFromUser FAR_Screen_GetStringFromUser
#define Screen_ShowUserTwoButtonDialog FAR_Screen_ShowUserTwoButtonDialog
#define Buffer_RefreshDisplay FAR_Buffer_RefreshDisplay
#define Buffer_NewMessage FAR_Buffer_NewMessage
#define Buffer_Initialize FAR_Buffer_Initialize
#define Buffer_Clear FAR_Buffer_Clear

// Forward declarations (macros expand these to FAR_ versions for internal calls)
void Screen_Render(void);
void Screen_DrawUI(void);
void Screen_SwapCopyDirectionIndicator(void);
void Screen_SetInitialMenuStates(byte num_disk_systems);
byte Screen_GetValidUserInput(void);
void Screen_UpdateMenuStates(UI_Menu_Enabler_Info* the_enabling_info);
void Screen_RenderMenu(bool sparse_render);
void Screen_UpdateSortIcons(byte the_panel_x, void* the_sort_compare_function);
void Screen_UpdateMeatloafIcon(byte the_panel_x, bool meatloaf_mode);
void Screen_ShowAppAboutInfo(void);
void Screen_DrawPanelHeader(byte panel_id, bool for_disk);
char* Screen_GetStringFromUser(char* dialog_title, char* dialog_body, char* starter_string, byte max_len);
bool Screen_ShowUserTwoButtonDialog(char* dialog_title, byte dialog_body_string_id, byte positive_btn_label_string_id, byte negative_btn_label_string_id);
void Buffer_RefreshDisplay(void);
void Buffer_NewMessage(const char *message);
void Buffer_Initialize(void);
void Buffer_Clear(void);

#pragma code( ovl_screen_code )
#include "screen.c"
#pragma code( code )

#undef Screen_Render
#undef Screen_DrawUI
#undef Screen_SwapCopyDirectionIndicator
#undef Screen_SetInitialMenuStates
#undef Screen_GetValidUserInput
#undef Screen_UpdateMenuStates
#undef Screen_RenderMenu
#undef Screen_UpdateSortIcons
#undef Screen_UpdateMeatloafIcon
#undef Screen_ShowAppAboutInfo
#undef Screen_DrawPanelHeader
#undef Screen_GetStringFromUser
#undef Screen_ShowUserTwoButtonDialog
#undef Buffer_RefreshDisplay
#undef Buffer_NewMessage
#undef Buffer_Initialize
#undef Buffer_Clear

// ===== Overlay 2: list_panel.c (bank 9) =====
#define Panel_Initialize FAR_Panel_Initialize
#define Panel_SwitchDevice FAR_Panel_SwitchDevice
#define Panel_SetCurrentDevice FAR_Panel_SetCurrentDevice
#define Panel_ToggleActiveState FAR_Panel_ToggleActiveState
#define Panel_MakeDir FAR_Panel_MakeDir
#define Panel_FormatDrive FAR_Panel_FormatDrive
#define Panel_Refresh FAR_Panel_Refresh
#define Panel_RenameCurrentFile FAR_Panel_RenameCurrentFile
#define Panel_DeleteCurrentFile FAR_Panel_DeleteCurrentFile
#define Panel_OpenCurrentFileOrFolder FAR_Panel_OpenCurrentFileOrFolder
#define Panel_CopyCurrentFile FAR_Panel_CopyCurrentFile
#define Panel_ViewCurrentFile FAR_Panel_ViewCurrentFile
#define Panel_SelectPrevFile FAR_Panel_SelectPrevFile
#define Panel_SelectNextFile FAR_Panel_SelectNextFile
#define Panel_SetFileSelectionByRow FAR_Panel_SetFileSelectionByRow
#define Panel_ReflowContent FAR_Panel_ReflowContent
#define Panel_ClearDisplay FAR_Panel_ClearDisplay
#define Panel_RenderContents FAR_Panel_RenderContents
#define Panel_SortAndDisplay FAR_Panel_SortAndDisplay
#define Panel_FillCurrentBank FAR_Panel_FillCurrentBank
#define Panel_ClearCurrentBank FAR_Panel_ClearCurrentBank
#define Panel_SearchCurrentBank FAR_Panel_SearchCurrentBank
#define Panel_OpenMeatloafURL FAR_Panel_OpenMeatloafURL

// Forward declarations (macros expand these to FAR_ versions for internal calls)
void Panel_Initialize(byte the_panel_id, WB2KViewPanel* the_panel, bool for_disk, byte x, byte y, byte width, byte height);
bool Panel_SwitchDevice(WB2KViewPanel* the_panel, device_number the_device);
void Panel_SetCurrentDevice(WB2KViewPanel* the_panel, device_number the_device_num);
void Panel_ToggleActiveState(WB2KViewPanel* the_panel);
bool Panel_MakeDir(WB2KViewPanel* the_panel);
bool Panel_FormatDrive(WB2KViewPanel* the_panel);
bool Panel_Refresh(WB2KViewPanel* the_panel);
bool Panel_RenameCurrentFile(WB2KViewPanel* the_panel);
bool Panel_DeleteCurrentFile(WB2KViewPanel* the_panel);
bool Panel_OpenCurrentFileOrFolder(WB2KViewPanel* the_panel);
bool Panel_CopyCurrentFile(WB2KViewPanel* the_panel, WB2KViewPanel* the_other_panel);
bool Panel_ViewCurrentFile(WB2KViewPanel* the_panel, byte the_viewer_type);
bool Panel_SelectPrevFile(WB2KViewPanel* the_panel);
bool Panel_SelectNextFile(WB2KViewPanel* the_panel);
bool Panel_SetFileSelectionByRow(WB2KViewPanel* the_panel, uint16_t the_row, bool do_selection);
void Panel_ReflowContent(WB2KViewPanel* the_panel);
void Panel_ClearDisplay(WB2KViewPanel* the_panel);
void Panel_RenderContents(WB2KViewPanel* the_panel);
void Panel_SortAndDisplay(WB2KViewPanel* the_panel);
bool Panel_FillCurrentBank(WB2KViewPanel* the_panel);
bool Panel_ClearCurrentBank(WB2KViewPanel* the_panel);
bool Panel_SearchCurrentBank(WB2KViewPanel* the_panel);
bool Panel_OpenMeatloafURL(WB2KViewPanel* the_panel);

#pragma code( ovl_panel_code )
#include "list_panel.c"
#pragma code( code )

#undef Panel_Initialize
#undef Panel_SwitchDevice
#undef Panel_SetCurrentDevice
#undef Panel_ToggleActiveState
#undef Panel_MakeDir
#undef Panel_FormatDrive
#undef Panel_Refresh
#undef Panel_RenameCurrentFile
#undef Panel_DeleteCurrentFile
#undef Panel_OpenCurrentFileOrFolder
#undef Panel_CopyCurrentFile
#undef Panel_ViewCurrentFile
#undef Panel_SelectPrevFile
#undef Panel_SelectNextFile
#undef Panel_SetFileSelectionByRow
#undef Panel_ReflowContent
#undef Panel_ClearDisplay
#undef Panel_RenderContents
#undef Panel_SortAndDisplay
#undef Panel_FillCurrentBank
#undef Panel_ClearCurrentBank
#undef Panel_SearchCurrentBank
#undef Panel_OpenMeatloafURL

// ===== Overlay 3: overlay_em.c (bank 10) =====
#define EM_DisplayAsText FAR_EM_DisplayAsText
#define EM_DisplayAsHex FAR_EM_DisplayAsHex
#define EM_SearchMemory FAR_EM_SearchMemory

// Forward declarations (macros expand these to FAR_ versions for internal calls)
void EM_DisplayAsText(byte em_bank_num, byte num_pages, char* the_name);
void EM_DisplayAsHex(byte em_bank_num, byte num_pages, char* the_name);
bool EM_SearchMemory(byte start_bank, byte start_page, byte start_byte, bool new_search);

#pragma code( ovl_em_code )
#include "overlay_em.c"
#pragma code( code )

#undef EM_DisplayAsText
#undef EM_DisplayAsHex
#undef EM_SearchMemory


// ---------------------------------------------------------------------------
// Trampolines (always in main memory)
// ---------------------------------------------------------------------------
// Each trampoline saves the current bank 5 mapping, switches to the overlay's
// physical block, calls the FAR_ function, then restores the original mapping.

// --- Screen trampolines (bank 8) ---

void Screen_Render(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_Render();
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_DrawUI(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_DrawUI();
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_SwapCopyDirectionIndicator(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_SwapCopyDirectionIndicator();
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_SetInitialMenuStates(byte n) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_SetInitialMenuStates(n);
	POKE(OVERLAY_MMU_REG, s);
}
byte Screen_GetValidUserInput(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	byte r = FAR_Screen_GetValidUserInput();
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
void Screen_UpdateMenuStates(UI_Menu_Enabler_Info* i) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_UpdateMenuStates(i);
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_RenderMenu(bool sparse) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_RenderMenu(sparse);
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_UpdateSortIcons(byte px, void* fn) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_UpdateSortIcons(px, fn);
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_UpdateMeatloafIcon(byte px, bool m) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_UpdateMeatloafIcon(px, m);
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_ShowAppAboutInfo(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_ShowAppAboutInfo();
	POKE(OVERLAY_MMU_REG, s);
}
void Screen_DrawPanelHeader(byte pid, bool fd) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Screen_DrawPanelHeader(pid, fd);
	POKE(OVERLAY_MMU_REG, s);
}
char* Screen_GetStringFromUser(char* t, char* b, char* st, byte ml) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	char* r = FAR_Screen_GetStringFromUser(t, b, st, ml);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Screen_ShowUserTwoButtonDialog(char* t, byte b, byte p, byte n) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	bool r = FAR_Screen_ShowUserTwoButtonDialog(t, b, p, n);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
void Buffer_RefreshDisplay(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Buffer_RefreshDisplay();
	POKE(OVERLAY_MMU_REG, s);
}
void Buffer_NewMessage(const char *msg) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Buffer_NewMessage(msg);
	POKE(OVERLAY_MMU_REG, s);
}
void Buffer_Initialize(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Buffer_Initialize();
	POKE(OVERLAY_MMU_REG, s);
}
void Buffer_Clear(void) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_SCREEN_BLOCK);
	FAR_Buffer_Clear();
	POKE(OVERLAY_MMU_REG, s);
}

// --- Panel trampolines (bank 9) ---

void Panel_Initialize(byte pid, WB2KViewPanel* p, bool fd, byte x, byte y, byte w, byte h) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_Initialize(pid, p, fd, x, y, w, h);
	POKE(OVERLAY_MMU_REG, s);
}
bool Panel_SwitchDevice(WB2KViewPanel* p, device_number d) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_SwitchDevice(p, d);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
void Panel_SetCurrentDevice(WB2KViewPanel* p, device_number d) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_SetCurrentDevice(p, d);
	POKE(OVERLAY_MMU_REG, s);
}
void Panel_ToggleActiveState(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_ToggleActiveState(p);
	POKE(OVERLAY_MMU_REG, s);
}
bool Panel_MakeDir(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_MakeDir(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_FormatDrive(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_FormatDrive(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_Refresh(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_Refresh(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_RenameCurrentFile(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_RenameCurrentFile(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_DeleteCurrentFile(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_DeleteCurrentFile(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_OpenCurrentFileOrFolder(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_OpenCurrentFileOrFolder(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_CopyCurrentFile(WB2KViewPanel* p, WB2KViewPanel* o) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_CopyCurrentFile(p, o);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_ViewCurrentFile(WB2KViewPanel* p, byte v) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_ViewCurrentFile(p, v);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_SelectPrevFile(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_SelectPrevFile(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_SelectNextFile(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_SelectNextFile(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_SetFileSelectionByRow(WB2KViewPanel* p, uint16_t r2, bool ds) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_SetFileSelectionByRow(p, r2, ds);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
void Panel_ReflowContent(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_ReflowContent(p);
	POKE(OVERLAY_MMU_REG, s);
}
void Panel_ClearDisplay(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_ClearDisplay(p);
	POKE(OVERLAY_MMU_REG, s);
}
void Panel_RenderContents(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_RenderContents(p);
	POKE(OVERLAY_MMU_REG, s);
}
void Panel_SortAndDisplay(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	FAR_Panel_SortAndDisplay(p);
	POKE(OVERLAY_MMU_REG, s);
}
bool Panel_FillCurrentBank(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_FillCurrentBank(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_ClearCurrentBank(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_ClearCurrentBank(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_SearchCurrentBank(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_SearchCurrentBank(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
bool Panel_OpenMeatloafURL(WB2KViewPanel* p) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_PANEL_BLOCK);
	bool r = FAR_Panel_OpenMeatloafURL(p);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}

// --- EM trampolines (bank 10) ---

void EM_DisplayAsText(byte bank, byte pages, char* name) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_EM_BLOCK);
	FAR_EM_DisplayAsText(bank, pages, name);
	POKE(OVERLAY_MMU_REG, s);
}
void EM_DisplayAsHex(byte bank, byte pages, char* name) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_EM_BLOCK);
	FAR_EM_DisplayAsHex(bank, pages, name);
	POKE(OVERLAY_MMU_REG, s);
}
bool EM_SearchMemory(byte sb, byte sp, byte sbt, bool ns) {
	volatile byte s = PEEK(OVERLAY_MMU_REG); POKE(OVERLAY_MMU_REG, OVL_EM_BLOCK);
	bool r = FAR_EM_SearchMemory(sb, sp, sbt, ns);
	POKE(OVERLAY_MMU_REG, s);
	return r;
}
