#include "f256lib.h"

int main(int argc, char *argv[]) {
	uint16_t howMany;
	uint8_t i, detected;
	uint8_t inst = 0;

	POKE(0xD620, 0x1F);
	POKE(0xD621, 0x2A);
	POKE(0xD622, 0x01);
	while (PEEK(0xD622) & 0x01);

	initVS1053MIDI();

	POKE(MIDI_FIFO_ALT, 0xC0);
	POKE(MIDI_FIFO_ALT, 30);

	while (true) {
		if (!(PEEK(MIDI_CTRL) & 0x02)) {
			howMany = PEEKW(MIDI_RXD) & 0x0FFF;
			for (i = 0; i < howMany; i++) {
				detected = (uint8_t)PEEK(MIDI_FIFO);
				POKE(MIDI_FIFO_ALT, detected);
			}
		}
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.raw) {
				case 0xb6:
					if (inst < 127) {
						inst++;
						POKE(MIDI_FIFO_ALT, 0xC0);
						POKE(MIDI_FIFO_ALT, inst);
						textGotoXY(0, 0);
						printf("%s                  ", midi_instruments[inst]);
					}
					break;
				case 0xb7:
					if (inst > 0) {
						inst--;
						POKE(MIDI_FIFO_ALT, 0xC0);
						POKE(MIDI_FIFO_ALT, inst);
						textGotoXY(0, 0);
						printf("%s                   ", midi_instruments[inst]);
					}
					break;
			}
		}
	}

	return 0;
}
