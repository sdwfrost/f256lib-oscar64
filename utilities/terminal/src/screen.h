/*
 * screen.h - UI layout, status bar, dialogs
 * Ported from F256-terminal CC65 version to oscar64/f256lib
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "f256lib.h"

#pragma compile("screen.c")


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

// Terminal body area (25 rows for ANSI display)
#define TERM_BODY_Y1           0
#define TERM_BODY_HEIGHT       25
#define TERM_BODY_Y2           (TERM_BODY_Y1 + TERM_BODY_HEIGHT - 1)

#define TERM_BODY_WIDTH        80
#define TERM_BODY_X1           0
#define TERM_BODY_X2           (TERM_BODY_X1 + TERM_BODY_WIDTH - 1)

// Title bar (row 25)
#define TITLE_BAR_Y            (TERM_BODY_Y2 + 1)

// Positions within the title bar
#define TERM_BAUD_X1           48
#define TERM_DATE_X1           62
#define TERM_ERROR_OE_X        38
#define TERM_ERROR_PE_X        (TERM_ERROR_OE_X + 1)
#define TERM_ERROR_FE_X        (TERM_ERROR_PE_X + 1)
#define TERM_ERROR_BI_X        (TERM_ERROR_FE_X + 1)
#define TERM_ERROR_ERR_X       (TERM_ERROR_BI_X + 1)

// Communication buffer area (4 rows below title bar)
#define COMM_BUFFER_NUM_COLS   78
#define COMM_BUFFER_NUM_ROWS   4
#define COMM_BUFFER_FIRST_COL  1
#define COMM_BUFFER_FIRST_ROW  26
#define COMM_BUFFER_LAST_COL   (COMM_BUFFER_NUM_COLS)
#define COMM_BUFFER_LAST_ROW   29
#define COMM_BUFFER_MAX_STRING_LEN  192

// Screen dimensions
#define SCREEN_NUM_COLS        80
#define SCREEN_NUM_ROWS        30


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Draw the communication area frame and title bar
void Screen_DrawUI(void);

// Draw the title bar
void Screen_DrawTitleBar(void);

// Show application about info
void Screen_ShowAppAboutInfo(void);

// Communication buffer management
void Buffer_Initialize(void);
void Buffer_Clear(void);
void Buffer_RefreshDisplay(void);
void Buffer_NewMessage(const char *message);


#endif /* SCREEN_H_ */
