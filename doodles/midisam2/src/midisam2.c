
#include "f256lib.h"
// TODO: Port needed for setup (../src/setup.h)
#include "f_midi.h"  //contains basic MIDI functions
// muMidiPlay2 functionality is provided by f256lib (f_midiplay.h)
// muVS1053b functionality is provided by f256lib (f_vs1053b.h)
// muFilePicker functionality is provided by f256lib (f_filepicker.h)
// muTimer0Int functionality is provided by f256lib (f_timer0.h)
// TODO: Port needed for muTextUI
// TODO: Port needed for mulcd

#define MIDI_PARSED 0x50000 //end of ram is 0x7FFFF, gives a nice 256kb of parsed midi

#define MUSIC_BASE 0x50000

#define TIMER_PLAYBACK_COOKIE 0
#define TIMER_DELAY_COOKIE 1
#define TIMER_SHIMMER_COOKIE 2
#define TIMER_SHIMMER_DELAY 3

#define CLONE_TO_UART_EMWHITE 0
#define ACTIVITY_CHAN_X 4
#define MENU_Y 29
// Embed cozyLCD image at 0x30000 (134400 bytes = 0x20D00)
#pragma section( cozylcddata, 0)
#pragma region( cozylcddata, 0x30000, 0x51000, , , {cozylcddata} )
#pragma data(cozylcddata)
__export const char cozyLCD[] = {
	#embed "../assets/cozyLCD"
};
#pragma data(data)

// Embed MIDI instruments at 0x20000 (2688 bytes = 0xA80)
#pragma section( midiinstdata, 0)
#pragma region( midiinstdata, 0x20000, 0x21000, , , {midiinstdata} )
#pragma data(midiinstdata)
__export const char midiinst[] = {
	#embed "../assets/midi_instruments.bin"
};
#pragma data(data)

//STRUCTS
struct timer_t playbackTimer, shimmerTimer;
struct time_t time_data;
bool isPaused = false;
bool isTrulyDone = false;
bool isLightShow = false; //to enable case RGB lights

struct midiRecord myRecord;

FILE *theMIDIfile;
//filePickRecord fpr;

bool repeatFlag = false;
//PROTOTYPES
short optimizedMIDIShimmering(void);
void zeroOutStuff(void);
void wipeShimmer(void);

char currentFilePicked[32];
uint8_t shimmerBuffer[16];

uint16_t disp[16] = {0xD6B3, 0xD6B4, 0xD6AD, 0xD6AE, 0xD6AF, 0xD6AA,
                     0xD6AB, 0xD6AC, 0xD6A7, 0xD6A9, 0xD6B3, 0xD6B4,
                     0xD6AD, 0xD6AE, 0xD6AF, 0xD6AA};

void wipeText(void) {
	uint8_t keep = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for(uint8_t j=0; j<60; j++)
		for(uint8_t i=0;i<80;i++)
			POKE(TEXT_MATRIX + i + j*80, 32);
	POKE(MMU_IO_CTRL, keep);
}

void setColors(void) {
	uint8_t backup, i;
	backup = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, 0);
	POKE(0xD800, 0xFF); POKE(0xD801, 0xFF); POKE(0xD802, 0xFF); POKE(0xD803, 0);
	for(i=1;i<6;i++) {
		POKE(0xD800+4*i, 0); POKE(0xD800+4*i+1, 0xCD+(i-1)*10); POKE(0xD800+4*i+2, 0xCD+(i-1)*10); POKE(0xD800+4*i+3, 0);
	}
	for(i=6;i<11;i++) {
		POKE(0xD800+4*i, 0); POKE(0xD800+4*i+1, (0xCD+(i-6)*10)/2); POKE(0xD800+4*i+2, 0xCD+(i-6)*10); POKE(0xD800+4*i+3, 0);
	}
	for(i=11;i<16;i++) {
		POKE(0xD800+4*i, 0); POKE(0xD800+4*i+1, 0); POKE(0xD800+4*i+2, 0xCD+(i-11)*10); POKE(0xD800+4*i+3, 0);
	}
	POKE(MMU_IO_CTRL, backup);
}

void wipeStatus(void) {
	textGotoXY(0, MENU_Y);
	for(uint8_t i=0;i<80;i++) f256putchar(32);
}

void initProgress(void) {
	textSetColor(15,0); textGotoXY(15,5); textPrint("[");
	for(uint8_t i=0;i<51;i++) textPrint("_");
	textPrint("]");
}

