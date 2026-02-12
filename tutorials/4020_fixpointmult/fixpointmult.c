// 4020 FixPointMult - Fixed-point multiplication with lmul4f12s
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdio.h>

#define PI 3.14159265

static const int FBITS = 12;
#define FCONST(x) (int)((x) * (1 << FBITS) + 0.5)
#define FFLOAT(x) ((float)(x) / (1 << FBITS))

static int lmul4f12s(int a, int b)
{
	return (int)(mathSignedMultiply(a, b) >> 12);
}

float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(int argc, char *argv[])
{
	textClear();
	textPrint("FixPointMult - lmul4f12s circle test\n\n");

	int fx = FCONST(1.0), fy = FCONST(0.0);
	static const int N = 200;
	static const int ds = FCONST(2.0 * PI / N);

	unsigned int t0 = 0;
	for (int i = 0; i < N; i++)
	{
		int dx = lmul4f12s(ds, -fy), dy = lmul4f12s(ds, fx);
		fx += dx; fy += dy;
		graphicsWaitVerticalBlank();
		t0++;
	}

	char buf[40];
	sprintf(buf, "V: %.3 %.3", FFLOAT(fx), FFLOAT(fy));
	textPrint(buf);
	sprintf(buf, "\nE: %.3", ferror(FFLOAT(fx), FFLOAT(fy)));
	textPrint(buf);
	textPrint("\nFrames: ");
	textPrintInt(t0);
	textPutChar('\n');

	return 0;
}
