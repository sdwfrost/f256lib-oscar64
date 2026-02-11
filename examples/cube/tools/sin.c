#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define FIX_PREC 9
#define TO_FIX(x) ((int32_t)((x)*(1<<FIX_PREC)))

#define SIN_SIZE 512
#define COS_OFF 128


int main(void) {
	int i;
	double v;
	for(i = 0; i < SIN_SIZE + COS_OFF; ++i) {
		v = sin(2.0 * M_PI * i / (double)SIN_SIZE);
		printf("%d, ", TO_FIX( v ));
	}
	printf("\n");
	return 0;
}