void displayInfo(struct midiRecord *rec) {
	textGotoXY(1,1);
	textSetColor(1,0); textPrint("Filename: ");
	textSetColor(0,0); printf("%s", currentFilePicked);
	textGotoXY(1,2);
	textSetColor(1,0); textPrint("Type "); textSetColor(0,0); printf(" %d ", rec->format);
	textSetColor(1,0); textPrint("MIDI file with ");
	textSetColor(0,0); printf("%d ", rec->trackcount);
	textSetColor(1,0); (rec->trackcount)>1 ? textPrint("tracks") : textPrint("track");
	textSetColor(0,0); textGotoXY(1,7); textPrint("CH   Instrument");
	for(uint8_t i=0;i<16;i++) {
		textGotoXY(1,8+i); printf("%02d ",i);
	}
	textGotoXY(0,25); printf(" ->Currently parsing file %s...", currentFilePicked);
}

void superExtraInfo(struct midiRecord *rec, uint8_t mChip) {
	uint16_t temp;
	temp = (uint16_t)((rec->totalDuration)/125000);
	temp = (uint16_t)(((float)temp)/((float)(rec->fudge)));
	rec->totalSec = temp;
	textSetColor(1,0); textGotoXY(1,MENU_Y); textPrint("[ESC]:");
	textSetColor(0,0); textPrint("quit ");
	textSetColor(1,0); textPrint("[SPACE]:");
	textSetColor(0,0); textPrint("pause ");
	textSetColor(1,0); textPrint("[F1]:");
	textSetColor(0,0); textPrint("Light Show ");
	textSetColor(1,0); textPrint("[F3]:");
	textSetColor(0,0); textPrint("Load ");
	textSetColor(1,0); textPrint("[F5]:");
	textSetColor(0,0); textPrint("SAM2695 VS1053b");
	textGotoXY(1,26); textPrint("  [r] toggle repeat when done");
	textSetColor(1,0);
	if(mChip==0) { textGotoXY(57,MENU_Y); textPrint("SAM2695"); }
	else { textGotoXY(65,MENU_Y); textPrint("VS1053b"); }
}

//FUNCTIONS
void K2LCD()
{
if(platformHasCaseLCD())lcdDisplayImage(0x30000, 2);
kernelPause(1);
}

