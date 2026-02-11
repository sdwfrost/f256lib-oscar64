/*
 *	Common utility functions for doodle examples.
 *	Ported from mu0nlibs/muUtils for oscar64.
 */

#include "f256lib.h"
#include "muUtils.h"


bool setTimer(const struct timer_t *timer) {
	kernelArgs->u.timer.units    = timer->units;
	kernelArgs->u.timer.absolute = timer->absolute;
	kernelArgs->u.timer.cookie   = timer->cookie;
	kernelCall(Clock.SetTimer);
	return !kernelError;
}


uint8_t getTimerAbsolute(uint8_t units) {
	kernelArgs->u.timer.units = units | 0x80;
	return kernelCall(Clock.SetTimer);
}


void lilpause(uint8_t timedelay) {
	struct timer_t pauseTimer;
	bool noteExitFlag = false;
	pauseTimer.units = 0;
	pauseTimer.cookie = 213;
	pauseTimer.absolute = getTimerAbsolute(0) + timedelay;
	setTimer(&pauseTimer);
	while (!noteExitFlag) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			if (kernelEventData.u.timer.cookie == 213) {
				noteExitFlag = true;
			}
		}
	}
}


bool isAnyK(void) {
	uint8_t value = PEEK(0xD6A7) & 0x1F;
	return (value >= 0x10 && value <= 0x16);
}


bool isK2(void) {
	uint8_t value = PEEK(0xD6A7) & 0x1F;
	return (value == 0x11);
}


bool hasCaseLCD(void) {
	return ((PEEK(0xDDC1) & 0x02) == 0);
}


bool isWave2(void) {
	uint8_t mid;
	mid = PEEK(0xD6A7) & 0x3F;
	return (mid == 0x22 || mid == 0x11);
}


void hitspace(void) {
	bool exitFlag = false;
	while (exitFlag == false) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.raw) {
				case 148:
				case 32:
					exitFlag = true;
					break;
			}
		}
	}
}


void wipeBitmapBackground(uint8_t r, uint8_t g, uint8_t b) {
	POKE(0xD00D, b);
	POKE(0xD00E, g);
	POKE(0xD00F, r);
}
