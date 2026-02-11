/*
 *	Minimal SID example.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Uses f256lib's SID module instead of mu0nlibs.
 */

#include "f256lib.h"


void setup(void) {
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00000001);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
}

void resetSID(void) {
	sidClearRegisters();
	sidPrepInstruments();
	sidSetMono();
}

int main(int argc, char *argv[]) {
	uint16_t i;

	setup();
	resetSID();

	printf("testing SID");

	POKE(0xD400, 0xF9);
	POKE(0xD401, 0x10);
	POKE(0xD405, 0x44);
	POKE(0xD406, 0x44);
	POKE(0xD402, 0x44);
	POKE(0xD403, 0x00);
	POKE(0xD418, 0x0F);
	POKE(0xD404, 0x21);

	kernelWaitKey();

	POKE(0xD404, 0x20);
	kernelWaitKey();
	return 0;
}
