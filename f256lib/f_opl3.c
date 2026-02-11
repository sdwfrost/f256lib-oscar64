/*
 *	OPL3 (YMF262) FM synthesis support for F256.
 *	Adapted from mu0nlibs/muopl3 for oscar64.
 */


#ifndef WITHOUT_OPL3


#include "f256lib.h"


opl3InstrumentT opl3InstrumentDefs[18];
byte opl3ChipVTPerc;
byte opl3ChipPairs;
byte opl3ChipEnable;
byte opl3ChipNotesel;

const uint16_t opl3Fnums[12] = {
	0x205, 0x223, 0x244, 0x267, 0x28B, 0x2B2,
	0x2DB, 0x306, 0x334, 0x365, 0x399, 0x3CF
};


static byte reverseChan(byte inject) {
	if (inject > 0x0F) return 6 + inject - 0x10;
	else if (inject > 0x07) return 3 + inject - 0x08;
	else return inject;
}


void opl3Write(uint16_t address, byte value) {
	if (address < 0x100) {
		POKE(OPL_ADDR_L, (byte)address);
	} else {
		POKE(OPL_ADDR_H, (byte)(address & 0xFF));
	}
	POKE(OPL_DATA, value);
}


void opl3InitializeDefs(void) {
	byte i;
	opl3ChipVTPerc = 0;
	opl3ChipPairs = 0;
	opl3ChipEnable = 0;
	for (i = 0; i < 18; i++) {
		opl3InstrumentDefs[i].OP2_TVSKF = 0;
		opl3InstrumentDefs[i].OP1_TVSKF = 0;
		opl3InstrumentDefs[i].OP2_KSLVOL = 0;
		opl3InstrumentDefs[i].OP1_KSLVOL = 0;
		opl3InstrumentDefs[i].OP2_AD = 0;
		opl3InstrumentDefs[i].OP1_AD = 0;
		opl3InstrumentDefs[i].OP2_SR = 0;
		opl3InstrumentDefs[i].OP1_SR = 0;
		opl3InstrumentDefs[i].OP2_WAV = 0;
		opl3InstrumentDefs[i].OP1_WAV = 0;
		opl3InstrumentDefs[i].CHAN_FEED = 0;
		opl3InstrumentDefs[i].CHAN_FRLO = 0;
		opl3InstrumentDefs[i].CHAN_FNUM = 0;
		opl3InstrumentDefs[i].KEYHIT = 0x00;
	}
}


void opl3QuietAll(void) {
	byte channel;
	for (channel = 0; channel < 9; channel++) {
		opl3Write(OPL_CH_KBF_HI | channel, opl3InstrumentDefs[channel].CHAN_FNUM & 0xDF);
	}
	for (channel = 0; channel < 9; channel++) {
		opl3Write(0x0100 | (uint16_t)OPL_CH_KBF_HI | (uint16_t)channel,
		          opl3InstrumentDefs[channel + 9].CHAN_FNUM & 0xDF);
	}
}


void opl3Initialize(void) {
	byte i;

	opl3Write(OPL_EN, 0x20);
	opl3Write(OPL_T1, 0x00);
	opl3Write(OPL_T2, 0x00);
	opl3Write(OPL_FOE, 0x00);
	opl3Write(OPL_OPL3, 0x01);
	opl3Write(OPL_CSW, 0x00);
	opl3Write(OPL_PERC, 0x00);

	for (i = 0; i < 9; i++) {
		opl3Write((uint16_t)OPL_CH_FEED | (uint16_t)i, 0x30);
	}
	for (i = 0; i < 9; i++) {
		opl3Write(0x0100 | (uint16_t)OPL_CH_FEED | (uint16_t)i, 0x30);
	}
	opl3QuietAll();
}


