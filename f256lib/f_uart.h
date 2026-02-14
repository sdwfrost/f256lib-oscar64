/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef UART_H
#define UART_H
#ifndef WITHOUT_UART


#include "f256lib.h"


// Baud rate divisors for 16750-compatible UART at 1.8432 MHz reference clock
#define UART_BAUD_DIV_300      0x0180
#define UART_BAUD_DIV_1200     0x0060
#define UART_BAUD_DIV_2400     0x0030
#define UART_BAUD_DIV_3600     0x0020
#define UART_BAUD_DIV_4800     0x0018
#define UART_BAUD_DIV_9600     0x000C
#define UART_BAUD_DIV_19200    0x0006
#define UART_BAUD_DIV_38400    0x0003
#define UART_BAUD_DIV_57600    0x0002
#define UART_BAUD_DIV_115200   0x0001

// UART DLAB mask (Line Control Register bit 7)
#define UART_DLAB_MASK         0x80

// UART line config: 8 data bits, 1 stop bit, no parity, no break
#define UART_DATA_BITS         0x03
#define UART_STOP_BITS         0x00
#define UART_PARITY            0x00
#define UART_NO_BRK_SIG       0x00

// UART LSR status bits
#define UART_LSR_DATA_READY    0x01
#define UART_LSR_OVERRUN_ERR   0x02
#define UART_LSR_PARITY_ERR    0x04
#define UART_LSR_FRAMING_ERR   0x08
#define UART_LSR_BREAK_INT     0x10
#define UART_LSR_THR_EMPTY     0x20
#define UART_LSR_TX_EMPTY      0x40
#define UART_LSR_FIFO_ERR      0x80

// UART FIFO Control Register values
#define UART_FCR_ENABLE        0x01
#define UART_FCR_CLEAR_RX      0x02
#define UART_FCR_CLEAR_TX      0x04
#define UART_FCR_TRIGGER_14    0xC0


// Initialize UART with the specified baud rate divisor
void uartInit(uint16_t baud_divisor);

// Change baud rate (UART must already be initialized)
void uartSetBaud(uint16_t baud_divisor);

// Send a single byte over the UART
void uartSendByte(byte b);

// Send a buffer of bytes over the UART
void uartSendData(const byte *data, uint16_t len);

// Check if data is available to read
bool uartDataReady(void);

// Read a single byte from the UART (blocks until data available)
byte uartReadByte(void);

// Flush the receive buffer by reading and discarding all available data
void uartFlush(void);


#pragma compile("f_uart.c")


#endif
#endif // UART_H