void wipeShimmer()
{
	for(uint8_t i=0;i<16;i++) //channels
		{
		shimmerBuffer[i]=21;
		textGotoXY(ACTIVITY_CHAN_X,8+i);
		for(uint8_t j=0;j<40;j++)  f256putchar(32);
		}
}
short optimizedMIDIShimmering() {

	if(kernelEventData.type == kernelEvent(timer.EXPIRED))
	{
		switch(kernelEventData.u.timer.cookie)
		{
			case TIMER_SHIMMER_COOKIE:

				for(uint8_t i=0;i<16;i++) //channels
					{
					if(shimmerBuffer[i]==0)continue;
					if(shimmerBuffer[i]==14) {
						textGotoXY(ACTIVITY_CHAN_X,8+i);
						shimmerBuffer[i]=0;
						f256putchar(32);

						if(isLightShow)POKE(disp[i],0);
						continue;
					}
					textSetColor(i+1,0);
					textGotoXY(ACTIVITY_CHAN_X,8+i);
					f256putchar(shimmerBuffer[i]);
					if(isLightShow)POKE(disp[i],1<<(shimmerBuffer[i]-14));
					shimmerBuffer[i]--;
					}

				shimmerTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES)+TIMER_SHIMMER_DELAY;
				kernelSetTimer(&shimmerTimer);
				break;
		}
	}

	if(kernelEventData.type == kernelEvent(key.PRESSED))
		{
			if(kernelEventData.u.key.raw == 0x83) //F3
				{
					if(isPaused==false)
					{
						midiShutAllChannels(true);
						midiShutAllChannels(false);
						isPaused = true;
						textSetColor(1,0);textGotoXY(20,MENU_Y);textPrint("pause");
					}
					destroyTrack();
					zeroOutStuff();
					wipeText();
					if(getTheFile_far(name, 1, 6, "mid", "", "", "")!=0) return 0;

					setColors();
					textGotoXY(0,25);printf("->Currently Loading file %s...",name);


					loadSMFile(name, MUSIC_BASE);


					detectStructure(0, &myRecord);
					initTrack(MUSIC_BASE);
					wipeText();
					initProgress();
					displayInfo(&myRecord);
					superExtraInfo(&myRecord,midiChip);
					isPaused = false;
					textSetColor(0,0);textGotoXY(20,MENU_Y);textPrint("pause");
					shimmerTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES)+TIMER_SHIMMER_DELAY;
					wipeShimmer();
					kernelSetTimer(&shimmerTimer);

					if(isLightShow)
					{textSetColor(1,0);
					textGotoXY(31,MENU_Y);textPrint("Light Show");
					}
				}
			if(kernelEventData.u.key.raw == 146) //esc
				{
				midiShutAllChannels(midiChip);
				isTrulyDone = true;
				return 1;
				}
			if(kernelEventData.u.key.raw == 32) //space
			{
				if(isPaused==false)
				{
					midiShutAllChannels(midiChip);
					isPaused = true;
					textSetColor(1,0);textGotoXY(20,MENU_Y);textPrint("pause");
				}
				else
				{
					isPaused = false;
					textSetColor(0,0);textGotoXY(20,MENU_Y);textPrint("pause");
				}
			}
			if(kernelEventData.u.key.raw == 129) //F1
			{
				if(isLightShow==true)
					{
					textSetColor(0,0);
					textGotoXY(31,MENU_Y);textPrint("Light Show");
					isLightShow = false;

					POKE(0xD6A0,PEEK(0xD6A0) & 0x9C);//turn off
					}
				else
					{
					textSetColor(1,0);
					textGotoXY(31,MENU_Y);textPrint("Light Show");
					isLightShow = true;

					POKE(0xD6A0,PEEK(0xD6A0) | 0x63); //turn on
					}


			}
			if(kernelEventData.u.key.raw == 0x85) //F5
				{
				midiShutAllChannels(midiChip);
				if(midiChip==true)
					{
					textSetColor(1,0);textGotoXY(57,MENU_Y);textPrint("SAM2695");
					textSetColor(0,0);textGotoXY(65,MENU_Y);textPrint("VS1053b");
					midiChip = false;
					}
				else
					{
					textSetColor(0,0);textGotoXY(57,MENU_Y);textPrint("SAM2695");
					textSetColor(1,0);textGotoXY(65,MENU_Y);textPrint("VS1053b");
					midiChip = true;
					}
				}
			if(kernelEventData.u.key.raw == 0x72) //r
				{
				repeatFlag = !repeatFlag;
				if(repeatFlag)
				{
				textSetColor(1,0);textGotoXY(3,26);textPrint("[r]");
				}
				else {
				textSetColor(0,0);textGotoXY(3,26);textPrint("[r]");
					}
				}
			if(kernelEventData.u.key.raw == 0x2E) //.
			{
			for(uint8_t i=0;i<16;i++)
					{
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0xB0 | i); // control change message
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x5B); // reverb
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x00);

						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0xB0 | i); // control change message
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x5D); // chorus
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x00);

					}
			}
			if(kernelEventData.u.key.raw == 0x2C) //,
			{
			for(uint8_t i=0;i<16;i++)
					{
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0xB0 | i); // control change message
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x07); // reverb
						POKE(midiChip?MIDI_FIFO_ALT:MIDI_FIFO, 0x7F);

					}
			}
		//printf("%02x",kernelEventData.u.key.raw);
		} // end if key pressed
return 0;
}

void machineDependent()
{

	uint8_t machineCheck=0;
	//VS1053b
	machineCheck=PEEK(0xD6A7)&0x3F;
	/*
	if(machineCheck == 0x22 || machineCheck == 0x11) //22 is Jr2 and 11 is K2
	{
		*/
	// vs1053bBoostClock() inlined:
	POKE(VS_SCI_ADDR, 0x03); POKEW(VS_SCI_DATA, 0x9800); POKE(VS_SCI_CTRL, 1); POKE(VS_SCI_CTRL, 0); while(PEEK(VS_SCI_CTRL) & 0x80);
	vs1053bInitRTMIDI();
/*
	}

	setupSerial();
*/
}

void zeroOutStuff()
{
	for(uint8_t k=0;k<32;k++) currentFilePicked[k]=0;

	for(uint8_t i=0;i<16;i++)
	{
		for(uint8_t j=0;j<8;j++)
		{
			shimmerBuffer[i]=14;
		}
	}
	if(myRecord.fileName != NULL) free(myRecord.fileName);
	myRecord.fileName = NULL;
	initMidiRecord(&myRecord, MUSIC_BASE, MUSIC_BASE);
}