void opl3SetInstrument(opl3InstrumentT inst, byte chan) {
	uint16_t highb = 0x0000;
	byte offset = 0x00;

	offset += chan;
	if (chan > 2)  offset += 0x05;
	if (chan > 5)  offset += 0x05;
	if (chan > 8) {
		offset -= 19;
		highb = 0x0100;
	}
	if (chan > 11) offset += 0x05;
	if (chan > 14) offset += 0x05;

	opl3Write(highb | (uint16_t)OPL_OP_WAV    | (uint16_t)offset,                    inst.OP1_WAV);
	opl3Write(highb | (uint16_t)OPL_OP_WAV    | ((uint16_t)offset + (uint16_t)0x03), inst.OP2_WAV);

	opl3Write(highb | (uint16_t)OPL_OP_TVSKF  | (uint16_t)offset,                    inst.OP1_TVSKF);
	opl3Write(highb | (uint16_t)OPL_OP_TVSKF  | ((uint16_t)offset + (uint16_t)0x03), inst.OP2_TVSKF);

	opl3Write(highb | (uint16_t)OPL_OP_KSLVOL | (uint16_t)offset,                    inst.OP1_KSLVOL);
	opl3Write(highb | (uint16_t)OPL_OP_KSLVOL | ((uint16_t)offset + (uint16_t)0x03), inst.OP2_KSLVOL);

	opl3Write(highb | (uint16_t)OPL_OP_AD     | (uint16_t)offset,                    inst.OP1_AD);
	opl3Write(highb | (uint16_t)OPL_OP_AD     | ((uint16_t)offset + (uint16_t)0x03), inst.OP2_AD);

	opl3Write(highb | (uint16_t)OPL_OP_SR     | (uint16_t)offset,                    inst.OP1_SR);
	opl3Write(highb | (uint16_t)OPL_OP_SR     | ((uint16_t)offset + (uint16_t)0x03), inst.OP2_SR);
}


void opl3SetFeed(byte val, byte which) {
	if (which > 8) {
		opl3Write(0x100 | (uint16_t)OPL_CH_FEED | (uint16_t)which, val);
	} else {
		opl3Write((uint16_t)OPL_CH_FEED | (uint16_t)which, val);
	}
}


void opl3SetFrLo(byte val, byte which) {
	if (which > 8) {
		opl3Write(0x100 | (uint16_t)OPL_CH_F_LO | (uint16_t)which, val);
	} else {
		opl3Write((uint16_t)OPL_CH_F_LO | (uint16_t)which, val);
	}
}


void opl3SetFnum(byte val, byte which) {
	if (which > 8) {
		opl3Write(0x100 | (uint16_t)OPL_CH_KBF_HI | (uint16_t)which, val);
	} else {
		opl3Write((uint16_t)OPL_CH_KBF_HI | (uint16_t)which, val);
	}
}


