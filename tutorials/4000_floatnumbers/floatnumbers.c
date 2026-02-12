// 4000 FloatNumbers - Floating-point circle test
// Ported from OscarTutorials to F256K using f256lib

#include "f256lib.h"
#include <stdio.h>

float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(int argc, char *argv[])
{
	textClear();
	float fx = 1.0, fy = 0.0;
	int N = 200;
	float ds = 0.031415926;

	for (int i = 0; i < N; i++)
	{
		float dx = ds * (0.0 - fy);
		float dy = ds * fx;
		fx = fx + dx;
		fy = fy + dy;
	}

	char buf[40];
	sprintf(buf, "V: %.3 %.3", fx, fy);
	textPrint(buf);
	sprintf(buf, "  E: %.3\n", ferror(fx, fy));
	textPrint(buf);

	return 0;
}
