/*
 *  Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *  (Original overlay concept and llvm-mos version)
 *
 *  Oscar64 port: Demonstrates F256K MMU bank switching with cross-segment
 *  function calls, ported from the llvm-mos overlay tool approach.
 *
 *  Original (llvm-mos): The overlay tool automatically generated trampolines
 *  for functions in SEGMENT_FIRST (block 8) and SEGMENT_SECOND (block 9).
 *  Cross-segment calls (block 8 -> block 9 -> block 8) worked because each
 *  trampoline saves/restores the MMU state on the stack.
 *
 *  Oscar64 port: All far functions are compiled for address 0xA000 and
 *  placed into a single far block (block 8). Cross-segment call trampolines
 *  are written manually. The chain firstSegment -> secondSegment ->
 *  moreFirstSegment works because each trampoline saves/restores MMU state.
 *
 *  NOTE: The original used two separate far blocks (8 and 9) to demonstrate
 *  that different overlay segments can call each other. In this oscar64 port
 *  all far functions share block 8, but the trampoline mechanism is identical.
 */

#include "f256lib.h"
#include <string.h>

// Define far code section at the swap slot address (0xA000-0xBFFF = slot 5)
#pragma section(farcode, 0)
#pragma region(farblk, 0xA000, 0xC000, , , { farcode })

// Trampoline prototypes (in main memory)
void firstSegment(int arg1, int arg2);
void secondSegment(int arg1, int arg2);
void moreFirstSegment(int arg1, int arg2);

// Far function prototypes (compiled for 0xA000, run from block 8)
void FAR8_firstSegment(int arg1, int arg2);
void FAR8_secondSegment(int arg1, int arg2);
void FAR8_moreFirstSegment(int arg1, int arg2);

// ====================================================================
// TRAMPOLINES (in main memory)
//
// Each saves MMU slot 5 state, maps block 8, calls far function, restores.
// Cross-segment calls work because the far function calls BACK to a
// trampoline (in main memory), which does its own save/swap/restore.
// ====================================================================

void firstSegment(int arg1, int arg2) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_firstSegment(arg1, arg2);
	POKE(MMU_MEM_BANK_5, savedMMU);
}

void secondSegment(int arg1, int arg2) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_secondSegment(arg1, arg2);
	POKE(MMU_MEM_BANK_5, savedMMU);
}

void moreFirstSegment(int arg1, int arg2) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_moreFirstSegment(arg1, arg2);
	POKE(MMU_MEM_BANK_5, savedMMU);
}

// ====================================================================
// FAR FUNCTIONS (compiled for address 0xA000, will run from block 8)
//
// Note: Cross-segment calls go through the trampolines above (which
// are in main memory and always visible). The call chain is:
//   main -> firstSegment trampoline -> FAR8_firstSegment (block 8)
//     -> secondSegment trampoline (main mem) -> FAR8_secondSegment (block 8)
//       -> moreFirstSegment trampoline (main mem) -> FAR8_moreFirstSegment
// ====================================================================
#pragma code(farcode)

void FAR8_firstSegment(int arg1, int arg2) {
	printf("firstSegment = %d\n", arg1 + arg2);
	secondSegment(arg1, arg2);  // calls trampoline in main memory
}

void FAR8_secondSegment(int arg1, int arg2) {
	printf("secondSegment = %d\n", arg1 + arg2);
	moreFirstSegment(arg1, arg2);  // calls trampoline in main memory
}

void FAR8_moreFirstSegment(int arg1, int arg2) {
	printf("moreFirstSegment = %d\n", arg1 + arg2);
}

// Switch back to main code section
#pragma code(code)

// Copy far code from block 5 (PGZ load location) to block 8
void initOverlay(void) {
	uint8_t buffer[256];
	uint16_t offset;

	for (offset = 0; offset < EIGHTK; offset += 256) {
		memcpy(buffer, (void *)(0xA000u + offset), 256);
		POKE(MMU_MEM_BANK_5, 8);
		memcpy((void *)(0xA000u + offset), buffer, 256);
		POKE(MMU_MEM_BANK_5, 5);
	}
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	initOverlay();

	// The overlay tool (llvm-mos) generated trampoline macros automatically
	// so no code changes were required. In oscar64, we write them manually.
	firstSegment(1, 2);

	// Spin.
	for (;;);

	return 0;
}
