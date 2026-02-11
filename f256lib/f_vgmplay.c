/*
 *	VGM file playback engine for F256 OPL3.
 *	Adapted from mu0nlibs/muvgmplay for oscar64.
 */


#ifndef WITHOUT_VGMPLAY


#include "f256lib.h"


uint32_t vgmTooBigWait = 0x00000000;
bool     vgmComeRightThrough;
uint32_t vgmNeedle;
uint32_t vgmTotalWait;
uint32_t vgmLoopBackTo;
uint32_t vgmSamplesSoFar;
static uint32_t vgmGd3Location;

bool vgmOneLoop = false;


FILE *vgmLoadFile(char *name) {
	FILE *theVGMfile;
	theVGMfile = fileOpen(name, "r");
	if (theVGMfile == NULL) {
		return NULL;
	}
	return theVGMfile;
}


uint8_t vgmGetStart(uint16_t ver) {
	if (ver < 0x150) return 0x40;
	else return 0x00;
}


void vgmCopyToRAM(FILE *theVGMfile) {
	bool exitFlag = true;
	char buffer[255];
	uint8_t bytesRead = 0;
	uint32_t soFar = 0;

	while (exitFlag) {
		bytesRead = fileRead(buffer, sizeof(uint8_t), 255, theVGMfile);
		if (bytesRead != 255) exitFlag = false;
		uint8_t i;
		for (i = 0; i < bytesRead; i++) {
			FAR_POKE(VGM_BODY + (uint32_t)i + (uint32_t)soFar, buffer[i]);
		}
		soFar += bytesRead;
	}
}


void vgmCheckHeader(FILE *theVGMfile) {
	uint8_t buffer[16];
	uint16_t version = 0;
	uint8_t dataOffset = 0x00;
	uint8_t i;
	vgmGd3Location = 0;
	vgmOneLoop = false;

	for (i = 0; i < 8; i++) {
		fileRead(buffer, sizeof(uint8_t), 16, theVGMfile);

		if (i == 0) {
			version = (uint16_t)(buffer[0x9] << 8) | (uint16_t)buffer[0x8];
			dataOffset = vgmGetStart(version);
		}
		if (i == 1) {
			vgmGd3Location = (uint32_t)buffer[0x4] | ((uint32_t)buffer[0x5]) << 8 |
			                 ((uint32_t)buffer[6]) << 16 | ((uint32_t)buffer[7]) << 24;
			vgmTotalWait = (uint32_t)buffer[0x8] | ((uint32_t)buffer[0x9]) << 8 |
			               ((uint32_t)buffer[0xA]) << 16 | ((uint32_t)buffer[0xB]) << 24;
			vgmLoopBackTo = (uint32_t)buffer[0xC] | ((uint32_t)buffer[0xD]) << 8 |
			                ((uint32_t)buffer[0xE]) << 16 | ((uint32_t)buffer[0xF]) << 24;
		}

		if (i == 3 && dataOffset == 0) {
			dataOffset = buffer[4] + 0x34;
		}
		if (i == 5) {
			if ((buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0 || buffer[3] != 0))
				opl3Write(0x105, 0); // set chip in OPL2 mode

			if ((buffer[12] != 0 || buffer[13] != 0 || buffer[14] != 0 || buffer[15] != 0))
				opl3Write(0x105, 1); // set chip in OPL3 mode
		}
	}

	textGotoXY(0, 2); textSetColor(7, 0);
	printf("Using v%04x", version);

	// Clear line 3 and show loading message
	textGotoXY(0, 3);
	{
		uint8_t c;
		for (c = 0; c < 80; c++) printf(" ");
	}
	textGotoXY(0, 3); textSetColor(7, 0);
	printf("Loading, please wait");

	fileSeek(theVGMfile, (uint32_t)dataOffset, SEEK_SET);
	vgmCopyToRAM(theVGMfile);

	textGotoXY(0, 3); textSetColor(7, 0);
	printf("Playing:");
	vgmNeedle = VGM_BODY;
	if (vgmLoopBackTo != 0)
		vgmLoopBackTo = VGM_BODY + (uint32_t)vgmLoopBackTo - (uint32_t)dataOffset;
	if (vgmGd3Location != 0)
		vgmGd3Location = VGM_BODY + vgmGd3Location - (uint32_t)dataOffset + (uint32_t)0x14;
	vgmSamplesSoFar = 0;

	fileClose(theVGMfile);
}


