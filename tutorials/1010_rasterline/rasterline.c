// 1010 RasterLine - Raster line color effect
// Ported from OscarTutorials to F256K using f256lib
//
// Changes border color per-frame to show raster timing.

#include "f256lib.h"

int main(int argc, char *argv[])
{
	textClear();
	textPrint("RASTER LINE EFFECT\nPRESS ANY KEY TO STOP\n");

	byte c = 0;
	while (!keyboardHit())
	{
		// Change border color to show frame timing
		graphicsSetBorderC64Color(c & 15);
		graphicsWaitVerticalBlank();
		graphicsSetBorderC64Color(0);
		c++;
	}

	return 0;
}
