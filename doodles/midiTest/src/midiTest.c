

#define VELO_MIN 0x40 //minimum MIDI in note velocity, to avoid being too quiet

 //TIMER_TEXT for the 1-frame long text refresh timer for data display
 //TIMER_NOTE is for a midi note timer
#define TIMER_TEXT_COOKIE 0
#define TIMER_TEXT_DELAY 9

// f256lib now provides: vs1053b, midi, psg, opl3, presetBeats, beatFileOps, dispatch, sid, textui
#include "stdio.h"

#define PSG_DEFAULT_VOL 0x4C

#define WITHOUT_TILE
#define WITHOUT_SPRITE
#include "f256lib.h"

// TODO: EMBED(palpiano, "../assets/piano.pal", 0x30000);
// TODO: EMBED(pia1, "../assets/piano.raw", 0x38000);


// ============================================================
// Local declarations for modules not yet ported to f256lib
// (originally from mu0nlibs: mudispatch, presetBeats, textui)
// ============================================================

// --- Extended dispatch globals (adds fields missing from dispatchGlobalsT) ---
typedef struct {
	// Same layout as dispatchGlobalsT (must stay in sync)
	bool wantVS1053;
	uint8_t sidInstChoice;
	uint8_t opl3InstChoice;
	uint8_t chipChoice;
	sidInstrumentT *sidValues;
	uint8_t o_2_tvskf, o_1_tvskf;
	uint8_t o_2_kslvol, o_1_kslvol;
	uint8_t o_2_ad, o_1_ad;
	uint8_t o_2_sr, o_1_sr;
	uint8_t o_2_wav, o_1_wav;
	uint8_t o_chanfeed;
	// Extended fields (from original globalThings struct)
	uint8_t *prgInst;
	uint8_t chSelect;
	bool isTwinLinked;
	uint8_t selectBeat;
	uint8_t mainTempo;
} dispatchGlobalsExtT;

// Cast macro: use DG-> for extended fields, dispatchGlobals-> still works for base fields
#define DG ((dispatchGlobalsExtT*)dispatchGlobals)

// Local init for extended fields (called after dispatchResetGlobals)
static void dispatchResetExtendedGlobals(void) {
	DG->chSelect = 0;
	DG->isTwinLinked = false;
	DG->selectBeat = 0;
	DG->mainTempo = 120;
	if (DG->prgInst == NULL) DG->prgInst = malloc(sizeof(uint8_t) * 10);
	for (uint8_t i = 0; i < 10; i++) DG->prgInst[i] = 0;
}


// --- Polyphony buffers (static in f_dispatch.c, need local copies for clearing) ---
static uint8_t polyOPL3Buffer[18];
static uint8_t polySIDBuffer[6];
static uint8_t polyPSGBuffer[6];


// --- Beat types and constants (from presetBeats.h) ---

#define TIMER_BEAT_1A 6
#define TIMER_BEAT_1B 7
#define BASE_PRESETS 0x20000

typedef struct aB {
	bool isActive;
	bool pendingRelaunch;
	uint8_t activeCount;
	uint8_t suggTempo;
	uint8_t howManyChans;
	uint32_t baseAddr;
	uint8_t *index;
	uint8_t *pending2x;
	struct timer_t *timers;
} aBeat;

typedef struct aT {
	uint8_t chip;
	uint8_t chan;
	uint8_t inst;
	uint8_t count;
} aTrack;


// --- Compatibility aliases for identifiers missing from f256lib ---
// (placed before beat functions which use prgChange)

#define sid_instrumentsSize  sidInstrumentsSize
#define opl3_instrumentsSize opl3InstrumentsSize
#define prgChange            midiProgramChange
#define midiShutAChannel     midiShutChannel

static void emptyMIDIINBuffer(void) {
	uint16_t i, toDo;
	if (!(PEEK(MIDI_CTRL) & 0x02)) {
		toDo = PEEKW(MIDI_RXD) & 0x0FFF;
		for (i = 0; i < toDo; i++) PEEK(MIDI_FIFO);
	}
}


// --- Beat helper functions (ported from presetBeats.c) ---

static void punchInFar(uint8_t value, uint32_t *whereTo) {
	FAR_POKE(*whereTo, value);
	(*whereTo)++;
}

static void punchInFarArray(const uint8_t *theArray, uint8_t howMany, uint32_t *whereTo) {
	for (uint8_t i = 0; i < howMany; i++) {
		FAR_POKE((*whereTo)++, theArray[i]);
	}
}

static int8_t setupMem4ABeat(struct aB *theB, uint8_t whichBeat, uint8_t tempo, uint8_t chanCountNeeded, uint32_t *whereAt) {
	theB[whichBeat].isActive = false;
	theB[whichBeat].pendingRelaunch = false;
	theB[whichBeat].activeCount = 0;
	theB[whichBeat].suggTempo = tempo;
	theB[whichBeat].howManyChans = chanCountNeeded;
	theB[whichBeat].baseAddr = *whereAt;
	theB[whichBeat].index = malloc(sizeof(uint8_t) * chanCountNeeded);
	if (theB[whichBeat].index == NULL) return -1;
	theB[whichBeat].pending2x = malloc(sizeof(uint8_t) * chanCountNeeded);
	if (theB[whichBeat].pending2x == NULL) return -1;
	theB[whichBeat].timers = malloc(sizeof(struct timer_t) * chanCountNeeded);
	if (theB[whichBeat].timers == NULL) return -1;
	for (uint8_t i = 0; i < chanCountNeeded; i++) {
		theB[whichBeat].index[i] = 0;
		theB[whichBeat].pending2x[i] = 0;
		theB[whichBeat].timers[i].units = TIMER_FRAMES;
		theB[whichBeat].timers[i].cookie = TIMER_BEAT_1A + i;
	}
	return 0;
}

