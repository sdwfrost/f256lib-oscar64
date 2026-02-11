/*
 *	Hardware Timer 0 support for F256.
 *	Adapted from mu0nlibs/muTimer0Int for oscar64.
 */


#ifndef TIMER0_H
#define TIMER0_H
#ifndef WITHOUT_TIMER0


#include "f256lib.h"


void timer0Set(uint32_t value);
void timer0Reset(void);
void timer0Load(uint32_t value);


// ============================================================
// Backward-compatible aliases (old mu0nlibs timer0 API)
// ============================================================

// Old 3-param setTimer0(lo,mid,hi) -> new timer0Set(uint32_t)
#define setTimer0(lo, mid, hi)  timer0Set(((uint32_t)(lo)) | (((uint32_t)(mid)) << 8) | (((uint32_t)(hi)) << 16))

// Old isTimer0Done() -> check INT_PENDING_0 & INT_TIMER_0
#define isTimer0Done()          (PEEK(INT_PENDING_0) & INT_TIMER_0)

// Old T0_PEND register address alias (guard to avoid redefinition in doodles that define it locally)
#ifndef T0_PEND
#define T0_PEND                 INT_PEND_0
#endif


#pragma compile("f_timer0.c")


#endif
#endif // TIMER0_H
