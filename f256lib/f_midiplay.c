/*
 *	MIDI file playback engine for F256.
 *	Supports both v1 (pre-parsed) and v2 (real-time streaming) playback.
 *	Adapted from mu0nlibs/muMidiPlay and muMidiPlay2 for oscar64.
 */


#ifndef WITHOUT_MIDIPLAY


#include "f256lib.h"


// ============================================================
// Globals
// ============================================================

midiplayParserT  midiplayTheOne;
bool             midiplayChip = false;


// ============================================================
// Utility: Big-endian reads from far memory
// ============================================================

uint16_t midiplayReadBE16(uint32_t where) {
	uint8_t b0 = FAR_PEEK(where);
	uint8_t b1 = FAR_PEEK(where + 1);
	return ((uint16_t)b0 << 8) | (uint16_t)b1;
}

uint32_t midiplayReadBE32(uint32_t where) {
	uint8_t b0 = FAR_PEEK(where);
	uint8_t b1 = FAR_PEEK(where + 1);
	uint8_t b2 = FAR_PEEK(where + 2);
	uint8_t b3 = FAR_PEEK(where + 3);
	return (((uint32_t)b0) << 24) |
	       (((uint32_t)b1) << 16) |
	       (((uint32_t)b2) << 8)  |
	       (uint32_t)b3;
}


// ============================================================
// v1 functions: Record and list initialization
// ============================================================

void midiplayInitRecord(midiplayRecordT *rec, uint32_t baseAddr, uint32_t parsedAddr) {
	rec->totalDuration = 0;
	rec->fileName = malloc(sizeof(char) * 64);
	rec->format = 0;
	rec->trackcount = 0;
	rec->tick = 48;
	rec->fileSize = 0;
	rec->fudge = 25.1658;
	rec->nn = 4;
	rec->dd = 2;
	rec->cc = 24;
	rec->bb = 8;
	rec->currentSec = 0;
	rec->totalSec = 0;
	rec->baseAddr = baseAddr;
	rec->parsedAddr = parsedAddr;
	rec->bpm = 0;
	rec->nbTempoChanges = 0;
	rec->parsers = NULL;
}

void midiplayInitList(midiplayParsedListT *list) {
	list->hasBeenUsed = false;
	list->trackcount = 0;
	list->TrackEventList = (midiplayTableOfEventsT *)NULL;
}

uint32_t midiplayGetTotalLeft(midiplayParsedListT *list) {
	uint32_t sum = 0;
	uint16_t i;
	for (i = 0; i < list->trackcount; i++) {
		sum += list->TrackEventList[i].eventcount;
	}
	return sum;
}


// ============================================================
// Common: Load a standard MIDI file into far memory
// ============================================================

uint8_t midiplayLoadFile(const char *name, uint32_t targetAddress) {
	FILE *theMIDIfile;
	uint8_t buffer[255];
	size_t bytesRead = 0;
	uint32_t totalBytesRead = 0;
	uint16_t i = 0;

	theMIDIfile = fileOpen(name, "r");
	if (theMIDIfile == NULL) {
		return 1;
	}

	while ((bytesRead = fileRead(buffer, sizeof(uint8_t), 250, theMIDIfile)) > 0) {
		buffer[0] = buffer[0];
		for (i = 0; i < bytesRead; i++) {
			FAR_POKE((uint32_t)targetAddress + (uint32_t)totalBytesRead + (uint32_t)i, buffer[i]);
		}
		totalBytesRead += (uint32_t)bytesRead;
		if (bytesRead < 250) break;
	}
	fileClose(theMIDIfile);
	return 0;
}


// ============================================================
// v1: Find position of 'MThd' header
// ============================================================

int16_t midiplayFindHeader(uint32_t baseAddr) {
	char targetSequence[] = "MThd";
	char *position;
	int thePosition = 0;
	char buffer[64];
	int i = 0;

	for (i = 0; i < 64; i++) buffer[i] = FAR_PEEK(baseAddr + i);

	position = strstr(buffer, targetSequence);

	if (position != NULL) {
		thePosition = (int)(position - buffer);
		return thePosition;
	}
	return -1;
}


// ============================================================
// v1: Detect structure (3-param version for pre-parsed playback)
// ============================================================

void midiplayDetectStructureV1(uint16_t startIndex, midiplayRecordT *rec, midiplayParsedListT *list) {
	uint32_t trackLength = 0;
	uint32_t i = startIndex;
	uint32_t j = 0;
	uint16_t currentTrack = 0;

	i += 4; // skip header tag
	i += 4; // skip SIZE (always 6)

	rec->format =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)(FAR_PEEK(rec->baseAddr + i) << 8);
	i += 2;

	rec->trackcount =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)((FAR_PEEK(rec->baseAddr + i) << 8));
	i += 2;

	rec->tick =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)((FAR_PEEK(rec->baseAddr + i) << 8));
	i += 2;

	currentTrack = 0;

	list->hasBeenUsed = true;
	list->trackcount = rec->trackcount;
	list->TrackEventList = (midiplayTableOfEventsT *)malloc((sizeof(midiplayTableOfEventsT)) * list->trackcount);

	rec->parsers = (uint16_t *)malloc(sizeof(uint16_t) * rec->trackcount);

	while (currentTrack < rec->trackcount) {
		rec->parsers[currentTrack] = 0;
		currentTrack++;
		i += 4; // skip MTrk string

		trackLength = (((uint32_t)(FAR_PEEK(rec->baseAddr + i))) << 24)
		            | (((uint32_t)(FAR_PEEK(rec->baseAddr + i + 1))) << 16)
		            | (((uint32_t)(FAR_PEEK(rec->baseAddr + i + 2))) << 8)
		            | ((uint32_t)(FAR_PEEK(rec->baseAddr + i + 3)));
		i += 4;
		i += trackLength;
	}

	for (j = 0; j < list->trackcount; j++) {
		list->TrackEventList[j].trackno = j;
		list->TrackEventList[j].eventcount = 0;
		list->TrackEventList[j].baseOffset = 0;
	}
}