static void setupMem4Track(struct aT track, uint32_t *whereAt, const uint8_t *notes, const uint8_t *delays, uint8_t howMany) {
	punchInFar(track.chip, whereAt);
	punchInFar(track.chan, whereAt);
	punchInFar(track.inst, whereAt);
	punchInFar(howMany, whereAt);
	punchInFarArray(notes, howMany, whereAt);
	punchInFarArray(delays, howMany, whereAt);
}

void getBeatTrackNoteInfo(struct aB *theB, uint8_t whichBeat, uint8_t track, uint8_t *farNote, uint8_t *farDelay, struct aT *theT) {
	uint32_t addr = theB[whichBeat].baseAddr;
	uint8_t farCount = 0;
	for (uint8_t i = 0; i < theB[whichBeat].howManyChans; i++) {
		farCount = FAR_PEEK(addr + 3);
		if (i == track) {
			theT->chip = FAR_PEEK(addr);
			theT->chan = FAR_PEEK(addr + 1);
			theT->inst = FAR_PEEK(addr + 2);
			theT->count = FAR_PEEK(addr + 3);
			*farNote  = FAR_PEEK(addr + 4 + (uint32_t)theB[whichBeat].index[track]);
			*farDelay = FAR_PEEK(addr + 4 + (uint32_t)(farCount) + (uint32_t)theB[whichBeat].index[track]);
		} else {
			addr += (uint32_t)(2 * (farCount) + 4);
		}
	}
}

void beatSetInstruments(struct aT *theT) {
	switch (theT->chip) {
	case 0: // MIDI
		if (theT->chan != 0x09) {
			prgChange(theT->inst, theT->chan, true);
			prgChange(theT->inst, theT->chan, false);
		}
		break;
	case 1: // SID
		sidSetInstrument((theT->chan) / 3, (theT->chan) - ((theT->chan) / 3) * 3, sidInstrumentDefs[theT->inst]);
		sidSetSIDWide(theT->inst);
		break;
	case 3: // OPL3
		opl3SetInstrument(opl3InstrumentDefs[theT->inst], theT->chan);
		opl3SetFeed(opl3InstrumentDefs[theT->inst].CHAN_FEED, theT->chan);
		break;
	}
}


// --- Preset beat data ---

static const uint8_t beat00_notes[] = {0x24, 0x26};
static const uint8_t beat00_delays[] = {3, 3};
static const struct aT beat00 = {.chip = 0, .chan = 9, .inst = 0, .count = 2};

static const uint8_t beat10_notes[] = {0x24, 0x28, 0x28, 0x24, 0x28};
static const uint8_t beat10_delays[] = {3, 2, 2, 3, 3};
static const uint8_t beat11_notes[] = {0x4B, 0x63, 0x00, 0x63, 0x4B, 0x63, 0x4B, 0x63};
static const uint8_t beat11_delays[] = {2, 2, 2, 2, 2, 2, 2, 2};
static const struct aT beat10 = {.chip = 0, .chan = 9, .inst = 0, .count = 5};
static const struct aT beat11 = {.chip = 0, .chan = 2, .inst = 0x73, .count = 8};

static const uint8_t beat20_notes[] = {0x24, 0x26, 0x24, 0x26};
static const uint8_t beat20_delays[] = {3, 3, 3, 3};
static const uint8_t beat21_notes[] = {0x39, 0x39, 0x39, 0x39, 0x39, 0x39};
static const uint8_t beat21_delays[] = {3, 13, 12, 3, 13, 12};
static const struct aT beat20 = {.chip = 0, .chan = 9, .inst = 0, .count = 4};
static const struct aT beat21 = {.chip = 0, .chan = 9, .inst = 0, .count = 6};

static const uint8_t beat30_notes[] = {0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,
                                       0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2C,0x2E,0x2E,0x2E,0x2E,0x2C,0x2C};
static const uint8_t beat30_delays[] = {16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15,16,15};
static const uint8_t beat31_notes[] = {0x24,0x00,0x24,0x26,0x00,0x24,0x00,0x24,0x24,0x26,0x00,0x24};
static const uint8_t beat31_delays[] = {17,16,15,17,16,15,16,15,17,17,16,15};
static const struct aT beat30 = {.chip = 0, .chan = 9, .inst = 0, .count = 32};
static const struct aT beat31 = {.chip = 0, .chan = 9, .inst = 0, .count = 12};

static const uint8_t beat40_notes[] = {0,0,0,0,
                                       41,53,33,45,34,46,35,47,
                                       36,48,38,50,39,51,40,52,
                                       41,53,33,45,34,46,35,47,
                                       36,48,38,50,39,51,40,52,
                                       41,53,33,45,34,46,35,47,
                                       39,51,37,49,39,51,37,49,
                                       41,53,33,45,34,46,35,47,
                                       39,51,37,49,39,51,37,49,
                                       41,53,33,45,34,46,35,47,
                                       39,51,37,49,39,51,37,49,
                                       41,53,33,45,34,46,35,47,
                                       39,51,37,49,39,51,37,49,
                                       53,0,53,53,53,0};
static const uint8_t beat40_delays[] = {5,5,5,5,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,
                                        3,2,1,1,3,3};
static const uint8_t beat41_notes[] = {23,35,47,35,23,35,47,35,
                                       23,35,47,23,47,23,47,35};
static const uint8_t beat41_delays[] = {2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2};
static const struct aT beat40 = {.chip = 1, .chan = 0, .inst = 1, .count = 105};
static const struct aT beat41 = {.chip = 1, .chan = 2, .inst = 3, .count = 16};

const uint8_t presetBeatCount = 5;
static const char *presetBeatCount_names[] = {
	"WaveSynth        ",
	"Da Da Da         ",
	"Jazzy",
	"Funky",
	"M.U.L.E.         ",
};

