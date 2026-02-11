/*
 * overlay3 - Manual MMU bank switching demo (oscar64 port)
 *
 * Original (llvm-mos): Used __attribute__((noinline, section(".block8")))
 * to place functions in far memory block 8. The PGZ loader directly wrote
 * code to physical block 8 (address 0x10000). Hand-written trampolines
 * at 0x000D (MMU slot 5) swapped block 8 into 0xA000-0xBFFF.
 *
 * Oscar64 port: Uses #pragma section/region to compile far functions for
 * address 0xA000. The PGZ loader places them in block 5 (default mapping).
 * At startup, initOverlay() copies the code to block 8. Trampolines then
 * swap block 8 into slot 5 before calling.
 *
 * This demonstrates the F256K's MMU bank switching mechanism for placing
 * and executing code in memory beyond the 64KB address space.
 */

#include "f256lib.h"
#include <string.h>

// Define a section for far code at the swap slot address (0xA000-0xBFFF).
// This is slot 5 of the MMU lookup table (MMU_MEM_BANK_5 = 0x000D).
// The PGZ loader will write this code to physical block 5 (default mapping).
#pragma section(farcode8, 0)
#pragma region(block8rgn, 0xA000, 0xC000, , , { farcode8 })

// Forward declarations of far functions (compiled for 0xA000)
void FAR8_myFunc(void);
void FAR8_myFunc2(void);

// ====================================================================
// TRAMPOLINES (in main memory)
//
// Each trampoline: saves current slot 5 mapping, maps block 8 into
// slot 5 (0xA000-0xBFFF), calls the far function, then restores.
// ====================================================================

void myFunc(void) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_myFunc();
	POKE(MMU_MEM_BANK_5, savedMMU);
}

void myFunc2(void) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_myFunc2();
	POKE(MMU_MEM_BANK_5, savedMMU);
}

// ====================================================================
// FAR FUNCTIONS (compiled for address 0xA000, will run from block 8)
// ====================================================================
#pragma code(farcode8)

void FAR8_myFunc(void) {
	printf("\nallo");
	printf("allo");
	printf("allo");
	printf("allo");
}

void FAR8_myFunc2(void) {
	printf("\nbonjour");
	printf("bonjour");
	printf("bonjour");
	printf("bonjour");
	printf("bonjour");
}

// Switch back to main code section
#pragma code(code)

// ====================================================================
// initOverlay: copy far code from block 5 (PGZ load target) to block 8
//
// The PGZ loader writes the far section to 0xA000, which maps to
// physical block 5 by default. We need the code in block 8 so we
// copy 8KB block-by-block using a small buffer in main memory.
// ====================================================================
void initOverlay(void) {
	uint8_t buffer[256];
	uint16_t offset;

	for (offset = 0; offset < EIGHTK; offset += 256) {
		// Read chunk from 0xA000 (block 5 - the PGZ load location)
		memcpy(buffer, (void *)(0xA000u + offset), 256);

		// Map block 8 to slot 5
		POKE(MMU_MEM_BANK_5, 8);

		// Write chunk to 0xA000 (now block 8)
		memcpy((void *)(0xA000u + offset), buffer, 256);

		// Restore block 5 to slot 5
		POKE(MMU_MEM_BANK_5, 5);
	}
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// Copy far code from block 5 to block 8
	initOverlay();

	printf("start\n");
	myFunc();    // Called via trampoline - executes from block 8
	myFunc2();   // Called via trampoline - executes from block 8
	getchar();
	return 0;
}
