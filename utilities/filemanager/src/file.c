/*
 * file.c - File object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "file.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                           File-scoped Variables                           */
/*****************************************************************************/

static char		file_compare_filename_buffer[FILE_MAX_FILENAME_SIZE];
static byte		temp_file_extension_buffer[FILE_MAX_EXTENSION_SIZE];


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

static byte File_GetFileTypeFromExtension(byte default_file_type, const char* the_file_name);
static const char* File_GetFileTypeString(byte filetype_id);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

static byte File_GetFileTypeFromExtension(byte default_file_type, const char* the_file_name)
{
	if (!generalExtractFileExtension(the_file_name, (char*)temp_file_extension_buffer))
		return default_file_type;

	if (generalStrncasecmp((char*)temp_file_extension_buffer, "pgz", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_EXE;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "bas", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_BASIC;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "txt", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_TEXT;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "mod", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_MUSIC;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "mid", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_MIDI;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "vgm", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_VGM;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "rsd", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_RSD;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "mp3", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_MP3;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "ogg", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_OGG;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "wav", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_WAV;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "pgx", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_EXE;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "fnt", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_FONT;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "kup", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_EXE;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "lbm", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_IMAGE;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "256", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_IMAGE;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "src", FILE_MAX_EXTENSION_SIZE) == 0)
		return FNX_FILETYPE_TEXT;
	if (generalStrncasecmp((char*)temp_file_extension_buffer, "url", FILE_MAX_EXTENSION_SIZE) == 0)
		return _CBM_T_DIR;

	return default_file_type;
}


static const char* File_GetFileTypeString(byte filetype_id)
{
	switch (filetype_id)
	{
		case _CBM_T_CBM:    return GetString(ID_STR_FILETYPE_SUBDIR);
		case _CBM_T_DIR:    return GetString(ID_STR_FILETYPE_DIR);
		case _CBM_T_LNK:    return GetString(ID_STR_FILETYPE_LINK);
		case _CBM_T_HEADER: return GetString(ID_STR_FILETYPE_HEADER);
		case FNX_FILETYPE_BASIC:  return GetString(ID_STR_FILETYPE_BASIC);
		case FNX_FILETYPE_FONT:   return GetString(ID_STR_FILETYPE_FONT);
		case FNX_FILETYPE_EXE:    return GetString(ID_STR_FILETYPE_EXE);
		case FNX_FILETYPE_IMAGE:  return GetString(ID_STR_FILETYPE_IMAGE);
		case FNX_FILETYPE_MUSIC:  return GetString(ID_STR_FILETYPE_MUSIC);
		case FNX_FILETYPE_MP3:    return GetString(ID_STR_FILETYPE_MP3);
		case FNX_FILETYPE_OGG:    return GetString(ID_STR_FILETYPE_OGG);
		case FNX_FILETYPE_WAV:    return GetString(ID_STR_FILETYPE_WAV);
		case FNX_FILETYPE_TEXT:   return GetString(ID_STR_FILETYPE_TEXT);
		case FNX_FILETYPE_MIDI:   return GetString(ID_STR_FILETYPE_MIDI);
		case FNX_FILETYPE_VGM:    return GetString(ID_STR_FILETYPE_VGM);
		case FNX_FILETYPE_RSD:    return GetString(ID_STR_FILETYPE_RSD);
		default:                  return GetString(ID_STR_FILETYPE_OTHER);
	}
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

WB2KFileObject* File_New(byte the_panel_id, const char* the_file_name, bool is_directory, uint32_t the_filesize, byte the_filetype, byte the_row, DateTime* the_datetime)
{
	WB2KFileObject* the_file;

	the_file = (WB2KFileObject*)malloc(sizeof(WB2KFileObject));
	if (the_file == NULL)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
		return NULL;
	}

	memset(the_file, 0, sizeof(WB2KFileObject));

	the_file->panel_id_ = the_panel_id;
	the_file->row_ = the_row;
	the_file->id_ = the_row;

	// Store filename in the filename storage system
	App_SetFilename(the_panel_id, the_row, the_file_name);

	the_file->size_ = the_filesize;

	// Determine file type from extension if needed
	if (the_filetype == _CBM_T_REG)
		the_file->file_type_ = File_GetFileTypeFromExtension(the_filetype, the_file_name);
	else
		the_file->file_type_ = the_filetype;

	the_file->is_directory_ = is_directory;
	the_file->selected_ = false;

	if (the_datetime != NULL)
	{
		the_file->datetime_.year = the_datetime->year;
		the_file->datetime_.month = the_datetime->month;
		the_file->datetime_.day = the_datetime->day;
		the_file->datetime_.hour = the_datetime->hour;
		the_file->datetime_.min = the_datetime->min;
		the_file->datetime_.sec = the_datetime->sec;
	}

	return the_file;
}


