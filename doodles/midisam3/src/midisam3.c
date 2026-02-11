
#include "f256lib.h"
#include "f_sid.h" //sid chip
// f256lib provides: file picker (far-memory API)

#include "f_midi.h"  //contains basic MIDI functions
// muMidiPlay2 functionality is provided by f256lib (f_midiplay.h)
// muTimer0Int functionality is provided by f256lib (f_timer0.h)

#define MIDI_PARSED 0x50000 //end of ram is 0x7FFFF, gives a nice 256kb of parsed midi
#define MUSIC_BASE 0x50000

#define TIMER_PLAYBACK_COOKIE 0
#define TIMER_DELAY_COOKIE 1

#define DIRECTORY_X 1
#define DIRECTORY_Y 6

#define MENU_Y 29

#define textColorOrange 0x09


// ============================================================
// Local variables ported from mudispatch / mumusicmap / muTextUI
// ============================================================

// Mute state per MIDI channel: 0 = not muted, non-zero = muted
uint8_t muteArray[16]  = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
// Which MIDI channels are present (have received notes): 0 = absent, 1 = present
uint8_t presentArray[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
// Index into presentArray for mute-selection cursor
uint8_t presentIndex = 0;

// Maps MIDI channel -> sound chip routing
// High nibble: sub-channel on target chip; Low nibble: 0=SAM, 1=SID, 2=PSG, 3=OPL3, 4=VS
uint8_t chipXChannel[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

// Local SID instrument copies (6 voices: 3 per SID chip x 2 chips)
sidInstrumentT sidInstruments[6];


//STRUCTS
struct timer_t playbackTimer;
struct time_t time_data;
bool isPaused = false;
bool isTrulyDone = false;

struct midiRecord myRecord;

FILE *theMIDIfile;
// file picker uses far-memory API (no struct needed)

bool repeatFlag = false;
//PROTOTYPES
short optimizedMIDIShimmering(void);
void zeroOutStuff(void);
void updateMuteDisplay(uint8_t chan);
void cycleMuteSelection(int8_t upOrDown);
void wipeText(void);
void wipeStatus(void);
void setColors(void);
void initProgress(void);
void displayInfo(struct midiRecord *rec);
void superExtraInfo(struct midiRecord *rec);

char currentFilePicked[32];

short optimizedMIDIShimmering() {

	if(kernelEventData.type == kernelEvent(key.PRESSED))
		{
			if((kernelEventData.u.key.raw == 0x5B || kernelEventData.u.key.raw == 0x5D)  && isPaused ) //left bracket [
				{
					muteArray[presentIndex] = ~muteArray[presentIndex];
					updateMuteDisplay(presentIndex);
				}
			if(kernelEventData.u.key.raw == 0xB6 && isPaused) //up arrow
				{
				cycleMuteSelection(-1);
				}
			if(kernelEventData.u.key.raw == 0xB7 && isPaused) //down arrow
				{
				cycleMuteSelection(1);
				}
			if(kernelEventData.u.key.raw == 0x83) //F3
				{
					if(isPaused==false)
					{
						sidShutAllVoices();
						midiShutAllChannels(false);
						//shutPSG();
						isPaused = true;
						textSetColor(1,0);textGotoXY(26,MENU_Y);textPrint("pause");
					}
					destroyTrack();
					zeroOutStuff();
					wipeText();
					if(getTheFile_far(myRecord.fileName, DIRECTORY_X, DIRECTORY_Y, "mid", "", "", "")!=0) return 0;
					loadSMFile(myRecord.fileName, MUSIC_BASE);
					setColors();

					detectStructure(0, &myRecord);
					initTrack(MUSIC_BASE);
					wipeText();
					initProgress();
					displayInfo(&myRecord);
					superExtraInfo(&myRecord);
					isPaused = false;

					textSetColor(0,0);textGotoXY(26,MENU_Y);textPrint("pause");

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
					sidShutAllVoices();
					//shutPSG();
					isPaused = true;
					textSetColor(1,0);textGotoXY(26,MENU_Y);textPrint("pause");

					cycleMuteSelection(0);  //set the cursor in the first detected channel, for muting control
				}
				else
				{
					isPaused = false;
					textSetColor(0,0);textGotoXY(26,MENU_Y);textPrint("pause");
				}
			}
			if(kernelEventData.u.key.raw == 129) //F1
			{
				midiShutAllChannels(midiChip);
				if(midiChip==true)
					{
					textSetColor(1,0);textGotoXY(42,MENU_Y);textPrint("SID    ");
					textSetColor(0,0);textGotoXY(52,MENU_Y);textPrint("SAM2695");
					midiChip = false;
					}
				else
					{
					textSetColor(0,0);textGotoXY(42,MENU_Y);textPrint("SID    ");
					textSetColor(1,0);textGotoXY(52,MENU_Y);textPrint("SAM2695");
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
		} // end if key pressed
return 0;
}


void zeroOutStuff()
{
	for(uint8_t k=0;k<32;k++) currentFilePicked[k]=0;

	if(myRecord.fileName != NULL) free(myRecord.fileName);
	myRecord.fileName = NULL;
	initMidiRecord(&myRecord, MUSIC_BASE, MUSIC_BASE);
	for(uint8_t ind = 0;ind<16;ind++)
		{
			muteArray[ind] = 0;
			presentArray[ind] = 0;
		}
}


// ============================================================
// UI functions ported from muTextUI.c
// ============================================================

void wipeText(void) {
	uint8_t keep = PEEK(MMU_IO_CTRL);
	POKE(MMU_IO_CTRL, MMU_IO_TEXT);
	for(uint8_t j=0; j<60; j++)
		for(uint8_t i=0;i<80;i++)
			POKE(TEXT_MATRIX + i + j*80, 32);
	POKE(MMU_IO_CTRL, keep);
}

void wipeStatus(void) {
	wipeText();
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

void initProgress(void) {
	textSetColor(15,0); textGotoXY(15,5); textPrint("[");
	for(uint8_t i=0;i<51;i++) textPrint("_");
	textPrint("]");
}

void displayInfo(struct midiRecord *rec) {
	textGotoXY(1,1);
	textSetColor(1,0); textPrint("Filename: ");
	textSetColor(0,0); printf("%s", rec->fileName);
	textGotoXY(1,2);
	textSetColor(1,0); textPrint("Type "); textSetColor(0,0); printf(" %d ", rec->format);
	textSetColor(1,0); textPrint("MIDI file with ");
	textSetColor(0,0); printf("%d ", rec->trackcount);
	textSetColor(1,0); (rec->trackcount)>1 ? textPrint("tracks") : textPrint("track");
	textSetColor(0,0); textGotoXY(1,7); textPrint("CH   Instrument");
	for(uint8_t i=0;i<16;i++) {
		textGotoXY(1,8+i); printf("%02d ",i);
	}
	textGotoXY(0,25); printf(" ->Currently parsing file %s...", rec->fileName);
}

void superExtraInfo(struct midiRecord *rec) {
	uint16_t temp;
	temp = (uint16_t)((rec->totalDuration)/125000);
	temp = (uint16_t)(((float)temp)/((float)(rec->fudge)));
	rec->totalSec = temp;
	textSetColor(1,0); textGotoXY(1,MENU_Y); textPrint("[ESC]: ");
	textSetColor(0,0); textPrint("quit");
	textSetColor(1,0); textPrint("    [SPACE]:");
	textSetColor(0,0); textPrint("  pause    ");
	textSetColor(1,0); textPrint("[F1]   ");
	textSetColor(1,0); textPrint("SAM2695");
	textSetColor(0,0); textPrint("   VS1053b");
	textSetColor(1,0); textGotoXY(62,MENU_Y); textPrint("[F3]:");
	textSetColor(0,0); textPrint("  Load");
	textGotoXY(1,26); textPrint("  [r] toggle repeat when done");
}

void updateMuteDisplay(uint8_t chan)
{
	textGotoXY(4+15,8+chan);textSetColor(0,0);
	if(muteArray[chan])textPrint("Y");
	else textPrint("N");
}

void cycleMuteSelection(int8_t upOrDown) //+1 going down the list, -1 going up the list
{
	uint8_t infiniteGuard = 0;
	uint8_t oldIndex = presentIndex;
	int8_t startLookingHere = presentIndex;

	if(upOrDown == -1)
	{
		startLookingHere--; infiniteGuard++;
		if(startLookingHere<0) startLookingHere=15;
		while(presentArray[startLookingHere] == 0)
		{
			startLookingHere--; infiniteGuard++;
			if(infiniteGuard >16) return;
			if(startLookingHere<0) startLookingHere=15;
		}
		presentIndex = startLookingHere;
	}
	else if(upOrDown ==  1)
	{
		startLookingHere++; infiniteGuard++;
		if(startLookingHere>15) startLookingHere=0;
		while(presentArray[startLookingHere] == 0)
		{
			startLookingHere++; infiniteGuard++;
			if(infiniteGuard >16) return;
			if(startLookingHere>15) startLookingHere=0;
		}
		presentIndex = startLookingHere;
	}
	textGotoXY(4+14,8+oldIndex);printf(" ");
	textGotoXY(4+16,8+oldIndex);printf(" ");

	textSetColor(textColorOrange,0x00);
	textGotoXY(4+14,8+presentIndex);printf("%c",0xFA);
	textGotoXY(4+16,8+presentIndex);printf("%c",0xF9);
	textSetColor(0,0);
}


int main(int argc, char *argv[]) {
	uint16_t i;

	midiChip = false;
	playbackTimer.units = TIMER_SECONDS;
	playbackTimer.cookie = TIMER_PLAYBACK_COOKIE;

	//graphicsSetBackgroundRGB(0x2F,0x2F,0x2F);
	POKE(MMU_IO_CTRL, 0x00);
	// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
	POKE(VKY_MSTR_CTRL_0, 0b00000001); //sprite,graph,overlay,text
	// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
	POKE(VKY_MSTR_CTRL_1, 0b00000100); //font overlay, double height text, 320x240 at 60 Hz;

	zeroOutStuff();
	setColors();

	sidClearRegisters();
	sidSetMono();


	//check if the midi directory is here, if not, target the root
	{
		char *dirOpenResult = fileOpenDir("midi");
		if(dirOpenResult != NULL)
		{
			fpr_set_currentPath("midi");
			fileCloseDir(dirOpenResult);
		}
		else fpr_set_currentPath("0:");
	}

	if(getTheFile_far(myRecord.fileName, DIRECTORY_X, DIRECTORY_Y, "mid", "", "", "")!=0) return 0;

	printf("**** %s *****",myRecord.fileName);
	kernelWaitKey();
	loadSMFile(myRecord.fileName, MUSIC_BASE);

	setColors();textGotoXY(0,25);printf("->Currently Loading file %s...",myRecord.fileName);

	wipeText();
	detectStructure(0, &myRecord);
	displayInfo(&myRecord);
	midiResetInstruments(false); //resets all channels to piano, for sam2695
	midiPanic(false); //ends trailing previous notes if any, for sam2695

	midiShutAllChannels(false);


    initTrack(MUSIC_BASE);


	copySidInstrument(sid_instrument_defs[2], &sidInstruments[0]); //sawtooth
	sid_setInstrument(0, 0, sidInstruments[0]);

	copySidInstrument(sid_instrument_defs[0], &sidInstruments[1]); //sinus
	sid_setInstrument(0, 1, sidInstruments[1]);

	copySidInstrument(sid_instrument_defs[3], &sidInstruments[2]); //noise
	sid_setInstrument(0, 2, sidInstruments[2]);

	copySidInstrument(sid_instrument_defs[1], &sidInstruments[3]); //triangle
	sid_setInstrument(1, 0, sidInstruments[3]);

	copySidInstrument(sid_instrument_defs[1], &sidInstruments[4]); //triangle
	sid_setInstrument(1, 1, sidInstruments[4]);

	copySidInstrument(sid_instrument_defs[0], &sidInstruments[5]); //sinus
	sid_setInstrument(1, 2, sidInstruments[5]);

	sidSetSIDWide(0);
	chipXChannel[0] = 0x01; //voice 0 | sid  (sawtooth)
	chipXChannel[1] = 0x11; //voice 1 | sid  (sine)
	chipXChannel[2] = 0x31; //voice 1 | sid  (tri)
	chipXChannel[3] = 0x41; //voice 1 | sid  (tri)
	chipXChannel[4] = 0x51; //voice 1 | sid  (squ)
	chipXChannel[9] = 0x21; //voice 2 | sid  (noise)


/*
	printf("SID instrument chosen\n%02x maxVol\n%02x pwdLo %02x pwdHi\n%02x ad %02x sr\n%02x ctrl\n%02x fcfLo %02x fcfHi%02x frr\n", sidInstruments[0].maxVolume,
sidInstruments[0].pwdLo,
sidInstruments[0].pwdHi,
sidInstruments[0].ad,
sidInstruments[0].sr,
sidInstruments[0].ctrl,
sidInstruments[0].fcfLo,
sidInstruments[0].fcfHi,
sidInstruments[0].frr);


	printf("SID instrument chosen\n%02x maxVol\n%02x pwdLo %02x pwdHi\n%02x ad %02x sr\n%02x ctrl\n%02x fcfLo %02x fcfHi\n%02x frr", sidInstruments[2].maxVolume,
sidInstruments[2].pwdLo,
sidInstruments[2].pwdHi,
sidInstruments[2].ad,
sidInstruments[2].sr,
sidInstruments[2].ctrl,
sidInstruments[2].fcfLo,
sidInstruments[2].fcfHi,
sidInstruments[2].frr);



*/
	//chipXChannel[2] = 0x02;
//find what to do and exhaust all zero delay events at the start



	for(uint16_t i=0;i<theOne.nbTracks;i++)	exhaustZeroes(i);

	timer0Reset();

	while(!isTrulyDone)
	{
			wipeStatus();
			superExtraInfo(&myRecord);

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

					midiShutAllChannels(false);
					//shutPSG();
					sidClearRegisters();

					destroyTrack();
					zeroOutStuff();

					detectStructure(0, &myRecord);
					initTrack(MUSIC_BASE);
					textSetColor(1,0);textGotoXY(3,26);textPrint("[r]");

					}
					else
					{
						isPaused = true;
						break; //really quit no matter what
					}
				}

				}

	midiShutAllChannels(false);
	}

	return 0;}