// ============================================================
// v1: Get and analyze MIDI
// ============================================================

int16_t midiplayGetAndAnalyze(midiplayRecordT *rec, midiplayParsedListT *list) {
	int16_t indexToStart = 0;
	indexToStart = midiplayFindHeader(rec->baseAddr);
	if (indexToStart == -1) {
		return -1;
	}
	midiplayDetectStructureV1(indexToStart, rec, list);
	return indexToStart;
}


// ============================================================
// v1: Adjust offsets for multi-track parsed data
// ============================================================

void midiplayAdjustOffsets(midiplayParsedListT *list) {
	uint16_t i = 0, k = 0;
	uint32_t currentEventCount = 0;

	for (i = 0; i < list->trackcount; i++) {
		list->TrackEventList[i].baseOffset = (uint32_t)0;
		for (k = 0; k < i; k++) {
			currentEventCount = (uint32_t)list->TrackEventList[k].eventcount;
			list->TrackEventList[i].baseOffset += (currentEventCount * (uint32_t)MIDI_EVENT_FAR_SIZE);
		}
	}
}


// ============================================================
// v1: Parse MIDI file into events stored in far memory
// ============================================================

int8_t midiplayParse(uint16_t startIndex, bool wantCmds, midiplayRecordT *rec, midiplayParsedListT *list) {
	uint32_t trackLength = 0;
	uint32_t i = startIndex;
	uint16_t currentTrack = 0;
	uint32_t tempCalc = 0;
	uint8_t last_cmd = 0x00;
	uint32_t currentI;
	uint32_t timer0PerTick = 0;
	uint32_t usPerTick = 0;
	uint16_t interestingIndex = 0;
	uint32_t nValue, nValue2, nValue3, nValue4, timeDelta;
	uint8_t status_byte = 0x00, extra_byte = 0x00, extra_byte2 = 0x00;
	uint8_t meta_byte = 0x00;
	uint32_t data_byte = 0x00, data_byte2 = 0x00, data_byte3 = 0x00, data_byte4 = 0x00;
	uint32_t usPerBeat = 500000;
	bool lastCmdPreserver = false;
	uint32_t superTotal = 0;
	uint32_t whereTo = 0;

	i += 4; // skip MThd
	i += 4; // skip size

	rec->format =
		(((uint16_t)FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1))
		| ((uint16_t)FAR_PEEK(rec->baseAddr + (uint32_t)i)) << 8);
	i += 2;

	rec->trackcount =
		((uint16_t)(FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1)))
		| (((uint16_t)FAR_PEEK(rec->baseAddr + (uint32_t)i)) << 8);
	i += 2;

	rec->tick = (uint16_t)(
		((uint16_t)(FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1)))
		| (((uint16_t)FAR_PEEK(rec->baseAddr + (uint32_t)i)) << 8));
	i += 2;

	currentTrack = 0;

	while (currentTrack < rec->trackcount) {
		i += 4; // skip MTrk
		trackLength = ((((uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i))) << 24)
		            | ((((uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1))) << 16)
		            | ((((uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)2))) << 8)
		            | ((((uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)3))));
		i += 4;

		last_cmd = 0x00;
		currentI = i;
		interestingIndex = 0;

		while (i < (trackLength + currentI)) {
			nValue = 0x00000000;
			nValue2 = 0x00000000;
			nValue3 = 0x00000000;
			nValue4 = 0x00000000;
			data_byte = 0x00000000;
			status_byte = 0x00;

			nValue = (uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i);
			i++;
			if (nValue & 0x00000080) {
				nValue &= 0x0000007F;
				nValue <<= 7;
				nValue2 = (uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i);
				i++;
				if (nValue2 & 0x00000080) {
					nValue2 &= 0x0000007F;
					nValue2 <<= 7;
					nValue <<= 7;
					nValue3 = (uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i);
					i++;
					if (nValue3 & 0x00000080) {
						nValue3 &= 0x0000007F;
						nValue3 <<= 7;
						nValue2 <<= 7;
						nValue <<= 7;
						nValue4 = (uint32_t)FAR_PEEK(rec->baseAddr + (uint32_t)i);
						i++;
					}
				}
			}
			timeDelta = nValue | nValue2 | nValue3 | nValue4;

			status_byte = FAR_PEEK(rec->baseAddr + (uint32_t)i);
			extra_byte = FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1);
			extra_byte2 = FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)2);
			i++;

			lastCmdPreserver = false;
			if (status_byte < 0x80) {
				i--;
				status_byte = last_cmd;
				extra_byte = FAR_PEEK(rec->baseAddr + (uint32_t)i);
				extra_byte2 = FAR_PEEK(rec->baseAddr + (uint32_t)i + (uint32_t)1);
			}

			if (status_byte == 0xFF) {
				meta_byte = FAR_PEEK(rec->baseAddr + (uint32_t)i);
				i++;
				if (meta_byte == MIDI_META_SEQUENCE || meta_byte == MIDI_META_CHANNEL_PREFIX || meta_byte == MIDI_META_CHANGE_PORT) {
					i += (uint32_t)2;
				} else if (meta_byte == MIDI_META_TEXT || meta_byte == MIDI_META_COPYRIGHT || meta_byte == MIDI_META_TRACK_NAME || meta_byte == MIDI_META_INSTRUMENT_NAME
				           || meta_byte == MIDI_META_LYRICS || meta_byte == MIDI_META_MARKER || meta_byte == MIDI_META_CUE_POINT) {
					data_byte = (uint8_t)FAR_PEEK(rec->baseAddr + i);
					i += (uint32_t)data_byte + (uint32_t)1;
				} else if (meta_byte == MIDI_META_END_OF_TRACK) {
					i++;
					continue;
				} else if (meta_byte == MIDI_META_SET_TEMPO) {
					data_byte = FAR_PEEK(rec->baseAddr + (uint32_t)i);
					i++;
					data_byte2 = FAR_PEEK(rec->baseAddr + (uint32_t)i);
					i++;
					data_byte3 = FAR_PEEK(rec->baseAddr + (uint32_t)i);
					i++;
					data_byte4 = FAR_PEEK(rec->baseAddr + (uint32_t)i);
					i++;

					usPerBeat = (((uint32_t)data_byte2) << 16)
					          | (((uint32_t)data_byte3) << 8)
					          | ((uint32_t)data_byte4);

					usPerTick = (uint32_t)usPerBeat / ((uint32_t)rec->tick);
					timer0PerTick = (uint32_t)((float)usPerTick * (float)rec->fudge);
					rec->bpm = (uint16_t)((uint32_t)60000000UL / ((uint32_t)usPerBeat));
				} else if (meta_byte == MIDI_META_SMPTE_OFFSET) {
					i += 6;
				} else if (meta_byte == MIDI_META_TIME_SIGNATURE) {
					i++;
					rec->nn = (uint8_t)FAR_PEEK(rec->baseAddr + i); i++;
					rec->dd = (uint8_t)FAR_PEEK(rec->baseAddr + i); i++;
					rec->cc = (uint8_t)FAR_PEEK(rec->baseAddr + i); i++;
					rec->bb = (uint8_t)FAR_PEEK(rec->baseAddr + i); i++;
				} else if (meta_byte == MIDI_META_KEY_SIGNATURE) {
					i += 3;
				} else if (meta_byte == MIDI_META_SEQUENCER_SPECIFIC) {
					continue;
				}
			}
			// Program change 0xC_ or Channel Pressure 0xD_
			else if (status_byte >= 0xC0 && status_byte <= 0xDF) {
				if (wantCmds == false) {
					list->TrackEventList[currentTrack].eventcount++;
				}
				if (wantCmds) {
					whereTo = (uint32_t)(list->TrackEventList[currentTrack].baseOffset);
					whereTo += (uint32_t)((uint32_t)interestingIndex * (uint32_t)MIDI_EVENT_FAR_SIZE);

					tempCalc = timer0PerTick * timeDelta;
					superTotal += (uint32_t)(tempCalc) >> 3;

					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo,                (uint8_t)((tempCalc & 0x000000FF)));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)1,  (uint8_t)((tempCalc & 0x0000FF00) >> 8));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)2,  (uint8_t)((tempCalc & 0x00FF0000) >> 16));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)3,  (uint8_t)((tempCalc & 0xFF000000) >> 24));

					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_BYTECOUNT, 0x02);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG, status_byte);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + (uint32_t)1, extra_byte);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + (uint32_t)2, 0x00);

					interestingIndex++;
				}
				i++;
			}
			// Note off/on, Aftertouch, Control Change, Pitch Bend
			else if ((status_byte >= 0x80 && status_byte <= 0xBF) || (status_byte >= 0xE0 && status_byte <= 0xEF)) {
				if (wantCmds == false) {
					list->TrackEventList[currentTrack].eventcount++;
				}
				if (wantCmds) {
					if ((status_byte & 0xF0) == 0x90 && extra_byte2 == 0x00) {
						status_byte = status_byte & 0x8F;
						extra_byte2 = 0x7F;
						lastCmdPreserver = true;
					}
					whereTo = (uint32_t)(list->TrackEventList[currentTrack].baseOffset);
					whereTo += (uint32_t)((uint32_t)interestingIndex * (uint32_t)MIDI_EVENT_FAR_SIZE);

					tempCalc = timer0PerTick * timeDelta;
					superTotal += (uint32_t)(tempCalc) >> 3;

					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo,                (uint8_t)((tempCalc & 0x000000FF)));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)1,  (uint8_t)((tempCalc & 0x0000FF00) >> 8));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)2,  (uint8_t)((tempCalc & 0x00FF0000) >> 16));
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)3,  (uint8_t)((tempCalc & 0xFF000000) >> 24));

					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_BYTECOUNT, 0x03);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG, status_byte);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + (uint32_t)1, extra_byte);
					FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + (uint32_t)2, extra_byte2);

					interestingIndex++;
				}
				i += 2;
			} else {
				// Unrecognized event
			}

			last_cmd = status_byte;
			if (lastCmdPreserver) last_cmd = (status_byte & 0x0F) | 0x90;
		} // end of parsing a track
		currentTrack++;
	} // end of parsing all tracks

	rec->totalDuration = superTotal;
	return 0;
}


