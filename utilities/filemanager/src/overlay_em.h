/*
 * overlay_em.h - Extended memory display (hex/text viewer)
 * Ported from F256-FileManager CC65 version
 */

#ifndef OVERLAY_EM_H_
#define OVERLAY_EM_H_

#include "app.h"

#include <stdint.h>

// overlay_em.c compiled via overlay in filemanager.c


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define NUM_MEMORY_BANKS		0x80

#define PARAM_START_FROM_THIS_BANK		true
#define PARAM_START_AFTER_LAST_HIT		false


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Display content as text with wrapping
void EM_DisplayAsText(byte em_bank_num, byte num_pages, char* the_name);

// Display content as hex dump with ASCII sidebar
void EM_DisplayAsHex(byte em_bank_num, byte num_pages, char* the_name);

// Search memory for a byte sequence
// Returns true if found, false if not
bool EM_SearchMemory(byte start_bank, byte start_page, byte start_byte, bool new_search);


#endif /* OVERLAY_EM_H_ */
