/*
 *	VGM file playback engine for F256 OPL3.
 *	Adapted from mu0nlibs/muvgmplay for oscar64.
 */


#ifndef VGMPLAY_H
#define VGMPLAY_H
#ifndef WITHOUT_VGMPLAY


#include "f256lib.h"


#define VGM_BODY  0x100000


extern uint32_t vgmTooBigWait;
extern bool     vgmComeRightThrough;
extern uint32_t vgmNeedle;
extern uint32_t vgmTotalWait;
extern uint32_t vgmLoopBackTo;
extern uint32_t vgmSamplesSoFar;
extern bool     vgmOneLoop;


FILE    *vgmLoadFile(char *name);
void     vgmCopyToRAM(FILE *theVGMfile);
void     vgmCheckHeader(FILE *theVGMfile);
int8_t   vgmPlayback(FILE *theVGMfile, bool iRT, bool pReq);
uint8_t  vgmGetStart(uint16_t ver);


#pragma compile("f_vgmplay.c")


#endif
#endif // VGMPLAY_H
