/*
 *	MIDI file playback engine for F256.
 *	Supports both v1 (pre-parsed) and v2 (real-time streaming) playback.
 *	Adapted from mu0nlibs/muMidiPlay and muMidiPlay2 for oscar64.
 */


#ifndef MIDIPLAY_H
#define MIDIPLAY_H
#ifndef WITHOUT_MIDIPLAY


#include "f256lib.h"
#include <string.h>


// ============================================================
// v1 types: Tempo change tracking
// ============================================================

typedef struct midiplayTempoChange {
	uint32_t *absTick;       // when it occurs
	uint32_t *usPerTick;     // microseconds per tick
	uint32_t *usPerTimer0;   // timer0 units per tick
} midiplayTempoChangeT;


// ============================================================
// v1 types: MIDI file record (info about the loaded file)
// ============================================================

typedef struct midiplayRecord {
	midiplayTempoChangeT myTempos;  // keeps all tempo changes here
	char           *fileName;
	uint16_t        format;         // 0: single track, 1: multitrack
	uint16_t        trackcount;
	uint32_t        tick;           // ticks per beat (quarter note), default 48
	uint32_t        fileSize;       // number of bytes for the midi file
	float           fudge;          // conversion factor for timer0 units, default 25.1658
	uint8_t         nn, dd, cc, bb; // time signature: numerator, denominator, clocks, 32nds
	uint16_t       *parsers;        // indices for the various type 1 tracks during playback
	uint32_t        totalDuration;  // in units to be divided by 125000 and fudge to get seconds
	uint16_t        totalSec;
	uint16_t        currentSec;
	uint8_t         nbTempoChanges; // count of tempo changes to perform during playback
	uint32_t        baseAddr;       // where the raw MIDI file is loaded in far memory
	uint32_t        parsedAddr;     // where parsed events are stored in far memory
	uint16_t        bpm;            // beats per minute (computed from tempo meta events)
} midiplayRecordT;


// ============================================================
// v1 types: Parsed MIDI events for pre-parsed playback
// ============================================================

typedef struct midiplayEvent {
	uint32_t deltaToGo;
	uint8_t  bytecount;
	uint8_t  msgToSend[3];
} midiplayEventT;

typedef struct midiplayTableOfEvents {
	uint8_t  trackno;
	uint16_t eventcount;      // total events for playback
	uint32_t baseOffset;      // offset in far memory for this track's events
} midiplayTableOfEventsT;

typedef struct midiplayParsedList {
	bool                      hasBeenUsed;
	uint16_t                  trackcount;
	midiplayTableOfEventsT   *TrackEventList;
} midiplayParsedListT;


// ============================================================
// v2 types: Real-time streaming parser
// ============================================================

typedef struct midiplayTrackParser {
	uint32_t length, offset, start;
	uint32_t delta;
	uint8_t  cmd[6];
	uint8_t  lastCmd;
	bool     is2B;
	bool     isDone;
} midiplayTrackParserT;

typedef struct midiplayParser {
	uint16_t                  nbTracks;
	uint16_t                  ticks;
	uint32_t                  timer0PerTick;
	uint32_t                  progTime;
	bool                      isWaiting;
	uint32_t                  cuedDelta;
	uint16_t                  cuedIndex;
	uint16_t                  isMasterDone;
	midiplayTrackParserT     *tracks;
} midiplayParserT;


// ============================================================
// Backward-compatible typedefs (old names -> new names)
// ============================================================

typedef midiplayTempoChangeT     tCh;
typedef midiplayTempoChangeT     tempoChange;
typedef midiplayRecordT          midiRec;
typedef midiplayRecordT         *midiRecPtr;
typedef midiplayEventT           aME;
typedef midiplayEventT          *aMEPtr;
typedef midiplayTableOfEventsT   aTOE;
typedef midiplayTableOfEventsT  *aTOEPtr;
typedef midiplayParsedListT      bigParsed;
typedef midiplayParsedListT     *bigParsedPtr;
typedef midiplayTrackParserT     MIDTrackP;
typedef midiplayParserT          MIDP;

// Backward-compatible struct tags
#define midiRecord       midiplayRecord
#define aMIDIEvent       midiplayEvent
#define aTableOfEvent    midiplayTableOfEvents
#define bigParsedEventList  midiplayParsedList
#define MIDITrackParser  midiplayTrackParser
#define MIDIParser       midiplayParser


// ============================================================
// Globals
// ============================================================

