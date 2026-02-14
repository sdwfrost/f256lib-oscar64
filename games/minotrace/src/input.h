#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

extern signed char joyx, joyy;
extern bool joyb;
extern bool joy_pause;

// Poll NES gamepad + keyboard and update joyx/joyy/joyb/joy_pause
void joy_poll_input(void);

#pragma compile("input.c")

#endif
