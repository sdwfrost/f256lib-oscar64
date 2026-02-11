/*
 * Text UI for OPL3 tweaker.
 * Ported from F256KsimpleCdoodles/opl3tweak/src/textUI.c
 * Adapted for f256lib naming conventions.
 */

#include "f256lib.h"
#include "opl3Ext.h"
#include "textUI.h"


struct OPL3Field opl3_fields[MAXUIINDEX];
uint8_t indexUI = 0;

uint8_t navWSJumps[MAXUIINDEX] = {0x21,
						  0x11,
						  0x12,0x22,
						  0x22,0x22,
						  0x22,0x22,
						  0x22,0x22,
						  0x23,0x23,
				     0x24,0x33,0x33,0x42,
						  0x32,0x32,0x22,0x22,0x22,0x21
						  }; //high nibble how to jump back, low nibble how to jump forward

void init_opl3_field(struct OPL3Field *f, uint8_t *target, bool highNib, uint8_t x, uint8_t y, uint8_t startVal) {
    f->value = startVal;
    f->opl3IPtr = target;
    f->isHighNib = highNib;
    f->is_dirty = true;
    f->tX = x;
    f->tY = y;
}

void randomInst()
{
opl3_fields[0].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[1].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[2].value=  (uint8_t)randomRead()&0x03;
opl3_fields[3].value=  (uint8_t)randomRead()&0x03;
opl3_fields[4].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[5].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[6].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[7].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[8].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[9].value=  (uint8_t)randomRead()&0x0F;
opl3_fields[10].value= (uint8_t)randomRead()&0x0F;
opl3_fields[11].value= (uint8_t)randomRead()&0x0F;
opl3_fields[16].value= (uint8_t)randomRead()&0x03;
opl3_fields[17].value= (uint8_t)randomRead()&0x03;
opl3_fields[18].value= (uint8_t)randomRead()&0x0F;
opl3_fields[19].value= (uint8_t)randomRead()&0x0F;
opl3_fields[20].value= (uint8_t)randomRead()&0x0F;
opl3_fields[21].value= (uint8_t)randomRead()&0x0F;

for(uint8_t i=0; i<MAXUIINDEX; i++) opl3_fields[i].is_dirty=true;
updateValues();
}


void initSIDFields()
{
	init_opl3_field(&(opl3_fields[0]), &(opl3ExtInst.VT_DEPTH),      true, 34, 3, 0x00);
	init_opl3_field(&(opl3_fields[1]), &(opl3ExtInst.CHAN_FEED),    false, 34, 6, 0x08);
	init_opl3_field(&(opl3_fields[2]), &(opl3ExtInst.OP2_WAV),      false, 34, 9, 0x00);
	init_opl3_field(&(opl3_fields[3]), &(opl3ExtInst.OP1_WAV),      false, 35, 9, 0x01);
	init_opl3_field(&(opl3_fields[4]), &(opl3ExtInst.OP2_AD),        true, 34,10, 0x05);
	init_opl3_field(&(opl3_fields[5]), &(opl3ExtInst.OP1_AD),        true, 35,10, 0x08);
	init_opl3_field(&(opl3_fields[6]), &(opl3ExtInst.OP2_AD),       false, 34,11, 0x01);
	init_opl3_field(&(opl3_fields[7]), &(opl3ExtInst.OP1_AD),       false, 35,11, 0x03);
	init_opl3_field(&(opl3_fields[8]), &(opl3ExtInst.OP2_SR),        true, 34,12, 0x08);
	init_opl3_field(&(opl3_fields[9]), &(opl3ExtInst.OP1_SR),        true, 35,12, 0x08);
	init_opl3_field(&(opl3_fields[10]),&(opl3ExtInst.OP2_SR),       false, 34,13, 0x08);
	init_opl3_field(&(opl3_fields[11]),&(opl3ExtInst.OP1_SR),       false, 35,13, 0x08);
	init_opl3_field(&(opl3_fields[12]),&(opl3ExtInst.OP2_KSLVOL),    true, 33,14, 0x00);
	init_opl3_field(&(opl3_fields[13]),&(opl3ExtInst.OP2_KSLVOL),   false, 34,14, 0x08);
	init_opl3_field(&(opl3_fields[14]),&(opl3ExtInst.OP1_KSLVOL),    true, 35,14, 0x00);
	init_opl3_field(&(opl3_fields[15]),&(opl3ExtInst.OP1_KSLVOL),   false, 36,14, 0x08);
	init_opl3_field(&(opl3_fields[16]),&(opl3ExtInst.OP2_KSLVOL),    true, 34,15, 0x00);
	init_opl3_field(&(opl3_fields[17]),&(opl3ExtInst.OP1_KSLVOL),    true, 35,15, 0x00);
	init_opl3_field(&(opl3_fields[18]),&(opl3ExtInst.OP2_TVSKF),     true, 34,16, 0x02);
	init_opl3_field(&(opl3_fields[19]),&(opl3ExtInst.OP1_TVSKF),     true, 35,16, 0x02);
	init_opl3_field(&(opl3_fields[20]),&(opl3ExtInst.OP2_TVSKF),    false, 34,17, 0x02);
	init_opl3_field(&(opl3_fields[21]),&(opl3ExtInst.OP1_TVSKF),    false, 35,17, 0x02);
}

