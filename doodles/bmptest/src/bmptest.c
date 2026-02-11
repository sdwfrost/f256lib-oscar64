/*
 * bmptest - Bitmap diagnostic test for F256K2
 *
 * Draws known lines at specific coordinates to verify:
 * 1. bitmapLine works across full 320-pixel width
 * 2. Pixel plotting at x=0, x=160, x=319 is correct
 * 3. randomRead() produces full-range values
 * 4. divmod remainder (%320) is correct
 *
 * Expected display:
 *   - White diagonal from (0,0) to (319,239)
 *   - Gray horizontal line at y=120 across full width
 *   - Dark gray vertical lines at x=0, x=160, x=319
 *   - Text at top showing random values and %320 results
 *
 * Press ESC to exit.
 */

#include "f256lib.h"
#include <stdio.h>

void setup()
{
	POKE(MMU_IO_CTRL, 0x00);
	POKE(VKY_MSTR_CTRL_0, 0b00001111);
	POKE(VKY_MSTR_CTRL_1, 0b00010101);

	POKE(0xD00D, 0x00);
	POKE(0xD00E, 0x00);
	POKE(0xD00F, 0x00);
}

int main(int argc, char *argv[])
{
	uint16_t r, mod;
	uint16_t i;

	setup();

	graphicsSetLayerBitmap(0, 0);
	bitmapSetActive(0);
	bitmapSetCLUT(0);
	bitmapSetColor(0);
	bitmapClear();
	bitmapSetVisible(0, true);

	/* Test 1: Diagonal line corner to corner (white) */
	bitmapSetColor(255);
	bitmapLine(0, 0, 319, 239);

	/* Test 2: Reverse diagonal (light gray) */
	bitmapSetColor(200);
	bitmapLine(319, 0, 0, 239);

	/* Test 3: Horizontal line across full width at y=120 (medium gray) */
	bitmapSetColor(160);
	bitmapLine(0, 120, 319, 120);

	/* Test 4: Vertical lines at x=0, x=80, x=160, x=240, x=319 (dark gray) */
	bitmapSetColor(100);
	bitmapLine(0, 0, 0, 239);
	bitmapLine(80, 0, 80, 239);
	bitmapLine(160, 0, 160, 239);
	bitmapLine(240, 0, 240, 239);
	bitmapLine(319, 0, 319, 239);

	/* Test 5: Short horizontal lines at specific x positions */
	bitmapSetColor(255);
	bitmapLine(300, 10, 319, 10);  /* Line at far right, y=10 */
	bitmapLine(300, 20, 319, 20);  /* Line at far right, y=20 */

	/* Test 6: Individual pixels at key positions */
	bitmapSetColor(255);
	bitmapPutPixel(0, 0);
	bitmapPutPixel(160, 0);
	bitmapPutPixel(319, 0);
	bitmapPutPixel(0, 239);
	bitmapPutPixel(160, 239);
	bitmapPutPixel(319, 239);

	/* Display diagnostic text */
	textGotoXY(0, 0);
	printf("bmptest: lines should span full 320px");
	textGotoXY(0, 1);
	printf("Diag, H-line, V-lines at 0,80,160,240,319");

	/* Show randomRead and %320 values */
	textGotoXY(0, 3);
	printf("Random samples:");
	for (i = 0; i < 8; i++) {
		r = randomRead();
		mod = r % 320;
		textGotoXY(0, 4 + i);
		printf("r=%05u %%320=%03u", r, mod);
	}

	textGotoXY(0, 13);
	printf("ESC to exit");

	while (1) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			if (kernelEventData.u.key.raw == 146)
				return 0;
		}
	}

	return 0;
}
