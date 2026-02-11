/*
 *	Simple timer1 example.
 *	Ported from F256KsimpleCdoodles for oscar64.
 */

#include "f256lib.h"

#define T1_CTR      0xD658
#define T1_CMP_CTR  0xD65C
#define T1_CMP_L    0xD65D
#define T1_CMP_M    0xD65E
#define T1_CMP_H    0xD65F
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
void setTimer1(uint32_t);
void loadTimer1(uint32_t);
void resetTimer1(void);

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

void loadTimer1(uint32_t value) {
	POKE(T1_CTR, CTR_CLEAR);
	POKEA(T1_CMP_L, value);
}

void resetTimer1() {
	POKE(T1_CMP_CTR, 0);
	POKE(T1_CTR, CTR_CLEAR);
	POKE(T1_CTR, CTR_INTEN | CTR_UPDOWN | CTR_ENABLE);
}

void setTimer1(uint32_t value) {
	loadTimer1(value);
	resetTimer1();
}

int main(int argc, char *argv[]) {

	setTimer1(0x0000003F);
	while (true) {
		if (PEEK(INT_PENDING_0) & 0x20) {
			noteOnMIDI();
			writeStars();
			POKE(INT_PENDING_0, 0x20);
			setTimer1(0x0000003F);
		}
	}
	return 0;
}
