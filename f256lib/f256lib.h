/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef F256LIB_H
#define F256LIB_H


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


// ============================================================
// Types
// ============================================================

typedef unsigned char byte;


typedef struct colorS {
	byte r;
	byte g;
	byte b;
} colorT;


// ============================================================
// Constants
// ============================================================

#define EIGHTK       0x2000

#define TEXT_MATRIX  0xc000  // I/O Page 2

#define RAST_ROW_L   0xd01a
#define RAST_ROW_H   0xd01b

#define VIRQ  0xfffe

#define JOY_UP         1
#define JOY_DOWN       2
#define JOY_LEFT       4
#define JOY_RIGHT      8
#define JOY_BUTTON_1  16
#define JOY_BUTTON_2  32
#define JOY_BUTTON_3  64


// ============================================================
// Memory access macros (PEEK/POKE)
// ============================================================

// Single-byte
#define PEEK(addy)         ((byte)*(volatile byte *)(addy))
#define POKE(addy, value)  (*(volatile byte *)(addy) = (value))

// MMU / bank-register write — acts as a memory fence.
// Routes through a volatile __memmap struct member (same pattern as C128 mmu.h)
// so the compiler will not reorder any memory access across this store.
struct __memmap_byte { volatile __memmap byte v; };
#define POKE_MEMMAP(addy, value)  (((struct __memmap_byte *)(addy))->v = (value))

// Word (two bytes)
#define PEEKW(addy)        ((uint16_t)*(volatile uint16_t *)(addy))
#define POKEW(addy, value) (*(volatile uint16_t *)(addy) = (value))

// Address (three bytes) - wrapped in do/while for safe use as a statement
#define POKEA(addy, value) do { \
	POKE((addy), (value) & 0xFF); \
	POKE((addy) + 1, ((value) >> 8) & 0xFF); \
	POKE((addy) + 2, ((value) >> 16) & 0xFF); \
} while(0)

// Double-word (four bytes)
#define PEEKD(addy)        ((uint32_t)*(volatile uint32_t *)(addy))
#define POKED(addy,value)  (*(volatile uint32_t *)(addy) = (value))


// ============================================================
// Bit manipulation macros
// ============================================================

#define LOW_BYTE(x)         ((byte)(x))
#define HIGH_BYTE(x)        ((byte)(((uint16_t)(x)) >> 8))
#define SWAP_NIBBLES(x)     ((x & 0x0F) << 4 | (x & 0xF0) >> 4)
#define SWAP_UINT16(x)      (((x) >> 8) | ((x) << 8))
#define CHECK_BIT(x, pos)   (x & (1UL << pos))
#define TOGGLE_BIT(x, pos)  (x ^= (1U << pos))
#define CLEAR_BIT(x, pos)   (x &= (~(1U << pos)))
#define SET_BIT(x, pos)     (x |= (1U << pos))


// ============================================================
// Feature flag dependencies
// ============================================================

#if (defined WITHOUT_BITMAP && defined WITHOUT_TILE && defined WITHOUT_SPRITE)
#define WITHOUT_GRAPHICS
#endif

#ifdef WITHOUT_GRAPHICS
#define WITHOUT_BITMAP
#define WITHOUT_TILE
#define WITHOUT_SPRITE
#endif

#ifdef WITHOUT_KERNEL
#define WITHOUT_FILE
#define WITHOUT_MAIN
#define WITHOUT_PLATFORM
#endif

#ifdef WITHOUT_TEXT
#define WITHOUT_PLATFORM
#endif

#ifdef WITHOUT_MATH
#define WITHOUT_TEXT
#define WITHOUT_PLATFORM
#endif


// ============================================================
// Hardware register definitions
// ============================================================

#include "f256_regs.h"
#include "f_api.h"


// ============================================================
// Far memory swap slot configuration
// (Must be after f256_regs.h which defines MMU_MEM_BANK_*)
// ============================================================

#ifndef SWAP_SLOT
#define SWAP_SLOT  MMU_MEM_BANK_7
#endif

#if SWAP_SLOT == MMU_MEM_BANK_7

#define SWAP_IO_SETUP() \
	byte sios_ram = PEEK(MMU_MEM_BANK_7); \
	__asm volatile { sei }

#define SWAP_IO_SHUTDOWN() \
	POKE_MEMMAP(MMU_MEM_BANK_7, sios_ram); \
	__asm volatile { cli }

#else

#define SWAP_IO_SETUP()
#define SWAP_IO_SHUTDOWN()

#endif

#ifdef SWAP_RESTORE
#define SWAP_RESTORE_SLOT()   POKE_MEMMAP(SWAP_SLOT, SWAP_SLOT - MMU_MEM_BANK_0)
#else
#define SWAP_RESTORE_SLOT()
#endif

#define SWAP_ADDR  ((uint16_t)(SWAP_SLOT - MMU_MEM_BANK_0) * (uint16_t)0x2000)


// ============================================================
// Module headers (each triggers #pragma compile for its .c file)
// ============================================================

#include "f_kernel.h"
#include "f_dma.h"
#include "f_math.h"
#include "f_random.h"
#include "f_text.h"
#include "f_bitmap.h"
#include "f_tile.h"
#include "f_graphics.h"
#include "f_sprite.h"
#include "f_keyboard.h"
#include "f_midi.h"
#include "f_sid.h"
#include "f_opl3.h"
#include "f_psg.h"
#include "f_pads.h"
#include "f_mouse.h"
#include "f_leds.h"
#include "f_lcd.h"
#include "f_timer0.h"
#include "f_vs1053b.h"
#include "f_dispatch.h"
#include "f_midiin.h"
#include "f_file.h"
#include "f_midiplay.h"
#include "f_vgmplay.h"
#include "f_filepicker.h"
#include "f_platform.h"
#include "f_uart.h"
#include "f_rtc.h"
#include "f_general.h"


// ============================================================
// Core functions
// ============================================================

void      f256Init(void);
void      f256Reset(void);
byte      FAR_PEEK(uint32_t address);
uint16_t  FAR_PEEKW(uint32_t address);
void     *FAR_POINTER(uint32_t address);
void      FAR_POKE(uint32_t address, byte value);
void      FAR_POKEW(uint32_t address, uint16_t value);


// ============================================================
// Auto-init: rename main to f256main so f256lib.c can wrap it
// ============================================================

#ifndef WITHOUT_MAIN
#define main f256main
#endif


#pragma compile("f256lib.c")


#endif // F256LIB_H
