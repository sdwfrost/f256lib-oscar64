
#include "f256lib.h"
// f256lib provides lcdDisplayImage via f_lcd module

// TODO: EMBED - original: EMBED(mac, "../assets/wildbits.bin", 0x10000);

int main(int argc, char *argv[]) {

	textGotoXY(1,10);
	if(platformHasCaseLCD() && platformIsAnyK())
	{
    textPrint("K2 detected. custom LCD image loading on the screen case.");
	lcdDisplayImage(0x10000, 1);
	}
	else
	{
    textPrint("K2 is not detected. can't load a custom LCD picture on the screen case.");
	textGotoXY(1,11);
    textPrint("Program stalled. Hit Reset.");
	}
	while(true);
	return 0;
}