void opl3SetInstrumentAllChannels(byte which, bool steamRoll) {
	byte indx;

	opl3Write(OPL_EN, opl3ChipEnable);
	opl3Write(OPL_PERC, opl3ChipVTPerc & 0xE0);
	opl3Write(OPL_CSW, opl3ChipNotesel);

	for (indx = 0; indx < 18; indx++) {
		if (steamRoll) {
			opl3SetFeed(opl3InstrumentDefs[which].CHAN_FEED, indx);
			opl3SetFrLo(opl3InstrumentDefs[which].CHAN_FRLO, indx);
			opl3SetFnum(opl3InstrumentDefs[which].CHAN_FNUM & 0xDF, indx);
			opl3SetInstrument(opl3InstrumentDefs[which], indx);
		} else {
			opl3SetFeed(opl3InstrumentDefs[indx].CHAN_FEED, indx);
			if (indx == 7 && opl3ChipVTPerc > 0) {
				opl3Write(OPL_OP_WAV    | (uint16_t)0x14, opl3InstrumentDefs[indx].OP2_WAV);
				opl3Write(OPL_OP_TVSKF  | (uint16_t)0x14, opl3InstrumentDefs[indx].OP2_TVSKF);
				opl3Write(OPL_OP_KSLVOL | (uint16_t)0x14, opl3InstrumentDefs[indx].OP2_KSLVOL);
				opl3Write(OPL_OP_AD     | (uint16_t)0x14, opl3InstrumentDefs[indx].OP2_AD);
				opl3Write(OPL_OP_SR     | (uint16_t)0x14, opl3InstrumentDefs[indx].OP2_SR);
				opl3Write(OPL_OP_WAV    | (uint16_t)0x11, opl3InstrumentDefs[indx].OP1_WAV);
				opl3Write(OPL_OP_TVSKF  | (uint16_t)0x11, opl3InstrumentDefs[indx].OP1_TVSKF);
				opl3Write(OPL_OP_KSLVOL | (uint16_t)0x11, opl3InstrumentDefs[indx].OP1_KSLVOL);
				opl3Write(OPL_OP_AD     | (uint16_t)0x11, opl3InstrumentDefs[indx].OP1_AD);
				opl3Write(OPL_OP_SR     | (uint16_t)0x11, opl3InstrumentDefs[indx].OP1_SR);
			} else if (indx == 8 && opl3ChipVTPerc > 0) {
				opl3Write(OPL_OP_WAV    + (uint16_t)0x15, opl3InstrumentDefs[indx].OP2_WAV);
				opl3Write(OPL_OP_TVSKF  + (uint16_t)0x15, opl3InstrumentDefs[indx].OP2_TVSKF);
				opl3Write(OPL_OP_KSLVOL + (uint16_t)0x15, opl3InstrumentDefs[indx].OP2_KSLVOL);
				opl3Write(OPL_OP_AD     + (uint16_t)0x15, opl3InstrumentDefs[indx].OP2_AD);
				opl3Write(OPL_OP_SR     + (uint16_t)0x15, opl3InstrumentDefs[indx].OP2_SR);
				opl3Write(OPL_OP_WAV    + (uint16_t)0x12, opl3InstrumentDefs[indx].OP1_WAV);
				opl3Write(OPL_OP_TVSKF  + (uint16_t)0x12, opl3InstrumentDefs[indx].OP1_TVSKF);
				opl3Write(OPL_OP_KSLVOL + (uint16_t)0x12, opl3InstrumentDefs[indx].OP1_KSLVOL);
				opl3Write(OPL_OP_AD     + (uint16_t)0x12, opl3InstrumentDefs[indx].OP1_AD);
				opl3Write(OPL_OP_SR     + (uint16_t)0x12, opl3InstrumentDefs[indx].OP1_SR);
			} else {
				opl3SetInstrument(opl3InstrumentDefs[indx], indx);
			}
		}
	}
}


