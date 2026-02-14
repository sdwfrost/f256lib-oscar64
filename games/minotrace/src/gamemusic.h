#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

#include <stdbool.h>

enum Tune
{
	TUNE_MAIN,
	TUNE_GAME_1,
	TUNE_GAME_2,
	TUNE_GAME_3,
	TUNE_GAME_4,
	TUNE_RESTART
};

enum SoundEffect
{
	SFX_BOUNCE,
	SFX_EXPLOSION,
	SFX_BEEP_SHORT,
	SFX_BEEP_HURRY,
	SFX_BEEP_LONG
};

// Start playback of a tune (no-op for now, music not yet ported)
void music_init(Tune tune);

// Iterate music each frame (no-op)
void music_play(void);

// Disable or enable voice 3 in the SID player (no-op)
void music_patch_voice3(bool enable);

// Play a sound effect on SID voice 3
void sidfx_play_effect(SoundEffect sfx);

// Update sound effects each frame
void sidfx_update(void);

#pragma compile("gamemusic.c")

#endif