WB2KFileObject* File_Duplicate(WB2KFileObject* the_original_file)
{
	WB2KFileObject* the_dup;

	if (the_original_file == NULL) return NULL;

	the_dup = (WB2KFileObject*)malloc(sizeof(WB2KFileObject));
	if (the_dup == NULL) return NULL;

	memcpy(the_dup, the_original_file, sizeof(WB2KFileObject));
	the_dup->selected_ = false;

	return the_dup;
}


void File_Destroy(WB2KFileObject** the_file)
{
	if (*the_file == NULL) return;
	free(*the_file);
	*the_file = NULL;
}


void File_UpdatePos(WB2KFileObject* the_file, byte x, int8_t display_row, uint16_t row)
{
	if (the_file == NULL) return;
	the_file->x_ = x;
	the_file->display_row_ = display_row;
	the_file->row_ = (byte)row;
}


bool File_UpdateFileName(WB2KFileObject* the_file, const char* new_file_name)
{
	if (the_file == NULL) return false;
	App_SetFilename(the_file->panel_id_, the_file->id_, new_file_name);
	return true;
}


bool File_IsSelected(WB2KFileObject* the_file)
{
	if (the_file == NULL) return false;
	return the_file->selected_;
}


bool File_IsFolder(WB2KFileObject* the_file)
{
	if (the_file == NULL) return false;
	return the_file->is_directory_;
}


bool File_ReadFontData(char* the_file_path)
{
	byte    buf[256];
	int16_t bytes_read;
	uint16_t bytes_still_needed;
	FILE*   fh;

	if (the_file_path == NULL) return false;

	bytes_still_needed = 2048; // TEXT_FONT_BYTE_SIZE

	fh = fopen(the_file_path, "r");
	if (fh == NULL)
	{
		Buffer_NewMessage(GetString(ID_STR_ERROR_FAIL_OPEN_FILE));
		return false;
	}

	// Load font data into VICKY font memory via I/O page
	{
		uint16_t offset = 0;
		byte mmu = PEEK(MMU_IO_CTRL);

		while (bytes_still_needed > 0)
		{
			bytes_read = fread(buf, 1, 256, fh);
			if (bytes_read <= 0) break;

			POKE_MEMMAP(MMU_IO_CTRL, 0x01); // font/LUT I/O page
			{
				uint16_t i;
				for (i = 0; i < (uint16_t)bytes_read; i++)
					POKE(0xC000 + offset + i, buf[i]);
			}
			POKE_MEMMAP(MMU_IO_CTRL, mmu);

			offset += bytes_read;
			bytes_still_needed -= bytes_read;
		}

		POKE_MEMMAP(MMU_IO_CTRL, mmu);
	}

	fclose(fh);
	return true;
}


bool File_CheckForFile(char* the_file_path, byte feedback_string_id)
{
	FILE* fh;

	fh = fopen(the_file_path, "r");
	if (fh == NULL)
	{
		Buffer_NewMessage(GetString(feedback_string_id));
		return false;
	}
	fclose(fh);
	return true;
}


bool File_Delete(char* the_file_path, bool is_directory)
{
	int8_t result;

	if (is_directory)
		result = fileRemoveDir(the_file_path);
	else
		result = fileUnlink(the_file_path);

	return (result == 0);
}


