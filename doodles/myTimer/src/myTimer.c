#include "f256lib.h"

#define TIMER_QUERY_A 0
#define TIMER_QUERY_B 1
#define TIMER_QUERY_C 2
#define TIMER_QUERY_D 3
#define TIMER_QUERY_E 4
#define TIMER_QUERY_F 5

#define TIMER_DELAY_A 1
#define TIMER_DELAY_B 5
#define TIMER_DELAY_C 20
#define TIMER_DELAY_D 1
#define TIMER_DELAY_E 3
#define TIMER_DELAY_F 60


struct timer_t myTimers[6];
uint8_t animFrame[6];
uint8_t delays[6];

void injectChar(uint8_t x, uint8_t y, uint8_t theChar) {
	POKE(0x0001, 0x02);
	POKE(0xC000 + 40 * y + x, theChar);
	POKE(0x0001, 0x00);
}

void textSetup(void) {
	textClear();
	textSetDouble(true, true);
	textGotoXY(10, 3);
	textPrint("- Every frame");
	textGotoXY(10, 5);
	textPrint("- Every 5 frames");
	textGotoXY(10, 7);
	textPrint("- Every 20 frames");
	textGotoXY(10, 9);
	textPrint("- Every second");
	textGotoXY(10, 11);
	textPrint("- Every 3 seconds");
	textGotoXY(10, 13);
	textPrint("- Every minute");
}

void timerSetup(void) {
	uint8_t i;

	for (i = 0; i < 6; i++) animFrame[i] = 1;

	delays[0] = TIMER_DELAY_A;
	delays[1] = TIMER_DELAY_B;
	delays[2] = TIMER_DELAY_C;
	delays[3] = TIMER_DELAY_D;
	delays[4] = TIMER_DELAY_E;
	delays[5] = TIMER_DELAY_F;

	myTimers[0].units = TIMER_FRAMES;
	myTimers[1].units = TIMER_FRAMES;
	myTimers[2].units = TIMER_FRAMES;
	myTimers[3].units = TIMER_SECONDS;
	myTimers[4].units = TIMER_SECONDS;
	myTimers[5].units = TIMER_SECONDS;

	kernelNextEvent();

	myTimers[0].cookie = TIMER_QUERY_A;
	myTimers[1].cookie = TIMER_QUERY_B;
	myTimers[2].cookie = TIMER_QUERY_C;
	myTimers[3].cookie = TIMER_QUERY_D;
	myTimers[4].cookie = TIMER_QUERY_E;
	myTimers[5].cookie = TIMER_QUERY_F;

	myTimers[0].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + delays[0];
	myTimers[1].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + delays[1];
	myTimers[2].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + delays[2];
	myTimers[3].absolute = kernelGetTimerAbsolute(TIMER_SECONDS) + delays[3];
	myTimers[4].absolute = kernelGetTimerAbsolute(TIMER_SECONDS) + delays[4];
	myTimers[5].absolute = kernelGetTimerAbsolute(TIMER_SECONDS) + delays[5];

	for (i = 0; i < 6; i++) kernelSetTimer(&myTimers[i]);
}

int main(int argc, char *argv[]) {
	byte curTimer;

	textSetup();
	timerSetup();

	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			curTimer = kernelEventData.u.timer.cookie;
			injectChar(8, 3 + 2 * curTimer, animFrame[curTimer]++);
			if (animFrame[curTimer] > 14) animFrame[curTimer] = 2;
			myTimers[curTimer].absolute += delays[curTimer];
			kernelSetTimer(&myTimers[curTimer]);
		}
	}
	return 0;
}
