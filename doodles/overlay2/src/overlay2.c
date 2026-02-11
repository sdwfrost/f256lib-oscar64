/*
 * overlay2 - Multi-file overlay demo (oscar64 port)
 *
 * Original (llvm-mos): extra.c contained doSomething() in SEGMENT_FIRST.
 * The overlay tool auto-generated a trampoline for cross-file calls.
 *
 * Oscar64 port: extra.c uses #pragma section/region to compile
 * FAR8_doSomething() for 0xA000, with a manual trampoline.
 * initOverlay() copies the far code to block 8 at startup.
 */

#include "f256lib.h"
#include "extra.h"

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// Copy far code from block 5 to block 8
	initOverlay();

	doSomething();
	for (;;);
	return 0;
}
