/*
 * Menu sound effects for the mustart doodle.
 * Ported from F256KsimpleCdoodles/mustart/src/menuSound.h
 * Uses f256lib naming conventions.
 */

#ifndef MENUSOUND_H
#define MENUSOUND_H

#include "f256lib.h"

#define TIMER_MENUSOUND_COOKIE 1
#define TIMER_MENUSOUND_DELAY 2


void killSound(void);
void initMenuSoundTimer(void);
void relaunchTimer(uint8_t);

extern struct timer_t menuSoundTimer;

#pragma compile("menuSound.c")

#endif // MENUSOUND_H
