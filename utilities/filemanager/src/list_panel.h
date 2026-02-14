/*
 * list_panel.h - View panel controller for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef LIST_PANEL_H_
#define LIST_PANEL_H_

#include "folder.h"
#include "list.h"
#include "app.h"
#include "memsys.h"

// list_panel.c compiled via overlay in filemanager.c


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PANEL_LIST_MAX_ROWS			255

#define PARAM_VIEW_AS_HEX			0
#define PARAM_VIEW_AS_TEXT			1

#define PARAM_INITIALIZE_FOR_DISK	true
#define PARAM_INITIALIZE_FOR_MEMORY	false

#define PARAM_MARK_SELECTED			true
#define PARAM_MARK_UNSELECTED		false


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct WB2KViewPanel
{
	WB2KFolderObject*	root_folder_;
	FMMemorySystem*		memory_system_;
	byte				id_;
	byte				x_;
	byte				y_;
	byte				width_;
	byte				height_;
	byte				num_rows_;
	byte				content_top_;
	device_number		device_number_;
	bool				active_;
	bool				for_disk_;
	void*				sort_compare_function_;
} WB2KViewPanel;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Constructor / initializer
void Panel_Initialize(byte the_panel_id, WB2KViewPanel* the_panel, bool for_disk, byte x, byte y, byte width, byte height);

// Switch to a different device
bool Panel_SwitchDevice(WB2KViewPanel* the_panel, device_number the_device);

// Setters
void Panel_SetCurrentDevice(WB2KViewPanel* the_panel, device_number the_device_num);
void Panel_ToggleActiveState(WB2KViewPanel* the_panel);

// Operations
bool Panel_MakeDir(WB2KViewPanel* the_panel);
bool Panel_FormatDrive(WB2KViewPanel* the_panel);
bool Panel_Refresh(WB2KViewPanel* the_panel);
bool Panel_RenameCurrentFile(WB2KViewPanel* the_panel);
bool Panel_DeleteCurrentFile(WB2KViewPanel* the_panel);
bool Panel_OpenCurrentFileOrFolder(WB2KViewPanel* the_panel);
bool Panel_CopyCurrentFile(WB2KViewPanel* the_panel, WB2KViewPanel* the_other_panel);
bool Panel_ViewCurrentFile(WB2KViewPanel* the_panel, byte the_viewer_type);

// Navigation
bool Panel_SelectPrevFile(WB2KViewPanel* the_panel);
bool Panel_SelectNextFile(WB2KViewPanel* the_panel);
bool Panel_SetFileSelectionByRow(WB2KViewPanel* the_panel, uint16_t the_row, bool do_selection);

// Display
void Panel_ReflowContent(WB2KViewPanel* the_panel);
void Panel_ClearDisplay(WB2KViewPanel* the_panel);
void Panel_RenderContents(WB2KViewPanel* the_panel);
void Panel_SortAndDisplay(WB2KViewPanel* the_panel);

// Memory operations
bool Panel_FillCurrentBank(WB2KViewPanel* the_panel);
bool Panel_ClearCurrentBank(WB2KViewPanel* the_panel);
bool Panel_SearchCurrentBank(WB2KViewPanel* the_panel);

// Meatloaf
bool Panel_OpenMeatloafURL(WB2KViewPanel* the_panel);


#endif /* LIST_PANEL_H_ */
