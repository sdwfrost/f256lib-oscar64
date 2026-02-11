/*
 *	Timer0 interrupt example.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *
 *	NOTE: This doodle used llvm-mos specific interrupt attributes
 *	(__attribute__((interrupt_norecurse))) and the muTimer0Int module
 *	for IRQ vector manipulation. These are not directly portable to oscar64.
 *	This version uses polling instead of interrupts as a workaround.
 */

#include "f256lib.h"
#undef setTimer0

// TODO: Port needed for muTimer0Int (IRQ vector manipulation)

#define RATE 0x00FFFFFF

#define T0_CTR      0xD650
#define T0_CMP_CTR  0xD654
#define T0_CMP_L    0xD655
#undef INT_PENDING_0
#define INT_PENDING_0  0xD660

#define CTR_INTEN   0x80
#define CTR_ENABLE  0x01
#define CTR_CLEAR   0x02
#define CTR_UPDOWN  0x08

uint8_t color = 1;

void writeStars(void) {
	color = color ? 0 : 1;
	textSetColor(color, 0);
	textGotoXY(0, 1);
	textPrint("*");
}

void loadTimer(uint32_t value) {
	POKE(T0_CTR, CTR_CLEAR);
	POKEA(T0_CMP_L, value);
}

void setTimer0(uint32_t value) {
	loadTimer(value);
	POKE(T0_CMP_CTR, 0);
	POKE(T0_CTR, CTR_CLEAR);
	POKE(T0_CTR, CTR_INTEN | CTR_UPDOWN | CTR_ENABLE);
}

int main(int argc, char *argv[]) {

	printf("this is supposed to trigger every 2/3rds of a second\n");

	setTimer0(RATE);

	/* Polling version - original used IRQ handler */
	while (true) {
		if (PEEK(INT_PENDING_0) & 0x10) {
			writeStars();
			POKE(INT_PENDING_0, 0x10);
			loadTimer(RATE);
		}
	}
	return 0;
}
