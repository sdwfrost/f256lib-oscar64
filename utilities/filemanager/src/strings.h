/*
 * strings.h - String constants for the File Manager
 * Ported from F256-FileManager CC65 version
 *
 * In the CC65 version these were stored in extended memory.
 * In the oscar64 port we use simple C string arrays.
 */

#ifndef STRINGS_H_
#define STRINGS_H_


static const char * const fm_strings[] = {
	"Quit f/manager?",                  // 0  ID_STR_DLG_QUIT_CONFIRM
	"Are you sure?",                    // 1  ID_STR_DLG_ARE_YOU_SURE
	"Rename File",                      // 2  ID_STR_DLG_RENAME_TITLE
	"Copy to...",                       // 3  ID_STR_DLG_COPY_TO_FILE_TITLE
	"Enter file name",                  // 4  ID_STR_DLG_ENTER_FILE_NAME
	"Delete File",                      // 5  ID_STR_DLG_DELETE_TITLE
	"Format Disk",                      // 6  ID_STR_DLG_FORMAT_TITLE
	"Yes",                              // 7  ID_STR_DLG_YES
	"No",                               // 8  ID_STR_DLG_NO
	"OK",                               // 9  ID_STR_DLG_OK
	"Cancel",                           // 10 ID_STR_DLG_CANCEL
	"Enter new name",                   // 11 ID_STR_DLG_ENTER_NEW_NAME
	"Set Clock",                        // 12 ID_STR_DLG_SET_CLOCK_TITLE
	"YY-MM-DD HH:MM",                  // 13 ID_STR_DLG_SET_CLOCK_BODY
	"Meatloaf URL",                     // 14 ID_STR_DLG_MEATLOAF_URL_TITLE
	"Enter URL",                        // 15 ID_STR_DLG_MEATLOAF_URL_BODY
	"http://",                          // 16 ID_STR_DLG_MEATLOAF_DEFAULT_URL
	"Fill Bank",                        // 17 ID_STR_DLG_FILL_BANK_TITLE
	"Enter hex byte value",             // 18 ID_STR_DLG_FILL_BANK_BODY
	"",                                 // 19 (unused)
	"Search Bank",                      // 20 ID_STR_DLG_SEARCH_BANK_TITLE
	"Enter search phrase",              // 21 ID_STR_DLG_SEARCH_BANK_BODY
	"New Folder",                       // 22 ID_STR_DLG_NEW_FOLDER_TITLE
	"Enter folder name",                // 23 ID_STR_DLG_ENTER_NEW_FOLDER_NAME
	"App",                              // 24 ID_STR_MENU_APP
	"Set Clock",                        // 25 ID_STR_APP_SET_CLOCK
	"About",                            // 26 ID_STR_APP_ABOUT
	"Quit",                             // 27 ID_STR_APP_QUIT
	"Exit BASIC",                       // 28 ID_STR_APP_EXIT_TO_BASIC
	"Exit DOS",                         // 29 ID_STR_APP_EXIT_TO_DOS
	"Device",                           // 30 ID_STR_MENU_DEVICE
	"SD",                               // 31 ID_STR_DEV_SD
	"Floppy 1",                         // 32 ID_STR_DEV_FLOPPY_1
	"Floppy 2",                         // 33 ID_STR_DEV_FLOPPY_2
	"RAM",                              // 34 ID_STR_DEV_RAM
	"Flash",                            // 35 ID_STR_DEV_FLASH
	"Format",                           // 36 ID_STR_DEV_FORMAT
	"SD Card",                          // 37 ID_STR_DEV_SD_CARD
	"Directory",                        // 38 ID_STR_MENU_DIRECTORY
	"mkdir",                            // 39 ID_STR_DEV_MAKE_DIR
	"Refresh",                          // 40 ID_STR_DEV_REFRESH_LISTING
	"Meatloaf",                         // 41 ID_STR_DEV_ENTER_MEATLOAF_URL
	"Sort Type",                        // 42 ID_STR_DEV_SORT_BY_TYPE
	"Sort Name",                        // 43 ID_STR_DEV_SORT_BY_NAME
	"Sort Size",                        // 44 ID_STR_DEV_SORT_BY_SIZE
	"File",                             // 45 ID_STR_MENU_FILE
	"Copy <-",                          // 46 ID_STR_FILE_COPY_LEFT
	"Copy ->",                          // 47 ID_STR_FILE_COPY_RIGHT
	"Delete",                           // 48 ID_STR_FILE_DELETE
	"Duplicate",                        // 49 ID_STR_FILE_DUP
	"Rename",                           // 50 ID_STR_FILE_RENAME
	"Text View",                        // 51 ID_STR_FILE_TEXT_PREVIEW
	"Hex View",                         // 52 ID_STR_FILE_HEX_PREVIEW
	"Load",                             // 53 ID_STR_FILE_LOAD
	"Clear",                            // 54 ID_STR_BANK_CLEAR
	"Fill",                             // 55 ID_STR_BANK_FILL
	"Find",                             // 56 ID_STR_BANK_FIND
	"Find Next",                        // 57 ID_STR_BANK_FIND_NEXT
	"Name",                             // 58 ID_STR_LBL_FILENAME
	"Date",                             // 59 ID_STR_LBL_FILEDAYS
	"Type",                             // 60 ID_STR_LBL_FILETYPE
	"Size",                             // 61 ID_STR_LBL_FILESIZE
	"Bank",                             // 62 ID_STR_LBL_BANK_NUM
	"Address",                          // 63 ID_STR_LBL_BANK_ADDRESS
	"ML Dir",                           // 64 ID_STR_LBL_MEATLOAF_DIR_NAME
	"ML Local",                         // 65 ID_STR_LBL_MEATLOAF_LOCAL_MODIFIER
	"ML Mode",                          // 66 ID_STR_LBL_MEATLOAF_MODE
	"Scanning...",                      // 67 ID_STR_MSG_SCANNING
	"%d drives found",                  // 68 ID_STR_MSG_SHOW_DRIVE_COUNT
	"Formatting...",                    // 69 ID_STR_MSG_FORMATTING
	"Copying...",                       // 70 ID_STR_MSG_COPYING
	"Deleting...",                      // 71 ID_STR_MSG_DELETING
	"Reading dir...",                   // 72 ID_STR_MSG_READING_DIR
	"Delete OK",                        // 73 ID_STR_MSG_DELETE_SUCCESS
	"Rename OK",                        // 74 ID_STR_MSG_RENAME_SUCCESS
	"Done.",                            // 75 ID_STR_MSG_DONE
	"More...",                          // 76 ID_STR_MSG_MORE
	"%d files found",                   // 77 ID_STR_N_FILES_FOUND
	"%u bytes free",                    // 78 ID_STR_N_BYTES_FREE
	"Dir truncated (too many files)",   // 79 ID_STR_TRUNCATED_DIR_WARNING
	"Press any key...",                 // 80 ID_STR_MSG_HIT_ANY_KEY
	"Hex: Q=quit",                      // 81 ID_STR_MSG_HEX_VIEW_INSTRUCTIONS
	"Text: Q=quit",                     // 82 ID_STR_MSG_TEXT_VIEW_INSTRUCTIONS
	"BASIC: type LOAD then RUN",        // 83 ID_STR_MSG_BASIC_LOAD_INSTRUCTIONS
	"Found!",                           // 84 ID_STR_MSG_SEARCH_BANK_SUCCESS
	"Not found.",                       // 85 ID_STR_MSG_SEARCH_BANK_FAILURE
	"/sd/apps/edit",                    // 86 ID_STR_APP_PATH_TEXT_EDITOR
	"/sd/apps/midiplay",                // 87 ID_STR_APP_PATH_MIDI_PLAYER
	"/sd/apps/modplay",                 // 88 ID_STR_APP_PATH_MOD_PLAYER
	"/sd/apps/audio",                   // 89 ID_STR_APP_PATH_AUDIO_PLAYER
	"/sd/apps/vgmplay",                 // 90 ID_STR_APP_PATH_VGM_PLAYER
	"/sd/apps/rsdplay",                 // 91 ID_STR_APP_PATH_RSD_PLAYER
	"HDR",                              // 92 ID_STR_FILETYPE_HEADER
	"DIR",                              // 93 ID_STR_FILETYPE_DIR
	"LNK",                              // 94 ID_STR_FILETYPE_LINK
	"SUB",                              // 95 ID_STR_FILETYPE_SUBDIR
	"---",                              // 96 ID_STR_FILETYPE_OTHER
	"BAS",                              // 97 ID_STR_FILETYPE_BASIC
	"EXE",                              // 98 ID_STR_FILETYPE_EXE
	"FNT",                              // 99 ID_STR_FILETYPE_FONT
	"MUS",                              // 100 ID_STR_FILETYPE_MUSIC
	"IMG",                              // 101 ID_STR_FILETYPE_IMAGE
	"TXT",                              // 102 ID_STR_FILETYPE_TEXT
	"MID",                              // 103 ID_STR_FILETYPE_MIDI
	"MP3",                              // 104 ID_STR_FILETYPE_MP3
	"OGG",                              // 105 ID_STR_FILETYPE_OGG
	"WAV",                              // 106 ID_STR_FILETYPE_WAV
	"VGM",                              // 107 ID_STR_FILETYPE_VGM
	"RSD",                              // 108 ID_STR_FILETYPE_RSD
	"Fatal error: %d",                  // 109 ID_STR_MSG_FATAL_ERROR
	"Delete file failed",               // 110 ID_STR_MSG_DELETE_FILE_FAILURE
	"Delete dir failed",                // 111 ID_STR_MSG_DELETE_DIR_FAILURE
	"Press any key to restart.",         // 112 ID_STR_MSG_FATAL_ERROR_BODY
	"No drives found",                  // 113 ID_STR_MSG_NO_DRIVES_AVAILABLE
	"Disk full",                        // 114 ID_STR_ERROR_DISK_FULL
	"Disk error",                       // 115 ID_STR_ERROR_GENERIC_DISK
	"No disk",                          // 116 ID_STR_ERROR_NO_DISK
	"Can't view file",                  // 117 ID_STR_ERROR_FAIL_VIEW_FILE
	"Can't open file",                  // 118 ID_STR_ERROR_FAIL_OPEN_FILE
	"Can't open dir",                   // 119 ID_STR_ERROR_FAIL_OPEN_DIR
	"Can't copy to self",              // 120 ID_STR_ERROR_ATTEMPT_COPY_BANK_TO_ITSELF
	"Protected RAM area",              // 121 ID_STR_ERROR_ATTEMPT_TO_OVERWRITE_FM_RAM
	"No text editor",                   // 122 ID_STR_ERROR_NO_TEXT_EDITOR
	"No MIDI player",                   // 123 ID_STR_ERROR_NO_MIDI_PLAYER
	"No MOD player",                    // 124 ID_STR_ERROR_NO_MOD_PLAYER
	"No audio player",                  // 125 ID_STR_ERROR_NO_AUDIO_PLAYER
	"No VGM player",                    // 126 ID_STR_ERROR_NO_VGM_PLAYER
	"No RSD player",                    // 127 ID_STR_ERROR_NO_RSD_PLAYER
	"Memory allocation failed",         // 128 ID_STR_ERROR_ALLOC_FAIL
	"github.com/WartyMN/F256-FileManager",  // 129 ID_STR_ABOUT_GIT
	"Hardware details",                 // 130 ID_STR_ABOUT_HARDWARE_DETAILS
	"Microkernel",                      // 131 ID_STR_ABOUT_MICROKERNEL
	"Flash Loader",                     // 132 ID_STR_ABOUT_FLASH_LOADER
	"SuperBASIC",                       // 133 ID_STR_ABOUT_SUPERBASIC
	"f/manager - F256 File Commander",  // 134 ID_STR_ABOUT_FMANAGER
	"F256Jr",                           // 135 ID_STR_MACHINE_JR
	"F256K",                            // 136 ID_STR_MACHINE_K
	"Unknown",                          // 137 ID_STR_MACHINE_UNKNOWN
};

