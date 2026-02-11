/*
 *	VIA Timer interrupt example.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *
 *	NOTE: This doodle used llvm-mos specific __attribute__((interrupt))
 *	for the ISR. This version uses polling instead of interrupts as a workaround.
 */

#define VIA_BASE 0xDC00
#define VIA_T1CL (VIA_BASE + 0x04) // Timer 1 Counter Low
#define VIA_T1CH (VIA_BASE + 0x05) // Timer 1 Counter High
#define VIA_ACR (VIA_BASE + 0x0B)  // Auxiliary Control Register
#define VIA_IFR (VIA_BASE + 0x0D)  // Interrupt Flag Register
#define VIA_IER (VIA_BASE + 0x0E)  // Interrupt Enable Register


#include "f256lib.h"

void setup_timer0(void);

void setup_timer0() {
	// Set the timer counter value
	POKE(VIA_T1CL, 0xFF); // Low byte
	POKE(VIA_T1CH, 0xFF); // High byte

	// Configure the timer in the ACR
	POKE(VIA_ACR, PEEK(VIA_ACR) | 0x40); // Continuous mode

	// Enable the timer interrupt in the IER
	POKE(VIA_IER, PEEK(VIA_IER) | 0x82); // Enable Timer 1 interrupt
}

int main(int argc, char *argv[]) {
	setup_timer0();

	/* Polling version - original used __attribute__((interrupt)) ISR */
	while (true) {
		if (PEEK(VIA_IFR) & 0x40) {
			// Clear the interrupt flag
			POKE(VIA_IFR, 0x40);

			// Callback: send MIDI note
			POKE(0xDDA1, 0x90);
			POKE(0xDDA1, 0x39);
			POKE(0xDDA1, 0x3F);
		}
	}
	return 0;
}
