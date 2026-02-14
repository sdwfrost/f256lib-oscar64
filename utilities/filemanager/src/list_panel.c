/*
 * list_panel.c - View panel controller for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "list_panel.h"
#include "bank.h"
#include "file.h"
#include "folder.h"
#include "list.h"
#include "memsys.h"
#include "overlay_em.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static void Panel_ReflowContentForDisk(WB2KViewPanel* the_panel);
static void Panel_ReflowContentForMemory(WB2KViewPanel* the_panel);
static void Panel_RenderTitleOnly(WB2KViewPanel* the_panel);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

static void Panel_ReflowContentForDisk(WB2KViewPanel* the_panel)
{
	WB2KList*	the_item;
	uint16_t	num_rows;
	uint16_t	row;
	byte		num_files;
	int8_t		display_row;
	byte		first_viz_row = the_panel->content_top_;
	byte		last_viz_row = first_viz_row + the_panel->height_ - 1;

	num_files = Folder_GetCountFiles(the_panel->root_folder_);
	num_rows = num_files;

	if (num_files == 0)
	{
		the_panel->num_rows_ = 0;
		return;
	}

	the_item = *(the_panel->root_folder_->list_);

	if (the_item == NULL)
	{
		App_Exit(ERROR_NO_FILES_IN_FILE_LIST);
	}

	for (row = 0; row < num_rows && the_item; row++)
	{
		WB2KFileObject* this_file;

		if (row >= first_viz_row && row <= last_viz_row)
			display_row = row - first_viz_row;
		else
			display_row = -1;

		this_file = (WB2KFileObject*)(the_item->payload_);
		File_UpdatePos(this_file, the_panel->x_, display_row, row);

		the_item = the_item->next_item_;
	}

	the_panel->num_rows_ = num_rows;
}


static void Panel_ReflowContentForMemory(WB2KViewPanel* the_panel)
{
	uint16_t	row;
	int8_t		display_row;
	byte		first_viz_row = the_panel->content_top_;
	byte		last_viz_row = first_viz_row + the_panel->height_ - 1;

	for (row = 0; row < MEMORY_BANK_COUNT; row++)
	{
		FMBankObject* this_bank;

		if (row >= first_viz_row && row <= last_viz_row)
			display_row = row - first_viz_row;
		else
			display_row = -1;

		this_bank = &the_panel->memory_system_->bank_[row];
		Bank_UpdatePos(this_bank, the_panel->x_, display_row, row);
	}

	the_panel->num_rows_ = MEMORY_BANK_COUNT;
}


static void Panel_RenderTitleOnly(WB2KViewPanel* the_panel)
{
	byte	back_color;
	byte	fore_color;

	if (the_panel->active_ == true)
	{
		fore_color = PANEL_BACKGROUND_COLOR;
		back_color = LIST_ACTIVE_COLOR;
	}
	else
	{
		fore_color = LIST_INACTIVE_COLOR;
		back_color = PANEL_BACKGROUND_COLOR;
	}

	textFillBox(the_panel->x_, the_panel->y_ - 3, the_panel->x_ + (UI_PANEL_TAB_WIDTH - 3), the_panel->y_ - 3, ' ', fore_color, back_color);

	if (the_panel->for_disk_ == true)
	{
		textDrawStringAt(the_panel->x_, the_panel->y_ - 3, the_panel->root_folder_->file_name_, fore_color, back_color);
	}
	else if (the_panel->device_number_ == DEVICE_RAM)
	{
		textDrawStringAt(the_panel->x_, the_panel->y_ - 3, "RAM", fore_color, back_color);
	}
	else
	{
		textDrawStringAt(the_panel->x_, the_panel->y_ - 3, "Flash", fore_color, back_color);
	}
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


void Panel_Initialize(byte the_panel_id, WB2KViewPanel* the_panel, bool for_disk, byte x, byte y, byte width, byte height)
{
	if (for_disk)
	{
		the_panel->memory_system_ = NULL;
		the_panel->sort_compare_function_ = (void*)&File_CompareName;
	}
	else
	{
		the_panel->root_folder_ = NULL;

		if (the_panel->memory_system_->is_flash_)
			the_panel->device_number_ = DEVICE_FLASH;
		else
			the_panel->device_number_ = DEVICE_RAM;
	}

	the_panel->for_disk_ = for_disk;
	the_panel->id_ = the_panel_id;
	the_panel->x_ = x;
	the_panel->y_ = y;
	the_panel->width_ = width;
	the_panel->height_ = height;
	the_panel->content_top_ = 0;
	the_panel->num_rows_ = 0;
}


bool Panel_SwitchDevice(WB2KViewPanel* the_panel, device_number the_device)
{
	char	path_buff[3];
	bool	for_flash;
	bool	was_disk;

	the_panel->device_number_ = the_device;
	was_disk = the_panel->for_disk_;

	if (the_device < DEVICE_MAX_DISK_DEVICE)
		the_panel->for_disk_ = true;
	else
		the_panel->for_disk_ = false;

	if (was_disk == true && the_panel->for_disk_ == false)
	{
		// Switched from disk to memsys: free folder memory
		Folder_Destroy(&the_panel->root_folder_);
	}
	else if (was_disk == false && the_panel->for_disk_ == true)
	{
		// Switched from memsys to disk: free memsys memory
		MemSys_Destroy(&the_panel->memory_system_);
	}

	if (the_panel->for_disk_ == true)
	{
		the_panel->sort_compare_function_ = (void*)&File_CompareName;
		sprintf(path_buff, "%d:", the_device);

		if ((the_panel->root_folder_ = Folder_NewOrReset(the_panel->root_folder_, the_device, path_buff)) == NULL)
		{
			App_Exit(ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ);
		}
	}
	else
	{
		for_flash = (the_device != DEVICE_RAM);

		if ((the_panel->memory_system_ = MemSys_NewOrReset(the_panel->memory_system_, for_flash)) == NULL)
		{
			Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
			App_Exit(ERROR_COULD_NOT_CREATE_OR_RESET_MEMSYS_OBJ);
		}
	}

	Panel_Refresh(the_panel);
	return true;
}


void Panel_SetCurrentDevice(WB2KViewPanel* the_panel, device_number the_device_num)
{
	the_panel->device_number_ = the_device_num;
}


void Panel_ToggleActiveState(WB2KViewPanel* the_panel)
{
	the_panel->active_ = !the_panel->active_;
	Panel_RenderTitleOnly(the_panel);
	Panel_RenderContents(the_panel);
}


bool Panel_MakeDir(WB2KViewPanel* the_panel)
{
	bool	success;
	byte	current_path_len;
	byte	available_len;
	char*	new_folder_name;

	current_path_len = generalStrnlen(the_panel->root_folder_->file_path_, FILE_MAX_PATHNAME_SIZE);
	available_len = FILE_MAX_PATHNAME_SIZE - current_path_len - 1;

	if (available_len > APP_DIALOG_WIDTH - 2)
		available_len = APP_DIALOG_WIDTH - 2;

	global_string_buff1[0] = 0;

	new_folder_name = Screen_GetStringFromUser(
		(char*)GetString(ID_STR_DLG_NEW_FOLDER_TITLE),
		(char*)GetString(ID_STR_DLG_ENTER_NEW_FOLDER_NAME),
		global_string_buff1,
		available_len
	);

	if (new_folder_name == NULL)
		return false;

	// Build full path: folder_path / new_folder_name
	generalStrlcpy(global_temp_path_1, the_panel->root_folder_->file_path_, FILE_MAX_PATHNAME_SIZE);
	generalStrlcat(global_temp_path_1, new_folder_name, FILE_MAX_PATHNAME_SIZE);

	// Use kernel to create directory
	// TODO: implement mkdir via kernel call when available
	// success = Kernal_MkDir(global_temp_path_1, the_panel->device_number_);
	success = false;

	if (success)
		Panel_Refresh(the_panel);

	return success;
}


bool Panel_FormatDrive(WB2KViewPanel* the_panel)
{
	char* new_name;

	if (Screen_ShowUserTwoButtonDialog(
		(char*)GetString(ID_STR_DLG_FORMAT_TITLE),
		ID_STR_DLG_ARE_YOU_SURE,
		ID_STR_DLG_YES,
		ID_STR_DLG_NO) != true)
	{
		return false;
	}

	generalStrlcpy(global_string_buff1, GetString(ID_STR_DLG_ENTER_NEW_NAME), 70);
	global_temp_path_2[0] = 0;

	new_name = Screen_GetStringFromUser(
		(char*)GetString(ID_STR_DLG_FORMAT_TITLE),
		global_string_buff1,
		global_temp_path_2,
		FILE_MAX_FILENAME_SIZE
	);

	if (new_name == NULL)
		return false;

	Buffer_NewMessage(GetString(ID_STR_MSG_FORMATTING));

	// TODO: implement format via kernel call when available
	// result_code = mkfs(new_name, the_panel->device_number_);

	Buffer_NewMessage(GetString(ID_STR_MSG_DONE));
	return true;
}


bool Panel_Refresh(WB2KViewPanel* the_panel)
{
	byte the_error_code;

	if (the_panel == NULL)
		App_Exit(ERROR_PANEL_WAS_NULL);

	the_panel->content_top_ = 0;

	if (the_panel->for_disk_ == true)
	{
		Buffer_NewMessage(GetString(ID_STR_MSG_READING_DIR));

		Folder_DestroyAllFiles(the_panel->root_folder_);

		if ((the_error_code = Folder_PopulateFiles(the_panel->id_, the_panel->root_folder_)) > ERROR_NO_ERROR)
		{
			return false;
		}
	}
	else
	{
		MemSys_ResetAllBanks(the_panel->memory_system_);
		MemSys_PopulateBanks(the_panel->memory_system_);
	}

	Panel_SortAndDisplay(the_panel);
	return true;
}


bool Panel_RenameCurrentFile(WB2KViewPanel* the_panel)
{
	WB2KFileObject*	the_file;
	bool			success;
	char*			new_file_name;
	char			old_name_copy[FILE_MAX_FILENAME_SIZE];

	the_file = Folder_GetCurrentFile(the_panel->root_folder_);
	if (the_file == NULL)
		return false;

	sprintf(global_string_buff1, "%s %s", GetString(ID_STR_DLG_RENAME_TITLE), App_GetFilename(the_file->panel_id_, the_file->id_));

	// Pre-fill edit buffer with current name
	generalStrlcpy(global_temp_path_2, App_GetFilename(the_file->panel_id_, the_file->id_), FILE_MAX_FILENAME_SIZE);

	new_file_name = Screen_GetStringFromUser(
		global_string_buff1,
		(char*)GetString(ID_STR_DLG_ENTER_NEW_NAME),
		global_temp_path_2,
		FILE_MAX_FILENAME_SIZE
	);

	if (new_file_name == NULL)
		return false;

	// Build old and new paths
	generalStrlcpy(old_name_copy, App_GetFilename(the_file->panel_id_, the_file->id_), FILE_MAX_FILENAME_SIZE);

	sprintf(global_temp_path_1, "%u:%s", the_panel->root_folder_->device_number_, old_name_copy);
	sprintf(global_temp_path_2, "%u:%s", the_panel->root_folder_->device_number_, new_file_name);

	success = File_Rename(the_file, new_file_name, global_temp_path_1, global_temp_path_2);

	if (success)
		Panel_RenderContents(the_panel);

	return success;
}


bool Panel_DeleteCurrentFile(WB2KViewPanel* the_panel)
{
	WB2KFileObject*	the_file;
	int16_t			the_current_row;
	bool			success;
	char			delete_file_name[FILE_MAX_FILENAME_SIZE];

	the_current_row = Folder_GetCurrentRow(the_panel->root_folder_);
	if (the_current_row < 0)
		return false;

	the_file = Folder_FindFileByRow(the_panel->root_folder_, the_current_row);
	generalStrlcpy(delete_file_name, App_GetFilename(the_file->panel_id_, the_file->id_), FILE_MAX_FILENAME_SIZE);

	sprintf(global_string_buff1, "%s %s", GetString(ID_STR_DLG_DELETE_TITLE), delete_file_name);

	if (Screen_ShowUserTwoButtonDialog(
		global_string_buff1,
		ID_STR_DLG_ARE_YOU_SURE,
		ID_STR_DLG_YES,
		ID_STR_DLG_NO) != true)
	{
		return false;
	}

	// Build full path
	sprintf(global_temp_path_1, "%u:%s", the_panel->root_folder_->device_number_, App_GetFilename(the_file->panel_id_, the_file->id_));

	success = File_Delete(global_temp_path_1, the_file->is_directory_);

	if (success == false)
	{
		if (the_file->is_directory_)
			Buffer_NewMessage(GetString(ID_STR_MSG_DELETE_DIR_FAILURE));
		else
			Buffer_NewMessage(GetString(ID_STR_MSG_DELETE_FILE_FAILURE));

		return false;
	}

	Panel_Refresh(the_panel);
	Panel_SetFileSelectionByRow(the_panel, the_current_row, true);

	sprintf(global_string_buff1, "%s %s", GetString(ID_STR_MSG_DELETE_SUCCESS), delete_file_name);
	Buffer_NewMessage(global_string_buff1);

	return success;
}


bool Panel_OpenCurrentFileOrFolder(WB2KViewPanel* the_panel)
{
	WB2KFileObject*	the_file;
	bool			success;

	if (the_panel->for_disk_ == true)
	{
		the_file = Folder_GetCurrentFile(the_panel->root_folder_);
		if (the_file == NULL)
			return false;

		// Build full file path
		sprintf(global_temp_path_1, "%u:%s",
			the_panel->root_folder_->device_number_,
			App_GetFilename(the_file->panel_id_, the_file->id_));

		if (the_file->file_type_ == _CBM_T_DIR)
		{
			// Open directory
			if ((the_panel->root_folder_ = Folder_NewOrReset(the_panel->root_folder_, the_panel->device_number_, global_temp_path_1)) == NULL)
			{
				App_Exit(ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ);
			}

			Panel_Refresh(the_panel);
			success = true;
		}
		else if (the_file->file_type_ == FNX_FILETYPE_FONT)
		{
			success = File_ReadFontData(global_temp_path_1);
		}
		else if (the_file->file_type_ == FNX_FILETYPE_EXE || the_file->file_type_ == FNX_FILETYPE_IMAGE)
		{
			// Load and run via kernel
			kernelCall(RunNamed);
			success = true;
		}
		else if (the_file->file_type_ == FNX_FILETYPE_MUSIC)
		{
			if (File_CheckForFile((char*)GetString(ID_STR_APP_PATH_MOD_PLAYER), ID_STR_ERROR_NO_MOD_PLAYER))
			{
				kernelCall(RunNamed);
				success = true;
			}
			else
				success = false;
		}
		else if (the_file->file_type_ == FNX_FILETYPE_TEXT)
		{
			if (File_CheckForFile((char*)GetString(ID_STR_APP_PATH_TEXT_EDITOR), ID_STR_ERROR_NO_TEXT_EDITOR))
			{
				kernelCall(RunNamed);
				success = true;
			}
			else
				success = false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		success = MemSys_ExecuteCurrentRow(the_panel->memory_system_);
	}

	return success;
}


bool Panel_CopyCurrentFile(WB2KViewPanel* the_panel, WB2KViewPanel* the_other_panel)
{
	byte			src_bank_num;
	byte			dst_bank_num;
	bool			success = false;

	if (the_panel->for_disk_ == false)
		src_bank_num = MemSys_GetCurrentBankNum(the_panel->memory_system_);

	if (the_other_panel->for_disk_ == false)
		dst_bank_num = MemSys_GetCurrentBankNum(the_other_panel->memory_system_);

	if (the_panel->for_disk_ == true && the_other_panel->for_disk_ == true)
	{
		// Disk to disk copy
		success = Folder_CopyCurrentFile(the_panel->root_folder_, the_other_panel->root_folder_);

		if (success)
			Buffer_NewMessage(GetString(ID_STR_MSG_DONE));
		else
			Buffer_NewMessage(GetString(ID_STR_ERROR_GENERIC_DISK));
	}
	else if (the_panel->for_disk_ == true && the_other_panel->for_disk_ == false)
	{
		// Load file from disk into memory bank
		WB2KFileObject* the_file = Folder_GetCurrentFile(the_panel->root_folder_);
		sprintf(global_temp_path_1, "%u:%s",
			the_panel->root_folder_->device_number_,
			App_GetFilename(the_file->panel_id_, the_file->id_));

		// Load file into the destination bank
		// Use bank switching to copy file data into the bank
		{
			FILE*    src_fh;
			byte     buf[STORAGE_FILE_BUFFER_1_LEN];
			int16_t  bytes_read;
			byte     saved_bank;
			uint16_t offset = 0;

			src_fh = fopen(global_temp_path_1, "r");
			if (src_fh == NULL)
			{
				success = false;
			}
			else
			{
				saved_bank = PEEK(SWAP_SLOT);
				POKE_MEMMAP(SWAP_SLOT, dst_bank_num);

				App_ShowProgressBar();

				while ((bytes_read = fread(buf, 1, STORAGE_FILE_BUFFER_1_LEN, src_fh)) > 0 && offset < 0x2000)
				{
					uint16_t i;
					for (i = 0; i < bytes_read && (offset + i) < 0x2000; i++)
						POKE(SWAP_ADDR + offset + i, buf[i]);

					offset += bytes_read;
					App_UpdateProgressBar((byte)((uint32_t)offset * 100 / 0x2000));
				}

				POKE_MEMMAP(SWAP_SLOT, saved_bank);
				fclose(src_fh);

				App_HideProgressBar();
				success = true;
			}
		}
	}
	else if (the_panel->for_disk_ == false && the_other_panel->for_disk_ == true)
	{
		// Copy memory bank to file on disk
		char* the_name;

		sprintf(global_temp_path_2, "Bank_%02X.bin", src_bank_num);

		the_name = Screen_GetStringFromUser(
			(char*)GetString(ID_STR_DLG_COPY_TO_FILE_TITLE),
			(char*)GetString(ID_STR_DLG_ENTER_FILE_NAME),
			global_temp_path_2,
			FILE_MAX_FILENAME_SIZE
		);

		if (the_name == NULL)
			return false;

		// Build target path
		sprintf(global_temp_path_1, "%u:%s",
			the_other_panel->root_folder_->device_number_, the_name);

		// Write bank data to file
		{
			FILE*    dst_fh;
			byte     buf[STORAGE_FILE_BUFFER_1_LEN];
			byte     saved_bank;
			uint16_t offset;

			dst_fh = fopen(global_temp_path_1, "w");
			if (dst_fh == NULL)
			{
				success = false;
			}
			else
			{
				App_ShowProgressBar();

				saved_bank = PEEK(SWAP_SLOT);
				POKE_MEMMAP(SWAP_SLOT, src_bank_num);

				for (offset = 0; offset < 0x2000; offset += STORAGE_FILE_BUFFER_1_LEN)
				{
					uint16_t i;
					for (i = 0; i < STORAGE_FILE_BUFFER_1_LEN; i++)
						buf[i] = PEEK(SWAP_ADDR + offset + i);

					fwrite(buf, 1, STORAGE_FILE_BUFFER_1_LEN, dst_fh);
					App_UpdateProgressBar((byte)((uint32_t)offset * 100 / 0x2000));
				}

				POKE_MEMMAP(SWAP_SLOT, saved_bank);
				fclose(dst_fh);

				App_HideProgressBar();
				success = true;
			}
		}
	}
	else if (the_panel->for_disk_ == false && the_other_panel->for_disk_ == false)
	{
		// Memory bank to memory bank copy
		byte     buf[STORAGE_FILE_BUFFER_1_LEN];
		byte     saved_bank;
		uint16_t offset;

		if (src_bank_num == dst_bank_num)
		{
			Buffer_NewMessage(GetString(ID_STR_ERROR_ATTEMPT_COPY_BANK_TO_ITSELF));
			return false;
		}

		if (MemSys_BankIsWriteable(the_other_panel->memory_system_) == false)
		{
			Buffer_NewMessage(GetString(ID_STR_ERROR_ATTEMPT_TO_OVERWRITE_FM_RAM));
			return false;
		}

		saved_bank = PEEK(SWAP_SLOT);

		// Read from source bank in chunks
		for (offset = 0; offset < 0x2000; offset += STORAGE_FILE_BUFFER_1_LEN)
		{
			uint16_t i;

			POKE_MEMMAP(SWAP_SLOT, src_bank_num);
			for (i = 0; i < STORAGE_FILE_BUFFER_1_LEN; i++)
				buf[i] = PEEK(SWAP_ADDR + offset + i);

			POKE_MEMMAP(SWAP_SLOT, dst_bank_num);
			for (i = 0; i < STORAGE_FILE_BUFFER_1_LEN; i++)
				POKE(SWAP_ADDR + offset + i, buf[i]);
		}

		POKE_MEMMAP(SWAP_SLOT, saved_bank);
		success = true;
	}

	Panel_Refresh(the_other_panel);

	if (the_other_panel == the_panel)
		Panel_Refresh(the_panel);

	return success;
}


bool Panel_ViewCurrentFile(WB2KViewPanel* the_panel, byte the_viewer_type)
{
	int16_t			the_current_row;
	byte			num_pages;
	byte			bank_num;
	WB2KFileObject*	the_file;
	bool			success;
	char*			the_name;

	if (the_panel->for_disk_ == true)
		the_current_row = Folder_GetCurrentRow(the_panel->root_folder_);
	else
		the_current_row = MemSys_GetCurrentRow(the_panel->memory_system_);

	if (the_current_row < 0)
		return false;

	if (the_panel->for_disk_ == true)
	{
		the_file = Folder_FindFileByRow(the_panel->root_folder_, the_current_row);
		the_name = App_GetFilename(the_file->panel_id_, the_file->id_);

		// Build path and load file into a temp bank for viewing
		sprintf(global_temp_path_1, "%u:%s",
			the_panel->root_folder_->device_number_, the_name);

		num_pages = the_file->size_ / 256;
		if (num_pages == 0) num_pages = 1;
		bank_num = 0x14;  // EM_STORAGE_START_PHYS_BANK_NUM equivalent

		// Load file into bank for viewing
		{
			FILE*    src_fh;
			byte     saved_bank;
			uint16_t offset = 0;
			int16_t  bytes_read;
			byte     buf[STORAGE_FILE_BUFFER_1_LEN];

			src_fh = fopen(global_temp_path_1, "r");
			if (src_fh == NULL)
			{
				success = false;
				goto view_done;
			}

			saved_bank = PEEK(SWAP_SLOT);
			POKE_MEMMAP(SWAP_SLOT, bank_num);

			while ((bytes_read = fread(buf, 1, STORAGE_FILE_BUFFER_1_LEN, src_fh)) > 0 && offset < 0x2000)
			{
				uint16_t i;
				for (i = 0; i < bytes_read && (offset + i) < 0x2000; i++)
					POKE(SWAP_ADDR + offset + i, buf[i]);
				offset += bytes_read;
			}

			POKE_MEMMAP(SWAP_SLOT, saved_bank);
			fclose(src_fh);
			success = true;
		}
	}
	else
	{
		the_name = the_panel->memory_system_->bank_[the_current_row].name_;
		num_pages = 32;  // 8192/256 = 32 pages per bank
		bank_num = the_panel->memory_system_->bank_[the_current_row].bank_num_;
		success = true;
	}

view_done:
	if (success)
	{
		if (the_viewer_type == PARAM_VIEW_AS_HEX)
			EM_DisplayAsHex(bank_num, num_pages, the_name);
		else
			EM_DisplayAsText(bank_num, num_pages, the_name);
	}
	else
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_FAIL_VIEW_FILE));
	}

	return success;
}


bool Panel_SelectPrevFile(WB2KViewPanel* the_panel)
{
	int16_t	the_current_row;

	if (the_panel->for_disk_ == true)
		the_current_row = Folder_GetCurrentRow(the_panel->root_folder_);
	else
		the_current_row = MemSys_GetCurrentRow(the_panel->memory_system_);

	if (--the_current_row < 0)
		return false;

	return Panel_SetFileSelectionByRow(the_panel, the_current_row, true);
}


bool Panel_SelectNextFile(WB2KViewPanel* the_panel)
{
	int16_t		the_current_row;
	uint16_t	the_item_count;

	if (the_panel->for_disk_ == true)
	{
		the_current_row = Folder_GetCurrentRow(the_panel->root_folder_);
		the_item_count = Folder_GetCountFiles(the_panel->root_folder_);
	}
	else
	{
		the_current_row = MemSys_GetCurrentRow(the_panel->memory_system_);
		the_item_count = MEMORY_BANK_COUNT;
	}

	if (++the_current_row == the_item_count)
		return false;

	return Panel_SetFileSelectionByRow(the_panel, the_current_row, true);
}


bool Panel_SetFileSelectionByRow(WB2KViewPanel* the_panel, uint16_t the_row, bool do_selection)
{
	bool			scroll_needed = false;
	bool			success;
	byte			content_top = the_panel->content_top_;
	WB2KFileObject*	the_file;
	FMBankObject*	the_bank;

	if (the_panel == NULL)
		App_Exit(ERROR_PANEL_WAS_NULL);

	if (the_panel->for_disk_ == true)
	{
		the_file = Folder_SetFileSelectionByRow(the_panel->root_folder_, the_row, do_selection, the_panel->y_);
		success = (the_file != NULL);
	}
	else
	{
		the_bank = MemSys_SetBankSelectionByRow(the_panel->memory_system_, the_row, do_selection, the_panel->y_, the_panel->active_);
		success = (the_bank != NULL);
	}

	if (success)
	{
		if (the_row >= (content_top + the_panel->height_))
		{
			// Row is off the bottom - scroll up
			scroll_needed = true;
			++the_panel->content_top_;
			textScrollRowsUp(the_panel->y_, the_panel->y_ + the_panel->height_ - 1);
		}
		else if (the_row < content_top)
		{
			// Row is off the top - scroll down
			scroll_needed = true;
			--the_panel->content_top_;
			textScrollRowsDown(the_panel->y_, the_panel->y_ + the_panel->height_ - 1);
		}

		if (scroll_needed == true)
		{
			Panel_ReflowContent(the_panel);

			if (the_panel->for_disk_ == true)
				File_Render(the_file, File_IsSelected(the_file), the_panel->y_, the_panel->active_);
			else
				Bank_Render(the_bank, Bank_IsSelected(the_bank), the_panel->y_, the_panel->active_);
		}
	}

	return success;
}


void Panel_ReflowContent(WB2KViewPanel* the_panel)
{
	if (the_panel->for_disk_ == true)
		Panel_ReflowContentForDisk(the_panel);
	else
		Panel_ReflowContentForMemory(the_panel);
}


void Panel_ClearDisplay(WB2KViewPanel* the_panel)
{
	textFillBox(
		the_panel->x_, the_panel->y_,
		the_panel->x_ + the_panel->width_ - 1, the_panel->y_ + the_panel->height_ - 1,
		' ', LIST_ACTIVE_COLOR, APP_BACKGROUND_COLOR
	);
}


void Panel_RenderContents(WB2KViewPanel* the_panel)
{
	// Clear the panel
	textFillBox(
		the_panel->x_, the_panel->y_,
		the_panel->x_ + (UI_PANEL_INNER_WIDTH - 1), the_panel->y_ + (UI_PANEL_INNER_HEIGHT - 2),
		' ', LIST_ACTIVE_COLOR, APP_BACKGROUND_COLOR
	);

	// Draw panel header
	Screen_DrawPanelHeader(the_panel->x_, the_panel->for_disk_);

	// Render content items
	if (the_panel->for_disk_ == true)
	{
		WB2KList* the_item = *(the_panel->root_folder_->list_);

		while (the_item != NULL)
		{
			WB2KFileObject* this_file = (WB2KFileObject*)(the_item->payload_);
			File_Render(this_file, File_IsSelected(this_file), the_panel->y_, the_panel->active_);
			the_item = the_item->next_item_;
		}
	}
	else
	{
		byte row;

		for (row = 0; row < MEMORY_BANK_COUNT; row++)
		{
			FMBankObject* this_bank = &the_panel->memory_system_->bank_[row];
			Bank_Render(this_bank, Bank_IsSelected(this_bank), the_panel->y_, the_panel->active_);
		}
	}

	// Draw folder title in the top tab
	Panel_RenderTitleOnly(the_panel);
}


void Panel_SortAndDisplay(WB2KViewPanel* the_panel)
{
	WB2KFileObject* the_file;

	if (the_panel->for_disk_ == true)
	{
		List_InitMergeSort(the_panel->root_folder_->list_, the_panel->sort_compare_function_);

		Panel_ReflowContent(the_panel);
		Panel_RenderContents(the_panel);

		// Select first file after sort
		if (the_panel->root_folder_->file_count_ > 0)
		{
			the_file = Folder_SetFileSelectionByRow(the_panel->root_folder_, 0, true, the_panel->y_);
			if (the_file)
				File_Render(the_file, File_IsSelected(the_file), the_panel->y_, the_panel->active_);
		}
		else
		{
			Folder_SetCurrentRow(the_panel->root_folder_, -1);
		}

		Screen_UpdateSortIcons(the_panel->x_, the_panel->sort_compare_function_);
		Screen_UpdateMeatloafIcon(the_panel->x_, the_panel->root_folder_->is_meatloaf_);
	}
	else
	{
		Panel_ReflowContent(the_panel);
		Panel_RenderContents(the_panel);

		Screen_UpdateMeatloafIcon(the_panel->x_, false);

		MemSys_SetBankSelectionByRow(the_panel->memory_system_, 0, PARAM_MARK_SELECTED, the_panel->y_, the_panel->active_);
	}
}


bool Panel_FillCurrentBank(WB2KViewPanel* the_panel)
{
	if (MemSys_BankIsWriteable(the_panel->memory_system_) == false)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_ATTEMPT_TO_OVERWRITE_FM_RAM));
		return false;
	}

	return MemSys_FillCurrentBank(the_panel->memory_system_);
}


bool Panel_ClearCurrentBank(WB2KViewPanel* the_panel)
{
	if (MemSys_BankIsWriteable(the_panel->memory_system_) == false)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_ATTEMPT_TO_OVERWRITE_FM_RAM));
		return false;
	}

	return MemSys_ClearCurrentBank(the_panel->memory_system_);
}


bool Panel_SearchCurrentBank(WB2KViewPanel* the_panel)
{
	byte	the_bank_num;
	char*	search_phrase;

	if (the_panel->for_disk_ == true)
		return false;

	the_bank_num = MemSys_GetCurrentBankNum(the_panel->memory_system_);

	// Copy previous search phrase as default
	generalStrlcpy(global_string_buff1, global_search_phrase_human_readable, MAX_SEARCH_PHRASE_LEN + 1);

	search_phrase = Screen_GetStringFromUser(
		(char*)GetString(ID_STR_DLG_SEARCH_BANK_TITLE),
		(char*)GetString(ID_STR_DLG_SEARCH_BANK_BODY),
		global_string_buff1,
		MAX_SEARCH_PHRASE_LEN
	);

	if (search_phrase == NULL)
	{
		global_search_phrase_len = 0;
		global_search_phrase[0] = 0;
		return false;
	}

	// Save human-readable copy
	generalStrlcpy(global_search_phrase_human_readable, search_phrase, MAX_SEARCH_PHRASE_LEN + 1);
	global_search_phrase_len = generalStrnlen(search_phrase, MAX_SEARCH_PHRASE_LEN);
	memcpy(global_search_phrase, search_phrase, global_search_phrase_len);

	if ((global_find_next_enabled = EM_SearchMemory(the_bank_num, 0, 0, true)) == false)
		return false;

	return true;
}


bool Panel_OpenMeatloafURL(WB2KViewPanel* the_panel)
{
	char* the_name;

	if (the_panel->for_disk_ == false)
		return false;

	if (the_panel->root_folder_->is_meatloaf_ == false)
		return false;

	generalStrlcpy(global_temp_path_2, GetString(ID_STR_DLG_MEATLOAF_DEFAULT_URL), FILE_MAX_FILENAME_SIZE);

	the_name = Screen_GetStringFromUser(
		(char*)GetString(ID_STR_DLG_MEATLOAF_URL_TITLE),
		(char*)GetString(ID_STR_DLG_MEATLOAF_URL_BODY),
		global_temp_path_2,
		FILE_MAX_FILENAME_SIZE
	);

	if (the_name == NULL)
		return false;

	// Try to "load" the URL as a directory (Meatloaf protocol)
	// This is a stub - actual Meatloaf integration would use kernel file open
	Panel_Refresh(the_panel);

	return true;
}
