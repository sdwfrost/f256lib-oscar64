/*
 * extra.c - Far memory function for overlay2 demo
 *
 * Original (llvm-mos): Used #define SEGMENT_FIRST to place doSomething()
 * in far memory block 8. The overlay tool auto-generated the trampoline.
 *
 * Oscar64 port: The far function FAR8_doSomething() is compiled for
 * address 0xA000 using #pragma code. The trampoline doSomething() swaps
 * block 8 into slot 5 before calling it.
 */

#include "f256lib.h"
#include "extra.h"
#include <string.h>

// Define far code section at the swap slot address (slot 5 = 0xA000-0xBFFF)
#pragma section(farcode, 0)
#pragma region(farblk, 0xA000, 0xC000, , , { farcode })

// Far function prototype
void FAR8_doSomething(void);

// ====================================================================
// TRAMPOLINE (in main memory)
// ====================================================================
void doSomething(void) {
	volatile uint8_t savedMMU = PEEK(MMU_MEM_BANK_5);
	POKE(MMU_MEM_BANK_5, 8);
	FAR8_doSomething();
	POKE(MMU_MEM_BANK_5, savedMMU);
}

// ====================================================================
// FAR FUNCTION (compiled for 0xA000, executes from block 8)
// ====================================================================
#pragma code(farcode)

void FAR8_doSomething(void) {
	printf("******************************\n");
	printf("*                            *\n");
	printf("*     Welcome to PGZ Box     *\n");
	printf("*                            *\n");
	printf("*  This is a test message.   *\n");
	printf("*  It simulates a text box.  *\n");
	printf("*                            *\n");
	printf("*  You can change the text   *\n");
	printf("*  or make it interactive.   *\n");
	printf("*                            *\n");
	printf("*  Segment loading complete. *\n");
	printf("*  Memory checks passed.     *\n");
	printf("*                            *\n");
	printf("*  Starting execution at     *\n");
	printf("*  address 0x300   .         *\n");
	printf("*                            *\n");
	printf("*  Execution finished.       *\n");
	printf("*  Press any key to exit.    *\n");
	printf("*                            *\n");
	printf("******************************\n");
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
