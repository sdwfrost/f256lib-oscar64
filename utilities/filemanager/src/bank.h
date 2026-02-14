/*
 * bank.h - Memory bank object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef BANK_H_
#define BANK_H_

#include "app.h"

#pragma compile("bank.c")


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PAGES_PER_BANK	32


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

typedef enum bank_type
{
	BANK_KUP_PRIMARY	= 0,
	BANK_KUP_SECONDARY	= 1,
	BANK_NON_KUP		= 2,
	BANK_MAX_TYPE		= 3,
} bank_type;


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct FMBankObject
{
	bool		is_kup_;
	byte		bank_num_;
	bool		selected_;
	byte		x_;
	int8_t		display_row_;
	byte		row_;
	uint32_t	addr_;
	char*		name_;
	char*		description_;
} FMBankObject;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

void Bank_Init(FMBankObject* the_bank, const char* the_name, const char* the_description, bank_type the_bank_type, byte the_bank_num, byte the_row);
void Bank_Reset(FMBankObject* the_bank);

void Bank_UpdatePos(FMBankObject* the_bank, byte x, int8_t display_row, uint16_t row);
bool Bank_IsSelected(FMBankObject* the_bank);

void Bank_Clear(FMBankObject* the_bank);
void Bank_Fill(FMBankObject* the_bank, byte the_fill_value);
int16_t Bank_AskForFillValue(void);

bool Bank_MarkSelected(FMBankObject* the_bank, int8_t y_offset, bool as_active);
bool Bank_MarkUnSelected(FMBankObject* the_bank, int8_t y_offset);
void Bank_Render(FMBankObject* the_bank, bool as_selected, int8_t y_offset, bool as_active);


#endif /* BANK_H_ */
