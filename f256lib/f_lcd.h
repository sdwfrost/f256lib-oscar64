/*
 *	LCD display support for F256K2 case display.
 *	Adapted from mu0nlibs/mulcd for oscar64.
 */


#ifndef LCD_H
#define LCD_H
#ifndef WITHOUT_LCD


#include "f256lib.h"


void lcdDisplayImage(uint32_t addr, uint8_t boost);


#pragma compile("f_lcd.c")


#endif
#endif // LCD_H