byte opl3Shadow(byte offset, byte value, byte hinb) {
	byte chan = 0;
	byte temp = 0;
	byte isHitRelease = 0;

	if (hinb == 1) {
		chan = 9;
	}

	switch (offset) {
		case 0x01:
			opl3ChipEnable = value;
			break;
		case 0x04:
			if (hinb == 1) {
				opl3ChipPairs = value;
			}
			break;
		case 0x08:
			opl3ChipNotesel = value;
			break;
		case 0xBD:
			opl3ChipVTPerc = value;
			break;
		default:
			if (offset >= 0x20 && offset <= 0x22) {
				temp = offset - 0x20; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_TVSKF = value;
			} else if (offset >= 0x28 && offset <= 0x2A) {
				temp = offset - 0x20; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_TVSKF = value;
			} else if (offset >= 0x30 && offset <= 0x32) {
				temp = offset - 0x20; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_TVSKF = value;
			} else if (offset >= 0x23 && offset <= 0x25) {
				temp = offset - 0x23; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_TVSKF = value;
			} else if (offset >= 0x2B && offset <= 0x2D) {
				temp = offset - 0x23; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_TVSKF = value;
			} else if (offset >= 0x33 && offset <= 0x35) {
				temp = offset - 0x23; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_TVSKF = value;
			} else if (offset >= 0x40 && offset <= 0x42) {
				temp = offset - 0x40; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_KSLVOL = value;
			} else if (offset >= 0x48 && offset <= 0x4A) {
				temp = offset - 0x40; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_KSLVOL = value;
			} else if (offset >= 0x50 && offset <= 0x52) {
				temp = offset - 0x40; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_KSLVOL = value;
			} else if (offset >= 0x43 && offset <= 0x45) {
				temp = offset - 0x43; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_KSLVOL = value;
			} else if (offset >= 0x4B && offset <= 0x4D) {
				temp = offset - 0x43; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_KSLVOL = value;
			} else if (offset >= 0x53 && offset <= 0x55) {
				temp = offset - 0x43; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_KSLVOL = value;
			} else if (offset >= 0x60 && offset <= 0x62) {
				temp = offset - 0x60; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_AD = value;
			} else if (offset >= 0x68 && offset <= 0x6A) {
				temp = offset - 0x60; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_AD = value;
			} else if (offset >= 0x70 && offset <= 0x72) {
				temp = offset - 0x60; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_AD = value;
			} else if (offset >= 0x63 && offset <= 0x65) {
				temp = offset - 0x63; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_AD = value;
			} else if (offset >= 0x6B && offset <= 0x6D) {
				temp = offset - 0x63; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_AD = value;
			} else if (offset >= 0x73 && offset <= 0x75) {
				temp = offset - 0x63; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_AD = value;
			} else if (offset >= 0x80 && offset <= 0x82) {
				temp = offset - 0x80; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_SR = value;
			} else if (offset >= 0x88 && offset <= 0x8A) {
				temp = offset - 0x80; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_SR = value;
			} else if (offset >= 0x90 && offset <= 0x92) {
				temp = offset - 0x80; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_SR = value;
			} else if (offset >= 0x83 && offset <= 0x85) {
				temp = offset - 0x83; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_SR = value;
			} else if (offset >= 0x8B && offset <= 0x8D) {
				temp = offset - 0x83; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_SR = value;
			} else if (offset >= 0x93 && offset <= 0x95) {
				temp = offset - 0x83; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_SR = value;
			} else if (offset >= 0xE0 && offset <= 0xE2) {
				temp = offset - 0xE0; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_WAV = value;
			} else if (offset >= 0xE8 && offset <= 0xEA) {
				temp = offset - 0xE0; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_WAV = value;
			} else if (offset >= 0xF0 && offset <= 0xF2) {
				temp = offset - 0xE0; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP1_WAV = value;
			} else if (offset >= 0xE3 && offset <= 0xE5) {
				temp = offset - 0xE3; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_WAV = value;
			} else if (offset >= 0xEB && offset <= 0xED) {
				temp = offset - 0xE3; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_WAV = value;
			} else if (offset >= 0xF3 && offset <= 0xF5) {
				temp = offset - 0xE3; chan += reverseChan(temp);
				opl3InstrumentDefs[chan].OP2_WAV = value;
			} else if (offset >= 0xC0 && offset <= 0xC8) {
				chan += (offset - 0xC0);
				opl3InstrumentDefs[chan].CHAN_FEED = value;
			} else if (offset >= 0xA0 && offset <= 0xA8) {
				chan += (offset - 0xA0);
				opl3InstrumentDefs[chan].CHAN_FRLO = value;
			} else if (offset >= 0xB0 && offset <= 0xB8) {
				chan += (offset - 0xB0);
				if ((opl3InstrumentDefs[chan].KEYHIT & 0x20) != (value & 0x20))
					isHitRelease = 1;
				opl3InstrumentDefs[chan].KEYHIT = (value & 0x20);
				opl3InstrumentDefs[chan].CHAN_FNUM = value;
			}
			break;
	}

	return isHitRelease;
}


void opl3Note(byte channel, uint16_t fnum, byte block, bool onOrOff) {
	uint16_t hb = 0x0000;
	byte reduce = 0;
	if (channel > 8) {
		hb = 0x0100;
		reduce = 9;
	}
	opl3Write(hb | (uint16_t)OPL_CH_F_LO | (uint16_t)(channel - reduce), fnum & 0xFF);
	opl3Write(hb | (uint16_t)OPL_CH_KBF_HI | (uint16_t)(channel - reduce),
	          ((fnum >> 8) & 0x03) | ((uint16_t)block << 2) | (onOrOff ? 0x20 : 0x00));
}


#endif
