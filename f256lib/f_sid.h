/*
 *	SID chip support for F256.
 *	Adapted from mu0nlibs/musid for oscar64.
 */


#ifndef SID_H
#define SID_H
#ifndef WITHOUT_SID


#include "f256lib.h"


typedef struct sidInstrumentS {
	byte maxVolume;
	byte pwdLo, pwdHi;
	byte ad, sr;
	byte ctrl;
	byte fcfLo, fcfHi;
	byte frr;
	byte v3Lo, v3Hi;  // voice 3 frequency for modulation (ring/sync)
} sidInstrumentT;

// Alias for backward compatibility with old mu0nlibs code
typedef sidInstrumentT sidI;


extern const char    *sidInstrumentNames[];
extern const byte     sidInstrumentsSize;
extern sidInstrumentT sidInstrumentDefs[];
extern const byte     sidLow[96];
extern const byte     sidHigh[96];


void sidClearRegisters(void);
void sidShutAllVoices(void);
void sidNoteOnOrOff(uint16_t voice, byte ctrl, bool isOn);
void sidSetInstrument(byte chip, byte voice, sidInstrumentT inst);
void sidSetSIDWide(byte which);
void sidSetInstrumentAllChannels(byte which);
void sidPrepInstruments(void);
void sidSetMono(void);
void sidSetStereo(void);
byte sidFetchCtrl(byte which);

// Convenience functions: set registers across all voices/chips at once
void sidSetCTRL(byte ctrl);
void sidSetPWM(byte lo, byte hi);
void sidSetFF(byte lo, byte hi);
void sidSetModVol(byte data);
void sidSetFILT(byte data);
void sidSetADSR(byte ad, byte sr);
void sidSetV3(byte whichChip, sidInstrumentT sI);
void sidSetAll(sidInstrumentT sI);


// ============================================================
// Backward-compatible aliases (old mu0nlibs names)
// ============================================================

#define copySidInstrument(src, dst) (*(dst) = (src))
#define clearSIDRegisters           sidClearRegisters
#define prepSIDinstruments          sidPrepInstruments
#define setMonoSID                  sidSetMono
#define setStereoSID                sidSetStereo
#define sid_instrument_defs         sidInstrumentDefs
#define sid_instrument_names        sidInstrumentNames
#define sid_setInstrument           sidSetInstrument
#define sid_setInstrumentAllChannels sidSetInstrumentAllChannels
#define sid_setSIDWide              sidSetSIDWide
#define sid_instrumentsSize         sidInstrumentsSize


#pragma compile("f_sid.c")


#endif
#endif // SID_H
