/*
 *	MIDI support for F256 SAM2695 and VS1053b.
 *	Adapted from mu0nlibs/muMidi for oscar64.
 */


#ifndef MIDI_H
#define MIDI_H
#ifndef WITHOUT_MIDI


#include "f256lib.h"


// MIDI meta event codes
#define MIDI_META_SEQUENCE          0x00
#define MIDI_META_TEXT              0x01
#define MIDI_META_COPYRIGHT         0x02
#define MIDI_META_TRACK_NAME       0x03
#define MIDI_META_INSTRUMENT_NAME  0x04
#define MIDI_META_LYRICS           0x05
#define MIDI_META_MARKER           0x06
#define MIDI_META_CUE_POINT       0x07
#define MIDI_META_CHANNEL_PREFIX   0x20
#define MIDI_META_CHANGE_PORT      0x21
#define MIDI_META_END_OF_TRACK     0x2F
#define MIDI_META_SET_TEMPO        0x51
#define MIDI_META_SMPTE_OFFSET     0x54
#define MIDI_META_TIME_SIGNATURE   0x58
#define MIDI_META_KEY_SIGNATURE    0x59
#define MIDI_META_SEQUENCER_SPECIFIC 0x7F

// Far-memory MIDI event layout
#define MIDI_EVENT_DELTA           0    // 4 bytes
#define MIDI_EVENT_BYTECOUNT       4    // 1 byte
#define MIDI_EVENT_MSG             5    // up to 3 bytes
#define MIDI_EVENT_FAR_SIZE        8    // total struct size


// VS1053b real-time MIDI plugin data
extern const uint16_t midiVS1053bPlugin[28];

// General MIDI instrument name table
extern const char *midiInstrumentNames[128];


void midiNoteOn(byte channel, byte note, byte velocity, bool useAlt);
void midiNoteOff(byte channel, byte note, byte velocity, bool useAlt);
void midiProgramChange(byte program, byte channel, bool useAlt);
void midiResetInstruments(bool useAlt);
void midiShutChannel(byte channel, bool useAlt);
void midiShutAllChannels(bool useAlt);
void midiPanic(bool useAlt);
void midiEmptyRxBuffer(void);


// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

// Meta event code aliases (old muMidi.h names -> new MIDI_META_XXX names)
#define MetaSequence         MIDI_META_SEQUENCE
#define MetaText             MIDI_META_TEXT
#define MetaCopyright        MIDI_META_COPYRIGHT
#define MetaTrackName        MIDI_META_TRACK_NAME
#define MetaInstrumentName   MIDI_META_INSTRUMENT_NAME
#define MetaLyrics           MIDI_META_LYRICS
#define MetaMarker           MIDI_META_MARKER
#define MetaCuePoint         MIDI_META_CUE_POINT
#define MetaChannelPrefix    MIDI_META_CHANNEL_PREFIX
#define MetaChangePort       MIDI_META_CHANGE_PORT
#define MetaEndOfTrack       MIDI_META_END_OF_TRACK
#define MetaSetTempo         MIDI_META_SET_TEMPO
#define MetaSMPTEOffset      MIDI_META_SMPTE_OFFSET
#define MetaTimeSignature    MIDI_META_TIME_SIGNATURE
#define MetaKeySignature     MIDI_META_KEY_SIGNATURE
#define MetaSequencerSpecific MIDI_META_SEQUENCER_SPECIFIC

// Far-memory MIDI event layout aliases (old names -> new names)
#define AME_DELTA            MIDI_EVENT_DELTA
#define AME_BYTECOUNT        MIDI_EVENT_BYTECOUNT
#define AME_MSG              MIDI_EVENT_MSG

// Instrument name table alias
#define midi_instruments     midiInstrumentNames


#pragma compile("f_midi.c")


#endif
#endif // MIDI_H
