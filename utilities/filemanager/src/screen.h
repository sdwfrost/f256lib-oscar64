/*
 * screen.h - UI layout, menus, rendering for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "app.h"

// screen.c compiled via overlay in filemanager.c


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PARAM_ONLY_RENDER_CHANGED_ITEMS		true
#define PARAM_RENDER_ALL_MENU_ITEMS			false

// Buttons
#define NUM_BUTTONS					27

#define BUTTON_ID_DEV_SD_CARD		0
#define BUTTON_ID_DEV_FLOPPY_1		1
#define BUTTON_ID_DEV_FLOPPY_2		2
#define BUTTON_ID_DEV_RAM			3
#define BUTTON_ID_DEV_FLASH			4
#define BUTTON_ID_REFRESH			5
#define BUTTON_ID_FORMAT			6
#define BUTTON_ID_MAKE_DIR			7
#define BUTTON_ID_SORT_BY_TYPE		8
#define BUTTON_ID_SORT_BY_NAME		9
#define BUTTON_ID_SORT_BY_SIZE		10
#define BUTTON_ID_COPY				11
#define BUTTON_ID_DELETE			12
#define BUTTON_ID_DUPLICATE			13
#define BUTTON_ID_RENAME			14
#define BUTTON_ID_TEXT_VIEW			15
#define BUTTON_ID_HEX_VIEW			16
#define BUTTON_ID_LOAD				17
#define BUTTON_ID_BANK_FILL			18
#define BUTTON_ID_BANK_CLEAR		19
#define BUTTON_ID_BANK_FIND			20
#define BUTTON_ID_BANK_FIND_NEXT	21
#define BUTTON_ID_SET_CLOCK			22
#define BUTTON_ID_ABOUT				23
#define BUTTON_ID_EXIT_TO_BASIC		24
#define BUTTON_ID_EXIT_TO_DOS		25
#define BUTTON_ID_QUIT				26

// UI layout - middle area (between panels)
#define UI_MIDDLE_AREA_START_X		35
#define UI_MIDDLE_AREA_START_Y		4
#define UI_MIDDLE_AREA_WIDTH		10

#define UI_MIDDLE_AREA_DEV_MENU_Y	(UI_MIDDLE_AREA_START_Y + 2)
#define UI_MIDDLE_AREA_DEV_CMD_Y	(UI_MIDDLE_AREA_DEV_MENU_Y + 3)

#define UI_MIDDLE_AREA_DIR_MENU_Y	(UI_MIDDLE_AREA_DEV_CMD_Y + 8)
#define UI_MIDDLE_AREA_DIR_CMD_Y	(UI_MIDDLE_AREA_DIR_MENU_Y + 3)

#define UI_MIDDLE_AREA_FILE_MENU_Y	(UI_MIDDLE_AREA_DIR_CMD_Y + 5)
#define UI_MIDDLE_AREA_FILE_CMD_Y	(UI_MIDDLE_AREA_FILE_MENU_Y + 3)

#define UI_MIDDLE_AREA_APP_MENU_Y	(UI_MIDDLE_AREA_FILE_CMD_Y + 12)
#define UI_MIDDLE_AREA_APP_CMD_Y	(UI_MIDDLE_AREA_APP_MENU_Y + 3)

// Panel dimensions
#define UI_PANEL_INNER_WIDTH		33
#define UI_PANEL_OUTER_WIDTH		(UI_PANEL_INNER_WIDTH + 2)
#define UI_PANEL_INNER_HEIGHT		42
#define UI_PANEL_OUTER_HEIGHT		(UI_PANEL_INNER_HEIGHT + 2)
#define UI_PANEL_TAB_WIDTH			28
#define UI_PANEL_TAB_HEIGHT			3

// Column offsets within panel
#define UI_PANEL_FILENAME_OFFSET	1
#define UI_PANEL_FILETYPE_OFFSET	23
#define UI_PANEL_FILESIZE_OFFSET	5
#define UI_PANEL_BANK_NUM_OFFSET	21
#define UI_PANEL_BANK_ADDR_OFFSET	5

// Panel positioning
#define UI_VIEW_PANEL_TITLE_TAB_Y1	3
#define UI_VIEW_PANEL_HEADER_Y		(UI_VIEW_PANEL_TITLE_TAB_Y1 + UI_PANEL_TAB_HEIGHT + 1)
#define UI_VIEW_PANEL_BODY_Y1		6
#define UI_VIEW_PANEL_BODY_HEIGHT	UI_PANEL_OUTER_HEIGHT
#define UI_VIEW_PANEL_BODY_Y2		(UI_VIEW_PANEL_BODY_Y1 + UI_VIEW_PANEL_BODY_HEIGHT - 1)

#define UI_VIEW_PANEL_BODY_WIDTH	UI_PANEL_OUTER_WIDTH
#define UI_LEFT_PANEL_BODY_X1		0
#define UI_LEFT_PANEL_BODY_X2		(UI_LEFT_PANEL_BODY_X1 + UI_VIEW_PANEL_BODY_WIDTH - 1)

#define UI_RIGHT_PANEL_X_DELTA		45
#define UI_RIGHT_PANEL_BODY_X1		(UI_LEFT_PANEL_BODY_X1 + UI_RIGHT_PANEL_X_DELTA)
#define UI_RIGHT_PANEL_BODY_X2		(UI_RIGHT_PANEL_BODY_X1 + UI_VIEW_PANEL_BODY_WIDTH - 1)

#define UI_FULL_PATH_LINE_Y			(UI_VIEW_PANEL_BODY_Y2 + 1)

#define UI_VIEW_PANEL_SCROLL_CNT	(UI_PANEL_INNER_HEIGHT - 2)

// Communication buffer area
#define COMM_BUFFER_NUM_COLS		78
#define COMM_BUFFER_NUM_ROWS		4
#define COMM_BUFFER_FIRST_COL		1
#define COMM_BUFFER_FIRST_ROW		(UI_VIEW_PANEL_BODY_Y2 + 3)
#define COMM_BUFFER_LAST_COL		(COMM_BUFFER_NUM_COLS)
#define COMM_BUFFER_LAST_ROW		(COMM_BUFFER_FIRST_ROW + COMM_BUFFER_NUM_ROWS - 1)

// Progress bar
#define UI_COPY_PROGRESS_Y			UI_MIDDLE_AREA_FILE_CMD_Y
#define CH_PROGRESS_BAR_CHECKER_CH1	207
#define CH_CHECKERBOARD				199
#define CH_UNDERSCORE				148
#define CH_OVERSCORE				0x0e
#define CH_SORT_ICON				248


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct UI_Menu_Enabler_Info
{
	byte		file_type_;
	bool		for_disk_;
	bool		for_flash_;
	bool		is_kup_;
	bool		is_meatloaf_;
	bool		other_panel_is_meatloaf_;
	bool		other_panel_for_disk_;
	bool		other_panel_for_flash_;
} UI_Menu_Enabler_Info;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Screen rendering
void Screen_Render(void);
void Screen_DrawUI(void);

// Menu management
void Screen_SwapCopyDirectionIndicator(void);
void Screen_SetInitialMenuStates(byte num_disk_systems);
byte Screen_GetValidUserInput(void);
void Screen_UpdateMenuStates(UI_Menu_Enabler_Info* the_enabling_info);
void Screen_RenderMenu(bool sparse_render);

// Sort and meatloaf icons
void Screen_UpdateSortIcons(byte the_panel_x, void* the_sort_compare_function);
void Screen_UpdateMeatloafIcon(byte the_panel_x, bool meatloaf_mode);

// About info
void Screen_ShowAppAboutInfo(void);

// Panel headers
void Screen_DrawPanelHeader(byte panel_id, bool for_disk);

// User dialogs
char* Screen_GetStringFromUser(char* dialog_title, char* dialog_body, char* starter_string, byte max_len);
bool Screen_ShowUserTwoButtonDialog(char* dialog_title, byte dialog_body_string_id, byte positive_btn_label_string_id, byte negative_btn_label_string_id);

// Communication buffer
void Buffer_RefreshDisplay(void);


#endif /* SCREEN_H_ */
