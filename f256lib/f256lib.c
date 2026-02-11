/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#include "f256lib.h"


void f256Init(void) {
	// Swap I/O page 0 into bank 6.  This is our normal state.
	POKE(MMU_IO_CTRL, MMU_IO_PAGE_0);

	POKE(VKY_MSTR_CTRL_0, 63); // Enable text and all graphics.

	POKE(MMU_MEM_CTRL, 0xb3);  // MLUT editing enabled, editing 3, 3 is active.

	// Set all memory slots to be CPU memory.
	POKE(MMU_MEM_BANK_0, 0);
	POKE(MMU_MEM_BANK_1, 1);
	POKE(MMU_MEM_BANK_2, 2);
	POKE(MMU_MEM_BANK_3, 3);
	POKE(MMU_MEM_BANK_4, 4);
	POKE(MMU_MEM_BANK_5, 5);
	//POKE(MMU_MEM_BANK_6, 6);  // Don't use this - it's for the micro kernel.
	//POKE(MMU_MEM_BANK_7, 7);  // Don't use this - it's for the micro kernel.

#ifndef WITHOUT_KERNEL
	kernelReset();
#endif
#ifndef WITHOUT_GRAPHICS
	graphicsReset();
#endif
#ifndef WITHOUT_TEXT
	textReset();
#endif
#ifndef WITHOUT_BITMAP
	bitmapReset();
#endif
#ifndef WITHOUT_TILE
	tileReset();
#endif
#ifndef WITHOUT_SPRITE
	spriteReset();
#endif
#ifndef WITHOUT_FILE
	fileReset();
#endif
#ifndef WITHOUT_RANDOM
	randomReset();
#endif

	// Re-enable interrupts after initialization.
	// Oscar64's CRT startup does SEI; the kernel needs IRQs
	// for timer events (kernelPause, etc.).
	__asm volatile { cli }
}


void f256Reset(void) {
	__asm volatile { sei }
	POKE(MMU_MEM_BANK_7, 7);
	POKE(0xD6A2, 0xDE);
	POKE(0xD6A3, 0xAD);
	POKE(0xD6A0, 0xF0);
	POKE(0xD6A0, 0x00);
	__asm volatile { jmp ($FFFC) }
}


byte FAR_PEEK(uint32_t address) {
	byte block;
	byte result;

	SWAP_IO_SETUP();

	block = address / EIGHTK;
	address &= 0x1FFF;
	POKE(SWAP_SLOT, block);
	result = PEEK(SWAP_ADDR + address);
	SWAP_RESTORE_SLOT();

	SWAP_IO_SHUTDOWN();

	return result;
}


uint16_t FAR_PEEKW(uint32_t address) {
	byte     block;
	uint16_t result;

	SWAP_IO_SETUP();

	block = address / EIGHTK;
	address &= 0x1FFF;
	POKE(SWAP_SLOT, block);
	result = PEEKW(SWAP_ADDR + address);
	SWAP_RESTORE_SLOT();

	SWAP_IO_SHUTDOWN();

	return result;
}


void *FAR_POINTER(uint32_t address) {
#if SWAP_SLOT == MMU_MEM_BANK_5
	byte block;

	block = address / EIGHTK;
	address &= 0x1FFF;
	POKE(SWAP_SLOT, block);
	return (void *)address;
#else
	return 0;
#endif
}


void FAR_POKE(uint32_t address, byte value) {
	byte block;

	SWAP_IO_SETUP();

	block = address / EIGHTK;
	address &= 0x1FFF;
	POKE(SWAP_SLOT, block);
	POKE(SWAP_ADDR + address, value);
	SWAP_RESTORE_SLOT();

	SWAP_IO_SHUTDOWN();
}


void FAR_POKEW(uint32_t address, uint16_t value) {
	byte block;

	SWAP_IO_SETUP();

	block = address / EIGHTK;
	address &= 0x1FFF;
	POKE(SWAP_SLOT, block);
	POKEW(SWAP_ADDR + address, value);
	SWAP_RESTORE_SLOT();

	SWAP_IO_SHUTDOWN();
}


#ifndef WITHOUT_MAIN
// Undo the main→f256main rename for this wrapper
#ifdef main
#undef main
#endif
int f256main(int argc, char *argv[]);
int main(void) {
	f256Init();
	f256main(kernelArgs->u.common.extlen / 2, (char **)kernelArgs->u.common.ext);
	f256Reset();
	return 0;
}
#define main f256main
#endif
