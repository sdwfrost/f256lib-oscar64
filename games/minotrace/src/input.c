#include "input.h"
#include "f256lib.h"

signed char joyx, joyy;
bool joyb;
bool joy_pause;

void joy_poll_input(void)
{
	joyx = 0;
	joyy = 0;
	joyb = false;
	joy_pause = false;

	// Try NES gamepad poll (non-blocking with timeout)
	padsPollNES();
	for (unsigned int timeout = 0; timeout < 1000; timeout++)
	{
		if (padsPollIsReady())
		{
			unsigned char pad = PEEK(PAD0);

			// NES pad: bit is 0 when pressed, 0xFF = no buttons / no controller
			if (pad != 0xFF && pad != 0x00)
			{
				if (!(pad & NES_LEFT))  joyx = -1;
				if (!(pad & NES_RIGHT)) joyx = 1;
				if (!(pad & NES_UP))    joyy = -1;
				if (!(pad & NES_DOWN))  joyy = 1;
				if (!(pad & NES_A))     joyb = true;
				if (!(pad & NES_B))     joyb = true;
				if (!(pad & NES_START)) joy_pause = true;
			}
			break;
		}
	}

	// Drain keyboard events directly from kernel
	for (;;)
	{
		kernelNextEvent();
		if (kernelEventData.type == 0)
			break;  // No more events

		if (kernelEventData.type != kernelEvent(key.PRESSED))
			continue;

		// Check ASCII field first (printable keys with no modifier flags)
		char c = kernelEventData.u.key.ascii;
		if (c && !kernelEventData.u.key.flags)
		{
			switch (c)
			{
				case ' ':  joyb = true; break;
				case 13:   joyb = true; break;
				case 27:   joy_pause = true; break;
			}
			continue;
		}

		// Map raw microkernel key codes for non-ASCII keys
		switch (kernelEventData.u.key.raw)
		{
			case 0xB6: joyy = -1; break;         // UP
			case 0xB7: joyy = 1;  break;         // DOWN
			case 0xB8: joyx = -1; break;         // LEFT
			case 0xB9: joyx = 1;  break;         // RIGHT
			case 0x94: joyb = true; break;       // ENTER
			case 0x95: joy_pause = true; break;  // ESC
		}
	}
}