// Use same IDs as CC65 version
#define ID_STR_DLG_QUIT_CONFIRM           0
#define ID_STR_DLG_ARE_YOU_SURE           1
#define ID_STR_DLG_RENAME_TITLE           2
#define ID_STR_DLG_COPY_TO_FILE_TITLE     3
#define ID_STR_DLG_ENTER_FILE_NAME        4
#define ID_STR_DLG_DELETE_TITLE           5
#define ID_STR_DLG_FORMAT_TITLE           6
#define ID_STR_DLG_YES                    7
#define ID_STR_DLG_NO                     8
#define ID_STR_DLG_OK                     9
#define ID_STR_DLG_CANCEL                10
#define ID_STR_DLG_ENTER_NEW_NAME        11
#define ID_STR_DLG_SET_CLOCK_TITLE       12
#define ID_STR_DLG_SET_CLOCK_BODY        13
#define ID_STR_DLG_MEATLOAF_URL_TITLE   14
#define ID_STR_DLG_MEATLOAF_URL_BODY    15
#define ID_STR_DLG_MEATLOAF_DEFAULT_URL 16
#define ID_STR_DLG_FILL_BANK_TITLE      17
#define ID_STR_DLG_FILL_BANK_BODY       18
#define ID_STR_DLG_SEARCH_BANK_TITLE    20
#define ID_STR_DLG_SEARCH_BANK_BODY     21
#define ID_STR_DLG_NEW_FOLDER_TITLE     22
#define ID_STR_DLG_ENTER_NEW_FOLDER_NAME 23
#define ID_STR_MENU_APP                 24
#define ID_STR_APP_SET_CLOCK            25
#define ID_STR_APP_ABOUT                26
#define ID_STR_APP_QUIT                 27
#define ID_STR_APP_EXIT_TO_BASIC        28
#define ID_STR_APP_EXIT_TO_DOS          29
#define ID_STR_MENU_DEVICE              30
#define ID_STR_DEV_SD                   31
#define ID_STR_DEV_FLOPPY_1            32
#define ID_STR_DEV_FLOPPY_2            33
#define ID_STR_DEV_RAM                  34
#define ID_STR_DEV_FLASH                35
#define ID_STR_DEV_FORMAT               36
#define ID_STR_DEV_SD_CARD              37
#define ID_STR_MENU_DIRECTORY           38
#define ID_STR_DEV_MAKE_DIR             39
#define ID_STR_DEV_REFRESH_LISTING      40
#define ID_STR_DEV_ENTER_MEATLOAF_URL   41
#define ID_STR_DEV_SORT_BY_TYPE         42
#define ID_STR_DEV_SORT_BY_NAME         43
#define ID_STR_DEV_SORT_BY_SIZE         44
#define ID_STR_MENU_FILE                45
#define ID_STR_FILE_COPY_LEFT           46
#define ID_STR_FILE_COPY_RIGHT          47
#define ID_STR_FILE_DELETE              48
#define ID_STR_FILE_DUP                 49
#define ID_STR_FILE_RENAME              50
#define ID_STR_FILE_TEXT_PREVIEW        51
#define ID_STR_FILE_HEX_PREVIEW         52
#define ID_STR_FILE_LOAD                53
#define ID_STR_BANK_CLEAR               54
#define ID_STR_BANK_FILL                55
#define ID_STR_BANK_FIND                56
#define ID_STR_BANK_FIND_NEXT           57
#define ID_STR_LBL_FILENAME             58
#define ID_STR_LBL_FILETYPE             60
#define ID_STR_LBL_FILESIZE             61
#define ID_STR_LBL_BANK_NUM             62
#define ID_STR_LBL_BANK_ADDRESS         63
#define ID_STR_MSG_SHOW_DRIVE_COUNT     68
#define ID_STR_MSG_FORMATTING           69
#define ID_STR_MSG_COPYING              70
#define ID_STR_MSG_DELETE_SUCCESS        73
#define ID_STR_MSG_RENAME_SUCCESS        74
#define ID_STR_N_FILES_FOUND             77
#define ID_STR_MSG_HEX_VIEW_INSTRUCTIONS  81
#define ID_STR_MSG_TEXT_VIEW_INSTRUCTIONS 82
#define ID_STR_MSG_BASIC_LOAD_INSTRUCTIONS 83
#define ID_STR_MSG_SEARCH_BANK_SUCCESS   84
#define ID_STR_MSG_SEARCH_BANK_FAILURE   85
#define ID_STR_APP_PATH_TEXT_EDITOR       86
#define ID_STR_APP_PATH_MIDI_PLAYER      87
#define ID_STR_APP_PATH_MOD_PLAYER       88
#define ID_STR_APP_PATH_AUDIO_PLAYER     89
#define ID_STR_APP_PATH_VGM_PLAYER       90
#define ID_STR_APP_PATH_RSD_PLAYER       91
#define ID_STR_MSG_DELETE_FILE_FAILURE   110
#define ID_STR_MSG_DELETE_DIR_FAILURE    111
#define ID_STR_MSG_NO_DRIVES_AVAILABLE  113
#define ID_STR_ERROR_GENERIC_DISK       115
#define ID_STR_ERROR_FAIL_VIEW_FILE     117
#define ID_STR_ERROR_ATTEMPT_COPY_BANK_TO_ITSELF 120
#define ID_STR_ERROR_ATTEMPT_TO_OVERWRITE_FM_RAM 121
#define ID_STR_ERROR_NO_TEXT_EDITOR      122
#define ID_STR_ERROR_NO_MIDI_PLAYER      123
#define ID_STR_ERROR_NO_MOD_PLAYER       124
#define ID_STR_ERROR_NO_AUDIO_PLAYER     125
#define ID_STR_ERROR_NO_VGM_PLAYER       126
#define ID_STR_ERROR_NO_RSD_PLAYER       127
#define ID_STR_ERROR_FAIL_OPEN_DIR      119
#define ID_STR_ERROR_FAIL_OPEN_FILE     118
#define ID_STR_FILETYPE_HEADER           92
#define ID_STR_FILETYPE_DIR              93
#define ID_STR_FILETYPE_LINK             94
#define ID_STR_FILETYPE_SUBDIR           95
#define ID_STR_FILETYPE_OTHER            96
#define ID_STR_FILETYPE_BASIC            97
#define ID_STR_FILETYPE_EXE             98
#define ID_STR_FILETYPE_FONT            99
#define ID_STR_FILETYPE_MUSIC           100
#define ID_STR_FILETYPE_IMAGE           101
#define ID_STR_FILETYPE_TEXT            102
#define ID_STR_FILETYPE_MIDI            103
#define ID_STR_FILETYPE_MP3             104
#define ID_STR_FILETYPE_OGG             105
#define ID_STR_FILETYPE_WAV             106
#define ID_STR_FILETYPE_VGM             107
#define ID_STR_FILETYPE_RSD             108
#define ID_STR_DLG_QUIT_CONFIRM_BODY     0
#define ID_STR_MSG_FATAL_ERROR          109
#define ID_STR_MSG_FATAL_ERROR_BODY     112
#define ID_STR_ABOUT_FMANAGER           134
#define ID_STR_ABOUT_GIT                129
#define ID_STR_MACHINE_JR               135
#define ID_STR_MACHINE_K                136
#define ID_STR_MACHINE_UNKNOWN          137
#define ID_STR_MSG_HIT_ANY_KEY           80
#define ID_STR_MSG_SCANNING              67
#define ID_STR_MSG_READING_DIR           72
#define ID_STR_MSG_DONE                  75
#define ID_STR_ERROR_ALLOC_FAIL         128
#define NUM_STRINGS                     138

#define GetString(id)  (fm_strings[(id)])


#endif /* STRINGS_H_ */