void setupBeats(struct aB *theBeats) {
	uint32_t whereAt = BASE_PRESETS;
	setupMem4ABeat(theBeats, 0, 90, 1, &whereAt);
	setupMem4Track(beat00, &whereAt, beat00_notes, beat00_delays, sizeof(beat00_notes));
	setupMem4ABeat(theBeats, 1, 128, 2, &whereAt);
	setupMem4Track(beat10, &whereAt, beat10_notes, beat10_delays, sizeof(beat10_notes));
	setupMem4Track(beat11, &whereAt, beat11_notes, beat11_delays, sizeof(beat11_notes));
	setupMem4ABeat(theBeats, 2, 100, 2, &whereAt);
	setupMem4Track(beat20, &whereAt, beat20_notes, beat20_delays, sizeof(beat20_notes));
	setupMem4Track(beat21, &whereAt, beat21_notes, beat21_delays, sizeof(beat21_notes));
	setupMem4ABeat(theBeats, 3, 80, 2, &whereAt);
	setupMem4Track(beat30, &whereAt, beat30_notes, beat30_delays, sizeof(beat30_notes));
	setupMem4Track(beat31, &whereAt, beat31_notes, beat31_delays, sizeof(beat31_notes));
	setupMem4ABeat(theBeats, 4, 120, 2, &whereAt);
	setupMem4Track(beat40, &whereAt, beat40_notes, beat40_delays, sizeof(beat40_notes));
	setupMem4Track(beat41, &whereAt, beat41_notes, beat41_delays, sizeof(beat41_notes));
}


// --- Text UI color constants (from textui.h) ---

#define textColorGreen  0x04
#define textColorOrange 0x09
#define textColorRed    0x91
#define textColorBlue   0x07
#define textColorGray   0x05


// --- Text UI function stubs (from textui module, not yet ported) ---

static void realTextClear(void) { textClear(); }
static void textTitle(dispatchGlobalsT *g) { (void)g; }
static void refreshInstrumentText(dispatchGlobalsT *g) { (void)g; }
static void channelTextMenu(dispatchGlobalsT *g) { (void)g; }
static void refreshBeatTextChoice(dispatchGlobalsT *g) { (void)g; }
static void updateTempoText(uint8_t tempo) { (void)tempo; }
static void showMIDIChoiceText(dispatchGlobalsT *g) { (void)g; }
static void showChipChoiceText(dispatchGlobalsT *g) { (void)g; }
static void instListShow(dispatchGlobalsT *g) { (void)g; }
static void modalMoveUp(dispatchGlobalsT *g, bool shift) { (void)g; (void)shift; }
static void modalMoveDown(dispatchGlobalsT *g, bool shift) { (void)g; (void)shift; }
static void modalMoveLeft(dispatchGlobalsT *g) { (void)g; }
static void modalMoveRight(dispatchGlobalsT *g) { (void)g; }
static void refreshChipAct(uint8_t *chipAct) { (void)chipAct; }


// ============================================================
// End of local stubs
// ============================================================


struct aB *theBeats;

struct timer_t spaceNotetimer, refTimer, snareTimer; //spaceNotetimer: used when you hit space, produces a 1s delay before NoteOff comes in
//refTimer: is 1 frame long, used to display updated text when you hit keys on a midi controller
//snareTimer is a pre-programmed beat at 30 frames

uint16_t note = 0x36, oldNote, oldCursorNote; /*note is the current midi hex note code to send. oldNote keeps the previous one so it can be Note_off'ed away after the timer expires, or a new note is called*/

//reference tempoLUT, close to 112.5 bpm where a quarter note = 32 frames = 0,533s
uint8_t tempoLUTRef[18] = {4, 8, 16, 32, 64, 128, //         32nd, 16th, 8th, 4th, half, whole
						   8,12, 24, 48, 96, 192, //dotted   32nd, 16th, 8th, 4th, half, whole
					      11,21, 32,  5, 11,  144}; //triplets of 4th: lengths of 1, 2, 3,  triplets of 8ths of length 1, 2, 3


uint8_t mainTempoLUT[18]; //contains the delays between notes in prepared beats

uint8_t diagBuffer[255]; //info dump on screen

uint8_t blockCharacters[] = {22,21,19,20,22,23,26,27,28,29,24,23,24,25,25,22,23,24,21,20,21,23,20};
uint8_t *testArray;

bool instSelectMode = false; //is currently in the mode where you see the whole instrument list
bool needsToWaitExpiration = false;  //when tempo is changed, must wait to expire all pending timers before sounding again.
bool altHit = false, shiftHit = false; //keyboard modifiers

bool noteColors[88]={1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1,0,1, 1};
uint8_t KeyCodesToMIDINotes[123] = {0,0,0,0,0,0,0,0,  //0-7
									0,0,0,0,0,0,0,0,  //8-15
									0,0,0,0,0,0,0,0,  //16-23
									0,0,0,0,0,0,0,0,  //24-31
									0,0,0,0,0,0,0,0,  //32-39
									0,0,0,0,55,0,57,59,  //40-47
									75,0,61,63,0,66,68,70,  //48-55
									0,73,0,58,0,78,0,0,  //56-63
									0,0,0,0,0,0,0,0,  //64-71
									0,0,0,0,0,0,0,0,  //72-79
									0,0,0,0,0,0,0,0,  //80-87
									0,0,0,77,0,79,0,0,  //88-95
									0,42,50,47,46,64,0,49,  //96-103
									51,72,0,54,56,53,52,74,  //104-111
									76,60,65,44,67,71,48,62,  //112-119
									45,69,43}; //120-122


void escReset(void);


void emptyOPL3Buffer()
{
	uint8_t i;
	for(i=0;i<18;i++) polyOPL3Buffer[i]=0;
}
void emptySIDBuffer()
{
	uint8_t i;
	for(i=0;i<6;i++) polySIDBuffer[i]=0;
}
void emptyPSGBuffer()
{
	uint8_t i;
	for(i=0;i<6;i++) polyPSGBuffer[i]=0;
}


