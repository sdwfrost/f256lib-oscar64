#include "f256lib.h"


void oldText(void) {
	uint8_t x = 0, y = 0;

	POKE(VKY_MSTR_CTRL_0, 0b00100111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);

	textSetColor(0xF, 0);
	textClear();
	textPrint("Let's see those text colors ya? [F1] Toggle old/new [F3] Double Height toggle");

	for (x = 0; x < 16; x++) {
		textSetColor(x, 0);
		textGotoXY(0 + x * 5, 1);
		printf(" %02d  ", x);
	}

	for (x = 0; x < 16; x++) {
		for (y = 0; y < 16; y++) {
			textSetColor(x, y);
			textGotoXY(0 + x * 5, 2 + y);
			textPrint("@#3E ");
		}
	}
}

void newText(void) {
	uint8_t buf = 0;
	textSetColor(3, 4);
	textClear();

	POKE(MMU_IO_CTRL, 0b01000000);

	POKE(VKY_MSTR_CTRL_0, 0b00000111);
	POKE(VKY_MSTR_CTRL_1, 0b11010000);

	POKE(0xD300, 0x01);

	POKEA(0xD304, 0x10000);
	for (uint16_t x = 0; x < 0x2580; x += 2) {
		uint16_t val = (x / 2) % 256;
		uint32_t addr = 0x00010000 + (uint32_t)x;
		FAR_POKE(addr, (uint8_t)val);
		FAR_POKE(addr + (uint32_t)1, 0);
	}
	POKEA(0xD308, 0x20000);
	for (uint16_t x = 0; x < 0x2580; x += 2) {
		uint16_t val = (x / 2) % 256;
		uint32_t addr = 0x00020000 + (uint32_t)x;
		FAR_POKE(addr, (uint8_t)val);
		FAR_POKE(addr + (uint32_t)1, (uint8_t)val);
	}

	POKE(MMU_IO_CTRL, 0b00001000);

	for (uint16_t x = 0; x < 0x400; x += 4) {
		uint16_t val = (x) % 256;
		uint16_t lav = 256 - val;
		POKE(0xC000 + (uint16_t)(x), (uint8_t)lav);
		POKE(0xC000 + (uint16_t)(x + 1), 0);
		POKE(0xC000 + (uint16_t)(x + 2), 0);
		POKE(0xC000 + (uint16_t)(x + 3), 0);
		POKE(0xC400 + (uint16_t)(x), 0);
		POKE(0xC400 + (uint16_t)(x + 1), 0);
		POKE(0xC400 + (uint16_t)(x + 2), (uint8_t)lav);
		POKE(0xC400 + (uint16_t)(x + 3), 0);
		POKE(0xC800 + (uint16_t)(x), (uint8_t)val);
		POKE(0xC800 + (uint16_t)(x + 1), (uint8_t)lav);
		POKE(0xC800 + (uint16_t)(x + 2), (uint8_t)lav);
		POKE(0xC800 + (uint16_t)(x + 3), 0);
		POKE(0xCC00 + (uint16_t)(x), 0);
		POKE(0xCC00 + (uint16_t)(x + 1), (uint8_t)lav);
		POKE(0xCC00 + (uint16_t)(x + 2), 0);
		POKE(0xCC00 + (uint16_t)(x + 3), 0);
	}

	POKE(MMU_IO_CTRL, 0);
}

int main(int argc, char *argv[]) {
	bool oldOrNew = true, regOrDouble = true;

	POKE(VKY_MSTR_CTRL_0, 0b00100111);
	POKE(VKY_MSTR_CTRL_1, 0b00010000);

	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);

	POKE(VKY_MSTR_CTRL_1, (PEEK(VKY_MSTR_CTRL_1) & 0xEF) | 0x10);

	oldText();
	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.raw) {
				case 0x92:
					return 0;
				case 0x81:
					if (oldOrNew) {
						newText();
						oldOrNew = false;
					} else {
						oldText();
						oldOrNew = true;
					}
					break;
				case 0x83:
					if (regOrDouble) {
						POKE(MMU_IO_CTRL, 0x00);
						POKE(VKY_MSTR_CTRL_1, 0b00010100);
						regOrDouble = false;
					} else {
						POKE(MMU_IO_CTRL, 0x00);
						POKE(VKY_MSTR_CTRL_1, 0b00010000);
						regOrDouble = true;
					}
					break;
			}
		}
	}

	return 0;
}
