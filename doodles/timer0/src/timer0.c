/*
 *	Simple timer0 example.
 *	Ported from F256KsimpleCdoodles for oscar64.
 */

#include "f256lib.h"
#undef setTimer0
#undef isTimer0Done

#undef T0_CTR
#define T0_CTR      0xD650
#define T0_CMP_CTR  0xD654
#define T0_CMP_L    0xD655
#define T0_CMP_M    0xD656
#define T0_CMP_H    0xD657
#undef INT_PENDING_0
#define INT_PENDING_0  0xD660

#define CTR_INTEN   0x80
#define CTR_ENABLE  0x01
#define CTR_CLEAR   0x02
#define CTR_LOAD    0x04
#define CTR_UPDOWN  0x08

#define MIDI 0xDDA1

uint8_t color = 1;

void noteOnMIDI(void);
void writeStars(void);
void setTimer0(uint32_t);
void loadTimer(uint32_t);
void resetTimer0(void);

void noteOnMIDI() {
	POKE(MIDI, 0x90);
	POKE(MIDI, 0x39);
	POKE(MIDI, 0x4F);
}

void writeStars() {
	color = color ? 0 : 1;
	textSetColor(color + 10, 0);
	textPrint("**");
}

void loadTimer(uint32_t value) {
	POKE(T0_CTR, CTR_CLEAR);
	POKEA(T0_CMP_L, value);
}

void resetTimer0() {
	POKE(T0_CMP_CTR, 0);
	POKE(T0_CTR, CTR_CLEAR);
	POKE(T0_CTR, CTR_INTEN | CTR_UPDOWN | CTR_ENABLE);
}

void setTimer0(uint32_t value) {
	loadTimer(value);
	resetTimer0();
}

int main(int argc, char *argv[]) {

	setTimer0(0x00FFFFFF);
	while (true) {
		if (PEEK(INT_PENDING_0) & 0x10) {
			noteOnMIDI();
			writeStars();
			POKE(INT_PENDING_0, 0x10);
			setTimer0(0x00FFFFFF);
		}
	}
	return 0;
}
