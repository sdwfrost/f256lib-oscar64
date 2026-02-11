/*
 *	SID chip support for F256.
 *	Adapted from mu0nlibs/musid for oscar64.
 */


#ifndef WITHOUT_SID


#include "f256lib.h"


const char *sidInstrumentNames[] = {
	"Triangle",
	"SawTooth",
	"Pulse",
	"Noise",
	"Pad",
};

sidInstrumentT sidInstrumentDefs[] = {
	{ 0x0F, 0x44, 0x00, 0x27, 0x5B, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0F, 0x88, 0x00, 0x09, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0F, 0x00, 0x08, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0F, 0x44, 0x00, 0x70, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0A, 0x90, 0x04, 0xD6, 0xBA, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

const byte sidInstrumentsSize = 5;

const byte sidHigh[96] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04,
	0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x08,
	0x08, 0x08, 0x09, 0x0A, 0x0A, 0x0B, 0x0C, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
	0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1C, 0x1E, 0x20,
	0x21, 0x23, 0x26, 0x28, 0x2A, 0x2D, 0x30, 0x32, 0x35, 0x39, 0x3C, 0x40,
	0x43, 0x47, 0x4C, 0x50, 0x55, 0x5A, 0x60, 0x65, 0x6B, 0x72, 0x78, 0x80,
	0x87, 0x8F, 0x98, 0xA1, 0xAB, 0xB5, 0xC0, 0xCB, 0xD7, 0xE4, 0xF1, 0x00
};

const byte sidLow[96] = {
	0x0F, 0x1F, 0x30, 0x43, 0x56, 0x6A, 0x80, 0x96, 0xAF, 0xC8, 0xE3, 0x00,
	0x1F, 0x3F, 0x61, 0x86, 0xAC, 0xD5, 0x00, 0x2D, 0x5E, 0x91, 0xC7, 0x01,
	0x3E, 0x7F, 0xC3, 0x0C, 0x58, 0xAA, 0x00, 0x5B, 0xBC, 0x23, 0x8F, 0x02,
	0x7C, 0xFE, 0x86, 0x18, 0xB1, 0x54, 0x00, 0xB7, 0x78, 0x46, 0x1F, 0x05,
	0xF9, 0xFC, 0x0D, 0x30, 0x63, 0xA8, 0x01, 0x6E, 0xF1, 0x8C, 0x3F, 0x0B,
	0xF3, 0xF8, 0x1A, 0x60, 0xC6, 0x51, 0x02, 0xDD, 0xE3, 0x18, 0x7E, 0x16,
	0xE7, 0xF0, 0x35, 0xC0, 0x8D, 0xA3, 0x05, 0xBB, 0xC7, 0x30, 0xFD, 0x2D,
	0xCE, 0xE1, 0x6A, 0x80, 0x1A, 0x46, 0x0B, 0x77, 0x8F, 0x61, 0xFA, 0x5B
};


void sidClearRegisters(void) {
	byte i;
	for (i = 0; i <= 0x18; i++) {
		POKE(SID1 + i, 0);
		POKE(SID2 + i, 0);
	}
}


void sidShutAllVoices(void) {
	POKE(SID1 + SID_VOICE1 + SID_CTRL, PEEK(SID1 + SID_VOICE1 + SID_CTRL) & 0xFE);
	POKE(SID1 + SID_VOICE2 + SID_CTRL, PEEK(SID1 + SID_VOICE2 + SID_CTRL) & 0xFE);
	POKE(SID1 + SID_VOICE3 + SID_CTRL, PEEK(SID1 + SID_VOICE3 + SID_CTRL) & 0xFE);
	POKE(SID2 + SID_VOICE1 + SID_CTRL, PEEK(SID2 + SID_VOICE1 + SID_CTRL) & 0xFE);
	POKE(SID2 + SID_VOICE2 + SID_CTRL, PEEK(SID2 + SID_VOICE2 + SID_CTRL) & 0xFE);
	POKE(SID2 + SID_VOICE3 + SID_CTRL, PEEK(SID2 + SID_VOICE3 + SID_CTRL) & 0xFE);
}


void sidNoteOnOrOff(uint16_t voice, byte ctrl, bool isOn) {
	POKE(voice, isOn ? (ctrl | 0x01) : (ctrl & 0xFE));
}


void sidSetInstrument(byte chip, byte voice, sidInstrumentT inst) {
	uint16_t addr = (chip == 0) ? (uint16_t)SID1 : (uint16_t)SID2;
	if (voice == 1) addr += (uint16_t)SID_VOICE2;
	if (voice == 2) addr += (uint16_t)SID_VOICE3;

	POKE(addr + (uint16_t)SID_LO_PWDC, inst.pwdLo);
	POKE(addr + (uint16_t)SID_HI_PWDC, inst.pwdHi);
	POKE(addr + (uint16_t)SID_ATK_DEC, inst.ad);
	POKE(addr + (uint16_t)SID_SUS_REL, inst.sr);
	POKE(addr + (uint16_t)SID_CTRL, inst.ctrl);
}


void sidSetSIDWide(byte which) {
	POKE(SID1 + SID_LO_FCF, sidInstrumentDefs[which].fcfLo);
	POKE(SID1 + SID_HI_FCF, sidInstrumentDefs[which].fcfHi);
	POKE(SID1 + SID_FRR,    sidInstrumentDefs[which].frr);
	POKE(SID1 + SID_FM_VC,  sidInstrumentDefs[which].maxVolume);

	POKE(SID2 + SID_LO_FCF, sidInstrumentDefs[which].fcfLo);
	POKE(SID2 + SID_HI_FCF, sidInstrumentDefs[which].fcfHi);
	POKE(SID2 + SID_FRR,    sidInstrumentDefs[which].frr);
	POKE(SID2 + SID_FM_VC,  sidInstrumentDefs[which].maxVolume);
}


void sidSetInstrumentAllChannels(byte which) {
	byte c;
	for (c = 0; c < 3; c++) {
		sidSetInstrument(0, c, sidInstrumentDefs[which]);
		sidSetInstrument(1, c, sidInstrumentDefs[which]);
	}
	sidSetSIDWide(which);
}


void sidPrepInstruments(void) {
	sidSetInstrumentAllChannels(0);
}


void sidSetMono(void) {
	byte sys1 = PEEK(SID_SYS1);
	sys1 = sys1 & 0xF7;
	POKE(SID_SYS1, sys1);
}


void sidSetStereo(void) {
	byte sys1 = PEEK(SID_SYS1);
	sys1 = sys1 | 0x08;
	POKE(SID_SYS1, sys1);
}


byte sidFetchCtrl(byte which) {
	return sidInstrumentDefs[which].ctrl;
}


void sidSetCTRL(byte ctrl) {
	POKE(SID1 + SID_VOICE1 + SID_CTRL, ctrl);
	POKE(SID1 + SID_VOICE2 + SID_CTRL, ctrl);
	POKE(SID1 + SID_VOICE3 + SID_CTRL, ctrl);
	POKE(SID2 + SID_VOICE1 + SID_CTRL, ctrl);
	POKE(SID2 + SID_VOICE2 + SID_CTRL, ctrl);
	POKE(SID2 + SID_VOICE3 + SID_CTRL, ctrl);
}


void sidSetPWM(byte lo, byte hi) {
	POKE(SID1 + SID_VOICE1 + SID_LO_PWDC, lo);
	POKE(SID1 + SID_VOICE2 + SID_LO_PWDC, lo);
	POKE(SID1 + SID_VOICE3 + SID_LO_PWDC, lo);
	POKE(SID2 + SID_VOICE1 + SID_LO_PWDC, lo);
	POKE(SID2 + SID_VOICE2 + SID_LO_PWDC, lo);
	POKE(SID2 + SID_VOICE3 + SID_LO_PWDC, lo);

	POKE(SID1 + SID_VOICE1 + SID_HI_PWDC, hi);
	POKE(SID1 + SID_VOICE2 + SID_HI_PWDC, hi);
	POKE(SID1 + SID_VOICE3 + SID_HI_PWDC, hi);
	POKE(SID2 + SID_VOICE1 + SID_HI_PWDC, hi);
	POKE(SID2 + SID_VOICE2 + SID_HI_PWDC, hi);
	POKE(SID2 + SID_VOICE3 + SID_HI_PWDC, hi);
}


void sidSetFF(byte lo, byte hi) {
	POKE(SID1 + SID_LO_FCF, lo);
	POKE(SID2 + SID_LO_FCF, lo);

	POKE(SID1 + SID_HI_FCF, hi);
	POKE(SID2 + SID_HI_FCF, hi);
}


void sidSetModVol(byte data) {
	POKE(SID1 + SID_FM_VC, data);
	POKE(SID2 + SID_FM_VC, data);
}


void sidSetFILT(byte data) {
	POKE(SID1 + SID_FRR, data);
	POKE(SID2 + SID_FRR, data);
}


void sidSetADSR(byte ad, byte sr) {
	POKE(SID1 + SID_VOICE1 + SID_ATK_DEC, ad);
	POKE(SID1 + SID_VOICE2 + SID_ATK_DEC, ad);
	POKE(SID1 + SID_VOICE3 + SID_ATK_DEC, ad);
	POKE(SID2 + SID_VOICE1 + SID_ATK_DEC, ad);
	POKE(SID2 + SID_VOICE2 + SID_ATK_DEC, ad);
	POKE(SID2 + SID_VOICE3 + SID_ATK_DEC, ad);

	POKE(SID1 + SID_VOICE1 + SID_SUS_REL, sr);
	POKE(SID1 + SID_VOICE2 + SID_SUS_REL, sr);
	POKE(SID1 + SID_VOICE3 + SID_SUS_REL, sr);
	POKE(SID2 + SID_VOICE1 + SID_SUS_REL, sr);
	POKE(SID2 + SID_VOICE2 + SID_SUS_REL, sr);
	POKE(SID2 + SID_VOICE3 + SID_SUS_REL, sr);
}


void sidSetV3(byte whichChip, sidInstrumentT sI) {
	if (whichChip == 0 || whichChip == 2) {
		POKE(SID1 + SID_VOICE3 + SID_LO_B, sI.v3Lo);
		POKE(SID1 + SID_VOICE3 + SID_HI_B, sI.v3Hi);
	}
	if (whichChip == 1 || whichChip == 2) {
		POKE(SID2 + SID_VOICE3 + SID_LO_B, sI.v3Lo);
		POKE(SID2 + SID_VOICE3 + SID_HI_B, sI.v3Hi);
	}
}


void sidSetAll(sidInstrumentT sI) {
	byte i;
	uint16_t chipAddr, voiceOff;
	static const byte voiceOffs[3] = { SID_VOICE1, SID_VOICE2, SID_VOICE3 };

	for (i = 0; i < 6; i++) {
		chipAddr = (i < 3) ? SID1 : SID2;
		voiceOff = voiceOffs[i % 3];
		POKE(chipAddr + voiceOff + SID_ATK_DEC, sI.ad);
		POKE(chipAddr + voiceOff + SID_SUS_REL, sI.sr);
		POKE(chipAddr + voiceOff + SID_CTRL,    sI.ctrl);
		POKE(chipAddr + voiceOff + SID_LO_PWDC, sI.pwdLo);
		POKE(chipAddr + voiceOff + SID_HI_PWDC, sI.pwdHi);
	}
	POKE(SID1 + SID_LO_FCF, sI.fcfLo);
	POKE(SID1 + SID_HI_FCF, sI.fcfHi);
	POKE(SID1 + SID_FRR,    sI.frr);
	POKE(SID1 + SID_FM_VC,  sI.maxVolume);
	POKE(SID2 + SID_LO_FCF, sI.fcfLo);
	POKE(SID2 + SID_HI_FCF, sI.fcfHi);
	POKE(SID2 + SID_FRR,    sI.frr);
	POKE(SID2 + SID_FM_VC,  sI.maxVolume);
}


#endif