// ============================================================
// v1: Write digested MIDI file
// ============================================================

uint8_t midiplayWriteDigest(char *name, midiplayRecordT *rec, midiplayParsedListT *theBigList) {
	FILE *fileID = 0;
	uint16_t i, imax;
	uint16_t j;
	uint16_t k, kmax;
	uint32_t tracker = 0x00000000;
	uint8_t delta1, delta2;
	uint8_t cmdByte = 0;

	fileID = fileOpen(name, "w");

	tracker = rec->parsedAddr;
	imax = theBigList->trackcount;

	fileWrite(&imax, sizeof(uint16_t), 1, fileID);
	kernelNextEvent();

	for (i = 0; i < imax; i++) {
		fileWrite(&(theBigList->TrackEventList[i].eventcount), sizeof(uint16_t), 1, fileID);
		kernelNextEvent();
	}

	for (i = 0; i < imax; i++) {
		for (j = 0; j < theBigList->TrackEventList[i].eventcount; j++) {
			delta1 = FAR_PEEK(tracker); tracker += (uint32_t)1;
			fileWrite(&delta1, sizeof(uint8_t), 1, fileID);
			delta1 = FAR_PEEK(tracker); tracker += (uint32_t)1;
			fileWrite(&delta1, sizeof(uint8_t), 1, fileID);

			delta2 = FAR_PEEK(tracker); tracker += (uint32_t)1;
			fileWrite(&delta2, sizeof(uint8_t), 1, fileID);
			delta2 = FAR_PEEK(tracker); tracker += (uint32_t)1;
			fileWrite(&delta2, sizeof(uint8_t), 1, fileID);

			kmax = FAR_PEEK(tracker);
			fileWrite(&kmax, sizeof(uint8_t), 1, fileID);
			tracker += (uint32_t)1;

			for (k = 0; k < kmax; k++) {
				cmdByte = FAR_PEEK(tracker);
				fileWrite(&cmdByte, sizeof(uint8_t), 1, fileID);
				tracker += (uint32_t)1;
			}
			if (kmax == 2) {
				cmdByte = 0;
				fileWrite(&cmdByte, sizeof(uint8_t), 1, fileID);
				tracker += (uint32_t)1;
			}
		}
	}

	fileClose(fileID);
	textGotoXY(20, 3); printf("%04ld bytes written to %s\n", tracker - rec->parsedAddr, name);

	return 0;
}


