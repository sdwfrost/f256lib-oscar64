//DEFINES

#define TIMER_SPRANIM_COOKIE 0
#define TIMER_DSHANIM_COOKIE 10
#define TIMER_PAD_COOKIE 99

#define TIMER_LILPAUSE_COOKIE 100
#define TIMER_PAD_DELAY 2





#define CHUNK8K 0x2000
#define CHUNK4K 0x1000
#define CHUNK2K 0x0800
#define CHUNK1K 0x0400
#define CHUNK128B 0x80
#define CHUNK64B 0x40
#define CHUNK32B 0x20


#define SPEED_BASE 1
#define DASH_DISPLACE 24

#define PAL_BASE         0x10000

//INCLUDES
#include "f256lib.h"
// TODO: Port needed - original used #include "../src/muVS1053b.h"
// f256lib has pad functions (padsPollNES, padsPollSNES)
#include "musprite.h"
#include <stdlib.h>

// TODO: EMBED - original: EMBED(palgrudge, "../assets/grudge.pal", 0x10000);//1kb
// TODO: EMBED - original: EMBED(boy1, "../assets/boy1.bin",   0x10400);//1kb
// TODO: EMBED - original: EMBED(thing, "../assets/thing.bin", 0x10800);//1kb
// TODO: EMBED - original: EMBED(girl1, "../assets/girl1.bin", 0x10C00);//1kb
// TODO: EMBED - original: EMBED(cath, "../assets/cath.bin",   0x11000);//1kb
// TODO: EMBED - original: EMBED(cat, "../assets/cat.bin",     0x11400);//1kb
// TODO: EMBED - original: EMBED(dash, "../assets/dash.bin",   0x13000); //2kb


//FUNCTION PROTOTYPES
void setup(void);
void updateFrames(void);
void checkPads(uint8_t, bool);
void kernelPause(uint8_t);

//GLOBALS

struct timer_t padTimer, lilPauseTimer; //delay for pad timer updating

//updates all shown frame graphics to the current state of all sprites
void updateFrames()
{
	uint8_t i = 0;
	uint32_t offset;
	for(i=0;i<5;i++)
	{
		offset = (uint32_t)SPR_SIZE_SQUARED * (uint32_t) spriteStatuses[i].frame;
		mySetSpriteAddr(i,spriteStatuses[i].addr + offset);
		spriteStatuses[i].x +=  spriteStatuses[i].sx;
		spriteStatuses[i].y +=  spriteStatuses[i].sy;
		spriteSetPosition(i,spriteStatuses[i].x, spriteStatuses[i].y);
	}
	for(i=10;i<15;i++)
	{
		offset = (uint32_t)SPR_SIZE_SQUARED * (uint32_t) spriteStatuses[i].frame;
		mySetSpriteAddr(i,spriteStatuses[i].addr + offset);
		spriteSetPosition(i,spriteStatuses[i].x, spriteStatuses[i].y);
	}
}


void setup()
{
	uint32_t c;

	POKE(MMU_IO_CTRL, 0x00);
	// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
	POKE(VKY_MSTR_CTRL_0, 0b00101111); //sprite,graph,overlay,text
	// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
	POKE(VKY_MSTR_CTRL_1, 0b00000100); //font overlay, double height text, 320x240 at 60 Hz;
	POKE(VKY_LAYER_CTRL_0, 0b00000001); //bitmap 1 in layer 0, bitmap 0 in layer 1
	POKE(VKY_LAYER_CTRL_1, 0b00000010); //bitmap 2 in layer 2
	POKE(0xD00D,0xBB); //force dark gray graphics background
	POKE(0xD00E,0xBB);
	POKE(0xD00F,0xBB);
	POKE(MMU_IO_CTRL,1);  //MMU I/O to page 1
	// Set up CLUT0.
	for(c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(PAL_BASE+c));
	}


	POKE(MMU_IO_CTRL,0);

	bitmapSetVisible(0,false);
	bitmapSetVisible(1,false);
	bitmapSetVisible(2,false);

	mySpriteDefine(0, SPR_BOY1_BASE, SPR_SIZE, 0, 0, 0, 100, 50, true, 0);
	mySpriteDefine(1, SPR_THNG_BASE, SPR_SIZE, 0, 0, 2, 200, 50, true, 0);
	mySpriteDefine(2, SPR_GLR1_BASE, SPR_SIZE, 0, 0, 0, 100, 80, true, 0);
	mySpriteDefine(3, SPR_CATH_BASE, SPR_SIZE, 0, 0, 2, 200, 80, true, 0);
	mySpriteDefine(4, SPR_CAT_BASE,  SPR_SIZE, 0, 0, 0, 100, 110, true, 0);

	mySpriteDefine(10, SPR_DASH_BASE, SPR_SIZE, 0, 0, 0, 0, 0, false, 1);
	mySpriteDefine(11, SPR_DASH_BASE, SPR_SIZE, 0, 0, 0, 0, 0, false, 1);
	mySpriteDefine(12, SPR_DASH_BASE, SPR_SIZE, 0, 0, 0, 0, 0, false, 1);
	mySpriteDefine(13, SPR_DASH_BASE, SPR_SIZE, 0, 0, 0, 0, 0, false, 1);
	mySpriteDefine(14, SPR_DASH_BASE, SPR_SIZE, 0, 0, 0, 0, 0, false, 1);

	spriteStatuses[0].state = 0;
	spriteStatuses[0].rightOrLeft = true;
	spriteStatuses[1].state = 1;
	spriteStatuses[1].rightOrLeft = false;
	spriteStatuses[2].state = 0;

	spriteStatuses[2].rightOrLeft = true;
	spriteStatuses[3].state = 1;
	spriteStatuses[3].rightOrLeft = false;
	spriteStatuses[4].state = 0;
	spriteStatuses[4].rightOrLeft = true;

	updateFrames();

	padTimer.units = TIMER_FRAMES;
	padTimer.cookie = TIMER_PAD_COOKIE;


	// openAllCODEC() inlined:
	POKE(0xD620, 0x1F); POKE(0xD621, 0x2A); POKE(0xD622, 0x01); while(PEEK(0xD622) & 0x01);
	vs1053bBoostClock(); //VS1053b proper speed
	vs1053bInitBigPatch();
}

