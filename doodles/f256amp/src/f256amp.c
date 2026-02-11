// f256amp - MP3 player with spectrum analyzer for F256K2
// Ported to oscar64 from F256KsimpleCdoodles
// NOTE: This doodle requires VS1053b, file picker modules from f256lib
// which are not yet ported to the shared utilities. Those functions are
// stubbed with TODO comments below.

#include "f256lib.h"
// f256lib provides: vs1053bBoostClock, vs1053bInitBigPatch, vs1053bInitSpectrum, vs1053bGetSAValues
// f256lib provides: filePickModal_far, getTheFile_far
// textUI is per-doodle (not a shared library module)

#define CHUNK8K 0x2000
#define CHUNK4K 0x1000
#define CHUNK2K 0x0800
#define CHUNK1K 0x0400
#define CHUNK128B 0x80
#define CHUNK64B 0x40
#define CHUNK32B 0x20

#define DIRECTORY_X 1
#define DIRECTORY_Y 5

#define LCD_BAND_WIDTH    2
#define LCD_BAND_INTERV   8
#define LCD_BAND_HEIGHT   61
#define LCD_BAND_START_Y  40
#define LCD_BAND_START_X  60
#define LCD_BAND_AREA     (LCD_BAND_HEIGHT * LCD_BAND_WIDTH)

#define NB_BANDS 15

#define LCD_CMD_CMD 0xDD40
#define LCD_RST 0x10
#define LCD_BL  0x20
#define LCD_WIN_X 0x2A
#define LCD_WIN_Y 0x2B
#define LCD_WRI   0x2C
#define LCD_RD    0x2E
#define LCD_MAD   0x36
#define LCD_TE  0x40

#define LCD_CMD_DTA 0xDD41
#define LCD_PIX_LO   0xDD42
#define LCD_PIX_HI   0xDD43
#define LCD_CTRL_REG 0xDD44

#define LCD_PURE_RED  0xF800
#define LCD_PURE_GRN  0x07E0
#define LCD_PURE_BLU  0x001F
#define LCD_PURE_WHI  0xFFFF
#define LCD_PURE_BLK  0x0000

#define LCDBIN 0x20000
#define LOGOPAL 0x43000
#define LOGOBIN 0x43400
#define VSPATCH 0x56000
#define VSPLUGIN 0x59000

// TODO: Replace EMBED with oscar64 #pragma section/#embed for binary assets:
// EMBED(mac, "../assets/f256amp.bin", 0x20000);
// EMBED(allo, "../assets/mainlogo.raw.pal", 0x43000);
// EMBED(popo, "../assets/mainlogo.raw", 0x43400);
// EMBED(patch, "../assets/bigpatch.bin", 0x56000);
// EMBED(saplug, "../assets/saplugin2.bin", 0x59000);

// TODO: These structs/types require muFilePicker.h
// filePickRecord fpr;
// char finalName[64];

FILE *theMP3file;

void read8KChunk(uint8_t *, FILE *);
uint8_t openMP3File(char *);
void setTextLUT1(void);
void setTextLUT2(void);
void backgroundSetup(void);
bool K2LCD(void);
bool wrongMachineTest(void);
void clearVisible(uint16_t);
void displayImage(uint32_t);
void prepareRect(uint8_t, uint16_t, uint8_t, uint16_t);
void setLCDReverseY(void);

void read8KChunk(uint8_t *buf, FILE *f) {
	uint16_t i;
	for(i=0;i<64;i++)
		{
		fileRead((buf+i*0x80), sizeof(uint8_t), 128, f);
		}
}

uint8_t openMP3File(char *name) {
	theMP3file = fileOpen(name,"rb");
	if(theMP3file == NULL)
	{
		return 1;
	}
	return 0;
}

void setTextLUT1() {
	uint16_t c=0;
	POKE(MMU_IO_CTRL,0);
	for(c=0;c<8;c++)
	{
		POKE(0xD800+c*4,0);
		POKE(0xD801+c*4,0xFF);
		POKE(0xD802+c*4,0+(c<<5));
	}
	for(c=8;c<16;c++)
	{
		POKE(0xD800+c*4,0);
		POKE(0xD801+c*4,0xFF-((c-8)<<5));
		POKE(0xD802+c*4,0xFF);
	}
}

void setTextLUT2() {
	uint16_t c=0;
	POKE(MMU_IO_CTRL,0);
	for(c=0;c<8;c++)
		{
		POKE(0xD800+c*4,0xFF);
		POKE(0xD801+c*4,0+(c<<5));
		POKE(0xD802+c*4,0);
		}
	for(c=8;c<16;c++)
		{
		POKE(0xD800+c*4,0xFF-((c-8)<<5));
		POKE(0xD801+c*4,0xFF);
		POKE(0xD802+c*4,0);
		}
}

