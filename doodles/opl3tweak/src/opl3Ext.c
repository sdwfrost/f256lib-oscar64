/*
 * Extended OPL3 stage-write functions for the opl3tweak doodle.
 * Ported from F256KsimpleCdoodles/opl3tweak/src/muopl3.c
 * Uses f256lib naming: opl3Write instead of opl3_write.
 */

#include "f256lib.h"
#include "textUI.h"
#include "opl3Ext.h"

opl3InstrumentExtT opl3ExtInst;

void opl3ExtStageOne(void)
{
	uint8_t i = 0;
	// chip wide settings
	opl3Write(OPL_PERC, opl3_fields[0].value << 4);

	// channel wide settings
	for (i = 0; i < 9; i++) {
		opl3Write((uint16_t)OPL_CH_FEED | (uint16_t)i, 0x30 | opl3_fields[1].value);
	}
	for (i = 0; i < 9; i++) {
		opl3Write(0x0100 | (uint16_t)OPL_CH_FEED | (uint16_t)i, 0x30 | opl3_fields[1].value);
	}

	for (i = 0; i < 18; i++) opl3ExtStageTwo(i);
}

void opl3ExtStageTwo(uint8_t chan)
{
	uint16_t highb = 0x0000;
	uint8_t offset = 0x00;

	offset += chan;
	if (chan > 2) offset += 0x05;
	if (chan > 5) offset += 0x05;
	if (chan > 8)
	{
		offset -= 19;
		highb = 0x0100;
	}
	if (chan > 11) offset += 0x05;
	if (chan > 14) offset += 0x05;

	opl3Write(highb | (uint16_t)OPL_OP_TVSKF   | (uint16_t)offset,         (opl3_fields[18].value << 4) | opl3_fields[20].value);
	opl3Write(highb | (uint16_t)OPL_OP_TVSKF   | (uint16_t)(offset + 0x03), (opl3_fields[19].value << 4) | opl3_fields[21].value);

	opl3Write(highb | (uint16_t)OPL_OP_KSLVOL  | (uint16_t)offset,         (opl3_fields[16].value << 6) | (opl3_fields[12].value << 4) | opl3_fields[13].value);
	opl3Write(highb | (uint16_t)OPL_OP_KSLVOL  | (uint16_t)(offset + 0x03), (opl3_fields[17].value << 6) | (opl3_fields[14].value << 4) | opl3_fields[15].value);

	opl3Write(highb | (uint16_t)OPL_OP_AD      | (uint16_t)offset,         (opl3_fields[4].value << 4) | opl3_fields[6].value);
	opl3Write(highb | (uint16_t)OPL_OP_AD      | (uint16_t)(offset + 0x03), (opl3_fields[5].value << 4) | opl3_fields[7].value);

	opl3Write(highb | (uint16_t)OPL_OP_SR      | (uint16_t)offset,         (opl3_fields[8].value << 4) | opl3_fields[10].value);
	opl3Write(highb | (uint16_t)OPL_OP_SR      | (uint16_t)(offset + 0x03), (opl3_fields[9].value << 4) | opl3_fields[11].value);

	opl3Write(highb | (uint16_t)OPL_OP_WAV     | (uint16_t)offset,         (opl3_fields[2].value << 4));
	opl3Write(highb | (uint16_t)OPL_OP_WAV     | (uint16_t)(offset + 0x03), (opl3_fields[3].value << 4));
}
