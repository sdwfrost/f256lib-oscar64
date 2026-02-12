// 4010 FixPointNumbers - Fixed-point circle test
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdio.h>

#define PI 3.14159265

static const int FBITS = 12;
#define FCONST(x) (int)((x) * (1 << FBITS) + 0.5)
#define FMUL(x, y) (int)((long)(x) * (long)(y) >> FBITS)
#define FFLOAT(x) ((float)(x) / (1 << FBITS))

float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(int argc, char *argv[])
{
	textClear();
	int fx = FCONST(1.0), fy = FCONST(0.0);
	static const int N = 200;
	static const int ds = FCONST(2.0 * PI / N);

	for (int i = 0; i < N; i++)
	{
		int dx = FMUL(ds, -fy), dy = FMUL(ds, fx);
		fx += dx;
		fy += dy;
	}

	float ffx = FFLOAT(fx), ffy = FFLOAT(fy);
	char buf[40];
	sprintf(buf, "V: %.3 %.3", ffx, ffy);
	textPrint(buf);
	sprintf(buf, "  E: %.3\n", ferror(ffx, ffy));
	textPrint(buf);

	return 0;
}
