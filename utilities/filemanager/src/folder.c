/*
 * folder.c - Folder object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "folder.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                           File-scoped Variables                           */
/*****************************************************************************/

static char folder_temp_filename_buffer[FILE_MAX_FILENAME_SIZE];


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static bool Folder_CopyFileBytes(WB2KFolderObject* the_folder, WB2KFileObject* the_file, const char* target_path);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

static bool Folder_CopyFileBytes(WB2KFolderObject* the_folder, WB2KFileObject* the_file, const char* target_path)
{
	FILE*    src_fh;
	FILE*    dst_fh;
	byte     buf[STORAGE_FILE_BUFFER_1_LEN];
	int16_t  bytes_read;
	uint32_t total_read = 0;
	byte     progress;
	char*    src_name;

	(void)the_folder;

	// Build source path
	src_name = App_GetFilename(the_file->panel_id_, the_file->id_);
	sprintf(global_temp_path_1, "%u:%s", the_folder->device_number_, src_name);

	src_fh = fopen(global_temp_path_1, "r");
	if (src_fh == NULL) return false;

	dst_fh = fopen(target_path, "w");
	if (dst_fh == NULL)
	{
		fclose(src_fh);
		return false;
	}

	App_ShowProgressBar();

	while (1)
	{
		bytes_read = fread(buf, 1, STORAGE_FILE_BUFFER_1_LEN, src_fh);
		if (bytes_read <= 0) break;

		fwrite(buf, 1, bytes_read, dst_fh);
		total_read += bytes_read;

		if (the_file->size_ > 0)
		{
			progress = (byte)((total_read * 100) / the_file->size_);
			App_UpdateProgressBar(progress);
		}
	}

	fclose(src_fh);
	fclose(dst_fh);
	App_HideProgressBar();

	return true;
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

WB2KFolderObject* Folder_NewOrReset(WB2KFolderObject* the_existing_folder, byte the_device_number, char* new_path)
{
	WB2KFolderObject* the_folder;

	if (the_existing_folder != NULL)
	{
		the_folder = the_existing_folder;
		Folder_DestroyAllFiles(the_folder);
	}
	else
	{
		the_folder = (WB2KFolderObject*)malloc(sizeof(WB2KFolderObject));
		if (the_folder == NULL)
		{
			Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
			return NULL;
		}
		memset(the_folder, 0, sizeof(WB2KFolderObject));

		the_folder->list_ = (WB2KList**)malloc(sizeof(WB2KList*));
		if (the_folder->list_ == NULL)
		{
			free(the_folder);
			return NULL;
		}
		*(the_folder->list_) = NULL;
	}

	// Set or update folder path
	if (the_folder->file_path_ != NULL)
		free(the_folder->file_path_);

	{
		int16_t len = generalStrnlen(new_path, FILE_MAX_PATHNAME_SIZE);
		the_folder->file_path_ = (char*)malloc(len + 1);
		if (the_folder->file_path_ != NULL)
			generalStrlcpy(the_folder->file_path_, new_path, len + 1);
	}

	// Set or update folder name
	if (the_folder->file_name_ != NULL)
		free(the_folder->file_name_);

	{
		int16_t len = generalStrnlen(new_path, FILE_MAX_FILENAME_SIZE);
		the_folder->file_name_ = (char*)malloc(len + 1);
		if (the_folder->file_name_ != NULL)
			generalStrlcpy(the_folder->file_name_, new_path, len + 1);
	}

	the_folder->device_number_ = the_device_number;
	the_folder->file_count_ = 0;
	the_folder->cur_row_ = -1;
	the_folder->is_meatloaf_ = false;

	return the_folder;
}


void Folder_Destroy(WB2KFolderObject** the_folder)
{
	if (*the_folder == NULL) return;

	Folder_DestroyAllFiles(*the_folder);

	if ((*the_folder)->file_name_ != NULL)
	{
		free((*the_folder)->file_name_);
		(*the_folder)->file_name_ = NULL;
	}

	if ((*the_folder)->file_path_ != NULL)
	{
		free((*the_folder)->file_path_);
		(*the_folder)->file_path_ = NULL;
	}

	if ((*the_folder)->list_ != NULL)
	{
		free((*the_folder)->list_);
		(*the_folder)->list_ = NULL;
	}

	free(*the_folder);
	*the_folder = NULL;
}


void Folder_DestroyAllFiles(WB2KFolderObject* the_folder)
{
	WB2KList* the_item;

	if (the_folder == NULL) return;
	if (the_folder->list_ == NULL) return;

	while (*(the_folder->list_) != NULL)
	{
		the_item = *(the_folder->list_);
		*(the_folder->list_) = the_item->next_item_;

		if (the_item->payload_ != NULL)
		{
			WB2KFileObject* the_file = (WB2KFileObject*)the_item->payload_;
			File_Destroy(&the_file);
		}
		free(the_item);
	}

	the_folder->file_count_ = 0;
	the_folder->cur_row_ = -1;
}


void Folder_SetCurrentRow(WB2KFolderObject* the_folder, int16_t the_row_number)
{
	if (the_folder == NULL) return;
	the_folder->cur_row_ = the_row_number;
}


bool Folder_HasChildren(WB2KFolderObject* the_folder)
{
	if (the_folder == NULL) return false;
	return (the_folder->file_count_ > 0);
}


uint16_t Folder_GetCountFiles(WB2KFolderObject* the_folder)
{
	if (the_folder == NULL) return 0;
	return the_folder->file_count_;
}


int16_t Folder_GetCurrentRow(WB2KFolderObject* the_folder)
{
	if (the_folder == NULL) return -1;
	return the_folder->cur_row_;
}


WB2KFileObject* Folder_GetCurrentFile(WB2KFolderObject* the_folder)
{
	if (the_folder == NULL) return NULL;
	if (the_folder->cur_row_ < 0) return NULL;
	return Folder_FindFileByRow(the_folder, (byte)the_folder->cur_row_);
}


byte Folder_GetCurrentFileType(WB2KFolderObject* the_folder)
{
	WB2KFileObject* the_file = Folder_GetCurrentFile(the_folder);
	if (the_file == NULL) return 0;
	return the_file->file_type_;
}


WB2KFileObject* Folder_FindFileByRow(WB2KFolderObject* the_folder, byte the_row)
{
	WB2KList* the_item;

	if (the_folder == NULL) return NULL;
	if (the_folder->list_ == NULL) return NULL;

	the_item = *(the_folder->list_);

	while (the_item != NULL)
	{
		WB2KFileObject* the_file = (WB2KFileObject*)the_item->payload_;
		if (the_file != NULL && the_file->row_ == the_row)
			return the_file;
		the_item = the_item->next_item_;
	}

	return NULL;
}


bool Folder_AddNewFile(WB2KFolderObject* the_folder, WB2KFileObject* the_file)
{
	WB2KList* new_item;

	if (the_folder == NULL || the_file == NULL) return false;

	new_item = List_NewItem(the_file);
	if (new_item == NULL) return false;

	List_AddItem(the_folder->list_, new_item);
	the_folder->file_count_++;

	return true;
}


bool Folder_AddNewFileAsCopy(WB2KFolderObject* the_folder, WB2KFileObject* the_file)
{
	WB2KFileObject* the_copy;

	if (the_folder == NULL || the_file == NULL) return false;

	the_copy = File_Duplicate(the_file);
	if (the_copy == NULL) return false;

	return Folder_AddNewFile(the_folder, the_copy);
}


bool Folder_CopyFile(WB2KFolderObject* the_folder, WB2KFileObject* the_file, WB2KFolderObject* the_target_folder)
{
	char*  src_name;

	if (the_folder == NULL || the_file == NULL || the_target_folder == NULL) return false;

	Buffer_NewMessage(GetString(ID_STR_MSG_COPYING));

	// Build target path
	src_name = App_GetFilename(the_file->panel_id_, the_file->id_);
	sprintf(global_temp_path_2, "%u:%s", the_target_folder->device_number_, src_name);

	// Do the copy
	if (!Folder_CopyFileBytes(the_folder, the_file, global_temp_path_2))
		return false;

	Buffer_NewMessage(GetString(ID_STR_MSG_DONE));
	return true;
}


bool Folder_CopyCurrentFile(WB2KFolderObject* the_folder, WB2KFolderObject* the_target_folder)
{
	WB2KFileObject* the_file;

	if (the_folder == NULL) return false;

	the_file = Folder_GetCurrentFile(the_folder);
	if (the_file == NULL) return false;

	return Folder_CopyFile(the_folder, the_file, the_target_folder);
}


byte Folder_PopulateFiles(byte the_panel_id, WB2KFolderObject* the_folder)
{
	// Use kernel directory API to read files
	// f256lib's fileDirEntT has: d_blocks, d_type, d_name[256]
	// d_type: 0=file, 1=dir, 2=label (use _DE_ISREG/_DE_ISDIR/_DE_ISLBL macros)

	fileDirEntT*  dir_entry_ptr;
	char*         dir_handle;
	byte          row = 0;
	DateTime      dt;
	bool          is_dir;
	byte          file_type;
	uint32_t      file_size;
	uint16_t      max_file_cnt;

	if (the_folder == NULL) return 0;

	Buffer_NewMessage(GetString(ID_STR_MSG_READING_DIR));

	max_file_cnt = 200;  // reasonable upper bound for 8-bit system

	// Open directory - returns handle or NULL on failure
	dir_handle = fileOpenDir(the_folder->file_path_);
	if (dir_handle == NULL)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_FAIL_OPEN_DIR));
		return 0;
	}

	// Read directory entries
	while (row < max_file_cnt)
	{
		dir_entry_ptr = fileReadDir(dir_handle);
		if (dir_entry_ptr == NULL) break;

		// Skip deleted entries and disk labels
		if (dir_entry_ptr->d_name[0] == 0) break;
		if (_DE_ISLBL(dir_entry_ptr->d_type)) continue;

		// Determine if directory
		is_dir = _DE_ISDIR(dir_entry_ptr->d_type);

		// File type
		if (is_dir)
			file_type = _CBM_T_DIR;
		else
			file_type = _CBM_T_REG;

		// File size from blocks (each block is 256 bytes)
		file_size = (uint32_t)dir_entry_ptr->d_blocks * FILE_BYTES_PER_BLOCK;

		// No date/time info available from kernel directory API
		memset(&dt, 0, sizeof(DateTime));

		// Create file object
		{
			WB2KFileObject* new_file = File_New(
				the_panel_id,
				dir_entry_ptr->d_name,
				is_dir,
				file_size,
				file_type,
				row,
				&dt
			);

			if (new_file == NULL) break;

			Folder_AddNewFile(the_folder, new_file);
			row++;
		}
	}

	fileCloseDir(dir_handle);

	// Set first file as current if any files exist
	if (row > 0)
		the_folder->cur_row_ = 0;

	sprintf(global_string_buff1, GetString(ID_STR_N_FILES_FOUND), row);
	Buffer_NewMessage(global_string_buff1);

	Buffer_NewMessage(GetString(ID_STR_MSG_DONE));
	return row;
}


WB2KFileObject* Folder_SetFileSelectionByRow(WB2KFolderObject* the_folder, uint16_t the_row, bool do_selection, byte y_offset)
{
	WB2KFileObject*  the_file;
	WB2KFileObject*  prev_file;

	if (the_folder == NULL) return NULL;

	the_file = Folder_FindFileByRow(the_folder, (byte)the_row);
	if (the_file == NULL) return NULL;

	// Deselect previously selected file if different
	if (the_folder->cur_row_ >= 0 && the_folder->cur_row_ != (int16_t)the_row)
	{
		prev_file = Folder_FindFileByRow(the_folder, (byte)the_folder->cur_row_);
		if (prev_file != NULL)
			File_MarkUnSelected(prev_file, y_offset);
	}

	if (do_selection)
	{
		File_MarkSelected(the_file, y_offset);
		the_folder->cur_row_ = (int16_t)the_row;
	}
	else
	{
		File_MarkUnSelected(the_file, y_offset);
	}

	return the_file;
}
