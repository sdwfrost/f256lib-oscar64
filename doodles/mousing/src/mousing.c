#include "f256lib.h"


void backgroundSetup(void) {
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00101111);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
	POKE(VKY_LAYER_CTRL_0, 0b00000001);
	POKE(VKY_LAYER_CTRL_1, 0b00000010);
	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	bitmapSetActive(0);
	bitmapSetCLUT(0);

	bitmapSetVisible(0, false);
	bitmapSetVisible(1, false);
	bitmapSetVisible(2, false);
}

int main(int argc, char *argv[]) {
	int16_t newX, newY;
	int8_t boost;
	uint32_t clicks = 0;

	backgroundSetup();
	mouseInit();

	textGotoXY(10, 10);
	printf("Number of clicks: ");
	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(mouse.CLICKS)) {
			textGotoXY(10, 11);
			printf("%ld", ++clicks);
		} else if (kernelEventData.type == kernelEvent(mouse.DELTA)) {
			if ((int8_t)kernelEventData.u.mouse.delta.x > 4 || (int8_t)kernelEventData.u.mouse.delta.x < -4) boost = 2;
			else boost = 1;
			newX = PEEKW(PS2_M_X_LO) + boost * (int8_t)kernelEventData.u.mouse.delta.x;
			if ((int8_t)kernelEventData.u.mouse.delta.y > 4 || (int8_t)kernelEventData.u.mouse.delta.y < -4) boost = 2;
			else boost = 1;
			newY = PEEKW(PS2_M_Y_LO) + boost * (int8_t)kernelEventData.u.mouse.delta.y;

			if (newX < 0) newX = 0;
			if (newX > 640 - 16) newX = 640 - 16;
			if (newY < 0) newY = 0;
			if (newY > 480 - 16) newY = 480 - 16;
			POKEW(PS2_M_X_LO, newX);
			POKEW(PS2_M_Y_LO, newY);
		}
	}

	return 0;
}
