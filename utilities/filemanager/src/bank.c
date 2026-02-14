/*
 * bank.c - Memory bank object for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "bank.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

void Bank_Init(FMBankObject* the_bank, const char* the_name, const char* the_description, bank_type the_bank_type, byte the_bank_num, byte the_row)
{
	int16_t len;

	if (the_bank == NULL) return;

	the_bank->bank_num_ = the_bank_num;
	the_bank->row_ = the_row;
	the_bank->addr_ = (uint32_t)the_bank_num * 0x2000;
	the_bank->is_kup_ = (the_bank_type == BANK_KUP_PRIMARY);
	the_bank->selected_ = false;
	the_bank->display_row_ = -1;

	// Allocate and copy name
	if (the_bank->name_ != NULL) free(the_bank->name_);
	len = generalStrnlen(the_name, 20);
	the_bank->name_ = (char*)malloc(len + 1);
	if (the_bank->name_ != NULL)
		generalStrlcpy(the_bank->name_, the_name, len + 1);

	// Allocate and copy description
	if (the_bank->description_ != NULL) free(the_bank->description_);
	len = generalStrnlen(the_description, 20);
	the_bank->description_ = (char*)malloc(len + 1);
	if (the_bank->description_ != NULL)
		generalStrlcpy(the_bank->description_, the_description, len + 1);
}


void Bank_Reset(FMBankObject* the_bank)
{
	if (the_bank == NULL) return;

	if (the_bank->name_ != NULL)
	{
		free(the_bank->name_);
		the_bank->name_ = NULL;
	}

	if (the_bank->description_ != NULL)
	{
		free(the_bank->description_);
		the_bank->description_ = NULL;
	}

	the_bank->is_kup_ = false;
	the_bank->selected_ = false;
}


void Bank_UpdatePos(FMBankObject* the_bank, byte x, int8_t display_row, uint16_t row)
{
	if (the_bank == NULL) return;
	the_bank->x_ = x;
	the_bank->display_row_ = display_row;
	the_bank->row_ = (byte)row;
}


bool Bank_IsSelected(FMBankObject* the_bank)
{
	if (the_bank == NULL) return false;
	return the_bank->selected_;
}


void Bank_Clear(FMBankObject* the_bank)
{
	Bank_Fill(the_bank, 0x00);
}


void Bank_Fill(FMBankObject* the_bank, byte the_fill_value)
{
	byte saved_bank;

	if (the_bank == NULL) return;

	// Use FAR_POKE to fill the bank's physical memory
	{
		uint32_t base_addr = the_bank->addr_;
		uint16_t i;

		// Map bank into swap slot and fill
		saved_bank = PEEK(SWAP_SLOT);
		POKE_MEMMAP(SWAP_SLOT, the_bank->bank_num_);

		for (i = 0; i < 0x2000; i++)
			POKE(SWAP_ADDR + i, the_fill_value);

		POKE_MEMMAP(SWAP_SLOT, saved_bank);
	}
}


int16_t Bank_AskForFillValue(void)
{
	char* user_input;

	{
		static char empty_str[] = "";
		user_input = Screen_GetStringFromUser(
			(char*)GetString(ID_STR_DLG_FILL_BANK_TITLE),
			(char*)GetString(ID_STR_DLG_FILL_BANK_BODY),
			empty_str,
			3
		);
	}

	if (user_input == NULL) return -1;

	// Parse as hex or decimal
	if (user_input[0] >= '0' && user_input[0] <= '9')
	{
		int val = 0;
		// Simple hex/decimal parse
		if (user_input[0] == '$' || user_input[0] == '0' && user_input[1] == 'x')
		{
			// hex
		}
		else
		{
			// decimal
			val = atoi(user_input);
		}
		return (int16_t)(val & 0xFF);
	}
	else
	{
		// Treat first char as the fill value
		return (int16_t)(byte)user_input[0];
	}
}


bool Bank_MarkSelected(FMBankObject* the_bank, int8_t y_offset, bool as_active)
{
	if (the_bank == NULL) return false;

	if (the_bank->selected_ == false)
	{
		the_bank->selected_ = true;
		Bank_Render(the_bank, true, y_offset, as_active);
	}
	return true;
}


bool Bank_MarkUnSelected(FMBankObject* the_bank, int8_t y_offset)
{
	if (the_bank == NULL) return false;

	if (the_bank->selected_ == true)
	{
		the_bank->selected_ = false;
		Bank_Render(the_bank, false, y_offset, true);
	}
	return true;
}


void Bank_Render(FMBankObject* the_bank, bool as_selected, int8_t y_offset, bool as_active)
{
	byte	x1, x2, the_color;
	int8_t	y;

	if (the_bank == NULL) return;
	if (the_bank->display_row_ == -1) return;

	the_color = as_active ? LIST_ACTIVE_COLOR : LIST_INACTIVE_COLOR;

	x1 = the_bank->x_;
	x2 = the_bank->x_ + (UI_PANEL_INNER_WIDTH - 1);
	y = the_bank->display_row_ + y_offset;

	// Clear row
	textFillBox(x1, y, x2, y, ' ', the_color, APP_BACKGROUND_COLOR);

	// Draw name
	if (the_bank->name_ != NULL)
		textDrawStringAt(x1, y, the_bank->name_, the_color, APP_BACKGROUND_COLOR);

	// Draw bank number
	sprintf(global_string_buff1, "%3u", the_bank->bank_num_);
	textDrawStringAt(x1 + UI_PANEL_BANK_NUM_OFFSET, y, global_string_buff1, the_color, APP_BACKGROUND_COLOR);

	// Draw address
	sprintf(global_string_buff1, "%05lX", the_bank->addr_);
	textDrawStringAt(x1 + UI_PANEL_BANK_NUM_OFFSET + UI_PANEL_BANK_ADDR_OFFSET, y, global_string_buff1, the_color, APP_BACKGROUND_COLOR);

	if (as_selected)
		textInvertBox(x1, y, x2, y);
}
