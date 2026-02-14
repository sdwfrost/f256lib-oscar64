/*
 * memsys.c - Memory system manager for the file manager
 * Ported from F256-FileManager CC65 version
 */

#include "memsys.h"
#include "screen.h"
#include "strings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

FMMemorySystem* MemSys_NewOrReset(FMMemorySystem* existing_memsys, bool is_flash)
{
	FMMemorySystem* the_memsys;

	if (existing_memsys != NULL)
	{
		the_memsys = existing_memsys;
		MemSys_ResetAllBanks(the_memsys);
	}
	else
	{
		the_memsys = (FMMemorySystem*)malloc(sizeof(FMMemorySystem));
		if (the_memsys == NULL)
		{
			Buffer_NewMessage(GetString(ID_STR_ERROR_ALLOC_FAIL));
			return NULL;
		}
		memset(the_memsys, 0, sizeof(FMMemorySystem));
	}

	the_memsys->is_flash_ = is_flash;
	the_memsys->cur_row_ = -1;

	// Populate banks
	MemSys_PopulateBanks(the_memsys);

	return the_memsys;
}


void MemSys_Destroy(FMMemorySystem** the_memsys)
{
	if (*the_memsys == NULL) return;

	MemSys_ResetAllBanks(*the_memsys);
	free(*the_memsys);
	*the_memsys = NULL;
}


void MemSys_ResetAllBanks(FMMemorySystem* the_memsys)
{
	byte i;

	if (the_memsys == NULL) return;

	for (i = 0; i < MEMORY_BANK_COUNT; i++)
		Bank_Reset(&the_memsys->bank_[i]);
}


void MemSys_SetCurrentRow(FMMemorySystem* the_memsys, int16_t the_row_number)
{
	if (the_memsys == NULL) return;
	the_memsys->cur_row_ = the_row_number;
}


int16_t MemSys_GetCurrentRow(FMMemorySystem* the_memsys)
{
	if (the_memsys == NULL) return -1;
	return the_memsys->cur_row_;
}


byte MemSys_GetCurrentBankNum(FMMemorySystem* the_memsys)
{
	if (the_memsys == NULL) return 255;
	if (the_memsys->cur_row_ < 0) return 255;
	return the_memsys->bank_[the_memsys->cur_row_].bank_num_;
}


bool MemSys_GetCurrentRowKUPState(FMMemorySystem* the_memsys)
{
	if (the_memsys == NULL) return false;
	if (the_memsys->cur_row_ < 0) return false;
	return the_memsys->bank_[the_memsys->cur_row_].is_kup_;
}


FMBankObject* MemSys_FindBankByRow(FMMemorySystem* the_memsys, byte the_row)
{
	if (the_memsys == NULL) return NULL;
	if (the_row >= MEMORY_BANK_COUNT) return NULL;
	return &the_memsys->bank_[the_row];
}


void MemSys_PopulateBanks(FMMemorySystem* the_memsys)
{
	byte	i;
	byte	base_bank;
	char	name_buf[16];
	char	desc_buf[16];

	if (the_memsys == NULL) return;

	// RAM banks start at 0, Flash banks at 64
	base_bank = the_memsys->is_flash_ ? 64 : 0;

	for (i = 0; i < MEMORY_BANK_COUNT; i++)
	{
		byte bank_num = base_bank + i;
		bank_type btype = BANK_NON_KUP;

		// Check if this bank contains a KUP program
		// KUP signature: first 4 bytes at the bank's start are "F256"
		{
			byte saved = PEEK(SWAP_SLOT);
			POKE_MEMMAP(SWAP_SLOT, bank_num);

			if (PEEK(SWAP_ADDR) == 'F' &&
				PEEK(SWAP_ADDR + 1) == '2' &&
				PEEK(SWAP_ADDR + 2) == '5' &&
				PEEK(SWAP_ADDR + 3) == '6')
			{
				btype = BANK_KUP_PRIMARY;
				// Extract KUP name from header (bytes 8-23)
				{
					byte j;
					for (j = 0; j < 15 && PEEK(SWAP_ADDR + 8 + j) != 0; j++)
						name_buf[j] = PEEK(SWAP_ADDR + 8 + j);
					name_buf[j] = 0;
				}
			}
			else
			{
				sprintf(name_buf, "Bank %02X", bank_num);
			}

			POKE_MEMMAP(SWAP_SLOT, saved);
		}

		if (the_memsys->is_flash_)
			sprintf(desc_buf, "Flash");
		else
			sprintf(desc_buf, "RAM");

		Bank_Init(&the_memsys->bank_[i], name_buf, desc_buf, btype, bank_num, i);
	}

	// Select first bank by default
	if (MEMORY_BANK_COUNT > 0)
		the_memsys->cur_row_ = 0;
}


FMBankObject* MemSys_SetBankSelectionByRow(FMMemorySystem* the_memsys, uint16_t the_row, bool do_selection, byte y_offset, bool as_active)
{
	FMBankObject*  the_bank;
	FMBankObject*  prev_bank;

	if (the_memsys == NULL) return NULL;
	if (the_row >= MEMORY_BANK_COUNT) return NULL;

	the_bank = &the_memsys->bank_[the_row];

	// Deselect previously selected bank if different
	if (the_memsys->cur_row_ >= 0 && the_memsys->cur_row_ != (int16_t)the_row)
	{
		prev_bank = &the_memsys->bank_[the_memsys->cur_row_];
		Bank_MarkUnSelected(prev_bank, y_offset);
	}

	if (do_selection)
	{
		Bank_MarkSelected(the_bank, y_offset, as_active);
		the_memsys->cur_row_ = (int16_t)the_row;
	}
	else
	{
		Bank_MarkUnSelected(the_bank, y_offset);
	}

	return the_bank;
}


bool MemSys_ExecuteCurrentRow(FMMemorySystem* the_memsys)
{
	byte bank_num;

	if (the_memsys == NULL) return false;
	if (the_memsys->cur_row_ < 0) return false;
	if (!the_memsys->bank_[the_memsys->cur_row_].is_kup_) return false;

	bank_num = the_memsys->bank_[the_memsys->cur_row_].bank_num_;

	// Execute the KUP program via kernel
	// Map the bank and jump to its entry point
	// This is hardware-specific - use kernel call
	kernelCall(RunNamed);  // This may need adjustment for actual KUP execution

	return true;
}


bool MemSys_BankIsWriteable(FMMemorySystem* the_memsys)
{
	if (the_memsys == NULL) return false;
	if (the_memsys->is_flash_) return false;  // Flash is read-only
	if (the_memsys->cur_row_ < 0) return false;

	// Banks 0-7 are used by the system, don't allow writing
	if (the_memsys->bank_[the_memsys->cur_row_].bank_num_ < 8) return false;

	return true;
}


bool MemSys_FillCurrentBank(FMMemorySystem* the_memsys)
{
	int16_t fill_value;

	if (!MemSys_BankIsWriteable(the_memsys)) return false;

	fill_value = Bank_AskForFillValue();
	if (fill_value < 0) return false;

	Bank_Fill(&the_memsys->bank_[the_memsys->cur_row_], (byte)fill_value);
	return true;
}


bool MemSys_ClearCurrentBank(FMMemorySystem* the_memsys)
{
	if (!MemSys_BankIsWriteable(the_memsys)) return false;

	Bank_Clear(&the_memsys->bank_[the_memsys->cur_row_]);
	return true;
}
