/*
 * strings.h - String constants for the terminal emulator
 * Ported from F256-terminal CC65 version
 *
 * In the CC65 version these were stored in extended memory and retrieved
 * via General_GetString(). In the oscar64 port we use simple C string arrays.
 */

#ifndef STRINGS_H_
#define STRINGS_H_


// String table - direct C arrays instead of EM-stored strings
static const char * const app_strings[] = {
	"Quit f/term?",                           // 0  ID_STR_DLG_QUIT_CONFIRM
	"Are you sure?",                          // 1  ID_STR_DLG_ARE_YOU_SURE
	"Set Clock",                              // 2  ID_STR_DLG_SET_CLOCK_TITLE
	"YY-MM-DD HH:MM",                        // 3  ID_STR_DLG_SET_CLOCK_BODY
	"Set Baud Rate",                          // 4  ID_STR_DLG_BAUD_TITLE
	"Select baud rate",                       // 5  ID_STR_DLG_BAUD_BODY
	"Dial BBS",                               // 6  ID_STR_DLG_DIAL_BBS_TITLE
	"Enter BBS address",                      // 7  ID_STR_DLG_DIAL_BBS_BODY
	"Hang Up",                                // 8  ID_STR_DLG_HANGUP_TITLE
	"Yes",                                    // 9  ID_STR_DLG_YES
	"No",                                     // 10 ID_STR_DLG_NO
	"OK",                                     // 11 ID_STR_DLG_OK
	"Cancel",                                 // 12 ID_STR_DLG_CANCEL
	"Menu",                                   // 13 ID_STR_MENU_APP
	"Set Baud",                               // 14 ID_STR_APP_SET_BAUD
	"Dial Foenix BBS",                        // 15 ID_STR_APP_DIAL_FOENIX_BBS
	"Dial BBS",                               // 16 ID_STR_APP_DIAL_BBS
	"About",                                  // 17 ID_STR_APP_ABOUT
	"Quit",                                   // 18 ID_STR_APP_QUIT
	"Init failure",                           // 19 ID_STR_ERROR_INIT_FAILURE
	"Serial overflow",                        // 20 ID_STR_ERROR_SERIAL_OVERFLOW
	"Unknown ANSI sequence",                  // 21 ID_STR_ERROR_UNKNOWN_ANSI_SEQUENCE
	"Error",                                  // 22 ID_STR_ERROR_GENERIC
	"Fatal error: %d",                        // 23 ID_STR_MSG_FATAL_ERROR
	"Press any key to restart.",              // 24 ID_STR_MSG_FATAL_ERROR_BODY
	"Disk error",                             // 25 ID_STR_ERROR_GENERIC_DISK
	"Memory allocation failed",               // 26 ID_STR_ERROR_ALLOC_FAIL
	"Press any key...",                        // 27 ID_STR_MSG_HIT_ANY_KEY
	"%u bytes free",                          // 28 ID_STR_N_BYTES_FREE
	"Serial buffer dumped.",                  // 29 ID_STR_MSG_DEBUG_DUMP
	"Baud set to 300",                        // 30 ID_STR_MSG_SET_BAUD_300
	"Baud set to 1200",                       // 31 ID_STR_MSG_SET_BAUD_1200
	"Baud set to 2400",                       // 32 ID_STR_MSG_SET_BAUD_2400
	"Baud set to 3600",                       // 33 ID_STR_MSG_SET_BAUD_3600
	"Baud set to 4800",                       // 34 ID_STR_MSG_SET_BAUD_4800
	"Baud set to 9600",                       // 35 ID_STR_MSG_SET_BAUD_9600
	"Baud set to 19200",                      // 36 ID_STR_MSG_SET_BAUD_19200
	"Baud set to 38400",                      // 37 ID_STR_MSG_SET_BAUD_38400
	"Baud set to 57600",                      // 38 ID_STR_MSG_SET_BAUD_57600
	"Baud set to 115200",                     // 39 ID_STR_MSG_SET_BAUD_115200
	"300",                                    // 40 ID_STR_BAUD_300
	"600",                                    // 41 ID_STR_BAUD_600
	"1200",                                   // 42 ID_STR_BAUD_1200
	"2400",                                   // 43 ID_STR_BAUD_2400
	"3600",                                   // 44 ID_STR_BAUD_3600
	"4800",                                   // 45 ID_STR_BAUD_4800
	"9600",                                   // 46 ID_STR_BAUD_9600
	"19200",                                  // 47 ID_STR_BAUD_19200
	"38400",                                  // 48 ID_STR_BAUD_38400
	"57600",                                  // 49 ID_STR_BAUD_57600
	"115200",                                 // 50 ID_STR_BAUD_115200
	"Font: ANSI",                             // 51 ID_STR_MSG_SELECT_FONT_ANSI
	"Font: IBM",                              // 52 ID_STR_MSG_SELECT_FONT_IBM
	"Font: Foenix",                           // 53 ID_STR_MSG_SELECT_FONT_FOENIX
	"github.com/WartyMN/F256-terminal",       // 54 ID_STR_ABOUT_GIT
	"f/term - F256 ANSI Terminal",            // 55 ID_STR_ABOUT_FTERM
	"F256Jr",                                 // 56 ID_STR_MACHINE_JR
	"F256K",                                  // 57 ID_STR_MACHINE_K
	"Unknown",                                // 58 ID_STR_MACHINE_UNKNOWN
};

