#include "gamemusic.h"
#include "f256lib.h"

// SID voice 3 base address (on SID chip 1)
#define SID_V3_BASE  (SID1 + SID_VOICE3)

// Current effect state
static unsigned char sfx_frames_left;

void music_init(Tune tune)
{
	// No embedded music player for F256K yet
	(void)tune;
}

void music_play(void)
{
	// No-op
}

void music_patch_voice3(bool enable)
{
	(void)enable;
}

void sidfx_play_effect(SoundEffect sfx)
{
	// Set SID volume
	POKE(SID1 + SID_FM_VC, 0x0F);

	switch (sfx)
	{
		case SFX_BOUNCE:
			// Short noise burst
			POKE(SID_V3_BASE + SID_LO_B, 0x00);
			POKE(SID_V3_BASE + SID_HI_B, 0x08);
			POKE(SID_V3_BASE + SID_ATK_DEC, 0x09);
			POKE(SID_V3_BASE + SID_SUS_REL, 0xF3);
			POKE(SID_V3_BASE + SID_CTRL, 0x81);  // Noise + gate
			sfx_frames_left = 4;
			break;

		case SFX_EXPLOSION:
			// Long noise burst
			POKE(SID_V3_BASE + SID_LO_B, 0x00);
			POKE(SID_V3_BASE + SID_HI_B, 0x0C);
			POKE(SID_V3_BASE + SID_ATK_DEC, 0x09);
			POKE(SID_V3_BASE + SID_SUS_REL, 0xF8);
			POKE(SID_V3_BASE + SID_CTRL, 0x81);  // Noise + gate
			sfx_frames_left = 30;
			break;

		case SFX_BEEP_SHORT:
			// Short rectangle beep
			POKE(SID_V3_BASE + SID_LO_B, 0x40);
			POKE(SID_V3_BASE + SID_HI_B, 0x1F);
			POKE(SID_V3_BASE + SID_LO_PWDC, 0x00);
			POKE(SID_V3_BASE + SID_HI_PWDC, 0x08);
			POKE(SID_V3_BASE + SID_ATK_DEC, 0x09);
			POKE(SID_V3_BASE + SID_SUS_REL, 0xF1);
			POKE(SID_V3_BASE + SID_CTRL, 0x41);  // Pulse + gate
			sfx_frames_left = 4;
			break;

		case SFX_BEEP_HURRY:
			// Higher beep
			POKE(SID_V3_BASE + SID_LO_B, 0x40);
			POKE(SID_V3_BASE + SID_HI_B, 0x1F);
			POKE(SID_V3_BASE + SID_LO_PWDC, 0x00);
			POKE(SID_V3_BASE + SID_HI_PWDC, 0x08);
			POKE(SID_V3_BASE + SID_ATK_DEC, 0x09);
			POKE(SID_V3_BASE + SID_SUS_REL, 0xF1);
			POKE(SID_V3_BASE + SID_CTRL, 0x41);  // Pulse + gate
			sfx_frames_left = 8;
			break;

		case SFX_BEEP_LONG:
			// Long start beep
			POKE(SID_V3_BASE + SID_LO_B, 0xE0);
			POKE(SID_V3_BASE + SID_HI_B, 0x2E);
			POKE(SID_V3_BASE + SID_LO_PWDC, 0x00);
			POKE(SID_V3_BASE + SID_HI_PWDC, 0x08);
			POKE(SID_V3_BASE + SID_ATK_DEC, 0x09);
			POKE(SID_V3_BASE + SID_SUS_REL, 0xF3);
			POKE(SID_V3_BASE + SID_CTRL, 0x41);  // Pulse + gate
			sfx_frames_left = 20;
			break;
	}
}

void sidfx_update(void)
{
	if (sfx_frames_left > 0)
	{
		sfx_frames_left--;
		if (sfx_frames_left == 0)
		{
			// Release gate
			unsigned char ctrl = PEEK(SID_V3_BASE + SID_CTRL);
			POKE(SID_V3_BASE + SID_CTRL, ctrl & 0xFE);
		}
	}
}
