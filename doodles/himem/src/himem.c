/*
 *	High memory (24-bit addressing) test.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Uses f256lib's FAR_PEEK/FAR_POKE instead of llvm-mos inline asm.
 */

#include "f256lib.h"

int main(int argc, char *argv[]) {
	short i = 0;

	char test1[] = "hello";

	printf("\n\n");

	printf("writing at 0x17F FF0\n");
	for (i = 0; i < 5; i++) {
		FAR_POKE(0x17FFF0 + i, test1[i]);
		printf("%c", test1[i]);
	}
	printf("\n");
	for (i = 0; i < 5; i++) printf("%c", FAR_PEEK(0x17FFF0 + i));

	printf("\n\n");
	printf("writing at 0x180 000\n");
	for (i = 0; i < 5; i++) {
		FAR_POKE(0x180000 + i, test1[i]);
		printf("%c", test1[i]);
	}
	printf("\n");
	for (i = 0; i < 5; i++) printf("%c", FAR_PEEK(0x180000 + i));

	printf("\n\n");
	printf("writing at 0x1FF F00\n");
	for (i = 0; i < 5; i++) {
		FAR_POKE(0x1FFF00 + i, test1[i]);
		printf("%c", test1[i]);
	}
	printf("\n");
	for (i = 0; i < 5; i++) printf("%c", FAR_PEEK(0x1FFF00 + i));
	kernelWaitKey();
	printf("yes");

	while (1);

	return 0;
}