extern midiplayParserT  midiplayTheOne;
extern bool             midiplayChip;

// Backward-compatible names
#define theOne      midiplayTheOne
#define midiChip    midiplayChip


// ============================================================
// v1 functions: Pre-parsed MIDI playback
// ============================================================

void     midiplayInitRecord(midiplayRecordT *rec, uint32_t baseAddr, uint32_t parsedAddr);
void     midiplayInitList(midiplayParsedListT *list);
uint32_t midiplayGetTotalLeft(midiplayParsedListT *list);
uint8_t  midiplayLoadFile(const char *name, uint32_t targetAddress);
int16_t  midiplayGetAndAnalyze(midiplayRecordT *rec, midiplayParsedListT *list);
void     midiplayDetectStructureV1(uint16_t startIndex, midiplayRecordT *rec, midiplayParsedListT *list);
int16_t  midiplayFindHeader(uint32_t baseAddr);
void     midiplayAdjustOffsets(midiplayParsedListT *list);
int8_t   midiplayParse(uint16_t startIndex, bool wantCmds, midiplayRecordT *rec, midiplayParsedListT *list);
uint8_t  midiplayWriteDigest(char *name, midiplayRecordT *rec, midiplayParsedListT *list);
uint8_t  midiplayReadDigest(char *name, midiplayRecordT *rec, midiplayParsedListT *list);
uint8_t  midiplayPlayV1(midiplayRecordT *rec, midiplayParsedListT *list);
uint8_t  midiplayPlayType0(midiplayRecordT *rec, midiplayParsedListT *list);
void     midiplaySendEventV1(midiplayEventT *midiEvent, bool useAlt);


// ============================================================
// v2 functions: Real-time streaming MIDI playback
// ============================================================

void     midiplayDetectStructure(uint16_t startIndex, midiplayRecordT *rec);
void     midiplayInitTrack(uint32_t baseAddr);
void     midiplayDestroyTrack(void);
void     midiplayResetTrack(uint32_t baseAddr);
void     midiplayPlay(void);
void     midiplaySniffNext(void);
void     midiplayExhaustZeroes(uint8_t track);
uint8_t  midiplayReadEvent(uint8_t track);
uint32_t midiplayReadDelta(uint8_t track);
uint8_t  midiplayReadCmd(uint8_t track);
uint8_t  midiplaySkipFFCmd(uint8_t track, uint8_t meta_byte, uint8_t data_byte);
void     midiplayChainEvent(uint8_t track);
void     midiplayPerformCmd(uint8_t track);
void     midiplaySendEvent(uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t byteCount, bool useAlt);

// Utility
uint16_t midiplayReadBE16(uint32_t where);
uint32_t midiplayReadBE32(uint32_t where);


// ============================================================
// Backward-compatible function name aliases
// ============================================================

// v1 aliases
#define initMidiRecord(rec, base, parsed)  midiplayInitRecord((rec), (base), (parsed))
#define initBigList                        midiplayInitList
#define getTotalLeft                        midiplayGetTotalLeft
#define loadSMFile                          midiplayLoadFile
#define getAndAnalyzeMIDI                   midiplayGetAndAnalyze
#define findPositionOfHeader                midiplayFindHeader
#define adjustOffsets                       midiplayAdjustOffsets
#define parse                               midiplayParse
#define playmidi                             midiplayPlayV1
#define playmiditype0                        midiplayPlayType0
#define writeDigestFile                     midiplayWriteDigest
#define readDigestFile                      midiplayReadDigest
#define sendEventV1                         midiplaySendEventV1

// v2 aliases
#define detectStructure(si, rec)            midiplayDetectStructure((si), (rec))
#define initTrack                           midiplayInitTrack
#define destroyTrack                        midiplayDestroyTrack
#define resetTrack                          midiplayResetTrack
#define playMidi                            midiplayPlay
#define sniffNextMIDI                       midiplaySniffNext
#define exhaustZeroes                       midiplayExhaustZeroes
#define readBigEndian16                     midiplayReadBE16
#define readBigEndian32                     midiplayReadBE32

// These aliases allow the old sendAME signature for v1 (pointer) and v2 (bytes)
// Doodles that define their own sendAME should #undef this first.
// Default: v2-style sendAME (4 bytes + bool)
#define sendAME                             midiplaySendEvent


#pragma compile("f_midiplay.c")


#endif
#endif // MIDIPLAY_H
