/*
 * file.h - File object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef FILE_H_
#define FILE_H_

#include "app.h"

#pragma compile("file.c")


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PARAM_FILE_IS_FOLDER		true
#define PARAM_FILE_IS_NOT_FOLDER	false

// CBM/kernel file type constants
#define _CBM_T_DEL      0x00
#define _CBM_T_CBM      0x01
#define _CBM_T_DIR      0x02
#define _CBM_T_LNK      0x03
#define _CBM_T_OTHER    0x04
#define _CBM_T_HEADER   0x05
#define _CBM_T_REG      0x10
#define _CBM_T_SEQ      0x10
#define _CBM_T_PRG      0x11

// F256 custom file type IDs (200+)
#define FNX_FILETYPE_FONT	200
#define FNX_FILETYPE_EXE	201
#define FNX_FILETYPE_BASIC	202
#define FNX_FILETYPE_MUSIC	203
#define FNX_FILETYPE_IMAGE	204
#define FNX_FILETYPE_TEXT	205
#define FNX_FILETYPE_MIDI	206
#define FNX_FILETYPE_MP3	207
#define FNX_FILETYPE_OGG	208
#define FNX_FILETYPE_WAV	209
#define FNX_FILETYPE_VGM	210
#define FNX_FILETYPE_RSD	211


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct WB2KFileObject
{
	uint32_t		size_;
	DateTime		datetime_;
	bool			is_directory_;
	bool			selected_;
	byte			panel_id_;
	byte			id_;
	byte			file_type_;
	byte			x_;
	byte			row_;
	int8_t			display_row_;
} WB2KFileObject;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Constructor
WB2KFileObject* File_New(byte the_panel_id, const char* the_file_name, bool is_directory, uint32_t the_filesize, byte the_filetype, byte the_row, DateTime* the_datetime);

// Duplicator
WB2KFileObject* File_Duplicate(WB2KFileObject* the_original_file);

// Destructor
void File_Destroy(WB2KFileObject** the_file);

// Position update
void File_UpdatePos(WB2KFileObject* the_file, byte x, int8_t display_row, uint16_t row);

// Filename update
bool File_UpdateFileName(WB2KFileObject* the_file, const char* new_file_name);

// Getters
bool File_IsSelected(WB2KFileObject* the_file);
bool File_IsFolder(WB2KFileObject* the_file);

// Font loading
bool File_ReadFontData(char* the_file_path);

// File operations
bool File_CheckForFile(char* the_file_path, byte feedback_string_id);
bool File_Delete(char* the_file_path, bool is_directory);
bool File_Rename(WB2KFileObject* the_file, const char* new_file_name, const char* old_file_path, const char* new_file_path);

// Selection
bool File_MarkSelected(WB2KFileObject* the_file, int8_t y_offset);
bool File_MarkUnSelected(WB2KFileObject* the_file, int8_t y_offset);

// Rendering
void File_Render(WB2KFileObject* the_file, bool as_selected, int8_t y_offset, bool as_active);

// Comparison functions for sorting
bool File_CompareSize(void* first_payload, void* second_payload);
bool File_CompareName(void* first_payload, void* second_payload);
bool File_CompareFileTypeID(void* first_payload, void* second_payload);


#endif /* FILE_H_ */
