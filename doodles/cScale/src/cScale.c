#include "f256lib.h"

#define TIMER_NOTE_DELAY 30
#define TIMER_NOTE_COOKIE 0

struct timer_t midiTimer;
uint8_t cScaleNotes[8] = {40, 42, 44, 45, 47, 49, 51, 52};

void setPlayerInstrument(uint8_t choice) {
	POKE(MIDI_FIFO, 0xC1);
	POKE(MIDI_FIFO, choice);
}

void setInstruments(void) {
	POKE(MIDI_FIFO, 123);
	POKE(MIDI_FIFO, 0);
	POKE(MIDI_FIFO, 0xFF);
	POKE(MIDI_FIFO, 0xC0);
	POKE(MIDI_FIFO, 00);
}

void localMidiNoteOff(uint8_t wantNote, uint8_t chan) {
	POKE(MIDI_FIFO, 0x80 | chan);
	POKE(MIDI_FIFO, wantNote);
	POKE(MIDI_FIFO, 0x4F);
}

void localMidiNoteOn(uint8_t wantNote, uint8_t chan) {
	POKE(MIDI_FIFO, 0x90 | chan);
	POKE(MIDI_FIFO, wantNote);
	POKE(MIDI_FIFO, 0x7F);
}

int main(int argc, char *argv[]) {
	uint8_t noteCursor = 0, currentNote = 0;
	bool isScaleActive = false;

	setInstruments();

	POKE(MMU_IO_CTRL, 0);
	midiTimer.cookie = TIMER_NOTE_COOKIE;
	midiTimer.units = TIMER_FRAMES;

	printf("Press space to play a C scale with MIDI");
	while (true) {
		kernelNextEvent();
		if (kernelEventData.type == kernelEvent(timer.EXPIRED)) {
			switch (kernelEventData.u.timer.cookie) {
				case TIMER_NOTE_COOKIE:
					localMidiNoteOff(currentNote, 0);
					if (isScaleActive) {
						noteCursor++;
						if (noteCursor == 8) {
							isScaleActive = false;
							break;
						}
					}
					currentNote = cScaleNotes[noteCursor];
					localMidiNoteOn(currentNote, 0);

					midiTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_NOTE_DELAY;
					kernelSetTimer(&midiTimer);
					break;
			}
		} else if (kernelEventData.type == kernelEvent(key.PRESSED)) {
			switch (kernelEventData.u.key.raw) {
				case 32:
					if (isScaleActive == false) {
						noteCursor = 0;
						isScaleActive = true;
						currentNote = cScaleNotes[noteCursor];
						localMidiNoteOn(currentNote, 0);
						midiTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_NOTE_DELAY;
						kernelSetTimer(&midiTimer);
					}
					break;
			}
		}
	}
	return 0;
}
