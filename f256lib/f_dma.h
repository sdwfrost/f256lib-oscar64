/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef DMA_H
#define DMA_H
#ifndef WITHOUT_DMA


#include "f256lib.h"


void dmaFill(uint32_t start, uint32_t length, byte value);
void dma2dFill(uint32_t start, uint16_t width, uint16_t height, uint16_t stride, byte value);


#pragma compile("f_dma.c")


#endif
#endif // DMA_H
