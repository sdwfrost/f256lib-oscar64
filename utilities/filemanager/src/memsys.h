/*
 * memsys.h - Memory system manager for the file manager
 * Ported from F256-FileManager CC65 version
 */

#ifndef MEMORY_SYSTEM_H_
#define MEMORY_SYSTEM_H_

#include "app.h"
#include "bank.h"

#pragma compile("memsys.c")


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define MEMORY_BANK_COUNT	64


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct FMMemorySystem
{
	bool			is_flash_;
	FMBankObject	bank_[MEMORY_BANK_COUNT];
	int16_t			cur_row_;
} FMMemorySystem;


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

FMMemorySystem* MemSys_NewOrReset(FMMemorySystem* existing_memsys, bool is_flash);
void MemSys_Destroy(FMMemorySystem** the_memsys);
void MemSys_ResetAllBanks(FMMemorySystem* the_memsys);

void MemSys_SetCurrentRow(FMMemorySystem* the_memsys, int16_t the_row_number);
int16_t MemSys_GetCurrentRow(FMMemorySystem* the_memsys);
byte MemSys_GetCurrentBankNum(FMMemorySystem* the_memsys);
bool MemSys_GetCurrentRowKUPState(FMMemorySystem* the_memsys);
FMBankObject* MemSys_FindBankByRow(FMMemorySystem* the_memsys, byte the_row);

void MemSys_PopulateBanks(FMMemorySystem* the_memsys);
FMBankObject* MemSys_SetBankSelectionByRow(FMMemorySystem* the_memsys, uint16_t the_row, bool do_selection, byte y_offset, bool as_active);
bool MemSys_ExecuteCurrentRow(FMMemorySystem* the_memsys);
bool MemSys_BankIsWriteable(FMMemorySystem* the_memsys);

bool MemSys_FillCurrentBank(FMMemorySystem* the_memsys);
bool MemSys_ClearCurrentBank(FMMemorySystem* the_memsys);


#endif /* MEMORY_SYSTEM_H_ */
