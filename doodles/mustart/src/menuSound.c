/*
 * Menu sound effects for the mustart doodle.
 * Ported from F256KsimpleCdoodles/mustart/src/menuSound.c
 * Uses f256lib naming: psgShut, sidShutAllVoices, opl3QuietAll,
 * kernelGetTimerAbsolute, kernelSetTimer.
 */

#include "f256lib.h"
#include "menuSound.h"

struct timer_t menuSoundTimer;

void killSound()
{
	//MIDI
	midiShutAllChannels(false);
	//PSG
	psgShut();
	sidShutAllVoices();
	opl3QuietAll();
}

void initMenuSoundTimer()
{
	menuSoundTimer.units = TIMER_FRAMES;
	menuSoundTimer.cookie = TIMER_MENUSOUND_COOKIE;
	menuSoundTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_MENUSOUND_DELAY;
	kernelSetTimer(&menuSoundTimer);
}

void relaunchTimer(uint8_t choice)
{
	psgNoteOn(0, PSG_BOTH, psgLow[8 + 20*choice], psgHigh[8 + 20*choice], 0x5F);
	menuSoundTimer.absolute = kernelGetTimerAbsolute(TIMER_FRAMES) + TIMER_MENUSOUND_DELAY;
	kernelSetTimer(&menuSoundTimer);
}