// ============================================================
// v1: Read digested MIDI file
// ============================================================

uint8_t midiplayReadDigest(char *name, midiplayRecordT *rec, midiplayParsedListT *theBigList) {
	FILE *fileID = 0;
	uint8_t buffer[255];
	size_t bytesRead = 0;
	uint32_t totalBytesRead = 0;
	uint16_t i;
	uint32_t readChunk = 0;
	uint32_t leftToRead = 0;
	uint8_t read1, read2;
	uint16_t index = 0;

	fileID = fileOpen(name, "r");
	if (fileID == NULL) {
		return 1;
	}

	fileRead(&read1, sizeof(uint8_t), 1, fileID);
	fileRead(&read2, sizeof(uint8_t), 1, fileID);
	theBigList->trackcount = (uint16_t)(read1) | ((((uint16_t)read2) << 8) & 0xFF00);
	rec->trackcount = theBigList->trackcount;

	if (theBigList->TrackEventList != NULL) free(theBigList->TrackEventList);
	theBigList->TrackEventList = (midiplayTableOfEventsT *)malloc((sizeof(midiplayTableOfEventsT)) * theBigList->trackcount);
	if (rec->parsers != NULL) free(rec->parsers);
	rec->parsers = (uint16_t *)malloc(sizeof(uint16_t) * theBigList->trackcount);

	for (index = 0; index < theBigList->trackcount; index++) {
		if (index == theBigList->trackcount) break;
		rec->parsers[index] = 0;

		fileRead(&read1, sizeof(uint8_t), 1, fileID);
		fileRead(&read2, sizeof(uint8_t), 1, fileID);
		theBigList->TrackEventList[index].eventcount = (uint16_t)(read1) | ((((uint16_t)read2) << 8) & 0xFF00);
		theBigList->TrackEventList[index].trackno = index;
		theBigList->TrackEventList[index].baseOffset = 0;
	}
	midiplayAdjustOffsets(theBigList);

	for (uint16_t a = 0; a < theBigList->trackcount; a++) {
		leftToRead = (uint32_t)theBigList->TrackEventList[a].eventcount * (uint32_t)MIDI_EVENT_FAR_SIZE;
		if (leftToRead == 0) continue;
		readChunk = 250;
		if (readChunk > leftToRead) readChunk = leftToRead;
		while (leftToRead > 0) {
			bytesRead = fileRead(buffer, sizeof(uint8_t), readChunk, fileID);

			for (i = 0; i < bytesRead; i++) {
				FAR_POKE((uint32_t)rec->parsedAddr + (uint32_t)totalBytesRead + (uint32_t)i, buffer[i]);
			}
			totalBytesRead += (uint32_t)bytesRead;
			leftToRead -= readChunk;

			if (readChunk > leftToRead) readChunk = leftToRead;
		}
	}
	fileClose(fileID);
	return 0;
}


// ============================================================
// v1: Send a pre-parsed MIDI event (aMEPtr style)
// ============================================================

void midiplaySendEventV1(midiplayEventT *midiEvent, bool useAlt) {
	POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, midiEvent->msgToSend[0]);
	POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, midiEvent->msgToSend[1]);
	if (midiEvent->bytecount == 3) {
		POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, midiEvent->msgToSend[2]);
	}
}


// ============================================================
// v1: Play pre-parsed multi-track MIDI
// ============================================================