void prepTempoLUT()
{
	uint8_t t;
	mainTempoLUT[0] = (uint8_t)(1.0*112.5*tempoLUTRef[0]/(1.0*DG->mainTempo));

	for(t=1;t<6;t++) {
		 mainTempoLUT[t] = mainTempoLUT[0];
		 for(uint8_t i=0;i<t;i++) mainTempoLUT[t] = mainTempoLUT[t]<<1;
	}
	for(t=6;t<12;t++) {
		 mainTempoLUT[t] = (mainTempoLUT[t-6]*2/3);
	}
	mainTempoLUT[17] = mainTempoLUT[1] * 18;
	mainTempoLUT[11] = mainTempoLUT[17] /3 * 2;
	/*
	textGotoXY(0,10);
	for(t=0;t<18;t++) printf("%02d ",mainTempoLUT[t]);
	printf("\n");

	for(t=0;t<18;t++) printf("%02d ",t);
	*/
	//same with triplet eights
}


//setup is called just once during initial launching of the program
void setup()
{
	uint16_t c;

	//Foenix Vicky stuff
	POKE(MMU_IO_CTRL, 0x00);
	// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
	POKE(VKY_MSTR_CTRL_0, 0b00001111); //sprite,graph,overlay,text
	// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
	POKE(VKY_MSTR_CTRL_1, 0b00000000); //font overlay, 320x240 at 60 Hz;
	POKE(VKY_LAYER_CTRL_0, 0b00010000); //bitmap 0 in layer 0, bitmap 1 in layer 1
	POKE(VKY_LAYER_CTRL_1, 0b00000010); //bitmap 2 in layer 2
	POKE(0xD00D,0x00); //force black graphics background
	POKE(0xD00E,0x00);
	POKE(0xD00F,0x00);

	POKE(MMU_IO_CTRL,1);  //MMU I/O to page 1
	//prep to copy over the palette to the CLUT
	for(c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(0x30000+c)); //palette for piano
	}

	POKE(MMU_IO_CTRL,0); //MMU I/O to page 0
	POKE(0xD840, 0xFF);  //blue
	POKE(0xD841, 0xFF); //green
	POKE(0xD842, 0xFF); //red
	POKE(0xD843, 0);

	//piano bitmap at layer 0
	bitmapSetActive(0);
	bitmapSetAddress(0,0x38000);
	bitmapSetVisible(0,true);
	bitmapSetCLUT(0);
	bitmapSetVisible(1,false);
	bitmapSetVisible(2,false);

	//Globals (allocate extended struct, library sees the base portion)
	dispatchGlobals = (dispatchGlobalsT*)malloc(sizeof(dispatchGlobalsExtT));
	dispatchResetGlobals(dispatchGlobals);
	dispatchResetExtendedGlobals();

	//Beats
	theBeats = malloc(sizeof(aBeat) * presetBeatCount);
	setupBeats(theBeats);

	//super preparation
	escReset();

	//prep the tempoLUT
	prepTempoLUT();

	//Prep all the kernel timers
	refTimer.units = TIMER_FRAMES;
	refTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_TEXT_DELAY;
	refTimer.cookie = TIMER_TEXT_COOKIE;
	kernelSetTimer(&refTimer);


	textSetColor(textColorOrange,0x00);
	emptyMIDIINBuffer();

	//Prep SID stuff
	clearSIDRegisters();
	prepSIDinstruments();
	setMonoSID();


	//Prep PSG stuff
	setMonoPSG();

	//Prep OPL3 stuff
	opl3Initialize();
	opl3SetInstrumentAllChannels(0, true);

	for(c=0;c<255;c++) diagBuffer[c] = 0;

	vs1053bBoostClock();
	vs1053bInitRTMIDI();
	//openAllCODEC() inline expansion:
	POKE(0xD620, 0x1F); POKE(0xD621, 0x2A); POKE(0xD622, 0x01); while(PEEK(0xD622) & 0x01);
	//codec enable all lines
/*
	if(platformIsWave2())
	{
	//boost the VS1053b clock speed
	boostVSClock();
	//initialize the VS1053b real time midi plugin
	initRTMIDI();
	openAllCODEC();
	}
	*/

}
void escReset()
{
	midiResetInstruments(dispatchGlobals->wantVS1053);
	midiResetInstruments(~dispatchGlobals->wantVS1053);
	dispatchResetGlobals(dispatchGlobals);
	realTextClear();
	refreshInstrumentText(dispatchGlobals);
	textTitle(dispatchGlobals);
	sidShutAllVoices();
	psgShut();
	instSelectMode=false; //returns to default mode
	POKE(MIDI_FIFO_ALT,0xC2);
	POKE(MIDI_FIFO_ALT,0x73);//woodblock
	POKE(MIDI_FIFO,0xC2);
	POKE(MIDI_FIFO,0x73);//woodblock
	DG->prgInst[0]=0;DG->prgInst[1]=0;DG->prgInst[9]=0;

}


void shutAllMIDIchans()
{
	midiShutAChannel(0, true);midiShutAChannel(0, false);
	midiShutAChannel(1, true);midiShutAChannel(1, false);
	midiShutAChannel(9, true);midiShutAChannel(9, false);
}