int main(int argc, char *argv[]) {

	bool cliFile = false; //first time it loads, next times it keeps the cursor position

	K2LCD();

	midiChip = false;
	playbackTimer.units = TIMER_SECONDS;
	playbackTimer.cookie = TIMER_PLAYBACK_COOKIE;

	shimmerTimer.units = TIMER_FRAMES;
	shimmerTimer.cookie = TIMER_SHIMMER_COOKIE;

	//graphicsSetBackgroundRGB(0x2F,0x2F,0x2F);
	POKE(MMU_IO_CTRL, 0x00);
	// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
	POKE(VKY_MSTR_CTRL_0, 0b00000001); //sprite,graph,overlay,text
	// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
	POKE(VKY_MSTR_CTRL_1, 0b00000100); //font overlay, double height text, 320x240 at 60 Hz;

	machineDependent();
	zeroOutStuff();
	setColors();

	/*
	uint8_t v =0;
	for(;;)
	{
		printf("%c %02x\n", argv[1][v],argv[1][v]);
		v++;
		kernelWaitKey();
	}
	*/



	if(argc == 3)
		{
		uint8_t i=0, j=0,m=0;
		char *lastSlash;
		size_t dirLen=0;
		char cliPath[MAX_PATH_LEN];


		while(argv[1][i] != '\0') //copy the directory
			{
				cliPath[i] = argv[1][i];
				i++;
			}
		cliPath[i] = '\0';

		while(argv[1][j] != '\0') //copy the filename
			{

				if(argv[1][j]=='.') m++;
				if(m==1 && (argv[1][j]=='m' || argv[1][j]=='M')) m++;
				else m=0;
				if(m==2 && (argv[1][j]=='i' || argv[1][j]=='I')) m++;
				else m=0;
				if(m==3 && (argv[1][j]=='d' || argv[1][j]=='D')) m++;
				else m=0;
				name[j] = argv[1][j];
				j++;
				if(m==4){name[j] = '\0'; break;}
			}
		lastSlash = strrchr(name, '/');
		dirLen = lastSlash - name; // include '/'

		memset(cliPath, 0, sizeof(cliPath));
		strncpy(cliPath, name, dirLen);
		cliPath[dirLen] = '\0';
		fpr_set_currentPath(cliPath);

		setColors();textGotoXY(0,25);printf("->Currently Loading file %s...",name);
		loadSMFile(name, MUSIC_BASE);
		kernelPause(1);


		//sprintf(name, "%s%s%s", fpr.currentPath,"/", fpr.selectedFile);

		//printf("\nfinal form is: ");
		//printf("%s",name);

	cliFile = true;
		}
	else
		{
	//check if the midi directory is here, if not, target the root
		char *dirOpenResult = fileOpenDir("media/midi");
		if(dirOpenResult != NULL)
			{
			fpr_set_currentPath("media/midi");
			fileCloseDir(dirOpenResult);
			}
		else fpr_set_currentPath("0:");
		}

	if(cliFile == false)
	{

	if(getTheFile_far(name, 1, 6, "mid", "", "", "")!=0) return 0;

	setColors();textGotoXY(0,25);printf("->Currently Loading file %s...",name);
	loadSMFile(name, MUSIC_BASE);
	kernelPause(1);

	}

	wipeText();
	detectStructure(0, &myRecord);
	displayInfo(&myRecord);
	midiResetInstruments(midiChip); //resets all channels to piano, for sam2695
	midiPanic(true); //ends trailing previous notes if any, for sam2695
	midiPanic(false); //ends trailing previous notes if any, for sam2695

	midiShutAllChannels(true);
	midiShutAllChannels(false);

    initTrack(MUSIC_BASE);

//find what to do and exhaust all zero delay events at the start
	for(uint16_t i=0;i<theOne.nbTracks;i++)	exhaustZeroes(i);

	timer0Reset();
	shimmerTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES)+TIMER_SHIMMER_DELAY;
	kernelSetTimer(&shimmerTimer);
	while(!isTrulyDone)
	{
			wipeStatus();
			superExtraInfo(&myRecord, midiChip);

			initProgress();
			setColors();
			for(;;)
				{
				kernelNextEvent();
				if(optimizedMIDIShimmering()) break;
				if(!isPaused)
					{

					if(PEEK(INT_PENDING_0)&0x10) //when the timer0 delay is up, go here
						{
						POKE(INT_PENDING_0,0x10); //clear the timer0 delay
						playMidi(); //play the next chunk of the midi file, might deal with multiple 0 delay stuff
						}

					if(theOne.isWaiting == false)
						{
						sniffNextMIDI(); //find next event to play, will cue up a timer0 delay
						}

					}
				if(theOne.isMasterDone >= theOne.nbTracks)
				{
					if(repeatFlag)
					{

					midiShutAllChannels(true);
					midiShutAllChannels(false);

					destroyTrack();
					zeroOutStuff();

					detectStructure(0, &myRecord);
					initTrack(MUSIC_BASE);
					textSetColor(1,0);textGotoXY(3,26);textPrint("[r]");


					shimmerTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES)+TIMER_SHIMMER_DELAY;
					kernelSetTimer(&shimmerTimer);
					}
					else
					{
						isPaused = true;
						break; //really quit no matter what
					}
				}

				}

	midiShutAllChannels(true);
	midiShutAllChannels(false);
	}
	return 0;}