void colMatrixSetup()
{
	POKE(MMU_IO_CTRL,3);
	for(uint16_t h=0;h<20;h++)
		{
		for(uint16_t c=0;c<80;c++)
			{
			POKE(0xC910+c-h*80,0x0F+h*12);
			}
		}
	POKE(MMU_IO_CTRL, 0x00);
}

void backgroundSetup() {
	uint16_t c=0;

	POKE(MMU_IO_CTRL, 0);
	POKE(VKY_MSTR_CTRL_0, 0b00101111);
	POKE(VKY_MSTR_CTRL_1, 0b00000100);
	POKE(VKY_LAYER_CTRL_0, 0b00000001);
	POKE(VKY_LAYER_CTRL_1, 0b00000010);
	POKE(0xD00D,0x00);
	POKE(0xD00E,0x00);
	POKE(0xD00F,0x00);

	POKE(MMU_IO_CTRL,1);
	for(c=0;c<1023;c++)
	{
		POKE(VKY_GR_CLUT_0+c, FAR_PEEK(LOGOPAL+c));
	}
	POKE(MMU_IO_CTRL, 0);

	bitmapSetActive(0);
	bitmapSetAddress(0,LOGOBIN);
	bitmapSetCLUT(0);
	bitmapSetVisible(0,true);
	bitmapSetVisible(1,false);
	bitmapSetVisible(2,false);

	setTextLUT1();
	colMatrixSetup();
}

bool K2LCD() {
	if(platformIsK2())
	{
	displayImage(LCDBIN);
	return true;
	}
	else return false;
}

bool wrongMachineTest() {
	if(platformIsWave2() == false)
		{
		uint8_t mid = PEEK(0xD6A7);
		printf("Your machine ID is %02x.",mid);
		printf("\nIn order to work, a VS1053b chip needs to be present.");
		printf("\nThis chip is present in the Jr2 and K2.");
		printf("\nHit space if you think you have it.");
		kernelWaitKey();
		return false;
		}
	return true;
}

void clearVisible(uint16_t colorWord) {
	uint8_t i;
	uint16_t j;
	POKE(LCD_CMD_CMD, LCD_WIN_X);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 239);

	POKE(LCD_CMD_CMD, LCD_WIN_Y);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 20);
	POKE(LCD_CMD_DTA, 0x01);
	POKE(LCD_CMD_DTA, 0x40);

	POKE(LCD_CMD_CMD, LCD_WRI);

	for(j=0;j<280;j++)
	{
		for(i=0;i<240;i++)
		{
			POKEW(LCD_PIX_LO,colorWord);
		}
	}
}

void displayImage(uint32_t addr) {
	uint32_t i;
	uint32_t j;
	uint32_t index = 0;
	POKE(LCD_CMD_CMD, LCD_WIN_X);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 239);

	POKE(LCD_CMD_CMD, LCD_WIN_Y);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, 20);
	POKE(LCD_CMD_DTA, 0x01);
	POKE(LCD_CMD_DTA, 0x3F);

	POKE(LCD_CMD_CMD, LCD_WRI);

	for(j=0;j<280;j++)
	{
		for(i=0;i<240;i++)
		{
			POKE(LCD_PIX_LO, FAR_PEEK(addr + index));
			POKE(LCD_PIX_HI, FAR_PEEK(addr + index + (uint32_t)1));
			index+=2;
		}
	}
}

void prepareRect(uint8_t x, uint16_t y, uint8_t width, uint16_t height) {
	POKE(LCD_CMD_CMD, LCD_WIN_X);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, x);
	POKE(LCD_CMD_DTA, 0);
	POKE(LCD_CMD_DTA, x+width-1);

	POKE(LCD_CMD_CMD, LCD_WIN_Y);
	POKE(LCD_CMD_DTA, (y&0xFF00));
	POKE(LCD_CMD_DTA, 20 + (y&0x00FF));
	POKE(LCD_CMD_DTA, (y+height)&0x00FF);
	POKE(LCD_CMD_DTA, (y+height)&0x00FF-1);

	POKE(LCD_CMD_CMD, LCD_WRI);
}

void setLCDReverseY() {
	POKE(LCD_CMD_CMD, LCD_MAD);
	POKE(LCD_CMD_DTA, 0x80);
	POKE(LCD_CMD_CMD, LCD_WRI);
}

// TODO: writeTopRight requires fpr (filePickRecord) from muFilePicker
// void writeTopRight(bool wantCmds) { ... }

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	// TODO: Finish wiring up VS1053b and file picker from f256lib
	// modules to be fully functional. The core playback loop and display
	// code is ported above, but the main() orchestration depends on:
	// - vs1053bBoostClock() / vs1053bInitBigPatch() / vs1053bInitSpectrum() / vs1053bGetSAValues()
	// - filePickModal_far() / getTheFile_far()
	//
	// Once the main() orchestration is completed, remove stubs below.

	printf("f256amp requires muVS1053b, muFilePicker, muTextUI modules.\n");
	printf("These shared modules are not yet ported to oscar64.\n");
	kernelWaitKey();

	return 0;
}
