
#include "f256lib.h"
// MIDI is built into f256lib (midiNoteOn, midiNoteOff, midiShutAllChannels, etc.)
// muTimer0Int functionality is provided by f256lib (f_timer0.h)
// muMidiPlay2 functionality is provided by f256lib (f_midiplay.h)

#define MIDISTART 0x10000

// TODO: EMBED(music, "../doom.mid", 0x10000); — EMBED() not available in oscar64; 1kb


int main(int argc, char *argv[]) {

	midiShutAllChannels(false);
    initTrack(MIDISTART);

//find what to do and exhaust all zero delay events at the start
	for(uint16_t i=0;i<theOne.nbTracks;i++) exhaustZeroes(i);

	//insert game loop here
	while(true)
		{
		kernelNextEvent();
		if(PEEK(INT_PENDING_0)&0x10) //when the timer0 delay is up, go here
		{
			POKE(INT_PENDING_0,0x10); //clear the timer0 delay
			playMidi(); //play the next chunk of the midi file, might deal with multiple 0 delay stuff
		}
		sniffNextMIDI(); //find next event to play, will cue up a timer0 delay
		}
	destroyTrack();
}
