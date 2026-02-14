/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_UART


#include "f256lib.h"


#define UART_MAX_SEND_ATTEMPTS  1000


void uartInit(uint16_t baud_divisor) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Set DLAB to access divisor latch
	POKE(UART_LCR, UART_DLAB_MASK);

	// Set baud rate divisor
	POKE(UART_DLL, LOW_BYTE(baud_divisor));
	POKE(UART_DLH, HIGH_BYTE(baud_divisor));

	// Clear DLAB bit and set line config: 8N1
	POKE(UART_LCR, UART_DATA_BITS | UART_STOP_BITS | UART_PARITY | UART_NO_BRK_SIG);

	// Enable and clear FIFOs, set trigger level to 14 bytes
	POKE(UART_FCR, UART_FCR_ENABLE | UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX | UART_FCR_TRIGGER_14);

	// Disable all UART interrupts (we poll)
	POKE(UART_IER, 0x00);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void uartSetBaud(uint16_t baud_divisor) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Set DLAB to access divisor latch
	POKE(UART_LCR, PEEK(UART_LCR) | UART_DLAB_MASK);

	// Set baud rate divisor
	POKE(UART_DLL, LOW_BYTE(baud_divisor));
	POKE(UART_DLH, HIGH_BYTE(baud_divisor));

	// Clear DLAB
	POKE(UART_LCR, PEEK(UART_LCR) & ~UART_DLAB_MASK);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void uartSendByte(byte b) {
	byte     mmu      = PEEK(MMU_IO_CTRL);
	uint16_t attempts = 0;

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Wait for transmit holding register to be empty
	while (!(PEEK(UART_LSR) & UART_LSR_THR_EMPTY)) {
		if (++attempts >= UART_MAX_SEND_ATTEMPTS) break;
	}

	POKE(UART_TXR, b);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void uartSendData(const byte *data, uint16_t len) {
	uint16_t i;
	for (i = 0; i < len; i++) {
		uartSendByte(data[i]);
	}
}


bool uartDataReady(void) {
	byte mmu    = PEEK(MMU_IO_CTRL);
	bool result;

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);
	result = (PEEK(UART_LSR) & UART_LSR_DATA_READY) != 0;
	POKE_MEMMAP(MMU_IO_CTRL, mmu);

	return result;
}


byte uartReadByte(void) {
	byte mmu    = PEEK(MMU_IO_CTRL);
	byte result;

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Wait for data to be available
	while (!(PEEK(UART_LSR) & UART_LSR_DATA_READY)) {}

	result = PEEK(UART_RXD);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);

	return result;
}


void uartFlush(void) {
	byte mmu = PEEK(MMU_IO_CTRL);

	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	while (PEEK(UART_LSR) & UART_LSR_DATA_READY) {
		(void)PEEK(UART_RXD);
	}

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


#endif
