#include "f256lib.h"

void eraseLine(uint8_t line) {
	textGotoXY(0, line);
	printf("                                                                                ");
}

void setup(void) {
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00001111);
	POKE(VKY_MSTR_CTRL_1, 0b00010101);

	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);
}

void drawNewLine(uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2, uint8_t col) {
	POKE(0xD00A, 0x01);
	POKE(0xD180, 0x01);
	POKEW(0xD182, 0x00);
	POKE(0xD181, col);
	POKEW(0xD182, x1);
	POKEW(0xD184, x2);
	POKE(0xD186, y1);
	POKE(0xD187, y2);
	POKE(0xD180, 0x03);
	while ((PEEK(0xD180) & 0x80) != 0x80)
		;
	POKE(0xD00A, 0x00);
}

void loadGFX(void) {
	graphicsSetLayerBitmap(0, 0);
	bitmapSetActive(0);
	bitmapSetCLUT(0);
	bitmapSetColor(0);
	textGotoXY(0, 24);
	textPrint("[F1 toggle] old Lines    ");
	bitmapClear();
	bitmapSetVisible(0, true);
}

void activate2X(void) {
	uint8_t mmumemctrl = PEEK(MMU_MEM_CTRL);
	uint8_t mmuiomctrl = PEEK(MMU_IO_CTRL);
	POKE(MMU_MEM_CTRL, mmumemctrl | 0b00000100);
	POKE(MMU_IO_CTRL, mmuiomctrl | 0b00000000);
}

int main(int argc, char *argv[]) {
	uint8_t exitFlag = 0;
	uint16_t r1, r2, r3, r4;
	uint8_t y1, y2;
	uint8_t oldNewToggle = 0;

	setup();
	loadGFX();
	activate2X();

	while (exitFlag == 0) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			if (kernelEventData.u.key.raw == 146) {
				return 0;
			}
			if (kernelEventData.u.key.raw == 0x81) {
				oldNewToggle = (oldNewToggle == 0 ? 1 : 0);
				textGotoXY(0, 24);
				if (oldNewToggle == 1) {
					bitmapSetColor(0);
					textPrint("[F1 toggle] new 2x Lines");
					bitmapSetCLUT(1);
				} else {
					bitmapSetColor(0);
					textPrint("[F1 toggle] old Lines    ");
					bitmapSetCLUT(0);
				}
			}
		}

		r1 = randomRead() % 320;
		r2 = randomRead() % 320;
		r3 = randomRead();
		r4 = randomRead() % 255;

		y1 = HIGH_BYTE(r3) % 191;
		y2 = LOW_BYTE(r3) % 191;

		if (oldNewToggle == 0) {
			bitmapSetColor(LOW_BYTE(r4));
			bitmapLine(r1, y1, r2, y2);
		} else {
			drawNewLine(r1, r2, y1, y2, LOW_BYTE(r4));
		}
	}

	return 0;
}
