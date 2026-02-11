#include "f256lib.h"
// f256lib provides: SID, MIDI, dispatch, and MIDI-in modules
// textUI is per-doodle (not a shared library module)


// TODO: EMBED - original: EMBED(pianopal, "../assets/piano.pal", 0x30000); //1kb
// TODO: EMBED - original: EMBED(keys, "../assets/piano.raw", 0x30400); //

int8_t exitCode=1; //when = to 0, exit program
bool shiftTog = false;

// Tweak UI infrastructure (was in separate module pre-port)
#define MAXUIINDEX 15

typedef struct tweakFieldS {
	uint8_t value;
	bool is_dirty;
} tweakFieldT;

tweakFieldT sid_fields[MAXUIINDEX + 1];
uint8_t indexUI = 0;

// Navigation jump table: high nibble = up jump, low nibble = down jump
const uint8_t navWSJumps[MAXUIINDEX + 1] = {
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
};

// SID register names for the UI header
static const char *sidFieldNames[MAXUIINDEX + 1] = {
	"Wave", "PwdL", "PwdH", "Atk ", "Dec ", "Sus ", "Rel ", "Ctrl",
	"FcfL", "FcfH", "FRR ", "Vol ", "x12 ", "x13 ", "x14 ", "x15 "
};

void updateHighlight(uint8_t oldIdx, uint8_t newIdx) {
	// Unhighlight old
	textGotoXY(oldIdx * 5, 4);
	textSetColor(7, 0);
	textPrintHex(sid_fields[oldIdx].value, 2);
	// Highlight new
	textGotoXY(newIdx * 5, 4);
	textSetColor(0, 7);
	textPrintHex(sid_fields[newIdx].value, 2);
	indexUI = newIdx;
}

void updateValues(void) {
	for (uint8_t i = 0; i <= MAXUIINDEX; i++) {
		if (sid_fields[i].is_dirty) {
			textGotoXY(i * 5, 4);
			if (i == indexUI) textSetColor(0, 7);
			else textSetColor(7, 0);
			textPrintHex(sid_fields[i].value, 2);
			sid_fields[i].is_dirty = false;
		}
	}
}

void randomInst(void) {
	for (uint8_t i = 0; i <= MAXUIINDEX; i++) {
		sid_fields[i].value = LOW_BYTE(randomRead()) & 0x0F;
		sid_fields[i].is_dirty = true;
	}
	if (sid_fields[0].value > 7) sid_fields[0].value = 7;
	updateValues();
}

void initSIDFields(void) {
	for (uint8_t i = 0; i <= MAXUIINDEX; i++) {
		sid_fields[i].value = 0;
		sid_fields[i].is_dirty = true;
	}
}

void printInstrumentHeaders(void) {
	textSetColor(13, 0);
	textGotoXY(0, 2);
	textPrint("SID Tweak - F7:Random ESC:Quit");
	textGotoXY(0, 3);
	textSetColor(15, 0);
	for (uint8_t i = 0; i <= MAXUIINDEX; i++) {
		textPrint(sidFieldNames[i]);
		textPrint(" ");
	}
}

void setup(void);
void resetSID(void);
void dealKeyPressed(uint8_t);
void dealKeyb(void);
void goBack(uint8_t);
void goForth(uint8_t);

void setup()
{
	//openAllCODEC(); //if the VS1053b is used, this might be necessary for some board revisions

	//graphicsSetBackgroundRGB(0x2F,0x2F,0x2F);
	POKE(MMU_IO_CTRL, 0x00);
	// XXX GAMMA  SPRITE   TILE  | BITMAP  GRAPH  OVRLY  TEXT
	POKE(VKY_MSTR_CTRL_0, 0b00111111); //sprite,graph,overlay,text
	// XXX XXX  FON_SET FON_OVLY | MON_SLP DBL_Y  DBL_X  CLK_70
	POKE(VKY_MSTR_CTRL_1, 0b00000100); //font overlay, double height text, 320x240 at 60 Hz;


	POKE(0xD00D,0x00); //force black graphics background
	POKE(0xD00E,0x00);
	POKE(0xD00F,0x00);
//palette
	POKE(MMU_IO_CTRL,1);  //MMU I/O to page 1
	//prep to copy over the palette to the CLUT

	for(uint16_t c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(0x30000+c)); //palette for piano
	}
	POKE(MMU_IO_CTRL,0); //MMU I/O to page 0

	bitmapReset();
	bitmapSetActive(0);
	bitmapSetAddress(0,0x30400);
	bitmapSetVisible(0,true);
	dispatchResetGlobals(dispatchGlobals);

	textEnableBackgroundColors(true);
}

