/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_DMA


#include "f256lib.h"


void dmaFill(uint32_t start, uint32_t length, byte value) {
	while (PEEKW(RAST_ROW_L) != 482); // Wait for VBL.

	POKE(DMA_CTRL, DMA_CTRL_FILL | DMA_CTRL_ENABLE);
	POKE(DMA_FILL_VAL, value);
	POKEA(DMA_DST_ADDR, start);
	POKEA(DMA_COUNT, length);
	POKE(DMA_CTRL, PEEK(DMA_CTRL) | DMA_CTRL_START);
}


void dma2dFill(uint32_t start, uint16_t width, uint16_t height, uint16_t stride, byte value) {
	while (PEEKW(RAST_ROW_L) != 482); // Wait for VBL.

	__asm volatile { sei }
	POKE(DMA_CTRL, DMA_CTRL_2D | DMA_CTRL_FILL | DMA_CTRL_ENABLE);
	POKE(DMA_FILL_VAL, value);
	POKEA(DMA_DST_ADDR, start);
	POKEW(DMA_WIDTH, width);
	POKEW(DMA_HEIGHT, height);
	POKEW(DMA_STRIDE_DST, stride);
	POKE(DMA_CTRL, PEEK(DMA_CTRL) | DMA_CTRL_START);
	__asm volatile { nop }
	__asm volatile { nop }
	__asm volatile { nop }
	__asm volatile { nop }
	__asm volatile { nop }
	__asm volatile { nop }
	__asm volatile { cli }
}


#endif
