/*
 *	Multi-chip note dispatch for polyphonic playback.
 *	Adapted from mu0nlibs/mudispatch for oscar64.
 */


#ifndef WITHOUT_DISPATCH


#include "f256lib.h"


// Chip activity counters for text UI display
uint8_t dispatchChipAct[5] = {0, 0, 0, 0, 0};

// Polyphony buffers for SID (6 voices across 2 chips)
static uint8_t polySIDBuffer[6] = {0, 0, 0, 0, 0, 0};
static uint8_t sidChoiceToVoice[6] = {SID_VOICE1, SID_VOICE2, SID_VOICE3, SID_VOICE1, SID_VOICE2, SID_VOICE3};
uint8_t dispatchReservedSID[6] = {0, 0, 0, 0, 0, 0};

// Polyphony buffers for PSG (6 channels)
static uint8_t polyPSGBuffer[6] = {0, 0, 0, 0, 0, 0};
static uint8_t polyPSGChanBits[6] = {0x00, 0x20, 0x40, 0x00, 0x20, 0x40};
uint8_t dispatchReservedPSG[6] = {0, 0, 0, 0, 0, 0};

// Polyphony buffers for OPL3 (9 channels)
static uint8_t polyOPL3Buffer[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t dispatchReservedOPL3[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

dispatchGlobalsT *dispatchGlobals;


void dispatchResetGlobals(dispatchGlobalsT *gT) {
	gT->wantVS1053 = false;
	gT->sidInstChoice = 0;
	gT->opl3InstChoice = 0;
	gT->chipChoice = 0;
}


int8_t dispatchFindFreeChannel(uint8_t *ptr, uint8_t howManyChans, uint8_t *reserved) {
	uint8_t i;
	for (i = 0; i < howManyChans; i++) {
		if (reserved[i]) continue;
		if (ptr[i] == 0) return i;
	}
	return -1;
}


int8_t dispatchLiberateChannel(uint8_t note, uint8_t *ptr, uint8_t howManyChans) {
	uint8_t i;
	for (i = 0; i < howManyChans; i++) {
		if (ptr[i] == note) return i;
	}
	return -1;
}


void dispatchNote(bool isOn, uint8_t channel, uint8_t note, uint8_t speed,
                  bool wantAlt, uint8_t whichChip, bool isBeat, uint8_t beatChan) {
	uint16_t sidTarget, sidVoiceBase;
	int8_t foundFreeChan = -1;

	if (isOn && note == 0) return;

	if (whichChip == 0 || whichChip == 4) { // MIDI
		if (isOn) {
			midiNoteOn(channel, note, speed, whichChip == 4 ? true : wantAlt);
			if (wantAlt) dispatchChipAct[1]++;
			else dispatchChipAct[0]++;
		} else {
			midiNoteOff(channel, note, speed, whichChip == 4 ? true : wantAlt);
			if (wantAlt) { if (dispatchChipAct[1]) dispatchChipAct[1]--; }
			else { if (dispatchChipAct[0]) dispatchChipAct[0]--; }
		}
		return;
	}

	if (whichChip == 1) { // SID
		if (isOn) {
			if (isBeat) {
				foundFreeChan = channel;
				polySIDBuffer[channel] = note;
			} else {
				foundFreeChan = dispatchFindFreeChannel(polySIDBuffer, 6, dispatchReservedSID);
			}
			if (foundFreeChan != -1) {
				sidTarget = foundFreeChan > 2 ? SID2 : SID1;
				sidVoiceBase = sidChoiceToVoice[foundFreeChan];
				POKE(sidTarget + sidVoiceBase + SID_LO_B, sidLow[note - 11]);
				POKE(sidTarget + sidVoiceBase + SID_HI_B, sidHigh[note - 11]);
				sidNoteOnOrOff(sidTarget + sidVoiceBase + SID_CTRL, dispatchGlobals->sidValues->ctrl, isOn);
				polySIDBuffer[foundFreeChan] = note;
			}
			dispatchChipAct[2]++;
		} else {
			if (isBeat) {
				foundFreeChan = channel;
				polySIDBuffer[channel] = 0;
			} else {
				foundFreeChan = dispatchLiberateChannel(note, polySIDBuffer, 6);
			}
			if (foundFreeChan >= 0) {
				sidTarget = foundFreeChan > 2 ? SID2 : SID1;
				sidVoiceBase = sidChoiceToVoice[foundFreeChan];
				polySIDBuffer[foundFreeChan] = 0;
				POKE(sidTarget + sidVoiceBase + SID_LO_B, sidLow[note - 11]);
				POKE(sidTarget + sidVoiceBase + SID_HI_B, sidHigh[note - 11]);
				sidNoteOnOrOff(sidTarget + sidVoiceBase + SID_CTRL, dispatchGlobals->sidValues->ctrl, isOn);
			}
			if (dispatchChipAct[2]) dispatchChipAct[2]--;
		}
		return;
	}

	if (whichChip == 2) { // PSG
		if (isOn) {
			if (isBeat) foundFreeChan = channel;
			else foundFreeChan = dispatchFindFreeChannel(polyPSGBuffer, 6, dispatchReservedPSG);
			if (foundFreeChan != -1) {
				psgNoteOn(polyPSGChanBits[foundFreeChan],
				          foundFreeChan > 2 ? PSG_RIGHT : PSG_LEFT,
				          psgLow[note - 45], psgHigh[note - 45],
				          speed);
				polyPSGBuffer[foundFreeChan] = note;
			}
			dispatchChipAct[3]++;
		} else {
			foundFreeChan = dispatchLiberateChannel(note, polyPSGBuffer, 6);
			if (foundFreeChan >= 0) {
				polyPSGBuffer[foundFreeChan] = 0;
				psgNoteOff(polyPSGChanBits[foundFreeChan], foundFreeChan > 2 ? PSG_RIGHT : PSG_LEFT);
			}
			if (dispatchChipAct[3]) dispatchChipAct[3]--;
		}
		return;
	}

	if (whichChip == 3) { // OPL3
		if (isOn) {
			if (isBeat) foundFreeChan = channel;
			else foundFreeChan = dispatchFindFreeChannel(polyOPL3Buffer, 9, dispatchReservedOPL3);
			if (foundFreeChan != -1) {
				opl3Note(foundFreeChan, opl3Fnums[(note + 5) % 12], (note + 5) / 12 - 2, true);
				polyOPL3Buffer[foundFreeChan] = note;
			}
			dispatchChipAct[4]++;
		} else {
			foundFreeChan = dispatchLiberateChannel(note, polyOPL3Buffer, 9);
			if (foundFreeChan >= 0) {
				opl3Note(foundFreeChan, opl3Fnums[(note + 5) % 12], (note + 5) / 12 - 2, false);
				polyOPL3Buffer[foundFreeChan] = 0;
			}
			if (dispatchChipAct[4]) dispatchChipAct[4]--;
		}
	}
}


#endif