uint8_t midiplayPlayV1(midiplayRecordT *rec, midiplayParsedListT *list) {
	uint16_t i;
	uint16_t lowestTrack = 0;
	uint16_t localTotalLeft = 0;
	uint32_t lowestTimeFound = 0xFFFFFFFF;
	uint32_t whereTo, whereToLowest;
	uint16_t trackcount;
	uint32_t delta;
	uint32_t overFlow;
	bool exitFlag = false;
	uint32_t *soundBeholders;

	midiplayEventT msgGo;
	trackcount = list->trackcount;

	soundBeholders = (uint32_t *)malloc(trackcount * sizeof(uint32_t));

	localTotalLeft = midiplayGetTotalLeft(list);

	for (i = 0; i < trackcount; i++) {
		if (list->TrackEventList[i].eventcount == 0) {
			soundBeholders[i] = 0;
			continue;
		}
		whereTo = (uint32_t)(list->TrackEventList[i].baseOffset);
		soundBeholders[i] = (
			(((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo))) & 0x000000FF)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)1)) << 8) & 0x0000FF00)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)2)) << 16) & 0x00FF0000)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)3)) << 24) & 0xFF000000)
		);
	}

	while (localTotalLeft > 0 && !exitFlag) {
		lowestTimeFound = 0xFFFFFFFF;

		for (i = 0; i < trackcount; i++) {
			if (rec->parsers[i] >= (list->TrackEventList[i].eventcount)) continue;

			delta = soundBeholders[i];

			if (delta == 0) {
				lowestTimeFound = 0;
				lowestTrack = i;
				whereToLowest = (uint32_t)(list->TrackEventList[i].baseOffset);
				whereToLowest += (uint32_t)((uint32_t)rec->parsers[i] * (uint32_t)MIDI_EVENT_FAR_SIZE);
				break;
			}
			if (delta < lowestTimeFound) {
				lowestTimeFound = delta;
				lowestTrack = i;
				whereToLowest = (uint32_t)(list->TrackEventList[i].baseOffset);
				whereToLowest += (uint32_t)((uint32_t)rec->parsers[i] * (uint32_t)MIDI_EVENT_FAR_SIZE);
			}
		}

		msgGo.deltaToGo = lowestTimeFound;
		msgGo.bytecount = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)MIDI_EVENT_BYTECOUNT);
		msgGo.msgToSend[0] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)MIDI_EVENT_MSG);
		msgGo.msgToSend[1] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)MIDI_EVENT_MSG + (uint32_t)1);
		msgGo.msgToSend[2] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)MIDI_EVENT_MSG + (uint32_t)2);

		if (lowestTimeFound == 0) {
			midiplaySendEventV1(&msgGo, 0);
		} else {
			overFlow = lowestTimeFound;
			while (overFlow > 0x00FFFFFF) {
				timer0Set(0x00FFFFFF);
				while (!(PEEK(INT_PENDING_0) & INT_TIMER_0))
					;
				POKE(INT_PENDING_0, INT_TIMER_0);
				overFlow = overFlow - 0x00FFFFFF;
			}
			timer0Set(overFlow);
			while (!(PEEK(INT_PENDING_0) & INT_TIMER_0))
				;
			POKE(INT_PENDING_0, INT_TIMER_0);
			midiplaySendEventV1(&msgGo, 0);
		}

		rec->parsers[lowestTrack] += 1;
		whereToLowest = (uint32_t)(list->TrackEventList[lowestTrack].baseOffset);
		whereToLowest += (uint32_t)((uint32_t)rec->parsers[lowestTrack] * (uint32_t)MIDI_EVENT_FAR_SIZE);

		soundBeholders[lowestTrack] = (
			(((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest))) & 0x000000FF)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)1)) << 8) & 0x0000FF00)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)2)) << 16) & 0x00FF0000)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereToLowest + (uint32_t)3)) << 24) & 0xFF000000)
		);

		for (i = 0; i < trackcount; i++) {
			if (rec->parsers[i] >= (list->TrackEventList[i].eventcount)) continue;
			if (i == lowestTrack) continue;
			soundBeholders[i] -= lowestTimeFound;
		}
		localTotalLeft--;
	}
	free(soundBeholders);
	return 0;
}


// ============================================================
// v1: Play pre-parsed single-track (type 0) MIDI
// ============================================================

uint8_t midiplayPlayType0(midiplayRecordT *rec, midiplayParsedListT *list) {
	uint16_t localTotalLeft = 0;
	uint32_t whereTo;
	uint32_t overFlow;
	midiplayEventT msgGo;
	bool exitFlag = false;

	localTotalLeft = midiplayGetTotalLeft(list);

	while (localTotalLeft > 0 && !exitFlag) {
		whereTo = (uint32_t)(list->TrackEventList[0].baseOffset);
		whereTo += (uint32_t)((uint32_t)rec->parsers[0] * (uint32_t)MIDI_EVENT_FAR_SIZE);

		msgGo.deltaToGo = (
			(((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo))) & 0x000000FF)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)1)) << 8) & 0x0000FF00)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)2)) << 16) & 0x00FF0000)
			| (((uint32_t)(FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)3)) << 24) & 0xFF000000)
		);
		msgGo.bytecount = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_BYTECOUNT);
		msgGo.msgToSend[0] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG);
		msgGo.msgToSend[1] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + 1);
		msgGo.msgToSend[2] = FAR_PEEK((uint32_t)rec->parsedAddr + (uint32_t)whereTo + (uint32_t)MIDI_EVENT_MSG + 2);

		if (msgGo.deltaToGo > 0) {
			overFlow = msgGo.deltaToGo;
			while (overFlow > 0x00FFFFFF) {
				timer0Set(0x00FFFFFF);
				while (!(PEEK(INT_PENDING_0) & INT_TIMER_0))
					;
				POKE(INT_PENDING_0, INT_TIMER_0);
				overFlow = overFlow - 0x00FFFFFF;
			}
			timer0Set(overFlow);
			while (!(PEEK(INT_PENDING_0) & INT_TIMER_0))
				;
			POKE(INT_PENDING_0, INT_TIMER_0);
		}
		midiplaySendEventV1(&msgGo, 0);
		rec->parsers[0]++;
		localTotalLeft--;
	}
	return 0;
}


