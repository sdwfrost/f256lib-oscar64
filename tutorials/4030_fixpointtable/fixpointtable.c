// 4030 FixPointTable - Table-based fixed-point multiplication
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdio.h>

#define PI 3.14159265

static const int FBITS = 12;
#define FCONST(x) (int)((x) * (1 << FBITS) + 0.5)
#define FFLOAT(x) ((float)(x) / (1 << FBITS))

unsigned sqb[256];

inline unsigned bmul(char x, char y)
{
	unsigned x2 = sqb[x], y2 = sqb[y];
	if (x > y)
		return ((unsigned long)(x2 - sqb[x - y]) + (unsigned long)y2) >> 1;
	else
		return ((unsigned long)(y2 - sqb[y - x]) + (unsigned long)x2) >> 1;
}

unsigned lmul(unsigned x, unsigned y)
{
	unsigned lxy = bmul(x & 0xff, y & 0xff);
	unsigned mxy = bmul(x & 0xff, y >> 8);
	unsigned myx = bmul(x >> 8, y & 0xff);
	unsigned hxy = bmul(x >> 8, y >> 8);
	return ((unsigned long)lxy + ((unsigned long)mxy << 8) + ((unsigned long)myx << 8) + ((unsigned long)hxy << 16)) >> FBITS;
}

int lsmul(int x, int y)
{
	if (x < 0)
	{
		if (y < 0) return (int)lmul(-x, -y);
		else return -(int)lmul(-x, y);
	}
	else if (y < 0) return -(int)lmul(x, -y);
	else return (int)lmul(x, y);
}

float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(int argc, char *argv[])
{
	textClear();
	textPrint("FixPointTable - square table circle test\n\n");

	for (unsigned i = 0; i < 256; i++)
		sqb[i] = i * i;

	int fx = FCONST(1.0), fy = FCONST(0.0);
	static const int N = 200;
	static const int ds = FCONST(2.0 * PI / N);

	unsigned int t0 = 0;
	for (int i = 0; i < N; i++)
	{
		int dx = lsmul(ds, -fy), dy = lsmul(ds, fx);
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