bool File_Rename(WB2KFileObject* the_file, const char* new_file_name, const char* old_file_path, const char* new_file_path)
{
	int8_t result_code;

	if (the_file == NULL) return false;

	result_code = rename(old_file_path, new_file_path);
	if (result_code < 0) return false;

	if (File_UpdateFileName(the_file, new_file_name) == false) return false;

	the_file->file_type_ = File_GetFileTypeFromExtension(_CBM_T_REG, new_file_name);
	return true;
}


bool File_MarkSelected(WB2KFileObject* the_file, int8_t y_offset)
{
	if (the_file == NULL) return false;

	if (the_file->selected_ == false)
	{
		the_file->selected_ = true;
		File_Render(the_file, true, y_offset, true);
	}
	return true;
}


bool File_MarkUnSelected(WB2KFileObject* the_file, int8_t y_offset)
{
	if (the_file == NULL) return false;

	if (the_file->selected_ == true)
	{
		the_file->selected_ = false;
		File_Render(the_file, false, y_offset, true);
	}
	return true;
}


void File_Render(WB2KFileObject* the_file, bool as_selected, int8_t y_offset, bool as_active)
{
	byte	x1, x2, sizex, typex, the_color;
	int8_t	y;
	char*   filename;

	if (the_file == NULL) return;
	if (the_file->display_row_ == -1) return;

	the_color = as_active ? LIST_ACTIVE_COLOR : LIST_INACTIVE_COLOR;

	x1 = the_file->x_;
	x2 = the_file->x_ + (UI_PANEL_INNER_WIDTH - 1);
	typex = x1 + UI_PANEL_FILETYPE_OFFSET;
	sizex = typex + UI_PANEL_FILESIZE_OFFSET - 1;

	y = the_file->display_row_ + y_offset;

	// Get filename
	filename = App_GetFilename(the_file->panel_id_, the_file->id_);

	// Clear row, draw filename, size, and type
	textFillBox(x1, y, x2, y, ' ', the_color, APP_BACKGROUND_COLOR);

	sprintf(global_string_buff1, "%6lu", the_file->size_);
	textDrawStringAt(x1, y, filename, the_color, APP_BACKGROUND_COLOR);
	textDrawStringAt(sizex, y, global_string_buff1, the_color, APP_BACKGROUND_COLOR);
	textDrawStringAt(typex, y, File_GetFileTypeString(the_file->file_type_), the_color, APP_BACKGROUND_COLOR);

	if (as_selected)
	{
		textInvertBox(x1, y, x2, y);

		// Show filename on the status line
		textFillBox(0, UI_FULL_PATH_LINE_Y, 79, UI_FULL_PATH_LINE_Y, ' ', APP_BACKGROUND_COLOR, APP_BACKGROUND_COLOR);
		textDrawStringAt(0, UI_FULL_PATH_LINE_Y, filename, DARK_GREEN, APP_BACKGROUND_COLOR);
	}
}


bool File_CompareSize(void* first_payload, void* second_payload)
{
	WB2KFileObject* f1 = (WB2KFileObject*)first_payload;
	WB2KFileObject* f2 = (WB2KFileObject*)second_payload;
	return (f1->size_ > f2->size_);
}


bool File_CompareFileTypeID(void* first_payload, void* second_payload)
{
	WB2KFileObject* f1 = (WB2KFileObject*)first_payload;
	WB2KFileObject* f2 = (WB2KFileObject*)second_payload;
	return (f1->file_type_ > f2->file_type_);
}


bool File_CompareName(void* first_payload, void* second_payload)
{
	WB2KFileObject* f1 = (WB2KFileObject*)first_payload;
	WB2KFileObject* f2 = (WB2KFileObject*)second_payload;
	char* name1;
	char* name2;

	name2 = App_GetFilename(f2->panel_id_, f2->id_);
	memcpy(file_compare_filename_buffer, name2, FILE_MAX_FILENAME_SIZE);
	name1 = App_GetFilename(f1->panel_id_, f1->id_);

	return (generalStrncasecmp(name1, file_compare_filename_buffer, FILE_MAX_FILENAME_SIZE) > 0);
}
