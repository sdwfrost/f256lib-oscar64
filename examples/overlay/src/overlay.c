/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */


/*
 * F256K Code Overlay Example - oscar64 port
 *
 * Demonstrates how to place code in far memory (beyond the CPU's 64KB
 * address space) and execute it by switching MMU bank mappings.
 *
 * Memory layout:
 *   0x0200-0x9FFF  Main code, data, stack (always visible)
 *   0xA000-0xBFFF  Bank 5 window - overlay execution area
 *   0xC000-0xDFFF  I/O registers
 *   0xE000-0xFFFF  Kernel
 *
 * Overlay storage (in PGZ, loaded to physical RAM):
 *   0x10000-0x11FFF  Physical block 8 - Overlay 1 code
 *   0x12000-0x13FFF  Physical block 9 - Overlay 2 code
 *
 * At runtime, we swap MMU bank 5 to point to the desired physical
 * block, making that overlay's code visible at 0xA000.  Trampolines
 * in main memory handle the bank switching transparently.
 *
 * The original llvm-mos version used SEGMENT_* macros and an overlay
 * tool that generated trampolines automatically.  Here we do it by
 * hand using oscar64's #pragma section/region with a runtime address.
 */


#include "f256lib.h"


// ---------------------------------------------------------------------------
// Overlay memory configuration
// ---------------------------------------------------------------------------

// MMU register that controls which physical block maps to 0xA000-0xBFFF
#define OVERLAY_MMU_REG  MMU_MEM_BANK_5

// Physical block numbers (block = physical_address / 0x2000)
#define OVL1_BLOCK  8   // Physical 0x10000
#define OVL2_BLOCK  9   // Physical 0x12000

// Overlay 1: stored at physical 0x10000, compiled to run at 0xA000
#pragma section( ovl1_code, 0 )
#pragma region( ovl1, 0x10000, 0x12000, , 1, { ovl1_code }, 0xA000 )

// Overlay 2: stored at physical 0x12000, compiled to run at 0xA000
#pragma section( ovl2_code, 0 )
#pragma region( ovl2, 0x12000, 0x14000, , 2, { ovl2_code }, 0xA000 )


// ---------------------------------------------------------------------------
// Forward declarations of trampolines (in main code, always visible)
// ---------------------------------------------------------------------------

void firstSegment(int arg1, int arg2);
void secondSegment(int arg1, int arg2);
void moreFirstSegment(int arg1, int arg2);


// ---------------------------------------------------------------------------
// Overlay 1 functions (physical block 8, runs at 0xA000)
// ---------------------------------------------------------------------------

#pragma code( ovl1_code )

void FAR_firstSegment(int arg1, int arg2) {
	textPrint("firstSegment = ");
	textPrintInt((int32_t)(arg1 + arg2));
	textPrint("\n");
	// Call into overlay 2 via trampoline (in main memory)
	secondSegment(arg1, arg2);
}

void FAR_moreFirstSegment(int arg1, int arg2) {
	textPrint("moreFirstSegment = ");
	textPrintInt((int32_t)(arg1 + arg2));
	textPrint("\n");
}


// ---------------------------------------------------------------------------
// Overlay 2 functions (physical block 9, runs at 0xA000)
// ---------------------------------------------------------------------------

#pragma code( ovl2_code )

void FAR_secondSegment(int arg1, int arg2) {
	textPrint("secondSegment = ");
	textPrintInt((int32_t)(arg1 + arg2));
	textPrint("\n");
	// Call back into overlay 1 via trampoline (in main memory)
	moreFirstSegment(arg1, arg2);
}


// ---------------------------------------------------------------------------
// Main code and trampolines (always in main memory)
// ---------------------------------------------------------------------------

#pragma code( code )

// Trampolines save the current bank 5 mapping, switch to the target
// overlay's physical block, call the FAR function, then restore the
// original mapping.  This supports nested cross-overlay calls.

void firstSegment(int arg1, int arg2) {
	volatile byte saved = PEEK(OVERLAY_MMU_REG);
	POKE(OVERLAY_MMU_REG, OVL1_BLOCK);
	FAR_firstSegment(arg1, arg2);
	POKE(OVERLAY_MMU_REG, saved);
}

void secondSegment(int arg1, int arg2) {
	volatile byte saved = PEEK(OVERLAY_MMU_REG);
	POKE(OVERLAY_MMU_REG, OVL2_BLOCK);
	FAR_secondSegment(arg1, arg2);
	POKE(OVERLAY_MMU_REG, saved);
}

void moreFirstSegment(int arg1, int arg2) {
	volatile byte saved = PEEK(OVERLAY_MMU_REG);
	POKE(OVERLAY_MMU_REG, OVL1_BLOCK);
	FAR_moreFirstSegment(arg1, arg2);
	POKE(OVERLAY_MMU_REG, saved);
}


int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	textClear();
	textPrint("F256K Code Overlay Example\n");
	textPrint("==========================\n\n");

	// The call chain is:
	//   main -> firstSegment (ovl1) -> secondSegment (ovl2)
	//        -> moreFirstSegment (ovl1)
	// Each cross-overlay call goes through a trampoline that
	// switches MMU bank 5 to the correct physical block.
	firstSegment(1, 2);

	textPrint("\nDone! Spinning forever.\n");

	// Spin.
	for (;;);

	return 0;
}
