/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef RTC_H
#define RTC_H
#ifndef WITHOUT_RTC


#include "f256lib.h"


// RTC control bit masks
#define RTC_CTRL_UTI   0x08  // Update Transfer Inhibit
#define RTC_CTRL_STOP  0x04  // Stop (halt updates)
#define RTC_CTRL_24HR  0x02  // 24-hour mode
#define RTC_CTRL_DSE   0x01  // Daylight Saving Enable


typedef struct {
	byte year;    // 0-99 (BCD)
	byte month;   // 1-12 (BCD)
	byte day;     // 1-31 (BCD)
	byte hour;    // 0-23 (BCD, in 24hr mode)
	byte min;     // 0-59 (BCD)
	byte sec;     // 0-59 (BCD)
} rtcTimeT;


// Read the current time from the RTC (values in BCD)
void rtcRead(rtcTimeT *t);

// Write a time to the RTC (values in BCD)
void rtcWrite(const rtcTimeT *t);

// Convert a BCD byte to decimal
byte rtcBcdToDecimal(byte bcd);

// Convert a decimal byte to BCD
byte rtcDecimalToBcd(byte val);


#pragma compile("f_rtc.c")


#endif
#endif // RTC_H
