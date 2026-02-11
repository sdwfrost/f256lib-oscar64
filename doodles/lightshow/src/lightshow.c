#include "f256lib.h"

int main(int argc, char *argv[]) {
	uint16_t r1, r2, r3, r4, r5, r6;
	bool hasNet = false, hasLock = false;

	textGotoXY(1, 20);
	if (platformIsAnyK()) {
		if (platformHasCaseLCD()) {
			ledsEnableManual(true, true, true, true, true, true);
			textPrint("a F256K2 is detected. 4 LEDs will participate.");
			hasNet = true;
			hasLock = true;
		} else {
			ledsEnableManual(false, true, true, true, true, true);
			textPrint("a F256K is detected. 3 LEDs will participate.");
			hasLock = true;
		}
	} else {
		ledsEnableManual(false, false, true, true, true, true);
		textPrint("a F256Jr. or Jr2 is detected. 2 LEDs will participate.");
	}

	while (true) {
		r1 = randomRead();
		r2 = randomRead();
		r3 = randomRead();
		r4 = randomRead();
		r5 = randomRead();
		r6 = randomRead();
		POKE(LED_PWR_B, HIGH_BYTE(r1));
		POKE(LED_PWR_G, LOW_BYTE(r1));
		POKE(LED_PWR_R, HIGH_BYTE(r2));
		POKE(LED_SD_B, LOW_BYTE(r2));
		POKE(LED_SD_G, HIGH_BYTE(r3));
		POKE(LED_SD_R, LOW_BYTE(r3));
		if (hasLock) {
			POKE(LED_LCK_B, HIGH_BYTE(r4));
			POKE(LED_LCK_G, LOW_BYTE(r4));
			POKE(LED_LCK_R, HIGH_BYTE(r5));
		}
		if (hasNet) {
			POKE(LED_NET_B, LOW_BYTE(r5));
			POKE(LED_NET_G, HIGH_BYTE(r6));
			POKE(LED_NET_R, LOW_BYTE(r6));
		}
		kernelPause(20);
	}

	while (true);
	return 0;
}