void fieldToChip(uint8_t which)
{
	uint8_t temp = opl3_fields[which].value;
	if(opl3_fields[which].isHighNib) temp = temp << 4;
	*(opl3_fields[which].opl3IPtr) = (*(opl3_fields[which].opl3IPtr)) | temp;
}

void printInstrumentHeaders()
{
	uint8_t curLine = 2;

	textGotoXY(0,0);textSetColor(0x0F,0x03);textPrint("OPL3 Tweak v0.1");

	textGotoXY(0,21);
					        textPrint("[ESC]");
	textSetColor(0x0F,0x00);textPrint(" Quit ");
    textSetColor(0x0F,0x03);textPrint("[F1]");
	textSetColor(0x0F,0x00);textPrint(" Help ");
    textSetColor(0x0F,0x03);textPrint("[F3]");
	textSetColor(0x0F,0x00);textPrint(" Load ");
    textSetColor(0x0F,0x03);textPrint("[F5]");
	textSetColor(0x0F,0x00);textPrint(" Save ");
    textSetColor(0x0F,0x03);textPrint("[F7]");
	textSetColor(0x0F,0x00);textPrint(" Randomize!");

	textSetColor(0x0F,0x0A);
	textGotoXY(1,curLine++); printf("       OPL3 Chip wide settings:");

	textSetColor(0x0A,0x00);
	textGotoXY(1,curLine++); printf("         Tremo/Vibra/Perc Mode:");

	curLine++;

	textSetColor(0x0F,0x0A);
	textGotoXY(1,curLine++); printf("              Channel settings:");

	textSetColor(0x0A,0x00);
	textGotoXY(1,curLine++); printf("            Feedback/Algorithm:");

	curLine++;

	textSetColor(0x0F,0x0A);
	textGotoXY(1,curLine++); printf(" Modulator-Carrier Op settings:");

	textSetColor(0x0A,0x00);
	textGotoXY(1,curLine++); printf("                      Waveform:");
	textGotoXY(1,curLine++); printf("                        Attack:");
	textGotoXY(1,curLine++); printf("                         Decay:");
	textGotoXY(1,curLine++); printf("                       Sustain:");
	textGotoXY(1,curLine++); printf("                       Release:");
	textGotoXY(1,curLine++); printf("                        Volume:");
	textGotoXY(1,curLine++); printf("                Volume Scaling:");
	textGotoXY(1,curLine++); printf("  Tremo/Vibra/EnvType/EnvScale:");
	textGotoXY(1,curLine++); printf("               Freq Multiplier:");

	curLine++;
	textSetColor(0x0F,0x0A);
	textGotoXY(1,curLine++); printf("             Current polyphony:");
}

void updateValues()
{
	graphicsWaitVerticalBlank();

	textSetColor(0x0E,0x00);
	for(uint8_t i=0; i<MAXUIINDEX; i++)
		{
		if(i==indexUI) textSetColor(0x0F,0x03);
		else textSetColor(0x03,0x00);
		if(opl3_fields[i].is_dirty)
			{
			textGotoXY(opl3_fields[i].tX, opl3_fields[i].tY);printf("%01x",opl3_fields[i].value);
			fieldToChip(i);
			opl3_fields[i].is_dirty = false;
			}
		}
	textGotoXY(34,19);
	if((opl3_fields[5].value & 0x80) == 0x80) textPrint("4");
	else textPrint("18");

	opl3ExtStageOne();
}

void updateHighlight(uint8_t old, uint8_t nouv)
	{
	textSetColor(0x03,0x00);textGotoXY(opl3_fields[old].tX, opl3_fields[old].tY);printf("%01x",opl3_fields[old].value);
	textSetColor(0x0F,0x03);textGotoXY(opl3_fields[nouv].tX, opl3_fields[nouv].tY);printf("%01x",opl3_fields[nouv].value);
	indexUI = nouv;
	}
