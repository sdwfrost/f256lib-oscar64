/*
 *	SID player test - sixpack.sng from beek.
 *	Ported from F256KsimpleCdoodles for oscar64.
 *	Note: EMBED replaced with oscar64 #pragma section/#embed.
 */

#include "f256lib.h"

#include <stdio.h>
#include <stdlib.h>


#define TIMER_SIDPLAY_COOKIE 1

// Embed sixpack.bin at 0xA000 in far memory.
// Adjust the region end address as needed for the actual binary size.
#pragma section( siddata, 0)
#pragma region( siddata, 0xA000, 0xC000, , , {siddata} )
#pragma data(siddata)
__export const char sixpack[] = {
	#embed "../assets/sixpack.bin"
};
#pragma data(data)

typedef void (*mySIDBin)(void);

void setup()
{
POKE(MMU_IO_CTRL, 0x00);

// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
POKE(VKY_MSTR_CTRL_0, 0b00001111); //sprite,graph,overlay,text
// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
POKE(VKY_MSTR_CTRL_1, 0b00000100); //font overlay, double height text, 320x240 at 60 Hz;
POKE(VKY_LAYER_CTRL_0, 0b00010000); //bitmap 0 in layer 0, bitmap 1 in layer 1
POKE(0xD00D,0x00); //force black graphics background
POKE(0xD00E,0x00);
POKE(0xD00F,0x00);

}


int main(int argc, char *argv[]) {
uint8_t exitFlag = 0;
mySIDBin func = (mySIDBin)0xA000;
mySIDBin playb = (mySIDBin)0xA003;
struct timer_t sidTimer;


setup();



printf("Testing out the converted sixpack.sng from beek\n");
kernelWaitKey();

func();
sidTimer.units = TIMER_FRAMES;
sidTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + 1;
sidTimer.cookie = TIMER_SIDPLAY_COOKIE;
kernelSetTimer(&sidTimer);

while(true)
{
	kernelNextEvent();
	if(kernelEventData.type == kernelEvent(timer.EXPIRED))
	{
		if(kernelEventData.u.timer.cookie == TIMER_SIDPLAY_COOKIE)
		{
		playb();
		sidTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + 1;
		kernelSetTimer(&sidTimer);
		}
	}

}

return 0;
}