int8_t vgmPlayback(FILE *theVGMfile, bool iRT, bool pReq) {
	uint8_t nextRead;
	uint8_t reg, val;
	uint8_t hi, lo;
	int8_t canPause = 0;

	hi = 0; lo = 0;

	if (PEEK(INT_PENDING_0) & INT_TIMER_0 || vgmComeRightThrough == true) {
		if (vgmComeRightThrough == false)
			POKE(INT_PENDING_0, INT_TIMER_0);

		if (vgmTooBigWait > 0 && vgmComeRightThrough == false) {
			if (vgmTooBigWait > 0x00FFFFFF) {
				timer0Set(0x00FFFFFF);
				vgmTooBigWait -= 0x00FFFFFF;
				return 0;
			} else {
				timer0Set(vgmTooBigWait);
				vgmTooBigWait = 0;
				return 0;
			}
		} else {
			vgmComeRightThrough = false;

			if ((vgmSamplesSoFar >= vgmTotalWait || vgmNeedle >= vgmGd3Location) && vgmGd3Location != 0) {
				if (vgmLoopBackTo == 0 || vgmOneLoop == true) return -1;
				vgmNeedle = vgmLoopBackTo;
				vgmSamplesSoFar = 0;
				vgmOneLoop = true;
			}
			nextRead = FAR_PEEK(vgmNeedle++);

			// OPL2 write (YMF3812)
			if (nextRead == 0x5A) {
				reg = FAR_PEEK(vgmNeedle++);
				val = FAR_PEEK(vgmNeedle++);
				opl3Write(reg, val);
				opl3Shadow(reg, val, 0);
				vgmComeRightThrough = true;
			}
			// YMF262 write port 0
			else if (nextRead == 0x5E) {
				reg = FAR_PEEK(vgmNeedle++);
				val = FAR_PEEK(vgmNeedle++);
				opl3Write(reg, val);
				opl3Shadow(reg, val, 0);
				vgmComeRightThrough = true;
			}
			// YMF262 write port 1
			else if (nextRead == 0x5F) {
				reg = FAR_PEEK(vgmNeedle++);
				val = FAR_PEEK(vgmNeedle++);
				opl3Write((0x100 | (uint16_t)reg), val);
				opl3Shadow(reg, val, 1);
				vgmComeRightThrough = true;
			}
			// Wait n samples
			else if (nextRead == 0x61) {
				lo = FAR_PEEK(vgmNeedle++);
				hi = FAR_PEEK(vgmNeedle++);
				vgmSamplesSoFar += (uint32_t)lo | ((uint32_t)hi) << 8;
				vgmTooBigWait = (((uint32_t)hi << 8) | (uint32_t)lo) * (uint32_t)0x23A;
				if (vgmTooBigWait > 0x00FFFFFF) {
					timer0Set(0x00FFFFFF);
					vgmTooBigWait -= 0x00FFFFFF;
				} else {
					timer0Set(vgmTooBigWait);
					vgmTooBigWait = 0;
				}
			}
			// Wait 1/60th second
			else if (nextRead == 0x62) {
				timer0Set(0x66666);
				vgmSamplesSoFar += 735;
			}
			// Wait 1/50th second
			else if (nextRead == 0x63) {
				timer0Set(0x7aE15);
				vgmSamplesSoFar += 882;
			}
			// End of sound data
			else if (nextRead == 0x66) {
				return -1;
			}
			// Data block
			else if (nextRead == 0x67) {
				vgmNeedle += 2;
				uint32_t skippy = FAR_PEEK(vgmNeedle) |
				                  ((uint32_t)FAR_PEEK(vgmNeedle + 1) << 8) |
				                  ((uint32_t)FAR_PEEK(vgmNeedle + 2) << 16) |
				                  ((uint32_t)FAR_PEEK(vgmNeedle + 3) << 24);
				vgmNeedle += 4 + skippy;
				vgmComeRightThrough = true;
			}
			// Skip 1 data byte commands (0x30-0x3F, 0x4F, 0x50)
			else if ((nextRead >= 0x30 && nextRead <= 0x3F) ||
			         nextRead == 0x4F || nextRead == 0x50) {
				vgmNeedle++;
				vgmComeRightThrough = true;
			}
			// Skip 2 data byte commands
			else if ((nextRead >= 0x40 && nextRead <= 0x4E) ||
			         (nextRead >= 0x51 && nextRead <= 0x59) ||
			         (nextRead >= 0x5B && nextRead <= 0x5D) ||
			         nextRead == 0xA0 ||
			         (nextRead >= 0xB0 && nextRead <= 0xC8)) {
				vgmNeedle += 2;
				vgmComeRightThrough = true;
			}
			// Skip 3 data byte commands
			else if ((nextRead >= 0xC9 && nextRead <= 0xCF) ||
			         (nextRead >= 0xD0 && nextRead <= 0xDF)) {
				vgmNeedle += 3;
				vgmComeRightThrough = true;
			}
			// Skip 4 data byte commands
			else if (nextRead >= 0xE2) {
				vgmNeedle += 4;
				vgmComeRightThrough = true;
			}
			// Unknown command
			else {
				vgmComeRightThrough = true;
			}
		}
	}

	if (pReq) return canPause;
	else return 0;
}


#endif
