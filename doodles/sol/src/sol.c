#include "f256lib.h"

#define BRDR_CTRL   0xD004
#define BRDR_BLUE   0xD005
#define BRDR_GREEN  0xD006
#define BRDR_RED    0xD007
#define BRDR_WIDTH  0xD008
#define BRDR_HEIGHT 0xD009

int main(int argc, char *argv[]) {
	uint8_t x = 0;
	uint8_t c = 0;

	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0x01);
	POKE(VKY_MSTR_CTRL_1, 0x00);
	POKE(MMU_IO_CTRL, 0);

	POKE(0xD00D, 0x55);
	POKE(BRDR_CTRL, 0x01);
	POKE(BRDR_BLUE, 0x22);
	POKE(BRDR_RED, 0x55);
	POKE(BRDR_WIDTH, 31);
	POKE(BRDR_HEIGHT, 10);
	textGotoXY(10, 10);
	printf("allo");
	while (true) {
		POKE(0xD00D, c);
		POKE(0xD00E, c);
		POKE(BRDR_BLUE, c);
		c++;
		POKE(BRDR_WIDTH, x);
		x++;
		if (x > 31) x = 0;
	}
	return 0;
}
