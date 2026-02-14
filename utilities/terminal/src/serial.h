/*
 * serial.h - ANSI terminal processing and UART buffer management
 * Ported from F256-terminal CC65 version to oscar64/f256lib
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "f256lib.h"

#pragma compile("serial.c")


// ANSI color codes (standard 16-color)
#define ANSI_COLOR_BLACK           0x00
#define ANSI_COLOR_RED             0x01
#define ANSI_COLOR_GREEN           0x02
#define ANSI_COLOR_YELLOW          0x03
#define ANSI_COLOR_BLUE            0x04
#define ANSI_COLOR_MAGENTA         0x05
#define ANSI_COLOR_CYAN            0x06
#define ANSI_COLOR_WHITE           0x07
#define ANSI_COLOR_BRIGHT_BLACK    0x08
#define ANSI_COLOR_BRIGHT_RED      0x09
#define ANSI_COLOR_BRIGHT_GREEN    0x0A
#define ANSI_COLOR_BRIGHT_YELLOW   0x0B
#define ANSI_COLOR_BRIGHT_BLUE     0x0C
#define ANSI_COLOR_BRIGHT_MAGENTA  0x0D
#define ANSI_COLOR_BRIGHT_CYAN     0x0E
#define ANSI_COLOR_BRIGHT_WHITE    0x0F

// Circular buffer size for UART data (must be power of 2)
#define UART_BUFFER_SIZE   2048
#define UART_BUFFER_MASK   (UART_BUFFER_SIZE - 1)


// Initialize ANSI color LUTs in VICKY
void Serial_InitANSIColors(void);

// Initialize UART hardware
void Serial_InitUART(uint16_t baud_divisor);

// Change baud rate
void Serial_SetBaud(uint16_t baud_divisor);

// Send a single byte over UART
bool Serial_SendByte(byte the_byte);

// Send a buffer of bytes over UART
byte Serial_SendData(const byte *buffer, uint16_t len);

// Read UART data into circular buffer
bool Serial_ReadUART(void);

// Process available data in the circular buffer
bool Serial_ProcessAvailableData(void);

// Flush the receive buffer
void Serial_FlushInBuffer(void);

// Save/restore cursor position
void Serial_ANSICursorSave(void);
void Serial_ANSICursorRestore(void);

// Cycle the foreground color for the terminal area
void Serial_CycleForegroundColor(void);


#endif /* SERIAL_H_ */