// ============================================================
// v2: Detect structure (2-param version for real-time playback)
// ============================================================

void midiplayDetectStructure(uint16_t startIndex, midiplayRecordT *rec) {
	uint32_t trackLength = 0;
	uint32_t i = startIndex;
	uint16_t currentTrack = 0;

	i += 4; // skip header tag
	i += 4; // skip SIZE (always 6)

	rec->format =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)(FAR_PEEK(rec->baseAddr + i) << 8);
	i += 2;

	rec->trackcount =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)((FAR_PEEK(rec->baseAddr + i) << 8));
	i += 2;

	rec->tick =
		(uint16_t)(FAR_PEEK(rec->baseAddr + i + 1))
		| (uint16_t)((FAR_PEEK(rec->baseAddr + i) << 8));
	i += 2;

	currentTrack = 0;

	while (currentTrack < rec->trackcount) {
		rec->parsers[currentTrack] = 0;
		currentTrack++;
		i += 4; // skip MTrk string

		trackLength = (((uint32_t)(FAR_PEEK(rec->baseAddr + i))) << 24)
		            | (((uint32_t)(FAR_PEEK(rec->baseAddr + i + 1))) << 16)
		            | (((uint32_t)(FAR_PEEK(rec->baseAddr + i + 2))) << 8)
		            | ((uint32_t)(FAR_PEEK(rec->baseAddr + i + 3)));
		i += 4;
		i += trackLength;
	}
}


// ============================================================
// v2: Default send event (sends directly to MIDI FIFO)
// ============================================================

void midiplaySendEvent(uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t byteCount, bool useAlt) {
	// Send instrument changes to both MIDI devices
	if ((msg0 & 0xF0) == 0xC0) {
		POKE(useAlt ? MIDI_FIFO : MIDI_FIFO_ALT, msg0);
		POKE(useAlt ? MIDI_FIFO : MIDI_FIFO_ALT, msg1);
	}

	POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, msg0);
	POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, msg1);
	if (byteCount == 3) {
		POKE(useAlt ? MIDI_FIFO_ALT : MIDI_FIFO, msg2);
	}
}


// ============================================================
// v2: Read MIDI event from track
// ============================================================

uint8_t midiplayReadEvent(uint8_t track) {
	if (midiplayTheOne.tracks[track].offset >= midiplayTheOne.tracks[track].length) {
		return 2;
	}
	midiplayTheOne.tracks[track].delta = midiplayReadDelta(track);
	return midiplayReadCmd(track);
}


// ============================================================
// v2: Read variable-length delta time
// ============================================================

uint32_t midiplayReadDelta(uint8_t track) {
	uint32_t nValue, nValue2, nValue3, nValue4;
	uint8_t temp;

	nValue = 0x00000000;
	nValue2 = 0x00000000;
	nValue3 = 0x00000000;
	nValue4 = 0x00000000;

	temp = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset++);
	nValue = (uint32_t)temp;

	if (nValue & 0x00000080) {
		nValue &= 0x0000007F;
		nValue <<= 7;

		temp = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset++);
		nValue2 = (uint32_t)temp;

		if (nValue2 & 0x00000080) {
			nValue2 &= 0x0000007F;
			nValue2 <<= 7;
			nValue <<= 7;

			temp = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset++);
			nValue3 = (uint32_t)temp;

			if (nValue3 & 0x00000080) {
				nValue3 &= 0x0000007F;
				nValue3 <<= 7;
				nValue2 <<= 7;
				nValue <<= 7;

				temp = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset++);
				nValue4 = (uint32_t)temp;
			}
		}
	}
	return nValue | nValue2 | nValue3 | nValue4;
}


// ============================================================
// v2: Read MIDI command
// ============================================================

