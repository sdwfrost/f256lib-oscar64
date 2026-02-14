/*
 * folder.h - Folder object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef FOLDER_H_
#define FOLDER_H_

#include "app.h"
#include "file.h"
#include "list.h"
#include <stdio.h>

#pragma compile("folder.c")


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define FOLDER_SYSTEM_ROOT_NAME		"[ROOT]"

#define MARK_AS_SELECTED			true
#define MARK_AS_UNSELECTED			false

#define FOLDER_MAX_TRIES_AT_FOLDER_CREATION		128


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

typedef enum folder_error_code
{
	FOLDER_ERROR_OUT_OF_MEMORY = -10,
	FOLDER_ERROR_NO_FILE_EXTENSION = -2,
	FOLDER_ERROR_NON_LETHAL_ERROR = -1,
	FOLDER_ERROR_NO_ERROR = 0,
	FOLDER_ERROR_SUCCESS = 1,
} folder_error_code;


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct WB2KFolderObject
{
	WB2KList**		list_;
	char*			file_name_;
	char*			file_path_;
	uint16_t		file_count_;
	int16_t			cur_row_;
	bool			is_meatloaf_;
	byte			device_number_;
} WB2KFolderObject;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Constructor / reset
WB2KFolderObject* Folder_NewOrReset(WB2KFolderObject* the_existing_folder, byte the_device_number, char* new_path);

// Destructor
void Folder_Destroy(WB2KFolderObject** the_folder);

// Free all files in the folder list
void Folder_DestroyAllFiles(WB2KFolderObject* the_folder);

// Setters
void Folder_SetCurrentRow(WB2KFolderObject* the_folder, int16_t the_row_number);

// Getters
bool Folder_HasChildren(WB2KFolderObject* the_folder);
uint16_t Folder_GetCountFiles(WB2KFolderObject* the_folder);
int16_t Folder_GetCurrentRow(WB2KFolderObject* the_folder);
WB2KFileObject* Folder_GetCurrentFile(WB2KFolderObject* the_folder);
byte Folder_GetCurrentFileType(WB2KFolderObject* the_folder);
WB2KFileObject* Folder_FindFileByRow(WB2KFolderObject* the_folder, byte the_row);

// File list operations
bool Folder_AddNewFile(WB2KFolderObject* the_folder, WB2KFileObject* the_file);
bool Folder_AddNewFileAsCopy(WB2KFolderObject* the_folder, WB2KFileObject* the_file);

// File operations
bool Folder_CopyFile(WB2KFolderObject* the_folder, WB2KFileObject* the_file, WB2KFolderObject* the_target_folder);
bool Folder_CopyCurrentFile(WB2KFolderObject* the_folder, WB2KFolderObject* the_target_folder);

// Directory population
byte Folder_PopulateFiles(byte the_panel_id, WB2KFolderObject* the_folder);

// Selection
WB2KFileObject* Folder_SetFileSelectionByRow(WB2KFolderObject* the_folder, uint16_t the_row, bool do_selection, byte y_offset);


#endif /* FOLDER_H_ */
