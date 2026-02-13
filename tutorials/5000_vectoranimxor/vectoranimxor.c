// 5000 VectorAnimXor - Wireframe vector animation (clear-and-redraw)
// Ported from OscarTutorials to F256K using f256lib
//
// The C64 original uses XOR drawing for flicker-free erase.
// F256K 8bpp bitmap has no XOR mode, so we erase by redrawing
// the previous frame's lines in the background color.

#include "f256lib.h"

struct Point
{
	int x, y;
};

// Precomputed sine table, 8.8 fixed point
static const int SinTab[256] = {
	   0,    6,   12,   18,   25,   31,   37,   43,
	  49,   56,   62,   68,   74,   80,   86,   92,
	  97,  103,  109,  115,  120,  126,  131,  136,
	 142,  147,  152,  157,  162,  167,  171,  176,
	 181,  185,  189,  193,  197,  201,  205,  209,
	 212,  216,  219,  222,  225,  228,  231,  234,
	 236,  238,  241,  243,  244,  246,  248,  249,
	 251,  252,  253,  254,  254,  255,  255,  255,
	 256,  255,  255,  255,  254,  254,  253,  252,
	 251,  249,  248,  246,  244,  243,  241,  238,
	 236,  234,  231,  228,  225,  222,  219,  216,
	 212,  209,  205,  201,  197,  193,  189,  185,
	 181,  176,  171,  167,  162,  157,  152,  147,
	 142,  136,  131,  126,  120,  115,  109,  103,
	  97,   92,   86,   80,   74,   68,   62,   56,
	  49,   43,   37,   31,   25,   18,   12,    6,
	   0,   -6,  -12,  -18,  -25,  -31,  -37,  -43,
	 -49,  -56,  -62,  -68,  -74,  -80,  -86,  -92,
	 -97, -103, -109, -115, -120, -126, -131, -136,
	-142, -147, -152, -157, -162, -167, -171, -176,
	-181, -185, -189, -193, -197, -201, -205, -209,
	-212, -216, -219, -222, -225, -228, -231, -234,
	-236, -238, -241, -243, -244, -246, -248, -249,
	-251, -252, -253, -254, -254, -255, -255, -255,
	-256, -255, -255, -255, -254, -254, -253, -252,
	-251, -249, -248, -246, -244, -243, -241, -238,
	-236, -234, -231, -228, -225, -222, -219, -216,
	-212, -209, -205, -201, -197, -193, -189, -185,
	-181, -176, -171, -167, -162, -157, -152, -147,
	-142, -136, -131, -126, -120, -115, -109, -103,
	 -97,  -92,  -86,  -80,  -74,  -68,  -62,  -56,
	 -49,  -43,  -37,  -31,  -25,  -18,  -12,   -6,
};

// Draw polygon edges
void drawCorners(const struct Point *c, int n)
{
	byte j = n - 1;
	for (byte i = 0; i < n; i++)
	{
		bitmapLine(c[j].x, c[j].y, c[i].x, c[i].y);
		j = i;
	}
}

// 8.8 fixed-point signed multiply
static int lmul8f8s(int a, int b)
{
	return (int)(mathSignedMultiply(a, b) >> 8);
}

// Calculate star vertices
void calcStar(struct Point *c, int n, byte o, byte s)
{
	for (byte i = 0; i < n; i++)
	{
		int x = lmul8f8s(SinTab[o], s);
		int y = lmul8f8s(SinTab[(o + 64) & 255], s);

		if (i & 1)
		{
			x >>= 1;
			y >>= 1;
		}

		// Center on F256K screen (320x240)
		c[i].x = x + 160;
		c[i].y = y + 120;

		// Clamp to screen bounds (bitmapLine has no clipping)
		if (c[i].x < 0) c[i].x = 0;
		if (c[i].x > 319) c[i].x = 319;
		if (c[i].y < 0) c[i].y = 0;
		if (c[i].y > 239) c[i].y = 239;

		o = (o + 26) & 255;
	}
}

int main(int argc, char *argv[])
{
	// Enable bitmap mode
	POKE(VKY_MSTR_CTRL_0, PEEK(VKY_MSTR_CTRL_0) | VKY_GRAPH | VKY_BITMAP);
	bitmapSetVisible(0, true);

	graphicsDefineColor(0, 0, 0, 0, 0);       // black
	graphicsDefineColor(0, 1, 255, 255, 255);  // white

	graphicsSetBorderC64Color(1);
	bitmapSetActive(0);
	bitmapSetColor(0);
	bitmapClear();

	struct Point c[10];

	// Initialize with first star (size 0, all vertices at center)
	calcStar(c, 10, 0, 0);

	for (;;)
	{
		for (int i = 0; i < 256; i++)
		{
			graphicsWaitVerticalBlank();

			// Erase previous star and draw new one right after
			// vblank, so both complete before the raster scans
			bitmapSetColor(0);
			drawCorners(c, 10);

			calcStar(c, 10, (byte)i, (byte)i);
			bitmapSetColor(1);
			drawCorners(c, 10);
		}
	}

	return 0;
}