uint8_t midiplayReadCmd(uint8_t track) {
	uint8_t status_byte, extra_byte, extra_byte2;
	uint8_t extra_byte3, extra_byte4, extra_byte5;

	status_byte = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset);
	extra_byte = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset + (uint32_t)1);
	extra_byte2 = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset + (uint32_t)2);
	extra_byte3 = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset + (uint32_t)3);
	extra_byte4 = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset + (uint32_t)4);
	extra_byte5 = FAR_PEEK(midiplayTheOne.tracks[track].start + midiplayTheOne.tracks[track].offset + (uint32_t)5);
	midiplayTheOne.tracks[track].cmd[0] = status_byte;
	midiplayTheOne.tracks[track].cmd[1] = extra_byte;
	midiplayTheOne.tracks[track].cmd[2] = extra_byte2;

	midiplayTheOne.tracks[track].offset++;

	// Check for run-on commands
	if (status_byte < 0x80) {
		extra_byte2 = extra_byte;
		extra_byte = status_byte;
		status_byte = midiplayTheOne.tracks[track].lastCmd;

		midiplayTheOne.tracks[track].cmd[0] = status_byte;
		midiplayTheOne.tracks[track].cmd[1] = extra_byte;
		midiplayTheOne.tracks[track].cmd[2] = extra_byte2;

		midiplayTheOne.tracks[track].offset--;
	}

	// Meta events (0xFF)
	if (status_byte == 0xFF) {
		midiplayTheOne.tracks[track].cmd[0] = status_byte;
		midiplayTheOne.tracks[track].cmd[1] = extra_byte;
		midiplayTheOne.tracks[track].cmd[2] = extra_byte2;
		if (extra_byte == 0x51) { // tempo change
			midiplayTheOne.tracks[track].cmd[3] = extra_byte3;
			midiplayTheOne.tracks[track].cmd[4] = extra_byte4;
			midiplayTheOne.tracks[track].cmd[5] = extra_byte5;
		}
		midiplaySkipFFCmd(track, extra_byte, extra_byte2);
		return 0;
	}

	// Program change 0xC_ or Channel Pressure 0xD_
	else if (status_byte >= 0xC0 && status_byte <= 0xDF) {
		midiplayTheOne.tracks[track].offset++;
		midiplayTheOne.tracks[track].is2B = true;
		midiplayTheOne.tracks[track].lastCmd = status_byte;
		return 0;
	}

	// Note off/on, Aftertouch, Control Change, Pitch Bend
	else if ((status_byte >= 0x80 && status_byte <= 0xBF) || (status_byte >= 0xE0 && status_byte <= 0xEF)) {
		midiplayTheOne.tracks[track].lastCmd = status_byte;

		if ((status_byte & 0xF0) == 0x90 && extra_byte2 == 0x00) {
			status_byte = status_byte & 0x8F;
			extra_byte2 = 0x7F;
		}

		midiplayTheOne.tracks[track].offset += 2;
		midiplayTheOne.tracks[track].is2B = false;
		return 0;
	} else {
		// Unrecognized event
	}
	return 0;
}


// ============================================================
// v2: Skip meta-event data
// ============================================================

uint8_t midiplaySkipFFCmd(uint8_t track, uint8_t meta_byte, uint8_t data_byte) {
	if (meta_byte == MIDI_META_SEQUENCE || meta_byte == MIDI_META_CHANNEL_PREFIX || meta_byte == MIDI_META_CHANGE_PORT) {
		midiplayTheOne.tracks[track].offset += (uint32_t)3;
	} else if (meta_byte == MIDI_META_TEXT || meta_byte == MIDI_META_COPYRIGHT || meta_byte == MIDI_META_TRACK_NAME || meta_byte == MIDI_META_INSTRUMENT_NAME
	           || meta_byte == MIDI_META_LYRICS || meta_byte == MIDI_META_MARKER || meta_byte == MIDI_META_CUE_POINT) {
		midiplayTheOne.tracks[track].offset += (uint32_t)2 + (uint32_t)data_byte;
	} else if (meta_byte == MIDI_META_END_OF_TRACK) {
		midiplayTheOne.tracks[track].offset += (uint32_t)2;
	} else if (meta_byte == MIDI_META_SET_TEMPO) {
		midiplayTheOne.tracks[track].offset += (uint32_t)5;
	} else if (meta_byte == MIDI_META_SMPTE_OFFSET) {
		midiplayTheOne.tracks[track].offset += (uint32_t)7;
	} else if (meta_byte == MIDI_META_TIME_SIGNATURE) {
		midiplayTheOne.tracks[track].offset += (uint32_t)6;
	} else if (meta_byte == MIDI_META_KEY_SIGNATURE) {
		midiplayTheOne.tracks[track].offset += (uint32_t)4;
	} else if (meta_byte == MIDI_META_SEQUENCER_SPECIFIC) {
		// do nothing
	}
	return 0;
}


// ============================================================
// v2: Chain to next meaningful event
// ============================================================

void midiplayChainEvent(uint8_t track) {
	bool quitRefresh = false;
	for (;;) {
		switch (midiplayReadEvent(track)) {
		case 1: // skippable 0xFF event, continue
			continue;
		case 0: // regular event found
			quitRefresh = true;
			break;
		case 2: // end of track
			midiplayTheOne.tracks[track].isDone = true;
			midiplayTheOne.isMasterDone++;
			quitRefresh = true;
			break;
		}
		if (quitRefresh) break;
	}
}


// ============================================================
// v2: Perform a MIDI command (tempo change or sound event)
// ============================================================

void midiplayPerformCmd(uint8_t track) {
	if (midiplayTheOne.tracks[track].cmd[0] == 0xFF) {
		if (midiplayTheOne.tracks[track].cmd[1] == MIDI_META_END_OF_TRACK) {
			midiplayTheOne.tracks[track].isDone = true;
		}
		if (midiplayTheOne.tracks[track].cmd[1] == MIDI_META_SET_TEMPO) {
			uint32_t usPerBeat = (((uint32_t)midiplayTheOne.tracks[track].cmd[3]) << 16)
			                   | (((uint32_t)midiplayTheOne.tracks[track].cmd[4]) << 8)
			                   | ((uint32_t)midiplayTheOne.tracks[track].cmd[5]);

			uint32_t usPerTick = (uint32_t)usPerBeat / (uint32_t)midiplayTheOne.ticks;
			midiplayTheOne.timer0PerTick = (usPerTick << 3) + (usPerTick << 2); // convert to timer0 units
		}
		return;
	}

	midiplaySendEvent(
		midiplayTheOne.tracks[track].cmd[0],
		midiplayTheOne.tracks[track].cmd[1],
		midiplayTheOne.tracks[track].cmd[2],
		midiplayTheOne.tracks[track].is2B ? 2 : 3,
		midiplayChip
	);
}


// ============================================================
// v2: Exhaust all zero-delta events for a track
// ============================================================

