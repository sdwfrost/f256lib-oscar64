/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_RTC


#include "f256lib.h"


void rtcRead(rtcTimeT *t) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Inhibit updates so we get a consistent snapshot
	POKE(RTC_CTRL, PEEK(RTC_CTRL) | RTC_CTRL_UTI);

	t->sec   = PEEK(RTC_SECS);
	t->min   = PEEK(RTC_MINS);
	t->hour  = PEEK(RTC_HOURS);
	t->day   = PEEK(RTC_DAY);
	t->month = PEEK(RTC_MONTH);
	t->year  = PEEK(RTC_YEAR);

	// Re-enable updates: DSE + 24hr + running
	POKE(RTC_CTRL, RTC_CTRL_DSE | RTC_CTRL_24HR | RTC_CTRL_STOP);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


void rtcWrite(const rtcTimeT *t) {
	byte mmu = PEEK(MMU_IO_CTRL);
	POKE_MEMMAP(MMU_IO_CTRL, MMU_IO_PAGE_0);

	// Stop RTC and inhibit updates
	POKE(RTC_CTRL, RTC_CTRL_UTI | RTC_CTRL_STOP);

	POKE(RTC_SECS,  t->sec);
	POKE(RTC_MINS,  t->min);
	POKE(RTC_HOURS, t->hour);
	POKE(RTC_DAY,   t->day);
	POKE(RTC_MONTH, t->month);
	POKE(RTC_YEAR,  t->year);

	// Restart: DSE + 24hr + running
	POKE(RTC_CTRL, RTC_CTRL_DSE | RTC_CTRL_24HR | RTC_CTRL_STOP);

	POKE_MEMMAP(MMU_IO_CTRL, mmu);
}


byte rtcBcdToDecimal(byte bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}


byte rtcDecimalToBcd(byte val) {
	return ((val / 10) << 4) | (val % 10);
}


#endif
