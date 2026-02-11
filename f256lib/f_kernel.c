/*
 *	Copyright (c) 2024 Scott Duensing, scott@kangaroopunch.com
 *	Adapted for oscar64.
 */


#ifndef WITHOUT_KERNEL


#include "f256lib.h"


kernelEventT kernelEventData;  // Allocate some RAM to hold event data.
kernelArgsT *kernelArgs;       // Create an alias for the kernel args.
char         _kernelError;
unsigned int _kern_target;     // Target address for self-modifying kernel call.


unsigned char kernelGetPending(void) {
	return -kernelArgs->events.pending;
}


void kernelReset(void) {
	// Plop this into the zero page where the kernel can find it.
	kernelArgs = (kernelArgsT *)0x00f0;
	// Tell the kernel where our event buffer lives.
	kernelArgs->events.event = &kernelEventData;
}


bool kernelSetTimer(const struct timer_t *timer) {
	kernelArgs->u.timer.units    = timer->units;
	kernelArgs->u.timer.absolute = timer->absolute;
	kernelArgs->u.timer.cookie   = timer->cookie;
	kernelCall(Clock.SetTimer);
	return !kernelError;
}


uint8_t kernelGetTimerAbsolute(uint8_t units) {
	kernelArgs->u.timer.units = units | 0x80;
	return kernelCall(Clock.SetTimer);
}


void kernelPause(uint8_t frames) {
	struct timer_t pauseTimer;
	pauseTimer.units    = 0;
	pauseTimer.cookie   = 213;
	pauseTimer.absolute = kernelGetTimerAbsolute(0) + frames;
	kernelSetTimer(&pauseTimer);
	while (1) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			if (kernelEventData.u.timer.cookie == 213) {
				return;
			}
		}
	}
}


void kernelWaitKey(void) {
	while (1) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.raw) {
				case 148:  // Enter
				case 32:   // Space
					return;
			}
		}
	}
}


#endif