void midiplayExhaustZeroes(uint8_t track) {
	if (midiplayTheOne.tracks[track].isDone) return;
	if (midiplayTheOne.tracks[track].delta > 0) return;
	for (;;) {
		midiplayPerformCmd(track);
		midiplayChainEvent(track);
		if (midiplayTheOne.tracks[track].isDone) break;
		if (midiplayTheOne.tracks[track].delta > (uint32_t)0) break;
	}
}


// ============================================================
// v2: Play queued MIDI event (called when timer0 fires)
// ============================================================

void midiplayPlay(void) {
	if (midiplayTheOne.cuedDelta > 0x00FFFFFF) {
		midiplayTheOne.cuedDelta -= 0x00FFFFFF;
		timer0Set(midiplayTheOne.cuedDelta);
		return;
	}
	if (midiplayTheOne.cuedDelta > 0) {
		timer0Set(midiplayTheOne.cuedDelta);
		midiplayTheOne.cuedDelta = 0;
		return;
	}
	midiplayPerformCmd(midiplayTheOne.cuedIndex);

	// Adjust other tracks' deltas
	for (uint16_t i = 0; i < midiplayTheOne.nbTracks; i++) {
		if (midiplayTheOne.tracks[i].isDone) continue;
		if (i == midiplayTheOne.cuedIndex) continue;
		midiplayTheOne.tracks[i].delta -= midiplayTheOne.tracks[midiplayTheOne.cuedIndex].delta;
		if (midiplayTheOne.tracks[i].delta == 0) midiplayExhaustZeroes(i);
	}

	// Renew the one spent
	midiplayChainEvent(midiplayTheOne.cuedIndex);
	midiplayExhaustZeroes(midiplayTheOne.cuedIndex);

	midiplayTheOne.isWaiting = false;
}


// ============================================================
// v2: Reset all track positions (for looping)
// ============================================================

void midiplayResetTrack(uint32_t baseAddr) {
	uint32_t pos;

	midiplayTheOne.cuedDelta = 0xFFFFFFFF;
	midiplayTheOne.cuedIndex = 0;
	midiplayTheOne.isMasterDone = 0;

	for (uint16_t i = 0; i < midiplayTheOne.nbTracks; i++) {
		midiplayTheOne.tracks[i].length = 0;
		midiplayTheOne.tracks[i].offset = 0;
		midiplayTheOne.tracks[i].start = 0;
		midiplayTheOne.tracks[i].delta = 0;
		midiplayTheOne.tracks[i].cmd[0] = midiplayTheOne.tracks[i].cmd[1] = midiplayTheOne.tracks[i].cmd[2] = 0;
		midiplayTheOne.tracks[i].cmd[3] = midiplayTheOne.tracks[i].cmd[4] = midiplayTheOne.tracks[i].cmd[5] = 0;
		midiplayTheOne.tracks[i].lastCmd = 0;
		midiplayTheOne.tracks[i].is2B = true;
		midiplayTheOne.tracks[i].isDone = false;
	}

	pos = 14;
	for (uint16_t i = 0; i < midiplayTheOne.nbTracks; i++) {
		pos += 4; // skip header string
		uint32_t length = midiplayReadBE32(baseAddr + pos);
		midiplayTheOne.tracks[i].length = length;
		pos += 4;
		midiplayTheOne.tracks[i].start = baseAddr + pos;
		pos += length;
	}

	for (uint16_t i = 0; i < midiplayTheOne.nbTracks; i++) {
		if (midiplayTheOne.tracks[i].isDone) continue;
		if (midiplayTheOne.tracks[i].offset >= midiplayTheOne.tracks[i].length) {
			midiplayTheOne.tracks[i].isDone = true;
			continue;
		}
		midiplayChainEvent(i);
	}
}


// ============================================================
// v2: Initialize tracks (allocate and set up)
// ============================================================

void midiplayInitTrack(uint32_t baseAddr) {
	midiplayTheOne.nbTracks = midiplayReadBE16(baseAddr + (uint32_t)10);
	midiplayTheOne.tracks = (midiplayTrackParserT *)malloc(sizeof(midiplayTrackParserT) * midiplayTheOne.nbTracks);
	midiplayTheOne.isWaiting = false;
	midiplayTheOne.timer0PerTick = 500000;
	midiplayTheOne.ticks = 48;
	midiplayTheOne.progTime = 0;

	midiplayTheOne.ticks = midiplayReadBE16(baseAddr + (uint32_t)12);

	midiplayResetTrack(baseAddr);
}


// ============================================================
// v2: Destroy tracks (free memory)
// ============================================================

void midiplayDestroyTrack(void) {
	free(midiplayTheOne.tracks);
	midiplayTheOne.tracks = NULL;
}


// ============================================================
// v2: Find next event across all tracks and cue timer0
// ============================================================

void midiplaySniffNext(void) {
	uint32_t lowest = 0xFFFFFFFF;
	uint16_t lowestIndex = 0xFFFF;

	for (uint16_t i = 0; i < midiplayTheOne.nbTracks; i++) {
		if (midiplayTheOne.tracks[i].isDone) continue;

		if (midiplayTheOne.tracks[i].delta < lowest) {
			lowest = midiplayTheOne.tracks[i].delta;
			lowestIndex = i;
			midiplayTheOne.isWaiting = true;
			midiplayTheOne.cuedDelta = lowest;
			midiplayTheOne.cuedIndex = lowestIndex;
		}
	}

	if (midiplayTheOne.cuedDelta > 0) {
		midiplayTheOne.cuedDelta = midiplayTheOne.cuedDelta * midiplayTheOne.timer0PerTick;
	}
	timer0Set(midiplayTheOne.cuedDelta);
}


#endif