void checkPads(uint8_t whichPad, bool nesOrSnes)
{

	uint16_t addr;
	bool rightOrLeft, bothAxis=false;
	uint8_t rightBButton;

	if(nesOrSnes==true) addr = PAD0 + (uint16_t)whichPad*(uint16_t)2;
	else addr = PAD0 + (uint16_t)(whichPad-4) * (uint16_t)2;

	if((PEEK(addr)&NES_LEFT)==0)
	{
		spriteStatuses[whichPad].sx=-SPEED_BASE;
		spriteStatuses[whichPad].state = SPR_STATE_WALK_L;
		spriteStatuses[whichPad].rightOrLeft = false;
	}
	else if((PEEK(addr)&NES_RIGHT)==0)
	{
		spriteStatuses[whichPad].sx=SPEED_BASE;
		spriteStatuses[whichPad].state = SPR_STATE_WALK_R;
		spriteStatuses[whichPad].rightOrLeft = true;
	}
	else
	{
		spriteStatuses[whichPad].state = spriteStatuses[whichPad].rightOrLeft?SPR_STATE_IDLE_R:SPR_STATE_IDLE_L;
		spriteStatuses[whichPad].sx=0;
	}
	if((PEEK(addr)&NES_UP)==0)
	{
		spriteStatuses[whichPad].sy=-SPEED_BASE;
		spriteStatuses[whichPad].state = spriteStatuses[whichPad].rightOrLeft?SPR_STATE_WALK_R:SPR_STATE_WALK_L;
	}
	else if((PEEK(addr)&NES_DOWN)==0)
	{

		spriteStatuses[whichPad].sy=SPEED_BASE;
		spriteStatuses[whichPad].state = spriteStatuses[whichPad].rightOrLeft?SPR_STATE_WALK_R:SPR_STATE_WALK_L;
	}
	else
	{
		spriteStatuses[whichPad].sy=0;
	}
	if(spriteStatuses[whichPad].sx == 0 && spriteStatuses[whichPad].sy == 0)
		spriteStatuses[whichPad].state = spriteStatuses[whichPad].rightOrLeft?SPR_STATE_IDLE_R:SPR_STATE_IDLE_L;

	rightBButton = nesOrSnes? NES_B: SNES_B;
	if((PEEK(addr)&rightBButton)==0)
	{

		if(spriteStatuses[whichPad].sx != 0 && spriteStatuses[whichPad].sy !=0) bothAxis = true;
		if(spriteStatuses[whichPad].isDashing || (spriteStatuses[whichPad].sx==0&& spriteStatuses[whichPad].sy==0)) return; //dashing forfeits control
	    spriteStatuses[whichPad].isDashing = true;
		rightOrLeft = spriteStatuses[whichPad].rightOrLeft;

		//set the character in a dashing state
		spriteStatuses[whichPad].state = rightOrLeft?4:5;

		//set the action dust cloud
		spriteStatuses[whichPad+10].x = spriteStatuses[whichPad].x;
		spriteStatuses[whichPad+10].y = (uint16_t)spriteStatuses[whichPad].y+(uint16_t)1;
		spriteStatuses[whichPad+10].state = rightOrLeft? 0:1;
		spriteStatuses[whichPad+10].frame = rightOrLeft? 0:4;
		//spriteSetPosition(whichPad+10,spriteStatuses[whichPad+10].x, spriteStatuses[whichPad+10].y);

		spriteSetPosition(whichPad+10,spriteStatuses[whichPad+10].x,spriteStatuses[whichPad+10].y);
		spriteSetVisible(whichPad+10,true);
		spriteStatuses[whichPad+10].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + dashDelay;
		kernelSetTimer(&(spriteStatuses[whichPad+10].timer));

		spriteStatuses[whichPad].x += (bothAxis?17:DASH_DISPLACE) * spriteStatuses[whichPad].sx;
		spriteStatuses[whichPad].y += (bothAxis?17:DASH_DISPLACE) * spriteStatuses[whichPad].sy;
		spriteStatuses[whichPad].sx = 0;
		spriteStatuses[whichPad].sy = 0;
		spriteStatuses[whichPad].frame = rightOrLeft? 1:3;
		spriteStatuses[whichPad].state = rightOrLeft? 4:5;

		textGotoXY(0,0);textSetColor(4,0);
		printf("pad=%d x=%d y=%d state=%d frame=%d\n",whichPad,spriteStatuses[whichPad+10].x,spriteStatuses[whichPad+10].y, spriteStatuses[whichPad+10].state,spriteStatuses[whichPad+10].frame);
	}
}
int main(int argc, char *argv[]) {
	bool isDone = false;
	uint8_t state = 0, i=0;
	setup();

	//kick off a walking state
	spriteStatuses[0].state = 0; //idle right
	spriteStatuses[1].state = 2; //idle left
	spriteStatuses[2].state = 0; //idle right
	spriteStatuses[3].state = 2; //idle left
	spriteStatuses[4].state = 0; //idle right

	spriteStatuses[10].state = 0; //ejecting to left

	spriteStatuses[0].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + stateDelays[spriteStatuses[0].state];
	kernelSetTimer(&(spriteStatuses[0].timer));
	spriteStatuses[1].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + stateDelays[spriteStatuses[1].state];
	kernelSetTimer(&(spriteStatuses[1].timer));
	spriteStatuses[2].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + stateDelays[spriteStatuses[2].state];
	kernelSetTimer(&(spriteStatuses[2].timer));
	spriteStatuses[3].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + stateDelays[spriteStatuses[3].state];
	kernelSetTimer(&(spriteStatuses[3].timer));
	spriteStatuses[4].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + stateDelays[spriteStatuses[4].state];
	kernelSetTimer(&(spriteStatuses[4].timer));


	padTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_PAD_DELAY;
	kernelSetTimer(&padTimer);

	while(!isDone)
		{
		kernelNextEvent();
		if(kernelEventData.type == kernelEvent(timer.EXPIRED))
			{
				switch(kernelEventData.u.timer.cookie)
				{
					case TIMER_PAD_COOKIE:
					    padsPollNES();
						padsPollWait();
						for(i=0;i<4;i++)
							{
								checkPads(i, true); //nes mode
							}

						//perform the trig
						padsPollSNES();
						padsPollWait();
									;
						for(i=4;i<8;i++)
						{
								checkPads(i, false); //snes mode
						}

						//relaunch pad timer
						padTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_PAD_DELAY;
						kernelSetTimer(&padTimer);
						break;
					case 0 ... 9: //player animations to advance
						state = spriteStatuses[kernelEventData.u.timer.cookie].state;  //checks the state that it is in
						if(state == 4 || state == 5)
						{
							state = spriteStatuses[kernelEventData.u.timer.cookie].rightOrLeft ? 0:1;
							spriteStatuses[kernelEventData.u.timer.cookie].state = state;
							spriteStatuses[kernelEventData.u.timer.cookie].frame = state? 0:2;
						}
						spriteStatuses[kernelEventData.u.timer.cookie].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) +
																		stateDelays[spriteStatuses[kernelEventData.u.timer.cookie].state];
						kernelSetTimer(&(spriteStatuses[kernelEventData.u.timer.cookie].timer));

						//go to the next frame of the state
						spriteStatuses[kernelEventData.u.timer.cookie].frame++;
						//if we go over the last frame index of the state, go back to the beginning
						if(spriteStatuses[kernelEventData.u.timer.cookie].frame > spriteStatuses[kernelEventData.u.timer.cookie].maxIndexForState[state])
							spriteStatuses[kernelEventData.u.timer.cookie].frame = spriteStatuses[kernelEventData.u.timer.cookie].minIndexForState[state];

						updateFrames();
						break;
					case 10 ... 19: //dash animations to advance
						state = spriteStatuses[kernelEventData.u.timer.cookie].state;  //checks the state that it is in, either 0 or 1

						//go to the next frame of the state
						spriteStatuses[kernelEventData.u.timer.cookie].frame++;
						if(spriteStatuses[kernelEventData.u.timer.cookie].frame > spriteStatuses[kernelEventData.u.timer.cookie].maxIndexForState[state])
							{
								spriteStatuses[kernelEventData.u.timer.cookie].frame--;
							spriteSetVisible(kernelEventData.u.timer.cookie, false);
							spriteStatuses[kernelEventData.u.timer.cookie-10].isDashing = false;
							break;
							}
						else
						{
							spriteStatuses[kernelEventData.u.timer.cookie].timer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + dashDelay;
							kernelSetTimer(&(spriteStatuses[kernelEventData.u.timer.cookie].timer));
						}
						break;
				}
			}
			if(kernelEventData.type == kernelEvent(key.PRESSED))
				{
					if(kernelEventData.u.key.raw == 146) //1
						return 0;
				}
		}
	return 0;
}
