/*
 *	LED control for F256 series.
 *	Ported from mu0nlibs/muleds for oscar64.
 */

#ifndef MULEDS_H
#define MULEDS_H

#include "f256lib.h"


#define SYS_CTRL_REG   0xD6A0

#define SYS_NET 0x40
#define SYS_LCK 0x20
#define SYS_L1  0x08
#define SYS_L0  0x04
#define SYS_SD  0x02
#define SYS_PWR 0x01

#define LED_PWR_B      0xD6A7
#define LED_PWR_G      0xD6A8
#define LED_PWR_R      0xD6A9
#define LED_SD_B       0xD6AA
#define LED_SD_G       0xD6AB
#define LED_SD_R       0xD6AC
#define LED_LCK_B      0xD6AD
#define LED_LCK_G      0xD6AE
#define LED_LCK_R      0xD6AF
#define LED_NET_B      0xD6B3
#define LED_NET_G      0xD6B4
#define LED_NET_R      0xD6B5


void enableManualLEDs(bool wantNet, bool wantLock, bool wantL1, bool wantL0, bool wantMedia, bool wantPower);


#pragma compile("muleds.c")


#endif // MULEDS_H