void launchBeat()
{
	uint8_t j, noteScoop=0, delayScoop=0;
	struct aT *theT;

	theT = malloc(sizeof(aTrack));

	if(theBeats[DG->selectBeat].isActive) needsToWaitExpiration = true; //orders a dying down of the beat
	if(needsToWaitExpiration) {
		for(j=0;j<theBeats[DG->selectBeat].howManyChans;j++)
			{
				getBeatTrackNoteInfo(theBeats, DG->selectBeat, j, &noteScoop, &delayScoop, theT);
				if(theT->chip == 1) {
					for(uint8_t i=0;i < 6;i++) if(dispatchReservedSID[i]==1) dispatchReservedSID[i]=0;

				}
				if(theT->chip == 2) {
					for(uint8_t i=0;i < 6;i++) if(dispatchReservedPSG[i]==1) dispatchReservedPSG[i]=0;
				}
				if(theT->chip == 3) {
					for(uint8_t i=0;i < 18;i++) if(dispatchReservedOPL3[i]==1) dispatchReservedOPL3[i]=0;
				}
			}
		free(theT);
		return; //we're not done finishing the beat, do nothing new
	}
	if(theBeats[DG->selectBeat].isActive == false) //starts the beat
		{
		theBeats[DG->selectBeat].isActive = true;
		DG->mainTempo = theBeats[DG->selectBeat].suggTempo;

		prepTempoLUT();
		updateTempoText(DG->mainTempo);
		for(j=0;j<theBeats[DG->selectBeat].howManyChans;j++)
			{
				theBeats[DG->selectBeat].index[j] = 0;
				getBeatTrackNoteInfo(theBeats, DG->selectBeat, j, &noteScoop, &delayScoop, theT);

				beatSetInstruments(theT);
				dispatchNote(true, theT->chan, noteScoop,theT->chip==2?PSG_DEFAULT_VOL:0x7F, dispatchGlobals->wantVS1053, theT->chip, true, theT->inst);

				if(theT->chip == 1) dispatchReservedSID[theT->chan] = 1;
				theBeats[DG->selectBeat].activeCount+=1;
				theBeats[DG->selectBeat].timers[j].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + mainTempoLUT[delayScoop];
				kernelSetTimer(&(theBeats[DG->selectBeat].timers[j]));
			}
		}

	free(theT);
}
void dealKeyPressed(uint8_t keyRaw)
{
	uint8_t j;

	switch(keyRaw)
	{
		case 148: //enter
				if(instSelectMode){
					realTextClear();
					textTitle(dispatchGlobals);
					instSelectMode=false;
				}
				break;
		case 146: // top left backspace, meant as reset
			escReset();
			break;
		case 129: //F1
			instSelectMode = !instSelectMode; //toggle this mode
			if(instSelectMode == true) instListShow(dispatchGlobals);
			else if(instSelectMode==false) {
				realTextClear();
				textTitle(dispatchGlobals);
				}
			break;
		case 131: //F3
			if(instSelectMode==false) {
				DG->isTwinLinked=false;
				DG->chSelect++;
				if(DG->chSelect == 2)
					{
					DG->chSelect = 9;
					DG->isTwinLinked=false;
				}
				if(DG->chSelect == 10) DG->chSelect = 0;
				channelTextMenu(dispatchGlobals);
				refreshInstrumentText(dispatchGlobals);
				shutAllMIDIchans();
			}
			break;
		case 133: //F5
			if(instSelectMode==false) {
				if(theBeats[DG->selectBeat].isActive) needsToWaitExpiration = true; //orders a dying down of the beat
				if(needsToWaitExpiration) break; //we're not done finishing the beat, do nothing new

				if(theBeats[DG->selectBeat].isActive == false) //cycles the beat
					{
					DG->selectBeat++;
					if((DG->selectBeat)==presetBeatCount)DG->selectBeat=0;
					refreshBeatTextChoice(dispatchGlobals);
					}
			}
			break;
		case 135: //F7
			if(instSelectMode==false) {

					launchBeat();
			}
			break;
		case 0xb6: //up arrow
			if(instSelectMode==false)
				{
					if(dispatchGlobals->chipChoice==0)
					{
						if(DG->prgInst[DG->chSelect] < 127 - shiftHit*9) DG->prgInst[DG->chSelect] = DG->prgInst[DG->chSelect] + 1 + shiftHit *9; //go up 10 instrument ticks if shift is on, otherwise just 1
						if(altHit) DG->prgInst[DG->chSelect] = 127; //go to highest instrument, 127
						prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
						prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
					}
					if(dispatchGlobals->chipChoice==1)
					{
						if(dispatchGlobals->sidInstChoice<sid_instrumentsSize) dispatchGlobals->sidInstChoice++;
					}
					if(dispatchGlobals->chipChoice==3) if(dispatchGlobals->opl3InstChoice<opl3_instrumentsSize) dispatchGlobals->opl3InstChoice++;
					refreshInstrumentText(dispatchGlobals);
				}
				else if(instSelectMode==true)
				{
					if(dispatchGlobals->chipChoice==0)
					{
						if(DG->prgInst[DG->chSelect] > (shiftHit?29:2))
						{
							modalMoveUp(dispatchGlobals, shiftHit);
							prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
							prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
						}
					}
					if(dispatchGlobals->chipChoice==1 && dispatchGlobals->sidInstChoice>0)
						{
						modalMoveUp(dispatchGlobals, shiftHit);
						sid_setInstrumentAllChannels(dispatchGlobals->sidInstChoice);
						}
					if(dispatchGlobals->chipChoice==3 && dispatchGlobals->opl3InstChoice>0)
					{
						modalMoveUp(dispatchGlobals, shiftHit);
						opl3SetInstrumentAllChannels(dispatchGlobals->opl3InstChoice, true);
					}
				}
			break;
		case 0xb7: //down arrow
			if(instSelectMode==false)
					{
					if(dispatchGlobals->chipChoice==0)
						{
						if(DG->prgInst[DG->chSelect] > 0 + shiftHit*9) DG->prgInst[DG->chSelect] = DG->prgInst[DG->chSelect] - 1 - shiftHit *9; //go down 10 instrument ticks if shift is on, otherwise just 1
						if(altHit) DG->prgInst[DG->chSelect] = 0; //go to lowest instrument, 0
						prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
						prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
						}
					if(dispatchGlobals->chipChoice==1) if(dispatchGlobals->sidInstChoice>0) dispatchGlobals->sidInstChoice--;
						refreshInstrumentText(dispatchGlobals);
					}
				else if(instSelectMode==true)
				{
					if(dispatchGlobals->chipChoice==0) //MIDI
						{
							if(DG->prgInst[DG->chSelect] < (shiftHit?98:125))
							{
								modalMoveDown(dispatchGlobals, shiftHit);
								prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
								prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
							}
						}
					if(dispatchGlobals->chipChoice==1 && dispatchGlobals->sidInstChoice<(sid_instrumentsSize-1)) //SID
					{
					modalMoveDown(dispatchGlobals, shiftHit);
					sid_setInstrumentAllChannels(dispatchGlobals->sidInstChoice);
					}
					if(dispatchGlobals->chipChoice==3 && dispatchGlobals->opl3InstChoice<(opl3_instrumentsSize-1))  //OPL3
					{
						modalMoveDown(dispatchGlobals, shiftHit);
						opl3SetInstrumentAllChannels(dispatchGlobals->opl3InstChoice, true);
					}
				}
			break;
		case 0xb8: //left arrow
			if(instSelectMode==false)
					{
					if(note > (0 + shiftHit*11))
						{
						if(dispatchGlobals->chipChoice > 0) dispatchNote(false, 0, note+0x15, 0, false, dispatchGlobals->chipChoice, false, 0);
						note = note - 1 - shiftHit * 11;
						}
					if(altHit) note = 0; //go to the leftmost note
					graphicsDefineColor(0, oldCursorNote+0x61,0x00,0x00,0x00); //remove old cursor position
					graphicsDefineColor(0, note+0x61,0xFF,0x00,0x00); //set new cursor position
					oldCursorNote = note;
					}
			else if(instSelectMode==true)
				{
				if(DG->prgInst[DG->chSelect] > 0) modalMoveLeft(dispatchGlobals);
				prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
				prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
				}
			break;
		case 0xb9: //right arrow
				if(instSelectMode==false)
					{
					if(note < 87 - shiftHit*11)
						{
						if(dispatchGlobals->chipChoice > 0) dispatchNote(false, 0, note+0x15, 0, false, dispatchGlobals->chipChoice, false, 0);
						note = note + 1 + shiftHit * 11;
						}
					if(altHit) note = 87; //go to the rightmost note

					graphicsDefineColor(0, oldCursorNote+0x61,0x00,0x00,0x00);//remove old cursor position
					graphicsDefineColor(0, note+0x61,0xFF,0x00,0x00); //set new cursor position
					oldCursorNote = note;
					}
				else if(instSelectMode==true)
					{
					if(DG->prgInst[DG->chSelect] < 127) modalMoveRight(dispatchGlobals);
					prgChange(DG->prgInst[DG->chSelect],DG->chSelect, dispatchGlobals->wantVS1053);
					prgChange(DG->prgInst[DG->chSelect],DG->chSelect, !dispatchGlobals->wantVS1053);
					}
			break;
		case 32: //space
				//Send a Note
				if(dispatchGlobals->chipChoice !=3)
				{
				dispatchNote(true, 0x90 | DG->chSelect ,note+0x15,0x7F, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
				//keep track of that note so we can Note_Off it when needed
				oldNote = note+0x15; //make it possible to do the proper NoteOff when the timer expires
				}
				else
					{

					dispatchNote(true, 0,note+0x15,0,false, dispatchGlobals->chipChoice, false, 0);
					}

			break;
		case 5: //alt modifier
			altHit = true;
			break;
		case 1: //shift modifier
			shiftHit = true;
			break;
		case 91: // '['
			if(theBeats[DG->selectBeat].isActive) {
				DG->mainTempo -= (1 + shiftHit*9);
				prepTempoLUT();
				updateTempoText(DG->mainTempo);

				needsToWaitExpiration = true; //orders a dying down of the beat
				theBeats[DG->selectBeat].pendingRelaunch=true;
			}
			if(needsToWaitExpiration) break; //we're not done finishing the beat, do nothing new

			if(theBeats[DG->selectBeat].isActive == false) //changes the tempo
			{
				needsToWaitExpiration=true; //order an expiration of the beat
				break; //not done expiring a beat, don't start a new one
			}
			//proceed to change tempo freely if one isn't playing at all
			else if(instSelectMode==false && DG->mainTempo > 30 + shiftHit*9) {
				DG->mainTempo -= (1 + shiftHit*9);
				prepTempoLUT();
				updateTempoText(DG->mainTempo);

				shutAllMIDIchans();
				for(j=0;j<theBeats[DG->selectBeat].howManyChans;j++) theBeats[DG->selectBeat].index[j]=0;
			}
			break;
		case 93: // ']'
			if(theBeats[DG->selectBeat].isActive) {
				DG->mainTempo += (1 + shiftHit*9);
				prepTempoLUT();
				updateTempoText(DG->mainTempo);
				needsToWaitExpiration = true; //orders a dying down of the beat
				theBeats[DG->selectBeat].pendingRelaunch=true;
			}
			if(needsToWaitExpiration) break; //we're not done finishing the beat, do nothing new

			if(theBeats[DG->selectBeat].isActive)
				{
					needsToWaitExpiration=true; //order an expiration of the beat
					break; //not done expiring a beat, don't start a new one
				}
			else if(instSelectMode==false && DG->mainTempo < 255 - shiftHit*9) {
				DG->mainTempo += (1 + shiftHit*9);
				prepTempoLUT();
				updateTempoText(DG->mainTempo);

				shutAllMIDIchans();
				for(j=0;j<theBeats[DG->selectBeat].howManyChans;j++) theBeats[DG->selectBeat].index[j]=0;
			}
			break;
		case 120: // X - twin link mode
			if(instSelectMode==false) {
				DG->isTwinLinked = !DG->isTwinLinked;
				if(DG->isTwinLinked) {
					DG->chSelect=0;
					textSetColor(textColorOrange,0x00);
					textGotoXY(0,30);printf("%c",0xFA);
					textGotoXY(0,31);printf("%c",0xFA);
					textGotoXY(0,32);textPrint(" ");
				} else
				{
					DG->chSelect=0;
					textSetColor(textColorOrange,0x00);
					textGotoXY(0,30);printf("%c",0xFA);
					textGotoXY(0,31);textPrint(" ");
					textGotoXY(0,32);textPrint(" ");
				}

				channelTextMenu(dispatchGlobals);
				refreshInstrumentText(dispatchGlobals);
				shutAllMIDIchans();
			}
			break;
		case 109: // M - toggle the MIDI chip between default SAM2695 to VS1053b

			shutAllMIDIchans();
			dispatchGlobals->wantVS1053 = ~(dispatchGlobals->wantVS1053);
			shutAllMIDIchans();
			if(dispatchGlobals->wantVS1053) dispatchChipAct[0]=0;
			else dispatchChipAct[1]=0;

			showMIDIChoiceText(dispatchGlobals);
			break;
		case 99: // C - chip select mode: 0=MIDI, 1=SID, (todo) 2= PSG, (todo) 3=OPL3
		if(instSelectMode==false){

			dispatchGlobals->chipChoice+=1;
			if(dispatchGlobals->chipChoice==1)
			{
				prepSIDinstruments(); //just arrived in sid, prep sid
				dispatchChipAct[0]=0; dispatchChipAct[1]=0;
			}
			if(dispatchGlobals->chipChoice==2)
			{
				clearSIDRegisters();
				emptySIDBuffer();
				dispatchChipAct[2]=0;
			}
			if(dispatchGlobals->chipChoice==3)
			{
				emptyPSGBuffer();
				psgShut();
				dispatchChipAct[3]=0;
			}
			if(dispatchGlobals->chipChoice>3)
			{
				opl3QuietAll();
				emptyOPL3Buffer();
				dispatchGlobals->chipChoice=0; //loop back to midi at the start of the cyle
				dispatchChipAct[4]=0;
			}
			showChipChoiceText(dispatchGlobals);
					}
			break;
		default:
				//Send a Note
				if(dispatchGlobals->chipChoice !=3)
				{
				dispatchNote(true, 0x90 | DG->chSelect ,KeyCodesToMIDINotes[kernelEventData.u.key.raw],0x7F, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
				//keep track of that note so we can Note_Off it when needed
				oldNote = note+0x15; //make it possible to do the proper NoteOff when the timer expires
				}
				else
					{

					dispatchNote(true, 0,KeyCodesToMIDINotes[kernelEventData.u.key.raw],0,false, dispatchGlobals->chipChoice, false, 0);
					}
		break;
	}
	//the following line can be used to get keyboard codes
	//printf("\n %03d 0x%02x    ",kernelEventData.u.key.raw,kernelEventData.u.key.raw);
}

void dealKeyReleased(uint8_t rawKey)
{
switch(rawKey)
	{
	case 5:  //alt modifier
		altHit = false;
		break;
	case 1: //shift modifier
		shiftHit = false;
		break;
	case 32: //space
		dispatchNote(false, DG->chSelect ,note+0x15,0x3F, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
		break;
	}
}


int main(int argc, char *argv[]) {
	uint16_t toDo;
	uint16_t i;
	uint8_t j;
	uint8_t recByte, detectedNote, detectedColor, lastCmd=0x90;
	bool nextIsNote = false; //detect a 0x9? or 0x8? command, the next is a note byte, used for coloring the keyboard note-rects
	bool nextIsSpeed = false; //detects if we're at the end of a note on or off trio of bytes
	bool isHit = false; // true is hit, false is released
	POKE(1,0);
	uint8_t storedNote; //stored values when twinlinked
	uint8_t lastNote = 0; // for monophonic chips, store this to mute last note before doing a new one
	uint8_t bufferIndex=0;
	bool opl3Active = false;
	bool psgActive = false;
	bool sidActive = false;
	uint8_t noteScoop=0, delayScoop=0;
	struct aT *theT;

	theT = malloc(sizeof(aTrack));
	setup();

	note=39;
	oldCursorNote=39;
	graphicsDefineColor(0, note+0x61,0xFF,0x00,0x00);


	while(true)
        {
		if(!(PEEK(MIDI_CTRL) & 0x02)) //rx not empty
			{
				toDo = PEEKW(MIDI_RXD) & 0x0FFF; //discard top 4 bits of MIDI_RXD+1

				//erase the region where the last midi bytes received are shown
				if(instSelectMode==false){
					textGotoXY(5,4);textPrint("                        ");textGotoXY(5,4);
				}

				//deal with the MIDI bytes and exhaust the FIFO buffer
				for(i=0; i<toDo; i++)
				{
				//get the next MIDI in FIFO buffer byte
				recByte=PEEK(MIDI_FIFO);
				if(recByte == 0xfe) continue; //active sense, ignored
				if(bufferIndex==255) bufferIndex = 0;
				diagBuffer[bufferIndex++] = recByte;

				if(nextIsSpeed) //this block activates when a note is getting finished on the 3rd byte ie 0x90 0x39 0x40 (noteOn middleC midSpeed)
					{
					nextIsSpeed = false;
					//force a minimum level with this instead: recByte<0x70?0x70:recByte
					if(isHit == true) //turn the last one off before dealing with the new one
						{
						switch(dispatchGlobals->chipChoice) //remove last note before making new one
							{
							case 1:
								if(sidActive)
									{
									dispatchNote(false, DG->chSelect,lastNote,recByte<VELO_MIN?VELO_MIN:recByte, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
									}
								break;
							case 2:
								if(psgActive)
									{
									dispatchNote(false, DG->chSelect,lastNote,recByte<VELO_MIN?VELO_MIN:recByte, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
									}
								break;
							case 3:
								if(opl3Active)
									{
									dispatchNote(false, DG->chSelect,lastNote,recByte<VELO_MIN?VELO_MIN:recByte, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0);
									}
								break;
							}
						}
					dispatchNote(isHit, DG->chSelect,storedNote,recByte<VELO_MIN?VELO_MIN:recByte, dispatchGlobals->wantVS1053, dispatchGlobals->chipChoice, false, 0); //do the note or turn off the note
					if(isHit == false) //turn 'em off if the note is ended
						{
						switch(dispatchGlobals->chipChoice)
						{
							case 1:
								sidActive = false;
								break;
							case 2:
								psgActive = false;
								break;
							case 3:
								opl3Active = false;
								break;
							}
						}
					lastNote = storedNote;
					}
				else if(nextIsNote) //this block triggers if the previous byte was a NoteOn or NoteOff (0x90,0x80) command previously
					{
					//figure out which note on the graphic is going to be highlighted
					detectedNote = recByte-0x14;

					//first case is when the last command is a 0x90 'NoteOn' command
					if(isHit) {graphicsDefineColor(0, detectedNote,0xFF,0x00,0xFF); //paint it as a hit note
					textGotoXY(0,57);textPrintInt(recByte);
					}
					//otherwise it's a 0x80 'NoteOff' command
					else {
						detectedColor = noteColors[detectedNote-1]?0xFF:0x00;
						graphicsDefineColor(0, detectedNote,detectedColor,detectedColor,detectedColor); //swap back the original color according to this look up ref table noteColors
					}
					nextIsNote = false; //return to previous state after a note is being dealt with
					storedNote = recByte;
					nextIsSpeed = true;
					}
				else if((nextIsNote == false) && (nextIsSpeed == false)) //what command are we getting next?
					{
					switch(recByte & 0xF0)
						{
						case 0x90: //we know it's a 'NoteOn', get ready to analyze the note byte, which is next
							nextIsNote = true;
							isHit=true;
							lastCmd = recByte;
							break;
						case 0x80: //we know it's a 'NoteOff', get ready to analyze the note byte, which is next
							nextIsNote = true;
							isHit=false;
							lastCmd = recByte;
							break;
						case 0x00 ... 0x7F:
							storedNote = recByte;
							nextIsNote = false; //false because we just received it!
							nextIsSpeed = true;
							switch(lastCmd & 0xF0)
								{
								case 0x90:
									isHit=true;
									break;
								case 0x80:
									isHit=true;
									break;
								}
							break;
						}
					}
				//else if(dispatchGlobals->chipChoice==0 && nextIsNote == false && nextIsSpeed == false) POKE(dispatchGlobals->wantVS1053?MIDI_FIFO_ALT:MIDI_FIFO, recByte); //all other bytes are sent normally
				}
			}
		kernelNextEvent();
        if(kernelEventData.type == kernelEvent(timer.EXPIRED))
            {
			switch(kernelEventData.u.timer.cookie)
				{
				//all user interface related to text update through a 1 frame timer is managed here
				case TIMER_TEXT_COOKIE:
					refTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_TEXT_DELAY;
					kernelSetTimer(&refTimer);
					if(instSelectMode==false) refreshChipAct(dispatchChipAct);
					break;
				case TIMER_BEAT_1A ... 255:
					if(theBeats[DG->selectBeat].isActive)
						{
						switch(kernelEventData.u.timer.cookie)
							{
								default:
								j = kernelEventData.u.timer.cookie -TIMER_BEAT_1A; //find the right track of the beat to deal with
								break;
							}

						if (theBeats[DG->selectBeat].pending2x[j]== 2)theBeats[DG->selectBeat].pending2x[j]= 0; //restore for next pass

						if (theBeats[DG->selectBeat].pending2x[j]== 1){
							theBeats[DG->selectBeat].timers[j].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + mainTempoLUT[delayScoop];
							kernelSetTimer(&(theBeats[DG->selectBeat].timers[j]));
							theBeats[DG->selectBeat].pending2x[j]= 2;
							break;
							}


						getBeatTrackNoteInfo(theBeats, DG->selectBeat, j, &noteScoop, &delayScoop, theT);
						dispatchNote(false, theT->chan, noteScoop&0x7F,0x7F, dispatchGlobals->wantVS1053, theT->chip, true, theT->inst); // silence old note
						theBeats[DG->selectBeat].activeCount-=1;

						//textGotoXY(0,16);printf("activeCount %d",theBeats[DG->selectBeat].activeCount);

						//check if we need to expire the beat and act to die things down
						if(needsToWaitExpiration)
							{

								if(theBeats[DG->selectBeat].activeCount > 0)
								{
									break;
								}
								else
								{
									//textGotoXY(0,17);printf("reached end of activeCount");
									needsToWaitExpiration = false;
									theBeats[DG->selectBeat].isActive = false;
									if(theBeats[DG->selectBeat].pendingRelaunch) //oops, need to relaunch it at the end of it dying down
									{
										theBeats[DG->selectBeat].pendingRelaunch = false; //the relaunch will happen, so stop further relaunches for now
										launchBeat();
									}
									break;
								}
							}
							//otherwise proceed as normal and get the next note of that beat's track
						else {

							theBeats[DG->selectBeat].index[j]++;
							getBeatTrackNoteInfo(theBeats, DG->selectBeat, j, &noteScoop, &delayScoop, theT);
							if(theBeats[DG->selectBeat].index[j] ==  theT->count)
								{
								theBeats[DG->selectBeat].index[j] = 0;
								getBeatTrackNoteInfo(theBeats, DG->selectBeat, j, &noteScoop, &delayScoop, theT);
								}


							if((noteScoop&0x80)==0x80) {
								if (theBeats[DG->selectBeat].pending2x[j]==0) theBeats[DG->selectBeat].pending2x[j]=1;
								}

							dispatchNote(true,  theT->chan, noteScoop&0x7F, theT->chip==2?PSG_DEFAULT_VOL:0x7F, dispatchGlobals->wantVS1053,  theT->chip, true, theT->inst);
							theBeats[DG->selectBeat].activeCount++;
							theBeats[DG->selectBeat].timers[j].absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + mainTempoLUT[delayScoop];
							kernelSetTimer(&(theBeats[DG->selectBeat].timers[j]));
							}
						}
					break;
				}
            }

		else if(kernelEventData.type == kernelEvent(key.PRESSED))
			{
			dealKeyPressed(kernelEventData.u.key.raw);
			}
		else if(kernelEventData.type == kernelEvent(key.RELEASED))
			{
			dealKeyReleased(kernelEventData.u.key.raw);
			}
		}

free(theT);
return 0;
}
