#include "f256lib.h"

#define TIMER_QUERY_A 0
#define TIMER_QUERY_B 1
#define TIMER_QUERY_C 2
#define TIMER_QUERY_D 3
#define TIMER_QUERY_E 4

#define TIMER_DELAY_A 5
#define TIMER_DELAY_B 20
#define TIMER_DELAY_C 1
#define TIMER_DELAY_D 3
#define TIMER_DELAY_E 60


struct timer_t myTimers[5];
uint8_t animFrame[5];
uint8_t delays[5];

void textSetup(void) {
	textClear();
	textSetDouble(true, true);
}

void timerSetup(void) {
	uint8_t i;

	for (i = 0; i < 5; i++) animFrame[i] = 1;

	delays[0] = TIMER_DELAY_A;
	delays[1] = TIMER_DELAY_B;
	delays[2] = TIMER_DELAY_C;
	delays[3] = TIMER_DELAY_D;
	delays[4] = TIMER_DELAY_E;

	myTimers[0].units = TIMER_FRAMES;
	myTimers[1].units = TIMER_FRAMES;
	myTimers[2].units = TIMER_SECONDS;
	myTimers[3].units = TIMER_SECONDS;
	myTimers[4].units = TIMER_SECONDS;

	for (i = 0; i < 5; i++) {
		myTimers[i].absolute = delays[i];
	}

	myTimers[0].cookie = TIMER_QUERY_A;
	myTimers[1].cookie = TIMER_QUERY_B;
	myTimers[2].cookie = TIMER_QUERY_C;
	myTimers[3].cookie = TIMER_QUERY_D;
	myTimers[4].cookie = TIMER_QUERY_E;

	for (i = 0; i < 5; i++) kernelSetTimer(&myTimers[i]);
}

void touchSprite(uint8_t spriteIndex) {
	/* placeholder for sprite animation */
}

int main(int argc, char *argv[]) {
	byte curTimer;

	textSetup();
	timerSetup();

	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			curTimer = kernelEventData.u.timer.cookie;
			touchSprite(curTimer);
			if (animFrame[curTimer] > 14) animFrame[curTimer] = 1;
			myTimers[curTimer].absolute += delays[curTimer];
			kernelSetTimer(&myTimers[curTimer]);
		}
	}
	return 0;
}