// String ID definitions (matching CC65 strings.h)
#define ID_STR_DLG_QUIT_CONFIRM           0
#define ID_STR_DLG_ARE_YOU_SURE           1
#define ID_STR_DLG_SET_CLOCK_TITLE        2
#define ID_STR_DLG_SET_CLOCK_BODY         3
#define ID_STR_DLG_BAUD_TITLE             4
#define ID_STR_DLG_BAUD_BODY              5
#define ID_STR_DLG_DIAL_BBS_TITLE         6
#define ID_STR_DLG_DIAL_BBS_BODY          7
#define ID_STR_DLG_HANGUP_TITLE           8
#define ID_STR_DLG_YES                    9
#define ID_STR_DLG_NO                    10
#define ID_STR_DLG_OK                    11
#define ID_STR_DLG_CANCEL                12
#define ID_STR_MENU_APP                  13
#define ID_STR_APP_SET_BAUD              14
#define ID_STR_APP_DIAL_FOENIX_BBS       15
#define ID_STR_APP_DIAL_BBS              16
#define ID_STR_APP_ABOUT                 17
#define ID_STR_APP_QUIT                  18
#define ID_STR_ERROR_INIT_FAILURE        19
#define ID_STR_ERROR_SERIAL_OVERFLOW     20
#define ID_STR_ERROR_UNKNOWN_ANSI_SEQ    21
#define ID_STR_ERROR_GENERIC             22
#define ID_STR_MSG_FATAL_ERROR           23
#define ID_STR_MSG_FATAL_ERROR_BODY      24
#define ID_STR_ERROR_GENERIC_DISK        25
#define ID_STR_ERROR_ALLOC_FAIL          26
#define ID_STR_MSG_HIT_ANY_KEY           27
#define ID_STR_N_BYTES_FREE              28
#define ID_STR_MSG_DEBUG_DUMP            29
#define ID_STR_MSG_SET_BAUD_300          30
#define ID_STR_MSG_SET_BAUD_1200         31
#define ID_STR_MSG_SET_BAUD_2400         32
#define ID_STR_MSG_SET_BAUD_3600         33
#define ID_STR_MSG_SET_BAUD_4800         34
#define ID_STR_MSG_SET_BAUD_9600         35
#define ID_STR_MSG_SET_BAUD_19200        36
#define ID_STR_MSG_SET_BAUD_38400        37
#define ID_STR_MSG_SET_BAUD_57600        38
#define ID_STR_MSG_SET_BAUD_115200       39
#define ID_STR_BAUD_300                  40
#define ID_STR_BAUD_600                  41
#define ID_STR_BAUD_1200                 42
#define ID_STR_BAUD_2400                 43
#define ID_STR_BAUD_3600                 44
#define ID_STR_BAUD_4800                 45
#define ID_STR_BAUD_9600                 46
#define ID_STR_BAUD_19200               47
#define ID_STR_BAUD_38400               48
#define ID_STR_BAUD_57600               49
#define ID_STR_BAUD_115200              50
#define ID_STR_MSG_SELECT_FONT_ANSI      51
#define ID_STR_MSG_SELECT_FONT_IBM       52
#define ID_STR_MSG_SELECT_FONT_FOENIX    53
#define ID_STR_ABOUT_GIT                 54
#define ID_STR_ABOUT_FTERM               55
#define ID_STR_MACHINE_JR                56
#define ID_STR_MACHINE_K                 57
#define ID_STR_MACHINE_UNKNOWN           58
#define NUM_STRINGS                      59

// Helper macro to get a string by ID
#define GetString(id)  (app_strings[(id)])


#endif /* STRINGS_H_ */
