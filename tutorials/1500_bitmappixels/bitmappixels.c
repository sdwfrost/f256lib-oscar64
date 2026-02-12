// 1500 BitmapPixels - Lissajous curve using bitmap mode
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <math.h>

int main(int argc, char *argv[])
{
	// Enable bitmap mode
	POKE(VKY_MSTR_CTRL_0, PEEK(VKY_MSTR_CTRL_0) | VKY_GRAPH | VKY_BITMAP);
	bitmapSetVisible(0, true);

	// Set up a simple grayscale CLUT for plotting
	graphicsDefineColor(0, 0, 0, 0, 0);       // black background
	graphicsDefineColor(0, 1, 255, 255, 255);  // white pixel

	graphicsSetBorderC64Color(1);
	bitmapSetActive(0);
	bitmapSetColor(0);
	bitmapClear();
	bitmapSetColor(1);

	float w = 0.0;
	for (;;)
	{
		// Some fancy curves
		int x = 160 + cos(w) * 80 + cos(w * 5) * 20 + cos(w * 13) * 10;
		int y = 120 + sin(w * 2) * 80 + sin(w * 7) * 20 + sin(w * 11) * 10;

		bitmapPutPixel((uint16_t)x, (uint16_t)y);

		w += 0.01;
	}

	return 0;
}