void resetSID()
{
	sidClearRegisters();
	sidSetMono();
	sidPrepInstruments();
}


// resetMID replaced by library midiInReset()
void goForth(uint8_t isVert)
	{
	if(indexUI == 15)
		{
		updateHighlight(15, 0);
		return;
		}
	else if(isVert)
		{
		uint8_t newIndex = indexUI+(navWSJumps[indexUI]&0x0F);
		if(newIndex > 15) newIndex =0;
		updateHighlight(indexUI, newIndex);
		}
	else updateHighlight(indexUI, indexUI+1);
	}

void goBack(uint8_t isVert)
	{
	if(indexUI == 0 && isVert == false)
		{
		updateHighlight(0, 15);
		return;
		}
	else if(isVert)
		{
		uint8_t newIndex = indexUI-((navWSJumps[indexUI]&0xF0)>>4);
		if(newIndex > 15) newIndex = 15- (0xFF - newIndex);
		updateHighlight(indexUI, newIndex);
		}
	else updateHighlight(indexUI, indexUI-1);
	}

void updateValPunch(uint8_t valPunch)
{
sid_fields[indexUI].value = valPunch;
if(indexUI == 0 && valPunch > 7) sid_fields[indexUI].value = 7;
sid_fields[indexUI].is_dirty = true;
updateValues();
}

void dealKeyPressed(uint8_t keyRaw)
{

	switch(keyRaw)
	{
		case 0x87: //F7
			randomInst();
			break;
		case 146: // top left backspace, meant as reset
			exitCode = 0;
			break;
		case 0x00: // left shift
		case 0x01: // right shift
			shiftTog = true;
			break;
		case 0x93: //tab
			if(shiftTog) goBack(0);
			else goForth(0);
			break;
		case 0xB6: //up
			goBack(1);
			break;
		case 0xB7: //down
			goForth(1);
			break;
		case 0xB8: //left
			goBack(0);
			break;
		case 0xB9: //right
			goForth(0);
			break;
		case 0x3D: //=+
			sid_fields[indexUI].value ++;
			if(indexUI == 0 && sid_fields[indexUI].value>0x07) sid_fields[indexUI].value = 0x00;
			if(sid_fields[indexUI].value>0x0F) sid_fields[indexUI].value = 0x00;
			sid_fields[indexUI].is_dirty = true;
			updateValues();
			break;
		case 0x2D: //-_
			if(indexUI == 0 && sid_fields[indexUI].value==0x00) sid_fields[indexUI].value = 0x07;
			else if(sid_fields[indexUI].value==0x00) sid_fields[indexUI].value = 0x0F;
			else sid_fields[indexUI].value--;
			sid_fields[indexUI].is_dirty = true;
			updateValues();
			break;
		case 0x30: //0
			updateValPunch(0);
			break;
		case 0x31: //1
			updateValPunch(1);
			break;
		case 0x32: //2
			updateValPunch(2);
			break;
		case 0x33: //3
			updateValPunch(3);
			break;
		case 0x34: //4
			updateValPunch(4);
			break;
		case 0x35: //5
			updateValPunch(5);
			break;
		case 0x36: //6
			updateValPunch(6);
			break;
		case 0x37: //7
			updateValPunch(7);
			break;
		case 0x38: //8
			updateValPunch(8);
			break;
		case 0x39: //9
			updateValPunch(9);
			break;
		case 0x61: //A
			updateValPunch(0xA);
			break;
		case 0x62: //B
			updateValPunch(0xB);
			break;
		case 0x81: //C
			updateValPunch(0xC);
			break;
		case 0x64: //D
			updateValPunch(0xD);
			break;
		case 0x65: //E
			updateValPunch(0xE);
			break;
		case 0x66: //F
			updateValPunch(0xF);
			break;

	}
	//
}

void dealKeyb()
{
	kernelNextEvent();
	if(kernelEventData.type == kernelEvent(key.PRESSED))
			{
			dealKeyPressed(kernelEventData.u.key.raw);
			}
	else if(kernelEventData.type == kernelEvent(key.RELEASED))
		{
			if(kernelEventData.u.key.raw == 0x00 || kernelEventData.u.key.raw == 0x01) shiftTog = false;
		}
}

int main(int argc, char *argv[]) {
	midiInDataT gMID;

	setup();
	resetSID();
	midiInReset(&gMID);
	initSIDFields();

	printInstrumentHeaders();
	updateValues();

	while(exitCode)
	{
	midiInProcess(&gMID);
	dealKeyb();
	}

	resetSID();
	return 0;
}
